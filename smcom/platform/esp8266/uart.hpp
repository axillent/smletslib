#ifndef STAVRP_ESP8266_UART_H
#define STAVRP_ESP8266_UART_H

#include "../../uart.hpp"

//extern "C" {
//	#include "driver/uart_register.h"
//	#include "driver/uart.c"
//}

namespace STAVRP {
  namespace ESP8266 {

	template <const uint32_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
	  class UARTDriver {
	  protected:
		static Fifo<uint8_t, uint8_t, rx_buffer_size>	rx_buffer;
		static Fifo<uint8_t, uint8_t, tx_buffer_size>	tx_buffer;
//		static void RXHandler(void) {
//		  uint8_t byte = UART2_DR;
//		  rx_buffer.Push(byte);
//		  UART2_SR_bit.RXNE = 0;
//		}
//		static void TXHandler(void) {
//		  if( tx_buffer.AvailablePop() ) {
//			UART2_DR = tx_buffer.Pop();
//		  } else {
//			UART2_CR2_bit.TIEN = 0;
//		  }
//		}
	  public:
		static void Start(const uint32_t baudrate=baudrate_t) {
//			uart_init(BIT_RATE_115200, BIT_RATE_115200);
		}
		static void Stop() {
//		  UART2_CR2 = 0;
		}
		static void PutByte(const uint8_t byte) {
//			uart_tx_one_char(byte);
		}
	  };

//	template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
//	  Fifo<uint8_t, uint8_t, rx_buffer_size> UART2Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::rx_buffer;
//	template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
//	  Fifo<uint8_t, uint8_t, tx_buffer_size> UART2Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::tx_buffer;
//
//	template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
//	  class UART2 : public UARTClass<UART2Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>, baudrate_t> {};
//
//	//----------------------------------------
//	// UART1 RX vector
//	#pragma vector=UART2_R_RXNE_vector
//	__interrupt void uart2_rx_interrupt(void){
//	  STAVRP::HandlerData<UART2_R_RXNE_vector>::CallHandler();
//	}
//
//	//----------------------------------------
//	// UART1 TX vector
//	#pragma vector=UART2_T_TXE_vector
//	__interrupt void uart2_tx_interrupt(void){
//	  STAVRP::HandlerData<UART2_T_TXE_vector>::CallHandler();
//	}

	template <const uint32_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
	  Fifo<uint8_t, uint8_t, rx_buffer_size> UARTDriver<baudrate_t, rx_buffer_size, tx_buffer_size>::rx_buffer;
	template <const uint32_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
	  Fifo<uint8_t, uint8_t, tx_buffer_size> UARTDriver<baudrate_t, rx_buffer_size, tx_buffer_size>::tx_buffer;

	template <const uint32_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
	  class UART : public UARTClass<UARTDriver<baudrate_t, rx_buffer_size, tx_buffer_size>, baudrate_t> {};

  }
}


#endif
