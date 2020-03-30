/*
 *
 */

#ifndef _SMLETS_NODE_ARDUINO_ESP_PAGES_H
#define _SMLETS_NODE_ARDUINO_ESP_PAGES_H

#include <vector>

#include "smcom/platform/arduino/esp-config.hpp"

namespace Smlets {
namespace Node {
namespace Arduino {

struct EspPages {
	// --- static const atributes
//	static const uint16_t HWID = 0x0000;
//	static const uint16_t VersionMajor = 0;
//	static const uint16_t VersionMinor = 1;
	// --- user pages
	static const std::vector<Smcom::Arduino::EspConfig::Page> 			pages;
	static const std::vector<Smcom::Arduino::EspConfig::ParamUInt16>	params_uint16;
	static const std::vector<Smcom::Arduino::EspConfig::ParamUInt32>	params_uint32;
	static const std::vector<Smcom::Arduino::EspConfig::ParamIP>		params_ip;
	static const std::vector<Smcom::Arduino::EspConfig::ParamChar>		params_char;
	// --- data call backs
	static void Read() {}
	static void Write() {}
	// --- timers call backs
	static void Update1Sec(void) {}
	static void Update1Min(void) {}
};
//const std::vector<Page> BasicPages::pages = {};
//const std::vector<ParamUInt16> BasicPages::params_uint16 = {};
//const std::vector<ParamUInt32> BasicPages::params_uint32 = {};
//const std::vector<ParamIP> BasicPages::params_ip = {};
//const std::vector<ParamChar> BasicPages::params_char = {};

}}}

#endif
