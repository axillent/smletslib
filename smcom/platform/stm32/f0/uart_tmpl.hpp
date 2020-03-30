#ifndef SMCOM_STM32_F0_UART_TMPL_H
#define SMCOM_STM32_F0_UART_TMPL_H

#include "../../../common/buffer.hpp"
#include "../../../softdrv/uart.hpp"

#include "gpio.hpp"

namespace Smcom {
	namespace STM32 {

		template <uint32_t uart_base, IRQn_Type irqn, MCUCommon::APBENR apbenr_n, uint32_t apbenr_mask,
			class MCU, const uint32_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
			class UARTDriverTmpl : public Smcom::HandlerData<irqn> {
		protected:
			static struct Data {
              Smcom::Common::Fifo<uint8_t, uint16_t, rx_buffer_size>	rx_buffer;
              Smcom::Common::Fifo<uint8_t, uint16_t, tx_buffer_size>	tx_buffer;
			} data;
			static constexpr USART_TypeDef* uart() { return (USART_TypeDef*)uart_base; }
			static void Handler(void) {
				if( uart()->ISR & USART_ISR_RXNE ) {
					// rx ready
					data.rx_buffer.Push(uart()->RDR);
					uart()->RQR |= USART_RQR_RXFRQ;
				}
				
				if( uart()->ISR & USART_ISR_TXE ) {
					// --- TX complete
					if( data.tx_buffer.AvailablePop() ) {
						// next byte to transmit
						uart()->TDR = data.tx_buffer.Pop();
					} else {
						// disable TX interrupt 
						uart()->CR1 &= ~USART_CR1_TXEIE;
					}
				}
			}
		public:
			static void Start(const uint32_t baudrate=baudrate_t) {
				Smcom::HandlerData<irqn>::SetHandler(Handler);
				
                // --- enable clocking 
				*MCUCommon::RegApbEnr(apbenr_n) |= apbenr_mask;
                // --- clock source
                RCC->CFGR3 |= RCC_CFGR3_USART1SW_0;
                
                // --- baud rate, over8 should be 0, AHB prescaler = 1, APB1 prescaler = 1
                uart()->BRR = MCU::cpu_f / baudrate;
                
                // --- launch usart
				uart()->CR1 |=  USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE;
				uart()->CR1 |=  USART_CR1_UE;

				// GPIO
				PortA<3>::Init();
		  		PortA<2>::Init();
                GPIOA->MODER |= (2 << 4) | (2 << 6);
                GPIOA->AFR[0] |= (1 << 8) | (1 << 12);

				NVIC_EnableIRQ(irqn);
			}
			static void Stop() {
				uart()->CR1 &=  ~USART_CR1_UE;
			}
			static void PutByte(const uint8_t byte) {
				while( !data.tx_buffer.AvailablePush() );	// wait until space in buffer is available
				data.tx_buffer.Push(byte);
                uart()->CR1 |= USART_CR1_TXEIE;
			}
			static uint8_t GetByte(void);
		};

		template <uint32_t uart_base, IRQn_Type irqn, MCUCommon::APBENR apbenr_n, uint32_t apbenr_mask,
			class MCU, const uint32_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
			typename UARTDriverTmpl<uart_base, irqn, apbenr_n, apbenr_mask, MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::Data
				UARTDriverTmpl<uart_base, irqn, apbenr_n, apbenr_mask, MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::data;
		
	}
}

#endif
