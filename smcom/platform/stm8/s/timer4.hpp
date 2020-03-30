#ifndef SMCOM_STM8S_TIMER4_H
#define SMCOM_STM8S_TIMER4_H

#include "mcu.hpp"
#include "../../base/timer-common.hpp"

namespace Smcom {
  namespace STM8S {
    
    //----------------------------------------------------------------------
    // Timer4    freq =  CLK / 2^PSCR / ARR
    //----------------------------------------------------------------------
    typedef Smcom::HandlerData<TIM4_OVR_UIF_vector> Timer4Data;
    
    template <typename MCU, uint16_t freq, uint8_t pscr=0, uint8_t arr=0>
      class Timer4 : public Timer4Data {
      public:
        static uint16_t Init(void) {
          uint8_t p, a;
          
          if( freq ) Smcom::Platform::TimerFreqCalc<MCU::cpu_f, freq>::Calc3(p, a);
          else p = pscr, a = arr;

          TIM4_PSCR = p;
          TIM4_ARR  = a;
          //TIM4_IER  = 0;
          //TIM4_CR1  = 0;
          
          return MCU::cpu_f / (1 << p) / a;
        }
        static void Start(void) {
          TIM4_IER  = MASK_TIM4_IER_UIE;  			// overflow interrupt enable
          TIM4_CR1  = MASK_TIM4_CR1_URS | MASK_TIM4_CR1_CEN; 	// counting up & start timer
        }
        static void Stop(void) { TIM4_CR1 = 0; }
        static void ResetCounter() { TIM4_EGR |= MASK_TIM4_EGR_UG; }
        static uint16_t GetCounter() { return TIM4_CNTR; }
        static uint16_t GetCounterMax() { return TIM4_ARR; }
        static bool IsPendingUpdate() { return ( TIM4_SR_bit.UIF ) ? true : false; }
      };
      
      #pragma vector = TIM4_OVR_UIF_vector
      __interrupt void TIM4_OVR_UIF_handler(void) {
        Timer4Data::CallHandler();
        TIM4_SR_UIF = 0;                 // clearing handler flag
      }
      
  }
}

#endif
