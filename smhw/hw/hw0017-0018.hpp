//------------------------------------------------------------------------------
// Hardware
//------------------------------------------------------------------------------
#include <smcom/platform/stm32f030x4.hpp>

#include <smlets/node/appl/appl-proto.hpp>
#include <smcom/platform/stm32/f0/mcu.hpp>
#include <smcom/platform/stm32/f0/gpio.hpp>
#include <smcom/platform/stm32/f0/pwm.hpp>
#include <smcom/platform/stm32/f0/timer14.hpp>
#include <smcom/platform/stm32/f0/adcbase.hpp>
#include <smcom/platform/stm32/f0/uart1.hpp>

#include <smcom/softdrv/blink.hpp>
#include <smcom/softdrv/button.hpp>
#include <smcom/softdrv/timer.hpp>
#include <smcom/softdrv/ntc/mf52at_10k_40_125_05.hpp>
#include <smcom/softdrv/sensors/photoresistor.hpp>
#include <smcom/softdrv/sensors/zxct1009.hpp>

#include <smhw/hw-proto.hpp>
#include <smlets/node/node-uart.hpp>

using Smcom::STM32::PortA;
using Smcom::STM32::PortB;
using Smcom::STM32::PortF;
using Smcom::STM32::ADCBaseDriver;

using Smcom::SoftDrv::PinMode;
using Smcom::SoftDrv::PWMMode;
using Smcom::SoftDrv::NTC::NTC;
using Smcom::SoftDrv::Sensors::PhotoResistor;
using Smcom::SoftDrv::Sensors::ZXCT1009;
using Smcom::SoftDrv::ADCDrv::InputMultDiv;
using Smcom::SoftDrv::ADCDrv::Track;
using Smcom::SoftDrv::Timer::TaskRepeat;
using Smcom::SoftDrv::Timer::TaskOnceAfter;
using Smcom::SoftDrv::Blink;
using Smcom::SoftDrv::Button;

template <uint16_t zxct1009_max>
class HW0017 {
protected:
  // --- data
  enum State { NA, ON, OFF };
  static struct Data {
    smcom_funcp handler;
  } data;
  // --- main
  using MCU = Smcom::STM32::MCU<Smcom::STM32::MCUCommon::HSI_IN8_OUT16MHz>;
  using Timer =  Smcom::STM32::Timer14<MCU, 10>;
  // --- PWM
  using pwm_tim1 = Smcom::STM32::PWM::Timer1<1000, 100, MCU>;
  using pwm_tim3 = Smcom::STM32::PWM::Timer3<1000, 100, MCU>;
  // --- ntc thermometers
  using adcdriver = ADCBaseDriver<Smcom::STM32::ADCResolution::Bit10>;
  using ntc0 = Track<NTC<mf52at_10k_40_125_05, adcdriver, 4>, 10, 10, 300>;
  using cur0 = Track<ZXCT1009<adcdriver, 1, 50, zxct1009_max, 10>, 50, 10, 300>;
  using photo0 = Track<PhotoResistor<adcdriver, 0, 10>, 10, 10, 300>;
  using adcdisp = Smcom::SoftDrv::ADCDrv::Dispatcher<ntc0, cur0, photo0>;
  static void _Update100MSec(void) {
    LedTx::TimerUpdate();
    LedRx::TimerUpdate();
    PIR0::TimerUpdate();
    Sensor0::TimerUpdate();
    data.handler();
    Smcom::SoftDrv::Timer::Call<_Update1Sec, 10>::Handler();
  }
  static void _Update1Sec(void) {
    ntc0::TimerUpdate();
    cur0::TimerUpdate();
    photo0::TimerUpdate();
  }
public:
  // -- hw identification
  static const uint16_t HWID = 0x0017;
  static const char* GetHWName() {   return "Light3W";  }
  // --- objects
  using UartDriver = Smcom::STM32::UART1<MCU, 57600, 128, 128>;
  // --- gpio
  using LedTx = Blink<PortA<5, PinMode::Output | PinMode::Inverse> >;
  using LedRx = Blink<PortA<6, PinMode::Output | PinMode::Inverse> > ;
  using PIR0 = Smcom::SoftDrv::Input<PortF<0, PinMode::InputPullUp>, 2>;
  using Sensor0 = Smcom::SoftDrv::Input<PortA<7>, 2>;
  using Relay0 = PortF<1, PinMode::Output | PinMode::Inverse>;
  // --- PWM
  using Strip = Smcom::SoftDrv::PWM::Output<pwm_tim3, PWMMode::CH4, PinMode::PushPull>;
  using LedPWM = Smcom::SoftDrv::PWM::Output<pwm_tim1, PWMMode::CH2, PinMode::Output>;
  using LedI = Smcom::SoftDrv::PWM::Output<pwm_tim1, PWMMode::CH3, PinMode::PushPull>;
  // --- adc
  using NTC0 = ntc0;
  using Current0 = cur0;
  using Photo0 = photo0;
  // --- methods
  static void Init(smcom_funcp ptr) {
    data.handler = ptr;
    
    MCU::Init();
    Timer::Init();
    Timer::SetHandler(_Update100MSec);

    LedTx::Init();
    LedRx::Init();
    LedTx::Clr();
    LedRx::Clr();
    Relay0::Init();
    Relay0::Clr();
    // --- inputs
    PIR0::Init();
    Sensor0::Init();
    adcdisp::Init();
    // --- pwm
    LedPWM::Init();
    LedI::Init();
    Strip::Init();
    LedI::Set(100);
  }
  static void Start(void) {
    UartDriver::Start();
    Timer::Start();
    MCU::InterruptEnable();  
  }
  static void Loop(void) {
    adcdisp::Loop();
    PIR0::Check();
    Sensor0::Check();  
  }
};
template <uint16_t zxct1009_max>
typename HW0017<zxct1009_max>::Data HW0017<zxct1009_max>::data;

//------------------------------------------------------------------------------
// Application
//------------------------------------------------------------------------------
#include <smlets/node/appl/appl-proto.hpp>
#include <smcom/appl/thermostat.hpp>
#include <smcom/softdrv/timer.hpp>
#include <smcom/appl/light.hpp>

template <class Node>
class ApplHW0017 : public Smlets::Node::Appl::Proto {
protected:
  // --- data
  enum State { NA, ON, OFF };
  static struct Data {
    State       state;
    State       power;
    uint16_t    timeout;
    uint8_t     threshold;
  } data;
  // --- externals
  using ntc0 = typename Node::Hardware::NTC0;
  using cur0 = typename Node::Hardware::Current0;
  using photo0 = typename Node::Hardware::Photo0;
  using pir0 = typename Node::Hardware::PIR0;
  using sensor0 = typename Node::Hardware::Sensor0;
  using strip = typename Node::Hardware::Strip;
  using led_pwm = typename Node::Hardware::LedPWM;
  using relay = typename Node::Hardware::Relay0;
  // --- light
  using light = Smcom::Appl::Light<30, 100, 5>;
  // --- tasks
  static void Send(const char* key, int16_t value, uint8_t dot=0) {
    Smlets::Message::Queue::String2 msg(key, value, dot);
    Node::TX(msg);
  }
  static void SendTemp1() { Send("Temp0", ntc0::Read(), 1); }
  static void SendCurrent1() { Send("Current2", cur0::Read()); }
  static void SendPhoto1() { Send("Photo0", photo0::Read()); }
  static void SendSensor0() { Send("Sensor0", pir0::Get()); }
  static void SendSensor1() { Send("Sensor1", sensor0::Get()); }
  static void SendPower(uint8_t n, bool value) {
    const char* key = "";
    switch( n ) {
    case 0:
      key = "Power0";
      break;
    case 1:
      key = "Power1";
      break;
    case 2:
      key = "Power2";
      break;
    case 3:
      key = "Power3";
      break;
    }
    Smlets::Message::Queue::String2 msg(key, (value)?"ON":"OFF");
    Node::TX(msg);
  }
public:  
  static void Init() {
    data.state = ON;
    data.power = NA;
    data.timeout = 240;
    data.threshold = 50;    
    // --- light
    light::Init();
  }
  static void Start(void) {
    SendPower(0, true);
    SendPower(1, false);
    SendPower(2, false);
    SendPower(3, false);
  }
  static void Loop() {
    if( Node::RXAvailable() ) {
      Smlets::Message::MessageBase const & msg = Node::RX();
      bool executed = false;
      
      if( msg.type.data == Smlets::Message::Type::QueueString2 ) {
        
        auto msg_string2 = (Smlets::Message::Queue::String2 const &)msg;
        
        if( !strncmp(msg_string2.key, "Power", 5) ) {
          // --- Power CMD
          uint8_t ch = msg_string2.key[5];
          uint8_t value = (strcmp(msg_string2.value, "ON"))?0:100;
          switch( ch ) {
          case '0':
            data.state = (value)?ON:OFF;
            if( !value && (data.power == ON) ) {
              strip::Set(0);
              led_pwm::Set(0);
              relay::Set(0);
              SendPower(1, false);
              SendPower(2, false);
              SendPower(3, false);
              light::Loop(false, 1);
            }
            executed = true;
            break;
          case '1':
            if( data.state == OFF ) {
              strip::Set(value);
              executed = true;
            }
            break;
          case '2':
            if( data.state == OFF ) {
              led_pwm::Set(value);
              executed = true;
            }
            break;
          case '3':
            if( data.state == OFF ) {
              relay::Set(value);
              executed = true;
            }
            break;
          }
        } else if( !strncmp(msg_string2.key, "PWM", 3) ) {
          // --- PWM CMD
          uint8_t ch = msg_string2.key[3];
          uint8_t value = atoi(msg_string2.value);
          switch( ch ) {
          case '1':
            if( data.state == OFF ) {
              strip::Set(value);
              executed = true;
            }
            break;
          case '2':
            if( data.state == OFF ) {
              led_pwm::Set(value);
              executed = true;
            }
            break;
          }
        } else if( !strcmp(msg_string2.key, "Timeout0") ) {
          // --- record timeout
          uint16_t value = atoi(msg_string2.value);
          if( value >= 10 && value < 60000 ) {
            data.timeout = value;
            executed = true;
          }
        } else if( !strcmp(msg_string2.key, "Threshold0") ) {
          // --- photo sensor pthreshold
          uint16_t value = atoi(msg_string2.value);
          if( value <= 100 ) {
            data.threshold = value;
            executed = true;
          }
        } else if( !strcmp(msg_string2.key, "Status") ) {
          SendTemp1();
          SendCurrent1();
          SendPhoto1();
          SendSensor0();
          SendSensor1();
          executed = true;
        }

        if( executed ) {
          auto msg_out = msg_string2;
          Node::TX(msg_out);
        }
      }
      Node::CommitRX();
    }
  
    if( pir0::IsChanged(true) ) SendSensor0();
    if( sensor0::IsChanged(true) ) SendSensor1();
  
    if( ntc0::IsChanged(true) ) SendTemp1();
    if( cur0::IsChanged(true) ) SendCurrent1();
    if( photo0::IsChanged(true) ) SendPhoto1();
    
    if( data.state == ON ) {
      
      bool sens = pir0::Get() && (data.power == ON || photo0::Read() <= data.threshold);
      
      uint8_t pwm = light::Loop(sens, data.timeout);

      strip::Set(pwm);
      led_pwm::Set(pwm);
      relay::Set(pwm);
      
      State p_new = (pwm)?ON:OFF;
      if( p_new != data.power ) {
        SendPower(1, p_new==ON);
        data.power = p_new;
      }
    }
  }
  static void Update100MSec(void) {
    light::Update100MSec();
  }
  static void Update1Sec(void) {
    light::UpdateSec();
  }
  static void Update1Min(void) {
  }
  static Smlets::Node::Appl::AcceptState AcceptRX(const Smlets::Message::MessageBase& msg) {
    Node::Hardware::LedRx::Run(1, 1);
    return Smlets::Node::Appl::AcceptState::AcceptProceed;
  }
  static Smlets::Node::Appl::AcceptState AcceptTX(const Smlets::Message::MessageBase& msg) {
    Node::Hardware::LedTx::Run(1, 1);
    return Smlets::Node::Appl::AcceptState::AcceptProceed;
  }
};

template <class Node>
typename ApplHW0017<Node>::Data ApplHW0017<Node>::data;
