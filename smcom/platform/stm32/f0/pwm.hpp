#ifndef SMCOM_STM32_F0_PWM_H
#define SMCOM_STM32_F0_PWM_H

#include "gpio.hpp"
#include "mcu.hpp"

namespace Smcom {
  namespace STM32 {
    namespace PWM {
    
    //------------------------------------------------------------------------
    // Timer universal
    //------------------------------------------------------------------------
    template <uint32_t tim_base, MCUCommon::APBENR apbenr_n, uint32_t apbenr_mask, uint16_t pwm_freq, uint16_t pwm_max, class MCU>
    class TimerUniversal {
	protected:
      constexpr __attribute__((always_inline)) static inline auto tim() { return (TIM_TypeDef *)tim_base; }
    public:
      static void Init() {
        if( !(tim()->CR1 & TIM_CR1_CEN) ) {
          *MCUCommon::RegApbEnr(apbenr_n) |= apbenr_mask;
          tim()->PSC = MCU::cpu_f / pwm_freq / pwm_max;
          tim()->ARR = pwm_max;
          tim()->CR1 |= TIM_CR1_CEN;
        }
      }
      template <Smcom::SoftDrv::PWMMode::pwm_channel channel>
      static void ChannelSet(uint16_t pwm) {
        if constexpr ( channel == 1 ) tim()->CCR1 = pwm;
        if constexpr ( channel == 2 ) tim()->CCR2 = pwm;
        if constexpr ( channel == 3 ) tim()->CCR3 = pwm;
        if constexpr ( channel == 4 ) tim()->CCR4 = pwm;
      }
    };
    
    //------------------------------------------------------------------------
    // Timer 1
    //------------------------------------------------------------------------
    template <uint16_t pwm_freq = 1000, uint16_t pwm_max = 100, class MCU = Smcom::STM32::MCU<> >
      class Timer1 
        : public TimerUniversal<TIM1_BASE, MCUCommon::APB2ENR, RCC_APB2ENR_TIM1EN, pwm_freq, pwm_max, MCU> {
    public:
      template <Smcom::SoftDrv::PWMMode::pwm_channel channel, uint8_t gpio_mode, uint8_t pwm_mode>
      static void ChannelInit() {
        TIM1->BDTR |= TIM_BDTR_MOE;
        if constexpr ( channel == Smcom::SoftDrv::PWMMode::CH2 ) {
          TIM1->CCMR1 |= (pwm_mode << 12);
          TIM1->CCER |= TIM_CCER_CC2E;          
          Smcom::STM32::PortA<9, gpio_mode | Smcom::SoftDrv::PinMode::Alt>::Init();
          Smcom::STM32::PortA<9>::SetAFR(Smcom::STM32::AFR::AF2);
        }
        if constexpr ( channel == Smcom::SoftDrv::PWMMode::CH3 ) {
          TIM1->CCMR2 |= (pwm_mode << 4);
          TIM1->CCER |= TIM_CCER_CC3E;          
          Smcom::STM32::PortA<10, gpio_mode | Smcom::SoftDrv::PinMode::Alt>::Init();
          Smcom::STM32::PortA<10>::SetAFR(Smcom::STM32::AFR::AF2);
        }
      }
    };

    //------------------------------------------------------------------------
    // Timer 3
    //------------------------------------------------------------------------
    template <uint16_t pwm_freq = 1000, uint16_t pwm_max = 100, class MCU = Smcom::STM32::MCU<> >
    class Timer3 
        : public TimerUniversal<TIM3_BASE, MCUCommon::APB1ENR, RCC_APB1ENR_TIM3EN, pwm_freq, pwm_max, MCU> {
    public:
      template <Smcom::SoftDrv::PWMMode::pwm_channel channel, uint8_t gpio_mode, uint8_t pwm_mode>
      static void ChannelInit() {
        if constexpr ( channel == Smcom::SoftDrv::PWMMode::CH4 ) {
          TIM3->CCMR2 |= (pwm_mode << 12);
          TIM3->CCER |= TIM_CCER_CC4E;          
          Smcom::STM32::PortB<1, gpio_mode | Smcom::SoftDrv::PinMode::Alt>::Init();
          Smcom::STM32::PortB<1>::SetAFR(Smcom::STM32::AFR::AF1);
        }
      }
    };
    
}}}

#endif
