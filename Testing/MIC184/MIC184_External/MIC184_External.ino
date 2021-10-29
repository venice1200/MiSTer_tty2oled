/*
Testing MIC184 Extrnal Zone
*/
  
#include <eHaJo_LM75.h>          // << Extra Library, via Arduino Library Manager
#define I2C1_SDA 17              // I2C_1-SDA
#define I2C1_SCL 16              // I2C_1-SCL
EHAJO_LM75 tSensor;              // Create Sensor Class

void setup() {
  Serial.begin(115200);                      // 115200 for MiSTer ttyUSBx Device CP2102 Chip on ESP32
  Serial.flush();                            // Wait for empty Send Buffer
  Serial.setTimeout(500);                    // Set max. Serial "Waiting Time", default = 1000ms
  Wire.begin(I2C1_SDA, I2C1_SCL, 100000);  // Setup I2C-1 Port
}

void loop() {
  Serial.println("Set Sensor to Internal Zone");
  tSensor.setZONE(LM75_ZONE_INTERNAL);     // Internal = Standard/Default
  for (int i=10; i>0; i--) {
    delay(1000);
    Serial.print("Temperature = ");
    Serial.print(tSensor.getTemp());
    Serial.println("°C");
    Serial.println(i);
  }

  Serial.println("Set Sensor to Remote Zone");
  tSensor.setZONE(LM75_ZONE_REMOTE);       // Remote = External using MMBT3906
  for (int i=10; i>0; i--) {
    delay(1000);
    Serial.print("Temperature = ");
    Serial.print(tSensor.getTemp());
    Serial.println("°C");
    Serial.println(i);
  }
  
}
