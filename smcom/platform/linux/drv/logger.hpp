#ifndef _STAVRP_LINUX_DRV_LOGGER_H
#define _STAVRP_LINUX_DRV_LOGGER_H

#include <string>

#include "../../../stavrp.hpp"
#include <smartletsp/msg/msg_basic.hpp>
#include <smartletsp/msg/msg_tostring.hpp>

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

namespace STAVRP {
namespace Linux {

class LogIP {
protected:
	const uint8_t*	ip;
public:
	LogIP(const uint8_t* i) : ip(i) {}
	uint8_t operator [] (uint8_t index) const {
		return ip[index];
	}
};

class LoggerDrv {
protected:
	enum class Priority {
		Error,
		Debug
	} priority;
	log4cpp::Category*	root;
	std::string			line;
	void Print() {
		switch( priority ) {
		case Priority::Error:
			root->error(line);
			break;
		case Priority::Debug:
			root->debug(line);
			break;
		}
		line = "";
	}
public:
	void Init(std::string config) {
		line = "";
		log4cpp::PropertyConfigurator::configure(config);
		root = &log4cpp::Category::getRoot();

		log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("sub1"));
	}
	void setError() {
		if( line.size() ) Print();
		priority = Priority::Error;
	}
	void setDebug() {
		if( line.size() ) Print();
		priority = Priority::Debug;
	}
	~LoggerDrv() {
		log4cpp::Category::shutdown();
	}
	LoggerDrv& operator << (const char* str) {

		uint8_t	len = strlen(str);
		bool end = false;

		if( len ) {
			line += str;
			if( str[len - 1] == '\n' ) {
				line.erase(line.end());
				end = true;
			}
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
		line += std::to_string(n);
		return *this;
	}
	LoggerDrv& operator << (const LogIP& ip) {
		line += std::to_string(ip[0]);
		line += ".";
		line += std::to_string(ip[1]);
		line += ".";
		line += std::to_string(ip[2]);
		line += ".";
		line += std::to_string(ip[3]);
		return *this;
	}
};

class Logger {
protected:
	static LoggerDrv logger;
public:
	static void Init(std::string config = "log4cpp.properties") {
		logger.Init(config);
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
