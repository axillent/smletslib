#ifndef STAVRP_AVR8_DELAY_H
#define STAVRP_AVR8_DELAY_H

#include "../stavrp.h"

#include <inttypes.h>
#include <util/delay_basic.h>
#include <math.h>

namespace STAVRP {
	namespace AVR8 {

	template <class MCU>
	class Delay {
	public:
		__inline__ static void Microseconds(double __us) __attribute__((__always_inline__)) {
			double __tmp ;

			#if __HAS_DELAY_CYCLES && defined(__OPTIMIZE__) && \
				!defined(__DELAY_BACKWARD_COMPATIBLE__) &&	   \
				__STDC_HOSTED__

				uint32_t __ticks_dc;
				extern void __builtin_avr_delay_cycles(unsigned long);
				__tmp = ((MCU::cpu_f) / 1e6) * __us;

				#if defined(__DELAY_ROUND_DOWN__)
					__ticks_dc = (uint32_t)fabs(__tmp);
				#elif defined(__DELAY_ROUND_CLOSEST__)
					__ticks_dc = (uint32_t)(fabs(__tmp)+0.5);
				#else
					//round up by default
					__ticks_dc = (uint32_t)(ceil(fabs(__tmp)));
				#endif

				__builtin_avr_delay_cycles(__ticks_dc);

			#else

				uint8_t __ticks;
				double __tmp2 ;
				__tmp = ((MCU::cpu_f) / 3e6) * __us;
				__tmp2 = ((MCU::cpu_f) / 4e6) * __us;
				if (__tmp < 1.0)
				__ticks = 1;
				else if (__tmp2 > 65535)
				{
					Milliseconds(__us / 1000.0);
				}
				else if (__tmp > 255)
				{
					uint16_t __ticks=(uint16_t)__tmp2;
					_delay_loop_2(__ticks);
					return;
				}
				else
				__ticks = (uint8_t)__tmp;
				_delay_loop_1(__ticks);
				#endif

		}
		__inline__ static void Milliseconds(double __ms) __attribute__((__always_inline__)) {

			double __tmp ;

			#if __HAS_DELAY_CYCLES && defined(__OPTIMIZE__) && \
				!defined(__DELAY_BACKWARD_COMPATIBLE__) &&	   \
				__STDC_HOSTED__
	
				uint32_t __ticks_dc;
				extern void __builtin_avr_delay_cycles(unsigned long);
				__tmp = ((MCU::cpu_f) / 1e3) * __ms;

				#if defined(__DELAY_ROUND_DOWN__)
					__ticks_dc = (uint32_t)fabs(__tmp);
				#elif defined(__DELAY_ROUND_CLOSEST__)
					__ticks_dc = (uint32_t)(fabs(__tmp)+0.5);
				#else
	
				//round up by default
				__ticks_dc = (uint32_t)(ceil(fabs(__tmp)));
				#endif

				__builtin_avr_delay_cycles(__ticks_dc);

			#else

				uint16_t __ticks;
				__tmp = ((MCU::cpu_f) / 4e3) * __ms;
				if (__tmp < 1.0)
				__ticks = 1;
				else if (__tmp > 65535)
				{
					//	__ticks = requested delay in 1/10 ms
					__ticks = (uint16_t) (__ms * 10.0);
					while(__ticks)
					{
						// wait 1/10 ms
						_delay_loop_2(((MCU::cpu_f) / 4e3) / 10);
						__ticks --;
					}
					return;
				}
				else
				__ticks = (uint16_t)__tmp;
				_delay_loop_2(__ticks);

			#endif
		}
	};

	}
}

#endif
