#ifndef SMCOM_BYTEORDER_H
#define SMCOM_BYTEORDER_H

#include "smcom.hpp"

namespace Smcom {
namespace ByteOrder {

//------------------------------------------------
class Check {
public:
	// --- detect
	#ifdef SMCOM_PLATFORM_LITTLE_ENDIAN
		static inline bool LittleEndian() { return true; }
		static inline bool BigEndian() { return false; }
	#elif defined(SMCOM_PLATFORM_BIG_ENDIAN)
		static inline bool LittleEndian() { return false; }
		static inline bool BigEndian() { return true; }
	#else
		static bool LittleEndian() {
			uint16_t number = 1;
			return ( (*(uint8_t*)&number) == 1);
		}
		static inline bool BigEndian() { return !LittleEndian(); }
	#endif
};

class StaticSwap : public Check {
public:
	// --- word
	static void swap(uint16_t& data) {
		uint16_t new_data = (data << 8) | (data >> 8);
		data = new_data;
	}
	static void swap(int16_t& data) { swap(*(uint16_t*)&data); }
	// --- double word
	static void swap(uint32_t& data) {
		uint8_t* a = (uint8_t*)&data;
		uint8_t a01 = a[0];
		a[0] = a[3];
		a[3] = a01;
		a01 = a[1];
		a[1] = a[2];
		a[2] = a01;
	}
	static void swap(int32_t& data) { swap(*(uint32_t*)&data); }
};

class StaticNoSwap : public Check {
public:
	// --- word
	static void swap(uint16_t& data) {}
	static void swap(int16_t& data) {}
	// --- double word
	static void swap(uint32_t& data) {}
	static void swap(int32_t& data) {}
};

//------------------------------------------------
// order types
//------------------------------------------------
// --- swap
// --- word
template <typename T>
class swap_word {
protected:
  uint8_t	data0;
  uint8_t   data1;
public:
  swap_word() {}
  swap_word(T p) { 
    data0 = (static_cast<uint16_t>(p) >> 8); 
    data1 = static_cast<uint16_t>(p) & 0xff; 
  }
  operator T () const { return  (data0 << 8) | data1; }
//#ifdef SMCOM_PLATFORM_STM32F0
//  operator T () const __packed { return  (data0 << 8) | data1; }
//#endif
//  bool operator ==(const swap_word<T>& p) const { return data == p.data; }
};
// --- double word
template <typename T>
class swap_dword {
protected:
  uint32_t	data;
public:
  swap_dword() {}
  swap_dword(T p) {
    StaticSwap::swap(p);
    data = p;
  }
  operator T () const { 
    uint32_t p = data;
    StaticSwap::swap(p);
    return p; 
  }
  bool operator ==(const swap_dword<T>& p) const { return data == p.data; }
};

//------------------------------------------------
#ifdef SMCOM_PLATFORM_BIG_ENDIAN
  namespace LittleEndian {
    class Static : public StaticSwap {};
    typedef swap_word<uint16_t>  uint16;
    typedef swap_word<int16_t>	 int16;
    typedef swap_dword<uint32_t> uint32;
    typedef swap_dword<int32_t>  int32;
  }
  namespace BigEndian {
    class Static : public StaticNoSwap {};
    typedef uint16_t    uint16;
    typedef int16_t     int16;
    typedef uint32_t    uint32;
    typedef int32_t     int32;
  }
#elif defined(SMCOM_PLATFORM_LITTLE_ENDIAN)
  namespace LittleEndian {
    class Static : public StaticNoSwap {};
    typedef uint16_t    uint16;
    typedef int16_t     int16;
    typedef uint32_t    uint32;
    typedef int32_t     int32;
  }
  namespace BigEndian {
    class Static : public StaticSwap {};
    typedef swap_word<uint16_t>	 uint16;
    typedef swap_word<int16_t> 	 int16;
    typedef swap_dword<uint32_t> uint32;
    typedef swap_dword<int32_t>  int32;
  }
#else
	#error Need to write code
#endif

}}

#endif
