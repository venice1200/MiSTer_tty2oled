// ***************************
// 
// Get Temperature and send device to sleep afterwards
// 
// LM75-Addon-Board
//
// (C) 2019, Elektronik Hannes Jochriem, Germany
// 
// Licensed under the MIT-license, see LICENSE.txt for further information.
//
// ***************************

#include <eHaJo_LM75.h>

EHAJO_LM75 tempsensor;

void setup() {
	while(!Serial) {}
	Serial.begin(9600);

	Wire.begin();  // start I2C-stuff

	Serial.println("eHaJo Powerdown test");
}

void loop() {
	
	tempsensor.setMode(LM75_MODE_NORMAL);	// wake up nero
	delay(15); // The LM75 needs about 10ms for a conversion
	Serial.print("Temperature = ");
	Serial.print(tempsensor.getTemp());
	Serial.println(" C");
	tempsensor.setMode(LM75_MODE_SHUTDOWN);	// get some sleep!
	delay(1000);
}