#ifndef _STAVRP_ENCODING_MANCHESTER_DRIVER_H
#define _STAVRP_ENCODING_MANCHESTER_DRIVER_H

#include "common.hpp"

namespace STAVRP {
  namespace SoftDrv {
	namespace Manchester {

	  template <class InputPin, class OutputPin, class PinChangeDrviver, class TimerDriver, int rx_buffer_size, int tx_buffer_size>
		class Driver {
		protected:
		  //---
		  static const int wait_normal = 4;
		  //---
		  static struct Data {
			RXBuffer<uint8_t, rx_buffer_size>	rx_buffer;
			TXBuffer<uint8_t, tx_buffer_size>	tx_buffer;
			volatile	uint8_t					wait_count_rx, wait_count_tx;
			uint16_t							sync_min_count, sync_max_count;
		  } data;
		  //--------------------------------------------------------------
		  // Configure state
		  //--------------------------------------------------------------
		  //--- Normal state
		  static void State_Normal_Common() {
			data.wait_count_rx	= wait_normal;
			data.wait_count_tx	= wait_normal * 2;
			OutputPin::Set(true);
			TimerDriver::ResetScale();
			TimerDriver::SetHandler(TimerUpdate_Normal);
			PinChangeDrviver::SetHandler(PinChangeUpdate_Normal);
		  }
		  //--------------------------------------------------------------
		  // Normal state handlers
		  //--------------------------------------------------------------
		  static void TimerUpdate_Normal(void) {

			//--- check if line going to be free
			if( InputPin::Get() ) {
				if( data.wait_count_rx ) data.wait_count_rx--;
				if( data.wait_count_tx ) data.wait_count_tx--;
			}

			//--- check if TXBusy need to be activated
			if( !data.wait_count_rx && !data.wait_count_tx && data.tx_buffer.IsReady() ) {
			  data.tx_buffer.SetBusy();
			  TimerDriver::Scale();
			  TimerDriver::SetHandler(TimerUpdate_TXBusy);
			  PinChangeDrviver::SetHandler(PinChangeUpdate_TXBusy);
			}

		  }
		  static void PinChangeUpdate_Normal(void) {

			if( !InputPin::Get() ) {
				if( !data.wait_count_rx && data.rx_buffer.IsEmpty() ) {
				  //--- start bit received
				  data.rx_buffer.SetBusy();
				  data.wait_count_rx = wait_normal / 2;
				  TimerDriver::ResetCounter();
				  TimerDriver::SetHandler(TimerUpdate_RXStart);
				  PinChangeDrviver::SetHandler(PinChangeUpdate_RXStart);
				} else data.wait_count_rx = wait_normal;
			}

		  }
		  //--------------------------------------------------------------
		  // TXBusy state handlers
		  //--------------------------------------------------------------
		  static void TimerUpdate_TXBusy(void) {

			if( data.tx_buffer.AvailableGet() ) {
			  //--- transmitt mext bit
			  OutputPin::Set(data.tx_buffer.Get());
			} else {
			  //--- tx finished
			  data.tx_buffer.ResetAll();
			  State_Normal_Common();
			  data.wait_count_tx = wait_normal * 4; // after this TX wait longer before next TX
			}

		  }
		  static void PinChangeUpdate_TXBusy(void) {

			//--- check if we need to restart
			if( OutputPin::Get() != InputPin::Get() ) {
			  data.tx_buffer.ResetGet();
			  State_Normal_Common();
			}

		  }
		  //--------------------------------------------------------------
		  // RXStart state handlers
		  //--------------------------------------------------------------
		  static void TimerUpdate_RXStart(void) {
			if( !--(data.wait_count_rx) ) {
			  data.rx_buffer.ResetAll();
			  State_Normal_Common();
			}
		  }
		  static void PinChangeUpdate_RXStart(void) {
			//--- be prepared to receive
			uint16_t sync = TimerDriver::GetCounter();
			data.sync_min_count = data.sync_max_count = sync * 2;
			//--- 25% error
			sync /= 4;
			data.sync_min_count -= sync;
			data.sync_max_count += sync;
			TimerDriver::ResetCounter();
			//--- change handlers
			data.wait_count_rx = wait_normal / 2;
			TimerDriver::SetHandler(TimerUpdate_RXBusy);
			PinChangeDrviver::SetHandler(PinChangeUpdate_RXBusy);
		  }
		  //--------------------------------------------------------------
		  // RXBusy state handlers
		  //--------------------------------------------------------------
		  static void TimerUpdate_RXBusy(void) {
			if( !--(data.wait_count_rx) ) {
			  if( data.rx_buffer.IsConsistent() ) {
				//--- mark ready
				data.rx_buffer.SetReady();
			  } else {
				//--- reset
				data.rx_buffer.ResetAll();
			  }
			  State_Normal_Common();
			}
		  }
		  static void PinChangeUpdate_RXBusy(void) {
			//--- keep running
			data.wait_count_rx = wait_normal / 2;

			//--- check if  we neeed to read bit
			uint16_t count = TimerDriver::GetCounter();

			if( count >= data.sync_min_count && count <= data.sync_max_count ) {
			  // matched
			  data.rx_buffer.Put(InputPin::Get());
			  // resync
			  data.sync_min_count = data.sync_max_count = count;
			  //--- 25% error
			  count /= 4;
			  data.sync_min_count -= count;
			  data.sync_max_count += count;
			  TimerDriver::ResetCounter();
			}

		  }
		public:
		  //--------------------------------------------------------------
		  // Init
		  //--------------------------------------------------------------
		  static void Init() {
			//--- init GPIO
			OutputPin::Init();
			//--- init handlers
			PinChangeDrviver::Init();
			TimerDriver::Init();
			TimerDriver::Start();
			//--- normal state
			State_Normal_Common();
			//--- init GPIO
			InputPin::Init();
		  }
		  //--------------------------------------------------------------
		  // TXPut
		  //--------------------------------------------------------------
		  static void TXPut(uint8_t byte) {
			data.tx_buffer.Put(byte);
		  }
		  //--------------------------------------------------------------
		  // RXGet
		  //--------------------------------------------------------------
		  static uint8_t RXGet() {
			return data.rx_buffer.Get();
		  }
		  //--------------------------------------------------------------
		  // RXAvailable
		  //--------------------------------------------------------------
		  static bool RXAvailable() {
			return data.rx_buffer.AvailableGet();
		  }
		  //--------------------------------------------------------------
		  // TXAvailable
		  //--------------------------------------------------------------
		  static bool TXAvailable() {
			return data.tx_buffer.AvailablePut();
		  }
		  //--------------------------------------------------------------
		  // TXFlash
		  //--------------------------------------------------------------
		  static void TXFlash() {
			data.tx_buffer.SetReady();
		  }
		  //--------------------------------------------------------------
		  // RXFlash
		  //--------------------------------------------------------------
		  static void RXFlash() {
			if( data.rx_buffer.IsReady() ) data.rx_buffer.ResetAll();
		  }
		};

	  template <class InputPin, class OutputPin, class PinChangeDrviver, class TimerDriver, int rx_buffer_size, int tx_buffer_size>
		typename Driver<InputPin, OutputPin, PinChangeDrviver, TimerDriver, rx_buffer_size, tx_buffer_size>::Data
		  Driver<InputPin, OutputPin, PinChangeDrviver, TimerDriver, rx_buffer_size, tx_buffer_size>::data;
	}
  }
}

#endif
