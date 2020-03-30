#ifndef STAVRP_AVR8_WDT_H
#define STAVRP_AVR8_WDT_H

#include "mcu.h"
#include <avr/wdt.h>

namespace STAVRP {

	namespace AVR8 {

		enum class WDTTimeout {
			To15msec = WDTO_15MS,
			To30msec = WDTO_30MS,
			To60msec = WDTO_60MS,
			To120msec = WDTO_120MS,
			To250msec = WDTO_250MS,
			To500msec = WDTO_500MS,
			To1sec = WDTO_1S,
			To2sec = WDTO_2S,
			To4sec = WDTO_4S,
			To8sec = WDTO_8S
		};

		typedef STAVRP::HandlerData<WDT_vect_num> WDTData;		
		
		template <stavrp_funcp handler_ptr = nullptr>
		class WDT : public WDTData {
			public:
			static void Reset(void) { wdt_reset(); }
			static void Enable(WDTTimeout to) {
				SetHandler(handler_ptr);
				wdt_enable(static_cast<uint8_t>(to));
			}
			static void Disable(void) { wdt_disable(); }
		};
		
		ISR(WDT_vect) {
			WDTData::CallHandler();
		}

	}
}

#endif