#ifndef STAVRP_STM32_TIMER17_H
#define STAVRP_STM32_TIMER17_H

#include "timer_tmpl.h"

namespace STAVRP {
	namespace STM32 {

	template <typename MCU, int freq, uint16_t psc=0, uint16_t arr=0>
		using Timer17 
			= TimerTmpl<TIM17_BASE, TIM17_IRQn, MCUCommon::APB2ENR, 
			RCC_APB2ENR_TIM17EN, MCU, freq, psc, arr>;

	}
}

extern "C" {
	void TIM17_IRQHandler(void) {
		STAVRP::HandlerData<TIM17_IRQn>::CallHandler();
		TIM17->SR &= ~ TIM_SR_UIF;
	}
}

#endif
