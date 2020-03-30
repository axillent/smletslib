#ifndef SMCOM_SOFT_DRV_ADC_H
#define SMCOM_SOFT_DRV_ADC_H

#include "../smcom.hpp"

namespace Smcom {  namespace SoftDrv { namespace ADCDrv {
  
  //----------------------------------------------------------------------------
  // Input with none
  //----------------------------------------------------------------------------
  class ADCNone {
  public:
    static void Init() {}
    static bool IsDone() { return true; }
    static void Next() {}
  };
  
  //----------------------------------------------------------------------------
  // Input with function transformation
  //----------------------------------------------------------------------------
  int16_t adc_input_func(uint16_t data) { return data; }
  
  template <class ADCDriver, uint8_t channel, uint16_t count_p=1, int16_t (*func)(uint16_t data) = adc_input_func>
    class InputFunc {
    protected:
      enum Stat { NA, NotReady, Ready };
      static struct Data {
        Stat        stat;
        uint16_t    sum;
        uint8_t     count;
        int16_t     value;
      } data;
    public:
      static void Init() {
        ADCDriver::Start();
        data.stat = NA;
        data.sum = 0;
        data.count = 0;
      }
      static bool IsDone() {
        if( data.stat == Ready ) return true;
        if( data.stat == NA ) Next();
        if( ADCDriver::IsBusy() ) return false;
        
        data.sum += ADCDriver::ReadValue();
        
        if( ++data.count == count_p ) {
          data.value = func(data.sum);
          data.sum = 0;
          data.count = 0;
          data.stat = Ready;
          return true;
        }
        
        ADCDriver::Next();
        return false;
      }
      static void Next() {
        if( data.stat != NA && !IsDone() ) return;
        if( data.stat == NotReady ) return;
        ADCDriver::SetChannel(channel);
        ADCDriver::Next();
        data.stat = NotReady;
      }
      static int16_t Read() {
        if( data.stat == NA ) while( !IsDone() );
        return data.value;
      }
    };
  template <class ADCDriver, uint8_t channel, uint16_t count, int16_t (*func)(uint16_t data)>
    typename InputFunc<ADCDriver, channel, count, func>::Data InputFunc<ADCDriver, channel, count, func>::data;

  //----------------------------------------------------------------------------
  // Input with multiplicator & division transformation
  //----------------------------------------------------------------------------
  template <uint16_t mult, uint16_t div>
    static int16_t adc_multdiv_func(uint16_t data) {
      uint32_t val = data;
      val /= div;
      val *= mult;
      return data; 
    }
  template <class ADCDriver, uint8_t channel, uint16_t mult, uint16_t div, uint16_t count_p=1>
    class InputMultDiv : public InputFunc<ADCDriver, channel, count_p, adc_multdiv_func<mult, div> > {};

  //----------------------------------------------------------------------------
  // Input with tracking
  // move - absolute difference to track as change
  // move_delay - report move chnage that othen
  // delay - report change on this delay even no move
  //----------------------------------------------------------------------------
  template <class InputDrv, uint16_t move, uint16_t move_delay=0, uint16_t delay=0>
    class Track : public InputDrv {
    protected:
      static int16_t value;
      static volatile uint16_t timer1;
      static volatile uint16_t timer2;
      static uint16_t abs(int16_t a, int16_t b) { return (a < b)?b-a:a-b; }
    public:
      static bool IsChanged(bool clear=false) {
        // --- return if no measurement made
        if( InputDrv::data.stat == InputDrv::NA ) return false;
        // --- check for timeout
        if( delay && !timer2 ) {
          timer2 = delay;
          timer1 = move_delay;
          return true;
        }
        // --- check for move and move timer
        if( (!move_delay || !timer1) && abs(value, InputDrv::data.value) >= move ) {
          if( clear ) {
            value = InputDrv::data.value;
            if( move_delay ) timer1 = move_delay;
          }
          return true;
        }
        return false;
      }
      static void TimerUpdate() {
        if( move_delay && timer1 ) timer1--;
        if( delay && timer2 ) timer2--;
      }
    };
    template <class InputDrv, uint16_t move, uint16_t move_delay, uint16_t delay> int16_t Track<InputDrv, move, move_delay, delay>::value;
    template <class InputDrv, uint16_t move, uint16_t move_delay, uint16_t delay> volatile uint16_t Track<InputDrv, move, move_delay, delay>::timer1 = 0;
    template <class InputDrv, uint16_t move, uint16_t move_delay, uint16_t delay> volatile uint16_t Track<InputDrv, move, move_delay, delay>::timer2 = delay;
  
  //----------------------------------------------------------------------------
  // Dispatcher for up to 8 inputs
  //----------------------------------------------------------------------------
  template <class Input1, class Input2, class Input3=ADCNone, class Input4=ADCNone, class Input5=ADCNone, class Input6=ADCNone, class Input7=ADCNone, class Input8=ADCNone>
    class Dispatcher {
    protected:
      static struct Data {
        uint8_t n;
        bool    ready;
      } data;
    public:
      static void Init() {
        Input1::Init();
        Input2::Init();
        Input3::Init();
        Input4::Init();
        Input5::Init();
        Input6::Init();
        Input7::Init();
        Input8::Init();
        data.n = 1;
        data.ready = false;
        Input1::Next();
      }
      static void Loop() {
        // --- check status
        switch(data.n) {
        case 1: if( !Input1::IsDone() ) return; break;
        case 2: if( !Input2::IsDone() ) return; break;
        case 3: if( !Input3::IsDone() ) return; break;
        case 4: if( !Input4::IsDone() ) return; break;
        case 5: if( !Input5::IsDone() ) return; break;
        case 6: if( !Input6::IsDone() ) return; break;
        case 7: if( !Input7::IsDone() ) return; break;
        case 8: if( !Input8::IsDone() ) return; break;
        }
        // --- done
        if( data.n==8 ) {
          data.n = 1;
          data.ready = true;
        } else data.n++;
        // --- next
        switch(data.n) {
        case 1: Input1::Next(); break;
        case 2: Input2::Next(); break;
        case 3: Input3::Next(); break;
        case 4: Input4::Next(); break;
        case 5: Input5::Next(); break;
        case 6: Input6::Next(); break;
        case 7: Input7::Next(); break;
        case 8: Input8::Next(); break;
        }
      }
      static bool IsReady() { return data.ready; }
    };
  template <class Input1, class Input2, class Input3, class Input4, class Input5, class Input6, class Input7, class Input8>
    typename Dispatcher<Input1, Input2, Input3, Input4, Input5, Input6, Input7, Input8>::Data Dispatcher<Input1, Input2, Input3, Input4, Input5, Input6, Input7, Input8>::data;
  
}}}

#endif
