#ifndef _SMLETS_DEFAULTS_H
#define _SMLETS_DEFAULTS_H

#include "smlets.hpp"
//#include "msg/msg_basic.hpp"

namespace Smlets {

	/*class DefaultsSmall {
	public:
		typedef Smartlets::Message::Message<20>	Message;
			class W3P {
			public:
				enum {
					RXBufferSize = 22,
					TXBufferSize = 22,
					TimerRXARR = 32000,
					TimerTXARR = 4000,
					MsgBufferSize = 2
				};
			};
			class UART {
			public:
				enum {
					BaudRate = 115200,
					RXBufferSize = 64,
					TXBufferSize = 64,
					MsgBufferSize = 2
				};
			};
		};
		*/

	class Defaults {
	public:
//		typedef Smartlets::Message::Message<30>	Message;
		class W3P {
		public:
			enum {
				RXBufferSize = 32,
				TXBufferSize = 32,
				TimerRXARR = 32000,
				TimerTXARR = 4000,
				MsgBufferSize = 8
			};
		};
			//	class UART {
			//	public:
			//		static const uint32_t BaudRate = 115200;
			//		enum {
			//			RXBufferSize = 64,
			//			TXBufferSize = 8,
			//            MsgBufferSize = 8
			//		};
			//	};
			//	class ESP {
			//	public:
			//		static const uint32_t BaudRate = 115200;
			//		static const uint16_t TCPServerPort = 9050;
			//		enum {
			//			RXBufferSize = 64,
			//			TXBufferSize = 64,
			//            MsgBufferSize = 8
			//		};
			//	};
/*
#ifdef STAVR_PLATFORM_ARDUINO_STM32
// --- W5500Node configuration
			struct STM32 {
				// --- common
				static const uint32_t	BaudRate = 115200;
				static const uint8_t	MsgBufferSize = 16;
				static const uint16_t	tcp_port = 9050;
				static const uint16_t	fo_port = 9050;
				static const uint8_t	swi_id = 1;
				// --- bellow have to be defined in sketch
		//		static constexpr uint8_t  tcp_server[4] =  { 0, 0, 0, 0 };
		//		static constexpr uint8_t  fo_server[4]  =  { 0, 0, 0, 0 };
		//		static constexpr uint8_t  mac[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		//		static const int swi_segment 	= 0;
		//		static const int swi_segments	= 0;
			};
#endif


#ifdef STAVR_PLATFORM_LINUX
			struct SWIAppServer {

			};
#endif
*/

		// ---
		//	struct ApplThermostat {
		//		static const int 		PersistID 		= 1000;
		//	};

		struct ApplHeatingServo {
			static const uint16_t PumpID = 1;
			static const uint8_t PumpChannel = 0;
		};

	};

}

#endif
