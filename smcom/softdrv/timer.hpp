#ifndef SMCOM_SOFT_DRV_TIMER_H
#define SMCOM_SOFT_DRV_TIMER_H

#include "../smcom.hpp"

namespace Smcom { namespace SoftDrv { namespace Timer {

    //----------------------------------------------------------------------
    // Universal timer
    //----------------------------------------------------------------------
    class NextNop {
    public:
      static void Handler(void) {}
    };

    template <uint8_t precount, typename TimerDriver, smcom_funcp func, class Next=NextNop>
      class Universal {
      protected:
	static uint8_t  count;
	static void Handler(void) {
	  if( precount > 1 ) {
	    if( ++count < precount ) return;
	    count = 0;
	  }

	  func();
	  Next::Handler();
	}
      public:
	static void Start(void) {
	  TimerDriver::Init();
	  TimerDriver::SetHandler(Handler);
	  TimerDriver::Start();
	}
	static void Stop(void) {
	  TimerDriver::stop();
	}
      };
    template <uint8_t precount, typename TimerDriver, smcom_funcp func, class Next>
      uint8_t Universal<precount, TimerDriver, func, Next>::count;

    //----------------------------------------------------------------------
    // Call class
    //----------------------------------------------------------------------
    template <smcom_funcp func, uint8_t precount=1, class Next=NextNop>
      class Call {
      protected:
	static uint8_t  count;
      public:
	static void Handler(void) {
	  if( precount > 1 ) {
	    if( ++count < precount ) return;
	    count = 0;
	  }

	  func();
	  Next::Handler();
	};
      };
    template <smcom_funcp func, uint8_t precount, class Next>
      uint8_t Call<func, precount, Next>::count = 0;

    //----------------------------------------------------------------------
    // Verious timer helpers
    //----------------------------------------------------------------------
    template <uint16_t count_t, smcom_funcp func>
      class TaskRepeat {
      protected:
        static struct Data {
          volatile uint16_t  count;
        } data;
      public:
        static void Init() {
          data.count = count_t;
        }
        static void Loop() {
          if( !data.count ) {
            func();
            data.count = count_t;
          }
        }
        static void TimerUpdate() {
          if( data.count ) data.count--;
        }
        static void Reset() {
          data.count = count_t;
        }
      };

    template <uint16_t count_t, smcom_funcp func>
      typename TaskRepeat<count_t, func>::Data TaskRepeat<count_t, func>::data;

    // ---
    template <smcom_funcp func = nullfunc>
      class TaskOnceAfter {
      protected:
        static struct Data {
          volatile uint16_t  count;
        } data;
      public:
        static void Init() {
          data.count = 0;
        }
        static bool Loop() {
          if( data.count == 1 ) {
            data.count = 0;
            if( func != nullfunc ) func();
            return true;
          }
          return false;
        }
        static void TimerUpdate() {
          if( data.count > 1 ) data.count--;
        }
        static void Start(uint16_t count = 0) {
          data.count = count + 1;
        }
        static void Reset() {
          data.count = 0;
        }
      };
    template <smcom_funcp func>
      typename TaskOnceAfter<func>::Data TaskOnceAfter<func>::data;

}}}

#endif
