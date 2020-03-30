/**

	\todo template parameter to select reference voltage
	\todo template parameter to select prescaler
*/

#ifndef STAVRP_AVR8_ADC_H
#define STAVRP_AVR8_ADC_H

#include <avr/interrupt.h>

namespace STAVRP {
  namespace AVR8 {

    //----------------------------------------------------------------------
    // ADC driver class
    //----------------------------------------------------------------------
    typedef STAVRP::HandlerData<ADC_vect_num> ADCDriverData;

    class ADCDriver : public ADCDriverData {
    public:
      static void Start(void) {
		ADMUX = (1 << REFS0);
		ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADSC) | (1 << ADSC);
	  }
      static void Stop(void) { ADCSRA &= ~(1 << ADSC); }
      static void Next(void) { ADCSRA |= (1 << ADSC); }
      static void SetChannel(uint16_t channel) { ADMUX = (ADMUX & 0xf0) | channel; }
	  static uint8_t GetChannel(void) { return ADMUX & 0xf0; }
      static uint16_t GetValue(void) { return ADCW; }
    };

    ISR(ADC_vect) {
      ADCDriverData::CallHandler();
    }

  }
}

#endif

