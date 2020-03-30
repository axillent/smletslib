#ifndef SMCOM_BLINK_H
#define SMCOM_BLINK_H

namespace Smcom { namespace SoftDrv {

  template <class LedPin>
	class Blink : public LedPin {
    protected:
      static struct Data {
		volatile uint8_t	count;
		volatile uint8_t	timer;
				 uint8_t	delay_on, delay_off;
      } data;
    public:
	  Blink() { Init(); }
      static void Init(void) {
		LedPin::Init();
		data.count = 0;
      }
      static void Set() { Stop(true); }
      static void Set(bool pin) { Stop(pin); }
      static void Clr() { Stop(false); }
      static void Run(uint8_t count, uint8_t delay_on, uint8_t delay_off=0) {
		data.count	= count;
		data.timer	= 0;
		data.delay_on	= delay_on;
		data.delay_off	= (delay_off) ? delay_off : delay_on;
      }
      static void Stop(bool pin=false) {
		data.count = 0;
		LedPin::Set(pin);
      }
      static bool IsStop(void) {
		return !data.count;
      }
      static void TimerUpdate(void) {
		if( data.count ) {
		  if( ! data.timer ) {
			if( LedPin::Get() ) {
			  LedPin::Clr();
			  data.timer = data.delay_off;
			  if( data.count != 255 ) data.count--;
			} else {
			  LedPin::Set();
			  data.timer = data.delay_on;
			}
		  } else {
			data.timer--;
		  }
		}
      }
    };

  template <class LedPin>
    typename Blink<LedPin>::Data Blink<LedPin>::data;

}}

#endif
