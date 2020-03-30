#include "manchester.h"

namespace STAVRP {
  namespace Encoding {
	namespace Manchester {

	  //------------------------------------------------------------------------------
	  // RX side
	  //------------------------------------------------------------------------------
	  /**
	  \brief restart RX on busy line, internal function
	  */
	  template <class InputPinDriver, class InputPinChangeDriver, class TimerDriver>
		void Rx<InputPinDriver, InputPinChangeDriver, TimerDriver>::Restart(void) {

		  data.byte_index	= 0;
		  data.bit_index	= 0;
		  data.buffer[0]	= 0;
		  data.state		= State::Empty;
		  data.line_busy	= true;
		  SetWait();

		}

	  /**
	  \brief manchester RX general init
	  \param[in] data RX data structure
	  */
	  template <class InputPinDriver, class InputPinChangeDriver, class TimerDriver>
		void Rx<InputPinDriver, InputPinChangeDriver, TimerDriver>::Init(uint8_t* buffer, uint16_t buffer_size) {

		  // --- init byffer
		  data.buffer		= buffer;
		  data.buffer_size	= buffer_size;
		  // --- rx pin
		  InputPinDriver::Init();
		  InputPinChangeDriver::Init();
		  InputPinChangeDriver::SetHandler(PinChangeUpdate);
		  // --- timer
		  TimerDriver::Init();
		  TimerDriver::SetHandler(TimerUpdate);
		  TimerDriver::Start();
		  Restart();

		}

	  /**
	  manchester RX timer handler, shoud be called aproximetelly baudrate * 2
	  \param[in] data RX data structure
	  */
	  template <class InputPinDriver, class InputPinChangeDriver, class TimerDriver>
		void Rx<InputPinDriver, InputPinChangeDriver, TimerDriver>::TimerUpdate() {

		  if( data.line_busy ) {
			if( data.wait_count && InputPinDriver::Get() && !--(data.wait_count) ) data.line_busy = false;;
			return;
		  }

		  data.timer.Update();

		  switch(data.state) {
		  case State::BusyStartBit:
		  case State::Busy:
			// restart or finish receive
			if( data.wait_count && !--(data.wait_count) ) {
			  if( data.byte_index && !data.bit_index ) data.state = State::Ready;
			  else Restart();
			}
			break;
		  default:
			break;
		  }

		}

	  /// manchester RX pin change handler should be called on any RX pin change
	  template <class InputPinDriver, class InputPinChangeDriver, class TimerDriver>
		void Rx<InputPinDriver, InputPinChangeDriver, TimerDriver>::PinChangeUpdate(void) {

		  data.timer.RecordHardCnt();

		  uint8_t input = InputPinDriver::Get();

		  if( data.line_busy ) {
			if( !input ) SetWait();
			return;
		  }

		  switch(data.state) {
		  case State::Empty:
			if( !input ) {
			  // start bit received
			  data.state 	= State::BusyStartBit;
			  data.timer.Reset();
			}
			break;
		  case State::BusyStartBit:
			// update state
			data.state	= State::Busy;
			SetWaitShort();
			// sync
			data.timer.SyncStartBit();
			data.timer.Reset();
			break;
		  case State::Busy:
			{
			  // check if it match middle
			  if( data.timer.MatchMiddle() ) {
				// --- middle
				// reading bit
				// nothing todo for zero bit
				if( input ) {
				  // bit = 1
				  uint8_t& byte = (uint8_t&)data.buffer[data.byte_index];
				  byte |= (1 << data.bit_index);
				}
				// shift to next bit
				if( data.bit_index < 7 ) data.bit_index++;
				else {
				  // shift to next byte
				  if( ++data.byte_index < data.buffer_size ) {
					data.bit_index			= 0;
					data.buffer[data.byte_index]	= 0;
				  } else {
					Restart();
					return;
				  }
				}
				// reset counter
				data.timer.Sync();
				data.timer.Reset();
				SetWaitShort();
			  }
			}
			break;
		  }

		}

	  /// manchester RX status - return number of received bytes in buffer or 0 if none
	  template <class InputPinDriver, class InputPinChangeDriver, class TimerDriver>
		uint16_t Rx<InputPinDriver, InputPinChangeDriver, TimerDriver>::Get() {

		  return (data.state == State::Ready)?data.byte_index:0;

		}
	  /// manchester RX clear buffer function - call this as soon as you read data from buffer
	  /// before this call no new messages will be received
	  template <class InputPinDriver, class InputPinChangeDriver, class TimerDriver>
		void Rx<InputPinDriver, InputPinChangeDriver, TimerDriver>::Clear(void) {

		  if( data.state != State::Ready) return;
		  Restart();

		}

	  //------------------------------------------------------------------------------
	  // TX side
	  //------------------------------------------------------------------------------
	  /// manchester TX general init
	  template <class InputPinDriver, class InputPinChangeDriver, class OutputPinDriver, class TimerDriver>
		void Tx<InputPinDriver, InputPinChangeDriver, OutputPinDriver, TimerDriver>::Init(uint8_t* buffer, uint16_t buffer_size) {

		  // --- init values
		  data.buffer		= buffer;
		  data.buffer_size	= buffer_size;
		  data.state 		= State::Empty;
		  data.line_busy 	= true;
		  SetWait();
		  // --- GPIO
		  InputPinDriver::Init();
		  OutputPinDriver::Init();
		  OutputPinDriver::Set();
		  InputPinChangeDriver::Init();
		  InputPinChangeDriver::SetHandler(PinChangeUpdate);
		  // --- timer
		  TimerDriver::Init();
		  TimerDriver::SetHandler(TimerUpdate);
		  TimerDriver::Start();

		}

	  /**
	  \brief manchester TX transmit function
	  \param[in] data tx data
	  \param[in] count number of bytes from buffer to be send
	  */
	  template <class InputPinDriver, class InputPinChangeDriver, class OutputPinDriver, class TimerDriver>
		void Tx<InputPinDriver, InputPinChangeDriver, OutputPinDriver, TimerDriver>::Put(uint16_t count) {

		  switch(data.state) {
		  case State::Empty:
			data.bytes		= count;
			data.bit_takt	= 0;
			data.bit_index	= 0;
			data.byte_index	= 0;
			data.byte		= data.buffer[0];
			data.state = State::Ready;
			break;
		  default:
			break;
		  }

		}

	  /// manchester TX timer handler should be called baudrate * 2
	  template <class InputPinDriver, class InputPinChangeDriver, class OutputPinDriver, class TimerDriver>
		void Tx<InputPinDriver, InputPinChangeDriver, OutputPinDriver, TimerDriver>::TimerUpdate(void) {

		  uint8_t input = InputPinDriver::Get();

		  if( data.line_busy ) {
			// change state if line is free
			if( data.wait_count && input && !--(data.wait_count) ) data.line_busy = false;
			return;
		  }

		  switch(data.state) {
		  case State::Empty:
			break;
		  case State::Ready:
			// initiate sending data, first half of start bit
			TimerDriver::Scale();
			PinSet(0);
			data.state = State::BusyStartBit;
			break;
		  case State::BusyStartBit:
			// sending second half of the startup bit
			PinSet(1);
			data.state = State::Busy;
			break;
		  case State::Busy:
			// sending data
			{
			  uint8_t bit = data.byte & 0x1;
			  PinSet(( data.bit_takt ) ? bit : (( bit ) ? 0 : 1));

			  // next takt
			  if( data.bit_takt ) {
				data.bit_takt = 0;
				// next bit
				if( data.bit_index < 7 ) {
				  data.bit_index++;
				  data.byte >>= 1; // next bit of the same byte
				} else {
				  data.bit_index = 0;
				  // next byte if any
				  if( ++data.byte_index < *(uint8_t*)&data.bytes ) data.byte = data.buffer[data.byte_index];
				  else data.state = State::BusyLastBit;
				}
			  } else data.bit_takt = 1;
			}
			break;
		  case State::BusyLastBit:
			// finished
			TimerDriver::ResetScale();
			OutputPinDriver::Set();
			data.bytes		= 0;
			data.bit_out	= 1;
			data.state		= State::Empty;
			data.line_busy	= true;
			SetWait();
			break;
		  }

		}

	  /// manchester TX pin change handler should be called on any RX pin change
	  template <class InputPinDriver, class InputPinChangeDriver, class OutputPinDriver, class TimerDriver>
		void Tx<InputPinDriver, InputPinChangeDriver, OutputPinDriver, TimerDriver>::PinChangeUpdate(void) {

		  uint8_t input = InputPinDriver::Get();

		  if( data.line_busy ) {
			if( !input ) SetWait();
			return;
		  }

		  switch(data.state) {
		  case State::Empty:
		  case State::Ready:
			// resetting wait line counter if line goes down
			if( !input ) {
			  data.line_busy	= true;
			  SetWait();
			}
			break;
		  case State::BusyStartBit:
		  case State::Busy:
		  case State::BusyLastBit:
			// check that output and input are the same during sending
			if( input != data.bit_out) {
			  // release bus and wait
			  OutputPinDriver::Set();
			  data.state	= State::Ready;
			  data.line_busy	= true;
			  SetWait();
			}
			break;
		  }
		}

	}
  }
}