#ifndef STAVRP_PLATFORM_ESP8266_H
#define STAVRP_PLATFORM_ESP8266_H

#define _STAVR_MCU_ESP8266

//-----------------------------------------------------------------------------
// ESP specific
//-----------------------------------------------------------------------------

#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>
#include <gpio.h>

typedef void (*__func_)(void*);

extern "C" void ets_isr_attach(int, __func_, void*);
extern "C" void uart_div_modify(int, int);
extern "C" void ets_isr_unmask(int);
extern "C" void ets_install_putc1(void*);

// ===========================
// C includes and declarations
// ===========================
extern "C"
{
#include <user_interface.h>
//#include <mem.h>

//void *pvPortMalloc( size_t xWantedSize );
//void vPortFree( void *pv );
//void *pvPortZalloc(size_t size);

//#define os_malloc   pvPortMalloc
//#define os_free     vPortFree
//#define os_zalloc   pvPortZalloc

}

// ========================================
// These methods shall be defined anywhere.
// They are required for C++ compiler
// ========================================
//void *operator new(size_t size)
//{
//   return os_malloc(size);
//}
//
//void *operator new[](size_t size)
//{
//   return os_malloc(size);
//}
//
//void operator delete(void * ptr)
//{
//   os_free(ptr);
//}
//
//void operator delete[](void * ptr)
//{
//   os_free(ptr);
//}
//
//extern "C" void __cxa_pure_virtual(void) __attribute__ ((__noreturn__));
//extern "C" void __cxa_deleted_virtual(void) __attribute__ ((__noreturn__));
//extern "C" void abort()
//{
//   while(true); // enter an infinite loop and get reset by the WDT
//}
//
//void __cxa_pure_virtual(void) {
//  abort();
//}
//
//void __cxa_deleted_virtual(void) {
//  abort();
//}

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABBBCDDD
 *                A : rf cal
 *                B : at parameters
 *                C : rf init data
 *                D : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
extern "C" uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 8;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

//extern "C" void ICACHE_FLASH_ATTR user_rf_pre_init(void)
//{
//}

//-----------------------------------------------------------------------------
// STAVRP specific
//-----------------------------------------------------------------------------

#include "../stavrp.hpp"

#endif
