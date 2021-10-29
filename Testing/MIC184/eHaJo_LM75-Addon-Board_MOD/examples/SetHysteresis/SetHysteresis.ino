// ***************************
// 
// Set Hysteresis and Temperatur Overshoot
// Connect OS-Pin to Arduino pin 2
// LED goes on if temp>35°C and out if temp<30°C
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

	// config the Input
	pinMode(2, INPUT_PULLUP);
	pinMode(LED_BUILTIN, OUTPUT);

	Wire.begin();  // start I2C-stuff

	tempsensor.setTOS(35.0);  // Temperatur Overshoot at 35°C
	tempsensor.setHyst(30.0); // Hysteresis at 30°C
	tempsensor.setTOSPolarity(LM75_OSPOL_HIGH); // OS High if temp>TOS
	tempsensor.setOSMode(LM75_OS_COMPARATOR); // Comparator mode

	Serial.println("eHaJo TOS and Hysteresis test");
}

void loop() {
	Serial.print("Temperature = ");
	Serial.print(tempsensor.getTemp());
	Serial.println(" C");

	if (digitalRead(2) == HIGH) {
		digitalWrite(LED_BUILTIN, HIGH);
	} else {
		digitalWrite(LED_BUILTIN, LOW);
	}

	delay(200);
}