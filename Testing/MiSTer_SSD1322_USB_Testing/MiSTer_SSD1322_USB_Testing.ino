/*
  By Venice
  Get CORENAME from MiSTer via USB-Serial-TTY Device and show CORENAME related text, Pictures or Logos
  Using forked Adafruit SSD1327 Library https://github.com/adafruit/Adafruit_SSD1327 for the SSD1322

  -- G R A Y S C A L E  E D I T I O N --

  Needed libraries for the Arduino program:
  - Adafruit GFX (*)
  - U8G2 for Adafruit GFX (*)
  - Bounce2 (*) optional, needed for the tilt-sensor
  - eHaJo_LM75 (*) optional, needed for the MIC145 sensor on d.ti's tty2oled board
  - SSD1322 for Adafruit GFX, download and extract from here: https://github.com/venice1200/SSD1322_for_Adafruit_GFX/releases/latest
  (*) These Libraries can be installed using Arduino's library manager.
  See also https://github.com/venice1200/MiSTer_tty2oled/wiki/Arduino-HowTo-%28Windows%29

  QuickSelect/Copy&Paste for Arduino IDE v2.x:
  -ESP32 Dev Module
  -WEMOS LOLIN32
  -NodeMCU 1.0
  
  See changelog.md in Sketch folder for more details

  2021-10-22
  -d.ti Board only: If you sent "TEP184" via Command "CMDTEXT" the Text will be replaced with the Temperature Sensor Value and "°C"
   
  ToDo
  -Everything I forgot
  -Add u8g2_font_7Segments_26x42_mn
   
*/

// Set Version
#define BuildVersion "211021T"                    // "T" for Testing, "G" for Grayscale, "U" for U8G2 for Adafruit GFX

// Include Libraries
#include <Arduino.h>
#include <SSD1322_for_Adafruit_GFX.h>             // SSD1322 Controller Display Library https://github.com/venice1200/SSD1322_for_Adafruit_GFX
#include <U8g2_for_Adafruit_GFX.h>                // U8G2 Font Engine for Adafruit GFX  https://github.com/olikraus/U8g2_for_Adafruit_GFX
#include "logo.h"                                 // The Pics in XMB Format

// OTA and Reset only for ESP32
#ifdef ESP32
  #include "cred.h"              // Load your WLAN Credentials for OTA
  #include <WiFi.h>
  #include <ESPmDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  bool OTAEN=false;              // Will be set to "true" by Command "CMDENOTA"
#endif

// ---------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------- System Config -------------------------------------------------------
// ------------------------------------------- Activate your Options ---------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

// Comment (add "//" in front of the Option) to de-activate the Option
// Uncomment (remove "//" in front of the Option) to activate the Option

// Get Debugging Infos over Serial
//#define XDEBUG

// Uncomment for 180° StartUp Rotation (Display Connector up)
//#define XROTATE

// Uncomment for "Send Acknowledge" from tty2oled to MiSTer, need "waitfortty"
#define XSENDACK

// Uncomment for Tilt-Sensor based Display-Auto-Rotation. 
// The Sensor is connected to Pin 32 (with software activated Pullup) and GND.
#define XTILT
#ifdef XTILT
  #include <Bounce2.h>                     // << Extra Library, via Arduino Library Manager
  #define TILT_PIN 32                      // Tilt-Sensor Pin
  #define DEBOUNCE_TIME 25                 // Debounce Time
  Bounce RotationDebouncer = Bounce();     // Create Bounce class
#endif

// Uncomment for Temperatur Sensor Support MIC184 on d.ti's PCB
#define XDTI
#ifdef XDTI
  #include <eHaJo_LM75.h>          // << Extra Library, via Arduino Library Manager
  #define I2C1_SDA 17              // I2C_1-SDA
  #define I2C1_SCL 16              // I2C_1-SCL
  EHAJO_LM75 tSensor;              // Create Sensor Class
  #define USER_LED 19              // USER_LED
#endif

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------- Auto-Board-Config via Arduino IDE Board Selection --------------------------------------
// -------------------------------- Make sure the Manual-Config is not active ------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

#ifdef ARDUINO_ESP32_DEV
  #define USE_TTGOT8             // TTGO-T8, tty2oled Board by d.ti. Set Arduino Board to "ESP32 Dev Module", chose your xx MB Flash
#endif

#ifdef ARDUINO_LOLIN32
  #define USE_LOLIN32            // Wemos LOLIN32, LOLIN32, DevKit_V4. Set Arduino Board to "WEMOS LOLIN32"
#endif

#if defined(ARDUINO_ESP8266_NODEMCU) || defined(ARDUINO_ESP8266_NODEMCU_ESP12E)
  #define USE_NODEMCU            // ESP8266 NodeMCU v3. Set Arduino Board to "NodeMCU 1.0 (ESP-12E Module)"
#endif

// ---------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------ Manual-Board-Config ------------------------------------------------
// ------------------------------------ Make sure the Auto-Board-Config is not active ----------------------------------
// ---------------------------------------------------------------------------------------------------------------------

//#define USE_TTGOT8             // TTGO-T8. Set Arduino Board to ESP32 Dev Module, xx MB Flash, def. Part. Schema
//#define USE_LOLIN32            // Wemos LOLIN32, LOLIN32, DevKit_V4. Set Arduino Board to "WEMOS LOLIN32"
//#define USE_NODEMCU            // ESP8266 NodeMCU v3. Set Arduino Board to NodeMCU 1.0 (ESP-12E Module)

// ------------ Display Objects -----------------
// TTGO-T8 using VSPI SCLK = 18, MISO = 19, MOSI = 23 and...
#ifdef USE_TTGOT8
  #define OLED_CS 26
  #define OLED_DC 25
  #define OLED_RESET 27
#endif

// WEMOS LOLIN32/Devkit_V4 using VSPI SCLK = 18, MISO = 19, MOSI = 23, SS = 5 and...
#ifdef USE_LOLIN32
  #define OLED_CS 5
  #define OLED_DC 16
  #define OLED_RESET 17
#endif

// ESP8266-Board (NodeMCU v3)
#ifdef USE_NODEMCU
  #define OLED_CS 15
  #define OLED_DC 4
  #define OLED_RESET 5
#endif

// Hardware Constructor OLED Display
Adafruit_SSD1322 oled(256, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS);
U8G2_FOR_ADAFRUIT_GFX u8g2;

// -------------------------------------------------------------
// ------------------------- Variables -------------------------
// -------------------------------------------------------------

// Strings
String newCommand = "";                // Received Text, from MiSTer without "\n" currently (2021-01-11)
String prevCommand = "";
String actCorename = "No Core loaded"; // Actual Received Corename
uint8_t contrast = 5;                  // Contrast (brightness) of display, range: 0 (no contrast) to 255 (maximum)
//char *newCommandChar;
bool updateDisplay = false;

// Display Vars
uint16_t DispWidth, DispHeight, DispLineBytes1bpp, DispLineBytes4bpp;
unsigned int logoBytes1bpp=0;
unsigned int logoBytes4bpp=0;
const int cDelay=25;                          // Command Delay in ms for Handshake
size_t bytesReadCount=0;
uint8_t *logoBin;                             // <<== For malloc in Setup
enum picType {NONE, XBM, GSC};                // Enum Picture Type
int actPicType=0;
int16_t xs, ys;
uint16_t ws, hs;
const uint8_t minEffect=1, maxEffect=10;      // Min/Max Effects for Random
//const uint8_t minEffect=9, maxEffect=9;      // Min/Max Effects for Random

// Blinker 500ms Interval
const long interval = 500;                   // Interval for Blink (milliseconds)
bool blink = false;
bool prevblink = false;
bool blinkpos = false;  // Pos Flanc
bool blinkneg = false;  // Neg Flanc
unsigned long previousMillis = 0;
const int minInterval = 60;                   // Interval for Timer
int timer=0;                                  // Counter for Timer
bool timerpos;                                // Positive Timer Signal

// =============================================================================================================
// ================================================ SETUP ======================================================
// =============================================================================================================
void setup(void) {
  // Init Serial
  Serial.begin(115200);                      // 115200 for MiSTer ttyUSBx Device CP2102 Chip on ESP32
  Serial.flush();                            // Wait for empty Send Buffer
  Serial.setTimeout(500);                    // Set max. Serial "Waiting Time", default = 1000ms

  randomSeed(analogRead(34));                // Init Random Generator with empty Port Analog value

  // Init Display SSD1322
  oled.begin();
  oled.clearDisplay();
  oled.setRotation(0);
  oled.setContrast(contrast);                       // Set contrast of display
  oled.setTextSize(1);
  oled.setTextColor(SSD1322_WHITE, SSD1322_BLACK);  // White foreground, black background
  //oled.setFont(&FreeSans9pt7b);                   // Set Standard Font (available in 9/12/18/24 Pixel)
  oled.cp437(true);                                 // Enable Code Page 437-compatible charset (bugfix)

  // Init U8G2 for Adafruit GFX
  u8g2.begin(oled); 
  //u8g2.setFontMode(1);                             // Transpartent Font Mode, Background is transparent
  u8g2.setFontMode(0);                               // Non-Transpartent Font Mode, Background is overwritten
  u8g2.setForegroundColor(SSD1322_WHITE);            // apply Adafruit GFX color
  //u8g2.setBackgroundColor(SSD1322_BLACK);

   // Get Display Dimensions
  DispWidth = oled.width();
  DispHeight = oled.height();
  DispLineBytes1bpp = DispWidth / 8;                       // How many Bytes uses each Display Line at 1bpp
  DispLineBytes4bpp = DispWidth / 2;                       // How many Bytes uses each Display Line at 4bpp
  logoBytes1bpp = DispWidth * DispHeight / 8;              // 2048
  logoBytes4bpp = DispWidth * DispHeight / 2;              // 8192
  logoBin = (uint8_t *) malloc(logoBytes4bpp);             // Create Picture Buffer, better than create (malloc) & destroy (free)

// Activate Options

// 180° Rotation
#ifdef XROTATE
  oled.setRotation(2);
#endif

// Setup d.to Board (Temp.Sensor/USER_LED)
#ifdef XDTI
  pinMode(USER_LED, OUTPUT);
  Wire.begin(I2C1_SDA, I2C1_SCL, 100000);  // Setup I2C-1 Port
#ifdef XDEBUG
  Serial.print("Temperature = ");
  Serial.print(tSensor.getTemp());
  Serial.print("°C");
#endif
#endif

// Tilt Sensor
#ifdef XTILT
  // Setup Tilt-Sensor Input Pin
  RotationDebouncer.attach(TILT_PIN,INPUT_PULLUP);     // Attach the debouncer to a pin with INPUT mode
  RotationDebouncer.interval(DEBOUNCE_TIME);               // Use a debounce interval of 25 milliseconds
  // Set Startup Rotation
  if (digitalRead(TILT_PIN)) {
    oled.setRotation(0);
  }
  else {
    oled.setRotation(2);
  }
#endif

// Go...
  oled_mistertext();                                       // OLED Startup with Some Text
}

// =============================================================================================================
// =============================================== MAIN LOOP ===================================================
// =============================================================================================================
void loop(void) {
  unsigned long currentMillis = millis();

  // ESP32 OTA
#ifdef ESP32  // OTA and Reset only for ESP32
  if (OTAEN) ArduinoOTA.handle();                            // OTA active?
#endif

  // Tilt Sensor/Auto-Rotation
#ifdef XTILT
  RotationDebouncer.update();                                     // Update the Bounce instance
  if (RotationDebouncer.rose()) {
#ifdef XDEBUG
    Serial.println("Tilt Rose...");
#endif
    oled.setRotation(0);
    if (actCorename.startsWith("No Core")) {
      oled_mistertext();
    }
    else {
      usb2oled_drawlogo(0);
    }
  }
  if (RotationDebouncer.fell()) {
#ifdef XDEBUG
    Serial.println("Tilt Fell...");
#endif
    oled.setRotation(2);
    if (actCorename.startsWith("No Core")) {
      oled_mistertext();
    }
    else {
      usb2oled_drawlogo(0);
    }
  }
#endif

  // Blinker  low--pos--high--neg--low..
  if (currentMillis - previousMillis >= interval) {         // Interval check
    previousMillis = currentMillis;                         // save the last time you blinked the LED
    blink=!blink;
  }
  blinkpos = blink & !prevblink;
  blinkneg = !blink & prevblink;
  prevblink = blink;

  // Timer
  if (blinkpos) timer++;
  if (timer>minInterval) timer = 0;
  timerpos = (timer == minInterval) && blinkpos;

#ifdef XDEBUG
  if (blinkpos) Serial.println("Blink-Pos...");
  if (blinkneg) Serial.println("Blink-Neg...");
  if (timerpos) Serial.println("Blink-Pos-60...");
#endif

  // Get Serial Data
  if (Serial.available()) {
	prevCommand = newCommand;                                // Save old Command
    newCommand = Serial.readStringUntil('\n');             // Read string from serial until NewLine "\n" (from MiSTer's echo command) is detected or timeout (1000ms) happens.
    updateDisplay=true;                                    // Set Update-Display Flag

#ifdef XDEBUG
    Serial.printf("Received Corename or Command: %s\n", (char*)newCommand.c_str());
#endif
  }  // end serial available
    
  if (updateDisplay) {                                       // Proceed only if it's allowed because of new data from serial
    if (newCommand.endsWith("QWERTZ")) {                     // TESTING: Process first Transmission after PowerOn/Reboot.
        // Do nothing, just receive one string to clear the buffer.
    }                    

    // ----------------------------
    // ----- C O M M A N D 's -----
    // ----------------------------

    else if (newCommand=="cls") {                                        // Clear Screen
      oled.clearDisplay();
      oled.display();
    }
    else if (newCommand=="sorg")         oled_mistertext();
    else if (newCommand=="bye")          oled_drawlogo64h(sorgelig_icon64_width, sorgelig_icon64);
    
    // ---------------------------------------------------
    // -------------- Command Mode V2 --------------------
    // ---------------------------------------------------

    // -- Test Commands --
    else if (newCommand=="CMDCLS") {                                        // Clear Screen
      oled.clearDisplay();
      oled.display();
    }
    
    else if (newCommand=="CMDSORG") {                                       // Show Startscreen
      oled_mistertext();
    }
    
    else if (newCommand=="CMDBYE") {                                        // Show Sorgelig's Icon
      oled_drawlogo64h(sorgelig_icon64_width, sorgelig_icon64);
    }

    else if (newCommand=="CMDTEST") {                                       // Show Test-Picture
      oled_drawlogo64h(TestPicture_width, TestPicture);
    }

#ifdef XDTI
    else if (newCommand=="CMDSTEMP") {                                      // Show Temperature
    usb2oled_showtemperature();
    }
#endif

    else if (newCommand=="CMDSNAM") {                                       // Show actual loaded Corename
      usb2oled_showcorename();
    }

    else if (newCommand=="CMDSPIC") {                                       // Show actual loaded Picture with Transition
      usb2oled_drawlogo(random(minEffect,maxEffect+1));
    }

    else if (newCommand=="CMDDOFF") {                                       // Switch Display Off
      usb2oled_displayoff();
    }

    else if (newCommand=="CMDDON") {                                        // Switch Display On
      usb2oled_displayon();
    }

    else if (newCommand=="CMDDUPD") {                                       // Update Display Content
      usb2oled_updatedisplay();
    }

    else if (newCommand.startsWith("CMDTXT,")) {                            // Command from Serial to write Text
      usb2oled_readnwritetext();                                            // Read and Write Text
    }
    
    else if (newCommand.startsWith("CMDGEO,")) {                            // Command from Serial to draw geometrics
      usb2oled_readndrawgeo();                                              // Read and Draw Geometrics
    }

    else if (newCommand.startsWith("CMDAPD,")) {                            // Command from Serial to receive Picture Data via USB Serial from the MiSTer
      usb2oled_readlogo();                                                  // ESP32 Receive Picture Data... 
    }

    else if (newCommand.startsWith("CMDCOR,")) {                            // Command from Serial to receive Picture Data via USB Serial from the MiSTer
      if (usb2oled_readlogo()==1) {                                         // ESP32 Receive Picture Data... 
        usb2oled_drawlogo(random(minEffect,maxEffect+1));                   // ...and show them on the OLED with Transition Effect 1..10
      }
    }

    else if (newCommand.startsWith("CMDCOR0,")) {                           // Command from Serial to receive Picture Data via USB Serial from the MiSTer
      if (usb2oled_readlogo()==1) {                                         // ESP32 Receive Picture Data....
        usb2oled_drawlogo(0);                                               // ...and show them on the OLED with Transition Effect 0
      }
    }
    
    else if (newCommand.startsWith("CMDCON,")) {                            // Command from Serial to receive Contrast-Level Data from the MiSTer
      usb2oled_readnsetcontrast();                                          // Read and Set contrast                                   
    }

#ifdef XDTI
    else if (newCommand.startsWith("CMDULED,")) {                            // Command from Serial to receive Contrast-Level Data from the MiSTer
      usb2oled_readnsetuserled();                                            // Set LED                                   
    }
#endif

    else if (newCommand.startsWith("CMDROT,")) {                            // Command from Serial to set Rotation
      usb2oled_readnsetrotation();                                          // Set Rotation
    }

    // The following Commands are only for ESP32
#ifdef ESP32  // OTA and Reset only for ESP32
    else if (newCommand=="CMDENOTA") {                                      // Command from Serial to enable OTA on the ESP
      enableOTA();                                                          // Setup Wireless and enable OTA
    }

    else if (newCommand=="CMDRESET") {                                      // Command from Serial for Resetting the ESP
      ESP.restart();                                                        // Reset ESP
    }
#endif

    // -- Unidentified Core Name, just write it on screen
    else {
      actCorename=newCommand;
      actPicType=0;
      usb2oled_showcorename();
    }  // end ifs

#ifdef XSENDACK
    delay(cDelay);                           // Command Response Delay
    Serial.print("ttyack;");                 // Handshake with delimiter; MiSTer: "read -d ";" ttyresponse < ${TTYDEVICE}"
    Serial.flush();                          // Wait for sendbuffer is clear
#endif

    updateDisplay=false;                     // Clear Update-Display Flag
  } // end updateDisplay

  // Update Temp each Timer Interval
#ifdef XDTI
  if (newCommand=="CMDSTEMP" && timerpos) {                                      // Show Temperature
    usb2oled_showtemperature();
  }
#endif
  
} // End Main Loop

// =============================================================================================================
// ============================================== Functions ====================================================
// =============================================================================================================

// --------------------------------------------------------------
// -------------------- Show Start-Up Text ----------------------
// --------------------------------------------------------------
void oled_mistertext(void) {
  uint8_t color = 0;

#ifdef XDEBUG
  Serial.println("Show Startscreen");
#endif
  oled.clearDisplay();
  oled.drawXBitmap(82, 0, tty2oled_logo, tty2oled_logo_width, tty2oled_logo_height, SSD1322_WHITE);
  oled.display();
  delay(1000);
  //oled.setFont();
  for (int i=0; i<DispWidth; i+=16) {
    oled.fillRect(i,55,16,8,color);
    color++;
    oled.display();
    delay(25);
  }
  for (int i=0; i<DispWidth; i+=16) {
    oled.fillRect(i,55,16,8,SSD1322_BLACK);
    oled.display();
    delay(25);
  }
  delay(500);
  u8g2.setFont(u8g2_font_5x7_mf);            // 6 Pixel Font
  u8g2.setCursor(0,63);
  u8g2.print(BuildVersion);   
  oled.drawXBitmap(DispWidth-usb_icon_width, DispHeight-usb_icon_height, usb_icon, usb_icon_width, usb_icon_height, SSD1322_WHITE);

#ifdef XDTI
  u8g2.setCursor(111,63);
  u8g2.print(tSensor.getTemp());    // Show Temperature if Sensor available
  u8g2.print("\xb0");
  u8g2.print("C");
#endif

  oled.display();
} // end mistertext


// --------------------------------------------------------------
// ---- Draw Pictures with an height of 64 Pixel centerred ------
// --------------------------------------------------------------
void oled_drawlogo64h(uint16_t w, const uint8_t *bitmap) {
  oled.clearDisplay();
  oled.drawXBitmap(DispWidth/2-w/2, 0, bitmap, w, DispHeight, SSD1322_WHITE);
  oled.display();
} // end oled_drawlogo64h

#ifdef XDTI
// --------------------------------------------------------------
// ---------------- Just show the Temperature -------------------
// --------------------------------------------------------------
void usb2oled_showtemperature() {
  String myTemp="";
#ifdef XDEBUG
  Serial.println("Called Command CMDSTEMP");
#endif
  myTemp=String(tSensor.getTemp())+"\xb0"+"C";
  oled.clearDisplay();
  oled.drawRoundRect(0,0,256,64,4,10);
  u8g2.setFont(u8g2_font_luBS24_tf);
  u8g2.setCursor(DispWidth/2-(u8g2.getUTF8Width(myTemp.c_str())/2), DispHeight/2 + (u8g2.getFontAscent()/2));
  u8g2.print(myTemp);
  oled.display();
}
#endif

// --------------------------------------------------------------
// ----------------- Just show the Corename ---------------------
// --------------------------------------------------------------
void usb2oled_showcorename() {
#ifdef XDEBUG
  Serial.println("Called Command CMDSNAM");
#endif
  oled.clearDisplay();
  u8g2.setFont(u8g2_font_tenfatguys_tr);     // 10 Pixel Font
  u8g2.setCursor(DispWidth/2-(u8g2.getUTF8Width(actCorename.c_str())/2), DispHeight/2 + ( u8g2.getFontAscent()/2 ) );
  u8g2.print(actCorename);
  oled.display();
}

// --------------------------------------------------------------
// ------------------ Switch Display off ------------------------
// --------------------------------------------------------------
void usb2oled_displayoff(void) {
#ifdef XDEBUG
  Serial.println("Called Command CMDDOFF");
#endif
  
  oled.displayOff();                 // Switch Display off
}

// --------------------------------------------------------------
// ------------------- Switch Display on ------------------------
// --------------------------------------------------------------
void usb2oled_displayon(void) {
#ifdef XDEBUG
  Serial.println("Called Command CMDDOFF");
#endif
  
  oled.displayOn();                 // Switch Display on
}

// --------------------------------------------------------------
// -------------- Update Display Content ------------------------
// --------------------------------------------------------------
void usb2oled_updatedisplay(void) {
#ifdef XDEBUG
  Serial.println("Called Command CMDDUPD");
#endif
  
  oled.display();                 // Update Display Content
}

// --------------------------------------------------------------
// ----------------- Read an Set Contrast -----------------------
// --------------------------------------------------------------
void usb2oled_readnsetcontrast(void) {
  String cT="";
#ifdef XDEBUG
  Serial.println("Called Command CMDCON");
#endif
  
  cT=newCommand.substring(7);

#ifdef XDEBUG
  Serial.printf("Received Text: %s\n", (char*)cT.c_str());
#endif

  oled.setContrast(cT.toInt());            // Read and Set contrast  
}

#ifdef XDTI
// --------------------------------------------------------------
// ----------------- Read an Set User LED -----------------------
// --------------------------------------------------------------
void usb2oled_readnsetuserled(void) {
  String xT="";
#ifdef XDEBUG
  Serial.println("Called Command CMDULED");
#endif
  
  xT=newCommand.substring(8);

#ifdef XDEBUG
  Serial.printf("Received Text: %s\n", (char*)xT.c_str());
#endif

  //digitalWrite(USER_LED, lT.toInt());
  if (xT.toInt()==0) digitalWrite(USER_LED, LOW);
  if (xT.toInt()==1) digitalWrite(USER_LED, HIGH);
}
#endif

// --------------------------------------------------------------
// ----------- Command Read and Set Rotation --------------------
// --------------------------------------------------------------
void usb2oled_readnsetrotation(void) {
  String rT="";
  int r=0;
  
#ifdef XDEBUG
  Serial.println("Called Command CMDROT");
#endif
  
  rT=newCommand.substring(7);

#ifdef XDEBUG
  Serial.printf("Received Text: %s\n", (char*)rT.c_str());
#endif

  r=rT.toInt();
  
  switch (r) {
    case 0:
      oled.setRotation(0);
    break;
    case 1:
      oled.setRotation(2);
    break;
    default:
      oled.setRotation(0);
    break;
  }
}


// --------------------------------------------------------------
// ----------------------- Read Logo ----------------------------
// --------------------------------------------------------------
int usb2oled_readlogo() {
  //const int logoBytes = DispWidth * DispHeight / 8; // Make it more universal, here 2048
  //String cN="";

#ifdef XDEBUG
  Serial.println("Called Command CMDCOR");
#endif
  actCorename=newCommand.substring(7);               // Cre
#ifdef XDEBUG
  Serial.printf("Received Text: %s\n", (char*)actCorename.c_str());
#endif
  
#ifdef USE_NODEMCU
  yield();
#endif

  bytesReadCount = Serial.readBytes(logoBin, logoBytes4bpp);  // Read 2048 or 8192 Bytes from Serial

  // Set the Actual Picture Type
  if (bytesReadCount == 2048) actPicType = XBM;
  else if (bytesReadCount == 8192) actPicType = GSC;
  else actPicType = 0;

#ifdef USE_NODEMCU
  yield();
#endif

#ifdef XDEBUG
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.print(bytesReadCount);
  oled.display();
  delay(1000);
#endif

  // Check if 2048 or 8192 Bytes read
  if ((bytesReadCount != logoBytes1bpp) && (bytesReadCount != logoBytes4bpp)) {
    oled_drawlogo64h(transfererror_width, transfererror_pic);
    
    return 0;
  }
  else {
    return 1;
  }
}  //end usb2oled_readlogo

// --------------------------------------------------------------
// ----------------------- Draw Logo ----------------------------
// --------------------------------------------------------------
void usb2oled_drawlogo(uint8_t e) {
  int w,x,y,x2;
  unsigned char logoByteValue;
  int logoByte;

#ifdef XDEBUG
  Serial.println("Called Command CMDLOGO");
#endif
  
  switch (e) {
    case 1:                                  // Left to Right
      for (x=0; x<DispLineBytes1bpp; x++) {
        for (y=0; y<DispHeight; y++) {
          drawEightPixel(x, y);
        }
        oled.display();
      }
    break;  // 1

    case 2:                                  // Top to Bottom
      for (y=0; y<DispHeight; y++) {
        for (x=0; x<DispLineBytes1bpp; x++) {
          drawEightPixel(x, y);
        }
        oled.display();
      }
    break;  // 2

    case 3:                                  // Right to left
      for (x=DispLineBytes1bpp-1; x>=0; x--) {
        for (y=0; y<DispHeight; y++) {
          drawEightPixel(x, y);
        }
        oled.display();
      }
    break;  // 3

    case 4:                                  // Bottom to Top
      for (y=DispHeight-1; y>=0; y--) {
        for (x=0; x<DispLineBytes1bpp; x++) {
          drawEightPixel(x, y);
        }
        oled.display();
      }
    break;  // 4

    case 5:                                  // Even Line Left to Right / Odd Line Right to Left
      for (x=0; x<DispLineBytes1bpp; x++) {
        for (y=0; y<DispHeight; y++) {
          if ((y % 2) == 0) {
            x2 = x;
          }
          else {
            x2 = x*-1 + DispLineBytes1bpp -1;
          }
          drawEightPixel(x2, y);
        }  // end for y
        oled.display();
      }  // end for x
    break;  // 5

    case 6:                                     // Top Part Left to Right / Bottom Part Right to Left
      for (x=0; x<DispLineBytes1bpp; x++) {
        for (y=0; y<DispHeight; y++) {
          if (y < DispHeight/2) {
            x2 = x;
          }
          else {
            x2 = x*-1 + DispLineBytes1bpp -1;
          }
          drawEightPixel(x2, y);
        }  // end for y
        oled.display();
      }  // end for x
    break;  // 6

    case 7:                                     // Four Parts Left to Right to Left to Right...
      for (w=0; w<4; w++) {
        for (x=0; x<DispLineBytes1bpp; x++) {
          for (y=0; y<DispHeight/4; y++) {
            if (w%2==0) {
              x2 = x;
            }
            else {
              x2 = x*-1 + DispLineBytes1bpp -1;
            }
            drawEightPixel(x2, y+w*16);
          }  // end for y
          oled.display();
        }  // end for x
      }
    break;  // 7

    case 8:                                     // 4 Parts, Top-Left => Bottom-Right => Top-Right => Bottom-Left
      // Part 1 Top Left
      for (x=0; x<DispLineBytes1bpp/2; x++) {
        for (y=0; y<DispHeight/2; y++) {
          drawEightPixel(x, y);
        }  // end for y
        oled.display();
      }  // end for x
      // Part 2 Bottom Right
      for (x=DispLineBytes1bpp/2; x<DispLineBytes1bpp; x++) {
        for (y=DispHeight/2; y<DispHeight; y++) {
          drawEightPixel(x, y);
        }  // end for y
        oled.display();
      }  // end for x
      // Part 3 Top Right
      for (x=DispLineBytes1bpp-1; x>=DispLineBytes1bpp/2; x--) {
        for (y=0; y<DispHeight/2; y++) {
          drawEightPixel(x, y);
        }  // end for y
        oled.display();
      }  // end for x
      // Part 4 Bottom Left
      for (x=DispLineBytes1bpp/2-1; x>=0; x--) {
        for (y=DispHeight/2; y<DispHeight; y++) {
          drawEightPixel(x, y);
        }  // end for y
        oled.display();
      }  // end for x
    break; // 8

    case 9:                                      // Particle Effect
      for (w=0; w<7500; w++) {
        x = random(DispWidth);
        y = random(DispHeight);
        for (int offset=0; offset<8; offset++) {
          if (y+offset<64) drawEightPixel(x, y+offset);
        }
        // Different speed
        if (w<=1000) {
          if ((w % 25)==0) oled.display();
        }
        if ((w>1000) && (w<=2000)) {
          if ((w % 50)==0) oled.display();
        }
        if ((w>2000) && (w<=4000)) { 
          if ((w % 100)==0) oled.display();
        }
        if (w>4000) { 
          if ((w % 200)==0) oled.display();
        }
      }
      // Finally overwrite the Screen with fill Size Picture
      oled.clearDisplay();
      if (actPicType==XBM) oled.drawXBitmap(0, 0, logoBin, DispWidth, DispHeight, SSD1322_WHITE);
      if (actPicType==GSC) oled.draw4bppBitmap(logoBin);
      oled.display();
    break;  // 9

    case 10:                                       // Left to Right Diagonally
      for (x=0; x<DispLineBytes1bpp+DispHeight; x++) {
        for (y=0; y<DispHeight; y++) {
          // x2 calculation = Angle
          //x2=x-y;                                // Long Diagonal
          //x2=x-y/2;                              // Middle Diagonal
          x2=x-y/4;                                // Short Diagonal
          if ((x2>=0) && (x2<DispLineBytes1bpp)) {
            drawEightPixel(x2, y);
          }  // end for x2
          else {
#ifdef USE_NODEMCU
            yield();
#endif
          }
        }  // end for y
        oled.display();
      }  // end for x
    break;  // 10

    default:
      if (actPicType == XBM) {
#ifdef XDEBUG
        oled.clearDisplay();
        oled.setCursor(0,0);
        oled.print("drawXBitmap");
        oled.display();
        delay(1000);
#endif
        oled.clearDisplay();
        oled.drawXBitmap(0, 0, logoBin, DispWidth, DispHeight, SSD1322_WHITE);
        oled.display();
      }
      if (actPicType == GSC) {
#ifdef XDEBUG
        oled.clearDisplay();
        oled.setCursor(0,0);
        oled.print("draw4bppBitmap");
        oled.display();
        delay(1000);
#endif
        oled.clearDisplay();
        oled.draw4bppBitmap(logoBin);
        oled.display();
      }    
    break;
  } // end switch (e)
}  // end sd2oled_drawlogo

// --------------- Draw 8 Pixel to Display Buffer ----------------
// x,y: Data Coordinates of the Pixels in the Array
// 8 Pixels are written, Data Byte(s) are taken from Array
// Display Positions are calculated from x,y and Type of Pic
// --------------------------------------------------------------- 
void drawEightPixel(int x, int y) {
  unsigned char b;
  int i;
  switch (actPicType) {
    case XBM:
      b=logoBin[x+y*DispLineBytes1bpp];            // Get Data Byte for 8 Pixels
      for (i=0; i<8; i++){
        if (bitRead(b, i)) {
          oled.drawPixel(x*8+i,y,SSD1322_WHITE);   // Draw Pixel if "1"
        }
        else {
          oled.drawPixel(x*8+i,y,SSD1322_BLACK);   // Clear Pixel if "0"
        }
      }
    break;
    case GSC:
      for (i=0; i<4; i++) {
        b=logoBin[(x*4)+i+y*DispLineBytes4bpp];          // Get Data Byte for 2 Pixels
        oled.drawPixel(x*8+i*2+0, y, (0xF0 & b) >> 4);   // Draw Pixel 1, Left Nibble
        oled.drawPixel(x*8+i*2+1, y, 0x0F & b);          // Draw Pixel 2, Right Nibble
      }
    break;
  }
#ifdef USE_NODEMCU
  yield();
#endif
}

// ----------------------------------------------------------------------
// ----------------------- Read and Write Text --------------------------
// ----------------------------------------------------------------------
void usb2oled_readnwritetext(void) {
  int f=0,c=0,b=0,x=0,y=0,d1=0,d2=0,d3=0,d4=0,d5=0;
  int16_t x1,y1;
  uint16_t w1,h1;
  String TextIn="", fT="", cT="", bT="", xT="", yT="", TextOut="";
  bool clearMode=false;
  
#ifdef XDEBUG
  Serial.println("Called Command CMDTEXT");
#endif
 
  TextIn = newCommand.substring(7);            // Get Command Text from "newCommand"
  
#ifdef XDEBUG
  Serial.printf("Received Text: %s\n", (char*)TextIn.c_str());
#endif

  //Searching for the "," delimiter
  d1 = TextIn.indexOf(',');                 // Find location of first ","
  d2 = TextIn.indexOf(',', d1+1 );          // Find location of second ","
  d3 = TextIn.indexOf(',', d2+1 );          // Find location of third ","
  d4 = TextIn.indexOf(',', d3+1 );          // Find location of fourth ","
  d5 = TextIn.indexOf(',', d4+1 );          // Find location of fifth ","

  //Create Substrings
  fT = TextIn.substring(0, d1);             // Get String for Font-Type
  cT = TextIn.substring(d1+1, d2);          // Get String for Draw Color
  bT = TextIn.substring(d2+1, d3);          // Get String for Background Color
  xT = TextIn.substring(d3+1, d4);          // Get String for X-Position
  yT = TextIn.substring(d4+1, d5);          // Get String for Y-Position
  TextOut = TextIn.substring(d5+1);         // Get String for Text
  
#ifdef XDEBUG
  Serial.printf("Created Strings: F:%s C%s X:%s Y:%s T:%s\n", (char*)fT.c_str(), (char*)cT.c_str(), (char*)xT.c_str(), (char*)yT.c_str(), (char*)TextOut.c_str());
#endif

  // Convert Strings to Integer
  f = fT.toInt();
  c = cT.toInt();
  b = bT.toInt();
  x = xT.toInt();
  y = yT.toInt();
  
  // Parameter check
  if (f<0 || c<0 || c>15 || b<0 || b>15 || x<0 || x>DispWidth-1 || y<0 || y>DispHeight-1 || d1==-1 || d2==-1 || d3==-1 || d4==-1 || d5==-1) {
    f=1;
    c=15;
    x=5;
    y=40;
    TextOut="Error CMDTEXT";
  }

  if (f>=100) {                  // Do not run oled.display() after printing
    clearMode=true;
    f=f-100;
  }

#ifdef XDTI
  if (TextOut=="TEP184") {      // If Text is "TEP184" show Temperature Value
    //TextOut=String(tSensor.getTemp());
    TextOut=String(tSensor.getTemp())+"\xb0"+"C";
  }
#endif
  
  //Set Font
  switch (f) {
    case 0:
      u8g2.setFont(u8g2_font_5x7_mf);             // Transparent 6 Pixel Font
    break;
    case 1:
      u8g2.setFont(u8g2_font_luBS08_tf);          // Transparent Font 20x12, 8 Pixel A
    break;
    case 2:
      u8g2.setFont(u8g2_font_luBS10_tf);          // Transparent Font 26x15, 10 Pixel A
    break;
    case 3:
      u8g2.setFont(u8g2_font_luBS14_tf);          // Transparent Font 35x22, 14 Pixel A
    break;
    case 4:
      u8g2.setFont(u8g2_font_luBS18_tf);          // Transparent Font 44x28, 18 Pixel A
    break;
    case 5:
      u8g2.setFont(u8g2_font_luBS24_tf);          // Transparent Font 61x40, 24 Pixel A
    break;
    case 6:
      u8g2.setFont(u8g2_font_lucasarts_scumm_subtitle_o_tf); // Nice 12 Pixel Font
    break;
    case 7:
      u8g2.setFont(u8g2_font_tenfatguys_tr);      // Nice 10 Pixel Font
    break;
    case 8:
      u8g2.setFont(u8g2_font_7Segments_26x42_mn); // 7 Segments 42 Pixel Font
    break;
    default:
      u8g2.setFont(u8g2_font_tenfatguys_tr);      // Nice 10 Pixel Font
    break;
  }
  // Write or Clear Text
  u8g2.setForegroundColor(c);                           // Set Font Color
  u8g2.setBackgroundColor(b);                           // Set Backgrounf Color
  u8g2.setCursor(x,y);                                  // Set Cursor Position
  u8g2.print(TextOut);                                  // Write Text to Buffer
  if (!clearMode) oled.display();                       // Update Screen only if not Clear Mode (Font>100)
  u8g2.setForegroundColor(SSD1322_WHITE);               // Set Color back
  u8g2.setBackgroundColor(SSD1322_BLACK);
  //u8g2.setFontMode(0);
}


// --------------------------------------------------------------
// ------------------ Read and Draw Geometrics ------------------
// --------------------------------------------------------------
void usb2oled_readndrawgeo(void) {
  int g=0,c=0,x=0,y=0,i=0,j=0,k=0,l=0,d1=0,d2=0,d3=0,d4=0,d5=0,d6=0,d7=0;
  String TextIn="",gT="",cT="",xT="",yT="",iT="",jT="",kT="",lT="";
  bool pError=false;
  bool clearMode=false;
  
#ifdef XDEBUG
  Serial.println("Called Command CMDGEO");
#endif

  TextIn = newCommand.substring(7);             // Get Command Text from "newCommand"
  
#ifdef XDEBUG
  Serial.printf("Received Text: %s\n", (char*)TextIn.c_str());
#endif
  
  //Searching for the "," delimiter
  d1 = TextIn.indexOf(',');                 // Find location of first ","
  d2 = TextIn.indexOf(',', d1+1 );          // Find location of second ","
  d3 = TextIn.indexOf(',', d2+1 );          // Find location of third ","
  d4 = TextIn.indexOf(',', d3+1 );          // Find location of fourth ","
  d5 = TextIn.indexOf(',', d4+1 );          // Find location of fifth ","
  d6 = TextIn.indexOf(',', d5+1 );          // Find location of sixt ","
  d7 = TextIn.indexOf(',', d6+1 );          // Find location of seventh ","

  //Create Substrings
  gT = TextIn.substring(0, d1);           // Get String for Geometric-Type
  cT = TextIn.substring(d1+1, d2);        // Get String for Clear Flag
  xT = TextIn.substring(d2+1, d3);        // Get String for X-Position
  yT = TextIn.substring(d3+1, d4);        // Get String for Y-Position
  iT = TextIn.substring(d4+1, d5);        // Get String for Parameter i
  jT = TextIn.substring(d5+1, d6);        // Get String for Parameter j
  kT = TextIn.substring(d6+1, d7);        // Get String for Parameter k
  lT = TextIn.substring(d7+1);            // Get String for Parameter l

#ifdef XDEBUG
  Serial.printf("Part-Strings: G:%s C:%s X:%s Y:%s I:%s J:%s K:%s L:%s\n", (char*)gT.c_str(), (char*)cT.c_str(), (char*)xT.c_str(), (char*)yT.c_str(), (char*)iT.c_str(), (char*)jT.c_str(), (char*)kT.c_str(), (char*)lT.c_str());
#endif

  // Convert Strings to Integer
  g = gT.toInt();
  c = cT.toInt();
  x = xT.toInt();
  y = yT.toInt();
  i = iT.toInt();
  j = jT.toInt();
  k = kT.toInt();
  l = lT.toInt();

#ifdef XDEBUG
  Serial.printf("Values: G:%i C:%i X:%i Y:%i I:%i J:%i K:%i L:%i\n", g,c,x,y,i,j,k,l);
#endif

  // Enough Parameter given / Parameter Check
  if (g<1 || c<0 || c>15 || x<0 || x>DispWidth-1 || y<0 || y>DispHeight-1 || d1==-1 || d2==-1 || d3==-1  || d4==-1 || d5==-1  || d6==-1 || d7==-1) {
    pError=true;
  }
  
  if (g>100) {                  // Do not run oled.display() after drawing
    clearMode=true;
    g=g-100;
  }
  
  if (!pError) {
    switch (g) {
      case 1:  // Pixel x,y
        oled.drawPixel(x,y,c);
      break;
      case 2:  // Line x0,y0,x1,y1,c
        oled.drawLine(x,y,i,j,c);
      break;
      case 3:  // Rectangle x,y,w,h,c
        oled.drawRect(x,y,i,j,c);
      break;
      case 4:  // Filled Rectangle/Box x,y,w,h,c
        oled.fillRect(x,y,i,j,c);
      break;
      case 5:  // Circle x,y,r,c
        oled.drawCircle(x,y,i,c);
      break;
      case 6:  // Filled Circle x,y,r,c
        oled.fillCircle(x,y,i,c);
      break;
      case 7:  // Rounded Rectangle x,y,w,h,r,c
        oled.drawRoundRect(x,y,i,j,k,c);
      break;
      case 8: // Filled Rounded Rectangle x,y,w,h,r,c
        oled.fillRoundRect(x,y,i,j,k,c);
      break;
      case 9: // Triangle x0,y1,x1,y1,x2,y2,c
        oled.drawTriangle(x,y,i,j,k,l,c);
      break;
      case 10: // Filled Triangle x0,y1,x1,y1,x2,y2,c
        oled.fillTriangle(x,y,i,j,k,l,c);
      break;
      
      default:  // Just something :-)
        oled.drawCircle(128,32,32,15);
        oled.fillCircle(128,32,8,8);
        break;
    }
  }
  else {
    oled.setCursor(5, 40);
    oled.print("Error CMDGEO");
  }
  if (!clearMode) oled.display();                       // Update Screen only if not Clear Mode (Geo>100)
}


// --------------------------------------------------
// ---------------- Enable OTA ---------------------- 
// --------------------------------------------------
#ifdef ESP32  // OTA and Reset only for ESP32
void enableOTA (void) {
  Serial.println("Connecting to Wireless...");
  oled.setTextSize(1);
  oled.clearDisplay();
  oled.setCursor(10,15);
  oled.print("Connecting to Wireless...");
  oled.display();
  WiFi.mode(WIFI_STA);
  WiFi.begin(MySSID, MyPWD);
//  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection failed! Reboot...");;
    oled.clearDisplay();
    oled.setCursor(10,15);
    oled.print("Conn.failed! Reboot...");
    oled.display();
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
#ifdef XDEBUG
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
#endif
    oled.setCursor(95,55);
    oled.printf("%u%%", (progress / (total / 100)));
    oled.display();
    oled.setCursor(95,55);
    oled.setTextColor(SSD1322_BLACK);
    oled.printf("%u%%", (progress / (total / 100)));   // Re-Write the Value with Black to clear it for the next update
    oled.setTextColor(SSD1322_WHITE,SSD1322_BLACK);
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
  
#ifdef XDEBUG
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#endif
  
  oled.clearDisplay();
  oled.setCursor(10,15);
  oled.print("OTA Active!");
  oled.setCursor(10,35);
  oled.print("IP address: ");
  oled.print(WiFi.localIP());
  oled.setCursor(10,55);
  oled.printf("Progress:");
  oled.display();
  
  OTAEN = true;  // Set OTA Enabled to True for the "Handler" in "loop"
}
#endif

//========================== The end ================================
