#ifndef STAVRP_AVR8_TIMER1_H
#define STAVRP_AVR8_TIMER1_H

#include <stavrp/stavrp.h>
#include <avr/interrupt.h>

namespace STAVRP {
	namespace AVR8 {

    //----------------------------------------------------------------------
    // Timer1 class
    //----------------------------------------------------------------------
	enum class Timer1Pscr : uint8_t{
		Div1 = 1,
		Div8 = 2,
		Div64 = 3,
		Div256 = 4,
		Div1024 = 5	
	}; 
	
    typedef STAVRP::HandlerData<TIMER1_COMPA_vect_num> Timer1Data;

	template <typename MCU, uint16_t freq, Timer1Pscr pscr=Timer1Pscr::Div1, uint16_t arr=0, Timer1Pscr pscr2=Timer1Pscr::Div1, uint16_t arr2=0>
	class Timer1 : public Timer1Data {
		public:
		static uint16_t Init(void) {
			if( freq ) {
				return 0;
			}

			ResetScale();

			TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 <<  COM1B1) | (0 <<  COM1B0) | (0 <<  FOC1A) | (0 <<  FOC1B) | (0 <<  WGM11) | (0 <<  WGM10);
			TCCR1B = (0 << ICNC1) | (0 << ICES1) | (0 << WGM13) | (1 <<  WGM12) | static_cast<uint8_t>(pscr);
			ICR1 = arr;
			
			return MCU::cpu_f / arr;
		}
		static void Start(void) {
		}
		static void Stop(void) {
		}
		static void ResetCounter() {
		}
		static uint16_t GetCounter() {
			return 0;
		}
		static uint16_t GetCounterMax() {
			return 0;
		}
		static void SetCounterMax(uint16_t max) {
		}
		static bool IsPendingUpdate() {
			return false;
		}
		static void Scale() {
		}
		static void ResetScale() {
		}
	};

	ISR(TIMER1_COMPA_vect) {
	    Timer1Data::CallHandler();		
	}

	}
}

#endif
