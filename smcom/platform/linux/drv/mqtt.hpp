#ifndef _STAVRP_LINUX_DRV_MQTT_H
#define _STAVRP_LINUX_DRV_MQTT_H

#include <stdio.h>
#include <unistd.h>
#include <mosquittopp.h>

#include <string>
#include <vector>
#include <cstring>

#include "../../../stavrp.hpp"

namespace STAVRP {
namespace Linux {

//---------------------------------------------------------------------
// Interface
//---------------------------------------------------------------------
class MqttEntry {
public:
	std::string	key;
	std::string	payload;
	MqttEntry() {}
	MqttEntry(std::string key_p, std::string payload_p) : key(key_p), payload(payload_p) {}
};

template <class Config>
class MosquittoDrv : protected mosqpp::mosquittopp {
protected:
	struct Data {
		bool					connected;
		uint16_t				port;
		std::string				server;
		std::vector<MqttEntry>	rx_buffer;
		std::vector<MqttEntry>	tx_buffer;
	} data;
	virtual void on_connect(int rc) {
		data.connected = true;
		Config::Logger::Debug() << "mqtt-connected\n";
	}
	virtual void on_disconnect(int rc) {
		data.connected = false;
		Connect();
		Config::Logger::Debug() << "mqtt-disconnected\n";
	}
	virtual void on_publish(int mid) {}
	virtual void on_message(const struct mosquitto_message *message) {

		MqttEntry entry(std::string(message->topic), std::string(static_cast<char*>(message->payload), message->payloadlen));

		data.rx_buffer.push_back(entry);

		Config::Logger::Debug() << "mqtt-in message\n";
	}
	virtual void on_subscribe(int mid, int qos_count, const int *granted_qos) {
		Config::Logger::Debug() << "mqtt-subscribed\n";
	}
	virtual void on_unsubscribe(int mid) {}
	virtual void on_log(int level, const char *str) {
		Config::Logger::Debug() << "mqtt-log [" << str << "]";
	}
	virtual void on_error() {}
	void Publish(const MqttEntry& entry) {
		publish(NULL, entry.key.c_str(), entry.payload.size(), entry.payload.c_str(), 0);
	}
	void Loop() {
		loop();
		if( data.connected ) {
			while( data.tx_buffer.size() > 0 ) {
				Publish(data.tx_buffer.front());
				data.tx_buffer.erase(data.tx_buffer.begin());
			}
		}
	}
	void Connect() {
		if( data.port && data.connected == false ) {
			Config::Logger::Debug() << "mqtt-connecting " << data.server.c_str() << ":" << data.port << "";
			connect(data.server.c_str(), data.port, 60);
		}
	}
public:
	MosquittoDrv() : mosqpp::mosquittopp(NULL, true) {
		mosqpp::lib_init();
		data.connected = false;
		data.port = 0;
	}
	void Init() {
		char client_id[32];
		sprintf(client_id, "swi_appserver_%d", getpid());
		reinitialise(client_id, true);
		Connect();
	}
	void SetIPPort(const std::string& server, uint16_t port) {
		data.server = server;
		data.port = port;
	}
	const MqttEntry& RXGet() const {
		return data.rx_buffer.front();
	}
	void RXFlush() {
		data.rx_buffer.erase(data.rx_buffer.begin());
	}
	bool RXAvailable() {
		Loop();
		return data.rx_buffer.size() > 0;
	}
	void TXPut(const MqttEntry& entry) {
		if( data.connected ) Publish(entry);
		else data.tx_buffer.push_back(entry);
	}
	bool TXAvailable() {
		Loop();
		return true;
	}
	void TXFlush() const {}
	bool IsConnected() const { return data.connected; }
	void Subscribe(std::string key) {
		subscribe(NULL, key.c_str(), 0);
	}
};


}}

#endif
