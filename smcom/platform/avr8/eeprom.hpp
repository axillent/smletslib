#ifndef STAVRP_AVR8_EEPROM_H
#define STAVRP_AVR8_EEPROM_H

#include "../stavrp.h"

#include <avr/eeprom.h>

namespace STAVRP {
	namespace AVR8 {

		class EEPROMAccessor {
			public:
			static uint8_t Read(const uint8_t* address) {
				return eeprom_read_byte(address);
			}
			static void ReadBlock(uint8_t* dst, const uint8_t* src, uint16_t size) {
				eeprom_read_block(dst, src, size);
			}
			static void Write(uint8_t* address, uint8_t byte) {
				eeprom_write_byte(address, byte);
			}
			static void WriteBlock(uint8_t* dst, const uint8_t* src, uint16_t size) {
				eeprom_write_block(src, dst, size);
			}
		};

		template <typename T, int index=0, typename T2=int>
		class EEPROM {
			protected:
			static T data;
			public:
			static const  uint16_t size = sizeof(T);
			static T Get(void) {
				T dst;
				EEPROMAccessor::ReadBlock((uint8_t*)&dst, (uint8_t*)&data, sizeof(T));
				return dst;
			}
			static void Set(T src) { EEPROMAccessor::WriteBlock((uint8_t*)&data, (uint8_t*)&src, sizeof(T)); }
			EEPROM& operator=(const T val) {
				Set(val);
				return *this;
			}
			operator T() { return Get(); }
		};
		template <typename T, int index, typename T2>
		EEMEM T EEPROM<T, index, T2>::data;

		#include "../common/eeprom_inc.h"

	}
}

#endif
