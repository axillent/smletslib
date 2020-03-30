/**

	\todo add togle

 */

#ifndef SMCOM_SOFT_DRV_GPIO_H
#define SMCOM_SOFT_DRV_GPIO_H

namespace Smcom { namespace SoftDrv {

class PinMode {
protected:
	enum {
		out_bit		= (1 << 0),// 1 = output, 0 = input
		up_bit		= (1 << 1),// 1 = pullup/pushpull
		down_bit	= (1 << 2),// 1 = pulldown
		int_bit		= (1 << 3),// 1 = interrupt on this pin
		med_speed	= (1 << 4),// 1 = medium speed
		hi_speed	= (1 << 5),// 1 = high speed
		inv_bit		= (1 << 7),// 1 = inversed pin
		alt_bit		= (1 << 8),// 1 = alternative mode
        analog_bit  = (1 << 9) // 1 = analog mode
	};
public:
	enum type {
		// --- basic modes
		Input              	= 0,                           	// input mode without pullup/pulldown
		Output             	= out_bit,                     	// output
		PullUp				= up_bit,						// input pull up
		PushPull			= out_bit | up_bit,				// putput push-pull
		PullDown			= down_bit,						// input pull down
		Interrupt			= int_bit,						// interrupt enable mode
		LowSpeed			= 0,							// low speed
		MediumSpeed			= med_speed,					// medium speed
		HighSpeed			= hi_speed,						// high speed
		Inverse				= inv_bit,						// inverse mode
		// --- combined input modes
		InputPullUp        	= Input | PullUp,              	// input + pullup
		InputPullDown      	= Input | PullDown,            	// input + pulldown
		// --- combined output modes
		OutputPushPull     	= Output | PushPull,           	// output
        Alt                 = alt_bit,                      // alternative mode
        Analog              = analog_bit                    // analog mode
	} value;
	PinMode(type mode) { value = mode; }
	static bool IsOut(uint16_t mode)	{ return static_cast<uint16_t>(mode) & out_bit; 	}
	static bool IsUp(uint16_t mode)	    { return static_cast<uint16_t>(mode) & up_bit; 	    }
	static bool IsDown(uint16_t mode)   { return static_cast<uint16_t>(mode) & down_bit;    }
	static bool IsInv(uint16_t mode)    { return static_cast<uint16_t>(mode) & inv_bit; 	}
	static bool IsInt(uint16_t mode)	{ return static_cast<uint16_t>(mode) & int_bit; 	}
	static bool IsAlt(uint16_t mode)	{ return static_cast<uint16_t>(mode) & alt_bit; 	}
	static bool IsAnalog(uint16_t mode)	{ return static_cast<uint16_t>(mode) & analog_bit; 	}
};

class PWMMode {
public:
  enum pwm_mode { Mode1 = 6, Mode2 = 7  };
  enum pwm_channel { CH1, CH2, CH3, CH4, CH1N, CH2N, CH3N, CH4N };
};

//------------------------------------------------------------------------
// PortClass
//------------------------------------------------------------------------
template <class PinDriver, uint16_t mode_t>
class PortClass : public PinDriver {
public:
	PortClass(const uint16_t mode=mode_t) { Init(mode); }
	static void Init(const uint16_t mode=mode_t) {
		Clr();
		PinDriver::Init(mode);
	}
	inline static bool Get() {
		if( PinMode::IsInv(mode_t) ) return (PinDriver::Get())?false:true;
		else return (PinDriver::Get())?true:false;
	}
	inline static void Set() {
		if( PinMode::IsInv(mode_t) ) PinDriver::Clr(); else PinDriver::Set();
	}
	inline static void Set(const bool bit) {
		if( bit ) Set(); else Clr();
	}
	inline static void Clr() {
		if( PinMode::IsInv(mode_t) ) PinDriver::Set(); else PinDriver::Clr();
	}
	inline static void Toggle() {
		if( PinDriver::Get() ) PinDriver::Clr(); else PinDriver::Set();
	}
};

//------------------------------------------------------------------------
// Output
//------------------------------------------------------------------------
namespace PWM {
  template <class Timer, Smcom::SoftDrv::PWMMode::pwm_channel channel, uint8_t gpio_mode = Smcom::SoftDrv::PinMode::Output, uint8_t pwm_mode = Smcom::SoftDrv::PWMMode::Mode1>
    class Output {
    public:
      static void Init() {
        Timer::Init();
        Timer::template ChannelInit<channel, gpio_mode, pwm_mode>();
      }
      static void Set(uint16_t pwm) { Timer::template ChannelSet<channel>(pwm); }
    };
}

}}

#endif
