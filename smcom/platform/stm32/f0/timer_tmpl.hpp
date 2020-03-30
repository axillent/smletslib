#ifndef SMCOM_STM32_TIMER_TMPL_H
#define SMCOM_STM32_TIMER_TMPL_H

#include "mcu.hpp"

namespace Smcom {
  namespace STM32 {

		using Smcom::HandlerData;
		
		//----------------------------------------------------------------------
		// Timer template class
		//----------------------------------------------------------------------
		template <uint32_t tim_base, IRQn_Type irqn, MCUCommon::APBENR apbenr_n, 
			uint32_t apbenr_mask, typename MCU, const uint16_t freq_t, const uint16_t psc_t, const uint16_t arr_t>
		class TimerTmpl : public HandlerData<irqn> {
			protected:
				static constexpr TIM_TypeDef* tim() { return (TIM_TypeDef*)tim_base; }
			public:
			static uint16_t Init(uint16_t freq=freq_t, uint16_t psc=psc_t, uint16_t arr=arr_t) {
				// --- allow clocking for TIM
				*MCUCommon::RegApbEnr(apbenr_n) |= apbenr_mask;

				if( freq ) {
					// common
					uint32_t div = MCU::cpu_f / freq;
					if( div & 0xffff0000 ) {
						uint8_t n;
						for(n = 0; n < 16 && ((div & 0xffff0000) || !(div & 1)); n++) div >>= 1;
						psc = (1 << n) - 1;
						arr = div;						
					} else {
						psc = 0;
						arr = div;
					}
				}
				tim()->PSC = psc;
				tim()->ARR = arr;
				
				return MCU::cpu_f / (1 + psc) / arr;
			}
			static void Start(void) {
					// --- enable timer with update mode
					tim()->CR1 |= TIM_CR1_CEN | TIM_CR1_URS;
					// --- enable interrupts in update
					tim()->DIER |= TIM_DIER_UIE;
					// --- activate interrupts
					NVIC_EnableIRQ(irqn);
			}
			static void Stop(void) {
					// --- diactivate interruots
					NVIC_DisableIRQ(irqn);
					// --- stop clock
					*MCUCommon::RegApbEnr(apbenr_n) &= ~apbenr_mask;
					// disable timer
					tim()->CR1 &= ~TIM_CR1_CEN;
			}
			static void ResetCounter() {
				tim()->EGR |= TIM_EGR_UG;
			}
			static uint16_t GetCounter() { 
				return tim()->CNT; 
			}
			static uint16_t GetCounterMax() { 
				return tim()->ARR; 
			}
			static bool IsPendingUpdate() { 
				return ( tim()->SR & TIM_SR_UIF ) ? true : false;
			}
		};


	}
}

#endif
