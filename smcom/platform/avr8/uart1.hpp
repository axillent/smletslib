#ifndef STAVRP_AVR8_UART1_H
#define STAVRP_AVR8_UART1_H

#include "../buffer.h"
#include "../uart.h"

#include "uart_config.h"

namespace STAVRP {
	namespace AVR8 {

		typedef STAVRP::HandlerData<UART1_RECEIVE_INTERRUPT_num>	UART1RXData;
		typedef STAVRP::HandlerData<UART1_TRANSMIT_INTERRUPT_num>	UART1TXData;

		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		class UART1Driver {
		protected:
			static Fifo<uint8_t, uint8_t, rx_buffer_size>	rx_buffer;
			static Fifo<uint8_t, uint8_t, tx_buffer_size>	tx_buffer;
			static void RXHandler(void);
			static void TXHandler(void);
		public:
			static void Start(const uint16_t baudrate=baudrate_t);
			static void Stop() {}
			static void PutByte(const uint8_t byte);
			static uint8_t GetByte(void);
		};

		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		Fifo<uint8_t, uint8_t, rx_buffer_size> UART1Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::rx_buffer;

		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		Fifo<uint8_t, uint8_t, tx_buffer_size> UART1Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::tx_buffer;

		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		using UART1 = UARTClass<UART1Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>, baudrate_t>;
		
		// --- RX Handler
		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		void UART1Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::RXHandler(void) {
			rx_buffer.Push(UART1_DATA);
		}

		// --- TX Handler
		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		void UART1Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::TXHandler(void) {
			if( tx_buffer.AvailablePop() ) {
				UART1_DATA = tx_buffer.Pop();
			} else {
				/* disable UDRE interrupt */
				UART1_CONTROL &= ~(1 << UART1_UDRIE);
			}
		}

		// --- Init
		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		void UART1Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::Start(const uint16_t baudrate) {

			UART1RXData::SetHandler(RXHandler);
			UART1TXData::SetHandler(TXHandler);

			uint16_t uart1_div = UART_BAUD_RATE_DIV(MCU::cpu_f, baudrate);

			///* Set baud rate */
			if ( uart1_div & 0x8000 ) {
				UART1_STATUS = (1<<U2X1);  //Enable 2x speed
				uart1_div &= ~0x8000;
			}
			UBRR1H = (uint8_t)(uart1_div>>8);
			UBRR1L = (uint8_t) uart1_div;

			///* Enable USART receiver and transmitter and receive complete interrupt */
			UART1_CONTROL = _BV(RXCIE1)|(1<<RXEN1)|(1<<TXEN1);

			///* Set frame format: asynchronous, 8data, no parity, 1stop bit */
			#ifdef URSEL1
				UCSR1C = (1<<URSEL1)|(3<<UCSZ10);
			#else
				UCSR1C = (3<<UCSZ10);
			#endif
		}

		// --- PutByte
		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		void UART1Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::PutByte(const uint8_t byte) {
			while( !tx_buffer.AvailablePush() );	// wait until space in buffer is available
			tx_buffer.Push(byte);

			/* enable UDRE interrupt */
			UART1_CONTROL    |= (1 << UART1_UDRIE);		
		}

		ISR(UART1_RECEIVE_INTERRUPT)
		{
			UART1RXData::CallHandler();
		}

		ISR(UART1_TRANSMIT_INTERRUPT)
		{
			UART1TXData::CallHandler();
		}

	}
}

#endif