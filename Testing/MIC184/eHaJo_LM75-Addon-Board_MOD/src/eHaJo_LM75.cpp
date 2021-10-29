// ***************************
// 
// Arduino library for 
// 
// LM75-Addon-Board
//
// (C) 2019, Elektronik Hannes Jochriem, Germany
// 
// Licensed under the MIT-license, see LICENSE.txt for further information.
//
// ***************************

#include "eHaJo_LM75.h"


// ***************************
// 
// Constructor

EHAJO_LM75::EHAJO_LM75()
{	// Set standard address
	_lm75_address = LM75_BASE_ADDRESS;
}

// ***************************
// 
// public stuff

float EHAJO_LM75::getTemp()
{	// get the temperature from the device
	uint16_t t;
	t = read16bit(LM75_REGISTER_TEMP);
	return (float)t / 256.0f;
}

float EHAJO_LM75::CtoF(float temperatureC)
{	// convert to Fahrenheit for "who the hell still use this?"
	return (temperatureC * 1.8f) + 32.0f;
}

void EHAJO_LM75::setAddress(uint8_t bit_A0, uint8_t bit_A1, uint8_t bit_A2)
{	// Change address according to A0-A2 set on PCB
	_lm75_address = LM75_BASE_ADDRESS | ((bit_A0 & 0b00000001)<<0) | ((bit_A1 & 0b00000001)<<1) | ((bit_A2 & 0b00000001)<<2);
}

void EHAJO_LM75::setMode(LM75_opmode opmode)
{	// send the lil SO-8 to bed or wake him up
	uint8_t t = getConfRegister() & ~(LM75_MODE_SHUTDOWN);
	setConfRegister(t | opmode);
}

void EHAJO_LM75::setOSMode(LM75_OS_opmode opmode)
{	// define the OS-pin mode as INT or COMP
	uint8_t t = getConfRegister() & ~(LM75_OS_INTERRUPT);
	setConfRegister(t | opmode);
}

void EHAJO_LM75::setTOSPolarity(LM75_TOSPolarity polarity)
{	// Set if OSpin is high or low on TOS
	uint8_t t = getConfRegister() & ~(LM75_OSPOL_HIGH);
	setConfRegister(t | polarity);
}

void EHAJO_LM75::setOSFaultQueue(LM75_OS_Fault_queue numberfaults)
{	// Set how many faults must occur to set OS-output
	uint8_t t = getConfRegister() & ~(LM75_FAULTS_6);
	setConfRegister(t | numberfaults);
}
	
void EHAJO_LM75::setZONE(LM75_zone zone)
{	// // set temperatur selection zone to internal or remote
	uint8_t t = getConfRegister();                   // Read Config
	setConfRegister(t | 0b01000000);                 // Set IM Bit
	t = getConfRegister() & ~(LM75_ZONE_REMOTE);     // Read Config with Mask 0b00100000
	setConfRegister(t | zone);                       // Write Zone Value
}
	
float EHAJO_LM75::getTOS()
{	// Get Overtemperature shutdown Register
	uint16_t t;
	t = read16bit(LM75_REGISTER_TOS);
	return (float)t / 256.0f;
}

void EHAJO_LM75::setTOS(float temperature)
{
	write16bit(LM75_REGISTER_TOS, (uint16_t)(temperature*256.0));
}

float EHAJO_LM75::getHyst()
{	// Get Hysteresis temperature register
	uint16_t t;
	t = read16bit(LM75_REGISTER_THYST);
	return (float)t / 256.0f;
}

void EHAJO_LM75::setHyst(float temperature)
{
	write16bit(LM75_REGISTER_THYST, (uint16_t)(temperature*256.0));
}

	

// ***************************
// 
// private stuff

uint8_t EHAJO_LM75::read8bit(uint8_t address)
{
	Wire.beginTransmission(_lm75_address);
	Wire.write(address);
	Wire.endTransmission();
	Wire.requestFrom(_lm75_address, (uint8_t)1);
	return Wire.read();
}

uint16_t EHAJO_LM75::read16bit(uint8_t address)
{
	Wire.beginTransmission(_lm75_address);
	Wire.write(address);
	Wire.endTransmission();
	
	Wire.requestFrom(_lm75_address, (uint8_t)2);
	uint16_t t = Wire.read() << 8;
	t |= Wire.read();
	return t;
}

void EHAJO_LM75::write16bit(uint8_t address, uint16_t value)
{
	Wire.beginTransmission(_lm75_address);
	Wire.write(address);
	Wire.write((uint8_t)((value & 0xff00) >> 8));
	Wire.write((uint8_t)(value & 0x00ff));
	Wire.endTransmission();
}

void EHAJO_LM75::write8bit(uint8_t address, uint8_t value)
{
	Wire.beginTransmission(_lm75_address);
	Wire.write(address);
	Wire.write(value);
	Wire.endTransmission();
}

uint8_t EHAJO_LM75::getConfRegister()
{
	return read8bit(LM75_REGISTER_CONF);
}

void EHAJO_LM75::setConfRegister(uint8_t conf)
{
	write8bit(LM75_REGISTER_CONF, conf);
}