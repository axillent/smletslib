#ifndef _STAVRP_LINUX_LOG4CPP_HPP
#define _STAVRP_LINUX_LOG4CPP_HPP
-
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

class Logger {
	static struct Data {
		log4cpp::Category*	root;
		log4cpp::Category*	main;
	} data;
public:
	static void Init();
	static void Shutdown();
	static log4cpp::Category& root() { return *data.root; }
	static log4cpp::Category& main() { return *data.main; }
};

#endif

//---------------------------------------------------------------------------
#ifndef _STAVRP_LINUX_LOG4CPP_CPP
#define _STAVRP_LINUX_LOG4CPP_CPP

Logger::Data Logger::data;

void Logger::Init() {

	std::string initFileName = "log4cpp.properties";
	log4cpp::PropertyConfigurator::configure(initFileName);

	data.root = &log4cpp::Category::getRoot();
	data.main = &log4cpp::Category::getInstance(std::string("sub1"));

}

void Logger::Shutdown() {

	log4cpp::Category::shutdown();

}

#endif

