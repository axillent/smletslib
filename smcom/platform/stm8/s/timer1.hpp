#ifndef SMCOM_STM8S_TIMER1_H
#define SMCOM_STM8S_TIMER1_H

#include "mcu.hpp"
#include "../../base/timer-common.hpp"

namespace Smcom {
  namespace STM8S {

    //----------------------------------------------------------------------
    // Timer1   freq =  CLK / (PSCR + 1) / ARR
    //----------------------------------------------------------------------
    typedef Smcom::HandlerData<TIM1_OVR_UIF_vector> Timer1Data;
    
    template <typename MCU, uint16_t freq, uint16_t pscr=0, uint16_t arr=0>
      class Timer1 : public Timer1Data {
      public:
        static uint16_t Init() {
          TIM1_IER = 0;
          TIM1_CR1 = 0;
          
          if( freq ) Smcom::Platform::TimerFreqCalc<MCU::cpu_f, freq>::Calc1(pscr, arr);
          
          TIM1_PSCRH   = (pscr >> 8);	// freq =  CLK / (PSCR + 1)
          TIM1_PSCRL   = (pscr & 0xff);
          TIM1_ARRH  = (arr >> 8);
          TIM1_ARRL  = arr & 0xff;
          
          return MCU::cpu_f / (pscr+1) / arr;
        }
        static void Start(void) {
          TIM1_IER  = MASK_TIM1_IER_UIE;  					    // overflow interrupt enable
          TIM1_CR1  = MASK_TIM1_CR1_URS | MASK_TIM1_CR1_CEN; 	// counting up & start timer
        }
        static void Stop(void) {
          TIM1_CR1 = 0;
        }
      };
      
//      template <typename MCU, uint16_t freq> class Timer1<MCU, freq, 0, 0>;
//      template <uint16_t pscr, uint16_t arr> class Timer1<MCU<>, 0, pscr, arr>;
      
      #pragma vector = TIM1_OVR_UIF_vector
      __interrupt void TIM1_OVR_UIF_handler(void) {
        Timer1Data::CallHandler();
        TIM1_SR1_UIF = 0;                 // clearing handler flag
      }
  }
}

#endif
