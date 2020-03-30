#ifndef STAVRP_STM32_TIMER16_H
#define STAVRP_STM32_TIMER16_H

#include "timer_tmpl.h"

namespace STAVRP {
	namespace STM32 {

	template <typename MCU, int freq, uint16_t psc=0, uint16_t arr=0>
		using Timer16 
			= TimerTmpl<TIM16_BASE, TIM16_IRQn, MCUCommon::APB2ENR, 
			RCC_APB2ENR_TIM16EN, MCU, freq, psc, arr>;

	}
}

extern "C" {
	void TIM16_IRQHandler(void) {
		STAVRP::HandlerData<TIM16_IRQn>::CallHandler();
		TIM16->SR &= ~ TIM_SR_UIF;
	}
}

#endif
