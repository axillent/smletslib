#ifndef SMCOM_STM8S_TIMER6_H
#define SMCOM_STM8S_TIMER6_H

#include "mcu.hpp"
#include "../../base/timer-common.hpp"

namespace Smcom {
  namespace STM8S {
    
    //----------------------------------------------------------------------
    // Timer6    freq =  CLK / 2^PSCR / ARR
    //----------------------------------------------------------------------
    typedef Smcom::HandlerData<TIM6_OVR_UIF_vector> Timer6Data;
    
    template <typename MCU, int freq, int pscr=0, int arr=0>
      class Timer6 : public Timer6Data {
      public:
        static uint16_t Init(void) {
          uint8_t p, a;
          
          if( freq ) Smcom::Platform::TimerFreqCalc<MCU::cpu_f, freq>::Calc3(p, a);
          else p = pscr, a = arr;

          TIM6_PSCR = p;
          TIM6_ARR  = a;
          
          return MCU::cpu_f / (1 << p) / a;
        }
        static void Start(void) {
          TIM6_IER  = MASK_TIM6_IER_UIE;  			// overflow interrupt enable
          TIM6_CR1  = MASK_TIM6_CR1_URS | MASK_TIM6_CR1_CEN; 	// counting up & start timer
        }
        static void Stop(void) { TIM6_CR1 = 0; }
        static void ResetCounter() { TIM6_EGR |= MASK_TIM6_EGR_UG; }
        static uint16_t GetCounter() { return TIM6_CNTR; }
        static uint16_t GetCounterMax() { return TIM6_ARR; }
        static bool IsPendingUpdate() { return ( TIM6_SR_bit.UIF ) ? true : false; }
      };
      
    #pragma vector = TIM6_OVR_UIF_vector
    __interrupt void TIM6_OVR_UIF_handler(void) {
      Timer6Data::CallHandler();
      TIM6_SR_UIF = 0;                 // clearing handler flag
    }
      
  }
}

#endif
