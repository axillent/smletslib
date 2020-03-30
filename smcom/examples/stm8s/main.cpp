/*
 * example_mix_01.cpp
 *
 * Created: 25/09/2017 15:07:19
 * Author : abudnik
 */

#define STM8S005K6

#include <stavrp/stavrp.h>
#include <stavrp/timer.h>

void Update50MSec(void);
void UpdateSec(void);
void ADCFunc(uint8_t channel, uint16_t value);

#include <stavrp/stm8s/gpio.h>
#include <stavrp/stm8s/eeprom.h>
#include <stavrp/stm8s/timer1.h>
//#include <stavrp/stm8s/uart0.h>
//#include <stavrp/stm8s/uart1.h>
//#include <stavrp/stm8s/delay.h>
//#include <stavrp/stm8s/spi.h>

using namespace STAVRP::STM8S;

// --- MCU config
typedef MCU<MCUClock::HSI_16MHz> mcu;

// --- Timer, 8mhz / 256 / 256 = 122 Hz
typedef Timer1<mcu, 20>																			timer1ctrl;
// --- precount Call class to call UpdateSec each 20th timer interrupt, about once per second
typedef STAVRP::Timer::Call<UpdateSec, 20>														update_sec;
// --- Universal timer about each 1 seconds interrupt to UpdateSec function
typedef STAVRP::Timer::Universal<mcu::cpu_f/256/256/20, timer1ctrl, Update50MSec, update_sec>	timer;
// --- SPI
//typedef SPIDriver<SPIMode::Master, SPISpeed::Div4, SPIOpt::None>	spi;

// --- common
#include <stavrp/adc.h>
#include <stavrp/button.h>
#include <stavrp/blink.h>
#include <stavrp/buffer.h>
//#include <stavrp/data.h>

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

//typedef UART0<mcu, 9600, 8, 4>		uart0;
//typedef UART1<mcu, 9600, 8, 4>		uart1;

__eeprom int e_test1;
__eeprom int e_test2;

//Data::Storage<int, EEpromAccessor> test1((int*)&e_test1);
//Data::Storage<int, EEpromAccessor> test2((int*)&e_test2);

int main(void)
{
	led_pin::Set();

	mcu::Init();

//	test1 = 0;
//	test2 = 5;
//	test1 = test2;

	button::Init();
	led::Init();
	timer::Start();
//	uart0::Start();
//	uart1::Start();

//	spi::Start();
//	spi::Transfer(55);

	fifo.Init();

//	uart0::TXEmpty();
//	uart0::RXAvailable();
//	uart1::TXEmpty();
//	uart1::RXAvailable();

	adc::Start();

	mcu::InterruptEnable();

//	uart0::PutStr("-- started\n");

//	delay::Microseconds(5);
//	delay::Milliseconds(5);

//	uart1::PutStr("-- started\n");

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