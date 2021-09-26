/*
  By Venice
  Get CORENAME from MiSTer via Serial TTY Device and show CORENAME related text, Pictures or Logos
  Using Forked Adafruit SSD1327 Library https://github.com/adafruit/Adafruit_SSD1327for the SSD1322

  -- G R A Y S C A L E  E D I T I O N --
  
  2021-09-19
  -First Grayscale Test based on MiSTer_SSD1322_USB_Testing
  2021-09-25/26
  -Adding Effects 1-10

  ToDo
  -Text & Geo Commands
  -Everything I forgot
   
*/

#include <Arduino.h>
#include <SSD1322_for_Adafruit_GFX.h>             // Display Library
#include "logo.h"                                 // The Pics in XMB Format
#include <Fonts/Picopixel.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
//#include <Fonts/FreeSans24pt7b.h>

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
// ------------------------------------------- Activate your Options----------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

// Uncomment (remove "//" in front ofd the Option) to get some Debugging Infos over Serial especially for SD Debugging
//#define XDEBUG

// Version
#define BuildVersion "210926GT"    // "T" for Testing, "G" for Grayscale

// Uncomment to get the tty2oled Logo shown on Startscreen instead of text
#define XLOGO

// Uncomment for 180° StartUp Rotation (Display Connector up)
//#define XROTATE

// Uncomment for "Send Acknowledge" from tty2oled to MiSTer, need Daemon with "waitfortty"
#define XSENDACK

// Uncomment for Tilt-Sensor based Display-Auto-Rotation. 
// The Sensor is connected to Pin 32 (with Pullup) and GND.
//#define XTILT
#ifdef XTILT
  #include <Bounce2.h>             // << Extra Library
#endif

// Uncomment for Temperatur Sensor Support MIC184 on d.ti's PCB
//#define XMIC184
#ifdef XMIC184
  #include <eHaJo_LM75.h>          // << Extra Library
  #define xmic_SDA 17
  #define xmic_SCL 16
  EHAJO_LM75 tSensor;
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

// -------------------------------------------------------------
// ------------------------- Variables -------------------------
// -------------------------------------------------------------

// Strings
String newCommand = "";             // Received Text, from MiSTer without "\n" currently (2021-01-11)
String prevCommand = "";
String actCorename = "No Core";     // Actual Received Corename
uint8_t contrast = 5;               // Contrast (brightness) of display, range: 0 (no contrast) to 255 (maximum)
//char *newCommandChar;
bool updateDisplay = false;

// Display Vars
uint16_t DispWidth, DispHeight, DispLineBytes1bpp, DispLineBytes4bpp;
unsigned int logoBytes1bpp=0;
unsigned int logoBytes4bpp=0;
const int cDelay=25;                // Command Delay in ms for Handshake
size_t bytesReadCount=0;
uint8_t *logoBin;                   // <<== For malloc in Setup
enum picType {NONE, XBM, GSC};
int actPicType=0;
int16_t xs, ys;
uint16_t ws, hs;
const uint8_t minEffect=1, maxEffect=10;      // Min/Max Effects for Random
//uint8_t logoBin[8192];
//unsigned char *logoBin;           // <<== For malloc in Setup

#ifdef XTILT
// Input/Output
#define RotationPin 32
#define DebounceTime 25
Bounce RotationDebouncer = Bounce();     // Instantiate a Bounce object
#endif

// =============================================================================================================
// ================================================ SETUP ======================================================
// =============================================================================================================
void setup(void) {
  // Init Serial
  Serial.begin(115200);                      // 115200 for MiSTer ttyUSBx Device CP2102 Chip on ESP32
  Serial.flush();                            // Wait for empty Send Buffer
  Serial.setTimeout(500);                    // Set max. Serial "Waiting Time", default = 1000ms

  randomSeed(analogRead(34));                // Init Random Generator with empty Port Analog value

#ifdef XMIC184
  Wire.begin(xmic_SDA, xmic_SCL, 100000);
#ifdef XDEBUG
  Serial.print("Temperature = ");
  Serial.print(tSensor.getTemp());
  Serial.print("°C");
#endif
#endif

  // Init Display SSD1322
  oled.begin();
  oled.clearDisplay();
  oled.setRotation(0);
  oled.setContrast(contrast);                       // Set contrast of display
  oled.setTextSize(1);
  oled.setTextColor(SSD1322_WHITE, SSD1322_BLACK);  // White foreground, black background
  oled.setFont(&FreeSans9pt7b);                     // Set Standard Font (available in 9/12/18/24 Pixel)

  // Get Display Dimensions
  DispWidth = oled.width();
  DispHeight = oled.height();
  DispLineBytes1bpp = DispWidth / 8;                       // How many Bytes uses each Display Line at 1bpp
  DispLineBytes4bpp = DispWidth / 2;                       // How many Bytes uses each Display Line at 4bpp
  logoBytes1bpp = DispWidth * DispHeight / 8;              // 2048
  logoBytes4bpp = DispWidth * DispHeight / 2;              // 8192
  logoBin = (uint8_t *) malloc(logoBytes4bpp);             // Create Picture Buffer, better than create (malloc) & destroy (free)

#ifdef XROTATE
  oled.setRotation(2);
#endif

#ifdef XTILT
  // Setup Tilt-Sensor Input Pin
  RotationDebouncer.attach(RotationPin,INPUT_PULLUP);     // Attach the debouncer to a pin with INPUT mode
  RotationDebouncer.interval(DebounceTime);               // Use a debounce interval of 25 milliseconds
  // Set Startup Rotation
  if (digitalRead(RotationPin)) {
    oled.setRotation(0);
  }
  else {
    oled.setRotation(2);
  }
#endif

  oled_mistertext();                                       // OLED Startup with Some Text
}

// =============================================================================================================
// =============================================== MAIN LOOP ===================================================
// =============================================================================================================
void loop(void) {

#ifdef ESP32  // OTA and Reset only for ESP32
  if (OTAEN) ArduinoOTA.handle();                            // OTA active?
#endif

#ifdef XTILT
  RotationDebouncer.update();                                     // Update the Bounce instance
  if (RotationDebouncer.rose()) {
    oled.setRotation(0);
    if (actCorename == "No Core") {
      oled_mistertext();
    }
    else {
      usb2oled_drawlogo(0);
    }
  }
  if (RotationDebouncer.fell()) {
    oled.setRotation(2);
    if (actCorename == "No Core") {
      oled_mistertext();
    }
    else {
      usb2oled_drawlogo(0);
    }
  }
#endif

  // Serial Data
  if (Serial.available()) {
	prevCommand = newCommand;                                // Save old Command
    newCommand = Serial.readStringUntil('\n');             // Read string from serial until NewLine "\n" (from MiSTer's echo command) is detected or timeout (1000ms) happens.
    updateDisplay=true;                                    // Set Update-Display Flag

#ifdef XDEBUG
    Serial.printf("Received Corename or Command: %s\n", (char*)newCommand.c_str());
#endif
  }  // end serial available
    
  if (updateDisplay) {                                       // Proceed only if it's allowed because of new data from serial

    // -- First Transmission --
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

    else if (newCommand=="CMDSNAM") {                                       // Show actual loaded Corename
      usb2oled_showcorename();
    }

    else if (newCommand=="CMDSPIC") {                                       // Show actual loaded Picture with Transition
      usb2oled_drawlogo(random(minEffect,maxEffect+1));
    }

    else if (newCommand.startsWith("CMDAPD,")) {                            // Command from Serial to receive Picture Data via USB Serial from the MiSTer
      usb2oled_readlogo();                                                  // ESP32 Receive Picture Data... 
    }

    else if (newCommand.startsWith("CMDCOR,")) {                            // Command from Serial to receive Picture Data via USB Serial from the MiSTer
      if (usb2oled_readlogo()==1) {                                         // ESP32 Receive Picture Data... 
        usb2oled_drawlogo(random(minEffect,maxEffect+1));                          // ..and show them on the OLED with Transition Effect 1..10
      }
    }
    
    else if (newCommand.startsWith("CMDCON,")) {                            // Command from Serial to receive Contrast-Level Data from the MiSTer
      usb2oled_readnsetcontrast();                                       // Read and Set contrast                                   
    }

    // The following Commands are only for ESP32
#ifdef ESP32  // OTA and Reset only for ESP32
    else if (newCommand=="CMDENOTA") {                                      // Command from Serial to enable OTA on the ESP
      enableOTA();                                                       // Setup Wireless and enable OTA
    }

    else if (newCommand=="CMDRESET") {                                      // Command from Serial for Resetting the ESP
      ESP.restart();                                                     // Reset ESP
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
  oled.setFont(&Picopixel);
  oled.setCursor(1,62);
  //oled.setCursor(0,0);
  oled.print(BuildVersion);
  oled.drawXBitmap(DispWidth-usb_icon_width, DispHeight-usb_icon_height, usb_icon, usb_icon_width, usb_icon_height, SSD1322_WHITE);
  oled.display();
  oled.setFont(&FreeSans9pt7b);
} // end mistertext


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
  oled.setFont(&FreeSans18pt7b);
  oled.clearDisplay();
  oled.getTextBounds(actCorename,0,30,&xs,&ys,&ws,&hs);
  oled.setCursor(DispWidth/2-ws/2, DispHeight/2-hs/2);
  oled.print(actCorename);
  oled.display();
  oled.setFont(&FreeSans9pt7b);
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
      for (w=0; w<20000; w++) {
        x = random(DispWidth);
        y = random(DispHeight);
        drawEightPixel(x, y);
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
        if ((w>4000) && (w<=8000)) { 
          if ((w % 200)==0) oled.display();
        }
        if (w>8000) { 
          if ((w % 400)==0) oled.display();
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
/*      
      b0=logoBin[(x*4)+0+y*DispLineBytes4bpp];     // Get Byte 0, Pixel 1+2
      b1=logoBin[(x*4)+1+y*DispLineBytes4bpp];     // Get Byte 1, Pixel 3+4 
      b2=logoBin[(x*4)+2+y*DispLineBytes4bpp];     // Get Byte 2, Pixel 5+6
      b3=logoBin[(x*4)+3+y*DispLineBytes4bpp];     // Get Byte 3, Pixel 7+8
      oled.drawPixel(x*8+0, y, (0xF0 & b0) >> 4);  // Pixel 1, Byte 0 Left Nibble
      oled.drawPixel(x*8+1, y,  0x0F & b0);        // Draw Pixel 2, Byte 0 Right Nibble
      oled.drawPixel(x*8+2, y, (0xF0 & b1) >> 4);  // Draw Pixel 3, Byte 1 Left Nibble
      oled.drawPixel(x*8+3, y,  0x0F & b1);        // Draw Pixel 4, Byte 1 Right Nibble
      oled.drawPixel(x*8+4, y, (0xF0 & b2) >> 4);  // Draw Pixel 5, Byte 2 Left Nibble
      oled.drawPixel(x*8+5, y,  0x0F & b2);        // Draw Pixel 6, Byte 2 Right Nibble
      oled.drawPixel(x*8+6, y, (0xF0 & b3) >> 4);  // Draw Pixel 7, Byte 3 Left Nibble
      oled.drawPixel(x*8+7, y,  0x0F & b3);        // Draw Pixel 8, Byte 4 Right Nibble
*/
    break;
  }
#ifdef USE_NODEMCU
  yield();
#endif
}

// ---------------- Command Enable OTA ---------------------- 

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