#ifndef SMCOM_TIMER_COMMON_H
#define SMCOM_TIMER_COMMON_H

namespace Smcom { namespace Platform {

template <uint32_t mcu_freq, uint16_t freq>
class TimerFreqCalc {
public:
  //----------------------------------------------------------------------------
  // freq =  CLK / (PSCR + 1) / ARR
  // PSCR: 0-65535
  // ARR: 0-65535
  static void Calc1(uint16_t& pscr, uint16_t& arr) {
    uint32_t div = mcu_freq / freq;
    if( div & 0xffff0000 ) {
      // divider do not fit 16 bit
      // finding the best split
      uint8_t n;
      for(n = 0; n < 16 && ((div & 0xffff0000) || !(div & 1)); n++) div >>= 1;
      // filling registers
      pscr = (1 << n) - 1;
      arr = div;
    } else {
      // divider fit 16 bit
      pscr = 0;
      arr = div;
    }
  }
  //----------------------------------------------------------------------------
  // freq =  CLK / 2^PSCR / ARR
  // PSCR: 0-15
  // ARR: 0-65535
  static void Calc2(uint16_t& pscr, uint16_t& arr) {
    uint32_t div = mcu_freq / freq;
    if( div & 0xffff0000 ) {
      // divider do not fit 16 bit
      // finding the best split
      for(pscr = 0; pscr < 16 && (div & 0xffff0000); pscr++) div >>= 1;
      // filling registers
      arr = div;
    } else {
      // divider fit 16 bit
      pscr = 0;
      arr = div;
    }
  }
  //----------------------------------------------------------------------------
  // freq =  CLK / 2^PSCR / ARR
  // PSCR: 0-7
  // ARR: 0-255
  static void Calc3(uint8_t& pscr, uint8_t& arr) {
    uint32_t div = mcu_freq / freq;
    if( div & 0xffffff00 ) {
      // divider do not fit 8 bit
      // finding the best split
      for(pscr = 0; pscr < 8 && (div & 0xffffff00); pscr++) div >>= 1;
      // filling registers
      arr = div;
    } else {
      // divider fit 16 bit
      pscr = 0;
      arr = div;
    }
  }
};

}}

#endif
