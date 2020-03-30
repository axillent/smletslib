#ifndef _STAVRP_ARDUINO_DRV_SERVER_ESP8266_H
#define _STAVRP_ARDUINO_DRV_SERVER_ESP8266_H

#include <string>
#include <vector>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <IPAddress.h>
//#include <ESP8266mDNS.h>

#include "../../../stavrp.hpp"

namespace STAVRP {
namespace Arduino {
namespace ESP82xx {

//---------------------------------------------------------------------
// Interface Server
//---------------------------------------------------------------------
template <class Config, uint16_t t_port>
class TCPServerDrv {
protected:
	static WiFiServer	server;
	static WiFiClient	tcp_client;
	static struct Data {
		std::vector<uint8_t>	rx_buffer;
		uint16_t				port;
	} data;
public:
	static void Init(uint16_t p=t_port);
	static uint8_t RXGet();
	static void RXFlush() { }
	static bool RXAvailable();
	static void setPort(uint16_t p) { data.port = p; }
};

//---------------------------------------------------------------------
// Interface Client
//---------------------------------------------------------------------
template <class Config>
class TCPClientDrv {
protected:
//	static WiFiClient    tcp_client;
	static struct Data {
		std::vector<uint8_t>	tx_buffer;
		uint16_t				port;
		uint8_t					ip[4];
		uint16_t				failover_port;
		uint8_t					failover_ip[4];
	} data;
public:
	static void Init() { data.tx_buffer.clear(); }
	static void TXPut(uint8_t byte) { data.tx_buffer.push_back(byte); }
	static bool TXAvailable() { return data.tx_buffer.empty(); }
	static bool TXFlush() {
		WiFiClient tcp_client;

		bool result = tcp_client.connect(IPAddress(data.ip), data.port);

		if( !result ) {
			Serial.printf("$tcp failed connect to %d.%d.%d.%d:%d\n", (int)data.ip[0], (int)data.ip[1], (int)data.ip[2], (int)data.ip[3], data.port);
			result = tcp_client.connect(IPAddress(data.failover_ip), data.failover_port);
			if( result ) Serial.printf("$tcp failover to %d.%d.%d.%d:%d\n", (int)data.failover_ip[0], (int)data.failover_ip[1], (int)data.failover_ip[2], (int)data.failover_ip[3], data.failover_port);
			else Serial.printf("$tcp failed connect to failover %d.%d.%d.%d:%d\n", (int)data.failover_ip[0], (int)data.failover_ip[1], (int)data.failover_ip[2], (int)data.failover_ip[3], data.failover_port);
		}
		if( result ) {
			tcp_client.write((char*)data.tx_buffer.data(), data.tx_buffer.size());
			tcp_client.println();
			tcp_client.flush();
		}
		data.tx_buffer.clear();
		return result;
	}
	static bool setIPPort(const uint8_t* ip, uint16_t port) {
		memcpy(data.ip, ip, sizeof(data.ip));
		data.port = port;
	}
	static bool setFailoverIPPort(const uint8_t* ip, uint16_t port) {
		memcpy(data.failover_ip, ip, sizeof(data.failover_ip));
		data.failover_port = port;
	}
};
//WiFiClient TCPClientDrv::tcp_client;

//---------------------------------------------------------------------
// Interface Server + Client
//---------------------------------------------------------------------
template <class Config, uint16_t t_port=0>
class TCPDrv : public TCPServerDrv<Config, t_port>, public TCPClientDrv<Config> {
public:
	static void Init(uint16_t p=t_port) {
		TCPServerDrv<Config, t_port>::Init(p);
		TCPClientDrv<Config>::Init();
	}
};


//---------------------------------------------------------------------
// Implementation
//---------------------------------------------------------------------
template <class Config, uint16_t t_port>
WiFiServer TCPServerDrv<Config, t_port>::server(t_port);

template <class Config, uint16_t t_port>
WiFiClient TCPServerDrv<Config, t_port>::tcp_client;

template <class Config, uint16_t t_port>
typename TCPServerDrv<Config, t_port>::Data TCPServerDrv<Config, t_port>::data;

template <class Config>
typename TCPClientDrv<Config>::Data TCPClientDrv<Config>::data;

template <class Config, uint16_t t_port>
void TCPServerDrv<Config, t_port>::Init(uint16_t port) {

	if( !data.port && port ) data.port = port;
	if( data.port ) {
		Config::Logger::Debug() << "tcp server begin port=" << data.port << "";
		server.begin(data.port);
	}

}

template <class Config, uint16_t t_port>
uint8_t TCPServerDrv<Config, t_port>::RXGet() {

	if( data.rx_buffer.empty() ) return 0;

	uint8_t b = data.rx_buffer.front();
	data.rx_buffer.erase(data.rx_buffer.begin());
	return b;
}

template <class Config, uint16_t t_port>
bool TCPServerDrv<Config, t_port>::RXAvailable() {

	if( !tcp_client ) {
		tcp_client = server.available();
		if( tcp_client ) {
			Config::Logger::Debug() << "tcp input\n";
			data.rx_buffer.clear();
		}
	}

	if( tcp_client && tcp_client.available() ) {

		while( tcp_client.available() ) {
			data.rx_buffer.push_back(tcp_client.read());
		}
		tcp_client.println("$OK");
		tcp_client.flush();
		tcp_client.stop();

//		Serial.print("$json: '");
//		for(int i=0; i<data.rx_buffer.size(); i++) Serial.print((char)data.rx_buffer[i]);
//		Serial.println("'");

		return !data.rx_buffer.empty();
	}

	if( !data.rx_buffer.empty() ) return true;

	return false;

}

}}}

#endif
