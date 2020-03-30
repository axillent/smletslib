/*
 *
 */

#ifndef _SMLETS_NODE_ARDUINO_ESP_H
#define _SMLETS_NODE_ARDUINO_ESP_H

#include <ArduinoOTA.h>

#include "smlets/smlets.hpp"
#include "smlets/defaults.hpp"
#include "smlets/node/appl/appl-basic.hpp"

#include "smcom/platform/arduino/drv/eeprom-esp8266.hpp"
#include "smcom/common/persistence.hpp"
#include "smcom/platform/arduino/esp-manager.hpp"
#include "smcom/platform/arduino/esp-config.hpp"

#include "node-arduino-esp-pages.hpp"

namespace Smlets {
namespace Node {
namespace Arduino {

class ConfigDefaults {
public:
	class Defaults : public Smlets::Defaults {
	public:
		// --- ESP Node configuration
		struct EspNode {
			static const int 			PersistID	= 5000;
			static constexpr char*		SID			= "";
			static constexpr char*		Secret		= "";
			static const int 			LedPin		= 0;
			static const uint32_t 		BaudRate	= 115200;
			static constexpr char*		MQTTServer = "192.168.1.1";
		};
	};
	typedef SmCom::Common::PersistManager<SmCom::Arduino::ESP82xx::EEPROMDrv>	PersistManager;
};

template <class Config=ConfigDefaults, template <class> class ApplTemplate = Smlets::Node::Appl::Basic>
class Esp {
protected:
	// --- data
	static struct Data {
		Smcom::Arduino::EspManagerState	wifi_state;
	} data;
	// --- persistence data
	struct PersistDataStruct {
		char          	wifi_ssid[24];
		char          	wifi_secret[24];
		uint32_t      	baud_rate;
		uint16_t      	swi_segment;
		uint16_t       	swi_segments;
		uint16_t       	swi_id;
		char          	swi_name[17];
		char			mqtt_server[24];
	};
	typedef SmCom::Common::PersistType<typename Config::PersistManager, PersistDataStruct, Config::Defaults::EspNode::PersistID> PersistData;
	static PersistData	persist;
	// --- helper classes
	typedef Smcom::Arduino::EspManager<>	WiFiManager;
	typedef Smcom::Arduino::EspConfig::Server< Smcom::Arduino::EspConfig::BasicConfig, Smlets::Node::Arduino::EspPages>	WebConfig;
public:
	static void Init(void) {
		//-----------------------------------------------------------------------------------------
		// --- device configuration
		Config::PersistManager::Init();
		WiFiManager::Init();
		//WebConfig::Init();

		if (!persist.Read()) {
			// --- apply defaults
			strncpy(persist->wifi_ssid, Config::Defaults::EspNode::SID, sizeof(persist->wifi_ssid));
			strncpy(persist->wifi_secret, Config::Defaults::EspNode::Secret, sizeof(persist->wifi_secret));

			persist->baud_rate = Config::Defaults::EspNode::BaudRate;
			persist->swi_segment = 1;
			persist->swi_segments = 1;
			persist->swi_id = 1;

			sprintf(persist->swi_name, "swi_%X", ESP.getChipId());
		}

		WiFiManager::SetSSID(persist->wifi_ssid, persist->wifi_secret);

//		Serial.begin(static_cast<uint32_t>(persist->baud_rate));
		Serial.begin(115200);
		Serial.println();
		Serial.println("#Node::Esp start");

		//-----------------------------------------------------------------------------------------
		// --- OTA
		ArduinoOTA.setHostname(persist->swi_name);
		ArduinoOTA.onStart([]() {
			Serial.println("#ota start");
			});
		ArduinoOTA.onEnd([]() {
			Serial.println("#ota end");
			});

		ArduinoOTA.begin();

	}
	static void Loop(void) {

		Smcom::Arduino::EspManagerState new_wifi_state = WiFiManager::Loop();

		if (new_wifi_state != data.wifi_state) {
			switch (new_wifi_state) {
			case Smcom::Arduino::EspManagerState::off:
				Serial.println("#EspManager -> off");
				break;
			case Smcom::Arduino::EspManagerState::AP:
				Serial.println("#EspManager -> AP");
				break;
			case Smcom::Arduino::EspManagerState::APSTA:
				Serial.println("#EspManager -> APSTA");
				break;
			case Smcom::Arduino::EspManagerState::beforeAP:
				Serial.println("#EspManager -> beforeAP");
				break;
			case Smcom::Arduino::EspManagerState::beforeSTA:
				Serial.println("#EspManager -> beforeSTA");
				break;
			case Smcom::Arduino::EspManagerState::failToSTA:
				Serial.println("#EspManager -> failToSTA");
				break;
			case Smcom::Arduino::EspManagerState::STA:
				Serial.println("#EspManager -> STA");
				break;
			case Smcom::Arduino::EspManagerState::toAP:
				Serial.println("#EspManager -> toAP");
				break;
			case Smcom::Arduino::EspManagerState::toAPSTA:
				Serial.println("#EspManager -> toAPSTA");
				break;
			case Smcom::Arduino::EspManagerState::toSTA:
				Serial.println("#EspManager -> toSTA");
				break;
			}
			data.wifi_state = new_wifi_state;
		}

	}
};

template <class Config, template <class> class ApplTemplate>
typename Esp<Config, ApplTemplate>::Data Esp<Config, ApplTemplate>::data;

}}}

#endif
