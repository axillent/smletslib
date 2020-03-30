#ifndef _STAVRP_ARDUINO_DRV_STM32W5500_H
#define _STAVRP_ARDUINO_DRV_STM32W5500_H

#include <vector>

#include <SPI.h>
#include <Ethernet_STM.h>

#include "../../../stavrp.hpp"
#include "../../../buffer.hpp"

namespace STAVRP {
namespace Arduino {
namespace STM32W5500 {

//---------------------------------------------------------------------
// Interface Server
//---------------------------------------------------------------------
template <uint16_t port_t>
class TCPServerDrv {
protected:
	static EthernetServer server;
	static EthernetClient client;
public:
	static void Init() {
		server.begin();
	}
	static uint8_t RXGet() {
		int byte = client.read();
		return (byte != -1) ? byte : 0;
	}
	static void RXFlush() {}
	static bool RXAvailable() {
		if ( !client ) client = server.available();
		if ( !client ) return false;
		return client.available();
	}
};
template <uint16_t port_t> EthernetServer TCPServerDrv<port_t>::server(port_t);
template <uint16_t port_t> EthernetClient	 TCPServerDrv<port_t>::client;

//---------------------------------------------------------------------
// Interface Client
//---------------------------------------------------------------------
class TCPClientDrv {
protected:
	static EthernetClient client;
	static struct Data {
		std::vector<uint8_t>	tx_buffer;
		uint16_t	port;
		uint8_t		ip[4];
		uint16_t	failover_port;
		uint8_t		failover_ip[4];
	} data;
public:
	static void Init() { data.tx_buffer.clear(); }
	static void TXPut(uint8_t byte) { data.tx_buffer.push_back(byte); }
	static bool TXAvailable() { return true; }
	static bool TXFlush() {

		bool result = client.connect(IPAddress(data.ip), data.port);
		char buf[64];

		if( !result ) {

			sprintf(buf, "$tcp failed %d.%d.%d.%d:%d", (int)data.ip[0], (int)data.ip[1], (int)data.ip[2], (int)data.ip[3], data.port);
			Serial.println(buf);

			result = client.connect(IPAddress(data.failover_ip), data.failover_port);

			if( !result ) {
				sprintf(buf, "$tcp failed failover %d.%d.%d.%d:%d", (int)data.failover_ip[0], (int)data.failover_ip[1], (int)data.failover_ip[2], (int)data.failover_ip[3], data.failover_port);
				Serial.println(buf);
			}
		}

		if( client.connected() ) {
			client.write((char*)data.tx_buffer.data(), data.tx_buffer.size());
			client.println();
			client.flush();
		}
		client.stop();
		data.tx_buffer.clear();
		return true;
	}
	static bool setIPPort(const uint8_t* ip, uint16_t port) {
		memcpy(data.ip, ip, sizeof(data.ip));
		data.port = port;
	}
	static bool setFailoverIPPort(const uint8_t* ip, uint16_t port) {
		memcpy(data.failover_ip, ip, sizeof(data.ip));
		data.failover_port = port;
	}
};
TCPClientDrv::Data TCPClientDrv::data;
EthernetClient TCPClientDrv::client;

//---------------------------------------------------------------------
// Interface Server + Client
//---------------------------------------------------------------------
template <uint16_t port_t>
class TCPDrv : public TCPServerDrv<port_t>, public TCPClientDrv {
public:
	static void Init() {
		TCPServerDrv<port_t>::Init();
		TCPClientDrv::Init();
	}
};


}}}

#endif
