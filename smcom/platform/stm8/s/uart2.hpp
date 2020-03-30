#ifndef SMCOM_STM8S_UART2_H
#define SMCOM_STM8S_UART2_H

#include "gpio.hpp"
#include "../../../softdrv/uart.hpp"

namespace Smcom {
  namespace STM8S {

    using Smcom::Common::Fifo;
    
	template <class MCU, const uint32_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
	  class UART2Driver {
	  protected:
        static struct Data {
          Fifo<uint8_t, uint8_t, rx_buffer_size>	rx_buffer;
          Fifo<uint8_t, uint8_t, tx_buffer_size>	tx_buffer;
        } data;
		static void RXHandler(void) {
		  uint8_t byte = UART2_DR;
		  data.rx_buffer.Push(byte);
		  UART2_SR_bit.RXNE = 0;
		}
		static void TXHandler(void) {
		  if( data.tx_buffer.AvailablePop() ) {
			UART2_DR = data.tx_buffer.Pop();
		  } else {
			UART2_CR2_bit.TIEN = 0;
		  }
		}
	  public:
		static void Start(const uint32_t baudrate=baudrate_t) {
		  Smcom::HandlerData<UART2_R_RXNE_vector>::SetHandler(RXHandler);
		  Smcom::HandlerData<UART2_T_TXE_vector>::SetHandler(TXHandler);

		  // GPIO
          Smcom::STM8S::PortD<6, Smcom::SoftDrv::PinMode::Input>::Init();
		  Smcom::STM8S::PortD<5, Smcom::SoftDrv::PinMode::PushPull>::Init();

		  // baud rate registers
		  unsigned long brr = MCU::cpu_f / baudrate;

		  UART2_BRR2 = (brr & 0x000f) | (brr >> 12);
		  UART2_BRR1 = (brr >> 4) & 0x00ff;

		  UART2_CR1 = 0;
		  UART2_CR2 = MASK_UART2_CR2_REN | MASK_UART2_CR2_TEN | MASK_UART2_CR2_RIEN;
		  UART2_CR3 = 0;
		}
		static void Stop() {
		  UART2_CR2 = 0;
		}
		static void PutByte(const uint8_t byte) {
		  while( !data.tx_buffer.AvailablePush() );	// wait until space in buffer is available
		  data.tx_buffer.Push(byte);
		  UART2_CR2_bit.TIEN = 1;
		}
	  };

	template <class MCU, const uint32_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
      typename UART2Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::Data UART2Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::data;

	template <class MCU, const uint32_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
	  class UART2 : public Smcom::SoftDrv::UARTClass<UART2Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>, baudrate_t> {};

	//----------------------------------------
	// UART1 RX vector
	#pragma vector=UART2_R_RXNE_vector
	__interrupt void uart2_rx_interrupt(void){
	  Smcom::HandlerData<UART2_R_RXNE_vector>::CallHandler();
	}

	//----------------------------------------
	// UART1 TX vector
	#pragma vector=UART2_T_TXE_vector
	__interrupt void uart2_tx_interrupt(void){
	  Smcom::HandlerData<UART2_T_TXE_vector>::CallHandler();
	}

  }
}


#endif
