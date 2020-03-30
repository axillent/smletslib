#ifndef SMCOM_STM8S_PWM_H
#define SMCOM_STM8S_PWM_H

#include "gpio.hpp"
#include "../../base/timer-common.hpp"

namespace Smcom {
	namespace STM8S {
      namespace PWM {
        
    //------------------------------------------------------------------------
    // Timer 1      freq =  CLK / (PSCR + 1) / ARR
    //------------------------------------------------------------------------
    template <Smcom::SoftDrv::PWMMode::pwm_channel channel> 
      struct Timer1Channel { };

    template<> struct Timer1Channel<Smcom::SoftDrv::PWMMode::CH1> {
      template <uint8_t gpio_mode, uint8_t pwm_mode>
        static void Init() {
          Smcom::STM8S::PortC<1, gpio_mode>::Init();
          TIM1_CCMR1_bit.OC1PE  = 1;
          TIM1_CCMR1_bit.OC1FE  = 1;
          TIM1_CCMR1_bit.OC1M	= pwm_mode;	
          TIM1_CCER1_bit.CC1E   = 1;
          TIM1_BKR_bit.MOE	    = 1;
        }
        static void Set(uint16_t pwm) {
          TIM1_CCR1H = (pwm >> 8);
          TIM1_CCR1L = pwm & 0xff;
        }
      };
    template<> struct Timer1Channel<Smcom::SoftDrv::PWMMode::CH2> {
      template <uint8_t gpio_mode, uint8_t pwm_mode>
        static void Init() {
          Smcom::STM8S::PortC<2, gpio_mode>::Init();
          TIM1_CCMR2_bit.OC2PE  = 1;
          TIM1_CCMR2_bit.OC2FE  = 1;
          TIM1_CCMR2_bit.OC2M	= pwm_mode;	
          TIM1_CCER1_bit.CC2E   = 1;
          TIM1_BKR_bit.MOE	    = 1;
        }
        static void Set(uint16_t pwm) {
          TIM1_CCR2H = (pwm >> 8);
          TIM1_CCR2L = pwm & 0xff;
        }
      };
    template<> struct Timer1Channel<Smcom::SoftDrv::PWMMode::CH3> {
      template <uint8_t gpio_mode, uint8_t pwm_mode>
        static void Init() {
          Smcom::STM8S::PortC<3, gpio_mode>::Init();
          TIM1_CCMR3_bit.OC3PE  = 1;
          TIM1_CCMR3_bit.OC3FE  = 1;
          TIM1_CCMR3_bit.OC3M	= pwm_mode;
          TIM1_CCER2_bit.CC3E   = 1;
          TIM1_BKR_bit.MOE	    = 1;
        }
        static void Set(uint16_t pwm) {
          TIM1_CCR3H = (pwm >> 8);
          TIM1_CCR3L = pwm & 0xff;
        }
      };
    template<> struct Timer1Channel<Smcom::SoftDrv::PWMMode::CH3N> {
      template <uint8_t gpio_mode, uint8_t pwm_mode>
        static void Init() {
          Smcom::STM8S::PortB<2, gpio_mode>::Init();
          TIM1_CCMR3_bit.OC3PE  = 1;
          TIM1_CCMR3_bit.OC3FE  = 1;
          TIM1_CCMR3_bit.OC3M	= pwm_mode;	
          TIM1_CCER2_bit.CC3NE  = 1;
          TIM1_BKR_bit.MOE	    = 1;
        }
      static void Set(uint16_t pwm) {
        TIM1_CCR3H = (pwm >> 8);
        TIM1_CCR3L = pwm & 0xff;
      }
    };
    template<> struct Timer1Channel<Smcom::SoftDrv::PWMMode::CH4> {
      template <uint8_t gpio_mode, uint8_t pwm_mode>
        static void Init() {
          Smcom::STM8S::PortC<4, gpio_mode>::Init();
          TIM1_CCMR4_bit.OC4PE  = 1;
          TIM1_CCMR4_bit.OC4FE  = 1;
          TIM1_CCMR4_bit.OC4M	= pwm_mode;	
          TIM1_CCER2_bit.CC4E   = 1;
          TIM1_BKR_bit.MOE	    = 1;
        }
        static void Set(uint16_t pwm) {
          TIM1_CCR4H = (pwm >> 8);
          TIM1_CCR4L = pwm & 0xff;
        }
      };
    //--------------------------------------------------------------------------
    template <uint16_t pwm_freq = 1000, uint16_t pwm_max = 100, class MCU = Smcom::STM8S::MCU<> >
    class Timer1 {
    public:
      static void Init() {
        uint16_t pscr = MCU::cpu_f / pwm_freq / pwm_max;
        TIM1_PSCRH  = (pscr >> 8);	
        TIM1_PSCRL  = (pscr & 0xff);
        TIM1_ARRH   = (pwm_max >> 8);
        TIM1_ARRL   = pwm_max & 0xff;
        TIM1_CR1_bit.URS = 1;
        TIM1_CR1_bit.CEN = 1;
      }
      template <Smcom::SoftDrv::PWMMode::pwm_channel channel>
      static void ChannelSet(uint16_t pwm) {
        Timer1Channel<channel>::Set(pwm);
      }
      template <Smcom::SoftDrv::PWMMode::pwm_channel channel, uint8_t gpio_mode, uint8_t pwm_mode>
      static void ChannelInit() {
        Timer1Channel<channel>::template Init<gpio_mode, pwm_mode>();
      }
    };
    
        
}}}

#endif

/*      //------------------------------------------------------------------------
      // --- Timer 1
      // --- CH1
      class PWMTimer1CH1 : public PortC<1, PinMode::OutputPushPull> {
      public:
        static void Init() {
          // --- init gpio
          PortC<1, PinMode::OutputPushPull>::Init();
          // --- init pwm
          TIM1_CCMR1_bit.OC1PE  = 1;
          TIM1_CCMR1_bit.OC1FE  = 1;
          TIM1_CCMR1_bit.OC1M	= 6;	// 0b110 - PWM mode 1, 0b111 - PWM mode 2
          TIM1_CCER1_bit.CC1E   = 1;
          TIM1_BKR_bit.MOE	= 1;
        }
        static void SetPWM(uint16_t pwm) {
          TIM1_CCR1H = (pwm >> 8);
          TIM1_CCR1L = pwm & 0xff;
        }
      };
      // --- CH1N
      class PWMTimer1CH1N : public PortB<0, PinMode::OutputPushPull> {
      public:
        static void Init() {
          // --- init gpio
          PortB<0, PinMode::OutputPushPull>::Init();
          // --- init pwm
          TIM1_CCMR1_bit.OC1PE  = 1;
          TIM1_CCMR1_bit.OC1FE  = 1;
          TIM1_CCMR1_bit.OC1M	= 6;	// 0b110 - PWM mode 1, 0b111 - PWM mode 2
          TIM1_CCER1_bit.CC1NE  = 1;
        }
        static void SetPWM(uint16_t pwm) {
          TIM1_CCR1H = (pwm >> 8);
          TIM1_CCR1L = pwm & 0xff;
        }
      };
      // --- CH2
      class PWMTimer1CH2 : public PortC<2, PinMode::OutputPushPull> {
      public:
        static void Init() {
          // --- init gpio
          PortC<2, PinMode::OutputPushPull>::Init();
          // --- init pwm
          TIM1_CCMR2_bit.OC2PE  = 1;
          TIM1_CCMR2_bit.OC2FE  = 1;
          TIM1_CCMR2_bit.OC2M	= 6;	// 0b110 - PWM mode 1, 0b111 - PWM mode 2
          TIM1_CCER1_bit.CC2E   = 1;
        }
        static void SetPWM(uint16_t pwm) {
          TIM1_CCR2H = (pwm >> 8);
          TIM1_CCR2L = pwm & 0xff;
        }
      };
      // --- CH3
      class PWMTimer1CH3 : public PortC<3, PinMode::OutputPushPull> {
      public:
        static void Init() {
          // --- init gpio
          PortC<3, PinMode::OutputPushPull>::Init();
          // --- init pwm
          TIM1_CCMR3_bit.OC3PE  = 1;
          TIM1_CCMR3_bit.OC3FE  = 1;
          TIM1_CCMR3_bit.OC3M	= 6;	// 0b110 - PWM mode 1, 0b111 - PWM mode 2
          TIM1_CCER2_bit.CC3E   = 1;
        }
        static void SetPWM(uint16_t pwm) {
          TIM1_CCR3H = (pwm >> 8);
          TIM1_CCR3L = pwm & 0xff;
        }
      };
      // --- CH4
      class PWMTimer1CH4 : public PortC<4, PinMode::OutputPushPull> {
      public:
        static void Init() {
          // --- init gpio
          PortC<4, PinMode::OutputPushPull>::Init();
          // --- init pwm
          TIM1_CCMR4_bit.OC4PE  = 1;
          TIM1_CCMR4_bit.OC4FE  = 1;
          TIM1_CCMR4_bit.OC4M	= 6;	// 0b110 - PWM mode 1, 0b111 - PWM mode 2
          TIM1_CCER2_bit.CC4E   = 1;
        }
        static void SetPWM(uint16_t pwm) {
          TIM1_CCR4H = (pwm >> 8);
          TIM1_CCR4L = pwm & 0xff;
        }
      };

      //------------------------------------------------------------------------
      // --- Timer 2
      // --- CH1
      class PWMTimer2CH1 : public PortD<4, PinMode::OutputPushPull> {
      public:
        static void Init() {
          // --- init gpio
          PortD<4, PinMode::OutputPushPull>::Init();
          // --- init pwm
          TIM2_CCMR1_bit.OC1PE  = 1;
          TIM2_CCMR1_bit.OC1M	= 6;	// 0b110 - PWM mode 1, 0b111 - PWM mode 2
          TIM2_CCER1_bit.CC1E   = 1;
        }
        static void SetPWM(uint16_t pwm) {
          TIM2_CCR1H = (pwm >> 8);
          TIM2_CCR1L = pwm & 0xff;
        }
      };
      // --- CH2
      class PWMTimer2CH2 : public PortD<3, PinMode::OutputPushPull> {
      public:
        static void Init() {
          // --- init gpio
          PortD<3, PinMode::OutputPushPull>::Init();
          // --- init pwm
          TIM2_CCMR2_bit.OC2PE  = 1;
          TIM2_CCMR2_bit.OC2M	= 6;	// 0b110 - PWM mode 1, 0b111 - PWM mode 2
          TIM2_CCER1_bit.CC2E   = 1;
        }
        static void SetPWM(uint16_t pwm) {
          TIM2_CCR2H = (pwm >> 8);
          TIM2_CCR2L = pwm & 0xff;
        }
      };
      // --- CH3
      class PWMTimer2CH3 : public PortA<3, PinMode::OutputPushPull> {
      public:
        static void Init() {
          // --- init gpio
          PortA<3, PinMode::OutputPushPull>::Init();
          // --- init pwm
          TIM2_CCMR3_bit.OC3PE  = 1;
          TIM2_CCMR3_bit.OC3M	= 6;	// 0b110 - PWM mode 1, 0b111 - PWM mode 2
          TIM2_CCER2_bit.CC3E   = 1;
        }
        static void SetPWM(uint16_t pwm) {
          TIM2_CCR3H = (pwm >> 8);
          TIM2_CCR3L = pwm & 0xff;
        }
      };

      //------------------------------------------------------------------------
      // --- Timer 3
      // --- CH1
      class PWMTimer3CH1 : public PortD<2, PinMode::OutputPushPull> {
      public:
        static void Init() {
          // --- init gpio
          PortD<2, PinMode::OutputPushPull>::Init();
          // --- init pwm
          TIM3_CCMR1_bit.OC1PE  = 1;
          TIM3_CCMR1_bit.OC1M	= 6;	// 0b110 - PWM mode 1, 0b111 - PWM mode 2
          TIM3_CCER1_bit.CC1E   = 1;
        }
        static void SetPWM(uint16_t pwm) {
          TIM3_CCR1H = (pwm >> 8);
          TIM3_CCR1L = pwm & 0xff;
        }
      };
      // --- CH2
      class PWMTimer3CH2 : public PortD<0, PinMode::OutputPushPull> {
      public:
        static void Init() {
          // --- init gpio
          PortD<0, PinMode::OutputPushPull>::Init();
          // --- init pwm
          TIM3_CCMR2_bit.OC2PE  = 1;
          TIM3_CCMR2_bit.OC2M	= 6;	// 0b110 - PWM mode 1, 0b111 - PWM mode 2
          TIM3_CCER1_bit.CC2E   = 1;
        }
        static void SetPWM(uint16_t pwm) {
          TIM3_CCR2H = (pwm >> 8);
          TIM3_CCR2L = pwm & 0xff;
        }
      };
      
*/
