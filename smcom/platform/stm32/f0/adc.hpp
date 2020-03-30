/**
	\defgroup stm32/f0
	\ingroup stavr
	\brief STM32/F0 ADC driver
	\author axillent@gmail.com

	STAVRP universal avr8/stm8/stm32 library
	supported: avr-gcc for AVR8
	supported: IAR for STM8
	supported: Keil for STM32

	Example:
	\code
#include <stavrp/stm32/f0/adc.h>
	\endcode

*/

#ifndef SMCOM_STM32_F0_ADC_H
#define SMCOM_STM32_F0_ADC_H

#include "adcbase.hpp"

namespace Smcom {
  namespace STM32 {

	//----------------------------------------------------------------------
	// ADC driver class
	//----------------------------------------------------------------------
	typedef Smcom::HandlerData<ADC1_IRQn> ADCDriverData;

	class ADCDriver : public ADCDriverData, public ADCBaseDriver {
	public:
		static void Start(void) {
          ADCBaseDriver::Start();
          ADC1->CFGR1 |= ADC_CFGR1_CONT;	// continuous mode
          ADC1->IER |= ADC_IER_EOCIE;		// EOC interrupt enable
          ADC1->CR |= ADC_CR_ADSTART;		// start conversion
          NVIC_EnableIRQ(ADC1_IRQn);		// allow interrupts
		}
    };


  }
}

extern "C" {
		void ADC1_IRQHandler(void) {
			Smcom::STM32::ADCDriverData::CallHandler();
			ADC1->ISR |= ADC_ISR_EOC;
		}
}

#endif

