#ifndef SMCOM_SOFTDRV_BCD_MAP_HPP
#define SMCOM_SOFTDRV_BCD_MAP_HPP

#include "../smcom.hpp"
#include "flashmem.hpp"

namespace Smcom {	namespace SoftDrv {

class BCDMap {
protected:
	enum { SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G };
	enum { sym_space = 10, sym_minus, sym_equal, sym_I, sym_t, sym_E, sym_r, sym_o, sym_c, sym_n, sym_d, sym_h, sym_H, sym_P, sym_F };
    static Smcom_flash uint8_t sym_map[30];
    static Smcom_flash uint8_t bcd_map[25];
public:
  static uint8_t Get(char c) {
    
    if(c >= '0' && c <= '9') c -= '0';
    
    if(c <= 9) return flash_getbyte(bcd_map[c]);    // a number
    
    // lookup a char
    for(uint8_t i=0; i < sizeof(sym_map); i += 2)
      if( flash_getbyte(sym_map[i]) == c ) return flash_getbyte(bcd_map[sym_map[i+1]]);
    
    return flash_getbyte(bcd_map[sym_space]);       // unsupported char
  }
};

Smcom_flash uint8_t BCDMap::sym_map[30] = {
		' ', sym_space,
		'-', sym_minus,
		'=', sym_equal,
		'I', sym_I,
		't', sym_t,
		'E', sym_E,
		'r', sym_r,
		'o', sym_o,
		'c', sym_c,
		'n', sym_n,
		'd', sym_d,
		'h', sym_h,
		'H', sym_H,
		'P', sym_P,
		'F', sym_F
};

Smcom_flash uint8_t BCDMap::bcd_map[25] = {
		(1 << SEG_A) | (1 << SEG_B) | (1 << SEG_C) | (1 << SEG_D) | (1 << SEG_E) | (1 << SEG_F) | (0 << SEG_G),	// 0
		(0 << SEG_A) | (1 << SEG_B) | (1 << SEG_C) | (0 << SEG_D) | (0 << SEG_E) | (0 << SEG_F) | (0 << SEG_G),	// 1
		(1 << SEG_A) | (1 << SEG_B) | (0 << SEG_C) | (1 << SEG_D) | (1 << SEG_E) | (0 << SEG_F) | (1 << SEG_G),	// 2
		(1 << SEG_A) | (1 << SEG_B) | (1 << SEG_C) | (1 << SEG_D) | (0 << SEG_E) | (0 << SEG_F) | (1 << SEG_G),	// 3
		(0 << SEG_A) | (1 << SEG_B) | (1 << SEG_C) | (0 << SEG_D) | (0 << SEG_E) | (1 << SEG_F) | (1 << SEG_G),	// 4
		(1 << SEG_A) | (0 << SEG_B) | (1 << SEG_C) | (1 << SEG_D) | (0 << SEG_E) | (1 << SEG_F) | (1 << SEG_G),	// 5
		(1 << SEG_A) | (0 << SEG_B) | (1 << SEG_C) | (1 << SEG_D) | (1 << SEG_E) | (1 << SEG_F) | (1 << SEG_G),	// 6
		(1 << SEG_A) | (1 << SEG_B) | (1 << SEG_C) | (0 << SEG_D) | (0 << SEG_E) | (0 << SEG_F) | (0 << SEG_G),	// 7
		(1 << SEG_A) | (1 << SEG_B) | (1 << SEG_C) | (1 << SEG_D) | (1 << SEG_E) | (1 << SEG_F) | (1 << SEG_G),	// 8
		(1 << SEG_A) | (1 << SEG_B) | (1 << SEG_C) | (1 << SEG_D) | (0 << SEG_E) | (1 << SEG_F) | (1 << SEG_G),	// 9
		(0 << SEG_A) | (0 << SEG_B) | (0 << SEG_C) | (0 << SEG_D) | (0 << SEG_E) | (0 << SEG_F) | (0 << SEG_G),	// ' '
		(0 << SEG_A) | (0 << SEG_B) | (0 << SEG_C) | (0 << SEG_D) | (0 << SEG_E) | (0 << SEG_F) | (1 << SEG_G),	// -
		(0 << SEG_A) | (0 << SEG_B) | (0 << SEG_C) | (1 << SEG_D) | (0 << SEG_E) | (0 << SEG_F) | (1 << SEG_G),	// =
		(0 << SEG_A) | (1 << SEG_B) | (1 << SEG_C) | (0 << SEG_D) | (0 << SEG_E) | (0 << SEG_F) | (0 << SEG_G),	// I
		(0 << SEG_A) | (0 << SEG_B) | (0 << SEG_C) | (1 << SEG_D) | (1 << SEG_E) | (1 << SEG_F) | (1 << SEG_G),	// t
		(1 << SEG_A) | (0 << SEG_B) | (0 << SEG_C) | (1 << SEG_D) | (1 << SEG_E) | (1 << SEG_F) | (1 << SEG_G),	// E
		(0 << SEG_A) | (0 << SEG_B) | (0 << SEG_C) | (0 << SEG_D) | (1 << SEG_E) | (0 << SEG_F) | (1 << SEG_G),	// r
		(0 << SEG_A) | (0 << SEG_B) | (1 << SEG_C) | (1 << SEG_D) | (1 << SEG_E) | (0 << SEG_F) | (1 << SEG_G),	// o
		(0 << SEG_A) | (0 << SEG_B) | (0 << SEG_C) | (1 << SEG_D) | (1 << SEG_E) | (0 << SEG_F) | (1 << SEG_G),	// c
		(0 << SEG_A) | (0 << SEG_B) | (1 << SEG_C) | (0 << SEG_D) | (1 << SEG_E) | (0 << SEG_F) | (1 << SEG_G),	// n
		(0 << SEG_A) | (1 << SEG_B) | (1 << SEG_C) | (1 << SEG_D) | (1 << SEG_E) | (0 << SEG_F) | (1 << SEG_G),	// d
		(0 << SEG_A) | (0 << SEG_B) | (1 << SEG_C) | (0 << SEG_D) | (1 << SEG_E) | (1 << SEG_F) | (1 << SEG_G),	// h
		(0 << SEG_A) | (1 << SEG_B) | (1 << SEG_C) | (0 << SEG_D) | (1 << SEG_E) | (1 << SEG_F) | (1 << SEG_G),	// H
		(1 << SEG_A) | (1 << SEG_B) | (0 << SEG_C) | (0 << SEG_D) | (1 << SEG_E) | (1 << SEG_F) | (1 << SEG_G),	// P
		(1 << SEG_A) | (0 << SEG_B) | (0 << SEG_C) | (0 << SEG_D) | (1 << SEG_E) | (1 << SEG_F) | (1 << SEG_G)	// F
};

}}

#endif
