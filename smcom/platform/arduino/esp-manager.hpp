#ifndef SMCOM_PLATFORM_ARDUINO_ESP_MANAGER_HPP
#define SMCOM_PLATFORM_ARDUINO_ESP_MANAGER_HPP

// -------------------------------------------------
// WIFI
// -------------------------------------------------
#define WIFI_FAILSAFE_MASK          255,255,255,0

// -------------------------------------------------
#include <string>

#include <ESP8266WiFi.h>

namespace Smcom {
namespace Arduino {

class EspManagerConfig {
public:
	static constexpr uint8_t 	FailSafeIP[4] = { 192, 168, 100, 1 };
	static constexpr char*		FailSafeSecret = "1234554321";
	static constexpr char*		SSIDName = "";
	static constexpr char*		SSIDPass = "";
};

enum class EspManagerState {
	off=0,    // start state
	AP,       // access point only (failsafe)
	toAP,
	beforeAP,
	APSTA,    // both AP and regular connections
	toAPSTA,
	STA,      // normal state - only regular connection
	toSTA,    // STA requested but not yet activated
	failToSTA,// failed activate STA
	beforeSTA // STA activated, transition state
};

template <class Config=EspManagerConfig>
class EspManager {
protected:
	// --- run time data
	static struct Data {
		EspManagerState  	state;
		int               	sta_fail_count;
		std::string			ssid_name;
		std::string			ssid_pass;
	} data;
	// ---
	static boolean startAP() {
		IPAddress   failsafe_ip(Config::FailSafeIP[0], Config::FailSafeIP[1], Config::FailSafeIP[2], Config::FailSafeIP[3]);
		IPAddress   failsafe_mask(WIFI_FAILSAFE_MASK);

		WiFi.softAPConfig(failsafe_ip, failsafe_ip, failsafe_mask);
		WiFi.softAP("eso-node", Config::FailSafeSecret);

		if(WiFi.status() != WL_CONNECTED)  {
			return true;
		}
		return false;
	}
public:
	static void Init() {
		data.ssid_name = Config::SSIDName;
		data.ssid_pass = Config::SSIDPass;

		Restart();
	}
	static void SetSSID(std::string name, std::string pass) {
		data.ssid_name = name;
		data.ssid_pass = pass;
	}
	static void Restart() {
		WiFi.mode(WIFI_OFF);
		data.state = EspManagerState::off;
	}
	static EspManagerState Loop() {

		switch(data.state) {
		case EspManagerState::off:
			if( data.ssid_name.length() >= 3 && data.ssid_pass.length() >= 8 ) {
				data.sta_fail_count = 0;
				data.state = EspManagerState::toSTA;
			} else 
			data.state = EspManagerState::toAP;
			break;
		case EspManagerState::toAP:
			WiFi.mode(WIFI_AP);
			if(startAP()) {
				data.state = EspManagerState::beforeAP;
			} else {
				delay(1000);
			}
			break;
		case EspManagerState::beforeAP:
			data.state = EspManagerState::AP;
			break;
		case EspManagerState::AP:
			break;
		case EspManagerState::toAPSTA:
			//WiFi.mode(WIFI_AP_STA);
			WiFi.mode(WIFI_AP);
			if(startAP()) {
				data.state = EspManagerState::APSTA;
			} else {
				delay(1000);
			}
			break;
		case EspManagerState::APSTA:
			//WiFi.begin(config->get().wifi_ssid, config->get().wifi_secret);
			break;
		case EspManagerState::toSTA:
			WiFi.mode(WIFI_STA);
			WiFi.begin(data.ssid_name.c_str(), data.ssid_pass.c_str());
			if(WiFi.waitForConnectResult() != WL_CONNECTED) {
				data.state = EspManagerState::failToSTA;
			} else {
				data.state = EspManagerState::beforeSTA;
				delay(1000);
			}
			//          snprintf(var.base_topic, sizeof(var.base_topic), "/swi/%X/%d/", config.swi_segment, config.swi_id);
			break;
		case EspManagerState::failToSTA:
			if( ++data.sta_fail_count > 3 ) data.state = EspManagerState::toAPSTA;
			else data.state = EspManagerState::toSTA;
			break;
		case EspManagerState::beforeSTA:
			data.state = EspManagerState::STA;
		case EspManagerState::STA:
			if(WiFi.status() != WL_CONNECTED) {
				WiFi.mode(WIFI_OFF);
				delay(1000);
				data.state = EspManagerState::toSTA;
			}
			break;
		}
		return data.state;
	}
};

template <class Config> typename EspManager<Config>::Data EspManager<Config>::data;

}}

#endif
