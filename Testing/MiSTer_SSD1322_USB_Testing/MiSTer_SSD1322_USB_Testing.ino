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

  2022-03-02
  -Use more PicType "NONE" (Cosmetics)

  2022-03-16
  -Screensaver Mod, use 1/4 size Core Picture for the ScreenSaver
   
  ToDo
  -Everything I forgot
   
*/

// Set Version
#define BuildVersion "220316T"                    // "T" for Testing

// Include Libraries
#include <Arduino.h>
#include <SSD1322_for_Adafruit_GFX.h>             // SSD1322 Controller Display Library https://github.com/venice1200/SSD1322_for_Adafruit_GFX
#include <U8g2_for_Adafruit_GFX.h>                // U8G2 Font Engine for Adafruit GFX  https://github.com/olikraus/U8g2_for_Adafruit_GFX
#include "logo.h"                                 // Some needed Pictures

// ---------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------- System Config -------------------------------------------------------
// ------------------------------------------- Activate your Options ---------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

// How-To...
// Comment (add "//" in front of the Option) to de-activate the Option
// Uncomment (remove "//" in front of the Option) to activate the Option

// Get Debugging Infos over Serial
//#define XDEBUG

// Uncomment for 180° StartUp Rotation (Display Connector up)
//#define XROTATE

// Uncomment for "Send Acknowledge" from tty2oled to MiSTer
#define XSENDACK

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------- Auto-Board-Config via Arduino IDE Board Selection --------------------------------
// ------------------------------------ Make sure the Auto-Board-Config is not active ----------------------------------
// ---------------------------------------------------------------------------------------------------------------------

#ifdef ARDUINO_ESP32_DEV          // Set Arduino Board to "ESP32 Dev Module"
  #define USE_ESP32DEV            // TTGO-T8, tty2oled Board by d.ti
#endif

#ifdef ARDUINO_LOLIN32            // Set Arduino Board to "WEMOS LOLIN32"
  #define USE_LOLIN32             // Wemos LOLIN32, LOLIN32, DevKit_V4
#endif

#if defined(ARDUINO_ESP8266_NODEMCU) || defined(ARDUINO_ESP8266_NODEMCU_ESP12E)  // Set Arduino Board to "NodeMCU 1.0 (ESP-12E Module)"
  #define USE_NODEMCU             // ESP8266 NodeMCU v3
#endif

// ---------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------ Manual-Board-Config ------------------------------------------------
// ------------------------------------ Make sure the Auto-Board-Config is not active ----------------------------------
// ---------------------------------------------------------------------------------------------------------------------

//#define USE_ESP32DEV           // TTGO-T8. Set Arduino Board to ESP32 Dev Module, xx MB Flash, def. Part. Schema
//#define USE_LOLIN32            // Wemos LOLIN32, LOLIN32, DevKit_V4. Set Arduino Board to "WEMOS LOLIN32"
//#define USE_NODEMCU            // ESP8266 NodeMCU v3. Set Arduino Board to NodeMCU 1.0 (ESP-12E Module)

// ---------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------- Hardware-Config --------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

// TTGO-T8/d.ti/ESP32 Dev
// OLED Pins, Tilt Pin, I2C, User-LED for d.ti Board
// using VSPI SCLK = 18, MISO = 19, MOSI = 23 and...
#ifdef USE_ESP32DEV
  #define OLED_CS 26               // OLED Chip Select Pin
  #define OLED_DC 25               // OLED Data/Command Pin
  #define OLED_RESET 27            // OLED Reset Pin
  #define I2C1_SDA 17              // I2C_1-SDA
  #define I2C1_SCL 16              // I2C_1-SCL
  #define TILT_PIN 32              // Using internal PullUp
  #define USER_LED 19              // USER_LED/WS2812B
  #define PWRLED 5                 // Set Pin to "1" = LED's off
  #define BUZZER 4                 // Piezo Buzzer
  #define TONE_PWM_CHANNEL 0       // See: https://makeabilitylab.github.io/physcomp/esp32/tone.html
  #include <MIC184.h>              // MIC184 Library, get from https://github.com/venice1200/MIC184_Temperature_Sensor
  MIC184 tSensor;                  // Create Sensor Class
  #include <FastLED.h>             // FastLED Library, get from Library Manager
  #define NUM_WSLEDS 1             // Number of WS-LED's
  #define WS_BRIGHTNESS 50         // WS-LED Brightness
  CRGB wsleds[NUM_WSLEDS];         // Create WS-LED RGB Array
#endif

// WEMOS LOLIN32/Devkit_V4 using VSPI SCLK = 18, MISO = 19, MOSI = 23, SS = 5 and...
#ifdef USE_LOLIN32
  #define OLED_CS 5
  #define OLED_DC 16
  #define OLED_RESET 17
  #define TILT_PIN 32
#endif

// ESP8266-Board (NodeMCU v3)
#ifdef USE_NODEMCU
  #define OLED_CS 15
  #define OLED_DC 4
  #define OLED_RESET 5
  #define TILT_PIN 16
#endif

// Hardware Constructor OLED Display and U8G2 Support
Adafruit_SSD1322 oled(256, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS);
U8G2_FOR_ADAFRUIT_GFX u8g2;

// Tilt Sensor
#include <Bounce2.h>                     // << Extra Library, via Arduino Library Manager
#define DEBOUNCE_TIME 25                 // Debounce Time
Bounce RotationDebouncer = Bounce();     // Create Bounce class

#ifdef USE_ESP32DEV
#endif

// OTA and Reset only for ESP32
#ifdef ESP32
  #include "cred.h"                               // Load your WLAN Credentials for OTA
  #include <WiFi.h>
  #include <ESPmDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  bool OTAEN=false;                               // Will be set to "true" by Command "CMDENOTA"
#endif

// -------------------------------------------------------------
// ------------------------- Variables -------------------------
// -------------------------------------------------------------

// Strings
String newCommand = "";                // Received Text, from MiSTer without "\n" currently (2021-01-11)
String prevCommand = "";
String actCorename = "No Core loaded"; // Actual Received Corename
uint8_t contrast = 5;                  // Contrast (brightness) of display, range: 0 (no contrast) to 255 (maximum)
int tEffect = 0;                       // Run this Effect
//char *newCommandChar;

bool updateDisplay = false;
bool startScreenActive = false;

// Display Vars
uint16_t DispWidth, DispHeight, DispLineBytes1bpp, DispLineBytes4bpp;
int logoBytes1bpp=0;
int logoBytes4bpp=0;
//unsigned int logoBytes1bpp=0;
//unsigned int logoBytes4bpp=0;
const int cDelay=25;                          // Command Delay in ms for Handshake
const int hwDelay=100;                        // Delay for HWINFO Request
size_t bytesReadCount=0;
uint8_t *logoBin;                             // <<== For malloc in Setup
enum picType {NONE, XBM, GSC, TXT};           // Enum Picture Type
int actPicType=NONE;
int16_t xs, ys;
uint16_t ws, hs;
const uint8_t minEffect=1, maxEffect=23;      // Min/Max Effects for Random
//const uint8_t minEffect=22, maxEffect=23;      // Min/Max Effects for TESTING

// Blinker 500ms Interval
const long interval = 500;                    // Interval for Blink (milliseconds)
bool blink = false;
bool prevblink = false;
bool blinkpos = false;                        // Pos Flanc
bool blinkneg = false;                        // Neg Flanc
unsigned long previousMillis = 0;
const int interval10 = 10;                   // Interval for Timer 10sec
const int interval30 = 30;                   // Interval for Timer 30sec
const int interval60 = 60;                   // Interval for Timer 60sec
int timer=0;                                 // Counter for Timer
bool timer10pos;                             // Positive Timer 10 sec Signal
bool timer30pos;                             // Positive Timer 30 sec Signal
bool timer60pos;                             // Positive Timer 60 sec Signal

// ScreenSaver
bool ScreenSaverEnabled=false;
bool ScreenSaverActive=false;
int ScreenSaverTimer=0;                      // ScreenSaverTimer
int ScreenSaverInterval=60;                  // Interval for ScreenSaverTimer
bool ScreenSaverPos;                         // Positive Signal ScreenSaver
int ScreenSaverColor=1;                      // ScreenSaver Drawing Color
int ScreenSaverLogoTimer=0;                  // ScreenSaverLogo-Timer
int ScreenSaverLogoTime=60;                  // ScreenSaverLogoTime

// I2C Hardware
bool micAvail=false;                          // Is the MIC184 Sensor available?
const byte PCA9536_ADDR = 0x41;               // PCA9536 Base Address
const byte PCA9536_IREG = 0x00;               // PCA9536 Input Register
bool pcaAvail=false;                          // Is the PCA9536 Port-Extender Chip available?
byte pcaInputValue=255;                       // PCA9536 Input Pin State as Byte Value

// =============================================================================================================
// ====================================== NEEDED FUNCTION PROTOTYPES ===========================================
// =============================================================================================================

// Info about overloading found here
// https://stackoverflow.com/questions/1880866/can-i-set-a-default-argument-from-a-previous-argument
void drawEightPixelXY(int x, int y, int dx, int dy);
inline void drawEightPixelXY(int x, int y) { drawEightPixelXY(x,y,x,y); };

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
  DispLineBytes1bpp = DispWidth / 8;                       // How many Bytes uses each Display Line at 1bpp (32 byte for width 256 Pixel)
  DispLineBytes4bpp = DispWidth / 2;                       // How many Bytes uses each Display Line at 4bpp (128 byte for width 256 Pixel)
  logoBytes1bpp = DispWidth * DispHeight / 8;              // SSD1322 = 2048 Bytes
  logoBytes4bpp = DispWidth * DispHeight / 2;              // SSD1322 = 8192 Bytes
  logoBin = (uint8_t *) malloc(logoBytes4bpp);             // Create Picture Buffer, better than permanent create (malloc) and destroy (free)

// Activate Options

  // Setup d.ti Board (Temp.Sensor/USER_LED/PCA9536)
#ifdef USE_ESP32DEV                                             // Only for ESP-DEV (TTGO-T8/d.ti)
  pinMode(PWRLED, OUTPUT);                                      // Setup Power LED
  //ledcAttachPin(BUZZER, TONE_PWM_CHANNEL);                      // Buzzer Setup Move to playtone function
  
  Wire.begin(int(I2C1_SDA), int(I2C1_SCL), uint32_t(100000));   // Setup I2C-1 Port
  
  Wire.beginTransmission (MIC184_BASE_ADDRESS);                 // Check for MIC184 Sensor...
  if (Wire.endTransmission() == 0) {                           // ..and wait for Answer
    micAvail=true;                                              // If Answer OK Sensor available
  }
  //tSensor.setZone(MIC184_ZONE_REMOTE);                        // Remote = use External Sensor using LM3906/MMBT3906
#ifdef XDEBUG
  if (micAvail) {
    Serial.println("MIC184 Sensor available.");
    Serial.print("Temperature: ");
    Serial.print(tSensor.getTemp());
    Serial.println("°C");
  }
  else {
    Serial.println("MIC184 Sensor not available.");
  }
#endif  // XDEBUG

  Wire.beginTransmission(PCA9536_ADDR);                        // Check for PCA9536
  if (Wire.endTransmission() == 0) {                           // ..and wait for Answer
    pcaAvail=true;                                             // If Answer OK PCA available
  }

#ifdef XDEBUG
  if (pcaAvail) {
    Serial.println("PCA9536 available.");
  }
  else {
    Serial.println("PCA9536 not available.");
  }
#endif  // XDEBUG

  if (pcaAvail) {                                               // If PCA9536 available..
    Wire.beginTransmission(PCA9536_ADDR);                       // start transmission and.. 
    Wire.write(PCA9536_IREG);                                   // read Register 0 (Input Register).
    if (Wire.endTransmission() == 0) {                          // If OK...
      Wire.requestFrom(PCA9536_ADDR, byte(1));                  // request one byte from PCA
      if (Wire.available() == 1) {                              // If just one byte is available,
        pcaInputValue = Wire.read() & 0x0F;                     // read it and mask the high bits out.
#ifdef XDEBUG
        Serial.print("PCA9536 Input Register Value: ");
        Serial.println(pcaInputValue);
#endif  // XDEBUG
      }
      else {
        while (Wire.available()) Wire.read();                   // If more byte are available = something wrong ;-)
#ifdef XDEBUG
        Serial.println("PCA9536: too much bytes available!");
#endif  // XDEBUG
      }
    }
  }
  
  if (!pcaAvail) {                                                 // If PCA9536 is not available = d.ti Board Rev 1.1
    pinMode(USER_LED, OUTPUT);                                     // Setup User LED
  }
  else {                                                           // If PCA9536 is available = d.ti Board Rev 1.2 or greater
    FastLED.addLeds<WS2812B, USER_LED, GRB>(wsleds, NUM_WSLEDS);   // Setup User WS2812B LED
    FastLED.setBrightness(WS_BRIGHTNESS);                          // and set Brightness
  }
#endif  // USE_ESP32DEV

  // Tilt Sensor Rotation via Tilt-Sensor Pin
  RotationDebouncer.attach(TILT_PIN,INPUT_PULLUP);         // Attach the debouncer to a pin with INPUT mode
  RotationDebouncer.interval(DEBOUNCE_TIME);               // Use a debounce interval of 25 milliseconds
  // Set Startup Rotation
  if (digitalRead(TILT_PIN)) {                             // If Signal = 1 no Rotation
    oled.setRotation(0);
  }
  else {                                                   // If Signal = 0 180° Rotation
    oled.setRotation(2);
  }

// XROTATE Option Rotation
#ifdef XROTATE
  oled.setRotation(2);                                     // 180° Rotation
#endif

// Go...
  oled_showStartScreen();                                  // OLED Startup

  delay(cDelay);                                           // Command Response Delay
  Serial.print("ttyrdy;");                                 // Send "ttyrdy;" after setup is done.
  //Serial.println("ttyrdy;");                                 // Send "ttyrdy;" with "\n" after setup is done.
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
  RotationDebouncer.update();                                     // Update the Bounce instance
  if (RotationDebouncer.rose()) {
#ifdef XDEBUG
    Serial.println("Tilt Rose...");
#endif
    oled.setRotation(0);
    if (actCorename.startsWith("No Core")) {
      oled_showStartScreen();
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
      oled_showStartScreen();
    }
    else {
      usb2oled_drawlogo(0);
    }
  }

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
  timer10pos = (timer % interval10 == 0) && blinkpos;
  timer30pos = (timer % interval30 == 0) && blinkpos;
  timer60pos = (timer % interval60 == 0) && blinkpos;
  if (timer>=interval60) timer = 0;
  
  // ScreenSaver Logo-Timer
  if (ScreenSaverEnabled && !ScreenSaverActive && blinkpos) ScreenSaverLogoTimer++;
  ScreenSaverActive = (ScreenSaverLogoTimer>=ScreenSaverLogoTime) && ScreenSaverEnabled;
  
  // ScreenSaver Timer
  if (ScreenSaverActive && blinkpos) ScreenSaverTimer++;
  ScreenSaverPos = (ScreenSaverTimer == ScreenSaverInterval) && blinkpos;
  if (ScreenSaverTimer>=ScreenSaverInterval) ScreenSaverTimer=0;

#ifdef XDEBUG
  //if (blinkpos) Serial.println("Blink-Pos");
  //if (blinkneg) Serial.println("Blink-Neg");
  if (timer10pos) Serial.println("Blink-10s");
  if (timer30pos) Serial.println("Blink-30s");
  if (timer60pos) Serial.println("Blink-60s");
  if (blinkpos) {
    Serial.printf("ScreenSaverEnabled: %s, ScreenSaverActive: %s, ", ScreenSaverEnabled ? "true" : "false", ScreenSaverActive ? "true" : "false");
    Serial.printf("ScreenSaverLogoTimer: %d, ScreenSaverTimer: %d\n", ScreenSaverLogoTimer, ScreenSaverTimer);
  }  
  if (ScreenSaverPos) Serial.println("ScreenSaverTimer");
#endif

  // Get Serial Data
  if (Serial.available()) {
  	prevCommand = newCommand;                              // Save old Command
    newCommand = Serial.readStringUntil('\n');             // Read string from serial until NewLine "\n" (from MiSTer's echo command) is detected or timeout (1000ms) happens.
    updateDisplay=true;                                    // Set Update-Display Flag

#ifdef XDEBUG
    Serial.printf("\nReceived Corename or Command: %s\n", (char*)newCommand.c_str());
#endif
  }  // end serial available
    
  if (updateDisplay) {                                                                                 // Proceed only if it's allowed because of new data from serial
    if (startScreenActive && newCommand.startsWith("CMD") && !newCommand.startsWith("CMDTZONE")) {     // If any Command is processed the StartScreen isn't shown any more
      startScreenActive=false;                                                                         // This variable should prevent "side effects" with Commands and is used to disable automatic drawings
    }

    if (newCommand.endsWith("QWERTZ")) {                         // Process first Transmission after PowerOn/Reboot.
        // Do nothing, just receive one string to clear the buffer.
    }                    

    // ---------------------------------------------------
    // ---------------- C O M M A N D 's -----------------
    // ---------------------------------------------------

    else if (newCommand=="cls") {                                        // Clear Screen
      oled.clearDisplay();
      oled.display();
    }
    else if (newCommand=="sorg")         oled_showStartScreen();
    else if (newCommand=="bye")          oled_drawlogo64h(sorgelig_icon64_width, sorgelig_icon64);
    
    // ---------------------------------------------------
    // -------------- Command Mode V2 --------------------
    // ---------------------------------------------------

    // -- Test Commands --
    else if (newCommand=="CMDCLS") {                                        // Clear Screen with Display Update
      oled.clearDisplay();
      oled.display();
    }
    
    else if (newCommand=="CMDCLSWU") {                                      // Clear Screen without Display Update
      oled.clearDisplay();
    }

    else if (newCommand.startsWith("CMDCLST")) {                            // Clear/Fill Screen with Transition and Color
      usb2oled_clswithtransition();
    }
    
    else if (newCommand=="CMDSORG") {                                       // Show Startscreen
      oled_showStartScreen();
    }
    
    else if (newCommand=="CMDBYE") {                                        // Show Sorgelig's Icon
      oled_drawlogo64h(sorgelig_icon64_width, sorgelig_icon64);
    }

    else if (newCommand=="CMDHWINF") {                                      // Send HW Info
      oled_sendHardwareInfo();
    }

    else if (newCommand=="CMDTEST") {                                       // Show Test-Picture
      oled_drawlogo64h(TestPicture_width, TestPicture);
    }

    else if (newCommand=="CMDSNAM") {                                       // Show actual loaded Corename
      usb2oled_showcorename();
    }

    else if (newCommand.startsWith("CMDSPIC")) {                            // Show actual loaded Picture with(without Transition
      usb2oled_showpic();
      if (tEffect==-1) {                                                    // Send without Effect Parameter or Parameter = -1
        usb2oled_drawlogo(random(minEffect,maxEffect+1));                   // ...and show them on the OLED with Transition Effect 1..MaxEffect
      } 
      else {                                                                // Send with Effect "CMDSPIC,15"
        usb2oled_drawlogo(tEffect);
      }
    }

    else if (newCommand=="CMDSSLPIC") {                                     // Show actual loaded Core Picture but in 1/4 size
      oled_showSmallCorePicture(64,16);
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

    else if (newCommand.startsWith("CMDTXT")) {                             // Command from Serial to write Text
      usb2oled_readnwritetext();                                            // Read and Write Text
    }
    
    else if (newCommand.startsWith("CMDGEO")) {                             // Command from Serial to draw geometrics
      usb2oled_readndrawgeo();                                              // Read and Draw Geometrics
    }

    else if (newCommand.startsWith("CMDAPD")) {                             // Command from Serial to receive Picture Data via USB Serial from the MiSTer
      usb2oled_readlogo();                                                  // Receive Picture Data... 
    }

    else if (newCommand.startsWith("CMDCOR")) {                             // Command from Serial to receive Picture Data via USB Serial from the MiSTer
      if (usb2oled_readlogo()==1) {                                         // Receive Picture Data... 
        if (tEffect==-1) {                                                  // Send without Effect Parameter or with Effect Parameter -1
          usb2oled_drawlogo(random(minEffect,maxEffect+1));                 // ...and show them on the OLED with Transition Effect 1..MaxEffect
        } 
        else {                                                              // Send with Effect "CMDCOR,llander,15"
          usb2oled_drawlogo(tEffect);
        }
      }
    }

    else if (newCommand.startsWith("CMDCON")) {                            // Command from Serial to receive Contrast-Level Data from the MiSTer
      usb2oled_readnsetcontrast();                                          // Read and Set contrast                                   
    }

    else if (newCommand.startsWith("CMDROT")) {                            // Command from Serial to set Rotation
      usb2oled_readnsetrotation();                                         // Set Rotation
    }
    else if (newCommand.startsWith("CMDSAVER")) {                          // Command from Serial to set Screensaver
      usb2oled_readnsetscreensaver();                                      // Enable/Disable Screensaver
    }

    // The following Commands are only for the d.ti Board
#ifdef USE_ESP32DEV
    else if (newCommand.startsWith("CMDULED")) {                           // User LED
      usb2oled_readnsetuserled();                                           // Set LED                                   
    }
    
    else if (newCommand.startsWith("CMDPLED")) {                           // Power Control LED
      usb2oled_readnsetpowerled();                                          // Set LED
    }

    else if (newCommand=="CMDSTEMP") {                                      // Enable to show Temperature Big Picture
    usb2oled_showtemperature();
    }

    else if (newCommand.startsWith("CMDTZONE")) {                           // Set Temperature Zone
    usb2oled_settempzone();
    }

    else if (newCommand.startsWith("CMDPTONE")) {                           // Play Tone
    usb2oled_playtone();
    }

    else if (newCommand.startsWith("CMDPFREQ")) {                           // Play Frequency
    usb2oled_playfrequency();
    }
#endif  // USE_ESP32DEV

    // The following Commands are only for ESP32
#ifdef ESP32  // OTA and Reset only for ESP32
    else if (newCommand=="CMDENOTA") {                                      // Command from Serial to enable OTA on the ESP
      enableOTA();                                                          // Setup Wireless and enable OTA
    }

    else if (newCommand=="CMDRESET") {                                      // Command from Serial for Resetting the ESP
      ESP.restart();                                                        // Reset ESP
    }
#endif  // ESP32

    // -- Unidentified Core Name, just write it on screen
    else {
      actCorename=newCommand;
      actPicType=NONE;
      usb2oled_showcorename();
    }  // end ifs

#ifdef XSENDACK
    delay(cDelay);                           // Command Response Delay
    Serial.print("ttyack;");                 // Handshake with delimiter; MiSTer: "read -d ";" ttyresponse < ${TTYDEVICE}"
    Serial.flush();                          // Wait for sendbuffer is clear
#endif

    updateDisplay=false;                              // Clear Update-Display Flag
  } // endif updateDisplay

  // ScreenSaver if Active
  if (ScreenSaverActive && ScreenSaverPos) {              // Screensaver each 60secs
    oled_showScreenSaverPicture();
  }

// Show Temperature ESP32DEV only
#ifdef USE_ESP32DEV
  // Update Temp each Timer Interval only if MIC184 is available and..
  // ..if just the plain Boot Screen is shown..
  if (micAvail) {
    if (startScreenActive && timer30pos) {
      u8g2.setCursor(111,63);
      u8g2.print(tSensor.getTemp());                  // Show Temperature if Sensor available
      u8g2.print("\xb0");
      u8g2.print("C");
      oled.display();
    }
    // ..or CMDSTEMP was called
    if (newCommand=="CMDSTEMP" && timer30pos) {          // Show Temperature
      usb2oled_showtemperature();
    }
  }  // endif micAvail
#endif
  
} // End Main Loop

// =============================================================================================================
// ============================================== Functions ====================================================
// =============================================================================================================

// --------------------------------------------------------------
// -------------------- Show Start-Up Text ----------------------
// --------------------------------------------------------------
void oled_showStartScreen(void) {
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
    delay(20);
  }
  for (int i=0; i<DispWidth; i+=16) {
    oled.fillRect(i,55,16,8,SSD1322_BLACK);
    oled.display();
    delay(20);
  }
  delay(500);
  u8g2.setFont(u8g2_font_5x7_mf);            // 6 Pixel Font
  u8g2.setCursor(0,63);
  u8g2.print(BuildVersion);
  if (micAvail) u8g2.print("M");
  if (pcaAvail) u8g2.print("P");
  oled.drawXBitmap(DispWidth-usb_icon_width, DispHeight-usb_icon_height, usb_icon, usb_icon_width, usb_icon_height, SSD1322_WHITE);

#ifdef USE_ESP32DEV
  if (micAvail) {
    u8g2.setCursor(111,63);
    u8g2.print(tSensor.getTemp());    // Show Temperature if Sensor available
    u8g2.print("\xb0");
    u8g2.print("C");
  }
  if (pcaAvail) {
    digitalWrite(PWRLED,1);           // Power off Power LED's D2 & D3
  }
#endif

  oled.display();
  startScreenActive=true;
} // end mistertext


// --------------------------------------------------------------
// ----------------- Set ScreenSaver Mode -----------------------
// --------------------------------------------------------------
void usb2oled_readnsetscreensaver(void) {
  String TextIn="",mT="",iT="",lT="";
  int d1,d2,m,i,l;
#ifdef XDEBUG
  Serial.println("Called Command CMDSAVER");
#endif
  TextIn=newCommand.substring(9);
#ifdef XDEBUG
  Serial.printf("Received Text: %s\n", (char*)TextIn.c_str());
#endif
 
  //Searching for the "," delimiter
  d1 = TextIn.indexOf(',');                 // Find location of first ","
  d2 = TextIn.indexOf(',', d1+1 );          // Find location of second ","
  //Create Substrings
  mT = TextIn.substring(0, d1);             // Get String for Mode/Color
  iT = TextIn.substring(d1+1, d2);          // Get String for Interval
  lT = TextIn.substring(d2+1);              // Get String for Logo-Time

  m=mT.toInt();                             // Convert Mode/Color
  i=iT.toInt();                             // Convert Interval
  l=lT.toInt();                             // Convert Logo-Time

  if (m<0) m=0;                             // Check & Set Mode/Color low
  if (m>50) m=50;                           // Check & Set Mode/Color high
  if (i<5) i=5;                             // Check&Set Minimum Interval
  if (i>600) i=600;                         // Check&Set Maximiun Interval
  if (l<20) l=20;                           // Check&Set Minimum Logo-Time
  if (l>600) l=600;                         // Check&Set Maximiun Logo-Time
  
  
#ifdef XDEBUG
  Serial.printf("Created Strings: M:%s I:%s L:%s\n", (char*)mT.c_str(), (char*)iT.c_str(), (char*)lT.c_str());
  Serial.printf("Values: M:%i T:%i L:%i\n", m, i, l);
#endif

  if (m==0) {
#ifdef XDEBUG
    Serial.println("ScreenSaver Disabled!");
#endif
    ScreenSaverEnabled = false;
    ScreenSaverTimer=0;                       // Reset Screensaver-Timer
    ScreenSaverLogoTimer=0;                   // Reset ScreenSaverLogo-Timer
  }
  else{
#ifdef XDEBUG
    Serial.println("ScreenSaver Enabled!");
#endif
    ScreenSaverEnabled = true;
    //ScreenSaverActive = false;
    ScreenSaverColor = m;                     // Set ScreenSaver Color
    ScreenSaverInterval=i;                    // Set ScreenSaverTimer Interval
    ScreenSaverTimer=0;                       // Reset Screensaver-Timer
    ScreenSaverLogoTime=l-i;                  // Set ScreenSaverLogoTime (First Screensaver shown after ScreenSaverLogoTime-ScreenSaverInterval+ScreenSaverInterval)
    ScreenSaverLogoTimer=0;                   // Reset ScreenSaverLogo-Timer
  }
}


// --------------------------------------------------------------
// ------------- Show 1/4 Core Picture at Position --------------
// --------------------------------------------------------------
// xpos & ypos = Offset
void oled_showSmallCorePicture(int xpos, int ypos) {
  int x=0,y=0,px=0,py=0,i=0;
  unsigned char b;

  //Serial.printf("Show 1/4 Pic\n",actPicType);
  //actPicType = XBM;
  //actPicType = GSC;
  //Serial.printf("ActPicType: %d\n",actPicType);
  
  oled.clearDisplay();
  switch (actPicType) {
    case XBM:
      x=0;y=0;
      for (py=0; py<DispHeight; py=py+2) {
        for (px=0; px<DispLineBytes1bpp; px++) {
          b=logoBin[px+py*DispLineBytes1bpp];                // Get Data Byte for 8 Pixels
          for (i=0; i<8; i=i+2){
            if (bitRead(b, i)) {
              oled.drawPixel(xpos+x, ypos+y, SSD1322_WHITE);         // Draw Pixel if "1"
            }
            else {
              oled.drawPixel(xpos+x, ypos+y, SSD1322_BLACK);         // Clear Pixel if "0"
            }
            //Serial.printf("X: %d Y: %d\n",x,y);
            x++;
          }
#ifdef USE_NODEMCU
          yield();
#endif
        }
        x=0;
        y++;
      }
      oled.display();  
    break;
    case GSC:
      x=0;y=0;
      for (py=0; py<DispHeight; py=py+2) {
        for (px=0; px<DispLineBytes1bpp; px++) {
          for (i=0; i<4; i++) {
            b=logoBin[(px*4)+i+py*DispLineBytes4bpp];        // Get Data Byte for 2 Pixels
            oled.drawPixel(xpos+x, ypos+y, (0xF0 & b) >> 4);   // Draw only Pixel 1, Left Nibble
            //Serial.printf("X: %d Y: %d\n",x,y);
            x++;
          }
#ifdef USE_NODEMCU
          yield();
#endif
        }
      x=0;
      y++;
      }
      oled.display();  
    break;
    case NONE:
      usb2oled_showcorename();
    break;
  }
}

// --------------------------------------------------------------
// ---------------- Show ScreenSaver Picture  -------------------
// --------------------------------------------------------------
void oled_showScreenSaverPicture(void) {
  int l,x,y;
  oled.setContrast(ScreenSaverColor);  // Test
  l=random(1+2);
  switch (l) {
    case 0:
      oled.clearDisplay();
      x=random(DispWidth - mister_logo32_width);
      y=random(DispHeight - mister_logo32_height);
      oled.drawXBitmap(x, y, mister_logo32, mister_logo32_width, mister_logo32_height, SSD1322_WHITE);
      //oled.drawXBitmap(x, y, mister_logo32, mister_logo32_width, mister_logo32_height, ScreenSaverColor);
      oled.display();
    break;
    case 1:
      oled.clearDisplay();
      x=random(DispWidth - tty2oled_logo32_width);
      y=random(DispHeight - tty2oled_logo32_height);
      oled.drawXBitmap(x, y , tty2oled_logo32, tty2oled_logo32_width, tty2oled_logo32_height, SSD1322_WHITE);
      //oled.drawXBitmap(x, y , tty2oled_logo32, tty2oled_logo32_width, tty2oled_logo32_height, ScreenSaverColor);
      oled.display();
    break;
    case 2:
      x=random(DispWidth - DispWidth/2);
      y=random(DispHeight - DispHeight/2);
      oled_showSmallCorePicture(x,y);
    break;
  }
}


// --------------------------------------------------------------
// ----------- Send Hardware Info Back to the MiSTer ------------
// --------------------------------------------------------------
void oled_sendHardwareInfo(void) {
  int hwinfo=0;

#ifdef USE_ESP32DEV                        // TTGO-T8 & d.ti Board
  hwinfo=1;
#endif

#ifdef USE_LOLIN32                         // Wemos LOLIN32, LOLIN32, DevKit_V4 (Wemos Lolin32)
  hwinfo=2;
#endif

#ifdef USE_NODEMCU                         // ESP8266 NodeMCU
  hwinfo=3;
#endif

 
  delay(hwDelay);                            // Small Delay

  switch (hwinfo) {
    case 0:
      Serial.println("HWNONEXXX;" BuildVersion ";");              // No known Hardware in use
    break;
    case 1:
      Serial.println("HWESP32DE;" BuildVersion ";");              // ESP32-DEV, TTGO, DTI-Board without active Options
    break;
    case 2:
      Serial.println("HWLOLIN32;" BuildVersion ";");              // Wemos,Lolin,DevKit_V4
    break;
    case 3:
      Serial.println("HWESP8266;" BuildVersion ";");              // ESP8266
    break;
    case 4:
      Serial.println("HWDTIPCB0;" BuildVersion ";");              // DTI Board v1.0
    break;
    case 5:
      Serial.println("HWDTIPCB1;" BuildVersion ";");              // Currently unused
    break;
    default:
      Serial.println("HWNONEXXX;" BuildVersion ";");              // Default
    break;
  }
}


// --------------------------------------------------------------
// ---- Draw Pictures with an height of 64 Pixel centerred ------
// --------------------------------------------------------------
void oled_drawlogo64h(uint16_t w, const uint8_t *bitmap) {
  oled.clearDisplay();
  oled.drawXBitmap(DispWidth/2-w/2, 0, bitmap, w, DispHeight, SSD1322_WHITE);
  oled.display();
} // end oled_drawlogo64h


// --------------------------------------------------------------
// ----------------- Just show the Corename ---------------------
// --------------------------------------------------------------
void usb2oled_showcorename() {
#ifdef XDEBUG
  Serial.println("Called Command CMDSNAM");
#endif

  ScreenSaverTimer=0;                        // Reset ScreenSaver-Timer
  ScreenSaverLogoTimer=0;                    // Reset ScreenSaverLogo-Timer
  oled.setContrast(contrast);

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
  Serial.printf("\nReceived Text: %s\n", (char*)cT.c_str());
#endif
  contrast=cT.toInt();                   // Convert Value
  oled.setContrast(contrast);            // Read and Set contrast  
}

// --------------------------------------------------------------
// ---------------- Show Parameter Error ------------------------
// --------------------------------------------------------------
void usb2oled_showperror(void) {
  oled.clearDisplay();
  u8g2.setFont(u8g2_font_luBS14_tf);
  u8g2.setCursor(5, 20);
  u8g2.print("Parameter Error!");
  u8g2.setFont(u8g2_font_luBS10_tf);
  u8g2.setCursor(5, 40);
  u8g2.print(newCommand);
  oled.display();
}


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
  Serial.printf("\nReceived Text: %s\n", (char*)rT.c_str());
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
// --------------- Clear Display with Transition ----------------
// --------------------------------------------------------------
void usb2oled_clswithtransition() {
  String TextIn,tT="",cT="";
  //uint16_t w,t=0,c=0,d1=0;
  int w,t=0,c=0,d1=0;
  bool pError=false;

#ifdef XDEBUG
  Serial.println("Called Command CMDCLST");
#endif

  TextIn = newCommand.substring(8);         // Get Command Text from "newCommand"

  //Searching for the "," delimiter
  d1 = TextIn.indexOf(',');                 // Find location of first ","

  //Create Substrings
  tT = TextIn.substring(0, d1);             // Get String for Transition
  cT = TextIn.substring(d1+1);              // Get String for Draw Colour
  
#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)TextIn.c_str());
  Serial.printf("Created Strings: T:%s C:%s\n", (char*)tT.c_str(), (char*)cT.c_str());
#endif

  // Enough Parameter given / Parameter Check
  if (d1==-1) {
    pError=true;
  }

  // Convert Strings to Integer
  t = tT.toInt();
  c = cT.toInt();

#ifdef XDEBUG
  Serial.printf("Values: T:%i C:%i\n", t,c);
#endif

  // Parameter check
  if (t<-1) t=-1;
  if (t>maxEffect) t=maxEffect;
  if (c<0) c=0;
  if (c>15) c=15;
  
  if (t==-1) {
    t=random(minEffect,maxEffect+1);
  }

  actPicType=GSC;                        // Set Picture Type to GSC to enable Color Mode
  actCorename="No Core";
  for (w=0; w<logoBytes4bpp; w++) {
    logoBin[w]=(c << 4) | c;             // Fill Picture with given Color..
  }

  if (!pError) {
    usb2oled_drawlogo(t);                // ..and draw Picture to Display with Effect
  }
  else {
    usb2oled_showperror();
  }
}  // end usb2oled_clswithtransition


// --------------------------------------------------------------
// ------------------------- Showpic ----------------------------
// --------------------------------------------------------------
void usb2oled_showpic(void) {
#ifdef XDEBUG
  Serial.println("Called Command CMDSPIC");
#endif

  ScreenSaverTimer=0;                        // Reset ScreenSaver-Timer
  ScreenSaverLogoTimer=0;                    // Reset ScreenSaverLogo-Timer
  oled.setContrast(contrast);

  if (newCommand.length()>7) {                       // Parameter added?
    tEffect=newCommand.substring(8).toInt();         // Get Effect from Command String (is set to 0 if not convertable)
    if (tEffect<-1) tEffect=-1;                      // Check Effect minimum
    if (tEffect>maxEffect) tEffect=maxEffect;        // Check Effect maximum
  }
  else {
    tEffect=-1;                                      // Set Parameter to -1 (random)
  }
}

// --------------------------------------------------------------
// ----------------------- Read Logo ----------------------------
// --------------------------------------------------------------
int usb2oled_readlogo() {
  String TextIn="",tT="";
  int d1=0;
  
#ifdef XDEBUG
  Serial.println("Called Command CMDCOR");
#endif

  TextIn=newCommand.substring(7);                    // Get Command String
  d1 = TextIn.indexOf(',');                          // Search String for ","
  if (d1==-1) {                                      // No "," found = no Effect Parameter given
    actCorename=TextIn;                              // Get Corename
    tEffect=-1;                                      // Set Effect to -1 (Random)
#ifdef XDEBUG
    Serial.printf("\nReceived Text: %s, Transition T:None\n", (char*)actCorename.c_str());
#endif
  }
  else {                                             // "," found = Effect Parameter given
    actCorename=TextIn.substring(0, d1);             // Extract Corename from Command String
    tEffect=TextIn.substring(d1+1).toInt();          // Get Effect from Command String (set to 0 if not convertable)
    if (tEffect<-1) tEffect=-1;                      // Check Effect minimum
    if (tEffect>maxEffect) tEffect=maxEffect;        // Check Effect maximum
#ifdef XDEBUG
    Serial.printf("\nReceived Text: %s, Transition T:%i \n", (char*)actCorename.c_str(),tEffect);
#endif
  }
  
#ifdef USE_NODEMCU
  yield();
#endif

  // 2022-01-05 Add Cast Operator (char*) to the command
  bytesReadCount = Serial.readBytes((char*)logoBin, logoBytes4bpp);  // Read 2048 or 8192 Bytes from Serial

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
  if (((int)bytesReadCount != logoBytes1bpp) && ((int)bytesReadCount != logoBytes4bpp)) {
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
  int w,x,y,x2=0,y2=0;
  //unsigned char logoByteValue;
  //int logoByte;
  uint8_t vArray[DispLineBytes1bpp]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
  
#ifdef XDEBUG
  Serial.printf("Draw Logo with Transition Effect No. %d\n",e);
#endif

  ScreenSaverTimer=0;                        // Reset ScreenSaver-Timer
  ScreenSaverLogoTimer=0;                    // Reset ScreenSaverLogo-Timer
  oled.setContrast(contrast);

  switch (e) {
    case 1:                                  // Left to Right
      for (x=0; x<DispLineBytes1bpp; x++) {
        for (y=0; y<DispHeight; y++) {
          drawEightPixelXY(x, y);
        }
        oled.display();
      }
    break;  // 1

    case 2:                                  // Top to Bottom
      for (y=0; y<DispHeight; y++) {
        for (x=0; x<DispLineBytes1bpp; x++) {
          drawEightPixelXY(x, y);
        }
        if (y%2==1) oled.display();
      }
    break;  // 2

    case 3:                                  // Right to left
      for (x=DispLineBytes1bpp-1; x>=0; x--) {
        for (y=0; y<DispHeight; y++) {
          drawEightPixelXY(x, y);
        }
        oled.display();
      }
    break;  // 3

    case 4:                                  // Bottom to Top
      for (y=DispHeight-1; y>=0; y--) {
        for (x=0; x<DispLineBytes1bpp; x++) {
          drawEightPixelXY(x, y);
        }
        if (y%2==0) oled.display();
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
          drawEightPixelXY(x2, y);
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
          drawEightPixelXY(x2, y);
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
            drawEightPixelXY(x2, y+w*16);
          }  // end for y
          oled.display();
        }  // end for x
      }
    break;  // 7

    case 8:                                     // 4 Parts, Top-Left => Bottom-Right => Top-Right => Bottom-Left
      // Part 1 Top Left
      for (x=0; x<DispLineBytes1bpp/2; x++) {
        for (y=0; y<DispHeight/2; y++) {
          drawEightPixelXY(x, y);
        }  // end for y
        oled.display();
      }  // end for x
      // Part 2 Bottom Right
      for (x=DispLineBytes1bpp/2; x<DispLineBytes1bpp; x++) {
        for (y=DispHeight/2; y<DispHeight; y++) {
          drawEightPixelXY(x, y);
        }  // end for y
        oled.display();
      }  // end for x
      // Part 3 Top Right
      for (x=DispLineBytes1bpp-1; x>=DispLineBytes1bpp/2; x--) {
        for (y=0; y<DispHeight/2; y++) {
          drawEightPixelXY(x, y);
        }  // end for y
        oled.display();
      }  // end for x
      // Part 4 Bottom Left
      for (x=DispLineBytes1bpp/2-1; x>=0; x--) {
        for (y=DispHeight/2; y<DispHeight; y++) {
          drawEightPixelXY(x, y);
        }  // end for y
        oled.display();
      }  // end for x
    break; // 8
    
    case 9:                                      // Particle Effect NEW and faster
      for (w=0; w<1000; w++) {
        x = random(DispLineBytes1bpp);
        y = random(DispHeight/8);
        for (int offset=0; offset<8; offset++) {
          drawEightPixelXY(x, y*8+offset);
        }
        // Different speed
        if (w<=250) {
          if ((w % 5)==0) oled.display();
        }
        if (w>250 && w<=500) {
          if ((w % 10)==0) oled.display();
        }
        if (w>500) {
          if ((w % 20)==0) oled.display();
        }
      }
      // Finally overwrite the Screen with full Size Picture
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
            drawEightPixelXY(x2, y);
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
    
    case 11:                                      // Slide in left to right
      for (x=0; x<DispLineBytes1bpp; x++) {
        for (x2=DispLineBytes1bpp-1-x; x2<DispLineBytes1bpp; x2++) {
          for (y=0; y<DispHeight; y++) {
            drawEightPixelXY(x+x2-(DispLineBytes1bpp-1), y, x2, y);
          }
        }
        oled.display();
      }
    break;  // 11

    case 12:                                     // Slide in Top to Bottom
      for (y=0; y<DispHeight; y++) {
        for (y2=DispHeight-1-y; y2<DispHeight; y2++) {
          for (x=0; x<DispLineBytes1bpp; x++) {
            drawEightPixelXY(x, y+y2-(DispHeight-1), x, y2);
          }
        }
        if (y%2==1) oled.display();
      }
    break;  // 12

    case 13:                                  // Slide in Right to left
      for (x=DispLineBytes1bpp-1; x>=0; x--) {
        for (x2=DispLineBytes1bpp-1-x; x2>=0; x2--) {
          for (y=0; y<DispHeight; y++) {
            drawEightPixelXY(x+x2, y, x2, y);
          }
        }
        oled.display();
      }
    break;  // 13

    case 14:                                  // Slide in Bottom to Top
      for (y=DispHeight-1; y>=0; y--) {
        for (y2=DispHeight-1-y; y2>=0; y2--) {
          for (x=0; x<DispLineBytes1bpp; x++) {
            drawEightPixelXY(x, y+y2, x, y2); 
          }
        }
        if (y%2==0) oled.display();
      }
    break;  // 14

    case 15:                                  // Top and Bottom to Middle
      for (y=0; y<DispHeight/2; y++) {
        for (x=0; x<DispLineBytes1bpp; x++) {
          drawEightPixelXY(x, y);
          drawEightPixelXY(x, DispHeight-y-1);
        }
      oled.display();
      }
    break;  // 15

    case 16:                                  // Left and Right to Middle
      for (x=0; x<DispLineBytes1bpp/2; x++) {
        for (y=0; y<DispHeight; y++) {
          drawEightPixelXY(x, y);
          drawEightPixelXY(DispLineBytes1bpp-x-1, y);
        }
        oled.display();
      }
    break;  // 16

    case 17:                                  // Middle to Top and Bottom
      for (y=0; y<DispHeight/2; y++) {
        for (x=0; x<DispLineBytes1bpp; x++) {
          drawEightPixelXY(x, DispHeight/2-1-y);
          drawEightPixelXY(x, DispHeight/2+y);
        }
      oled.display();
      }
    break;  // 17

    case 18:                                  // Middle to Left and Right
      for (x=0; x<DispLineBytes1bpp/2; x++) {
        for (y=0; y<DispHeight; y++) {
          drawEightPixelXY(DispLineBytes1bpp/2-1-x, y);
          drawEightPixelXY(DispLineBytes1bpp/2+x, y);
        }
        oled.display();
      }
    break;  // 18
    
    case 19:                                  // Warp, Middle to Left, Right, Top and Bottom
      for (w=0; w<DispLineBytes1bpp/2; w++) {
        for (y=DispHeight/2-2-(w*2); y<=DispHeight/2+(w*2)+1; y++) {
          for (x=DispLineBytes1bpp/2-1-w; x<=DispLineBytes1bpp/2+w; x++) {
            drawEightPixelXY(x, y);
          }
        }
        oled.display();
      }
    break;  // 19

    case 20:                                  // Slightly Clockwise
      for (y=0; y<DispHeight/2;y++) {
        for (x=DispLineBytes1bpp-DispHeight/16;x<DispLineBytes1bpp;x++) {
          drawEightPixelXY(x, y);
        }
      //oled.display();  
      if (y%2==1) oled.display();             // Update only each uneven (second) round = faster 
      }
      for (x=DispLineBytes1bpp-1;x>=DispHeight/16;x--) {
        for (y=DispHeight/2; y<DispHeight;y++) {
          drawEightPixelXY(x, y);
        }
      oled.display();  
      }
      for (y=DispHeight-1; y>=DispHeight/2;y--) {
        for (x=0;x<DispHeight/16;x++) {
          drawEightPixelXY(x, y);
        }
      //oled.display();  
      if (y%2==0) oled.display();             // Update only each even (second) round = faster 
      }
      for (x=0;x<DispLineBytes1bpp-DispHeight/16;x++) {
        for (y=0; y<DispHeight/2;y++) {
          drawEightPixelXY(x, y);
        }
      oled.display();  
      }
    break;  // 20

    case 21:                                  // Shaft
      for (y=0; y<DispHeight; y++) {
        for (x=0; x<DispLineBytes1bpp; x++) {
          if ((x>=0 && x<DispLineBytes1bpp/4*1) || (x>=DispLineBytes1bpp/2 && x<DispLineBytes1bpp/4*3)) drawEightPixelXY(x, y);
          if ((x>=DispLineBytes1bpp/4*1 && x<DispLineBytes1bpp/2) || (x>=DispLineBytes1bpp/4*3 && x<DispLineBytes1bpp)) drawEightPixelXY(x, DispHeight-y-1);
        }
        //oled.display();
        if (y%2==1) oled.display();
      }
    break;
    
    case 22:                                  // Waterfall
      for (w=0; w<DispLineBytes1bpp; w++) {   // Shuffle the Array
        x2=random(DispLineBytes1bpp);
        x=vArray[w];
        vArray[w]=vArray[x2];
        vArray[x2]=x;
      }
      for (x=0; x<DispLineBytes1bpp/2;x++) {
        for (y=0; y<DispHeight; y++) {
          drawEightPixelXY(vArray[x*2], y);
          drawEightPixelXY(vArray[x*2+1], y);
          //if (y%8==7) oled.display();       // Waterfall Speed
          if (y%16==15) oled.display();       // Waterfall Speed
        }
      }
    break;  // 22

    case 23:                                  // Chess Field with 8 squares
      for (w=0; w<DispLineBytes1bpp/4; w++) { // Shuffle the Array
        x2=random(DispLineBytes1bpp/4);
        x=vArray[w];
        vArray[w]=vArray[x2];
        vArray[x2]=x;
      }
      for (w=0; w<DispLineBytes1bpp/4; w++) { // Set x,y for the fieled
        switch(vArray[w]) {
          case 0:
            x2=0; y2=0;
          break;
          case 1:
            x2=8; y2=0;
          break;
          case 2:
            x2=16; y2=0;
          break;
          case 3:
            x2=24; y2=0;
          break;
          case 4:
            x2=0; y2=32;
          break;
          case 5:
            x2=8; y2=32;
          break;
          case 6:
            x2=16; y2=32;
          break;
          case 7:
            x2=24; y2=32;
          break;
        }
        for (x=x2;x<x2+DispLineBytes1bpp/4;x++) {
          for (y=y2;y<y2+DispHeight/2;y++) {
            drawEightPixelXY(x, y);
          }
        }
        oled.display();
        delay(100);                           // Delay
      }
    break;  // 23

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


// --------------- Draw 8 Pixel to Display Buffer ----------------------
// x,y: Data Coordinates of the Pixels on the Display
// dx,dy: Data Coordinates of the Pixels in the Array
// Normaly x=dx and y=dy but for the slide effects it's different.
// 8 Pixels are written, Data Byte(s) are taken from Array
// Display Positions are calculated from x,y and Type of Pic (XBM/GSX)
// ---------------------------------------------------------------------
void drawEightPixelXY(int x, int y, int dx, int dy) {
  unsigned char b;
  int i;
  switch (actPicType) {
    case XBM:
      b=logoBin[dx+dy*DispLineBytes1bpp];                // Get Data Byte for 8 Pixels
      for (i=0; i<8; i++){
        if (bitRead(b, i)) {
          oled.drawPixel(x*8+i,y,SSD1322_WHITE);         // Draw Pixel if "1"
        }
        else {
          oled.drawPixel(x*8+i,y,SSD1322_BLACK);         // Clear Pixel if "0"
        }
      }
    break;
    case GSC:
      for (i=0; i<4; i++) {
        b=logoBin[(dx*4)+i+dy*DispLineBytes4bpp];        // Get Data Byte for 2 Pixels
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
  //int16_t x1,y1;
  //uint16_t w1,h1;
  String TextIn="", fT="", cT="", bT="", xT="", yT="", TextOut="";
  bool bufferMode=false;
  bool pError=false;
  
#ifdef XDEBUG
  Serial.println("Called Command CMDTEXT");
#endif
 
  TextIn = newCommand.substring(7);            // Get Command Text from "newCommand"
  
#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)TextIn.c_str());
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
  Serial.printf("\nCreated Strings: F:%s C%s B%s X:%s Y:%s T:%s\n", (char*)fT.c_str(), (char*)cT.c_str(), (char*)bT.c_str(), (char*)xT.c_str(), (char*)yT.c_str(), (char*)TextOut.c_str());
#endif

  // Convert Strings to Integer
  f = fT.toInt();
  c = cT.toInt();
  b = bT.toInt();
  x = xT.toInt();
  y = yT.toInt();
  
  // Parameter check
  if (f<0 || c<0 || c>15 || b<0 || b>15 || x<0 || x>DispWidth-1 || y<0 || y>DispHeight-1 || d1==-1 || d2==-1 || d3==-1 || d4==-1 || d5==-1) {
    pError=true;
  }

  if (f>=100) {                  // Do not run oled.display() after printing
    bufferMode=true;
    f=f-100;
  }

#ifdef USE_ESP32DEV
  if (TextOut=="TEP184") {      // If Text is "TEP184" replace Text with Temperature Value
    if (micAvail) {
      TextOut=String(tSensor.getTemp())+"\xb0"+"C";
    }
    else {
      TextOut="NA";
    }
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
  if (!pError) {
    // Write or Clear Text
    u8g2.setForegroundColor(c);                           // Set Font Color
    u8g2.setBackgroundColor(b);                           // Set Backgrounf Color
    u8g2.setCursor(x,y);                                  // Set Cursor Position
    u8g2.print(TextOut);                                  // Write Text to Buffer
    if (!bufferMode) oled.display();                       // Update Screen only if not Clear Mode (Font>100)
    u8g2.setForegroundColor(SSD1322_WHITE);               // Set Color back
    u8g2.setBackgroundColor(SSD1322_BLACK);
  }
  else { 
    usb2oled_showperror();
  }
}


// --------------------------------------------------------------
// ------------------ Read and Draw Geometrics ------------------
// --------------------------------------------------------------
void usb2oled_readndrawgeo(void) {
  int g=0,c=0,x=0,y=0,i=0,j=0,k=0,l=0,d1=0,d2=0,d3=0,d4=0,d5=0,d6=0,d7=0;
  String TextIn="",gT="",cT="",xT="",yT="",iT="",jT="",kT="",lT="";
  bool pError=false;
  bool bufferMode=false;
  
#ifdef XDEBUG
  Serial.println("Called Command CMDGEO");
#endif

  TextIn = newCommand.substring(7);             // Get Command Text from "newCommand"
  
#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)TextIn.c_str());
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
  Serial.printf("\nPart-Strings: G:%s C:%s X:%s Y:%s I:%s J:%s K:%s L:%s\n", (char*)gT.c_str(), (char*)cT.c_str(), (char*)xT.c_str(), (char*)yT.c_str(), (char*)iT.c_str(), (char*)jT.c_str(), (char*)kT.c_str(), (char*)lT.c_str());
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
  Serial.printf("\nValues: G:%i C:%i X:%i Y:%i I:%i J:%i K:%i L:%i\n", g,c,x,y,i,j,k,l);
#endif

  // Enough Parameter given / Parameter Check
  if (g<1 || c<0 || c>15 || x<0 || x>DispWidth-1 || y<0 || y>DispHeight-1 || d1==-1 || d2==-1 || d3==-1  || d4==-1 || d5==-1  || d6==-1 || d7==-1) {
    pError=true;
  }
  
  if (g>100) {                  // Do not run oled.display() after drawing
    bufferMode=true;
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
    if (!bufferMode) oled.display();                       // Update Screen only if not Buffer Mode (Geo>100)
  }
  else { 
    usb2oled_showperror();
  }
}


// ------------------ D.TI Board Funtions -----------------------
#ifdef USE_ESP32DEV

// --------------------------------------------------------------
// ---------------- Just show the Temperature -------------------
// --------------------------------------------------------------
void usb2oled_showtemperature() {
  String myTemp="";
#ifdef XDEBUG
  Serial.println("Called Command CMDSTEMP");
#endif
  if (micAvail) {
    myTemp=String(tSensor.getTemp())+"\xb0"+"C";
  }
  else {
    myTemp="NA";
  }
  oled.clearDisplay();
  oled.drawRoundRect(0,0,256,64,4,10);
  u8g2.setFont(u8g2_font_luBS24_tf);
  u8g2.setCursor(DispWidth/2-(u8g2.getUTF8Width(myTemp.c_str())/2), DispHeight/2 + (u8g2.getFontAscent()/2));
  u8g2.print(myTemp);
  oled.display();
}


// --------------------------------------------------------------
// ----------------------- Set User LED -------------------------
// --------------------------------------------------------------
void usb2oled_readnsetuserled(void) {
  String xT="";
  int x;
#ifdef XDEBUG
  Serial.println("Called Command CMDULED");
#endif
  
  xT=newCommand.substring(8);

#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)xT.c_str());
#endif
  
  x=xT.toInt();                                  // Convert Value
  if (x<0) x=0;
  
  if (!pcaAvail) {                               // PCA not avail = Board Rev 1.1 = LED
    if (x>1) x=1;
    digitalWrite(USER_LED,x);  
  }
  else {                                         // PCA avail = Board Rev 1.2 = WS2812B LED
    if (x>255) x=255;
    if (x==0) {
      wsleds[0] = CRGB::Black;                   // off
    }
    else {
      wsleds[0] = CHSV(x,255,255);               // color
    }
    FastLED.show();
  }
}


// --------------------------------------------------------------
// ------------- Set MIC184 Temperature Zone --------------------
// --------------------------------------------------------------
void usb2oled_settempzone(void) {
  String xZ="";
#ifdef XDEBUG
  Serial.println("Called Command CMDTZONE");
#endif
  
  xZ=newCommand.substring(9);

#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)xZ.c_str());
#endif
  if (micAvail) {
    if (xZ.toInt()==0) tSensor.setZONE(MIC184_ZONE_INTERNAL);
    if (xZ.toInt()==1) tSensor.setZONE(MIC184_ZONE_REMOTE);
    //delay(1000);
  }
}

// --------------------------------------------------------------
// ---------------------- Set Power LED -------------------------
// --------------------------------------------------------------
void usb2oled_readnsetpowerled(void) {
  String xT="";
  int x;
#ifdef XDEBUG
  Serial.println("Called Command CMDPLED");
#endif
  xT=newCommand.substring(8);
#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)xT.c_str());
#endif
  
  x=xT.toInt();                                 // Convert Value
  
  if (pcaAvail) {                               // PCA not avail = Board Rev 1.1 = LED
    if (x<0) x=0;
    if (x>1) x=1;
    digitalWrite(PWRLED,!x);                    // Need to negate Signal, Pin = 1 LED's off
  }
}

// --------------------------------------------------------------
// ------------ Play Tone using Piezo Beeper --------------------
// --------------------------------------------------------------
void usb2oled_playtone(void) {
  int d0=0,d1=0,d2=0,d3=0,d4=0,o=0,d=0,p=0;
  String TextIn="",nT="",oT="",dT="",pT="";
  note_t n;
  
#ifdef XDEBUG
  Serial.println("Called Command CMDPTONE");
#endif  
  TextIn=newCommand.substring(8);               // Start to find the first "," after the command
#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)TextIn.c_str());
#endif
  
  d0 = TextIn.indexOf(',');                 // Find location of the Starting ","
 
  ledcAttachPin(BUZZER, TONE_PWM_CHANNEL);
 
  Serial.printf("\nReceived Text: %s\n", (char*)TextIn.c_str());

  do {
    // find ","
    d1 = TextIn.indexOf(',', d0+1 );          // Find location of first ","
    d2 = TextIn.indexOf(',', d1+1 );          // Find location of second ","
    d3 = TextIn.indexOf(',', d2+1 );          // Find location of third ","
    d4 = TextIn.indexOf(',', d3+1 );          // Find location of fouth "," - Value = "-1" if not found = no more Notes available

    //Create Substrings
    nT = TextIn.substring(d0+1, d1);          // Get String for Note
    oT = TextIn.substring(d1+1, d2);          // Get String for Octave
    dT = TextIn.substring(d2+1, d3);          // Get String for Duration
    if (d4 != -1) {
      pT = TextIn.substring(d3+1, d4);        // Get String for Pause
      d0=d4;                                  // Set Index for next Note
    }
    else {
      pT = TextIn.substring(d3+1);            // Get String for Pause
    }

    // Build Note and convert Substrings to Integers
    // See https://github.com/espressif/arduino-esp32/blob/6a7bcabd6b7a33f074f93ed60e5cc4378d350b81/cores/esp32/esp32-hal-ledc.c#L141
    // NOTE_C, NOTE_Cs, NOTE_D, NOTE_Eb, NOTE_E, NOTE_F, NOTE_Fs, NOTE_G, NOTE_Gs, NOTE_A, NOTE_Bb, NOTE_B, NOTE_MAX
    nT.toUpperCase();                         // Set Note
    if (nT == "C")   n = NOTE_C;
    if (nT == "CS")  n = NOTE_Cs;
    if (nT == "D")   n = NOTE_D;
    if (nT == "EB")  n = NOTE_Eb;
    if (nT == "E")   n = NOTE_E;
    if (nT == "F")   n = NOTE_F;
    if (nT == "FS")  n = NOTE_Fs;
    if (nT == "G")   n = NOTE_G;
    if (nT == "GS")  n = NOTE_Gs;
    if (nT == "A")   n = NOTE_A;
    if (nT == "BB")  n = NOTE_Bb;
    if (nT == "B")   n = NOTE_B;
    if (nT == "MAX") n = NOTE_MAX;
    o = oT.toInt();                           // Octave
    d = dT.toInt();                           // Duration
    p = pT.toInt();                           // Pause after playing tone
#ifdef XDEBUG
    Serial.printf("Values to Play: %s %d %d %d\n", (char*)nT.c_str(), o, d, p);
#endif  
    ledcWriteNote(TONE_PWM_CHANNEL, n, o);    // Play Note
    delay(d);                                 // Duration
    ledcWriteTone(TONE_PWM_CHANNEL, 0);       // Buzzer off
    delay(p);                                 // Pause
    
  } while (d4 != -1);                         // Repeat as long a fourth "," is found

  ledcDetachPin(BUZZER);
}

// --------------------------------------------------------------
// --------- Play Frequency using Piezo Beeper ------------------
// --------------------------------------------------------------
void usb2oled_playfrequency(void) {
  int d0=0,d1=0,d2=0,d3=0,f=0,d=0,p=0;
  String TextIn="",fT="",dT="",pT="";
  
#ifdef XDEBUG
  Serial.println("Called Command CMDPFREQ");
#endif  
  TextIn=newCommand.substring(8);               // Start to find the first "," after the command
#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)TextIn.c_str());
#endif
  
  d0 = TextIn.indexOf(',');                 // Find location of the Starting ","
 
  ledcAttachPin(BUZZER, TONE_PWM_CHANNEL);
 
  Serial.printf("\nReceived Text: %s\n", (char*)TextIn.c_str());

  do {
    // find ","
    d1 = TextIn.indexOf(',', d0+1 );          // Find location of first ","
    d2 = TextIn.indexOf(',', d1+1 );          // Find location of second ","
    d3 = TextIn.indexOf(',', d2+1 );          // Find location of third "," - Value = "-1" if not found = no more Notes available

    //Create Substrings
    fT = TextIn.substring(d0+1, d1);          // Get String for Frequency
    dT = TextIn.substring(d1+1, d2);          // Get String for Duration
    if (d3 != -1) {
      pT = TextIn.substring(d2+1, d3);        // Get String for Pause
      d0=d3;                                  // Set Index in String for next Note
    }
    else {
      pT = TextIn.substring(d2+1);            // Get String for Pause
    }

    f = fT.toInt();                           // Octave
    d = dT.toInt();                           // Duration
    p = pT.toInt();                           // Pause after playing tone
#ifdef XDEBUG
    Serial.printf("Values to Play: %d %d %d\n", f, d, p);
#endif  
    ledcWriteTone(TONE_PWM_CHANNEL, f);       // Play Frequency
    delay(d);                                 // Duration
    ledcWriteTone(TONE_PWM_CHANNEL, 0);       // Buzzer off
    delay(p);                                 // Pause
   
  } while (d3 != -1);                         // Repeat as long a third "," is found

  ledcDetachPin(BUZZER);
}

#endif  // ----------- d.ti functions---------------


// -------------- ESP32 Funtions -------------------- 
#ifdef ESP32  // OTA and Reset only for ESP32
// --------------------------------------------------
// ---------------- Enable OTA ---------------------- 
// --------------------------------------------------
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
