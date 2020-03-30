/*
 * example_mix_01.cpp
 *
 * Created: 25/09/2017 15:07:19
 * Author : abudnik
 */ 

#include <stavrp/stavrp.h>
#include <stavrp/timer.h>

void Update50MSec(void);
void UpdateSec(void);
void ADCFunc(uint8_t channel, uint16_t value);

// --- platform specific
#ifdef _STAVR_PLATFORM_AVR_

	#include <stavrp/avr8/gpio.h>
	#include <stavrp/avr8/eeprom.h>
	#include <stavrp/avr8/timer0.h>
	#include <stavrp/avr8/uart0.h>
	#include <stavrp/avr8/uart1.h>
	#include <stavrp/avr8/delay.h>
	#include <stavrp/avr8/spi.h>

	using namespace STAVRP::AVR8;

	// --- MCU config
	typedef MCU<8000000UL, CPUDiv::Div1> mcu;
	
	// --- Timer, 8mhz / 256 / 256 = 122 Hz
	typedef Timer0<mcu, 0, Timer0Prescaler::Div256>													timer0ctrl;
	// --- precount Call class to call UpdateSec each 20th timer interrupt, about once per second
	typedef STAVRP::Timer::Call<UpdateSec, 20>														update_sec;
	// --- Universal timer about each 1 seconds interrupt to UpdateSec function
	typedef STAVRP::Timer::Universal<mcu::cpu_f/256/256/20, timer0ctrl, Update50MSec, update_sec>	timer;
	// --- programm delay
	typedef Delay<mcu>		delay;
	// --- SPI
	typedef SPIDriver<STAVRP::SPIMode::Master, SPISpeed::Div4, SPIOpt::None>	spi;
	
#elif defined(_STAVR_PLATFORM_STM8S_)

	#include <stavrp/stm8s/gpio.h>
	using namespace STAVRP::STM8S;

#endif

// --- common
#include <stavrp/adc.h>
#include <stavrp/button.h>
#include <stavrp/blink.h>
#include <stavrp/buffer.h>

using namespace STAVRP;

// --- ADC
//typedef STAVRP::ADC::ADCSingle<0, 1>							adc;
typedef STAVRP::ADC::ADC<STAVRP::ADC::Channel<0, ADCFunc>, 1>	adc;

// --- simple GPIO
typedef PortB<1, PinMode::Output>			led_pin;
typedef PortB<2, PinMode::Inverse | PinMode::InputPullUp>	button_pin;

// --- button linked to GPIO
typedef STAVRP::Button<button_pin>	button;

// --- led blinking linked to GPIO
typedef STAVRP::Blink<led_pin>		led;

// --- FIFO buffer
STAVRP::Fifo<uint8_t, uint8_t, 16>			fifo;

typedef UART0<mcu, 9600, 8, 4>		uart0;
typedef UART1<mcu, 9600, 8, 4>		uart1;

EEPROM<int, 0>	test1;
EEPROM<int, 1>	test2;

typedef	EEPROMKey<>	ep_key;
EEPROMValue<ep_key, int, 2>	test3(10);

int main(void)
{
	test1 = 0;
	test2 = 5;
	test1 = test2;

	mcu::Init();
	button::Init();
	led::Init();
	timer::Start();
	uart0::Start();
	uart1::Start();

	spi::Start();
	spi::Transfer(55);

	fifo.Init();

	uart0::TXEmpty();
	uart0::RXAvailable();
	uart1::TXEmpty();
	uart1::RXAvailable();
	
	adc::Start();
	
	mcu::InterruptEnable();

	uart0::PutStr("-- started\n");
	
	delay::Microseconds(5);
	delay::Milliseconds(5);

	uart1::PutStr("-- started\n");
	
    while (1) 
    {
		button::Check();
		
		if( button::PressedSingle() ) {
			button::Clear();
			led::Run(2, 1, 2);
		}
		
		mcu::InterruptWait();
    }
}

void UpdateSec() {
	led::TimerUpdate();
}

void Update50MSec() {
	button::TimerUpdate();
}

void ADCFunc(uint8_t channel, uint16_t value) {
	
}