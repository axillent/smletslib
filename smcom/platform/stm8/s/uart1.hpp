#ifndef SMCOM_STM8S_UART1_H
#define SMCOM_STM8S_UART1_H

#include "gpio.hpp"
#include "../../../softdrv/uart.hpp"

#ifdef STM8S903K3
  #define UART1_R_RXNE_vector   UART1_RXNE_vector
  #define UART1_T_TXE_vector    UART1_TXE_vector
#endif

namespace Smcom {
  namespace STM8S {

    using Smcom::Common::Fifo;
    
	template <class MCU, const uint32_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
	  class UART1Driver {
	  protected:
        static struct Data {
          Fifo<uint8_t, uint8_t, rx_buffer_size>	rx_buffer;
          Fifo<uint8_t, uint8_t, tx_buffer_size>	tx_buffer;
        } data;
		static void RXHandler(void) {
		  uint8_t byte = UART1_DR;
		  data.rx_buffer.Push(byte);
		  UART1_SR_bit.RXNE = 0;
		}
		static void TXHandler(void) {
		  if( data.tx_buffer.AvailablePop() ) {
			UART1_DR = data.tx_buffer.Pop();
		  } else {
			UART1_CR2_bit.TIEN = 0;
		  }
		}
	  public:
		static void Start(const uint32_t baudrate=baudrate_t) {
		  Smcom::HandlerData<UART1_R_RXNE_vector>::SetHandler(RXHandler);
		  Smcom::HandlerData<UART1_T_TXE_vector>::SetHandler(TXHandler);

		  // GPIO
		  PortD<6, Smcom::SoftDrv::PinMode::Input>::Init();
		  PortD<5, Smcom::SoftDrv::PinMode::PushPull>::Init();

		  // baud rate registers
		  unsigned long brr = MCU::cpu_f / baudrate;

		  UART1_BRR2 = (brr & 0x000f) | (brr >> 12);
		  UART1_BRR1 = (brr >> 4) & 0x00ff;

		  UART1_CR1 = 0;
		  UART1_CR2 = MASK_UART1_CR2_REN | MASK_UART1_CR2_TEN | MASK_UART1_CR2_RIEN;
		  UART1_CR3 = 0;
		}
		static void Stop() {
		  UART1_CR2 = 0;
		}
		static void PutByte(const uint8_t byte) {
		  while( !data.tx_buffer.AvailablePush() );	// wait until space in buffer is available
		  data.tx_buffer.Push(byte);
		  UART1_CR2_bit.TIEN = 1;
		}
	  };

	template <class MCU, const uint32_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
      typename UART1Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::Data UART1Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::data;

	template <class MCU, const uint32_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
	  class UART1 : public Smcom::SoftDrv::UARTClass<UART1Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>, baudrate_t> {};

	//----------------------------------------
	// UART1 RX vector
	#pragma vector=UART1_R_RXNE_vector
	__interrupt void uart1_rx_interrupt(void){
	  Smcom::HandlerData<UART1_R_RXNE_vector>::CallHandler();
	}

	//----------------------------------------
	// UART1 TX vector
	#pragma vector=UART1_T_TXE_vector
	__interrupt void uart1_tx_interrupt(void){
	  Smcom::HandlerData<UART1_T_TXE_vector>::CallHandler();
	}

  }
}


#endif
