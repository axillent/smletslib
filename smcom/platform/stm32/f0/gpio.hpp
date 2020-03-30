#ifndef SMCOM_STM32_F0_GPIO_H
#define SMCOM_STM32_F0_GPIO_H

#include "../../../softdrv/gpio.hpp"

namespace Smcom {
  namespace STM32 {

    using Smcom::SoftDrv::PinMode;
    using Smcom::SoftDrv::PortClass;
	
	  		
    struct AFR {
      enum afr { 
        AF0 = 0, 
        AF1 = 1,
        AF2 = 2,
        AF3 = 3, 
        AF4 = 4, 
        AF5 = 5, 
        AF6 = 5, 
        AF7 = 7
      };
    };
    
	template <uint32_t rcc_mask, uint32_t gpio, int pin, uint16_t mode_t>
		class PinDriver {
		public:
			static void Init(uint16_t mode = mode_t) {
				
				RCC->AHBENR |= rcc_mask | RCC_AHBENR_GPIOCEN;
				
                // --- setting mode
                ((GPIO_TypeDef*)gpio)->MODER &= ~(3 << pin*2);
				if( PinMode::IsAlt(mode) ) ((GPIO_TypeDef*)gpio)->MODER |= (2 << pin*2);
                else if( PinMode::IsAnalog(mode) ) ((GPIO_TypeDef*)gpio)->MODER |= (3 << pin*2);
                else if( PinMode::IsOut(mode) ) ((GPIO_TypeDef*)gpio)->MODER |= (1 << pin*2);

                if( PinMode::IsOut(mode) ) {
					// push-pull
					if( PinMode::IsUp(mode) ) ((GPIO_TypeDef*)gpio)->OTYPER &= ~(1 << pin);
					else ((GPIO_TypeDef*)gpio)->OTYPER |= (1 << pin);
				} else {
					// pullup/pulldown
                    ((GPIO_TypeDef*)gpio)->PUPDR &= ~(3 << pin*2);
					if( PinMode::IsUp(mode) ) ((GPIO_TypeDef*)gpio)->PUPDR |= (1 << pin*2);
					else if( PinMode::IsDown(mode) ) ((GPIO_TypeDef*)gpio)->PUPDR |= (2 << pin*2);
					else ((GPIO_TypeDef*)gpio)->PUPDR &= ~(3 << pin*2);
					
				}
			}
			static bool Get() {
				return ((GPIO_TypeDef*)gpio)->IDR & (1 << pin);
			}
			static void Set() {
				((GPIO_TypeDef*)gpio)->ODR |= (1 << pin);
			}
			static void Clr() {
				((GPIO_TypeDef*)gpio)->ODR &= ~(1 << pin);
			}
            static void SetAFR(AFR::afr mode) {
              ((GPIO_TypeDef*)gpio)->AFR[(pin<8)?0:1] |= (mode << (((pin<8)?pin:pin-8)*4));
            }
		};

		// --- Port A
    template <int pin, uint16_t mode_t=PinMode::Input>
      using PortA = PortClass<PinDriver<RCC_AHBENR_GPIOAEN, (uint32_t)GPIOA, pin, mode_t>, mode_t>;
		// --- Port B
    template <int pin, uint16_t mode_t=PinMode::Input>
      using PortB = PortClass<PinDriver<RCC_AHBENR_GPIOBEN, (uint32_t)GPIOB, pin, mode_t>, mode_t>;
		// --- Port C
    template <int pin, uint16_t mode_t=PinMode::Input>
      using PortC = PortClass<PinDriver<RCC_AHBENR_GPIOCEN, (uint32_t)GPIOC, pin, mode_t>, mode_t>;
		// --- Port D
    template <int pin, uint16_t mode_t=PinMode::Input>
      using PortD = PortClass<PinDriver<RCC_AHBENR_GPIODEN, (uint32_t)GPIOD, pin, mode_t>, mode_t>;
		// --- Port F
    template <int pin, uint16_t mode_t=PinMode::Input>
      using PortF = PortClass<PinDriver<RCC_AHBENR_GPIOFEN, (uint32_t)GPIOF, pin, mode_t>, mode_t>;
			
	}
}

#endif
