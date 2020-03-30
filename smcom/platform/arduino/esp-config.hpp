#ifndef SMCOM_PLATFORM_ARDUINO_ESP_CONFIG_HPP
#define SMCOM_PLATFORM_ARDUINO_ESP_CONFIG_HPP

#include <vector>

#include <ESP8266WebServer.h>

ESP8266WebServer  http_server(80);

namespace Smcom {
namespace Arduino {
namespace EspConfig {

	//---------------------------------------
	// structure to define user pages
	//---------------------------------------
	struct Page {
		std::string		name;
		uint16_t		(*html)(char*);
	};
	//---------------------------------------
	// structure to define user parameters
	//---------------------------------------
	struct ParamUInt16 {
		std::string		name;
		uint16_t&		param;
	};
	struct ParamUInt32 {
		std::string		name;
		uint32_t&		param;
	};
	struct ParamChar {
		std::string		name;
		char*			param;
		uint8_t			size;
	};
	struct ParamIP {
		std::string		name;
		uint8_t*		param;
	};
	struct PagesProto {
		// --- user pages
		static const std::vector<Page> 			pages;
		static const std::vector<ParamUInt16>	params_uint16;
		static const std::vector<ParamUInt32>	params_uint32;
		static const std::vector<ParamIP>		params_ip;
		static const std::vector<ParamChar>		params_char;
		// --- data call backs
		static void Read() {}
		static void Write() {}
		// --- timers call backs
		static void Update1Sec(void) {}
		static void Update1Min(void) {}
	};

	struct BasicConfig {
		const uint16_t	BufferSize = 4096;
	};

//---------------------------------------
// Main class
//---------------------------------------
template <class Config, class Pages>
class Server {
protected:
	// --- constant data
	static const std::vector<ParamUInt16>	params_uint16;
	static const std::vector<ParamUInt32>	params_uint32;
	static const std::vector<ParamChar>		params_char;
	static const std::vector<ParamIP>		params_ip;
	// --- data
	static struct Data {
		char    	buffer[Config::BufferSize];
		bool		changed;
	} data;
	// --- methods
	static void web_uint16(const std::vector<ParamUInt16>& params, uint16_t argsn) {
		for(const auto &param : params) {
			if( http_server.argName(argsn) != param.name.c_str() ) continue;
			param.param = http_server.arg(argsn).toInt();
			data.changed = true;
			yield();
		}
	}
	static void web_uint32(const std::vector<ParamUInt32>& params, uint16_t argsn) {
		for(const auto &param : params) {
			if( http_server.argName(argsn) != param.name.c_str() ) continue;
			param.param = http_server.arg(argsn).toInt();
			data.changed = true;
			yield();
		}
	}
	static void web_ip(const std::vector<ParamIP>& params, uint16_t argsn) {
		for(const auto &param : params) {
			if( http_server.argName(argsn) != param.name.c_str() ) continue;

			char		str[16];
			uint8_t		ip[4];

			http_server.arg(argsn).toCharArray(str, sizeof(str));
			char* ptr = strtok(str, ".");
			for(int i = 0; ptr != NULL && i < 4; i++, ptr = strtok(NULL, ".")) {
				ip[i] = atoi(ptr);
				if( i == 3 ) {
					for(int j = 0; j < 4; j++) param.param[j] = ip[j];
					data.changed = true;
				}
			}
			yield();
		}
	}
	static void web_char(const std::vector<ParamChar>& params, uint16_t argsn) {
		for(const auto &param : params) {
			if( http_server.argName(argsn) != param.name.c_str() ) continue;
			http_server.arg(argsn).toCharArray(param.param, param.size);
			data.changed = true;
			yield();
		}
	}
	static void handleNotFound() {

		int i = http_server.uri().substring(1).toInt();

		if( i >= 0 && i < Pages::pages.size() ) {

			Pages::Read();

			// --- valid page
			if(http_server.method() == HTTP_POST) {

				data.changed = false;

				for (int argsn = 0; argsn < http_server.args(); argsn++ ) {
					web_uint16(Pages::params_uint16, argsn);
					web_uint32(Pages::params_uint32, argsn);
					web_ip(Pages::params_ip, argsn);
					web_char(Pages::params_char, argsn);
				}

				if( data.changed ) Pages::Write();

			}
			yield();

			int l = print_menu(data.buffer);
			l += Pages::pages[i].html(data.buffer + l);
			print_bottom(data.buffer + l);
			http_server.send(200, "text/html", data.buffer);

		} else {
			http_server.send(404, "text/html", "Not found");
		}

	}
	static int print_menu(char* buffer) {
		uint16_t len = sprintf_P(buffer, PSTR("<table border='1'><tr><td><a href='/'>Main</a></td>"));

		int i = 0; for(auto page : Pages::pages) len += sprintf_P(buffer + len, PSTR("<td><a href='/%d'>%s</a></td>"), i++, page.name.c_str());

		yield();

		len +=  sprintf_P(buffer + len, PSTR("</tr></table><form method='post'>"));

		if( data.changed ) len += sprintf_P(buffer + len, PSTR("<font size='-1' color='red'>changed</font>"));
		data.changed = false;

		return len;
	}
	static int print_bottom(char* buffer) {
		return sprintf_P(buffer,
				PSTR("<br><br><table><tr><td><input type='submit' value='Save'></td></form>"\
						"<td><button type='button' onclick=\"location.href='/res'\">Restart</button></td></tr></table>"\
						"<hr><font size='-2'>chipID=0x%X MAC=%s WiFi %s<br>"\
						"hwid=0x%04X ver=%d.%03d"),
						ESP.getChipId(),
						WiFi.macAddress().c_str(),
						connected(WiFi.status() == WL_CONNECTED),
						Pages::HWID, Pages::VersionMajor, Pages::VersionMinor
		);
	}
	static int print_changed(char* buffer, boolean changed) {
		if(changed) return sprintf_P(buffer, PSTR("<font size='-1' color='red'>changed</font>"));
		return 0;
	}
	static uint16_t html_restart(char* buffer) {
		return sprintf_P(buffer,
				PSTR("<head><meta http-equiv='refresh' content='15; url=/'></head><body>"\
						"<div id=\"counter\"></div>"\
						"<script>"\
						"function CountDown() {"\
						"document.getElementById(\"counter\").innerHTML = \"Restarting \" + second;"\
						"if( second ) second--;}"\
						"var second = 10;"\
						"var cancel = setInterval(CountDown, 1000);"\
						"</script>")
		);
	}
	//----------------------------------------------
	// common functions
	//----------------------------------------------
	static void handle_page(uint16_t (*html)(char*)) {

		data.changed = false;

		if( http_server.method() == HTTP_POST) {

			for (int i = 0; i < http_server.args(); i++) {

				web_uint16(params_uint16, i);
				web_uint32(params_uint32, i);
				web_ip(params_ip, i);
				web_char(params_char, i);

			}
			yield();

		//	if( data.changed ) WiFiNode::persist.Write();
		}

		int l = print_menu(data.buffer);
		l += html(data.buffer + l);
		print_bottom(data.buffer + l);

		http_server.send(200, "text/html", data.buffer);
	}
	//----------------------------------------------
	// page handlers
	//----------------------------------------------
	static void handle_restart() { handle_page(html_restart); }
public:
	static void Init() {
		http_server.on("/res", handle_restart);
		http_server.onNotFound(handleNotFound);
		http_server.begin();
	}
	static void Loop() { http_server.handleClient(); }
	static const char* connected(boolean f) { return (f)?"<font color='green'>connected</font>":"<font color='red'>disconnected</font>"; }
	static const char* selected(boolean f) { return (f)?" selected":""; }
	static const char* checked(boolean f) { return (f)?" checked":""; }
};

}}}

#endif
