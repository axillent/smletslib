#ifndef SMCOM_SOFTDRV_SENSORS_PHOTO
#define SMCOM_SOFTDRV_SENSORS_PHOTO

#include "../adc.hpp"

namespace Smcom { 
  namespace SoftDrv {
    namespace Sensors {

int16_t photoresistor_func(uint16_t value) {
  value /= 10;
  if( value > 100 ) value = 100;
  value = 100 - value;
  return value;
}
      
template <class ADCDriver, uint8_t channel, uint16_t count=1>
using PhotoResistor = Smcom::SoftDrv::ADCDrv::InputFunc<ADCDriver, channel, count, photoresistor_func>;

}}}

#endif
