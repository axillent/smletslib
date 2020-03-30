#ifndef SMCOM_STM8S_TIMER3_H
#define SMCOM_STM8S_TIMER3_H

#include "mcu.hpp"
#include "../../base/timer-common.hpp"

namespace Smcom {
  namespace STM8S {
    
    //----------------------------------------------------------------------
    // Timer3   freq =  CLK / 2^PSCR / ARR
    //----------------------------------------------------------------------
    typedef Smcom::HandlerData<TIM3_OVR_UIF_vector> Timer3Data;
    
    template <typename MCU, uint16_t freq, uint16_t pscr=0, uint16_t arr=0>
      class Timer3 : public Timer3Data {
      public:
        static uint16_t Init(void) {
          uint16_t p, a;
          
          if( freq ) Smcom::Platform::TimerFreqCalc<MCU::cpu_f, freq>::Calc2(p, a);
          else p = pscr, a = arr;
          
          TIM3_PSCR  = p;                
          TIM3_ARRH  = (a >> 8) & 0xff; 	
          TIM3_ARRL  = a & 0xff;     		
          TIM3_EGR |= MASK_TIM3_EGR_UG;
          return MCU::cpu_f / (1 << p) / a;
        }
        static void Start(void) {
          TIM3_IER  = MASK_TIM3_IER_UIE;  						// overflow interrupt enable
          TIM3_CR1  = MASK_TIM3_CR1_URS | MASK_TIM3_CR1_CEN; 	// counting up & start timer
        }
        static void Stop(void) { TIM3_CR1 = 0; }
        static void ResetCounter() { TIM3_EGR |= MASK_TIM3_EGR_UG; }
        static uint16_t GetCounter() {
          uint16_t low = TIM3_CNTRL;
          uint16_t high = TIM3_CNTRH;
          return (high << 8) | low;
        }
        static uint16_t GetCounterMax() {
          uint16_t low = TIM3_ARRL;
          uint16_t high = TIM3_ARRH;
          return (high << 8) | low;
        }
        static void SetCounterMax(uint16_t max) {
          TIM3_ARRH  = (max >> 8) & 0xff;	// count top H
          TIM3_ARRL  = max & 0xff;			// count top L
          TIM3_EGR |= MASK_TIM3_EGR_UG;
        }
        static bool IsPendingUpdate() { return ( TIM3_SR1_bit.UIF ) ? true : false; }
      };
      
//      template <typename MCU, uint16_t freq> class Timer3<MCU, freq, 0, 0>;
//      template <uint16_t pscr, uint16_t arr> class Timer3<MCU<>, 0, pscr, arr>;

      #pragma vector = TIM3_OVR_UIF_vector
      __interrupt void TIM3_OVR_UIF_handler(void) {
        Timer3Data::CallHandler();
        TIM3_SR1_UIF = 0;                 // clearing handler flag
      }
      
  }}

#endif
