// ***************************
// 
// Get temperature from LM75-Board
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
  Wire.begin();	// start I2C-stuff
  
  Serial.println("eHaJo Temperature Sensor test");
}

void loop() {
  Serial.print("Temperature = ");
  Serial.print(tempsensor.getTemp());
  Serial.println(" C");

  delay(200);
}