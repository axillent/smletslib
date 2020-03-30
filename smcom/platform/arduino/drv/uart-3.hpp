#ifndef STAVRP_PLATFORM_ARDUINO_UART_3_COMM_H
#define STAVRP_PLATFORM_ARDUINO_UART_3_COMM_H

#include "../../../stavrp.hpp"

namespace STAVRP {
namespace Arduino {

template <const uint32_t baudrate_t>
class UART_3 {
public:
	static void Start(const uint32_t baudrate=baudrate_t) {
		if( baudrate ) Serial3.begin(baudrate);
	}
	static void Stop() {
		Serial3.end();
	}
	static void PutByte(const uint8_t byte) {
		Serial3.write(byte);
	}
	static uint8_t GetByte(void) {
		int c = Serial3.read();
		return (c != -1)?c:0;
	}
	static bool RXAvailable(void) {
		return Serial3.available() > 0;
	}
	static bool TXAvailable(void) {
		return Serial3.availableForWrite() > 0;
	}
	static void TXFlush(void) {
		Serial3.flush();
	}
	static void RXFlush(void) {
	}
//	static bool TXEmpty(void);
};

}}

#endif
