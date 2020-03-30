#ifndef SMCOM_PLATFORM_STM8S_DRV_EEPROM_HPP
#define SMCOM_PLATFORM_STM8S_DRV_EEPROM_HPP

#include "../../../smcom.hpp"
#include "eeprom-drv.hpp"

namespace Smcom {
namespace STM8S {

class EEPROMDrv {
protected:
	static uint8_t* ptr(uint16_t address) {
		return (uint8_t*)(EEPROM_START_ADDRESS + address);
	}
public:
	static void Init(size_t size) {
	}
	static uint8_t Read(uint16_t address) {
		return *ptr(address);
	}
	static void Write(uint16_t address, const uint8_t data) {
		EEPROM_Unlock();
		while(FLASH_IAPSR_bit.DUL == 0);

		*ptr(address) = data;

		EEPROM_Lock();
	}
	template <typename T>
	static void Read(uint16_t address, T& data) {
		memcpy(&data, ptr(address), sizeof(T));
	}
	template <typename T>
	static void Write(uint16_t address, const T& data) {
		EEPROM_Unlock();
		while(FLASH_IAPSR_bit.DUL == 0);

		memcpy(ptr(address), &data, sizeof(T));

		EEPROM_Lock();
	}
	static void Commit() {
	}
};

}}

#endif
