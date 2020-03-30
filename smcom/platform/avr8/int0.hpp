#ifndef STAVRP_AVR8_INT0_H
#define STAVRP_AVR8_INT0_H

#include "mcu.h"
#include <avr/interrupt.h>

namespace STAVRP {

	namespace AVR8 {

		enum class Int0Mode {
			Low		= (0 << ISC01) | (0 << ISC00),
			Change	= (0 << ISC01) | (1 << ISC00),
			Falling	= (1 << ISC01) | (0 << ISC00),
			Rising	= (1 << ISC01) | (1 << ISC00)
		};
		
		typedef STAVRP::HandlerData<INT0_vect_num>	Int0Data;
		
		template <stavrp_funcp handler_ptr = nullfunc>
		class Int0 : public Int0Data {
		public:
			static void Enable(Int0Mode mode) {
				Int0Data::SetHandler(handler_ptr);
				MCUCR &= ~3;
				MCUCR |= static_cast<uint8_t>(mode);
				GIMSK |= (1 << INT0);
			}
			static void Disable(void) { GIMSK &= ~(1 << INT0); }
		};
		
		ISR(INT0_vect) {
			Int0Data::CallHandler();
		}

	}
}

#endif