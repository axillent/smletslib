#ifndef _STAVRP_LINUX_DRV_FSTREAM_H
#define _STAVRP_LINUX_DRV_FSTREAM_H

#include <string>
#include <fstream>
#include <exception>

#include "../../../stavrp.hpp"

namespace STAVRP {
namespace Linux {

//---------------------------------------------------------------------
// Interface Server
//---------------------------------------------------------------------
template <class Config, int id=0>
class FStreamDrv {
protected:
	static struct Data {
		std::string		file_name;
		std::fstream	file_stream;
	} data;
public:
	static void setFileName(std::string name) {
		data.file_name = name;
	}
	static void Init() {

		if( !data.file_name.length() ) {
			Config::Logger::Debug() << "FStreamDrv file name not set\n";
			return;
		}

		try {
			data.file_stream.open(data.file_name, std::fstream::in | std::fstream::out);
		} catch(std::exception e) {
			Config::Logger::Debug() << "FStreamDrv failed to open '" << data.file_name << "'\n";
			return;
		}
		Config::Logger::Debug() << "FStreamDrv '" << data.file_name << "' opened\n";
	}
	static uint8_t RXGet() {
		int c = data.file_stream.get();
		if( c == EOF ) return 0;
		return c;
	}
	static void RXFlush() {}
	static bool RXAvailable() {
		return data.file_stream.rdbuf()->in_avail();
	}
	static void TXPut(uint8_t byte) {
		data.file_stream.put(byte);
	}
	static bool TXAvailable() {
		return true;
	}
	static bool TXFlush() {
		data.file_stream.flush();
		return true;
	}
};
template <class Config, int id> typename FStreamDrv<Config, id>::Data FStreamDrv<Config, id>::data;


}}

#endif
