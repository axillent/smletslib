#ifndef _STAVRP_ARDUINO_DRV_LOGGER_H
#define _STAVRP_ARDUINO_DRV_LOGGER_H

#include <string>

#include <IPAddress.h>

#include "../../../stavrp.hpp"
#include <smartletsp/msg/msg_basic.hpp>
#include <smartletsp/msg/msg_tostring.hpp>

namespace STAVRP {
namespace Arduino {

class LogIP {
protected:
	uint8_t	ip[4];
public:
	LogIP(const uint8_t* i) {
		ip[0] = i[0];
		ip[1] = i[1];
		ip[2] = i[2];
		ip[3] = i[3];
	}
	LogIP(const IPAddress& i) {
		ip[0] = i[0];
		ip[1] = i[1];
		ip[2] = i[2];
		ip[3] = i[3];
	}
	uint8_t operator [] (uint8_t index) const {
		return ip[index];
	}
};

class LoggerDrv {
protected:
	enum class Priority {
		Error,
		Debug,
		Info
	} priority;
	std::string	line;
	void Print() {
		switch( priority ) {
		case Priority::Error:
			Serial.print("$error: ");
			break;
		case Priority::Debug:
			Serial.print("$debug: ");
			break;
		case Priority::Info:
			Serial.print("$info: ");
			break;
		}
		Serial.println(line.c_str());
		line = "";
	}
public:
	void Init() {
		line = "";
	}
	void setError() {
		if( line.length() ) Print();
		priority = Priority::Error;
	}
	void setDebug() {
		if( line.length() ) Print();
		priority = Priority::Debug;
	}
	void setInfo() {
		if( line.length() ) Print();
		priority = Priority::Info;
	}
	~LoggerDrv() {
	}
	LoggerDrv& operator << (const char* str) {

		uint8_t	len = strlen(str);
		bool end = false;

		if( len ) {
			if( str[len - 1] == '\n' ) {
				line.append(str, len - 1);
				end = true;
			} else line += str;
		} else end = true;

		if( end ) Print();

		return *this;
	}
	LoggerDrv& operator << (const std::string& str) {
		return *this << str.c_str();
	}
	LoggerDrv& operator << (const Smartlets::Message::MessageBase& msg) {
		line += Smartlets::Message::ToString::Get(msg);
		return *this;
	}
	LoggerDrv& operator << (int n) {
		char buffer[16];
		line += itoa(n, buffer, 10);
		return *this;
	}
	LoggerDrv& operator << (const LogIP& ip) {
		char buffer[16];
		line += itoa(ip[0], buffer, 10);
		line += ".";
		line += itoa(ip[1], buffer, 10);
		line += ".";
		line += itoa(ip[2], buffer, 10);
		line += ".";
		line += itoa(ip[3], buffer, 10);
		return *this;
	}
};

class Logger {
protected:
	static LoggerDrv logger;
public:
	static void Init() {
		logger.Init();
	}
	static LoggerDrv& Info() {
		logger.setInfo();
		return logger;
	}
	static LoggerDrv& Debug() {
		logger.setDebug();
		return logger;
	}
	static LoggerDrv& Error() {
		logger.setError();
		return logger;
	}
};
LoggerDrv Logger::logger;

}}

#endif
