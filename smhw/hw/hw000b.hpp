#ifndef SMHW_HW_000B_HPP
#define SMHW_HW_000B_HPP

// --- STM8S library
#include <smcom/platform/stm8s903k3.hpp>
#include <smcom/platform/stm8/s/mcu.hpp>
#include <smcom/platform/stm8/s/gpio.hpp>
#include <smcom/platform/stm8/s/extint1B.hpp>
#include <smcom/platform/stm8/s/pwm.hpp>
#include <smcom/platform/stm8/s/timer5.hpp>
#include <smcom/platform/stm8/s/timer6.hpp>
#include <smcom/platform/stm8/s/uart1.hpp>
#include <smcom/platform/stm8/s/adcbase.hpp>
#include <smcom/platform/stm8/s/eeprom.hpp>

// --- common library
#include <smcom/softdrv/button.hpp>
#include <smcom/softdrv/timer.hpp>
#include <smcom/softdrv/led_scan.hpp>
#include <smcom/softdrv/display_segments.hpp>
#include <smcom/softdrv/adc.hpp>
#include <smcom/softdrv/ntc/mf52at_10k_40_125_05.hpp>

#include <smhw/hw-proto.hpp>

using Smcom::SoftDrv::PinMode;
using Smcom::STM8S::PortB;
using Smcom::STM8S::PortC;
using Smcom::STM8S::PortD;
using Smcom::STM8S::UART1;
using Smcom::STM8S::Timer6;
using Smcom::STM8S::Timer5;
using Smcom::SoftDrv::NTC::NTC;

using Smcom::SoftDrv::LedScan::RawColDrv;
using Smcom::SoftDrv::LedScan::Display;
using Smcom::SoftDrv::LedScan::PutReverseDrv;
using Smcom::SoftDrv::DisplaySegments;
using Smcom::SoftDrv::Timer::Universal;

class HW000B {
protected:
  static smcom_funcp handler;
  // --- LED display
  typedef PortD<0>	                    dig0;
  typedef PortD<3>	                    dig1;
  typedef PortD<7>	                    dig2;
  typedef PortC<1, PinMode::Inverse>	segA;
  typedef PortC<2, PinMode::Inverse>	segB;
  typedef PortC<3, PinMode::Inverse>	segC;
  typedef PortC<4, PinMode::Inverse>	segD;
  typedef PortC<5, PinMode::Inverse>	segE;
  typedef PortC<6, PinMode::Inverse>	segF;
  typedef PortC<7, PinMode::Inverse>	segG;
  typedef PortB<4, PinMode::Inverse>	segDP;
  typedef RawColDrv<3, dig0, dig1, dig2> 	                            LedDig;
  typedef RawColDrv<8, segA, segB, segC, segD, segE, segF, segG, segDP> LedSeg;
  typedef Display<LedSeg, LedDig, PutReverseDrv>                        LedDrv;
  typedef Smcom::STM8S::ADCBaseDriver<>                                 adcdriver;
public:
  // -- hw identification
  static const uint16_t HWID = 0x000B;
  static const char* GetHWName() {   return "SmartFan1";  }
  // --- public class
  typedef Smcom::STM8S::MCU<Smcom::STM8S::MCUCommon::HSI_16MHz>   MCU;
  typedef Smcom::STM8S::UART1<MCU, 57600, 64, 64>                 UartDriver;
  typedef DisplaySegments<LedDrv>                                 Led;
  typedef Smcom::SoftDrv::Button<PortB<3, PinMode::Interrupt> >   ButtonPlus;
  typedef Smcom::SoftDrv::Button<PortB<1, PinMode::Interrupt> >   ButtonMinus;
  typedef NTC<mf52at_10k_40_125_05, adcdriver, 0>                 Ntc;
protected:
  typedef Smcom::STM8S::Timer5<MCU, 10>                 Timer;
  typedef Universal<1, Timer6<MCU, 1600>, Led::Update>  LedTimer;
  typedef Smcom::STM8S::PWM::Timer1<1000, 100, MCU>     Timer1;
  typedef Smcom::SoftDrv::PWM::Output<Timer1, Smcom::SoftDrv::PWMMode::CH3N, Smcom::SoftDrv::PinMode::PushPull> Pwm;
  static void _Update100MSec(void) {
    ButtonPlus::TimerUpdate();
    ButtonMinus::TimerUpdate();
    handler();
  }
  static void HandlerB() {
    ButtonPlus::Check();
    ButtonMinus::Check();
  }
  // --- handler port B
  typedef Smcom::STM8S::HandlerPortB<Smcom::STM8S::MCUCommon::EXTINT_FallingRising, HandlerB> HandlerPortB;
public:
  static void SetPwm(uint16_t pwm) {
	Pwm::Set(pwm);
  }
  static void Init(smcom_funcp ptr) {
    handler = ptr;
    MCU::Init();
	HandlerPortB::Init();
    Timer::Init();
    Timer::SetHandler(_Update100MSec);
    ButtonPlus::Init();
    ButtonMinus::Init();
    Pwm::Init();
    Led::Init();
    Ntc::Init();
  }
  static void Start(void) {
    UartDriver::Start();
    LedTimer::Start();
    Timer::Start();
    MCU::InterruptEnable();
  }
  static void Loop(void) {
    Ntc::Next();
    MCU::InterruptWait();
  }
};
smcom_funcp HW000B::handler;

#endif