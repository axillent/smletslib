#ifndef STAVRP_AVR8_PWM_H
#define STAVRP_AVR8_PWM_H

#include "mcu.h"

namespace STAVRP {

	namespace AVR8 {
		
		namespace PWM {
		
			template <typename gpio_pin>
			class Channel0A {
			public:
				static void Init(void) { gpio_pin::Init(PinMode::Output); }
				static void Set(uint8_t pwm) {
					if( !pwm ) {
						TCCR0A &= ~(1 << COM0A1);
						gpio_pin::Clr();
					} else  {
						OCR0A = pwm;
						TCCR0A |= (1 << COM0A1);
					}
				}
			};
			
		}
	}	
}

#endif