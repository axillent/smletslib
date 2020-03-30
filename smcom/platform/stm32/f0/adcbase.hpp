/**

*/

#ifndef SMCOM_STM32_F0_ADCBASE_H
#define SMCOM_STM32_F0_ADCBASE_H

namespace Smcom {
  namespace STM32 {

	//----------------------------------------------------------------------
	// ADC driver class
	//----------------------------------------------------------------------
    class ADCResolution {
    public:
      enum type {
        Bit12 = 0,
        Bit10 = 8,
        Bit8 = 16,
        Bit6 = 24
      };
    };
    
    template <ADCResolution::type res>
	class ADCBaseDriver {
	public:
		static void Start(void) {
          if( !(ADC1->CR & ADC_CR_ADEN) ) {
            RCC->APB2ENR |= RCC_APB2ENR_ADCEN;			// enable clock
            ADC1->CFGR1 |= res;                         // resolution in bits
            ADC1->CFGR2 |= ADC_CFGR2_JITOFFDIV2;
            ADC1->CR |= ADC_CR_ADEN;					// enable
			while( !(ADC1->ISR & ADC_ISR_ADRDY) );	    // wait for startup to finish
          }
		}
		static void Stop(void) {
          while( IsBusy() );	        // wait for conversion to stop
          ADC1->CR |= ADC_CR_ADDIS;	    // disable
		}
        static bool IsBusy() {
          return ADC1->CR & ADC_CR_ADSTART;
        }
		static void Next(void) {
          while( IsBusy() );	        // wait for conversion to stop          
          ADC1->CR |= ADC_CR_ADSTART;	
        }
		static void SetChannel(uint8_t channel) { 
          ADC1->CHSELR = (1 << channel);
        }
		static uint8_t GetChannel(void) {
			for(uint8_t i=0; i<18; i++) if( ADC1->CHSELR & (1 << i) ) return i;
			return 255;
		}
		static uint16_t ReadValue(void) { 
          while( IsBusy() );	        // wait for conversion to stop          
          return ADC1->DR; 
        }
        static uint16_t MeasureValue() {
            Next();
            return ReadValue();
        }
        static uint16_t MeasureValue(uint16_t channel) {
          SetChannel(channel);
          return MeasureValue();
        }
		static void EnableTemperatureSensor(void) { ADC->CCR |= ADC_CCR_TSEN; }
		static void EnableVRefen(void) { ADC->CCR |= ADC_CCR_VREFEN; }
    };

  }
}


#endif

