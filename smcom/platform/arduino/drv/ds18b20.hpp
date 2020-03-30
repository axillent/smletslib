#ifndef STAVRP_PLATFORM_ARDUINO_DRV_DS18B20_H
#define STAVRP_PLATFORM_ARDUINO_DRV_DS18B20_H

#include "../../../stavrp.hpp"

#include <OneWire.h>
#include <DallasTemperature.h>

namespace STAVRP {
namespace Arduino {


template <int pin>
class DS18B20 {
protected:
	static OneWire				oneWire;
	static DallasTemperature	sensors;
	static struct Data {
		int      temp;
		boolean  requested;
		boolean  connected;
	} data;
public:
	static void Init() {
		sensors.begin();
		data.connected = sensors.getDeviceCount() == 1;
		sensors.setResolution(12);
	}
	static int16_t GetDiv10() {

		if( !data.connected ) return 0;

		if( data.requested ) {

			if( sensors.isConversionComplete() ) {
				data.temp = sensors.getTempCByIndex(0) * 10;
				data.requested = false;
			}

		} else {

			sensors.requestTemperatures();
			data.requested = true;

		}

		return data.temp;
	}
};
template <int pin> typename DS18B20<pin>::Data DS18B20<pin>::data;
template <int pin> OneWire DS18B20<pin>::oneWire(pin);
template <int pin> DallasTemperature DS18B20<pin>::sensors(&DS18B20<pin>::oneWire);

}}

#endif
