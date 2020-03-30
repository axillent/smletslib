/*
Port D7 / TLI
*/

#ifndef STAVRP_STM8S_EXTINTTLI_H
#define STAVRP_STM8S_EXTINTTLI_H

#include "mcu.hpp"

namespace STAVRP {
  namespace STM8S {
	
	const int EXTITLI_vector = 2;

	template <ExtIntMode::type mode_t=ExtIntMode::FallingRising, stavrp_funcp func_t=nullfunc>
	  class HandlerPortTLI : protected HandlerData<EXTITLI_vector> {
	  protected:
		static stavrp_funcp	func;
		static void Handler() {
		  // --- call func
		  if( func != nullfunc ) func();
		  // --- manage virtual mode
		  if( mode_t == ExtIntMode::FallingRising ) {
			PD_CR2_bit.C27 = 0;
			EXTI_CR2_bit.TLIS ^= 1;
			PD_CR2_bit.C27 = 1;
		  }
		}
	  public:
		HandlerPortTLI() { Init(); }
		static void Init(stavrp_funcp func_p=func_t, ExtIntMode::type mode=mode_t) {
		  func = func_p;
		  HandlerData<EXTITLI_vector>::SetHandler(Handler);
		  if( mode_t == ExtIntMode::FallingRising ) EXTI_CR2_bit.TLIS = 0;
		  else EXTI_CR2_bit.TLIS = mode & 1;
		}
      	static void SetHandler(stavrp_funcp ptr) { func = ptr; }
	  };
	template <ExtIntMode::type mode_t, stavrp_funcp func_t>	  
	  stavrp_funcp HandlerPortTLI<mode_t, func_t>::func;

	#pragma vector = EXTITLI_vector
	  __interrupt static void HandlerPortTLIVector(void) {
		HandlerData<EXTITLI_vector>::CallHandler();
	  }

  }
}

#endif