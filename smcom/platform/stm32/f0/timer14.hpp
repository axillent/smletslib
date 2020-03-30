#ifndef SMCOM_STM32_TIMER14_H
#define SMCOM_STM32_TIMER14_H

#include "timer_tmpl.hpp"

namespace Smcom {
	namespace STM32 {

	template <typename MCU, int freq, uint16_t psc=0, uint16_t arr=0>
		using Timer14 
			= TimerTmpl<TIM14_BASE, TIM14_IRQn, MCUCommon::APB1ENR, 
			RCC_APB1ENR_TIM14EN, MCU, freq, psc, arr>;

	}
}

extern "C" {
	void TIM14_IRQHandler(void) {
		Smcom::HandlerData<TIM14_IRQn>::CallHandler();
		TIM14->SR &= ~ TIM_SR_UIF;
	}
}


#endif
