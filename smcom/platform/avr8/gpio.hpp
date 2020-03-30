#ifndef STAVRP_AVR8_GPIO_H
#define STAVRP_AVR8_GPIO_H

#include "../../softdrv/gpio.hpp"

namespace STAVRP {
	namespace AVR8 {

		using STAVRP::SoftDrv::PinMode;
		using STAVRP::SoftDrv::PortClass;

		template<int port_ddr, int port_idr, int port_odr, int port_pue, uint8_t pin, uint8_t mode_t>
		class PinDriver {
		protected:
			static constexpr volatile uint8_t* RegODR = (uint8_t*)port_odr;
			static constexpr volatile uint8_t* RegDDR = (uint8_t*)port_ddr;
			static constexpr volatile uint8_t* RegIDR = (uint8_t*)port_idr;
			static constexpr volatile uint8_t* RegPUE = (uint8_t*)port_pue;
		public:
          static void Init(uint8_t mode = mode_t) {
	          if( PinMode::IsOut(mode) ) *RegDDR |= (1 << pin);
	          else {
				  *RegDDR &= ~(1 << pin);
				  if( PinMode::IsUp(mode) ) *RegPUE |= (1 << pin);
			  }
          }
          static bool Get() {
	          return (*RegIDR & (1 << pin))?true:false;
          }
          static void Set() {
	          *RegODR |= (1 << pin);
			  //PORTB |= (1 << pin);
          }
          static void Clr() {
	          *RegODR &= ~(1 << pin);
          }
		};

		#ifdef _AVR_ATtiny10_H_
		
			template<uint8_t pin, uint8_t mode=PinMode::Input>
			using PortB = PortClass<PinDriver<(int)&DDRB, (int)&PINB, (int)&PORTB, (int)&PUEB, pin, mode>, mode>;

		#else
			#error "test"
               
			#ifdef PORTA
				template<uint8_t pin, uint8_t mode=PinMode::Input>
				using PortA = PortClass<PinDriver<(int)&DDRA, (int)&PINA, (int)&PORTA, (int)&PORTA, pin, mode>, mode>;
			#endif
			#ifdef PORTB
				template<uint8_t pin, uint8_t mode=PinMode::Input>
				using PortB = PortClass<PinDriver<(int)&DDRB, (int)&PINB, (int)&PORTB, (int)&PORTB, pin, mode>, mode>;
			#endif
			#ifdef PORTC
				template<uint8_t pin, uint8_t mode=PinMode::Input>
				using PortC = PortClass<PinDriver<(int)&DDRC, (int)&PINC, (int)&PORTC, (int)&PORTC, pin, mode>, mode>;
			#endif
			#ifdef PORTD
				template<uint8_t pin, uint8_t mode=PinMode::Input>
				using PortD = PortClass<PinDriver<(int)&DDRD, (int)&PIND, (int)&PORTD, (int)&PORTD, pin, mode>, mode>;
			#endif
			#ifdef PORTE
			#endif
			#ifdef PORTF
			#endif
			#ifdef PORTG
			#endif
			#ifdef PORTH
			#endif
			#ifdef PORTJ
			#endif
			#ifdef PORTK
			#endif
			#ifdef PORTL
			#endif
			
		#endif
	}
}

#endif