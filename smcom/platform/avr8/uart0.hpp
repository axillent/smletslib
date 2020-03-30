#ifndef STAVRP_AVR8_UART0_H
#define STAVRP_AVR8_UART0_H

#include "../buffer.h"
#include "../uart.h"

#include "uart_config.h"

namespace STAVRP {
	namespace AVR8 {

		typedef STAVRP::HandlerData<UART0_RECEIVE_INTERRUPT_num>	UART0RXData;
		typedef STAVRP::HandlerData<UART0_TRANSMIT_INTERRUPT_num>	UART0TXData;

		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		class UART0Driver {
		protected:
			static Fifo<volatile uint8_t, uint8_t, rx_buffer_size>	rx_buffer;
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
		Fifo<volatile uint8_t, uint8_t, rx_buffer_size> UART0Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::rx_buffer;

		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		Fifo<uint8_t, uint8_t, tx_buffer_size> UART0Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::tx_buffer;

		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		using UART0 = UARTClass<UART0Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>, baudrate_t>;

		// --- RX Handler
		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		void UART0Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::RXHandler(void) {
			rx_buffer.Push(UART0_DATA);
		}

		// --- TX Handler
		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		void UART0Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::TXHandler(void) {
			if( tx_buffer.AvailablePop() ) {
				UART0_DATA = tx_buffer.Pop();
			} else {
				/* disable UDRE interrupt */
				UART0_CONTROL &= ~(1 << UART0_UDRIE);
			}
		}

		// --- Init
		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		void UART0Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::Start(const uint16_t baudrate) {

			UART0RXData::SetHandler(RXHandler);
			UART0TXData::SetHandler(TXHandler);

			uint16_t uart1_div = UART_BAUD_RATE_DIV(MCU::cpu_f, baudrate);

			#if defined( AT90_UART )
				/* set baud rate */
				UBRR = (uint8_t)uart1_div;

				/* enable UART receiver and transmitter and receive complete interrupt */
				UART0_CONTROL = _BV(RXCIE)|_BV(RXEN)|_BV(TXEN);

			#elif defined (ATMEGA_USART)
				/* Set baud rate */
				if ( uart1_div & 0x8000 ) {
					UART0_STATUS = (1<<U2X);  //Enable 2x speed
					uart1_div &= ~0x8000;
				}
				UBRRH = (uint8_t)(uart1_div>>8);
				UBRRL = (uint8_t) uart1_div;

				/* Enable USART receiver and transmitter and receive complete interrupt */
				UART0_CONTROL = _BV(RXCIE)|(1<<RXEN)|(1<<TXEN);

				/* Set frame format: asynchronous, 8data, no parity, 1stop bit */
				#ifdef URSEL
					UCSRC = (1<<URSEL)|(3<<UCSZ0);
				#else
					UCSRC = (3<<UCSZ0);
				#endif

			#elif defined ( ATMEGA_USART0 )
				/* Set baud rate */
				if ( uart1_div & 0x8000 ) {
					UART0_STATUS = (1<<U2X0);  //Enable 2x speed
					uart1_div &= ~0x8000;
				}
				UBRR0H = (uint8_t)(uart1_div>>8);
				UBRR0L = (uint8_t) uart1_div;

				/* Enable USART receiver and transmitter and receive complete interrupt */
				UART0_CONTROL = _BV(RXCIE0)|(1<<RXEN0)|(1<<TXEN0);

				/* Set frame format: asynchronous, 8data, no parity, 1stop bit */
				#ifdef URSEL0
				UCSR0C = (1<<URSEL0)|(3<<UCSZ00);
				#else
				UCSR0C = (3<<UCSZ00);
				#endif

			#elif defined ( ATMEGA_UART )
				/* set baud rate */
				if ( uart1_div & 0x8000 ) {
					UART0_STATUS = (1<<U2X);  //Enable 2x speed
					uart1_div &= ~0x8000;
				}
				UBRRHI = (uint8_t)(uart1_div>>8);
				UBRR   = (uint8_t) uart1_div;

				/* Enable UART receiver and transmitter and receive complete interrupt */
				UART0_CONTROL = _BV(RXCIE)|(1<<RXEN)|(1<<TXEN);
			#endif
		}

		// --- PutByte
		template <class MCU, const uint16_t baudrate_t, int rx_buffer_size, int tx_buffer_size>
		void UART0Driver<MCU, baudrate_t, rx_buffer_size, tx_buffer_size>::PutByte(const uint8_t byte) {
			while( !tx_buffer.AvailablePush() );	// wait until space in buffer is available
			tx_buffer.Push(byte);

			/* enable UDRE interrupt */
			UART0_CONTROL    |= (1 << UART0_UDRIE);
		}

		ISR(UART0_RECEIVE_INTERRUPT)
		{
			UART0RXData::CallHandler();
		}

		ISR(UART0_TRANSMIT_INTERRUPT)
		{
			UART0TXData::CallHandler();
		}

	}
}

#endif