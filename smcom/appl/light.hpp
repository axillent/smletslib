#ifndef SMCOM_APPL_LIGHT_H
#define SMCOM_APPL_LIGHT_H

#include "../smcom.hpp"

namespace Smcom {
  namespace Appl {
    
    struct LightState {
      enum type {
        StartUp,
        Off,			// led is off
        ToOn,			// led is going to be on
        ToOff,			// led is going to be off
        On,				// led is on
        HalfOn,			// led is in half power
        ToHalfOn		// led is going to be in half power
      };
    };
    
    template <int startup_delay_sec, int power_max, int power_step>
      class Light {
      protected:
        static struct Data {
          volatile LightState::type		status;
          uint8_t							sensor;
          volatile uint8_t				power;
          volatile uint16_t				timeout;
          volatile uint16_t				on_sec;
          volatile uint16_t				half_on_sec;
        } data;
        static void	Settimer(void);
        
      public:
        static void Init(void);
        static uint8_t	Loop(uint8_t const sensor, uint16_t const timeout);
        static void	Update100MSec(void);
        static void	UpdateSec(void);
        static LightState State(void) { return data.status; }
        
      };
    
    template <int startup_delay_sec, int power_max, int power_step>
      typename Light<startup_delay_sec, power_max, power_step>::Data Light<startup_delay_sec, power_max, power_step>::data;
    
    template <int startup_delay_sec, int power_max, int power_step>
      void Light<startup_delay_sec, power_max, power_step>::Init(void) {
        data.power = data.sensor = 0;
        data.status	= LightState::StartUp;
        data.on_sec = startup_delay_sec;
      }
    
    template <int startup_delay_sec, int power_max, int power_step>
      uint8_t Light<startup_delay_sec, power_max, power_step>::Loop(uint8_t const sens, uint16_t const to) {
        
        data.timeout = to;
        
        if(sens) {
          // sensor is on
          switch(data.status) {
          case LightState::Off:					
          case LightState::HalfOn:				
          case LightState::ToOff:					
          case LightState::ToHalfOn:				
			data.status = LightState::ToOn;		
			break;
          case LightState::ToOn:					
          case LightState::StartUp:
			break;
          case LightState::On:					
			Settimer();							
			break;
          }
        } else if( data.sensor && !sens ) {
          // change from 1 to 0
          switch(data.status) {
          case LightState::Off:
          case LightState::StartUp:
			break;
          case LightState::ToOn:
          case LightState::ToOff:
          case LightState::On:
          case LightState::HalfOn:
          case LightState::ToHalfOn:
			Settimer();
			break;
          }
        }
        data.sensor = sens;
        
        return data.power;
      }
    
    template <int startup_delay_sec, int power_max, int power_step>
      void Light<startup_delay_sec, power_max, power_step>::Update100MSec() {
        // 10 times per second
        
        switch(data.status) {
        case LightState::StartUp:
        case LightState::Off:
        case LightState::On:
        case LightState::HalfOn:
          break;
        case LightState::ToOn:
          if( data.power >= (power_max-power_step) ) {
			data.power = power_max;
			data.status = LightState::On;
			Settimer();
          } else {
			data.power += power_step;
          }
          break;
        case LightState::ToOff:
          if( !--data.power ) data.status = LightState::Off;
          break;
        case LightState::ToHalfOn:
          if( --data.power <= power_max / 2) data.status = LightState::HalfOn;
          break;
        }
      }
    
    template <int startup_delay_sec, int power_max, int power_step>
      void Light<startup_delay_sec, power_max, power_step>::UpdateSec() {
        
        switch(data.status) {
        case LightState::StartUp:
          if( data.on_sec && !--(data.on_sec) ) data.status = LightState::Off;
          break;
        case LightState::On:
        case LightState::HalfOn:
          if( data.on_sec && !--(data.on_sec) ) data.status = LightState::ToOff;
          if( data.half_on_sec && !--(data.half_on_sec) ) data.status = LightState::ToHalfOn;
          break;
        case LightState::Off:
        case LightState::ToHalfOn:
        case LightState::ToOff:
        case LightState::ToOn:
          break;
        }
      }
    
    template <int startup_delay_sec, int power_max, int power_step>
      void Light<startup_delay_sec, power_max, power_step>::Settimer(void) {
        data.on_sec = data.timeout;
        data.half_on_sec = data.on_sec / 2;
      }
    
  }}

#endif
