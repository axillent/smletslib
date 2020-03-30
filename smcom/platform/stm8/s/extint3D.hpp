/*
Port D
*/

#ifndef STAVRP_STM8S_EXTINT3_H
#define STAVRP_STM8S_EXTINT3_H

#include "mcu.hpp"

namespace STAVRP {
  namespace STM8S {

	template <ExtIntMode::type mode_t=ExtIntMode::FallingRising, stavrp_funcp func_t=nullfunc>
	  class HandlerPortD : public HandlerData<EXTI3_vector> {
	  public:
		HandlerPortD() { Init(); }
		static void Init(stavrp_funcp func=func_t, ExtIntMode::type mode=mode_t) {
		  SetHandler(func);
		  EXTI_CR1_bit.PDIS = mode;
		}
	  };

	#pragma vector = EXTI3_vector
	  __interrupt static void HandlerPortDVector(void) {
		HandlerData<EXTI3_vector>::CallHandler();
	  }

  }
}

#endif