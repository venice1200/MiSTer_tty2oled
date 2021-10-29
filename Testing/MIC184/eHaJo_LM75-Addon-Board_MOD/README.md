# eHaJo_LM75

This library is fitted for the LM75B-Addon-Board from eHaJo

Please see examples for further usage of it.

# License

(C) 2019 , Elektronik Hannes Jochriem, Germany

This library is open source and released under *[MIT license](LICENSE.txt)*. Please see this file for further information.

# LM75

The chip communicates via I2C and converts the temperature every 10ms in 11bit resolution (0.125°).
The standard I2C address is 0x48, you can configure three bits (A0, A1, A2) on the Addon-Board to use up to 7 boards.
This gives an extended Address range of:
Binary: 0 1 0 0  1 A2 A1 A0
Hex: 0x48 to 0x4F

# Power-up condition

After power-up following register settings are default:
 - Normal operation mode
 - OS comparator mode
 - T<sub>th(ots)</sub> = 80°C
 - T<sub>hys</sub> = 75°C
 - OS output active state is *LOW*
 - Pointer value is logic 00 (Temp)

# Functions in the library

## void setAddress(uint8_t bit_A0, uint8_t bit_A1, uint8_t bit_A2)

Set the address if you use something different for A0-A2
	
## float getTemp()

Returns the temperature as float value in °C.

## float CtoF(float temperatureC)

You can use this to convert the temperature from °C to °F (I have no idea why you should to this...)

## float getTOS()

Returns the Temperature overshoot temperature stored in the Tos-register.

## void setTOS(float temperature)

Sets the Temperature overshoot temperature. This register is compared with the temp-register after each conversion to set OS-pin.

## float getHyst()

Returns the Hysteresis temperature stored in Thyst-register

## void setHyst(float temperature)

Sets the Hysteresis temperature register. This register is compared after each conversion, if temp-register < Thyst than OS-pin is set.

## void setMode(LM75_opmode opmode)

This is used to send the device in Shutdown-mode. 
Available LM75_opmodes are:
 - LM75_MODE_NORMAL (default)
 - LM75_MODE_SHUTDOWN

## void setOSMode(LM75_OS_opmode opmode)

The OS-opmode defines how the device handles Tos and Thyst events. 
Available LM75_OS_opmodes are:
 - LM75_OS_COMPARATOR (default)
 - LM75_OS_INTERRUPT
 
## void setTOSPolarity(LM75_TOSPolarity polarity)

Defines if the polarity of OS-pin is high or low after TOS-event.
Available LM75_TOSPolarities are:
 - LM75_OSPOL_LOW (default)
 - LM75_OSPOL_HIGH

## void setOSFaultQueue(LM75_OS_Fault_queue numberfaults)

The fault queue is defined as the number of faults that must occur consecutively to activate the OS output.
Availabel LM75_OS_Fault_queues are:
 - LM75_FAULTS_1
 - LM75_FAULTS_2
 - LM75_FAULTS_4
 - LM75_FAULTS_6