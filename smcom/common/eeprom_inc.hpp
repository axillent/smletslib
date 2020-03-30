#ifndef STAVRP_COMMON_EEPROM_INC_H
#define STAVRP_COMMON_EEPROM_INC_H

struct EEPROMKeyData { uint8_t key; };

template <int index=0, uint8_t key=0x55>
class EEPROMKey : protected EEPROM<EEPROMKeyData, index> {
	public:
	static bool IsSet() { return EEPROMAccessor::Read((uint8_t*)&EEPROM<EEPROMKeyData, index>::data) == key; }
	static void Set() {
		if( !IsSet() ) EEPROMAccessor::Write((uint8_t*)&EEPROM<EEPROMKeyData, index>::data, key);
	}
};

template <class Key, typename T, int index=0, typename T2=int>
class EEPROMValue : public EEPROM<T, index, T2> {
	public:
	EEPROMValue() {}
	EEPROMValue(T src) { SetDefault(src); }
	static void SetDefault(T src) {
		if( !Key::IsSet() ) Set(src);
	}
	static void Set(T src) {
		EEPROM<T, index, T2>::Set(src);
	}
};


#endif
