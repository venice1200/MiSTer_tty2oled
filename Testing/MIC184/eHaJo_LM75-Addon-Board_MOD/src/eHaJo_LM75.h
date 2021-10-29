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
// Temperatures given in Celsius, use CtoF() to convert!
//
// ***************************

#ifndef EHAJO_LM75_H
#define EHAJO_LM75_H

#include <Arduino.h>
#include <Wire.h>

#define LM75_BASE_ADDRESS 0x48			// Base address 0b 1 0 0 1 A2 A1 A0
#define LM75_REGISTER_TEMP			0	// Temperatur store register (RO)
#define LM75_REGISTER_CONF			1	// Config register
#define LM75_REGISTER_THYST			2	// Hysteresis register
#define LM75_REGISTER_TOS			3	// Overtemperature shutdown register

#define LM75_CONF_OS_COMP_INT		1	// OS operation mode selection
#define LM75_CONF_OS_POL			2	// OS polarity selection
#define LM75_CONF_OS_F_QUE			3	// OS fault queue programming

enum LM75_OS_opmode : uint8_t
{
	LM75_OS_COMPARATOR = (0<<1),
	LM75_OS_INTERRUPT = (1<<1)
};

enum LM75_OS_Fault_queue : uint8_t
{
	LM75_FAULTS_1 = (0<<3),
	LM75_FAULTS_2 = (1<<3),
	LM75_FAULTS_4 = (2<<3),
	LM75_FAULTS_6 = (3<<3)
};

enum LM75_TOSPolarity : uint8_t
{
	LM75_OSPOL_LOW =  (0<<2),
	LM75_OSPOL_HIGH = (1<<2)
};

enum LM75_opmode : uint8_t
{
	LM75_MODE_NORMAL = 0,
	LM75_MODE_SHUTDOWN = 1
};

enum LM75_zone : uint8_t
{
	LM75_ZONE_INTERNAL = (0<<5),
	LM75_ZONE_REMOTE   = (1<<5)
};

class EHAJO_LM75
{
	// private stuff
	uint8_t _lm75_address;
	uint8_t read8bit(const uint8_t address);
	uint16_t read16bit(const uint8_t address);
	void write16bit(const uint8_t address, const uint16_t value);
	void write8bit(const uint8_t address, const uint8_t value);
	uint8_t getConfRegister();
	void setConfRegister(uint8_t conf);

public:
	// constructor
	EHAJO_LM75();
	
	// public stuff
	
	// I2C
	void setAddress(uint8_t bit_A0, uint8_t bit_A1, uint8_t bit_A2);
	
	// temperatures
	float getTemp();
	float CtoF(float temperatureC);
	float getTOS();
	void setTOS(float temperature);
	float getHyst();
	void setHyst(float temperature);
	void setOSFaultQueue(LM75_OS_Fault_queue numberfaults);

	
	// config register stuff:
	void setMode(LM75_opmode opmode);
	void setOSMode(LM75_OS_opmode opmode);
	void setTOSPolarity(LM75_TOSPolarity polarity);
	void setZONE(LM75_zone zone);
	
};

#endif // EHAJO_LM75_H
