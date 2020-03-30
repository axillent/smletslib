#ifndef SMCOM_SOFTDRV_FLASHMEM_H
#define SMCOM_SOFTDRV_FLASHMEM_H

//-----------------------------------------------------------------------------
#ifdef SMCOM_PLATFORM_AVR

	#include <avr/pgmspace.h>
	#include <string.h>

	typedef void (*_flash_f)(void);

	#define Smcom_flash	    	PROGMEM const

	#define flash_str(s)		PSTR(s)
	#define flash_memcpy		memcpy_P
	#define flash_strlen		strlen_P
	#define flash_strcpy		strcpy_P
	#define flash_strncpy		strncpy_P
	#define flash_strcmp		strcmp_P
	#define flash_strncmp		strncmp_P
	#define flash_strsep		strsep_P
	#define flash_getbyte(v)	pgm_read_byte(&v)
	#define flash_getword(v)	pgm_read_word(&v)
	#define flash_getptr(v)		pgm_read_ptr(&v)
	#define flash_call(func)	{ void (*f)(); f = pgm_read_ptr(&func); f(); }

#elif defined(SMCOM_PLATFORM_STM8S) || defined(SMCOM_PLATFORM_STM32F0)

	#include <string.h>

	#define Smcom_flash		    const

	#define flash_str(s)		s
	#define flash_memcpy		memcpy
	#define flash_strlen		strlen
	#define flash_strcpy		strcpy
	#define flash_strncpy		strncpy
	#define flash_strcmp		strcmp
	#define flash_strncmp		strncmp
	#define flash_strsep		strsep
	#define flash_getbyte(v)	v
	#define flash_getword(v)	v
	#define flash_getptr(v)		v
	#define flash_call(func)	func();

#else
	#error SMCOM flashmem MCU is not supported
#endif

//-----------------------------------------------------------------------------
namespace Smcom { 
  namespace SoftDrv { 
    namespace Flash {

		template <typename T, const T* data_ptr, uint16_t size_p>
		class Array {
		public:
			static const uint16_t size = size_p;
			static T get(uint16_t i);
		};

		template <typename T, const T* data_ptr, uint16_t size_p>
		T Array<T, data_ptr, size_p>::get(uint16_t i) {
			if( sizeof(T) == 1 ) return flash_getbyte(data_ptr[i]);
			if( sizeof(T) == 2 ) return flash_getword(data_ptr[i]);
			return 0;
		}

}}}


#endif
