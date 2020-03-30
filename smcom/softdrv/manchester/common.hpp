#ifndef _STAVRP_ENCODING_MANCHESTER_COMMON_H
#define _STAVRP_ENCODING_MANCHESTER_COMMON_H

#include "../../common/buffer.hpp"

namespace STAVRP {
  namespace SoftDrv {
	namespace Manchester {

	  //-----------------------------------------------------------------------
	  // Manchester Buffer State
	  //-----------------------------------------------------------------------
	  class BufferState {
	  protected:
		volatile enum { Empty, Busy, Ready } state;
	  public:
		BufferState() { SetEmpty(); }
		void SetEmpty() { state = Empty; }
		void SetBusy() { state = Busy; }
		void SetReady() { state = Ready; }
		bool IsEmpty() { return state == Empty; }
		bool IsBusy() { return state == Busy; }
		bool IsReady() { return state == Ready; }
	  };

	  //-----------------------------------------------------------------------
	  // Manchester TXBuffer
	  //-----------------------------------------------------------------------
	  template <typename Tsize, Tsize size>
		class TXBuffer: public STAVRP::Common::BufferFast<uint8_t, Tsize, size>,
			public BufferState {
		protected:
		  //--- constants
		  static const uint8_t bit_start_tact0	= 255;
		  static const uint8_t bit_start_tact1	= 254;
		  static const uint8_t bit_overflow		= 16;
		  //--- variables
		  uint8_t	get_byte;
		  uint8_t	get_bit;
		public:
		  //--- constructor
		  TXBuffer() : STAVRP::Common::BufferFast<uint8_t, Tsize, size>(),
		  	BufferState() {
			get_bit = bit_start_tact0;
		  }
		  //--- reset put & get
		  void ResetAll() {
			get_bit = bit_start_tact0;
			STAVRP::Common::BufferFast<uint8_t, Tsize, size>::ResetAll();
			BufferState::SetEmpty();
		  }
		  //--- reset only get
		  void ResetGet() {
			get_bit = bit_start_tact0;
			STAVRP::Common::BufferFast<uint8_t, Tsize, size>::ResetGet();
			BufferState::SetReady();
		  }
		  //--- true if some bits to get
		  bool AvailableGet() {
			return get_bit != bit_overflow || STAVRP::Common::BufferFast<uint8_t, Tsize, size>::AvailableGet();
		  }
		  //--- return next bit
		  uint8_t Get() {

			switch( get_bit ) {
			case bit_start_tact0:
			  get_bit = bit_start_tact1;
			  return 0;
			case bit_start_tact1:
			  get_bit = bit_overflow;
			  return 1;
			case bit_overflow:
			  get_byte =  STAVRP::Common::BufferFast<uint8_t, Tsize, size>::Get();
			  get_bit = 0;
			  break;
			}

			uint8_t bit = get_byte & 1;

			// if first tact
			if( !(get_bit++ & 1) ) return bit ^ 1;

			// last tact
			get_byte >>= 1;

			return bit;
		  }
		  //--- overrides Put
		  void Put(uint8_t byte) {
			if( BufferState::IsEmpty() ) STAVRP::Common::BufferFast<uint8_t, Tsize, size>::Put(byte);
		  }
		  //--- overrides AvailablePut
		  bool AvailablePut() {
			return BufferState::IsEmpty() && STAVRP::Common::BufferFast<uint8_t, Tsize, size>::AvailablePut();
		  }
		};

	  //-----------------------------------------------------------------------
	  // Manchester RXBuffer
	  //-----------------------------------------------------------------------
	  template <typename Tsize, Tsize size>
		class RXBuffer: public STAVRP::Common::BufferFast<uint8_t, Tsize, size>,
			public BufferState {
		protected:
		  uint8_t	put_byte;
		  uint8_t	put_bit;
		public:
		  RXBuffer() : STAVRP::Common::BufferFast<uint8_t, Tsize, size>(),
		  	BufferState() {
			put_bit = 0;
		  }
		  void ResetAll() {
			STAVRP::Common::BufferFast<uint8_t, Tsize, size>::ResetAll();
			put_bit = 0;
			BufferState::SetEmpty();
		  }
		  bool AvailablePut() {
			return put_bit < 8 || STAVRP::Common::BufferFast<uint8_t, Tsize, size>::AvailablePut();
		  }
		  void Put(uint8_t bit) {
			if( bit ) put_byte |= 0x80;

			put_bit++;

			if( put_bit == 8 ) {
			  STAVRP::Common::BufferFast<uint8_t, Tsize, size>::Put(put_byte);
			  put_byte = 0;
			  put_bit = 0;
			} else  put_byte >>= 1;

		  }
		  bool IsConsistent() {
			return !put_bit && STAVRP::Common::BufferFast<uint8_t, Tsize, size>::data.index_put;
		  }
		  //--- overrides Get
		  uint8_t Get() {
			return ( BufferState::IsReady() ) ? STAVRP::Common::BufferFast<uint8_t, Tsize, size>::Get() : 0;
		  }
		  //--- overrides AvailableGet
		  bool AvailableGet() {
			return BufferState::IsReady() && STAVRP::Common::BufferFast<uint8_t, Tsize, size>::AvailableGet();
		  }
		};

	}
  }
}

#endif
