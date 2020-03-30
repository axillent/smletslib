#ifndef _STAVRP_ARDUINO_DRV_ENC28J60_H
#define _STAVRP_ARDUINO_DRV_ENC28J60_H

#include <EtherCard.h>

#include "../../../stavrp.hpp"
#include "../../../buffer.hpp"

namespace STAVRP {
namespace Arduino {
namespace ENC28J60 {

//---------------------------------------------------------------------
// Interface Server
//---------------------------------------------------------------------
template <uint16_t t_port>
class TCPServerDrv {
protected:
	static struct Data {
		uint16_t	port;
	} data;
public:
	static void Init(uint16_t p=t_port);
	static uint8_t RXGet();
	static void RXFlush() {}
	static bool RXAvailable();
	static void setPort(uint16_t p) { data.port = p; }
};

//---------------------------------------------------------------------
// Interface Client
//---------------------------------------------------------------------
class TCPClientDrv {
protected:
	static struct Data {
		STAVRP::Fifo<uint8_t, uint8_t, 150>	tx_buffer;
		uint16_t				port;
		uint8_t					ip[4];
		uint16_t				failover_port;
		uint8_t					failover_ip[4];
	} data;
public:
	static void Init() { data.tx_buffer.Clear(); }
	static void TXPut(uint8_t byte) { data.tx_buffer.Push(byte); }
	static bool TXAvailable() { return data.tx_buffer.AvailablePush(); }
	static void TXFlush() {

//		if( tcp_client.connect(IPAddress(data.ip), data.port) ) {
//			Serial.printf("$tcp sending to %d.%d.%d.%d:%d\n", (int)data.ip[0], (int)data.ip[1], (int)data.ip[2], (int)data.ip[3], data.port);
//		} else if( tcp_client.connect(IPAddress(data.failover_ip), data.failover_port) ) {
//			Serial.printf("$tcp failover to %d.%d.%d.%d:%d\n", (int)data.failover_ip[0], (int)data.failover_ip[1], (int)data.failover_ip[2], (int)data.failover_ip[3], data.failover_port);
//		} else {
//			Serial.printf("$tcp sending failed to connect\n");
//		}
//		if( tcp_client.connected() ) {
//			tcp_client.write((char*)data.tx_buffer.data(), data.tx_buffer.size());
//			tcp_client.flush();
//		}
//		data.tx_buffer.clear();
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

//---------------------------------------------------------------------
// Interface Server + Client
//---------------------------------------------------------------------
template <uint16_t t_port>
class TCPDrv : public TCPServerDrv<t_port>, public TCPClientDrv {
public:
	static void Init(uint16_t p=t_port) {
		TCPServerDrv<t_port>::Init(p);
		TCPClientDrv::Init();
	}
};


//---------------------------------------------------------------------
// Implementation
//---------------------------------------------------------------------
//template <uint16_t t_port>
//WiFiServer TCPServerDrv<t_port>::server(t_port);

//template <uint16_t t_port>
//WiFiClient TCPServerDrv<t_port>::tcp_client;

template <uint16_t t_port>
typename TCPServerDrv<t_port>::Data TCPServerDrv<t_port>::data;

TCPClientDrv::Data TCPClientDrv::data;

template <uint16_t t_port>
void TCPServerDrv<t_port>::Init(uint16_t port) {

//	if( !data.port && port ) data.port = port;
//	if( data.port ) {
//		Serial.printf("$tcp server init port=%d\n", data.port);
//		server.begin(data.port);
//	}

}

template <uint16_t t_port>
uint8_t TCPServerDrv<t_port>::RXGet() {
//	uint8_t byte = tcp_client.read();
//	return (byte != -1)?byte:0;
}

template <uint16_t t_port>
bool TCPServerDrv<t_port>::RXAvailable() {

//	if ( !tcp_client ) tcp_client = server.available();
//	if ( !tcp_client ) return false;
//	return tcp_client.available();
}

}}}

#endif
