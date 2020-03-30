#ifndef SMCOM_COMMON_PERSISTENCE_HPP
#define SMCOM_COMMON_PERSISTENCE_HPP

#include "../smcom.hpp"

namespace SmCom {
namespace Common {

const uint8_t PersistKey = 0xc7;


SMCOM_DATAPACK_START
struct PersistRecord {
	uint8_t		length;
	uint16_t	high_id;
	uint8_t		low_id;
};
SMCOM_DATAPACK_END

template <class Driver>
class PersistManager {
protected:
	static struct Data {
		uint16_t 		size;
		uint16_t		pointer;
		smcom_funcp		last_func;
	} data;
public:
	static void Init() {
		Driver::Init(data.size);
	}
	static smcom_funcp Register(size_t s, smcom_funcp func) {
		data.size += s + sizeof(PersistRecord);

		smcom_funcp prev = data.last_func;
		data.last_func = func;

		return prev;
	}
	template <typename T>
	static bool Read(uint16_t high_id, uint8_t low_id, T& value) {

		if( Driver::Read(0) != PersistKey ) return false;	///< key not found

		// --- lookup record by id
		for(uint16_t address=1; address < data.size;) {

			PersistRecord	record;
			Driver::Read(address, record);

			if( !record.length ) break;

			address += sizeof(record);

			if( record.high_id == high_id
					&& record.low_id == low_id
					&& record.length == sizeof(T) ) {
				// --- record found
				Driver::Read(address, value);
				return true;
			}

			// --- next record
			address += record.length;

		}

		return false;
	}
	template <typename T>
	static bool Write(uint16_t high_id, uint8_t low_id, const T& value) {

		if( data.pointer + sizeof(PersistRecord) + sizeof(T) <= data.size ) {

			PersistRecord	record = { sizeof(T), high_id, low_id };

			Driver::Write(data.pointer, record);
			data.pointer += sizeof(record);

			Driver::Write(data.pointer, value);
			data.pointer += sizeof(T);
			return true;

		}

		return false;
	}
	static bool MarkWrite() {
		return data.pointer = 1;
	}
	static bool Commit(bool force=false) {

		if( (force || data.pointer) && data.last_func != nullfunc ) {

			Driver::Write(0, PersistKey);

			data.pointer = 1;
			data.last_func();

			Driver::Commit();

			data.pointer = 0;
			return true;

		}
		return false;
	}

};
template <class Driver>
typename PersistManager<Driver>::Data PersistManager<Driver>::data = { 1, 0, nullfunc };

template <class PersistManagerClass, typename T, uint16_t high_id, uint8_t low_id=0>
class PersistType {
protected:
	// --- data
	static struct Data {
		T			value;
		smcom_funcp	next;
	} data;
	// --- internal method
	static void InternalWrite() {

		PersistManagerClass::Write(high_id, low_id, data.value);

		if( data.next != nullfunc ) data.next();

	}
public:
	PersistType() {
		data.next = PersistManagerClass::Register(sizeof(T), InternalWrite);
	}
	/// reading data from persistance storage, returns true on success
	bool Read() {
		return PersistManagerClass::Read(high_id, low_id, data.value);
	}
	/// write data to persistance storage, returns true on success
	bool Write() {
		return PersistManagerClass::MarkWrite();
	}
    operator T& () const {
      return data.value;
    }
	T* operator ->() {
		return &data.value;
	}
};
template <class PersistManagerClass, typename T, uint16_t high_id, uint8_t low_id>
typename PersistType<PersistManagerClass, T, high_id, low_id>::Data PersistType<PersistManagerClass, T, high_id, low_id>::data;


}}

#endif
