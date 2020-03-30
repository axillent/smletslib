#ifndef SMCOM_DRV_TM1650_HPP
#define SMCOM_DRV_TM1650_HPP

#include "../smcom.hpp"

namespace Smcom {	namespace HardDrv {

template <class I2CDrv>
class TM1650 {
public:
  enum type { Segments7 = 8, Segments8 = 0 };
  static const int n = 4;
  // --- switching ON
  static void On(uint8_t bright = 7, type t = Segments8) {
    bright &= 7;
    bright = (bright == 7) ? 0 : bright + 1;
    I2CDrv::SendData(0x48, (bright << 4) | 1 | t);
  }
  // --- switching OFF
  static void Off() {
    I2CDrv::SendData(0x48, 0);
  }
  // --- put masked digit 0-3
  static void PutDigit(uint8_t digit, uint8_t mask) {
    I2CDrv::SendData(0x68 + (digit & 3)*2, mask);
  }
};

}}

#endif
