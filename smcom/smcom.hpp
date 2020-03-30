#ifndef SMCOM_H
#define SMCOM_H

//-------------------------------------------------------------------------------------------------------
// platform check
#if defined(ARDUINO)
    #ifdef ESP8266
        #define SMCOM_PLATFORM_ARDUINO_ESP8266
    #elif defined(__AVR__)
        #define SMCOM_PLATFORM_ARDUINO_AVR
    #else
        #define SMCOM_PLATFORM_ARDUINO_STM32
    #endif
#elif defined(__AVR__)
    #define SMCOM_PLATFORM_AVR
//#elif defined(__IAR_SYSTEMS_ICC__)
//#elif defined(QCCSTM8)
//#elif defined(__STM8__)
#elif defined(__ICCSTM8__)
    #define SMCOM_PLATFORM_STM8S
#elif defined(STM32F030x4) || defined(STM32F030x6)
    #define SMCOM_PLATFORM_STM32F0
#elif defined(SMCOM_MCU_ESP8266)
    #define SMCOM_PLATFORM_ESP8266
#elif defined(__linux__)
    #define SMCOM_PLATFORM_LINUX
#else
#warning SMCOM default platform
    #define SMCOM_PLATFORM_DEFAULT
#endif

// common datatypes etc.
// --- AVR
#ifdef SMCOM_PLATFORM_AVR
    #define SMCOM_PLATFORM_LITTLE_ENDIAN
    #define SMCOM_DATAPACK_START
    #define SMCOM_DATAPACK_END
    #include <avr/io.h>
    #include <string.h>
    typedef void (*smcom_funcp)();
    const smcom_funcp nullfunc = (smcom_funcp)0;
    #ifdef SMCOM_PRINT_PLATFORM
        #warning Platform AVR
    #endif
// --- STM8S
#elif defined(SMCOM_PLATFORM_STM8S)
    #define SMCOM_PLATFORM_BIG_ENDIAN
    #include <intrinsics.h>
    #ifdef STM8S003F3
        #include <iostm8s003f3.h>
    #elif defined STM8S103F3
        #include <iostm8s103f3.h>
    #elif defined STM8S903K3
        #include <iostm8s903k3.h>
    #elif defined(STM8S105K6)
        #include <iostm8s105k6.h>
    #elif defined(STM8S005K6)
        #include <iostm8s005k6.h>
    #elif defined STM8S105C6
        #include <iostm8s105c6.h>
    #endif
    #include <stdint.h>
    #include <string.h>
    typedef void (*smcom_funcp)();
    const smcom_funcp nullfunc = (smcom_funcp)0;
    #define SMCOM_DATAPACK_START
    #define SMCOM_DATAPACK_END
    #define SMCOM_DATAPACK
    #ifdef SMCOM_PRINT_PLATFORM
        #warning Platform STM8S
    #endif
// --- STM32F0
#elif defined(SMCOM_PLATFORM_STM32F0)
    #define SMCOM_PLATFORM_LITTLE_ENDIAN
    #define SMCOM_DATAPACK_START
    #define SMCOM_DATAPACK_END 
    #define SMCOM_DATAPACK __packed
//    #include "iostm32f0xx.h"
    #include <stdint.h>
    #include <string.h>
    typedef void (*smcom_funcp)();
    const smcom_funcp nullfunc = (smcom_funcp)0;
    #ifdef SMCOM_PRINT_PLATFORM
        #warning Platform STM32F0
    #endif
// --- ESP8266
#elif defined(SMCOM_PLATFORM_ESP8266)
    #define SMCOM_PLATFORM_LITTLE_ENDIAN
    #define SMCOM_DATAPACK_START _Pragma("pack(1)")
    #define SMCOM_DATAPACK_END _Pragma("pack()")
    #include <c_types.h>
    typedef void (*smcom_funcp)();
    const smcom_funcp nullfunc = (smcom_funcp)0;
    #ifdef SMCOM_PRINT_PLATFORM
        #warning Platform ESP8266
    #endif
// --- LINUX
#elif defined(SMCOM_PLATFORM_LINUX)
    #define SMCOM_PLATFORM_UNKNOWN_ENDIAN
    #define SMCOM_DATAPACK_START _Pragma("pack(1)")
    #define SMCOM_DATAPACK_END _Pragma("pack()")
    #include <stdint.h>
    typedef void (*smcom_funcp)();
    const smcom_funcp nullfunc = (smcom_funcp)0;
    #ifdef SMCOM_PRINT_PLATFORM
        #warning Platform Linux
    #endif
// --- Arduino AVR
#elif defined(SMCOM_PLATFORM_ARDUINO_AVR)
    #define SMCOM_PLATFORM_LITTLE_ENDIAN
    #define SMCOM_DATAPACK_START
    #define SMCOM_DATAPACK_END
    #include <stdint.h>
    typedef void (*smcom_funcp)();
    const smcom_funcp nullfunc = (smcom_funcp)0;
    #ifdef SMCOM_PRINT_PLATFORM
        #warning Platform Arduino AVR
    #endif
// --- Arduino ESP8266
#elif defined(SMCOM_PLATFORM_ARDUINO_ESP8266)
    #define SMCOM_PLATFORM_LITTLE_ENDIAN
    #define SMCOM_DATAPACK_START _Pragma("pack(1)")
    #define SMCOM_DATAPACK_END _Pragma("pack()")
    #include <stdint.h>
    typedef void (*smcom_funcp)();
    const smcom_funcp nullfunc = (smcom_funcp)0;
    //  #define SMCOM_DATAPACK_ATTR __attribute__((packed))
    #ifdef SMCOM_PRINT_PLATFORM
        #warning Platform Arduino ESP8266
    #endif
// --- Arduino STM32
#elif defined(SMCOM_PLATFORM_ARDUINO_STM32)
    #define SMCOM_PLATFORM_LITTLE_ENDIAN
    #define SMCOM_DATAPACK_START _Pragma("pack(1)")
    #define SMCOM_DATAPACK_END _Pragma("pack()")
    #include <stdint.h>
    typedef void (*smcom_funcp)();
    const smcom_funcp nullfunc = (smcom_funcp)0;
    #ifdef SMCOM_PRINT_PLATFORM
        #warning Platform Arduino STM32
    #endif
// --- Default
#else
    #include <stdint.h>
    typedef void (*smcom_funcp)();
    const smcom_funcp nullfunc = (smcom_funcp)0;
    #define SMCOM_DATAPACK_START
    #define SMCOM_DATAPACK_END
    #warning Platform Default
    #endif

#ifdef SMCOM_PLATFORM_UNKNOWN_ENDIAN
    #if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
		defined(__BIG_ENDIAN__) || \
		defined(__ARMEB__) || \
		defined(__THUMBEB__) || \
		defined(__AARCH64EB__) || \
		defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
            #define SMCOM_PLATFORM_BIG_ENDIAN
    #elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
		defined(__LITTLE_ENDIAN__) || \
		defined(__ARMEL__) || \
		defined(__THUMBEL__) || \
		defined(__AARCH64EL__) || \
		defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
            #define SMCOM_PLATFORM_LITTLE_ENDIAN
#else
    #error "I don't know what architecture this is!"
#endif
#endif

//--- common types  
namespace Smcom {

    typedef void (*smcom_adcfuncp)(uint16_t value);

#ifndef SMCOM_PLATFORM_LINUX
    void nopfunc() {}
#endif

    namespace Platform {
        enum type { AVR8 = 1, STM8S = 2, STM32F0 = 3, ESP82xx = 4 };
    }

    template <int vector_number_p>
    class HandlerData {
    protected:
        static volatile smcom_funcp handler_ptr;
    public:
        static const int vector_number = vector_number_p;
        static uint8_t IsNullHandler() { return handler_ptr == nullfunc; }
        static void SetHandler(smcom_funcp ptr) { handler_ptr = ptr; }
        static void CallHandler(void) { if (!IsNullHandler()) handler_ptr(); }
    };

    template <int vector_number_p>
    volatile smcom_funcp HandlerData<vector_number_p>::handler_ptr = nullfunc;

}

#endif
