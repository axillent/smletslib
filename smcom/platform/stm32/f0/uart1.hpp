#ifndef SMCOM_STM32_F0_UART1_H
#define SMCOM_STM32_F0_UART1_H

#include "uart_tmpl.hpp"

namespace Smcom {
	namespace STM32 {
		
		template <class MCU, const uint32_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
          using UART1 = Smcom::SoftDrv::UARTClass<UARTDriverTmpl<USART1_BASE, USART1_IRQn, MCUCommon::APB2ENR, 
				RCC_APB2ENR_USART1EN, MCU, baudrate_t, rx_buffer_size, tx_buffer_size>, baudrate_t>;

	}
}

extern "C" {
		void USART1_IRQHandler(void) {
			Smcom::HandlerData<USART1_IRQn>::CallHandler();
		}
}

#endif
