#ifndef SMCOM_SOFTDRV_BUTTON_H
#define SMCOM_SOFTDRV_BUTTON_H

#include "../smcom.hpp"

namespace Smcom { namespace SoftDrv {

/*
		Button is to provide complex input to sense single, double and long press

 */

template <typename Input, int count_single=1, int mult_long=8>
class Button {
protected:
	static const int count_long = count_single*mult_long;
	static struct Data {
		volatile enum {
			state_clear=0, 	// clear
			state_single, 	// single press was happened
			state_long,
			state_double,
			state_predouble
		} state;
		volatile enum { undepress_off=0, undepress_on=1 } state_underpress;
		volatile int8_t	cnt;
		volatile bool	get;
	} data;
public:
	Button(void) { Init(); }
	static void Init(void) {
		data.state		= data.state_clear;
		data.state_underpress	= data.undepress_off;
		data.cnt		= 0;
		data.get		= 0;
		Input::Init();
	}
	static void Check(void);
	static void TimerUpdate(void);
	static void Clear(void) { data.state = data.state_clear; }
	static bool IsClear() { return data.state == data.state_clear; }
	static bool PressedSingle() { return data.state == data.state_single; }
	static bool PressedDouble() { return data.state == data.state_double; }
	static bool PressedLong() { return data.state == data.state_long; }
	static bool PressedAny() { return PressedSingle() | PressedDouble() | PressedLong(); }
	static bool UnderPress() { return data.state_underpress == data.undepress_on; }
};
template <typename Input, int count_single, int mult_long>
typename Button<Input, count_single, mult_long>::Data Button<Input, count_single, mult_long>::data;

template <typename Input, int count_single, int mult_long>
void Button<Input, count_single, mult_long>::Check(void) {
	switch(data.state) {
	case Data::state_clear:
	case Data::state_predouble:
		if( data.get != Input::Get() ) {
			// pin changed
			data.get = (data.get) ? false : true;
			if( data.get ) {
				// pressed
				if( data.cnt < 0 && data.cnt >= -count_single)
					data.state = data.state_predouble;
				data.cnt = 1;
			} else {
				// released
				if( data.cnt < count_single ) data.state = data.state_clear;
				else if( data.cnt >= count_single && data.cnt < count_long ) {
					data.state = ( data.state == data.state_predouble ) ? data.state_double : data.state_single;
				} else data.state = data.state_long;
				data.cnt = -1;
			}
		}
		break;
	case Data::state_single:
	case Data::state_long:
	case Data::state_double:
		break;
	}
}

template <typename Input, int count_single, int mult_long>
void Button<Input, count_single, mult_long>::TimerUpdate(void) {

	if( data.cnt > 0 ) {
		if( data.cnt < count_long ) data.cnt++;
		if( data.cnt == count_single ) data.state_underpress = data.undepress_on;
	} else if( data.cnt < 0 ) {
		if( --(data.cnt) == -count_single ) data.state_underpress = data.undepress_off;
		else if( -data.cnt == -count_long )  data.cnt = 0;
	}

}

/*
		ACInput is used to sense AC input using photocoupler

 */
template <typename Input, uint8_t timer_before_on, uint8_t timer_before_off>
class ACInput {
protected:
	static struct Data {
		volatile	bool	pin;
		volatile	bool	value;
		volatile	uint8_t	timer_on;
		volatile	uint8_t	timer_off;
	} data;
public:
	ACInput(void) { Init(); }
	static void Init(void) {
		Input::Init();
		data.pin = Input::Get();
		data.value = false;
		data.timer_on = 0;
		data.timer_off = 0;
	}
	static bool Get(void) { return data.value; }
	static void TimerUpdate(void) {
		if( data.timer_off ) {
			if( !--(data.timer_off) ) {
				data.value = false;
				data.timer_on = 0;
			} else {
				if( !data.value  && data.timer_on && !--(data.timer_on) ) data.value = true;
			}
		}
	}
	static void PinChangeUpdate(void) {
		bool pin = Input::Get();
		if( pin != data.pin ) {
			// pin changed
			data.pin = pin;
			// reset off timer
			if( pin ) {
				// pin is on
				data.timer_off = timer_before_off;
				// reset on timer on first impulse from off stage
				if( !data.value && !data.timer_on ) data.timer_on = timer_before_on;
			}
		}
	}
};
template <typename Input, uint8_t delay_before_on, uint8_t delay_before_off>
typename ACInput<Input, delay_before_on, delay_before_off>::Data ACInput<Input, delay_before_on, delay_before_off>::data;

/*
		Input is used to track any general input

 */
template <class InputDrv, uint8_t timer_p=0>
class Input {
protected:
	static struct Data {
      bool      state_act;
      bool      state_req;
      bool      changed;
      uint8_t   timer;
    } data;
public:
	Input(void) { Init(); }
	static void Init(void) {
      InputDrv::Init();
      data.state_act = InputDrv::Get();
      data.changed = false;
	}
	static void Check(void) {
      if( data.changed ) return;
      
      bool inp = InputDrv::Get();
      
      if( data.state_act == inp && data.state_act != data.state_req ) data.state_req = data.state_act;
      else if( data.state_act != inp && data.state_act == data.state_req ) {        
        data.state_req = inp;
        data.timer = timer_p;
      }
      if( !data.timer && (data.state_act != data.state_req) ) {
        data.state_act = data.state_req;
        data.changed = true;
      }
    }
	static void TimerUpdate(void) {
      if( timer_p ) {
        if( data.changed ) return;
        if( data.timer-- && data.state_req != InputDrv::Get() ) {
          data.state_req = data.state_act;
          data.timer = 0;
        }
      }
    }
    static bool IsChanged(bool clear = false) {
      if( data.changed ) {
        if( clear ) data.changed = false;
        return true;
      }
      return false;
    }
	static void Clear(void) { IsChanged(true); }
    static bool Get() { return data.state_act; }
};
template <typename InputDrv, uint8_t timer_p>
typename Input<InputDrv, timer_p>::Data Input<InputDrv, timer_p>::data;

}}


#endif
