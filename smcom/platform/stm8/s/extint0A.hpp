#ifndef STAVRP_STM8S_EXTINT0_H
#define STAVRP_STM8S_EXTINT0_H

#include "mcu.hpp"

namespace STAVRP {
namespace STM8S {

template <ExtIntMode::type mode_t, stavrp_funcp func_t=nullfunc>
class HandlerPortA : public HandlerData<EXTI0_vector> {
public:
	HandlerPortA() { Init(); }
	static void Init(stavrp_funcp func=func_t, ExtIntMode::type mode=mode_t) {
		SetHandler(func);
		EXTI_CR1_bit.PAIS = mode;
	}
};

#pragma vector = EXTI0_vector
__interrupt static void HandlerPortAVector(void) {
	HandlerData<EXTI0_vector>::CallHandler();
}

}}

#endif
