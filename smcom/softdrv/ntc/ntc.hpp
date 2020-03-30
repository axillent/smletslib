#ifndef SMCOM_SOFTDRV_NTC_NTC_H
#define SMCOM_SOFTDRV_NTC_NTC_H

#include "../flashmem.hpp"
#include "../adc.hpp"

namespace Smcom { 
  namespace SoftDrv {
    namespace NTC {

      template <uint16_t (*read)(uint16_t ), uint16_t size_p, uint8_t count_p, int16_t temp_min_p, int16_t temp_max_p, uint8_t temp_step_p>
        class NTCData {
        public:
          static const int16_t  temp_min = temp_min_p;
          static const int16_t  temp_max = temp_max_p; 
          static const uint8_t  count    = count_p;
          static const uint8_t  temp_step= temp_step_p;
          static const uint16_t size = size_p / sizeof(uint16_t);
          static int16_t temp_C(uint16_t adc_sum) {
            
            uint16_t l = 0;
            uint16_t r = size - 1;

            // Проверка выхода за пределы и граничных значений
            uint16_t thigh = read(r);
            if (adc_sum <= thigh) {
                if (adc_sum < thigh) return temp_min;
                return temp_step * (int)r + temp_min;
            }

            uint16_t tlow = read(0);
            if (adc_sum >= tlow) {
                if (adc_sum > tlow) return temp_max;
                return temp_min;
            }

            // Двоичный поиск по таблице
            while ((r - l) > 1) {
              uint16_t m = (l + r) >> 1;
              uint16_t mid = read(m);
              if (adc_sum > mid) {
                r = m;
              } else {
                l = m;
              }
            }

            uint16_t vl = read(l);
            if (adc_sum >= vl) {
                return (int)l * temp_step + temp_min;
            }

            uint16_t vr = read(r);
            uint16_t vd = vl - vr;
            int16_t res = temp_min + (int)r * temp_step;
            if (vd) {
              // Линейная интерполяция
              res -= ((temp_step * (int32_t)(adc_sum - vr) + (vd >> 1)) / vd);
            }
            return res;
          }
        };
      
      template <class NTCData, class ADCDriver, uint8_t channel>
        class NTC : public Smcom::SoftDrv::ADCDrv::InputFunc<ADCDriver, channel, NTCData::count, NTCData::temp_C> {};
      
}}}

#endif

