#ifndef STAVRP_STM8S_I2C_H
#define STAVRP_STM8S_I2C_H

#include "gpio.hpp"

namespace STAVRP {	namespace STM8S {

template <class Mcu>
class I2C {
protected:
	static const uint32_t freqr = Mcu::cpu_f / 1000000UL;
	static const uint32_t ccr = Mcu::cpu_f / 2 / 100000UL;
	static const uint32_t tbase = Mcu::cpu_f / 1000UL;
	typedef PortB<4>	SCL;
	typedef PortB<5>	SDA;
public:
	static void Init() {
		SCL::Init();
		SDA::Init();

		I2C_CR1_bit.PE	= 0;		// disable I2C

		I2C_FREQR	= freqr;		// setting frequency in MHz
		I2C_CCRH	= (ccr >> 8);	// setting 100kHz
		I2C_CCRL	= ccr & 0xff;
		I2C_TRISER	= freqr + 1;
		I2C_CR2_bit.ACK	= 1;

		I2C_CR1_bit.PE	= 1;		// enable I2C
	}
	// --- send START
	static void SendStart() {
		for(uint32_t count=tbase*10; I2C_SR3_BUSY && count; count--) {}		// wait for bus to be free
		I2C_CR2_START = 1;													// sending START
		for(uint32_t count=tbase; !I2C_SR1_SB && count; count--) {}			// START was send
	}
	// --- send START
	static void SendStop() {
		I2C_CR2_STOP = 1;													// sending STOP
        for(uint32_t count=tbase; I2C_CR2_STOP && count; count--) {}		// STOP was send
	}
	// --- send address
	static void SendAddress(uint8_t address) {
		I2C_DR = address & 0xFE;											// sending device address
		for(uint32_t count=tbase; !I2C_SR1_ADDR && count; count--) {}		// wait
		volatile uint8_t x = I2C_SR3;										// clearing ADDR by readin SR3
	}
	// --- send byte
	static void SendByte(uint8_t b) {
		for(uint32_t count=tbase; !I2C_SR1_TXE && count; count--) {}		// wait
		I2C_DR = b;													// sendign data address
	}
	// --- send single byte
	static void SendData(uint8_t address, uint8_t data) {

		SendStart();
		SendAddress(address);
		SendByte(data);
		SendStop();

    }
	static void RegWrite(uint8_t address, uint8_t reg_addr,	const uint8_t* data, uint8_t length) {

		SendStart();
		SendAddress(address);
		SendByte(reg_addr);

		// --- sending data
		while( length-- ) SendByte(*data++);

        for(uint32_t count=tbase; !(I2C_SR1_TXE && I2C_SR1_BTF) && count; count--) {}

        SendStop();

	}
	static void RegRead(uint8_t address, uint8_t reg_addr,	uint8_t* data, uint8_t length) {

		I2C_CR2_ACK = 1;				// allow ack

		SendStart();
		SendAddress(address);
		SendByte(reg_addr);

		for(uint32_t count=tbase*1; !(I2C_SR1_TXE && I2C_SR1_BTF) && count; count--) {}

		SendStart();

		I2C_DR = address | 0x01;												// recording device address and prepeare reading

		// reading depends on number of reading bytes
		if(length == 1){

			I2C_CR2_ACK = 0;													// OFF ACK

			for(uint32_t count=tbase*1; !I2C_SR1_ADDR && count; count--) {}

			Mcu::InterruptDisable();											// work arround from errata
			volatile uint8_t x = I2C_SR3;										// clearing ADDR by readin SR3

			I2C_CR2_STOP = 1;													// sending STOP
			Mcu::InterruptEnable();												// work arround from errata

			for(uint32_t count=tbase*1; !I2C_SR1_RXNE && count; count--) {}

			*data = I2C_DR;														// reading byte

		} else if(length == 2){

			I2C_CR2_POS = 1;													// allow NACK on next byte
			for(uint32_t count=tbase*1; !I2C_SR1_ADDR && count; count--) {}

			Mcu::InterruptDisable();											// work arround from errata
			volatile uint8_t x = I2C_SR3;										// clearing ADDR by readin SR3

			I2C_CR2_ACK = 0;													// OFF ACK
			Mcu::InterruptEnable();												// work arround from errata

			for(uint32_t count=tbase*1; !I2C_SR1_BTF && count; count--) {}

			Mcu::InterruptDisable();											// work arround from errata
			I2C_CR2_STOP = 1;													// sending STOP

			*data++ = I2C_DR;													// reading data
			Mcu::InterruptEnable();												// work arround from errata
			*data = I2C_DR;														// reading data

		} else if(length > 2){

			for(uint32_t count=tbase*1; !I2C_SR1_ADDR && count; count--) {}

			Mcu::InterruptDisable();											// work arround from errata
			volatile uint8_t x = I2C_SR3;										// clearing ADDR by readin SR3

			Mcu::InterruptEnable();												// work arround from errata

			while( length-- > 3 ){
				for(uint32_t count=tbase*1; !I2C_SR1_BTF && count; count--) {}
				*data++ = I2C_DR;
			}

			// reading last 3 bytes
			for(uint32_t count=tbase*1; !I2C_SR1_BTF && count; count--) {}

			I2C_CR2_ACK = 0;													// OFF ACK

			Mcu::InterruptDisable();											// work arround from errata

			*data++ = I2C_DR;
			I2C_CR2_STOP = 1;
			*data++ = I2C_DR;

			Mcu::InterruptEnable();												// work arround from errata

			for(uint32_t count=tbase*1; !I2C_SR1_RXNE && count; count--) {}

			*data++ = I2C_DR;

		}

        for(uint32_t count=tbase; I2C_CR2_STOP && count; count--) {}
		I2C_CR2_POS = 0;														// clear POS

	}
};

}}

#endif
