/*
Port C
*/

#ifndef STAVRP_STM8S_EXTINT2_H
#define STAVRP_STM8S_EXTINT2_H

#include "mcu.hpp"

namespace STAVRP {
  namespace STM8S {

	template <ExtIntMode::type mode_t=ExtIntMode::FallingRising, stavrp_funcp func_t=nullfunc>
	  class HandlerPortC : public HandlerData<EXTI2_vector> {
	  public:
		HandlerPortC() { Init(); }
		static void Init(stavrp_funcp func=func_t, ExtIntMode::type mode=mode_t) {
		  SetHandler(func);
		  EXTI_CR1_bit.PCIS = mode;
		}
	  };

	#pragma vector = EXTI2_vector
	  __interrupt static void HandlerPortCVector(void) {
		HandlerData<EXTI2_vector>::CallHandler();
	  }

  }
}

#endif