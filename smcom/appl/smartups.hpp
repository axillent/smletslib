#ifndef STAVRP_APPL_SMARTUPS_H
#define STAVRP_APPL_SMARTUPS_H

#include "../stavrp.h"
#include "../buffer.h"

namespace STAVRP {
  namespace Appl {

	template <typename UART, stavrp_funcp driver_on_func=nullfunc,
		uint8_t wait_cmd=10, uint8_t cmd_buffer=16, uint8_t pull_time=60, uint16_t conn_timeout=120>
	  class SmartUPS {
	  public:
		enum Cmd {
		  TurnOn,				///< turn ON
		  GraceShutdown,		///< turn OFF after grace period, will not return on power restore
		  SoftShutdown,			///< turn OFF if on battery, will return on power restore
		  ImmediateShutdown,	///< turn OFF without grace
		  SelfTest,				///< execute seld test
		  RuntimeCalibration,	///< run runtime calibration
		  ResetToDefaults		///< reset all to defaults
		};
		enum State {
		  Undefined,			///< not yet read
		  Off,					///< UPS is OFF
		  OnLine,				///< UPS is on line from main supply
		  OnBattery,			///< UPS is on battery
		  LowBattery			///< UPS is on battery and battery is low
		};
		struct Profile {
		  char		model[8];			///< model name
		  uint16_t	battery_v;			///< battery volatge *10
		  uint16_t	line_v;				///< line volateg *10
		  uint8_t	load;				///< load in %
		  uint8_t	battery_capacity;	///< current charging state in %
		  uint16_t	runtime;			///< estimated runtime in minutes
		};
	  protected:
		Fifo<uint8_t, uint8_t, cmd_buffer>	commands;
		struct Data {
		  Profile			profile;
		  State				state;
		  volatile uint8_t	presecond;
		  volatile uint16_t	connection_timeout;
		  volatile uint8_t	wait_next_cmd;
		  volatile uint8_t	pool_next_cmd;
		  volatile uint8_t	pool_next_timer;
		} data;
	  public:
		//----------------------------------------------------------------------
		// Constructor
		//----------------------------------------------------------------------
		SmartUPS() { Init(); }
		//----------------------------------------------------------------------
		// Init
		//----------------------------------------------------------------------
		void Init(void) {
		  UART::Start();
		  data.presecond = 0;
		  data.connection_timeout = conn_timeout;
		  data.wait_next_cmd = wait_cmd;
		  data.state = Undefined;
		}
		//----------------------------------------------------------------------
		// Loop
		//----------------------------------------------------------------------
		void Loop(void) {
		  //--- check if any data available from UPS
		  if( UART::RXAvailable() ) {
			data.connection_timeout = conn_timeout;
		  }

		  //--- check if next command should be issued
		  if( !UART::RXAvailable() ) {
			if( !data.wait_next_cmd ) {
			  //--- ready to send next command
			  if( commands.AvailablePop() ) {
				uint8_t cmd = commands.Pop();
				if( driver_on_func!=nullfunc ) driver_on_func();
				switch( cmd ) {
				}
				data.wait_next_cmd = wait_cmd;
			  }
			}
		  }
		}
		//----------------------------------------------------------------------
		// Update100MSec
		//----------------------------------------------------------------------
		void Update100MSec(void) {
		  if( data.wait_next_cmd ) data.wait_next_cmd--;
		  if( ++(data.presecond) == 10 ) {
			data.presecond = 0;
			// each second
			if( data.connection_timeout ) data.connection_timeout--;
		  }
		}
		//----------------------------------------------------------------------
		// GetProfile
		//----------------------------------------------------------------------
		const Profile& GetProfile() const { return data.profile; }
		//----------------------------------------------------------------------
		// GetState
		//----------------------------------------------------------------------
		State GetState() const { return data.state; }
		//----------------------------------------------------------------------
		// IsConnected
		//----------------------------------------------------------------------
		bool IsConnected() const { return data.connection_timeout; }
		//----------------------------------------------------------------------
		// SendCmd
		//----------------------------------------------------------------------
		bool SendCmd(Cmd cmd) {
		  if( !commands.AvailablePush() ) return false;

		  commands.Push(cmd);
		  return true;
		}
	  };

  }
}

#endif
