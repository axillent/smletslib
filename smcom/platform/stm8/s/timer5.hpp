#ifndef SMCOM_STM8S_TIMER5_H
#define SMCOM_STM8S_TIMER5_H

#include "mcu.hpp"
#include "../../base/timer-common.hpp"

namespace Smcom {
  namespace STM8S {
    
    //----------------------------------------------------------------------
    // Timer5 class
    //----------------------------------------------------------------------
    typedef Smcom::HandlerData<TIM5_OVR_UIF_vector> Timer5Data;
    
    template <typename MCU, int freq, int pscr=0, int arr=0>
      class Timer5 : public Timer5Data {
      public:
        static uint16_t Init(void) {
          uint16_t p, a;
          
          if( freq ) Smcom::Platform::TimerFreqCalc<MCU::cpu_f, freq>::Calc2(p, a);
          else p = pscr, a = arr;
          
          TIM5_PSCR  = p;              
          TIM5_ARRH  = (a >> 8) & 0xff; 
          TIM5_ARRL  = a & 0xff;     	
          TIM5_EGR |= MASK_TIM5_EGR_UG;            
          return MCU::cpu_f / (1 << p) / a;
        }
        static void Start(void) {
          TIM5_IER  = MASK_TIM5_IER_UIE;  						// overflow interrupt enable
          TIM5_CR1  = MASK_TIM5_CR1_URS | MASK_TIM5_CR1_CEN; 	// counting up & start timer
        }
        static void Stop(void) { TIM5_CR1 = 0; }
        static void ResetCounter() { TIM5_EGR |= MASK_TIM5_EGR_UG; }
        static uint16_t GetCounter() {
          uint16_t low = TIM5_CNTRL;
          uint16_t high = TIM5_CNTRH;
          return (high << 8) | low;
        }
        static uint16_t GetCounterMax() {
          uint16_t low = TIM5_ARRL;
          uint16_t high = TIM5_ARRH;
          return (high << 8) | low;
        }
        static void SetCounterMax(uint16_t max) {
          TIM5_ARRH  = (max >> 8) & 0xff;	// count top H
          TIM5_ARRL  = max & 0xff;			// count top L
          TIM5_EGR |= MASK_TIM5_EGR_UG;
        }
        static bool IsPendingUpdate() { return ( TIM5_SR1_bit.UIF ) ? true : false; }
      };
      
      #pragma vector = TIM5_OVR_UIF_vector
      __interrupt void TIM5_OVR_UIF_handler(void) {
        Timer5Data::CallHandler();
        TIM5_SR1_UIF = 0;                 // clearing handler flag
      }
      
  }}

#endif
