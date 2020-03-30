#ifndef STAVRP_STM8S_UART_TMPL_H
#define STAVRP_STM8S_UART_TMPL_H

#include "../buffer.h"
#include "../uart.h"

namespace STAVRP {
	namespace STM8S {

	  struct UARTBitsCR2 {
		enum {
		  SBK = 0x01,
		  RWU = 0x02,
		  REN = 0x04,
		  TEN = 0x08,
		  ILIEN = 0x10,
		  RIEN = 0x20,
		  TCIEN = 0x40,
		  TIEN = 0x80
		};
	  };

		template <
		  	unsigned char volatile* RegCR1,
		  	unsigned char volatile* RegCR2,
		  	unsigned char volatile* RegCR3,
		  	unsigned char volatile* RegDR,
		  	unsigned char volatile* RegSR,
		  	class RXPin, class TXPin, int rx_vector, int tx_vector,
			class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		class UARTDriverTmpl {
		protected:
			static struct Data {
				Fifo<uint8_t, uint8_t, rx_buffer_size>	rx_buffer;
				Fifo<uint8_t, uint8_t, tx_buffer_size>	tx_buffer;
			} data;
			static void RXHandler(void) {
				data.rx_buffer.Push(*RegDR);
				*Reg = 0;
			}
			static void TXHandler(void) {
			}
//			static void Handler(void) {
//				if( uart()->ISR & USART_ISR_RXNE ) {
//					// rx ready
//					data.rx_buffer.Push(uart()->RDR);
//					uart()->RQR &= ~USART_RQR_RXFRQ;
//				}
//
//				if( uart()->ISR & USART_ISR_TC ) {
//					// --- TX complete
//					if( data.tx_buffer.AvailablePop() ) {
//						// next byte to transmit
//						uart()->TDR = data.tx_buffer.Pop();
//					} else {
//						// disable TX interrupt
//						uart()->ISR &= ~USART_ISR_TC;
//						uart()->CR1 &= ~USART_CR1_TXEIE;
//					}
//				}
//			}
		public:
			static void Start(const uint16_t baudrate=baudrate_t) {
				STAVRP::HandlerData<rx_vector>::SetHandler(RXHandler);
				STAVRP::HandlerData<tx_vector>::SetHandler(TXHandler);

				// GPIO
				RXPin::Init(PinMode::Input);
				TXPin::Init(PinMode::Output);

				// baud rate registers
//				UART1_BRR2 = (uart1_div & 0x000f) | (uart1_div >> 12);
//				UART1_BRR1 = (uart1_div >> 4) & 0x00ff;

				*RegCR1	= 0;
				*RegCR2 = UARTBitsCR2::RIEN | UARTBitsCR2::TEN | UARTBitsCR2::REN;
				*RegCR3 = 0;
			}
			static void Stop() {
				*RegCR2 = 0;
			}
			static void PutByte(const uint8_t byte) {
				while( !data.tx_buffer.AvailablePush() );	// wait until space in buffer is available
				data.tx_buffer.Push(byte);
			}
			static uint8_t GetByte(void);
		};

	}
}

#endif
