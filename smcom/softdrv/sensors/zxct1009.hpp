#ifndef SMCOM_SOFTDRV_SENSORS_ZXCT1009
#define SMCOM_SOFTDRV_SENSORS_ZXCT1009

#include "../adc.hpp"

namespace Smcom { 
  namespace SoftDrv {
    namespace Sensors {

template <uint16_t modul, uint16_t curmax, uint16_t count>
int16_t zxct1009_func(uint16_t value) {
  uint32_t v = value;
  v /= count;
  v *= curmax;
  v /= 1024;
  v = v - v % modul;
  return v;
}
      
template <class ADCDriver, uint8_t channel, uint16_t modul, uint16_t curmax, uint16_t count=1>
using ZXCT1009 
  = Smcom::SoftDrv::ADCDrv::InputFunc<ADCDriver, channel, count, zxct1009_func<modul, curmax, count> >;

}}}

#endif
