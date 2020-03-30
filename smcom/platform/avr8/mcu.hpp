#ifndef STAVRP_AVR8_MCU_H
#define STAVRP_AVR8_MCU_H

#include "../../stavrp.hpp"
#include <avr/interrupt.h>
#include <avr/sleep.h>

namespace STAVRP {

  namespace AVR8 {

    enum class SleepMode  {
	    Idle		= SLEEP_MODE_IDLE,
	    PowerDown	= SLEEP_MODE_PWR_DOWN,
	    SleepADC	= SLEEP_MODE_ADC
    };

    enum class CPUDiv {
      Div1=0, Div2, Div4, Div8, Div16, Div32, Div64, Div128
    };

	class MCUCommon {
	public:
        static void InterruptEnable(void) { sei(); }
        static void InterruptDisable(void) { cli(); }
        static void InterruptWait(void) { Sleep(SleepMode::Idle); }
        static void Sleep(SleepMode mode) {
	        set_sleep_mode((uint8_t) mode);
	        sleep_enable();
	        sei();
	        sleep_cpu();
	        sleep_disable();
        }
	};

    template <unsigned long base_freq, CPUDiv div_factor=CPUDiv::Div1>
      class MCU : public MCUCommon {
      public:
        static const STAVRP::Platform::type platform = STAVRP::Platform::AVR8;
        static const constexpr unsigned int cpu_div = (1 << (uint8_t)div_factor);
        static const constexpr uint32_t cpu_f = base_freq / cpu_div;
        static void Init(void) {
		#ifdef CLKPR
          CLKPR = (1 << CLKPCE);
          CLKPR = (uint8_t)div_factor;
		#endif
        }
      };

  }
}

#endif