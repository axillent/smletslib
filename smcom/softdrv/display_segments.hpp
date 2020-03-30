#ifndef SMCOM_SOFTDRV_DISPLAY_SEGMENTS_HPP
#define SMCOM_SOFTDRV_DISPLAY_SEGMENTS_HPP

#include "../smcom.hpp"
#include "../common/itoa.hpp"

#include "bcd_map.hpp"

namespace Smcom {	namespace SoftDrv {

template <class DisplayDrv, class Map = BCDMap>
class DisplaySegments : public DisplayDrv {
public:
  static void PutChar(char c, uint8_t digit = 0, bool dot = false) {
    DisplayDrv::PutDigit(digit, Map::Get(c) | ((dot)?128:0));
  }
  static void PutString(const char* str, uint8_t digit=0) {
      for(;digit < DisplayDrv::n && *str; digit++, str++) {
        bool dot = str[1] == '.';
		PutChar(*str, digit, dot);
        if( dot ) str++;
      }
	}
  static void PutInt(int n, uint8_t digit = 0, int8_t dot_digit = -1) {
    char buffer[7];
    itoa(n, buffer);
    for(uint8_t i = 0; digit < DisplayDrv::n && i < sizeof(buffer) && buffer[i]; i++, digit++) {
      PutChar(buffer[i], digit, i + 1 == dot_digit);
    }
  }
};

}}

#endif
