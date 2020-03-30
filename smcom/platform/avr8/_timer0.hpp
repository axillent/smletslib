#ifndef STAVRP_AVR8_TIMER0_H
#define STAVRP_AVR8_TIMER0_H

#include "mcu.h"

#if defined(__AVR_ATmega48__) ||defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || \
defined(__AVR_ATmega48P__) ||defined(__AVR_ATmega88P__) || defined(__AVR_ATmega168P__) || \
defined(__AVR_ATmega328P__) 
	#define _STAVRP_TIMER0_ENUM		Div1 = 1, Div8, Div64, Div256, Div1024
	#define _STAVRP_TIMER0_ENUMV(v)	(v==1)?1:((v==2)?8:((v==3)?64:((v==4)?256:1024)))
	#define _STAVRP_TIMER0_VECT		TIMER0_OVF_vect
	#define _STAVRP_TIMER0_VECT_NUM	TIMER0_OVF_vect_num
	#define _STAVRP_TIMER0_TIMSK	TIMSK0
	#define _STAVRP_TIMER0_SC		TCCR0B
	#define _STAVRP_TIMER0_WGM		TCCR0A
#elif defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) || defined(__AVR_ATmega128A__)
	#define _STAVRP_TIMER0_ENUM		Div1 = 1, Div8, Div32, Div64, Div128, Div256, Div1024
	#define _STAVRP_TIMER0_ENUMV(v)	(v==1)?1:((v==2)?8:((v==3)?32:((v==4)?64:((v==5)?128:((v==6)?256:1024)))))
	#define _STAVRP_TIMER0_VECT		TIMER0_OVF_vect
	#define _STAVRP_TIMER0_VECT_NUM	TIMER0_OVF_vect_num
	#define _STAVRP_TIMER0_TIMSK	TIMSK
	#define _STAVRP_TIMER0_SC		TCCR0
	#define _STAVRP_TIMER0_WGM		TCCR0
#elif defined(__AVR_ATtiny13A__)
	#define _STAVRP_TIMER0_ENUM		Div1 = 1, Div8, Div64, Div256, Div1024
	#define _STAVRP_TIMER0_ENUMV(v)	(v==1)?1:((v==2)?8:((v==3)?64:((v==4)?256:1024)))
	#define _STAVRP_TIMER0_VECT		TIM0_OVF_vect
	#define _STAVRP_TIMER0_VECT_NUM	TIM0_OVF_vect_num
	#define _STAVRP_TIMER0_TIMSK	TIMSK0
	#define _STAVRP_TIMER0_SC		TCCR0B
	#define _STAVRP_TIMER0_WGM		TCCR0A
#else
	#error STAVR timer TIMER2_FREQ10 not supported for MCU
#endif

namespace STAVRP {
	namespace AVR8 {

		enum class Timer0Mode { Normal, FastPWM };
		
		enum class Timer0Prescaler { _STAVRP_TIMER0_ENUM };	

        //----------------------------------------------------------------------
        // Timer0 class
        //----------------------------------------------------------------------
		typedef STAVRP::HandlerData<_STAVRP_TIMER0_VECT_NUM> Timer0Data;

		template <typename MCU, int freq=0, Timer0Prescaler pscr=Timer0Prescaler::Div1>
		class Timer0 : public Timer0Data {
		public:
			static uint16_t Init(void);
			static void Start(void);
			static void Stop(void);
			static void Mode(Timer0Mode mode);
		};

        //----------------------------------------------------------------------
        // Timer0 methods
        //----------------------------------------------------------------------
		template <typename MCU, int freq, Timer0Prescaler pscr>
        uint16_t Timer0<MCU, freq, pscr>::Init(void) {
			if( MCU::cpu_f >= 300000 && MCU::cpu_f <= 5000000 ) {
				_STAVRP_TIMER0_SC = (0 << CS02) | (1 << CS01) | (0 << CS00);		// /8
				return (MCU::cpu_f/8/256);
			}
			if( MCU::cpu_f > 5000000 && MCU::cpu_f < 16000000 ) {
				_STAVRP_TIMER0_SC = (0 << CS02) | (1 << CS01) | (1 << CS00);		// /64
				return (MCU::cpu_f/64/256);
			}
			if( freq == 0 ) {
				_STAVRP_TIMER0_SC = static_cast<uint8_t>(pscr);
				return (MCU::cpu_f/256/_STAVRP_TIMER0_ENUMV((uint8_t)pscr));
			}
			return 0;
        }
		template <typename MCU, int freq, Timer0Prescaler pscr>
        void Timer0<MCU, freq, pscr>::Start(void) {
            _STAVRP_TIMER0_TIMSK = (1 << TOIE0);
        }
		template <typename MCU, int freq, Timer0Prescaler pscr>
        void Timer0<MCU, freq, pscr>::Stop(void) {
            _STAVRP_TIMER0_TIMSK = 0;
        }
		template <typename MCU, int freq, Timer0Prescaler pscr>
        void Timer0<MCU, freq, pscr>::Mode(Timer0Mode mode) {
			switch(mode) {
				case Timer0Mode::Normal:
					_STAVRP_TIMER0_WGM &= ~((1 << WGM01) | (1 << WGM00));
					break;
				case Timer0Mode::FastPWM:
					_STAVRP_TIMER0_WGM |= (1 << WGM01) | (1 << WGM00);
					break;
			}
        }

        ISR(_STAVRP_TIMER0_VECT) {
			Timer0Data::CallHandler();
		}

	}
}


#endif