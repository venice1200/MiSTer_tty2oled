/*
  By Venice
  Get CORENAME from MiSTer via USB-Serial-TTY Device and show CORENAME related text, Pictures or Logos
  Using forked Adafruit SSD1327 Library https://github.com/adafruit/Adafruit_SSD1327 for the SSD1322

  -- G R A Y S C A L E  E D I T I O N --

  Needed libraries for the Arduino program:
  - Adafruit GFX (*)
  - U8G2 for Adafruit GFX (*)
  - Bounce2 (*) optional, needed for the tilt-sensor
  - ESP32Time (*) needed for all ESP32 Boards 
  - MIC184 needed for the MIC145 sensor on d.ti's boards, get it from: https://github.com/venice1200/MIC184_Temperature_Sensor/releases
  - SSD1322 for Adafruit GFX, download and extract it from here: https://github.com/venice1200/SSD1322_for_Adafruit_GFX/releases/latest
  (*) These Libraries can be installed using Arduino's library manager.
  See also https://github.com/venice1200/MiSTer_tty2oled/wiki/Arduino-HowTo-%28Windows%29

  QuickSelect/Copy&Paste for Arduino IDE v2.x:
  
  -ESP32-S3 Dev Module
  -ESP32 Dev Module
  -WEMOS LOLIN32
  -NodeMCU 1.0
   
  See changelog.md in Sketch folder for more details
  
  ToDo
  -Check why dtiv>=13 (Reason = POR of PCA9536) If the Power Cycle is too short the PCA9536 locks his state
  -Everything I forgot
   
  Defines?!
  I use a lot "#ifdef's...endif" to enable code for a specific MCU Type.
  USE_ESP32S3DEV   An ESP32-S3 is used with the Arduino IDE Profile "ESP32-S3 Dev Module".
  USE_ESP32DEV     An ESP32 is used with the Arduino IDE Profile "ESP32 Dev Module".
  USE_LOLIN32      An ESP32 is used with the Arduino IDE Profile "WEMOS LOLIN32".
  USE_NODEMCU      An ESP8266 is used with the Arduino IDE Profile  "NodeMCU 1.0 (ESP-12E Module)".
  USE_ESP32XDEV    An ESP32-S3 is used with the Arduino IDE Profile "ESP32-S3 Dev Module" OR an ESP32 is used with the Arduino IDE Profile "ESP32 Dev Module".
  ESP32X           An ESP32-S3 or ESP32 is used.
 
*/

// Set Version
#define BuildVersion "230108T"                    // "T" for Testing

// Include Libraries
#include <Arduino.h>
#include <SSD1322_for_Adafruit_GFX.h>             // SSD1322 Controller Display Library https://github.com/venice1200/SSD1322_for_Adafruit_GFX
#include <U8g2_for_Adafruit_GFX.h>                // U8G2 Font Engine for Adafruit GFX  https://github.com/olikraus/U8g2_for_Adafruit_GFX
#include "bitmaps.h"                              // Some needed pictures
#include "fonts.h"                                // Some needed fonts


// ---------------------------------------------------------------------------------------------------------------------
// ------------------------------------------- Activate your Options ---------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

// How-To...
// Comment (add "//" in front of the Option) to de-activate the Option
// Uncomment (remove "//" in front of the Option) to activate the Option

// Get Debugging Infos over Serial
//#define XDEBUG

// Uncomment for 180° StartUp Rotation (Display Connector up)
//#define XROTATE

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------- Auto-Board-Config via Arduino IDE Board Selection --------------------------------
// ----------------------------------- Make sure the Manual-Board-Config is not active ---------------------------------
// ---------------------------------------------------------------------------------------------------------------------

#ifdef ARDUINO_ESP32S3_DEV        // Set Arduino Board to "ESP32-S3 Dev Module"
  #define USE_ESP32S3DEV          // ESP32-S3-DevKitC-1
#endif

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

//#define USE_ESP32S3DEV         // ESP32-S3-DevKitC-1
//#define USE_ESP32DEV           // TTGO-T8. Set Arduino Board to ESP32 Dev Module, xx MB Flash, def. Part. Schema
//#define USE_LOLIN32            // Wemos LOLIN32, LOLIN32, DevKit_V4. Set Arduino Board to "WEMOS LOLIN32"
//#define USE_NODEMCU            // ESP8266 NodeMCU v3. Set Arduino Board to NodeMCU 1.0 (ESP-12E Module)


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------- Is the System ESP32 / ESP32-S3 based? --------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

#if defined(ESP32) || defined(ESP32S3)
  #define ESP32X
#endif

#if defined(USE_ESP32DEV) || defined(USE_ESP32S3DEV)
  #define USE_ESP32XDEV
#endif

// ---------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------- Hardware-Config --------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

// ESP32-S3
// SPI Original: CS(FSPICS0) = 10, MOSI(FSPID) = 11, SCLK(FSPICLK) = 12, MISO(FSPIQ) = 13
#ifdef USE_ESP32S3DEV
  int cDelay = 60;                 // Command Delay in ms for ACK-Handshake
  #define OLED_MOSI 1
  #define OLED_MISO 45 
  #define OLED_SCLK 39
  #define OLED_CS 18
  #define OLED_DC 17
  #define OLED_RESET 8
  #define TILT_PIN 15
  #define I2C1_SDA 37              // I2C_1-SDA
  #define I2C1_SCL 36              // I2C_1-SCL
  #define USER_LED 40              // USER_LED/WS2812B Pin on d.ti Board
  //#define USER_LED 48              // USER_LED/WS2812B Pin on ESP32-S3-DevKitC-1
  #define POWER_LED 38             // Set Pin to "1" = LED's off
  #define BUZZER 35                // Piezo Buzzer
  #define TONE_PWM_CHANNEL 0       // See: https://makeabilitylab.github.io/physcomp/esp32/tone.html
  #include <MIC184.h>              // MIC184 Library, get from https://github.com/venice1200/MIC184_Temperature_Sensor
  MIC184 tSensor;                  // Create Sensor Class
  #include <FastLED.h>             // FastLED Library, get from Library Manager
  #define NUM_WSLEDS 1             // Number of WS-LED's
  #define WS_BRIGHTNESS 50         // WS-LED Brightness
  CRGB wsleds[NUM_WSLEDS];         // Create WS-LED RGB Array
  #define PCA_POWER 48             // Switch PCA9536 on=0 and off=1 (without defining the Port as output the PCA is off)
 //SPIClass OLED_SPI(HSPI);
  SPIClass OLED_SPI(SPI);
  #include <Preferences.h>         // Needed for d.ti Board Revisions
  Preferences prefs;
#endif

// TTGO-T8/d.ti/ESP32 Dev
// OLED Pins, Tilt Pin, I2C, User-LED for d.ti Board
// using VSPI SCLK = 18, MISO = 19, MOSI = 23 and...
#ifdef USE_ESP32DEV
  int cDelay = 15;                 // Command Delay in ms for ACK-Handshake
  #define OLED_CS 26               // OLED Chip Select Pin
  #define OLED_DC 25               // OLED Data/Command Pin
  #define OLED_RESET 27            // OLED Reset Pin
  #define I2C1_SDA 17              // I2C_1-SDA
  #define I2C1_SCL 16              // I2C_1-SCL
  #define TILT_PIN 32              // Using internal PullUp
  #define USER_LED 19              // USER_LED/WS2812B
  #define POWER_LED 5              // Set Pin to "1" = LED's off
  #define BUZZER 4                 // Piezo Buzzer
  #define TONE_PWM_CHANNEL 0       // See: https://makeabilitylab.github.io/physcomp/esp32/tone.html
  #include <MIC184.h>              // MIC184 Library, get from https://github.com/venice1200/MIC184_Temperature_Sensor
  MIC184 tSensor;                  // Create Sensor Class
  #include <FastLED.h>             // FastLED Library, get from Library Manager
  #define NUM_WSLEDS 1             // Number of WS-LED's
  #define WS_BRIGHTNESS 50         // WS-LED Brightness
  CRGB wsleds[NUM_WSLEDS];         // Create WS-LED RGB Array
  #define PCA_POWER 2              // Switch PCA9536 on=0 and off=1 (without defining the Port as output the PCA is off)
  #include <Preferences.h>         // Needed for d.ti Board Revisions
  Preferences prefs;
#endif

// WEMOS LOLIN32/Devkit_V4 using VSPI SCLK = 18, MISO = 19, MOSI = 23, SS = 5 and...
#ifdef USE_LOLIN32
  int cDelay = 60;                 // Command Delay in ms for ACK-Handshake
  #define OLED_CS 5
  #define OLED_DC 16
  #define OLED_RESET 17
  #define TILT_PIN 32
#endif

// ESP8266-Board (NodeMCU v3)
#ifdef USE_NODEMCU
  int cDelay = 60;                 // Command Delay in ms for ACK-Handshake
  #define OLED_CS 15
  #define OLED_DC 4
  #define OLED_RESET 5
  #define TILT_PIN 10
  //#define TILT_PIN 16
#endif

// Create OLED Object
#ifdef USE_ESP32S3DEV
Adafruit_SSD1322 oled(256, 64, &OLED_SPI, OLED_DC, OLED_RESET, OLED_CS);
#else
// Hardware Constructor OLED Display and U8G2 Support
Adafruit_SSD1322 oled(256, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS);
#endif
U8G2_FOR_ADAFRUIT_GFX u8g2;

// Tilt Sensor
#include <Bounce2.h>                     // << Extra Library, via Arduino Library Manager
#define DEBOUNCE_TIME 25                 // Debounce Time
Bounce RotationDebouncer = Bounce();     // Create Bounce class


// OTA, Reset and RTC only for ESP32
#ifdef ESP32X
  #include <ESP32Time.h>                          // Time-Library
  ESP32Time rtc;                                  // Create Real-Time-Clock Device
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
bool timeIsSet = false;                // Time was set from external ?
bool runsTesting = false;              // Testing ?

// Display Vars
uint16_t DispWidth, DispHeight, DispLineBytes1bpp, DispLineBytes4bpp;
int logoBytes1bpp=0;
int logoBytes4bpp=0;
//unsigned int logoBytes1bpp=0;
//unsigned int logoBytes4bpp=0;
const int hwDelay=100;                        // Delay for HWINFO Request
size_t bytesReadCount=0;
//uint8_t *logoBin;                             // <<== For malloc in Setup
uint8_t logoBin[8192];                        // fixed definition
enum picType {NONE, XBM, GSC, TXT};           // Enum Picture Type
int actPicType=NONE;
int16_t xs, ys;
uint16_t ws, hs;
const uint8_t minEffect=1, maxEffect=23;      // Min/Max Effects for Random
//const uint8_t minEffect=22, maxEffect=23;   // Min/Max Effects for TESTING

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
int ScreenSaverMode=0;                       // ScreenSaver Drawing Color
int ScreenSaverLogoTimer=0;                  // ScreenSaverLogo-Timer
int ScreenSaverLogoTime=60;                  // ScreenSaverLogoTime
#ifdef USE_NODEMCU
const int ScreenSaverMaxScreens=3;           // Max ScreenSavers ESP8266 => 3 (bit)
#endif
#ifdef ESP32X
const int ScreenSaverMaxScreens=5;           // Max ScreenSavers ESP32 => 5 (bit)
#endif
int ScreenSaverActiveScreens[ScreenSaverMaxScreens]; // Array contains Pointer to Active ScreeenSavers (1=tty2oled,2=MiSTer,3=Core,4=Time,5=Date)
int ScreenSaverCountScreens=0;               // How many ScreenSaver Screens are Active?
const int ScreenSaverContrast=1;             // Contrast Value for ScreenSaver Mode

// Animated Screensaver only for ESP32
#ifdef ESP32X
bool ShowScreenSaverAnimated=false;
#define MinAnimatedScreenSaver 1
#define MaxAnimatedScreenSaver 2
int ShowAnimatedScreenSaverNo=MinAnimatedScreenSaver;

// Star Field Simulation
bool ShowScreenSaverStarField=false;          // Star Field ScreenSaver yes/no
const int starCount = 512;                    // Number of Stars in the Star Field ESP32
const int maxDepth = 32;                      // Maximum Distance away for a Star
double stars[starCount][3];                   // The Star Field - StarCount Stars represented as X, Y and Z Cooordinates
#define SCRSTARS 1

// Flying Toaster
bool ShowScreenSaverToaster=false;            // Flying Toasters ScreenSaver yes/no
#define TOAST_FLYERS   5 // Number of flying things
#define TOAST_MPIX 16    // Micropixel
#define TOAST_DELAY 50   // Toaster Delay
struct Flyer {       // Array of flying things
  int16_t x, y;      // Top-left position * 16 (for subpixel pos updates)
  int8_t  depth;     // Stacking order is also speed, 12-24 subpixels/frame
  uint8_t frame;     // Animation frame; Toasters cycle 0-3, Toast=255
} flyer[TOAST_FLYERS];
#define SCRTOASTER 2
#endif  // ESP32 Screensaver

// I2C Hardware
bool hasMIC=false;                            // tty2oled has a MIC184 Sensor (all d.to Boards)
const byte PCA9536_ADDR = 0x41;               // PCA9536 Base Address
const byte PCA9536_IREG = 0x00;               // PCA9536 Input Register
bool hasPCA=false;                            // tty2oled has a PCA9536 Port-Extender (d.ti Board >=v1.2)
//byte pcaInputValue=255;                     // PCA9536 Input Pin State as Byte Value
byte pcaInputValue=0;                         // PCA9536 Input Pin State as Byte Value
byte dtiv=0;                                  // d.ti Board Version 11=1.1, 12=1.2
byte edtiv=0;                                 // d.ti Board Version read/write from/to EERPOM 
bool usePREFS=false;                          // Got d.ti Board version from "Preferences" Values
bool hasLED=false;                            // tty2oled has a LED (d.ti Board = v1.1)
bool hasRGBLED=false;                         // tty2oled has a RGB LED (d.ti Board >=v1.2)
bool hasBUZZER=false;                         // tty2oled has a Buzzer (d.ti Board >=v1.2)
bool hasPLED=false;                           // tty2oled has a PowerLED (d.ti Board >=v1.2)


// =============================================================================================================
// ========================================== FUNCTION PROTOTYPES ==============================================
// =============================================================================================================

void oled_showStartScreen(void);
void oled_setTime(void);
void oled_setcdelay(void);
void oled_showcdelay(void);
void oled_switchscreensaver(void);
void oled_readnsetscreensaver(void);
void oled_showScreenSaverPicture(void);
void oled_showSmallCorePicture(int xpos, int ypos);
void oled_showSystemHardware(void);
void oled_sendHardwareInfo(void);
void oled_drawlogo64h(uint16_t w, const uint8_t *bitmap);
void oled_showcorename();
void oled_displayoff(void);
void oled_displayon(void);
void oled_updatedisplay(void);
void oled_readnsetcontrast(void);
void oled_showperror(void);
void oled_showcenterredtext(String text, int font);
void oled_setfont(int font);
void oled_readnsetrotation(void);
void oled_clswithtransition();
void oled_showpic(void);
int oled_readlogo();
void oled_drawlogo(uint8_t e);
void oled_drawEightPixelXY(int x, int y, int dx, int dy);
void oled_readnwritetext(void);
void oled_readndrawgeo(void);
void oled_showtemperature();
void oled_readnsetuserled(void);
void oled_settempzone(void);
void oled_readnsetpowerled(void);
void oled_playnote(void);
void oled_playtone(void);
void oled_drawScreenSaverStarField(void);
void oled_showtime(void);
void oled_enableOTA (void);
int getRandom(int lower, int upper);
void oled_drawScreenSaverStarField(void);
void oled_drawScreenSaverToaster(void);
void oled_readnsetedtiv(void);

// Info about overloading found here
// https://stackoverflow.com/questions/1880866/can-i-set-a-default-argument-from-a-previous-argument
inline void oled_drawEightPixelXY(int x, int y) { oled_drawEightPixelXY(x,y,x,y); };

// =============================================================================================================
// ================================================ SETUP ======================================================
// =============================================================================================================

void setup(void) {
  String bversion = BuildVersion;

  if (bversion.endsWith("T")) runsTesting=true;     // Running Testing Yes/No?

  Serial.begin(115200);                      // Init Serial with 115200 for MiSTer ttyUSBx Device CP2102 Chip on ESP32
  Serial.flush();                            // Wait for empty Send Buffer
  Serial.setTimeout(500);                    // Set max. Serial "Waiting Time", default = 1000ms

  randomSeed(analogRead(34));                // Init Random Generator with empty Port Analog value
  
#ifdef ESP32X  
  rtc.setTime(1640995200);                   // Set Time (2022-01-01) only for ESP32
#endif
  
  // Init SPI for ESP32-S3
#ifdef USE_ESP32S3DEV
  OLED_SPI.begin(OLED_SCLK, OLED_MISO, OLED_MOSI, OLED_CS);
#endif

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
  // u8g2.setFontMode(1);                             // Transparent Font Mode, Background is transparent
  u8g2.setFontMode(0);                               // Non-Transparent Font Mode, Background is overwritten (u8g2 default)
  u8g2.setForegroundColor(SSD1322_WHITE);            // apply Adafruit GFX color
  //u8g2.setBackgroundColor(SSD1322_BLACK);

   // Get Display Dimensions
  DispWidth = oled.width();
  DispHeight = oled.height();
  DispLineBytes1bpp = DispWidth / 8;                       // How many Bytes uses each Display Line at 1bpp (32 byte for width 256 Pixel)
  DispLineBytes4bpp = DispWidth / 2;                       // How many Bytes uses each Display Line at 4bpp (128 byte for width 256 Pixel)
  logoBytes1bpp = DispWidth * DispHeight / 8;              // SSD1322 = 2048 Bytes
  logoBytes4bpp = DispWidth * DispHeight / 2;              // SSD1322 = 8192 Bytes
  //logoBin = (uint8_t *) malloc(logoBytes4bpp);             // Create Picture Buffer, better than permanent create (malloc) and destroy (free)

// === Activate Options ===

// Setup d.ti Board (Temp.Sensor/USER_LED/PCA9536)
#ifdef USE_ESP32XDEV                                             // Only for ESP32-DEV or ESP32S3-DEV (TTGO-T8/d.ti)
// Output for d.ti Board Power LED
  pinMode(POWER_LED, OUTPUT);                                    // Set Mode POWER_LED Pin

// I2C Start  
  //Wire.begin(int(I2C1_SDA), int(I2C1_SCL), uint32_t(100000));    // Setup I2C-1 Port
  Wire.begin(int(I2C1_SDA), int(I2C1_SCL));                        // Setup I2C-1 Port
  
// MIC184 Handling  
  Wire.beginTransmission (MIC184_BASE_ADDRESS);                  // Check for MIC184 Sensor...
  if (Wire.endTransmission() == 0) {                             // ..and wait for Answer
    hasMIC=true;                                               // If Answer OK Sensor available
    dtiv=11;                                                     // d.ti Board >= 1.1
  }
  //tSensor.setZone(MIC184_ZONE_REMOTE);                         // Remote = use External Sensor using LM3906/MMBT3906
#ifdef XDEBUG
  if (hasMIC) {
    Serial.println("MIC184 Sensor available.");
    Serial.print("Temperature: ");
    Serial.print(tSensor.getTemp());
    Serial.println("°C");
  }
  else {
    Serial.println("MIC184 Sensor not available.");
  }
#endif  // XDEBUG

// PCA9536 handling
#ifdef XDEBUG
  Serial.println("Enable PCA");
#endif  // XDEBUG
  digitalWrite(PCA_POWER,0);                                     // Pre-Set Pin
  pinMode(PCA_POWER, OUTPUT);                                    // Set Mode for PCA_POWER Pin
  delay(50);
  digitalWrite(PCA_POWER,1);                                     // Switch PCA On
  delay(50);

  Wire.beginTransmission(PCA9536_ADDR);                          // Check for PCA9536
  if (Wire.endTransmission() == 0) {                             // ..and wait for Answer
    hasPCA=true;                                                 // If Answer OK PCA available
  }
#ifdef XDEBUG
  if (hasPCA) {
    Serial.println("PCA available.");
  }
  else {
    Serial.println("PCA not available.");
  }
#endif  // XDEBUG

  if (hasPCA) {                                                 // If PCA9536 available..
    Wire.beginTransmission(PCA9536_ADDR);                       // start transmission and.. 
    Wire.write(PCA9536_IREG);                                   // read Register 0 (Input Register).
    if (Wire.endTransmission() == 0) {                          // If OK...
      //delay(50);                                              // Just wait 50ms
      Wire.requestFrom(PCA9536_ADDR, byte(1));                  // request one byte from PCA
      if (Wire.available() == 1) {                              // If just one byte is available,
        pcaInputValue = Wire.read() & 0x0F;                     // read it and mask the higher bits out
        dtiv=12+pcaInputValue;                                  // d.ti Board >= 1.2
#ifdef XDEBUG
        Serial.printf("Get PCA Input Register Value: %i.\n", pcaInputValue);
#endif
      }
      else {
        while (Wire.available()) Wire.read();                   // If more byte are available = something wrong
#ifdef XDEBUG
       Serial.println("PCA Error, too much bytes!");
#endif  // XDEBUG
      }
    }
  }
#ifdef XDEBUG
  Serial.println("Disable PCA");
#endif  // XDEBUG
  digitalWrite(PCA_POWER,0);                                     // Switch PCA off

// Preferences dtiv (EEPROM)
  prefs.begin("tty2oled", false);                                // Preferences Handling, open Namespace "tty2oled" in RW Mode
  edtiv=prefs.getUChar("dtiv", 255);                             // Preferences Handling, read "dtiv" with default value "255"
#ifdef XDEBUG
  Serial.printf("Read DTIV %i from Preferences.\n", editv);
#endif  // XDEBUG
  if (edtiv==255) {                                              // Read PCA Value only if EEPROM has no Value
    prefs.putUChar("dtiv", dtiv);                                // Write Value to Preferences Namespace
    prefs.end();                                                 // Close the Preferences
    if (dtiv>10) {
      u8g2.setFont(u8g2_font_5x7_mf);                            // 6 Pixel Font
      u8g2.setCursor(0,63);
      u8g2.printf("d.ti Board v%.1f detected.", (float)dtiv/10);    
      oled.display();
      delay(4000);
      oled.clearDisplay();
    }
#ifdef XDEBUG
    Serial.printf("Writing DTIV to Preferences: %i\n", dtiv);
#endif
  }
  else {
    dtiv=edtiv;                                                    // Use EEPROM Value for dtiv
    usePREFS=true;
  }

  if (dtiv==11) {                                                  // If d.ti Board Rev 1.1
    pinMode(USER_LED, OUTPUT);                                     // Setup User LED
    hasLED=true;                                                   // tty2oled has a LED (d.ti Board v1.1)
  }
  if (dtiv>=12) {                                                  // If d.ti Board Rev 1.2 or greater
    FastLED.addLeds<WS2812B, USER_LED, GRB>(wsleds, NUM_WSLEDS);   // Setup User WS2812B LED
    FastLED.setBrightness(WS_BRIGHTNESS);                          // and set Brightness
    hasRGBLED=true;                                                // tty2oled has a RGB LED (d.ti Board >=v1.2)
    hasBUZZER=true;                                                // tty2oled has a Buzzer (d.ti Board >=v1.2)
    hasPLED=true;                                                  // tty2oled has a PowerLED which can be switched off
  }
#endif  // USE_ESP32DEV

// Setup Animated Screensaver
#ifdef ESP32X
  for (int i = 0; i < starCount; i++) {                            // Initialise the StarField with random Stars
    stars[i][0] = getRandom(-25, 25);
    stars[i][1] = getRandom(-25, 25);
    stars[i][2] = getRandom(0, maxDepth);
  }

  for (int i=0; i<TOAST_FLYERS; i++) {                             // Randomize initial flyer states
    flyer[i].x     = (-32 + random(255+32)) * TOAST_MPIX;
    flyer[i].y     = (-32 + random(63+32)) * TOAST_MPIX;
    flyer[i].frame = random(3) ? random(4) : 255;                  // 66% toaster, else toast
    flyer[i].depth = 10 + random(TOAST_MPIX);                      // Speed and stacking order
  }
#endif // ESP32X

// Tilt Sensor Rotation via Tilt-Sensor Pin
  RotationDebouncer.attach(TILT_PIN,INPUT_PULLUP);         // Attach the debouncer to a pin with INPUT mode
  RotationDebouncer.interval(DEBOUNCE_TIME);               // Use a debounce interval of 25 milliseconds
  delay(10);                                               // Short Delay
  if (digitalRead(TILT_PIN)) {                             // Set Startup Rotation
    oled.setRotation(0);                                   // If Signal = 1 no Rotation
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
  //Serial.println("ttyrdy;");                             // Send "ttyrdy;" with "\n" after setup is done.
}

// =============================================================================================================
// =============================================== MAIN LOOP ===================================================
// =============================================================================================================
void loop(void) {
  unsigned long currentMillis = millis();

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
      oled_drawlogo(0);
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
      oled_drawlogo(0);
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

/*
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
*/

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

    // -- Test Commands --
    else if (newCommand=="CMDNULL") {                                    // NULL-Command, RunTime Test Command
        // Do nothing
    }

    else if (newCommand=="cls") {                                        // Clear Screen
      oled.clearDisplay();
      oled.display();
    }
    
    else if (newCommand=="sorg") {                                       // Start Screen
      oled_showStartScreen();
    }
    
    else if (newCommand=="bye")  {                                        // Cat Screen
      oled_drawlogo64h(sorgelig_icon64_width, sorgelig_icon64);
    }
    
    else if (newCommand=="CMDCLS") {                                        // Clear Screen with Display Update
      oled.clearDisplay();
      oled.display();
    }
    
    else if (newCommand=="CMDCLSWU") {                                      // Clear Screen without Display Update
      oled.clearDisplay();
    }

    else if (newCommand.startsWith("CMDCLST")) {                            // Clear/Fill Screen with Transition and Color
      oled_clswithtransition();
    }
    
    else if (newCommand=="CMDSORG") {                                       // Show Startscreen
      oled_showStartScreen();
    }
    
    else if (newCommand=="CMDBYE") {                                        // Show Sorgelig's Icon
      oled_drawlogo64h(sorgelig_icon64_width, sorgelig_icon64);
    }

    else if (newCommand.startsWith("CMDSECD")) {                            // Set Command Delay
      oled_setcdelay();
    }

    else if (newCommand=="CMDSHCD") {                                       // Show Command Delay
      oled_showcdelay();
    }

    else if (newCommand=="CMDSHSYSHW") {                                    // Show System HW
      oled_showSystemHardware();
    }

    else if (newCommand=="CMDHWINF") {                                      // Send HW Info
      oled_sendHardwareInfo();
    }

    else if (newCommand=="CMDTEST") {                                       // Show Test-Picture
      oled_drawlogo64h(TestPicture_width, TestPicture);
    }

    else if (newCommand=="CMDSNAM") {                                       // Show actual loaded Corename
      oled_showcorename();
    }

    else if (newCommand.startsWith("CMDSPIC")) {                            // Show actual loaded Picture with(without Transition
      oled_showpic();
      if (tEffect==-1) {                                                    // Send without Effect Parameter or Parameter = -1
        oled_drawlogo(random(minEffect,maxEffect+1));                       // ...and show them on the OLED with Transition Effect 1..MaxEffect
      } 
      else {                                                                // Send with Effect "CMDSPIC,15"
        oled_drawlogo(tEffect);
      }
    }

    else if (newCommand=="CMDSSCP") {                                       // Show actual loaded Core Picture but in 1/4 size
      oled_showSmallCorePicture(64,16);
    }

    else if (newCommand=="CMDDOFF") {                                       // Switch Display Off
      oled_displayoff();
    }

    else if (newCommand=="CMDDON") {                                        // Switch Display On
      oled_displayon();
    }

    else if (newCommand=="CMDDUPD") {                                       // Update Display Content
      oled_updatedisplay();
    }

    else if (newCommand.startsWith("CMDTXT")) {                             // Command from Serial to write Text
      oled_readnwritetext();                                                // Read and Write Text
    }
    
    else if (newCommand.startsWith("CMDGEO")) {                             // Command from Serial to draw geometrics
      oled_readndrawgeo();                                                  // Read and Draw Geometrics
    }

    else if (newCommand.startsWith("CMDAPD")) {                             // Command from Serial to receive Picture Data via USB Serial from the MiSTer
      oled_readlogo();                                                      // Receive Picture Data... 
    }

    else if (newCommand.startsWith("CMDCOR")) {                             // Command from Serial to receive Picture Data via USB Serial from the MiSTer
      if (oled_readlogo()==1) {                                             // Receive Picture Data... 
        if (tEffect==-1) {                                                  // Send without Effect Parameter or with Effect Parameter -1
          oled_drawlogo(random(minEffect,maxEffect+1));                     // ...and show them on the OLED with Transition Effect 1..MaxEffect
        } 
        else {                                                              // Send with Effect "CMDCOR,llander,15"
          oled_drawlogo(tEffect);
        }
      }
    }

    else if (newCommand.startsWith("CMDSETTIME,")) {                        // Set date and time but only for ESP32 RTC
      oled_setTime();
    }

    else if (newCommand.startsWith("CMDCON")) {                            // Command from Serial to receive Contrast-Level Data from the MiSTer
      oled_readnsetcontrast();                                             // Read and Set contrast                                   
    }

    else if (newCommand.startsWith("CMDROT")) {                            // Command from Serial to set Rotation
      oled_readnsetrotation();                                             // Set Rotation
    }
    
    else if (newCommand.startsWith("CMDSWSAVER")) {                        // Command from Serial to set Screensaver
      oled_switchscreensaver();                                          // Enable/Disable Screensaver
    }
    
    else if (newCommand.startsWith("CMDSAVER")) {                          // Command from Serial to set Screensaver
      oled_readnsetscreensaver();                                          // Set Screensaver Settings & Enable/Disable
    }

// ---------------------------------------------------
// The following Commands are only for the d.ti Board
// ---------------------------------------------------
#ifdef USE_ESP32XDEV
    else if (newCommand.startsWith("CMDULED")) {                           // User LED
      oled_readnsetuserled();                                              // Set LED
    }
    
    else if (newCommand.startsWith("CMDPLED")) {                           // Power Control LED
      oled_readnsetpowerled();                                             // Set LED
    }

    else if (newCommand=="CMDSHTEMP") {                                    // Enable to show Temperature Big Picture
    oled_showtemperature();
    }

    else if (newCommand.startsWith("CMDTZONE")) {                          // Set Temperature Zone
    oled_settempzone();
    }

    else if (newCommand.startsWith("CMDPNOTE")) {                          // Play Note
    oled_playnote();
    }

    else if (newCommand.startsWith("CMDPTONE")) {                          // Play Tone/Frequency
    oled_playtone();
    }

    else if (newCommand.startsWith("CMDWRDTIV")) {                          // Write EEPROM
      oled_readnsetedtiv();
    }
#endif  // USE_ESP32DEV
// ---------------------------------------------------

// ---------------------------------------------------
// The following Commands are only for ESP32 Boards
// ---------------------------------------------------
#ifdef ESP32X  // Reset only for ESP32
    else if (newCommand=="CMDRESET") {                                      // Command from Serial for Resetting the ESP
      ESP.restart();                                                        // Reset ESP
    }

    else if (newCommand=="CMDSHTIME") {                                     // ShowTime
      oled_showtime();
    }
    
#endif  // ESP32

// ---------------------------------------------------
// -- Unidentified Core Name, just write it on screen
// ---------------------------------------------------
    else {
      actCorename=newCommand;
      actPicType=NONE;
      ScreenSaverTimer=0;                        // Reset ScreenSaver-Timer
      ScreenSaverLogoTimer=0;                    // Reset ScreenSaverLogo-Timer
      oled_showcorename();
    }  // end ifs

    delay(cDelay);                                    // Command Response Delay
    Serial.print("ttyack;");                          // Handshake with delimiter; MiSTer: "read -d ";" ttyresponse < ${TTYDEVICE}"
    // Serial.flush();                                // Wait for sendbuffer is clear

    updateDisplay=false;                              // Clear Update-Display Flag
  } // endif updateDisplay

// ---------------------------------------------------
// ---------- ScreenSaver if Active -----------------
// ---------------------------------------------------
  // ScreenSaver Logo-Timer
  if (ScreenSaverEnabled && !ScreenSaverActive && blinkpos) ScreenSaverLogoTimer++;
  ScreenSaverActive = (ScreenSaverLogoTimer>=ScreenSaverLogoTime) && ScreenSaverEnabled;
  
  // ScreenSaver Timer
  if (ScreenSaverActive && blinkpos) ScreenSaverTimer++;
  ScreenSaverPos = (ScreenSaverTimer == ScreenSaverInterval) && blinkpos;
  if (ScreenSaverTimer>=ScreenSaverInterval) ScreenSaverTimer=0;

#ifdef USE_NODEMCU
  if (ScreenSaverActive && ScreenSaverPos) {    // Screensaver each 60secs
    oled_showScreenSaverPicture();
  }
#endif

#ifdef ESP32X
  if (ScreenSaverActive && !ShowScreenSaverStarField && !ShowScreenSaverToaster && !ShowScreenSaverAnimated && ScreenSaverPos) {    // Screensaver each 60secs
    oled_showScreenSaverPicture();
  }
  if (ScreenSaverActive && (ShowScreenSaverStarField || (ShowScreenSaverAnimated && (ShowAnimatedScreenSaverNo==SCRSTARS) ))) {                       // StarField ScreenSaver
    oled_drawScreenSaverStarField();
  }
  if (ScreenSaverActive && (ShowScreenSaverToaster || (ShowScreenSaverAnimated && (ShowAnimatedScreenSaverNo==SCRTOASTER) ))) {                         // Flying Toasters ScreenSaver
    oled_drawScreenSaverToaster();
  }
#endif

// ---------------------------------------------------
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
  for (int i=0; i<DispWidth; i+=16) {            // Some Animation
    oled.fillRect(i,55,16,8,color);
    color++;
    oled.display();
#ifdef USE_ESP32XDEV
    if (dtiv>=12) {                              // Let the RGB LED light up
      wsleds[0] = CHSV(i,255,255);
      FastLED.show();
    }
#endif
    delay(20);
  }
  for (int i=0; i<DispWidth; i+=16) {            // Remove Animation Line
    oled.fillRect(i,55,16,8,SSD1322_BLACK);
    oled.display();
#ifdef USE_ESP32XDEV
    if (dtiv>=12) {                              // Let the RGB LED light up
      wsleds[0] = CHSV(255-i,255,255);
      FastLED.show();
    }
#endif
    delay(20);
  }
#ifdef USE_ESP32XDEV
  if (dtiv>=12) {
    digitalWrite(POWER_LED,1);                   // Power off Power LED's D2 & D3
    wsleds[0] = CRGB::Black;                     // RGB LED off
    FastLED.show();
  }
#endif
  delay(500);
  u8g2.setFont(u8g2_font_5x7_mf);               // 6 Pixel Font
  u8g2.setCursor(0,63);
  u8g2.print(BuildVersion);
  if (runsTesting) {
    if (hasMIC) u8g2.print("M");
    if (hasPCA) u8g2.print("P");
    if (dtiv>10) u8g2.print(dtiv);
    if (usePREFS) u8g2.print("E");
    oled.drawXBitmap(DispWidth-usb_icon_width, DispHeight-usb_icon_height, usb_icon, usb_icon_width, usb_icon_height, SSD1322_WHITE);
  }

#ifdef USE_ESP32XDEV
  if (hasMIC) {
    u8g2.setCursor(111,63);
    u8g2.print(tSensor.getTemp());    // Show Temperature if Sensor available
    u8g2.print("\xb0");
    u8g2.print("C");
  }
#endif

  oled.display();
  startScreenActive=true;
} // end mistertext


// --------------------------------------------------------------
// ---------------- Read and set RTC Time -----------------------
// --------------------------------------------------------------
void oled_setTime(void) {
  String tT="";
  
#ifdef XDEBUG
  Serial.println("Called Command CMDSETTIME");
#endif

  tT=newCommand.substring(newCommand.indexOf(',')+1);             // Get Command Parameter out of the string
  
#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)newCommand.c_str());
  Serial.printf("Received Value: %s\n", (char*)tT.c_str());
#endif

#ifdef ESP32X                                                      // Set Time only for ESP32 MCU's
  rtc.setTime(tT.toInt());                                        // Read and set RTC
  timeIsSet = true;                                               // Time is set!
#endif
}


// --------------------------------------------------------------
// ------------ Read and Set Command Delay ----------------------
// --------------------------------------------------------------
void oled_setcdelay(void) {
  String dT="";
#ifdef XDEBUG
  Serial.println("Called Command CMDSECD");
#endif
  
  dT=newCommand.substring(8);           // CMD-Length+1

#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)dT.c_str());
#endif
  cDelay=dT.toInt();                   // Convert Value
#ifdef XDEBUG
  Serial.printf("\nSet cDelay to: %d\n", cDelay);
#endif
}


// --------------------------------------------------------------
// ------------- Show Command Delay on Screen -------------------
// --------------------------------------------------------------
void oled_showcdelay(void) {
#ifdef XDEBUG
  Serial.println("Called Command CMDSHCD");
#endif
  
  oled.clearDisplay();
  u8g2.setFont(u8g2_font_tenfatguys_tr);
  u8g2.setCursor(20,32);
  u8g2.print("cDelay: ");
  u8g2.print(cDelay);
  oled.display();
}


// --------------------------------------------------------------
// ---------------- Switch Screensaver On/Off -------------------
// --------------------------------------------------------------
void oled_switchscreensaver(void) {
  String xT="";
  int x;
#ifdef XDEBUG
  Serial.println("Called Command CMDSWSAVER");
#endif
  xT=newCommand.substring(newCommand.indexOf(',')+1);
#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)xT.c_str());
#endif
  
  x=xT.toInt();                               // Convert Value
  if (x<0) x=0;                               // Range checks
  if (x>1) x=1;

  if (x==0) {
#ifdef XDEBUG
    Serial.println("Switch ScreenSaver off.");
#endif
    ScreenSaverEnabled = false;
    ScreenSaverTimer=0;                       // Reset Screensaver-Timer
    ScreenSaverLogoTimer=0;                   // Reset ScreenSaverLogo-Timer
  }  // endif

  if (x==1) {
    if (ScreenSaverMode>0) {
#ifdef XDEBUG
      Serial.printf("Switch ScreenSaver on, Mode: %i.\n", ScreenSaverMode);
#endif
      ScreenSaverEnabled = true;
      ScreenSaverTimer=0;                     // Reset Screensaver-Timer
      ScreenSaverLogoTimer=0;                 // Reset ScreenSaverLogo-Timer
    }
    else {
#ifdef XDEBUG
      Serial.println("ScreenSaver unset!");
#endif
    }
  }  //endif
}


// --------------------------------------------------------------
// ----------------- Set ScreenSaver Mode -----------------------
// --------------------------------------------------------------
void oled_readnsetscreensaver(void) {
  String TextIn="",mT="",iT="",lT="";
  int d1,d2,m,i,l,b;
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

  m=mT.toInt();                             // Convert Mode
  i=iT.toInt();                             // Convert Interval
  l=lT.toInt();                             // Convert Logo-Time

  if (m<0) m=0;                             // Check & Set Mode
  if (m>255) m=255;                         // Check & Set Mode (8 Bits = 0..255)
  if (i<5) i=5;                             // Check&Set Minimum Interval
  if (i>600) i=600;                         // Check&Set Maximiun Interval
  if (l<20) l=20;                           // Check&Set Minimum Logo-Time
  if (l>600) l=600;                         // Check&Set Maximiun Logo-Time

  // Get&Set Active ScreenSaverScreens
  ScreenSaverCountScreens=0;                                               // Reset Counter   
  for (b=0; b<ScreenSaverMaxScreens; b++) ScreenSaverActiveScreens[b]=0;   // Clear Array
  for (b=0; b<ScreenSaverMaxScreens; b++) {
    if (bitRead(m,b)) {                                                    // Read Bits out of Mode and if Bit is "1"...
      ScreenSaverActiveScreens[ScreenSaverCountScreens]=b+1;               // ...write the value in the Screens-Array[0..x] and..
      ScreenSaverCountScreens++;                                           // ...count up the Counter.
    }
  }
#ifdef ESP32X    
  ShowScreenSaverStarField=bitRead(m,5) && !bitRead(m,6);                  // StarField ScreenSaver active ?
  ShowScreenSaverToaster=!bitRead(m,5) && bitRead(m,6);                    // Toaster ScreenSaver active ?
  ShowScreenSaverAnimated=bitRead(m,5) && bitRead(m,6);                    // All Animated ScreenSaver
#endif

#ifdef XDEBUG
  Serial.printf("Active ScreenSaverScreens: %i\n", ScreenSaverCountScreens);
  for (b=0; b<ScreenSaverMaxScreens; b++) {
    Serial.printf("ScreenSaver Array Value No.%i: %i\n", b, ScreenSaverActiveScreens[b]);
  }
  if (ShowScreenSaverStarField) Serial.printf("ScreenSaver StarField active!\n");
#endif
  
#ifdef XDEBUG
  Serial.printf("Created Strings: M:%s I:%s L:%s\n", (char*)mT.c_str(), (char*)iT.c_str(), (char*)lT.c_str());
  Serial.printf("Values: M:%i T:%i L:%i\n", m, i, l);
#endif

  ScreenSaverMode=m;
  ScreenSaverTimer=0;                       // Reset Screensaver-Timer
  ScreenSaverLogoTimer=0;                   // Reset ScreenSaverLogo-Timer

  if (m==0) {
#ifdef XDEBUG
    Serial.println("ScreenSaver Disabled!");
#endif
    ScreenSaverEnabled=false;
  }
  else{
#ifdef XDEBUG
    Serial.println("ScreenSaver Enabled!");
#endif
    ScreenSaverEnabled=true;
    ScreenSaverInterval=i;                    // Set ScreenSaverTimer Interval
#ifdef USE_NODEMCU
    ScreenSaverLogoTime=l-i;                  // Set ScreenSaverLogoTime (First Screensaver shown after ScreenSaverLogoTime-ScreenSaverInterval+ScreenSaverInterval)
#endif
#ifdef ESP32X
    if (ShowScreenSaverStarField || ShowScreenSaverToaster || ShowScreenSaverAnimated) {
      ScreenSaverLogoTime=l;                  // Set ScreenSaverLogoTime (Screensaver shown after ScreenSaverLogoTime)
    }
    else {
      ScreenSaverLogoTime=l-i;                // Set ScreenSaverLogoTime (First Screensaver shown after ScreenSaverLogoTime-ScreenSaverInterval+ScreenSaverInterval)
    }
#endif
  }
}


// --------------------------------------------------------------
// ------------ Show ScreenSaver Pictures/Time  -----------------
// --------------------------------------------------------------
void oled_showScreenSaverPicture(void) {
  int l,x,y;
  String actTime="";
  oled.setContrast(ScreenSaverContrast);                        // Set Contrast for ScreenSaver Mode

  l=ScreenSaverActiveScreens[random(ScreenSaverCountScreens)];  // Get random Screen out of the Active-Screens-Array[0..x]
#ifdef XDEBUG
  Serial.printf("Screen: %i\n", l);
#endif

  switch (l) {
    case 1:                                             // Show tiny tty2oled Logo
      oled.clearDisplay();
      x=random(DispWidth - tty2oled_logo32_width);
      y=random(DispHeight - tty2oled_logo32_height);
      oled.drawXBitmap(x, y, tty2oled_logo32, tty2oled_logo32_width, tty2oled_logo32_height, SSD1322_WHITE);
      oled.display();
    break;
    case 2:                                             // Show tiny MiSTer Logo
      oled.clearDisplay();
      x=random(DispWidth - mister_logo32_width);
      y=random(DispHeight - mister_logo32_height);
      oled.drawXBitmap(x, y, mister_logo32, mister_logo32_width, mister_logo32_height, SSD1322_WHITE);
      oled.display();
    break;
    case 3:                                             // Show 1/4 Version of the actual Core
      x=random(DispWidth - DispWidth/2);
      y=random(DispHeight - DispHeight/2);
      oled_showSmallCorePicture(x,y);
    break;
#ifdef ESP32X
    case 4:                                             // Show Time for ESP32
      oled.clearDisplay();
      if (timeIsSet) {
        u8g2.setFont(u8g2_font_luBS24_tf);
        actTime=rtc.getTime("%H:%M");
        x=random(DispWidth - u8g2.getUTF8Width(actTime.c_str()));
        y=random(u8g2.getFontAscent(), DispHeight);
        u8g2.setCursor(x,y);
        u8g2.print(actTime);
      }
      else {
        oled_showcenterredtext("Time not set!",9);
      }
      oled.display();
    break;
    case 5:                                            // Show Date for ESP32
      oled.clearDisplay();
      if (timeIsSet) {
        u8g2.setFont(u8g2_font_luBS14_tf);
        actTime=rtc.getTime("%d-%b-%y");
        x=random(DispWidth - u8g2.getUTF8Width(actTime.c_str()));
        y=random(u8g2.getFontAscent(), DispHeight);
        u8g2.setCursor(x,y);
        u8g2.print(actTime);
      }
      else {
        oled_showcenterredtext("Date not set!",9);
      }
      oled.display();
    break;
#endif
    default:
      oled.clearDisplay();
      x=random(DispWidth - tty2oled_logo32_width);
      y=random(DispHeight - tty2oled_logo32_height);
      oled.drawXBitmap(x, y, tty2oled_logo32, tty2oled_logo32_width, tty2oled_logo32_height, SSD1322_WHITE);
      oled.display();
    break;
  } //end switch
}


// --------------------------------------------------------------
// ------------- Show 1/4 Core Picture at Position V2 -----------
// --------------------------------------------------------------
// xpos & ypos = Offset
void oled_showSmallCorePicture(int xpos, int ypos) {
  int x=0,y=0,px=0,py=0,i=0;
  unsigned char b1,b2,br;

#ifdef XDEBUG
  Serial.printf("Show 1/4 Pic, ActPicType: %d\n",actPicType);
#endif
  
  oled.clearDisplay();
  switch (actPicType) {
    case XBM:
      x=0;y=0;
      for (py=0; py<DispHeight; py=py+2) {
        for (px=0; px<DispLineBytes1bpp; px++) {
          b1=logoBin[px+py*DispLineBytes1bpp];                       // Get Data Byte for 8 Pixels
          for (i=0; i<8; i=i+2){
            if (bitRead(b1, i)) {
              oled.drawPixel(xpos+x, ypos+y, SSD1322_WHITE);         // Draw Pixel if "1"
            }
            else {
              oled.drawPixel(xpos+x, ypos+y, SSD1322_BLACK);         // Clear Pixel if "0"
            }
#ifdef XDEBUG
            Serial.printf("X: %d Y: %d\n",x,y);
#endif
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
            b1=logoBin[(px*4)+i+py*DispLineBytes4bpp];                                                  // Get Data Byte 1 for 2 Pixels
            b2=logoBin[(px*4)+i+(py+1)*DispLineBytes4bpp];                                              // Get Data Byte 2 for 2 Pixels
            //br=(((0xF0 & b1) >> 4) + (0x0F & b1) + ((0xF0 & b2) >> 4) + (0x0F & b2)) / 4;               // cutting
            br=round((((0xF0 & b1) >> 4) + (0x0F & b1) + ((0xF0 & b2) >> 4) + (0x0F & b2)) / 4);        // rounding
            oled.drawPixel(xpos+x, ypos+y, br);   // Draw only Pixel 1, Left Nibble
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
      oled_showcorename();
    break;
  }
}


// --------------------------------------------------------------
// ----------- Show System Hardware Info on Screen --------------
// --------------------------------------------------------------
void oled_showSystemHardware(void) {
  int hwinfo=0;

#ifdef XDEBUG
  Serial.println("Called Command CMDSHSYSHW");
#endif

#ifdef USE_ESP32DEV                        // TTGO-T8 & d.ti Board
  hwinfo=1;
#endif

#ifdef USE_LOLIN32                         // Wemos LOLIN32, LOLIN32, DevKit_V4 (Wemos Lolin32)
  hwinfo=2;
#endif

#ifdef USE_NODEMCU                         // ESP8266 NodeMCU
  hwinfo=3;
#endif

#ifdef USE_ESP32S3DEV
  hwinfo=4;
#endif

  oled.clearDisplay();
  u8g2.setFont(u8g2_font_luBS10_tf);
  u8g2.setCursor(0,10);
  u8g2.print("SysInfo");

  u8g2.setFont(u8g2_font_luBS08_tf);
  u8g2.setCursor(0,25);
  u8g2.print("FW Version: " BuildVersion);

  u8g2.setCursor(0,35);
  u8g2.print("Board Type: ");
  
  switch (hwinfo) {
    case 0:
      u8g2.print("Unknown");                 // Unknow Hardware
    break;
    case 1:
      u8g2.print("ESP32-DEV");               // ESP32-DEV, TTGO, DTI-Board
      if (dtiv>=11) {
        u8g2.printf(", d.ti v%.1f", (float)dtiv/10);
      }
    break;
    case 2:
      u8g2.print("WEMOS LOLIN32");           // Wemos,Lolin,DevKit_V4
    break;
    case 3:
      u8g2.print("NodeMCU 1.0");             // ESP8266
    break;
    case 4:
      u8g2.print("ESP32S3-DEV");             // ESP32S3
      if (dtiv>=11) {
        u8g2.printf(", d.ti v%.1f", (float)dtiv/10);
      }
    break;
    default:
      u8g2.print("Other");                   // Everything else
    break;
  }

  u8g2.setCursor(0,50);
  u8g2.print("Board Options: ");
  u8g2.setCursor(0,60);
  if (dtiv>=11) {
    if (hasMIC) u8g2.print("MIC");
    if (hasPCA) u8g2.print(",PCA");
    if (hasLED) u8g2.print(",LED");
    if (hasRGBLED) u8g2.print(",RGB-LED");
    if (hasBUZZER) u8g2.print(",Buzzer");
  }
  else {
    u8g2.print("None");
  }

  oled.display();  
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

#ifdef USE_ESP32S3DEV
  hwinfo=4;
#endif
 
  delay(hwDelay);                          // Small Delay

  switch (hwinfo) {
    case 0:
      Serial.println("HWNONEXXX;" BuildVersion ";");              // No known Hardware in use
    break;
    case 1:
      Serial.println("HWESP32DE;" BuildVersion ";");              // ESP32-DEV, TTGO, DTI-Board
    break;
    case 2:
      Serial.println("HWLOLIN32;" BuildVersion ";");              // Wemos,Lolin,DevKit_V4
    break;
    case 3:
      Serial.println("HWESP8266;" BuildVersion ";");              // ESP8266
    break;
    case 4:
      Serial.println("HWESP32S3;" BuildVersion ";");              // ESP32-S3
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
void oled_showcorename() {
#ifdef XDEBUG
  Serial.println("Called Command CMDSNAM");
#endif

  //ScreenSaverTimer=0;                        // Reset ScreenSaver-Timer
  //ScreenSaverLogoTimer=0;                    // Reset ScreenSaverLogo-Timer
  oled.setContrast(contrast);
  oled_showcenterredtext(actCorename,9);
}


// --------------------------------------------------------------
// ------------------ Switch Display off ------------------------
// --------------------------------------------------------------
void oled_displayoff(void) {
#ifdef XDEBUG
  Serial.println("Called Command CMDDOFF");
#endif
  
  oled.displayOff();                 // Switch Display off
}


// --------------------------------------------------------------
// ------------------- Switch Display on ------------------------
// --------------------------------------------------------------
void oled_displayon(void) {
#ifdef XDEBUG
  Serial.println("Called Command CMDDOFF");
#endif
  
  oled.displayOn();                 // Switch Display on
}


// --------------------------------------------------------------
// -------------- Update Display Content ------------------------
// --------------------------------------------------------------
void oled_updatedisplay(void) {
#ifdef XDEBUG
  Serial.println("Called Command CMDDUPD");
#endif

  oled.display();                 // Update Display Content
}


// --------------------------------------------------------------
// ----------------- Read an Set Contrast -----------------------
// --------------------------------------------------------------
void oled_readnsetcontrast(void) {
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
// ---------------- Show "Parameter Error" ----------------------
// --------------------------------------------------------------
void oled_showperror(void) {
  String actText="Parameter Error";
  
  oled.clearDisplay();
  u8g2.setFont(u8g2_font_luBS14_tf);
  u8g2.setCursor(DispWidth/2-(u8g2.getUTF8Width(actText.c_str())/2), 20);
  //u8g2.setCursor(5, 20);
  u8g2.print(actText);
  
  u8g2.setFont(u8g2_font_luBS10_tf);
  u8g2.setCursor(DispWidth/2-(u8g2.getUTF8Width(newCommand.c_str())/2), 40);
  //u8g2.setCursor(5, 40);
  u8g2.print(newCommand);
  oled.display();
}

// --------------------------------------------------------------
// ------------------ Show centerred Text -----------------------
// --------------------------------------------------------------
void oled_showcenterredtext(String text, int font) {
  
  oled.clearDisplay();
  oled_setfont(font);
  u8g2.setCursor(DispWidth/2-(u8g2.getUTF8Width(text.c_str())/2), DispHeight/2 + (u8g2.getFontAscent()/2));
  u8g2.print(text);
  oled.display();
}


// --------------------------------------------------------------
// -------------------------- Set Font --------------------------
// --------------------------------------------------------------
void oled_setfont(int font) {
  switch (font) {
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
    case 9:
      u8g2.setFont(u8g2_font_commodore64_tr);     // Commodore 64
    break;
    case 10:
      u8g2.setFont(u8g2_font_8bitclassic_tf);     // 8bitclassic
    break;

    default:
      u8g2.setFont(u8g2_font_tenfatguys_tr);      // Nice 10 Pixel Font
    break;
  }
}


// --------------------------------------------------------------
// ----------- Command Read and Set Rotation --------------------
// --------------------------------------------------------------
void oled_readnsetrotation(void) {
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
void oled_clswithtransition() {
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
    oled_drawlogo(t);                // ..and draw Picture to Display with Effect
  }
  else {
    oled_showperror();
  }
}  // end oled_clswithtransition


// --------------------------------------------------------------
// ------------------------- Showpic ----------------------------
// --------------------------------------------------------------
void oled_showpic(void) {
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
int oled_readlogo() {
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
    oled.setCursor(0,0);
    oled.print(bytesReadCount);
    oled.display();
    return 0;
  }
  else {
    return 1;
  }
}  //end oled_readlogo


// --------------------------------------------------------------
// ----------------------- Draw Logo ----------------------------
// --------------------------------------------------------------
void oled_drawlogo(uint8_t e) {
  int w,x,y,x2=0,y2=0;
  //unsigned char logoByteValue;
  //int logoByte;
  uint8_t vArray[DispLineBytes1bpp]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
  
#ifdef XDEBUG
  Serial.printf("Draw Logo with Transition Effect No. %d\n",e);
#endif

  ScreenSaverTimer=0;                        // Reset ScreenSaver-Timer
  ScreenSaverLogoTimer=0;                    // Reset ScreenSaverLogo-Timer
#ifdef ESP32X  
  ShowAnimatedScreenSaverNo=random(MinAnimatedScreenSaver, MaxAnimatedScreenSaver+1);
#endif
  oled.setContrast(contrast);

  switch (e) {
    case 1:                                  // Left to Right
      for (x=0; x<DispLineBytes1bpp; x++) {
        for (y=0; y<DispHeight; y++) {
          oled_drawEightPixelXY(x, y);
        }
        oled.display();
      }
    break;  // 1

    case 2:                                  // Top to Bottom
      for (y=0; y<DispHeight; y++) {
        for (x=0; x<DispLineBytes1bpp; x++) {
          oled_drawEightPixelXY(x, y);
        }
        if (y%2==1) oled.display();
      }
    break;  // 2

    case 3:                                  // Right to left
      for (x=DispLineBytes1bpp-1; x>=0; x--) {
        for (y=0; y<DispHeight; y++) {
          oled_drawEightPixelXY(x, y);
        }
        oled.display();
      }
    break;  // 3

    case 4:                                  // Bottom to Top
      for (y=DispHeight-1; y>=0; y--) {
        for (x=0; x<DispLineBytes1bpp; x++) {
          oled_drawEightPixelXY(x, y);
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
          oled_drawEightPixelXY(x2, y);
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
          oled_drawEightPixelXY(x2, y);
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
            oled_drawEightPixelXY(x2, y+w*16);
          }  // end for y
          oled.display();
        }  // end for x
      }
    break;  // 7

    case 8:                                     // 4 Parts, Top-Left => Bottom-Right => Top-Right => Bottom-Left
      // Part 1 Top Left
      for (x=0; x<DispLineBytes1bpp/2; x++) {
        for (y=0; y<DispHeight/2; y++) {
          oled_drawEightPixelXY(x, y);
        }  // end for y
        oled.display();
      }  // end for x
      // Part 2 Bottom Right
      for (x=DispLineBytes1bpp/2; x<DispLineBytes1bpp; x++) {
        for (y=DispHeight/2; y<DispHeight; y++) {
          oled_drawEightPixelXY(x, y);
        }  // end for y
        oled.display();
      }  // end for x
      // Part 3 Top Right
      for (x=DispLineBytes1bpp-1; x>=DispLineBytes1bpp/2; x--) {
        for (y=0; y<DispHeight/2; y++) {
          oled_drawEightPixelXY(x, y);
        }  // end for y
        oled.display();
      }  // end for x
      // Part 4 Bottom Left
      for (x=DispLineBytes1bpp/2-1; x>=0; x--) {
        for (y=DispHeight/2; y<DispHeight; y++) {
          oled_drawEightPixelXY(x, y);
        }  // end for y
        oled.display();
      }  // end for x
    break; // 8
    
    case 9:                                      // Particle Effect NEW and faster
      for (w=0; w<1000; w++) {
        x = random(DispLineBytes1bpp);
        y = random(DispHeight/8);
        for (int offset=0; offset<8; offset++) {
          oled_drawEightPixelXY(x, y*8+offset);
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
            oled_drawEightPixelXY(x2, y);
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
            oled_drawEightPixelXY(x+x2-(DispLineBytes1bpp-1), y, x2, y);
          }
        }
        oled.display();
      }
    break;  // 11

    case 12:                                     // Slide in Top to Bottom
      for (y=0; y<DispHeight; y++) {
        for (y2=DispHeight-1-y; y2<DispHeight; y2++) {
          for (x=0; x<DispLineBytes1bpp; x++) {
            oled_drawEightPixelXY(x, y+y2-(DispHeight-1), x, y2);
          }
        }
        if (y%2==1) oled.display();
      }
    break;  // 12

    case 13:                                  // Slide in Right to left
      for (x=DispLineBytes1bpp-1; x>=0; x--) {
        for (x2=DispLineBytes1bpp-1-x; x2>=0; x2--) {
          for (y=0; y<DispHeight; y++) {
            oled_drawEightPixelXY(x+x2, y, x2, y);
          }
        }
        oled.display();
      }
    break;  // 13

    case 14:                                  // Slide in Bottom to Top
      for (y=DispHeight-1; y>=0; y--) {
        for (y2=DispHeight-1-y; y2>=0; y2--) {
          for (x=0; x<DispLineBytes1bpp; x++) {
            oled_drawEightPixelXY(x, y+y2, x, y2); 
          }
        }
        if (y%2==0) oled.display();
      }
    break;  // 14

    case 15:                                  // Top and Bottom to Middle
      for (y=0; y<DispHeight/2; y++) {
        for (x=0; x<DispLineBytes1bpp; x++) {
          oled_drawEightPixelXY(x, y);
          oled_drawEightPixelXY(x, DispHeight-y-1);
        }
      oled.display();
      }
    break;  // 15

    case 16:                                  // Left and Right to Middle
      for (x=0; x<DispLineBytes1bpp/2; x++) {
        for (y=0; y<DispHeight; y++) {
          oled_drawEightPixelXY(x, y);
          oled_drawEightPixelXY(DispLineBytes1bpp-x-1, y);
        }
        oled.display();
      }
    break;  // 16

    case 17:                                  // Middle to Top and Bottom
      for (y=0; y<DispHeight/2; y++) {
        for (x=0; x<DispLineBytes1bpp; x++) {
          oled_drawEightPixelXY(x, DispHeight/2-1-y);
          oled_drawEightPixelXY(x, DispHeight/2+y);
        }
      oled.display();
      }
    break;  // 17

    case 18:                                  // Middle to Left and Right
      for (x=0; x<DispLineBytes1bpp/2; x++) {
        for (y=0; y<DispHeight; y++) {
          oled_drawEightPixelXY(DispLineBytes1bpp/2-1-x, y);
          oled_drawEightPixelXY(DispLineBytes1bpp/2+x, y);
        }
        oled.display();
      }
    break;  // 18
    
    case 19:                                  // Warp, Middle to Left, Right, Top and Bottom
      for (w=0; w<DispLineBytes1bpp/2; w++) {
        for (y=DispHeight/2-2-(w*2); y<=DispHeight/2+(w*2)+1; y++) {
          for (x=DispLineBytes1bpp/2-1-w; x<=DispLineBytes1bpp/2+w; x++) {
            oled_drawEightPixelXY(x, y);
          }
        }
        oled.display();
      }
    break;  // 19

    case 20:                                  // Slightly Clockwise
      for (y=0; y<DispHeight/2;y++) {
        for (x=DispLineBytes1bpp-DispHeight/16;x<DispLineBytes1bpp;x++) {
          oled_drawEightPixelXY(x, y);
        }
      //oled.display();  
      if (y%2==1) oled.display();             // Update only each uneven (second) round = faster 
      }
      for (x=DispLineBytes1bpp-1;x>=DispHeight/16;x--) {
        for (y=DispHeight/2; y<DispHeight;y++) {
          oled_drawEightPixelXY(x, y);
        }
      oled.display();  
      }
      for (y=DispHeight-1; y>=DispHeight/2;y--) {
        for (x=0;x<DispHeight/16;x++) {
          oled_drawEightPixelXY(x, y);
        }
      //oled.display();  
      if (y%2==0) oled.display();             // Update only each even (second) round = faster 
      }
      for (x=0;x<DispLineBytes1bpp-DispHeight/16;x++) {
        for (y=0; y<DispHeight/2;y++) {
          oled_drawEightPixelXY(x, y);
        }
      oled.display();  
      }
    break;  // 20

    case 21:                                  // Shaft
      for (y=0; y<DispHeight; y++) {
        for (x=0; x<DispLineBytes1bpp; x++) {
          if ((x>=0 && x<DispLineBytes1bpp/4*1) || (x>=DispLineBytes1bpp/2 && x<DispLineBytes1bpp/4*3)) oled_drawEightPixelXY(x, y);
          if ((x>=DispLineBytes1bpp/4*1 && x<DispLineBytes1bpp/2) || (x>=DispLineBytes1bpp/4*3 && x<DispLineBytes1bpp)) oled_drawEightPixelXY(x, DispHeight-y-1);
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
          oled_drawEightPixelXY(vArray[x*2], y);
          oled_drawEightPixelXY(vArray[x*2+1], y);
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
            oled_drawEightPixelXY(x, y);
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
void oled_drawEightPixelXY(int x, int y, int dx, int dy) {
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
void oled_readnwritetext(void) {
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

#ifdef USE_ESP32XDEV             // Only for d.ti Boards
  if (TextOut=="TEP184") {      // If Text is "TEP184" replace Text with Temperature Value
    if (hasMIC) {
      TextOut=String(tSensor.getTemp())+"\xb0"+"C";
    }
    else {
      TextOut="NA";
    }
  }
#endif
  
  oled_setfont(f);
  
  if (!pError) {
    // Write or Clear Text
    u8g2.setForegroundColor(c);                           // Set Font Color
    u8g2.setBackgroundColor(b);                           // Set Background Color
    u8g2.setCursor(x,y);                                  // Set Cursor Position
    u8g2.print(TextOut);                                  // Write Text to Buffer
    if (!bufferMode) oled.display();                      // Update Screen only if not "Clear Mode" (Font>100)
    u8g2.setForegroundColor(SSD1322_WHITE);               // Set Color back
    u8g2.setBackgroundColor(SSD1322_BLACK);
  }
  else { 
    oled_showperror();
  }
}


// --------------------------------------------------------------
// ------------------ Read and Draw Geometrics ------------------
// --------------------------------------------------------------
void oled_readndrawgeo(void) {
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
    oled_showperror();
  }
}


// ------------------ D.TI Board Funtions -----------------------
#ifdef USE_ESP32XDEV

// --------------------------------------------------------------
// ---------------- Just show the Temperature -------------------
// --------------------------------------------------------------
void oled_showtemperature() {
  String myTemp="";
#ifdef XDEBUG
  Serial.println("Called Command CMDSHTEMP");
#endif
  if (hasMIC) {
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
void oled_readnsetuserled(void) {
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
  
  if (hasLED) {                                  // d.ti Board Rev 1.1 = LED
    if (x>1) x=1;
    digitalWrite(USER_LED,x);  
  }
  else if (hasRGBLED){                                // d.ti Board Rev 1.2 = WS2812B LED
    if (x>255) x=255;
    if (x==0) {
      wsleds[0] = CRGB::Black;                   // off
    }
    else {
      wsleds[0] = CHSV(x,255,255);               // color
    }
    FastLED.show();
  }
  else {
    oled_showcenterredtext("No U-LED!",3);
  }
}


// --------------------------------------------------------------
// ------------- Set MIC184 Temperature Zone --------------------
// --------------------------------------------------------------
void oled_settempzone(void) {
  String xZ="";
#ifdef XDEBUG
  Serial.println("Called Command CMDTZONE");
#endif
  
  //xZ=newCommand.substring(9);
  xZ=newCommand.substring(newCommand.indexOf(',')+1);

#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)xZ.c_str());
#endif
  if (hasMIC) {
    if (xZ.toInt()==0) tSensor.setZONE(MIC184_ZONE_INTERNAL);
    if (xZ.toInt()==1) tSensor.setZONE(MIC184_ZONE_REMOTE);
    //delay(1000);
  }
}

// --------------------------------------------------------------
// ---------------------- Set Power LED -------------------------
// --------------------------------------------------------------
void oled_readnsetpowerled(void) {
  String xT="";
  int x;
#ifdef XDEBUG
  Serial.println("Called Command CMDPLED");
#endif
  //xT=newCommand.substring(8);
  xT=newCommand.substring(newCommand.indexOf(',')+1);
#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)xT.c_str());
#endif
  
  x=xT.toInt();                                 // Convert Value
  
  if (hasPLED) {                                // PCA not avail = Board Rev 1.1 = LED
    if (x<0) x=0;
    if (x>1) x=1;
    digitalWrite(POWER_LED,!x);                 // Need to negate Signal, Pin = 1 LED's off
  }
  else {
    oled_showcenterredtext("No P-LED!",3);
  }
}

// --------------------------------------------------------------
// --------- Play Note/Tone using Piezo Beeper ------------------
// --------------------------------------------------------------
void oled_playnote(void) {
  int d0=0,d1=0,d2=0,d3=0,d4=0,o=0,d=0,p=0;
  String TextIn="",nT="",oT="",dT="",pT="";
  note_t n=NOTE_C;
  bool pError=false;
  
#ifdef XDEBUG
  Serial.println("Called Command CMDPNOTE");
#endif  
  
  TextIn=newCommand.substring(newCommand.indexOf(','));  // Find the first "," after the command
  d0 = TextIn.indexOf(',');                 // Find location of the Starting ","

#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)TextIn.c_str());
  Serial.printf("Find first delimeter at: %d\n", d0);
#endif  

  if (hasBUZZER){                                 // only on d.ti Board >= Rev 1.2
    ledcAttachPin(BUZZER, TONE_PWM_CHANNEL);

    do {
      // find ","
      d1 = TextIn.indexOf(',', d0+1 );          // Find location of first ","
      d2 = TextIn.indexOf(',', d1+1 );          // Find location of second ","
      d3 = TextIn.indexOf(',', d2+1 );          // Find location of third ","
      d4 = TextIn.indexOf(',', d3+1 );          // Find location of fourth "," - Value = "-1" if not found = no more Notes available

#ifdef XDEBUG
      Serial.printf("Find delimeters at: %d %d %d %d\n", d1,d2,d3,d4);
#endif  

      //Create Substrings
      nT = TextIn.substring(d0+1, d1);          // Get String for Note
      oT = TextIn.substring(d1+1, d2);          // Get String for Octave
      dT = TextIn.substring(d2+1, d3);          // Get String for Duration
      if (d4 == -1) {                           // String finished
        pT = TextIn.substring(d3+1);            // Get String for Pause
      }
      else if (d3 == -1 || d2 == -1 || d1 == -1) {  // Parameter missing = pError
        pError=true;
      }
      else {                                    // String not finished
        pT = TextIn.substring(d3+1, d4);        // Get String for Pause
        d0=d4;                                  // Set Index for next Note
      }

#ifdef XDEBUG
      Serial.printf("Found strings: %s %s %s %s\n", (char*)nT.c_str(), (char*)oT.c_str(), (char*)dT.c_str(), (char*)pT.c_str());
#endif  

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

      if (o == 0 || d == 0 || p == 0) pError=true;
    
#ifdef XDEBUG
      Serial.printf("Values to Play: %s %d %d %d\n", (char*)nT.c_str(), o, d, p);
#endif  

      if (!pError) {
        ledcWriteNote(TONE_PWM_CHANNEL, n, o);    // Play Note
        delay(d);                                 // Duration
        ledcWriteTone(TONE_PWM_CHANNEL, 0);       // Buzzer off
        delay(p);                                 // Pause
      }
      else {
        oled_showperror(); 
#ifdef XDEBUG
        Serial.printf("Parameter Error!\n");
#endif  
      }
    } while (d4 != -1 && !pError);                         // Repeat as long a fourth "," is found

    ledcDetachPin(BUZZER);

  }  // endif dtiv>=12
  else {
    oled_showcenterredtext("No Buzzer!",3);
#ifdef XDEBUG
    Serial.printf("No Buzzer!\n");
#endif  
  }
}

// --------------------------------------------------------------
// -------- Play Tone/Frequency using Piezo Beeper --------------
// --------------------------------------------------------------
void oled_playtone(void) {
  int d0=0,d1=0,d2=0,d3=0,f=0,d=0,p=0;
  String TextIn="",fT="",dT="",pT="";
  bool pError=false;
  
#ifdef XDEBUG
  Serial.println("Called Command CMDPTONE");
#endif  

  TextIn=newCommand.substring(newCommand.indexOf(','));  // Find the first "," after the command
  d0 = TextIn.indexOf(',');                 // Find location of the Starting ","

#ifdef XDEBUG
  Serial.printf("\nReceived Text: %s\n", (char*)TextIn.c_str());
  Serial.printf("Find first delimeter at: %d\n", d0);
#endif  

  if (hasBUZZER){                                 // only on d.ti Board >= Rev 1.2
    ledcAttachPin(BUZZER, TONE_PWM_CHANNEL);

    do {
      // find ","
      d1 = TextIn.indexOf(',', d0+1 );          // Find location of first ","
      d2 = TextIn.indexOf(',', d1+1 );          // Find location of second ","
      d3 = TextIn.indexOf(',', d2+1 );          // Find location of third "," - Value = "-1" if not found = no more Notes available

      //Create Substrings
      fT = TextIn.substring(d0+1, d1);          // Get String for Frequency
      dT = TextIn.substring(d1+1, d2);          // Get String for Duration
      if (d3 == -1) {                           // String finished
        pT = TextIn.substring(d2+1);            // Get String for Pause
      }
      else if (d2 == -1 || d1 == -1) {          // Parameter missing = pError
        pError=true;
      }
      else {                                    // String not finished
        pT = TextIn.substring(d2+1, d3);        // Get String for Pause
        d0=d3;                                  // Set Index for next Tone
      }

      f = fT.toInt();                           // Octave
      d = dT.toInt();                           // Duration
      p = pT.toInt();                           // Pause after playing tone

      if (f == 0 || d == 0 || p == 0) pError=true;
    
#ifdef XDEBUG
      Serial.printf("Values to Play: %d %d %d\n", f, d, p);
#endif  
      if (!pError) {
        ledcWriteTone(TONE_PWM_CHANNEL, f);       // Play Frequency
        delay(d);                                 // Duration
        ledcWriteTone(TONE_PWM_CHANNEL, 0);       // Buzzer off
        delay(p);                                 // Pause
      }
      else {
        oled_showperror();      
#ifdef XDEBUG
        Serial.printf("Parameter Error!\n");
#endif  
      }
    } while (d3 != -1 && !pError);                         // Repeat as long as a third "," is found

    ledcDetachPin(BUZZER);

  }  // endif dtiv>=12
  else {
    oled_showcenterredtext("No Buzzer!",3);
#ifdef XDEBUG
    Serial.printf("No Buzzer!\n");
#endif  
  }
}

// --------------------------------------------------------------
// -------------------- Write DTIV Value ------------------------
// --------------------------------------------------------------
void oled_readnsetedtiv(void) {
  String vT="";
  int v;

#ifdef XDEBUG
  Serial.println("Called Command CMDWRDTIV");
#endif

  vT=newCommand.substring(newCommand.indexOf(',')+1);

#ifdef XDEBUG
  Serial.printf("Received Text: %s\n", (char*)TextIn.c_str());
#endif
 
  v=vT.toInt();                             // Convert Interval

  if (v<0) v=0;                             // Check Value
  if (v>255) v=255;                         // Check Value
  
  oled.clearDisplay();
  u8g2.setFont(u8g2_font_luBS08_tf);
  prefs.begin("tty2oled", false);             // Preferences Handling, open Namespace
  prefs.putUChar("dtiv", v);                  // Write Value to Preferences Namespace
  prefs.end();                                // Close the Preferences 
#ifdef XDEBUG
  Serial.printf("Write Value %i to Preferences DTIV.\n",v);
#endif
  u8g2.setCursor(0,25);
  u8g2.printf("Write Value %i to Preferences DTIV.\n",v);
  oled.display();
}

#endif  // ----------- d.ti functions endif ESP32DEV---------------


// -------------- ESP32 Functions -------------------- 
#ifdef ESP32X  // OTA, Reset and Time are only for ESP32

// --------------------------------------------------------------
// ----------- Draw the ScreenSaver Flying Toaster --------------
// --------------------------------------------------------------
void oled_drawScreenSaverToaster() {
  uint8_t i, f;
  int16_t x, y;
  bool resort = false;     // By default, don't re-sort depths

  oled.clearDisplay();
  for(i=0; i<TOAST_FLYERS; i++) { // For each flyer...
    // First draw each item...
    f = (flyer[i].frame == 255) ? 4 : (flyer[i].frame++ & 3); // Frame #
    x = flyer[i].x / TOAST_MPIX;
    y = flyer[i].y / TOAST_MPIX;
    oled.drawBitmap(x, y, tmask[f], 32, 32, SSD1322_BLACK);
    oled.drawBitmap(x, y, timg[f], 32, 32, SSD1322_WHITE);

    // Update position..
    flyer[i].x -= flyer[i].depth * 2; // Update position based on depth,
    flyer[i].y += flyer[i].depth;     // for a sort of pseudo-parallax effect.

    // ..checking if item moved off screen the re-set it.
    if((flyer[i].y >= (64*TOAST_MPIX)) || (flyer[i].x <= (-32*TOAST_MPIX))) {
      //flyer[i].x = (-32 + random(255+32)) * TOAST_MPIX;
      flyer[i].x = (-32 + random(256+32+32)) * TOAST_MPIX;
      if (flyer[i].x >= 256 * TOAST_MPIX) {
        flyer[i].y = (-32 + random(63+32)) * TOAST_MPIX;
        flyer[i].x = 256 * TOAST_MPIX;      
      }
      else {
        flyer[i].y = (-32) * TOAST_MPIX;
      }
      flyer[i].frame = random(3) ? random(4) : 255; // 66% toaster, else toast
      flyer[i].depth = 10 + random(TOAST_MPIX);
      resort = true;
    }
  }
  oled.display();
  delay(TOAST_DELAY);
}


// --------------------------------------------------------------
// ------ Calculate Random Values, used by the StarField --------
// --------------------------------------------------------------
int getRandom(int lower, int upper) {
    return lower + static_cast<int>(rand() % (upper - lower + 1));      // return a random number between lower and upper bound
}


// --------------------------------------------------------------
// -------------- Draw the ScreenSaver StarField ----------------
// --------------------------------------------------------------
void oled_drawScreenSaverStarField() {
  int origin_x = oled.width() / 2;
  int origin_y = oled.height() / 2;
  int x,y,s,c;
  double k;
  
   oled.clearDisplay();
  // Iterate through the stars reducing the z co-ordinate in order to move the Star closer.
  for (int i = 0; i < starCount; ++i) {
    stars[i][2] -= 0.19;
    // if the star has moved past the screen (z < 0) reposition it far away with random x and y positions.
    if (stars[i][2] <= 0) {
      stars[i][0] = getRandom(-25, 25);
      stars[i][1] = getRandom(-25, 25);
      stars[i][2] = maxDepth;
    }

    // Convert the 3D coordinates to 2D using perspective projection.
    k = oled.width() / stars[i][2];
    x = static_cast<int>(stars[i][0] * k + origin_x);
    y = static_cast<int>(stars[i][1] * k + origin_y);

    // Draw the star (if it is visible in the screen). Distant stars are smaller and brighter than closer stars.
    if ((0 <= x and x < oled.width()) and (0 <= y and y < oled.height())) {
      s = (1 - stars[i][2] / maxDepth) * 4;     // Star Size
      //c = (1 - stars[i][2] / maxDepth) * 20;  // Color
	  c = stars[i][2] / -2 + 16;                // Star Color, z=32..0 => c=0..16 
      if (c<0) c=0;
      if (c>15) c=15;
      oled.fillRect(x, y, s, s, c);
      //oled.fillRect(x, y, s, s, 15);
    }
  }
  oled.display();
}

// --------------------------------------------------
// ----------------- Show Time ---------------------- 
// --------------------------------------------------
void oled_showtime(void) {
  String actTime=rtc.getTime("%H:%M");                    // Get Hrs and Secs

#ifdef XDEBUG
  Serial.println("Called Command CMDSHTIME");
#endif  

  oled.clearDisplay();
  u8g2.setForegroundColor(15);                            // Set Font Color
  u8g2.setBackgroundColor(0);                             // Set Background Color
  u8g2.setFont(u8g2_font_5x7_mf);                         // Set Font
  u8g2.setCursor(0,8);                                    // Set Cursor Position
  u8g2.print("Show Time");                                     // Write Text
  u8g2.setFont(u8g2_font_7Segments_26x42_mn);             // Set Font
  u8g2.setCursor(55,58);                                  // Set Cursor Position
  u8g2.print(actTime);                                    // Write Text
  oled.display();                                         // Output Text
}

#endif // ESP32

//========================== The end ================================
