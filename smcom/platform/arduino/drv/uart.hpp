#ifndef STAVRP_PLATFORM_ARDUINO_UART_COMM_H
#define STAVRP_PLATFORM_ARDUINO_UART_COMM_H

#include "../../../stavrp.hpp"

namespace STAVRP {
namespace Arduino {

template <const uint32_t baudrate_t>
class UART {
public:
	static void Start(const uint32_t baudrate=baudrate_t) {
		if( baudrate ) Serial.begin(baudrate);
	}
	static void Stop() {
		Serial.end();
	}
	static void PutByte(const uint8_t byte) {
		Serial.write(byte);
	}
	static uint8_t GetByte(void) {
		int c = Serial.read();
		return (c != -1)?c:0;
	}
	static bool RXAvailable(void) {
		return Serial.available() > 0;
	}
	static bool TXAvailable(void) {
		return Serial.availableForWrite() > 0;
	}
	static void TXFlush(void) {
		Serial.flush();
	}
	static void RXFlush(void) {
	}
//	static bool TXEmpty(void);
};

}}

#endif
