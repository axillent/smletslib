#ifndef STAVRP_AVR8_SPI_H
#define STAVRP_AVR8_SPI_H

#include "../spi.h"
#include "gpio.h"

namespace STAVRP {
	namespace AVR8 {

	class SPIPinDriver {
	public:
#if  defined(__AVR_ATmega8__)  || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
|| defined(__AVR_ATmega323__)
#elif defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) || defined(__AVR_ATmega128A__)
		typedef PortB<3>		miso;
		typedef PortB<2>		mosi;
		typedef PortB<1>		sck;
		typedef PortB<0>		ss;
#elif defined(__AVR_ATmega48__) ||defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || \
defined(__AVR_ATmega48P__) ||defined(__AVR_ATmega88P__) || defined(__AVR_ATmega168P__) || \
defined(__AVR_ATmega328P__)
#elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega640__)
#elif defined(__AVR_ATmega644__)
#endif
	};

		struct SPIOpt {
			enum type {
				None = 0,
				LSBFirst = (1 << DORD),
				ClockHighWhenIdle = (1 << CPOL),
				TrailingEdge = (1 << CPHA)
			};
		};

		enum class SPISpeed {
			Div2	= 0b100,
			Div4	= 0b000,
			Div8	= 0b101,
			Div16	= 0b001,
			Div32	= 0b110,
			Div64	= 0b010,
			Div128	= 0b011
		};

		template <SPIMode::type mode, SPISpeed speed, uint8_t option>
		class SPIDriver {
		public:
			static void Start() {
				SPIPinDriver::miso::Init( (mode == SPIMode::Master) ? PinMode::Input : PinMode::Output );
				SPIPinDriver::mosi::Init( (mode == SPIMode::Master) ? PinMode::Output : PinMode::Input );
				SPIPinDriver::sck::Init( (mode == SPIMode::Master) ? PinMode::Output : PinMode::Input );
				//SPIPinDriver::sck::Init( (mode == SPIMode::Master) ? PinMode::Output : PinMode::Input );

				SPCR = (1 << SPE) 
					| option 
					| (static_cast<uint8_t>(speed) & 0b11) 
					| ((mode == SPIMode::Master) ? (1 << MSTR) : 0);
				SPSR = static_cast<uint8_t>(speed) >> 2;
			}
			static void Stop() {
				SPCR = 0;
			}
			static uint8_t Transfer(const uint8_t data) {
				SPDR = data;
				if( mode == SPIMode::Master ) while(!(SPSR & (1<<SPIF)));
				return SPDR;
			}
		};

	}
}

#endif
