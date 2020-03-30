#ifndef SMCOM_STM8S_ADC_H
#define SMCOM_STM8S_ADC_H

#include "../../../smcom.hpp"

namespace Smcom {
namespace STM8S {

//----------------------------------------------------------------------
// ADC common driver class
//----------------------------------------------------------------------
  class ADCCommon {
  public:
	enum div_type {
      Div2	= 0,
      Div3	= 1,
      Div4	= 2,
      Div6	= 3,
      Div8	= 4,
      Div10	= 5,
      Div12	= 6,
      Div18	= 7
	};
  };

  template <ADCCommon::div_type div = ADCCommon::Div2>
    class ADCBaseDriver {
    protected:
      enum Stat { NA, Busy, Ready };
      static struct Data {
        Stat        stat;
        uint16_t    data;
      } data;
    public:
      // --- initialize and power on
      static void Start() {
        if( !ADC_CR1_ADON ) {
          ADC_CSR_EOCIE   = 0;      // interrupt by measure end
          ADC_CR1_SPSEL   = div;    // prescaler
          ADC_CR1_CONT    = 0;      // free running
          ADC_CR2_ALIGN   = 1;      // 0/1 left/right adjustment
          ADC_CR1_ADON    = 1;      // power ON
        }
      }
      // --- stop running
      static void Stop(void) {
        ADC_CR1_ADON = 0;
      }
      // --- check if data is ready
      static bool IsBusy(void) {
        switch( data.stat ) {
        case NA:
        case Ready:
          return false;
        }
        if( ADC_CSR_EOC ) {
          data.data = ADC_DRL;
          uint16_t high = ADC_DRH;
          data.data |= (high << 8);
          data.stat = Ready;
          ADC_CSR_EOC  = 0;
          return false;
        }
        return true;
      }
      // --- run next measure
      static void Next(void) {
        while( IsBusy() ) {}
        data.stat = Busy;
        ADC_CR1_ADON = 1;
      }
      // --- setting channel for next run
      static void SetChannel(uint8_t channel) {
        ADC_CSR_CH = channel;
      }
      // --- get channel
      static uint8_t GetChannel(void) {
        return ADC_CSR_CH;
      }
      // --- get value without checking ready status
      // LSB first for right alignment, MSB for left
      static uint16_t ReadValue(void) {
        while( IsBusy() ) {}
        return data.data;
      }
    };
  template <ADCCommon::div_type div>
    typename ADCBaseDriver<div>::Data ADCBaseDriver<div>::data = { NA };

}}

#endif

