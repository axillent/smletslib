#ifndef SMCOM_HWDRV_REG595_HPP
#define SMCOM_HWDRV_REG595_HPP

#include <string.h>

#include "../smcom.hpp"

namespace Smcom {	namespace HwDrv {

template <class Mosi, class Clk, class Cs, uint8_t n=1>
class Reg595 {
protected:
  static struct Data {
    uint8_t   data[n];
  } data;
public:
  static void Init() {
    Mosi::Init();
    Clk::Init();
    Cs::Init();
    Cs::Set();
    
    memset(data.data, 0, sizeof(data.data));
    Update();
  }
  static void Update() {
    Cs::Clr();
    Clk::Clr();
    
    for(uint8_t j = 0; j < n; j++) {
      for(uint8_t i = 0; i < 8; i++) {
        if( data.data[j] & (1 << i) ) Mosi::Set(); else Mosi::Clr();
        Clk::Set();
        Clk::Clr();
      }
    }
    
    Cs::Set();
  }
  static void Set(uint16_t ch, bool val) {
    uint8_t i = ch << 3;
    uint8_t b = ch & 7;
    if( i >= sizeof(data.data) ) i = 0;
    if( val ) data.data[i] |= (1 << b);
    else data.data[i] &= ~(1 << b);
    Update();
  }
  static bool Get(uint16_t ch) {
    uint8_t i = ch << 3;
    uint8_t b = ch & 7;
    if( i >= sizeof(data.data) ) i = 0;
    return data.data[i] & (1 << b);
  }
};
template <class Mosi, class Clk, class Cs, uint8_t n>
typename Reg595<Mosi,Clk, Cs, n>::Data Reg595<Mosi,Clk, Cs, n>::data;

}}

#endif
