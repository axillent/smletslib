#ifndef SMCOM_STM8S_GPIO_H
#define SMCOM_STM8S_GPIO_H

#include "../../../softdrv/gpio.hpp"


namespace Smcom {
	namespace STM8S {

      template <unsigned char volatile *RegDDR, unsigned char volatile *RegIDR, unsigned char volatile *RegODR, unsigned char volatile *RegCR1, unsigned char volatile *RegCR2, int pin, uint8_t mode_t>
        class PinDriver {
        public:
          PinDriver(const uint8_t mode=mode_t) { Init(mode); }
          static void Init(const uint8_t mode=mode_t) {
            if( Smcom::SoftDrv::PinMode::IsOut(mode) ) {
              // output
              *RegDDR |= (1 << pin);
            } else {
              // input mode
              *RegDDR &= ~(1 << pin);
            }
            if( Smcom::SoftDrv::PinMode::IsUp(mode) ) *RegCR1 |= (1 << pin); else *RegCR1 &= ~(1 << pin);
            if( Smcom::SoftDrv::PinMode::IsInt(mode) ) *RegCR2 |= (1 << pin); else *RegCR2 &= ~(1 << pin);
          }
          inline static bool Get() {
            return *RegIDR & (1 << pin);
          }
          inline static void Set() {
            *RegODR |= (1 << pin);
          }
          inline static void Clr() {
            *RegODR &= ~(1 << pin);
          }
        };
      
		template <int pin, uint8_t mode_t=Smcom::SoftDrv::PinMode::Input>
			class PortA : public Smcom::SoftDrv::PortClass<PinDriver<&PA_DDR, &PA_IDR, &PA_ODR, &PA_CR1, &PA_CR2, pin, mode_t>, mode_t> {};

		template <int pin, uint8_t mode_t=Smcom::SoftDrv::PinMode::Input>
			class PortB : public Smcom::SoftDrv::PortClass<PinDriver<&PB_DDR, &PB_IDR, &PB_ODR, &PB_CR1, &PB_CR2, pin, mode_t>, mode_t> {};

		template <int pin, uint8_t mode_t=Smcom::SoftDrv::PinMode::Input>
			class PortC : public Smcom::SoftDrv::PortClass<PinDriver<&PC_DDR, &PC_IDR, &PC_ODR, &PC_CR1, &PC_CR2, pin, mode_t>, mode_t> {};

		template <int pin, uint8_t mode_t=Smcom::SoftDrv::PinMode::Input>
			class PortD : public Smcom::SoftDrv::PortClass<PinDriver<&PD_DDR, &PD_IDR, &PD_ODR, &PD_CR1, &PD_CR2, pin, mode_t>, mode_t> {};

		template <int pin, uint8_t mode_t=Smcom::SoftDrv::PinMode::Input>
			class PortE : public Smcom::SoftDrv::PortClass<PinDriver<&PE_DDR, &PE_IDR, &PE_ODR, &PE_CR1, &PE_CR2, pin, mode_t>, mode_t> {};

		template <int pin, uint8_t mode_t=Smcom::SoftDrv::PinMode::Input>
			class PortF : public Smcom::SoftDrv::PortClass<PinDriver<&PF_DDR, &PF_IDR, &PF_ODR, &PF_CR1, &PF_CR2, pin, mode_t>, mode_t> {};

	}
}

#endif
