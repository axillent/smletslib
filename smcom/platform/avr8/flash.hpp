#ifndef STAVRP_AVR8_FLASH_H
#define STAVRP_AVR8_FLASH_H

#include <stavrp/stavrp.h>

#include <avr/pgmspace.h>

namespace STAVRP {
	namespace AVR8 {

		class FlashAccessor {
			public:
			static uint8_t Read(const uint8_t* address) {
				return pgm_read_byte(address);
			}
			static void ReadBlock(uint8_t* dst, const uint8_t* src, uint16_t size) {
				while(size--) *(dst++) = pgm_read_byte(src++);
			}
		};

	}
}

#endif