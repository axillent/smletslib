#ifndef SMCOM_STM32_TIMER3_H
#define SMCOM_STM32_TIMER3_H

#include "timer_tmpl.hpp"

namespace Smcom {
	namespace STM32 {

	template <typename MCU, int freq, uint16_t psc=0, uint16_t arr=0>
		using Timer3 
			= TimerTmpl<TIM3_BASE, TIM3_IRQn, MCUCommon::APB1ENR, RCC_APB1ENR_TIM3EN,
			MCU, freq, psc, arr>;

	}
}

extern "C" {
	void TIM3_IRQHandler(void) {
		Smcom::HandlerData<TIM3_IRQn>::CallHandler();
		TIM3->SR &= ~ TIM_SR_UIF;
	}
}

#endif
