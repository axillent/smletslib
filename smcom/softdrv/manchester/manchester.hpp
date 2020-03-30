/*
universal avr8/stm8/stm32 manchester encoding library
author axillent@gmail.com

supported: avr-gcc for AVR8
supported: IAR for STM8

*/

/**
\defgroup stavr_encoding STAVR encoding
\ingroup stavr
\brief different types of encoding/decoding

\defgroup stavr_encoding_manchester STAVR encoding manchester
\ingroup stavr_encoding
\brief manchester encoding
*/

#ifndef _STAVRP_ENCODING_MANCHESTER_H
#define _STAVRP_ENCODING_MANCHESTER_H

#include "../stavrp.h"

namespace STAVRP {
  namespace Encoding {
	namespace Manchester {

	  const int STAVR_ENCODING_MANCHESTER_WAIT = 32;

	  struct State {
		enum type {
		  Empty,		///< tx/rx buffer is empty
		  Ready,		///< tx ready to start sending/rx ready with a new message received
		  BusyStartBit,	///< tx/rx is busy with sending/receiving start bit
		  BusyLastBit,	///< tx is busy with sending last bit
		  Busy			///< tx/rx is busy with sending/receiving message data
		};
	  };

	  //----------------------------------------------------------------------------------------------------
	  // RX side
	  //----------------------------------------------------------------------------------------------------
	  template <class InputPinDriver, class InputPinChangeDriver, class TimerDriver>
		class Rx {
		protected:
		  //--------------------------------------------------------------------
		  static struct Data {
			volatile  uint8_t*		buffer;
			uint8_t			buffer_size;
			volatile  uint8_t		wait_count;	 ///< count to wait bus release
			volatile  uint8_t		byte_index;	 ///< index of byte receiving now
			volatile  State::type	state;
			volatile  bool		line_busy;
			volatile  uint8_t		bit_index;	 ///< bit number receiving now
			//------------------------------------------------------------------
			struct {
			  volatile uint16_t soft_cnt;
			  volatile uint16_t hard_cnt;
			  volatile uint16_t sync_value;
			  //----------------------------------------------------------------
			  // --- manipulating methods
			  // --- reset software & hardware counter
			  void Reset(void) {
				soft_cnt = 0;
				TimerDriver::ResetCounter();
			  }
			  // --- update at timer interrupt
			  void Update(void) { soft_cnt += TimerDriver::GetCounterMax() + 1; }
			  // --- record hardware counter
			  void RecordHardCnt() {
				hard_cnt = TimerDriver::GetCounter();
				if( TimerDriver::IsPendingUpdate() ) hard_cnt += TimerDriver::GetCounterMax() + 1;
			  }
			  // --- software counter
			  uint16_t GetCnt (void) {
//				//return hard_cnt + *(uint16_t*)&soft_cnt;
				return TimerDriver::GetCounter();
			  }
			  // --- sync for start bit
			  void SyncStartBit(void) {	sync_value = 2 * GetCnt(); }
			  // --- sync for next bit
			  void Sync(void) {	sync_value = GetCnt(); }
			  // --- match middle
			  bool MatchMiddle(void) {
				uint16_t cnt = GetCnt();
				uint16_t interval = sync_value / 8;
				if( cnt < sync_value - interval || cnt > sync_value + interval ) return false;
				return true;
			  }
			} timer;	// software counter with reset function
			//------------------------------------------------------------------
		  } data;
		  static void Restart(void);
		  static void SetWait(void) { data.wait_count = STAVR_ENCODING_MANCHESTER_WAIT; }
		  static void SetWaitShort(void) { data.wait_count = STAVR_ENCODING_MANCHESTER_WAIT / 2; }
		public:
		  static void Init(uint8_t* buffer, uint16_t buffer_size);
		  static void TimerUpdate(void);
		  static void PinChangeUpdate(void);
		  static State::type GetState(void) { return data.state; }
		  static uint16_t Get(void);
		  static void Clear(void);
		};
	  template <class InputPinDriver, class InputPinChangeDriver, class TimerDriver>
		Rx<InputPinDriver, InputPinChangeDriver, TimerDriver>::Data Rx<InputPinDriver, InputPinChangeDriver, TimerDriver>::data;

	  //----------------------------------------------------------------------------------------------------
	  // TX side
	  //----------------------------------------------------------------------------------------------------
	  template <class InputPinDriver, class InputPinChangeDriver, class OutputPinDriver, class TimerDriver>
		class Tx {
		protected:
		  static struct Data {
			volatile  uint8_t*		buffer;			///< tx buffer
			uint8_t			buffer_size;
			volatile  uint8_t		wait_count;		///< count to wait bus release
			volatile  uint8_t		bytes;			///< bytes to send
			volatile  uint8_t		byte;			///< byte data sending now
			volatile  uint8_t		byte_index;		///< index of byte sending now
			volatile  State::type	state;			///< tx state
			volatile  bool		line_busy;
			volatile  uint8_t		bit_takt;		///< bit phase 0/1
			volatile  uint8_t		bit_index;		///< bit number sending now
			volatile  uint8_t		bit_out;		///< current output value
		  } data;
		  static void SetWait(void) { data.wait_count = 2 * STAVR_ENCODING_MANCHESTER_WAIT; }
		  static void PinSet(bool value) { OutputPinDriver::Set(data.bit_out = value); }
		public:
		  static void Init(uint8_t* buffer, uint16_t buffer_size);
		  static void TimerUpdate(void);
		  static void PinChangeUpdate(void);
		  static State::type GetState(void) { return data.state; }
		  static void Put(uint16_t bytes);
		  static void Clear(void);
		};
	  template <class InputPinDriver, class InputPinChangeDriver, class OutputPinDriver, class TimerDriver>
		Tx<InputPinDriver, InputPinChangeDriver, OutputPinDriver, TimerDriver>::Data Tx<InputPinDriver, InputPinChangeDriver, OutputPinDriver, TimerDriver>::data;

	}
  }
}

#include "manchester.c"

#endif
