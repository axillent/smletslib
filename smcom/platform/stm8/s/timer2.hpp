#ifndef SMCOM_STM8S_TIMER2_H
#define SMCOM_STM8S_TIMER2_H

#include "mcu.hpp"
#include "../../base/timer-common.hpp"

namespace Smcom {
  namespace STM8S {
    
    //----------------------------------------------------------------------
    // Timer2   freq =  CLK / 2^PSCR / ARR
    //----------------------------------------------------------------------
    typedef Smcom::HandlerData<TIM2_OVR_UIF_vector> Timer2Data;
    
    template <typename MCU, uint16_t freq, uint16_t pscr=0, uint16_t arr=0>
      class Timer2 : public Timer2Data {
      public:
        static uint16_t Init(void) {
          uint16_t p, a;
          
          if( freq ) Smcom::Platform::TimerFreqCalc<MCU::cpu_f, freq>::Calc2(p, a);
          else p = pscr, a = arr;
          
          TIM2_PSCR  = p;              
          TIM2_ARRH  = (a >> 8) & 0xff; 
          TIM2_ARRL  = a & 0xff;     	
          TIM2_EGR |= MASK_TIM2_EGR_UG;            
          return MCU::cpu_f / (1 << p) / a;
        }
        static void Start(void) {
          TIM2_IER  = MASK_TIM2_IER_UIE;  					// overflow interrupt enable
          TIM2_CR1  = MASK_TIM2_CR1_URS | MASK_TIM2_CR1_CEN; 	// counting up & start timer
        }
        static void Stop(void) { TIM2_CR1 = 0; }
        static void ResetCounter() { TIM2_EGR |= MASK_TIM2_EGR_UG; }
        static uint16_t GetCounter() {
          uint16_t low = TIM2_CNTRL;
          uint16_t high = TIM2_CNTRH;
          return (high << 8) | low;
        }
        static uint16_t GetCounterMax() {
          uint16_t low = TIM2_ARRL;
          uint16_t high = TIM2_ARRH;
          return (high << 8) | low;
        }
        static void SetCounterMax(uint16_t max) {
          TIM2_ARRH  = (max >> 8) & 0xff;	// count top H
          TIM2_ARRL  = max & 0xff;		// count top L
          TIM2_EGR |= MASK_TIM2_EGR_UG;
        }
        static bool IsPendingUpdate() { return ( TIM2_SR1_bit.UIF ) ? true : false; }
      };
      
      //      template <typename MCU, uint16_t freq> class Timer2<MCU, freq, 0, 0> : public Timer2<MCU, freq, 0, 0> {};
      //      template <uint16_t pscr, uint16_t arr> class Timer2<MCU<>, 0, pscr, arr>;
      
      #pragma vector = TIM2_OVR_UIF_vector
      __interrupt void TIM2_OVR_UIF_handler(void) {
        Timer2Data::CallHandler();
        TIM2_SR1_UIF = 0;                 // clearing handler flag
      }
      
  }}

#endif
