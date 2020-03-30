#ifndef SMCOM_PLATFORM_ARDUINO_DRV_EEPROM_ESP8266_HPP
#define SMCOM_PLATFORM_ARDUINO_DRV_EEPROM_ESP8266_HPP

#include "../../../smcom.hpp"

#include <EEPROM.h>

namespace SmCom {
namespace Arduino {
namespace ESP82xx {

class EEPROMDrv {
public:
	static void Init(size_t size) {
		EEPROM.begin(size);
	}
	static uint8_t Read(uint16_t address) {
		return EEPROM.read(address);
	}
	static void Write(uint16_t address, const uint8_t data) {
		EEPROM.write(address, data);
	}
	template <typename T>
	static void Read(uint16_t address, T& data) {
		EEPROM.get(address, data);
	}
	template <typename T>
	static void Write(uint16_t address, const T& data) {
		EEPROM.put(address, data);
	}
	static void Commit() {
		EEPROM.commit();
	}
};

}}}

#endif
