#ifndef STAVRP_STM8S_ADC_INT_H
#define STAVRP_STM8S_ADC_INT_H

#include "adc.hpp"

namespace STAVRP {  namespace STM8S {

//----------------------------------------------------------------------
// ADC driver class with interrupts
//----------------------------------------------------------------------
typedef STAVRP::HandlerData<ADC1_EOC_vector> ADCData;

class ADCInt : public ADCData, public ADC {
public:
	static void Init(DivType div = ADC::Div18, bool cont = true) {
		ADC::Init(div, cont);
		ADC_CSR_EOCIE   = 1;      // interrupt by measure end
	}
};

#pragma vector = ADC1_EOC_vector
__interrupt void ADC1_EOC_handler(void) {

	ADCData::CallHandler();
	ADCCommon::ClrReady();
}

}}

#endif

