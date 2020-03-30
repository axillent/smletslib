#ifndef SMCOM_COMMON_BUFFER_H
#define SMCOM_COMMON_BUFFER_H

namespace Smcom { namespace Common {

	//--------------------------------------------------------------------------------------------------
	// FIFO
	//--------------------------------------------------------------------------------------------------
	struct FifoState {
		enum type {
			Ok=0,
			Empty,
			Full,
			Overflow
		};
	};

	//--------------------------------------------------------------------------
	// Many elements
	//--------------------------------------------------------------------------
	template <typename Telem, typename Tsize, Tsize size>
	class Fifo {
	protected:
		struct Data {
						Telem			buffer[size];
			volatile	Tsize			ptr_first;
			volatile	Tsize			ptr_last;
			volatile	Tsize			n;
			volatile	FifoState::type	state;
		} data;
	public:
		Fifo() { Init(); }
		void Init(void) { Clear(); }
		void Clear(void) {
			data.ptr_first = data.ptr_last = data.n = 0;
			data.state = FifoState::Ok;
		}
		void Push(const Telem& value) {
			Tsize new_last = data.ptr_last;

			if( data.n ) {
				// buffer is not empty, look for empty cell
				if( ++new_last >= size ) new_last = 0;
				if( new_last == data.ptr_first ) {
					data.state = FifoState::Overflow;
					return;
				}
			}
			data.buffer[new_last] = value;
			data.ptr_last = new_last;
			data.n++;

		}
		/// return reference to first element
		const Telem& First() {
		  return data.buffer[data.ptr_first];
		}
		/// delete first element
		void DelFirst() {
		  	// --- nothing to delete
			if( !data.n ) return;

			// --- change remaining count
			if( !(--(data.n)) ) data.state = FifoState::Empty;

			// --- shift ptr
			Tsize last = data.ptr_last;
			if( data.ptr_first != last ) {
				if( data.ptr_first + 1 >= size) data.ptr_first = 0; else data.ptr_first++;
			}

		}
		/// pop first element and return it
		Telem Pop(void) {

		  	Telem element = First();
			DelFirst();

			return element;
		}
		FifoState::type State(void) const { return data.state; }
		Tsize AvailablePop(void) const { return data.n; }
		Tsize AvailablePush(void) const { return size - data.n; }
	};

	//--------------------------------------------------------------------------
	// Single elements
	//--------------------------------------------------------------------------
	template <typename Telem>
	class Fifo<Telem, uint8_t, 1> {
	protected:
		struct Data {
						Telem			element;
			volatile	FifoState::type	state;
		} data;
	public:
		Fifo() { Init(); }
		void Init(void) { Clear(); }
		void Clear(void) { data.state = FifoState::Empty; }
		void Push(const Telem& value) {
			switch( data.state ) {
			  case FifoState::Overflow:
			  case FifoState::Full:
				data.state = FifoState::Overflow;
				break;
			  case FifoState::Ok:
			  case FifoState::Empty:
				data.state = FifoState::Full;
				data.element = value;
			}
		}
		/// return reference to first element
		const Telem& First() { return data.element; }
		/// delete first element
		void DelFirst() { data.state = FifoState::Empty; }
		bool Pop(Telem& value) {
			if( data.state == FifoState::Empty ) return false;
			data.state = FifoState::Empty;
			value = data.element;
			return true;
		}
		Telem Pop(void) {
			data.state = FifoState::Empty;
			return data.element;
		}
		FifoState::type State(void) const { return data.state; }
		uint8_t AvailablePop(void) const { return (data.state == FifoState::Empty)?0:1; }
		uint8_t AvailablePush(void) const { return (data.state == FifoState::Empty)?1:0; }
	};

	//--------------------------------------------------------------------------------------------------
	// BUFFER
	//--------------------------------------------------------------------------------------------------
	template <typename Telem, typename Tsize, Tsize size>
	  class BufferFast {
	  protected:
		struct Data {
		  Telem		buffer[size];
		  Tsize		index_put;
		  Tsize		index_get;
		} data;
	  public:
		BufferFast() { ResetAll(); }
		void ResetAll() { data.index_put = data.index_get = 0; }
		void ResetGet() { data.index_get = 0; }
		void Put(Telem value) {
		  if( AvailablePut() ) data.buffer[data.index_put++] = value;
		}
		Telem Get() {
		  Telem elem = data.buffer[data.index_get];
		  if( data.index_get < data.index_put ) data.index_get++;
		  return elem;
		}
		Tsize Size() { return size; }
		Tsize AvailablePut() { return size - data.index_put; }
		Tsize AvailableGet() { return data.index_put - data.index_get;  }
	  };
}}

#endif
