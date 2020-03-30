#include <stavrp/stm32/f0/mcu.h>
#include <stavrp/stm32/f0/gpio.h>
//#include <stavrp/stm32/f0/uart1.h>

#include <stavrp/stm32/f0/timer1.h>
//#include <stavrp/stm32/f0/timer3.h>
//#include <stavrp/stm32/f0/timer14.h>
//#include <stavrp/stm32/f0/timer16.h>
//#include <stavrp/stm32/f0/timer17.h>

#include <stavrp/blink.h>
#include <stavrp/adc.h>
#include <stavrp/timer.h>

void adcfunc(uint8_t channel, uint16_t value) {}

using namespace STAVRP;
using namespace STAVRP::STM32;

void timerupdate();

typedef MCU<8000000UL>																mcu;
typedef Timer1<mcu, 10>																timer1;
typedef Timer::Universal<1, timer1, timerupdate>			timer;

typedef PortA<0, PinMode::Output | PinMode::PushPull> led;
typedef Blink<led>																		led_blink;
	
//typedef ADC::ADC<ADC::Channel<0, adcfunc>, 1> adc;
//typedef UART1<mcu, 9600, 8, 4>		uart1;

int main(void) {

	led_blink::Init();
	
//	adc::Start();
//	uart1::Start();

	timer::Start();
	
	mcu::InterruptEnable();
	
//	uart1::PutStr("-- Hello World \n");

	led_blink::Run(255, 10);
	
	while(1) {
	}
	
}

void timerupdate() {
	led_blink::TimerUpdate();
}
