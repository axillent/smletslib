#ifndef STAVRP_STM32_TIMER1_H
#define STAVRP_STM32_TIMER1_H

#include "timer_tmpl.h"

namespace STAVRP {
	namespace STM32 {

	template <typename MCU, uint16_t freq_t, uint16_t psc_t=0, uint16_t arr_t=0>
		using Timer1 
			= TimerTmpl<TIM1_BASE, TIM1_BRK_UP_TRG_COM_IRQn, MCUCommon::APB2ENR, 
			RCC_APB2ENR_TIM1EN, MCU, freq_t, psc_t, arr_t>;

	}
}

extern "C" {
	void TIM1_BRK_UP_TRG_COM_IRQHandler(void) {
		STAVRP::HandlerData<TIM1_BRK_UP_TRG_COM_IRQn>::CallHandler();
		TIM1->SR &= ~ TIM_SR_UIF;
	}
}

#endif
