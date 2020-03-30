#ifndef SMCOM_STM8S_EXTINT1_H
#define SMCOM_STM8S_EXTINT1_H

#include "mcu.hpp"

namespace Smcom {
  namespace STM8S {
	
	template <MCUCommon::extint mode, smcom_funcp func = nullfunc>
	  class HandlerPortB : public HandlerData<EXTI1_vector> {
	  public:
		HandlerPortB() { Init(); }
		static void Init() {
		  SetHandler(func);
		  EXTI_CR1_bit.PBIS = mode;
		}
	  };
	  
		#pragma vector = EXTI1_vector
	  __interrupt static void HandlerPortBVector(void) {
		HandlerData<EXTI1_vector>::CallHandler();
	  }
	  
  }
}

#endif