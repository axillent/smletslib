#ifndef STAVRP_PLATFORM_ARDUINO_UART_2_COMM_H
#define STAVRP_PLATFORM_ARDUINO_UART_2_COMM_H

#include "../../../stavrp.hpp"

namespace STAVRP {
namespace Arduino {

template <const uint32_t baudrate_t>
class UART_2 {
public:
	static void Start(const uint32_t baudrate=baudrate_t) {
		if( baudrate ) Serial2.begin(baudrate);
	}
	static void Stop() {
		Serial2.end();
	}
	static void PutByte(const uint8_t byte) {
		Serial2.write(byte);
	}
	static uint8_t GetByte(void) {
		int c = Serial2.read();
		return (c != -1)?c:0;
	}
	static bool RXAvailable(void) {
		return Serial2.available() > 0;
	}
	static bool TXAvailable(void) {
		return Serial2.availableForWrite() > 0;
	}
	static void TXFlush(void) {
		Serial2.flush();
	}
	static void RXFlush(void) {
	}
//	static bool TXEmpty(void);
};

}}

#endif
