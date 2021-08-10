/*
  By Venice
  Get CORENAME from MiSTer via Serial TTY Device and show CORENAME related text, Pictures or Logos
  Using the universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Problem: 
  Sometimes after Power-On "MENU" is written instead of the logo shown.
  So i added an "first transmission" without doing anything.

  2021-02
  -Change serial Speed to 57600
  -Adding SD Support for ESP32 using an dedicated SPI Interface
   Use now an TTGO-T8  ESP32 with integrated SD Micro Slot https://github.com/LilyGO/TTGO-T8-ESP32 .output
   All Logos or Picture have to be 256x64 Pixel (Display Size) in XBM Format.
   Place the Pictures to the root of the FAT or FAT32 formatted SD Card an give them the name like the core is named with extension (Lunar Lander = llander.xbm).
   Use Code from "XBM Read from FS" from https://github.com/lbernstone/eyes
  -Add "#define XDEBUG" for Debugging, can be Uncommented for more Infos over Serial

  2021-02-11
  -Trying to make it more universal in case of used display sizes. Using more "DispHeight" and "DispWidth" instead of fixed values.

  2021-02-12
  -Adding some Menu Effects

  2021-02-20/21
  -Using the Effects for all Picture/Logo Transistions (not only for the Menu)
  -"MENU" Picture moved from Code to the SD, makes it easier to change (if you like).
  -Modified Text Position calculation (see U8G2 Documentation for Details). 
   Change text reference position from TOP to BASELINE.
   Using now "getAscent" instead of "getMaxCharHeight" for Text Y-Position calculation.
  -Show a small "SD-Icon" on Startup Screen instead of the "dot" if SD Card was detected.

  2021-02-16 (not released)
  -Make it more Universal 
   Add DispLineBytes , using more DispHeight & DispWidth instead of fixed Values

  2021-03-06
  -Adding some Menu Effects
  -Code cleanup

  2021-03-07
  -Adding some Menu Effects
  -Code cleanup

  2021-03-14
  -Using/Testing SD_MMC Mode (Should not matter in case of SD Access speed because of too small files. 
   Board 1: Lolin32 Board with Adafruit Micro SD SPI/SDIO Adapter. SD Access possible using SPI or 1/4.Bit SD Bus Mode.
   Board 2: TTGO_T8 v1.7.1 with integrated SD Slot. SD Access possible using SPI or 1.Bit SD Bus Mode.
   
   SD Adapter Excample Connections:
   SD Card | ESP32 4-Bit | ESP32 1-Bit (TTGO-T8)
   ----------------------------------------------
     D0       2              2
     D1       4              
     D2       12             
     D3       13             13
     CLK      14             14
     CMD      15             15
     VSS      GND            GND
     VDD      3.3V           3.3V

  -Using #ifdef...#endif for my different HW-Configs (LOLIN32/TTGOT8)
   Choose ONLY one USE_xxxx Board definition at the beggining of the Sketch
  -Adding more Debug Infos

  2021-03-29
  -USB Version Based on SD Version
   Adding USB functionality from "ojaksch" (Many Thanks) and removed SD Stuff.
   Receive Picture-Data via Serial connection instead of the Corename from the MiSTer.
   Speed up Serial Interface to 115200
   Show a small "USB-Icon" on Startup Screen

  2021-04-??
  -Add "Set Contrast" to the Code by "ojaksch"

  2021-05-05
  -Add an Option to receive the Contrast Level from the MiSTer
   At First MiSTer needs to send "att"
   As Second MiSTer needs to send "CONTRAST"
   As Third MiSTer needs to send "[Contrast Value 0..255]"

  2021-05-22
  -Device CleanUp 

  2021-05-24
  -Trying to detect the Board chosen in the Arduino IDE (ESP32 Dev Module, WEMOS LOLIN32 or NODEMCU 8266) and set the Display config.
   No need to set the Board manually in the Sketch.
   But without the correct Board you get Compiler Errors.

  2021-06-14
  -Adding "TEXTOUTXY" Command
   With this additional function you can send text to the Display without using the tty2oled scripts.
   ! The Serial Interface needs to be correctly configured !
   The Command-Parameter Format is "xxx,yy,f,[Text]" (! Changed see 2021-06-18)
   xxx = 3 Digits X-Position 000..255
   yy  = 2 Digits Y-Position 00..63 
   f   = Font Type
   Tip: Use the command "cls" to clear the screen => echo "cls" > /dev/ttyUSB01
   Example/Command Order: 
   1: echo "att" > /dev/ttyUSB0
   2: echo "TEXTOUTXY" > /dev/ttyUSB0
   3: echo "010,10,1,Text Out" > /dev/ttyUSB0

  2021-06-17 
  -Adding Fonts for "TEXTOUTXY"
   https://github.com/olikraus/u8g2/wiki/fntlistall
   f:  Font Type            Width x Height, Size for Character A
   0:  u8g2_font_luBS08_tf (20x12, 8 Pixel A, Transparent)
   1:  u8g2_font_luBS10_tf (26x15, 10 Pixel A, Transparent)
   2:  u8g2_font_luBS14_tf (35x22, 14 Pixel A, Transparent)
   3:  u8g2_font_luBS18_tf (44x28, 18 Pixel A, Transparent)
   4:  u8g2_font_luBS24_tf (61x40, 24 Pixel A, Transparent)
   5:  u8g2_font_profont12_mf (6x12, 8 Pixel A, Non-Transparent)
   6:  u8g2_font_profont17_mf (9x17, 11 Pixel A, Non-Transparent)
   7:  u8g2_font_profont22_mf (12x22, 14 Pixel A, Non-Transparent)
   8:  u8g2_font_profont29_mf (16x29, 19 Pixel A, Non-Transparent)
   9:  u8g2_font_open_iconic_all_2x_t (16x16 Icons, Transparent)
   10: u8g2_font_lucasarts_scumm_subtitle_o_tf (Nice 12 Pixel Font, Transparent)

  2021-06-18
  -Change MALLOC for Picture Data Memory Allocation into Setup for more stabilty. <= TESTING
  -Disabled "Snake" Transition, need too much time.
  -Changed "TEXTOUTXY" Command-Parameter Format to "x,y,f,[Text]"
   x = X-Position 0..255
   y = Y-Position 0..63 
   f = Font Type 0.. (see Font List above)
   Better calculation using "indexof".
   Found here: https://forum.arduino.cc/t/how-to-parse-arduino-string-with-different-delimiters/324653/3

  2021-06-18
  -BasicOTA only for ESP32 <= TESTING
  
  2021-06-19
  -Adding "GEOOUTXY" Command
   Drawing/Clearing Geometric Figures (Pixel, Line, Frame, Box (Filled Frame), Circle, Disc (Filled Circle), Ellipse, Filled Ellipse, Rounded Frame, Rounded Box)
   Example/Command Order: 
   1: echo "att" > /dev/ttyUSB0
   2: echo "GEOOUTXY" > /dev/ttyUSB0
   Draw Example
   3: echo "3,0,20,10,30,20,0" > /dev/ttyUSB0 (Draw Frame starting at x=20, y=10 with width 30 and height=20)
   Clear Example
   3: echo "4,1,20,10,30,20,0" > /dev/ttyUSB0 (Clear Box starting at x=20, y=10 with width 30 and height=20)

   The Command-Parameter Format is "g,c,x,y,i,j,k"
   g = Geometric Type 1..10
   c = Draw Color 0=Clear, 1=Draw, 2=XOR/Invert Pixel
   x = X-Position 0..255
   y = Y-Position 0..63
   i = Parameter 1 (Depends on Geometric)
   j = Parameter 2 (Depends on Geometric)
   k = Parameter 3 (Depends on Geometric)
   
   Geometric Type            Parameter needed                     Link
   1:  Pixel                 need x,y                             https://github.com/olikraus/u8g2/wiki/u8g2reference#drawpixel
   2:  Line                  need x,y,i=x1, j=y1                  https://github.com/olikraus/u8g2/wiki/u8g2reference#drawline
   3:  Frame                 need x,y,i=width,j=height            https://github.com/olikraus/u8g2/wiki/u8g2reference#drawframe
   4:  Box (Filled Frame)    need x,y,i=width,j=height            https://github.com/olikraus/u8g2/wiki/u8g2reference#drawbox
   5:  Circle                need x,y,i=radius                    https://github.com/olikraus/u8g2/wiki/u8g2reference#drawcircle
   6:  Disc (Filled Circle)  need x,y,i=radius                    https://github.com/olikraus/u8g2/wiki/u8g2reference#drawdisc
   7:  Ellipse               need x,y,i=radiusx,j=radiusy         https://github.com/olikraus/u8g2/wiki/u8g2reference#drawellipse
   8:  Filled Ellipse        need x,y,i=radiusx,j=radiusy         https://github.com/olikraus/u8g2/wiki/u8g2reference#drawfilledellipse
   9:  Rounded Frame         need x,y,i=width,j=height,k=radius   https://github.com/olikraus/u8g2/wiki/u8g2reference#drawrframe
   10: Rounded Box           need x,y,i=width,j=height,k=radius   https://github.com/olikraus/u8g2/wiki/u8g2reference#drawrbox

  -Add "CMDTXT,[Parameter]" Format to "f,c,x,y,[Text]"
   f = Font Type 0.. (see Font List above)
   c = Draw Color 0=Clear, 1=Draw, 2=XOR/Invert Pixel
   x = X-Position 0..255
   y = Y-Position 0..63 
   [Text] = Text to Draw

  2021-06-22 
  -New Command Mode (Testing)
   "CMDCLS"
   "CMDSORG"
   "CMDBYE"
   "CMCOR,[Corename]"
   "CMDCON,[Contrast]"
   "CMDTXT,[Parameter]"   Text-Ouput,          Parameter-Format = "f,c,x,y,[Text]
   "CMDGEO,[Parmeter]"    Geometric-Output,    Parameter Format = "g,c,x,y,i,j,k"
   "CMDRESET" 
   "CMDENOTA"
   "CMDOFF,[Parameter]"   Set Power Save Mode, 0=Disabled/Display ON (default), 1=Enabled/Display OFF

  2021-06-24
  -Changed location of the "yield()" Command which is needed for 8266 NodeMCU

  2021-06-30
  -New Command "CMDROT"
   "CMDROT,[Parameter]" Set Display Rotation (0=180° (My Standard), 1=0 degrees) Rotation after Start
   You will see the Command Result after the next Write/Draw Command.

  2021-07-07/08
  -New Command "CMDTEST" which just show an fullscreen test-picture
  -Count amount of transferred picture bytes and if it doesn't match show an error picture
  -Change Command processing from "if (newCore!=oldCore)" to "if (updateDisplay)" to prevent a blank screen if multiple data packs are sent.
  -Add (Micro Font) Build Version to Start Screen
  
  2021-07-09
  -Some yield() fixes for ESP8266
  
  2021-07-10
  -Add Serial.flush to "Setup" to clear the transmit buffer, see https://misterfpga.org/viewtopic.php?p=29703#p29703

  2021-07-11
  -Some more yield() fixes for ESP8266
  -Bugfix Effekt 10 (Diagonal)

  2021-07-12
  -Combine the two separate LogoDrawing functions (usb2oled_readndrawlogo/usb2oled_readndrawlogo2) into one (usb2oled_readndrawlogo2)

  2021-07-18 (Testing)
  -Trying Handshake with the MiSTer Daemon by sending "ttyack;" (with delimiter) after each processed command.
   !! But this need a very little delay of [cDelay]ms before "ttyack" could be send. => Need more testing !!
   In tty2oled.ini the TTYPARAM Parameter needs "-echo"

  2021-07-12
  -New Command to set the Font Direction. 
   "CMDFOD,[Parameter]", Parameter Format "d", d: 0=Left to Right (0°, default), 1=Top to Down (90°), 2=Right to Left (180°), 3=Down to Top (270°)
   
  2021-08-03
  -Some of the included Logo's miss their U8X8_PROGMEM.

  2021-08-07
  -New/smaller USB Icon.

  2021-08-10
  -Reactivate System "#define USE_xxxx" set by Arduino's Board selection (Auto Mode) 
   or by uncommenting only the needed "#define USEe_XXX" for your Hardware
  -Add "XROTATE". Uncomment it for an Rotated Display.
  
*/

#define BuildVersion "210810T"    // "T" for Testing

#include <Arduino.h>
#include <U8g2lib.h>             // Display Library
#include "logo.h"                // The Pics in XMB Format

// OTA and Reset only for ESP32
#ifdef ESP32
#include "cred.h"                // Load your WLAN Credentials for OTA
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
bool OTAEN=false;                // Will be set to "true" by Command "CMDENOTA"
#endif

// ------------------------ System Config -------------------------------
// Uncomment to get some Debugging Infos over Serial especially for SD Debugging
//#define XDEBUG

// Uncomment for 180° Rotation
//#define XROTATED

// Uncomment for Temperatur Sensor Support
//#define XMIC184

// ----------- Auto-Board-Config via Arduino Board Selection ------------
#ifdef ARDUINO_ESP32_DEV
  #define USE_TTGOT8             // TTGO-T8. Set Arduino Board to "ESP32 Dev Module", chose your xx MB Flash
#endif

#ifdef ARDUINO_LOLIN32
  #define USE_LOLIN32            // Wemos LOLIN32, LOLIN32, DevKit_V4. Set Arduino Board to "WEMOS LOLIN32"
#endif

#if defined(ARDUINO_ESP8266_NODEMCU) || defined(ARDUINO_ESP8266_NODEMCU_ESP12E)
  #define USE_NODEMCU            // ESP8266 NodeMCU v3. Set Arduino Board to "NodeMCU 1.0 (ESP-12E Module)"
#endif

// -------------------- Manual-Board-Config ----------------------------
//#define USE_TTGOT8             // TTGO-T8. Set Arduino Board to ESP32 Dev Module, xx MB Flash, def. Part. Schema
//#define USE_LOLIN32            // Wemos LOLIN32, LOLIN32, DevKit_V4. Set Arduino Board to "WEMOS LOLIN32"
//#define USE_NODEMCU            // ESP8266 NodeMCU v3. Set Arduino Board to NodeMCU 1.0 (ESP-12E Module)

// ------------ Display Objects -----------------
// TTGO-T8 Display Constructor HW-SPI OLED & integrated SD Card, 180° Rotation => U8G2_R2, using VSPI SCLK = 18, MISO = 19, MOSI = 23 and...
#ifdef USE_TTGOT8
  #ifndef XROTATED
    U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 26, /* dc=*/ 25, /* reset=*/ 27);      // 180° Rotation = Default/Display Connector down
  #else
    U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 26, /* dc=*/ 25, /* reset=*/ 27);      // 0° Rotation = Display Connector up
  #endif
#endif

// WEMOS LOLIN32/Devkit_V4 Display Constructor HW-SPI & Adafruit SD_MMC Adapter 180° Rotation => U8G2_R2, using VSPI SCLK = 18, MISO = 19, MOSI = 23, SS = 5 and...
#ifdef USE_LOLIN32
  #ifndef XROTATED
    U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 5, /* dc=*/ 16, /* reset=*/ 17);  // Better because original SPI SS = 5
  #else
    U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 5, /* dc=*/ 16, /* reset=*/ 17);  // Better because original SPI SS = 5
  #endif
#endif

// ESP8266-Board (NodeMCU v3) Display Constructor HW-SPI 180° Rotation => U8G2_R2
#ifdef USE_NODEMCU
  #ifndef XROTATED
    U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 15, /* dc=*/ 4, /* reset=*/ 5);
  #else
    U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 15, /* dc=*/ 4, /* reset=*/ 5);
  #endif
#endif

// ------------ Variables ----------------

// Strings
String newCore = "";             // Received Text, from MiSTer without "\n\r" currently (2021-01-11)
String oldCore = "";             // Buffer String for Text change detection
uint8_t contrast = 5;            // Contrast (brightness) of display, range: 0 (no contrast) to 255 (maximum)
char *newCoreChar;
bool updateDisplay = false;

// Display Vars
u8g2_uint_t DispWidth, DispHeight, DispLineBytes;
unsigned char *logoBin;          // <<== For malloc in Setup
unsigned int logoBytes=0;
const int cDelay=25;             // Command Delay in ms for Handshake
  
// ================ SETUP ==================
void setup(void) {
  // Init Serial
  Serial.begin(115200);                      // 115200 for MiSTer ttyUSBx Device CP2102 Chip on ESP32
  Serial.flush();                            // Wait for empty Send Buffer 

  randomSeed(analogRead(34));                // Init Random Generator with empty Port Analog value

  // Init Display
  u8g2.begin();
  u8g2.setContrast(contrast);                // Set contrast of display
  u8g2.setDrawColor(1);                      // Set Font color to White ( 1= default value)
  u8g2.setFontMode(0);                       // Ser Font Mode (0 = default value)
  u8g2.setFontDirection(0);                  // Set drawing direction of all strings or glyphs (default value)
  u8g2.setFontPosBaseline();                 // Set font Reference Position to BASELINE (default value)
  u8g2.setFontRefHeightText();               // Set the calculation method for the ascent and descent of the current font (default value)
  u8g2.setFont(u8g2_font_tenfatguys_tr);     // 10 Pixel Font

  // Get Display Dimensions
  DispWidth = u8g2.getDisplayWidth();
  DispHeight = u8g2.getDisplayHeight();
  DispLineBytes = DispWidth / 8;                    // How many Bytes each Dipslay Line (SSD1322: 256Pixel/8Bit = 32Bytes each Line)

  // Create Picture Buffer, better than create (malloc) & destroy (free)
  logoBytes = DispWidth * DispHeight / 8;           // Make it more universal, here 2048
  logoBin = (unsigned char *) malloc(logoBytes);    // Reserve Memory for Picture-Data
  
  oled_mistertext();                                // OLED Startup with Some Text
}

// ================================= MAIN LOOP =========================================
void loop(void) {

#ifdef ESP32  // OTA and Reset only for ESP32
  if (OTAEN) ArduinoOTA.handle();                            // OTA active?
#endif

  // Serial Data
  if (Serial.available()) {
    newCore = Serial.readStringUntil('\n');                  // Read string from serial until NewLine "\n" (from MiSTer's echo command) is detected or timeout (1000ms) happens.
    updateDisplay=true;                                      // Set Update-Display Flag

#ifdef XDEBUG
    Serial.printf("Received Corename or Command: %s\n", (char*)newCore.c_str());
#endif
  }  // end serial available
    
  //if (newCore!=oldCore) {                                    // Proceed only if Core Name changed
  if (updateDisplay) {                                    // Proceed only if it's allowed because of new data from serial

    // -- First Transmission --
    if (newCore.endsWith("QWERTZ")) {                        // TESTING: Process first Transmission after PowerOn/Reboot.
        // Do nothing, just receive one string to clear the buffer.
    }                    

    // ----------------------------
    // ----- C O M M A N D 's -----
    // ----------------------------

    else if (newCore=="cls")          u8g2.clear();
    else if (newCore=="sorg")         oled_mistertext();
    else if (newCore=="bye")          oled_drawlogo64h(sorgelig_icon64_width, sorgelig_icon64);
    
    // -- Get "att" = Attention an Command follows
    else if (newCore=="att") {                                           // Do nothing but needed to get an following Command (CONTRAST/CORECHANGE/TEXTOUTXY) working
      // Do nothing (actually)
    }

    // -- Get Data via USB from the MiSTer and show them
    else if (newCore=="CORECHANGE") {                                    // Command from Serial to receive Data via USB Serial from the MiSTer
      usb2oled_readndrawlogo2(random(1,11));                             // ESP32 Receive Picture Data and show them on the OLED, Transition Effect Random Number 1..10
    }
 
    // -- Get Contrast Data via USB from the MiSTer and set them
    else if (newCore=="CONTRAST") {                                     // Command from Serial to receive Contrast-Level Data from the MiSTer
      usb2oled_readnsetcontrast();                                      // Read and Set contrast                                   
    }

    // -- Get Text Data via USB from the MiSTer and write it
    else if (newCore=="TEXTOUTXY") {                                    // Command from Serial to write Text
      usb2oled_readnwritetext();                                        // Read and Write Text
    }

    // -- Create Geometrics out of the Date send by the MiSTer
    else if (newCore=="GEOOUTXY") {                                     // Command from Serial to draw geometrics
      usb2oled_readndrawgeo();                                          // Read and Draw Geometrics
    }

    // ---------------------------------------------------
    // -------------- Command Mode V2 --------------------
    // ---------------------------------------------------

    // -- Test Commands --
    else if (newCore=="CMDCLS") {                                        // Clear Screen
      u8g2.clear();
    }
    
    else if (newCore=="CMDSORG") {                                       // Show Startscreen
      oled_mistertext();
    }
    
    else if (newCore=="CMDBYE") {                                        // Show Sorgelig's Icon
      oled_drawlogo64h(sorgelig_icon64_width, sorgelig_icon64);
    }

    else if (newCore=="CMDTEST") {                                       // Show Test-Picture
      oled_drawlogo64h(TestPicture_width, TestPicture);
    }

    else if (newCore.startsWith("CMDCOR,")) {                            // Command from Serial to receive Picture Data via USB Serial from the MiSTer
      usb2oled_readndrawlogo2(random(1,11));                             // ESP32 Receive Picture Data and show them on the OLED, Transition Effect Random Number 1..10
    }
    
    else if (newCore.startsWith("CMDCON,")) {                            // Command from Serial to receive Contrast-Level Data from the MiSTer
      usb2oled_readnsetcontrast2();                                      // Read and Set contrast                                   
    }

    else if (newCore.startsWith("CMDTXT,")) {                            // Command from Serial to write Text
      usb2oled_readnwritetext2();                                        // Read and Write Text
    }
    
    else if (newCore.startsWith("CMDGEO,")) {                            // Command from Serial to draw geometrics
      usb2oled_readndrawgeo2();                                          // Read and Draw Geometrics
    }

    else if (newCore.startsWith("CMDOFF,")) {                            // Command from Serial to set Power Save Mode
      usb2oled_readnopowersave();                                        // Set Power Save Mode
    }

    else if (newCore.startsWith("CMDROT,")) {                            // Command from Serial to set Rotation
      usb2oled_readnsetrotation();                                       // Set Rotation
    }

    else if (newCore.startsWith("CMDFOD,")) {                            // Command from Serial to set Font Diretcion
      usb2oled_readnsetfontdir();                                       // Set Rotation
    }

    // The following Commands are only for ESP32
#ifdef ESP32  // OTA and Reset only for ESP32
    else if (newCore=="CMDENOTA") {                                      // Command from Serial to enable OTA on the ESP
      enableOTA();                                                       // Setup Wireless and enable OTA
    }

    else if (newCore=="CMDRESET") {                                      // Command from Serial for Resetting the ESP
      ESP.restart();                                                     // Reset ESP
    }
#endif

    // -- Unidentified Core Name, just write it on screen
    else {
      // Get Font
      const uint8_t *old_font = u8g2.getU8g2()->font;
      newCoreChar = (char*)newCore.c_str();
      u8g2.clearBuffer();
      // Set font
      u8g2.setFont(u8g2_font_tenfatguys_tr);
      //u8g2.setFont(u8g2_font_bubble_tr);                        // 18 Pixel Font
      //u8g2.setFont(u8g2_font_maniac_tr);                          // 23 Pixel Font
      u8g2.drawStr(DispWidth/2-(u8g2.getStrWidth(newCoreChar)/2), ( DispHeight - u8g2.getAscent() ) / 2 + u8g2.getAscent(), newCoreChar);  // Write Corename to Display
      u8g2.sendBuffer(); 
      // Set font back
      u8g2.setFont(old_font);
    }  // end ifs
    
    delay(cDelay);                           // Command Response Delay
    Serial.print("ttyack;");                 // Handshake with delimiter; MiSTer: "read -d ";" ttyresponse < ${TTYDEVICE}"
    Serial.flush();                          // Wait for sendbuffer is clear
    //oldCore=newCore;                         // Update Buffer
    updateDisplay=false;                     // Clear Update-Display Flag
  } // end newCore!=oldCore or updateDisplay
} // End Main Loop

//=================== Functions =========================

// ---- oled_mistertext -- Show the Start-Up Text ----
void oled_mistertext(void) {
#ifdef XDEBUG
  Serial.println("Show Startscreen");
#endif
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x7_tr);            // 6 Pixel Font
  u8g2.setCursor(1,63);
  u8g2.print(BuildVersion);
  u8g2.setFont(u8g2_font_tenfatguys_tr);     // 10 Pixel Font
  u8g2.setCursor(DispWidth/2-(u8g2.getStrWidth("MiSTer FPGA")/2), ( DispHeight/2 - u8g2.getAscent() ) / 2 + u8g2.getAscent() );
  u8g2.print("MiSTer FPGA");
  u8g2.setCursor(DispWidth/2-(u8g2.getStrWidth("by Sorgelig")/2), ( DispHeight/2 - u8g2.getAscent() ) / 2 + u8g2.getAscent() + DispHeight/2 );
  u8g2.print("by Sorgelig");

  //u8g2.drawXBMP(DispWidth-usb_icon_width, 0, usb_icon_width, usb_icon_height, usb_icon);
  u8g2.drawXBMP(DispWidth-usb_icon_width, DispHeight-usb_icon_height, usb_icon_width, usb_icon_height, usb_icon);
  
  u8g2.sendBuffer();
}

// ---- oled_drawlogo64h -- Draw Pictures with an height of 64 Pixel centerred ----
void oled_drawlogo64h(u8g2_uint_t w, const uint8_t *bitmap) {
  u8g2.clearBuffer();
  u8g2.drawXBMP(DispWidth/2-w/2, 0, w, DispHeight, bitmap);
  u8g2.sendBuffer();
} // end oled_drawlogo64h

// --- usb2oled_readnsetcontrast -- Receive and set Display Contrast ----
void usb2oled_readnsetcontrast(void) {
#ifdef XDEBUG
  Serial.println("Called Function CONTRAST");
#endif
  while (!Serial.available()) {                                          //
    // Just wait here
  }
  u8g2.setContrast(Serial.readStringUntil('\n').toInt());            // Read and Set contrast  
}


// --- usb2oled_readnwritetext -- Receive and set Display Contrast ----
void usb2oled_readnwritetext(void) {
  int x=0,y=0,f=0,d1=0,d2=0,d3=0;
  String TextIn="", xPos="", yPos="", FontType="", TextOut="";
  //char *TextOutChar;
  
#ifdef XDEBUG
  Serial.println("Called Function TEXTOUTXY");
#endif
 
  while (!Serial.available()) {                                          //
    // Just wait here for the Text
  }
  TextIn = Serial.readStringUntil('\n');                                // Read Text
#ifdef XDEBUG
  Serial.printf("Received Text: %s\n", (char*)TextIn.c_str());
#endif

  //Searching for the "," delimiter
  d1 = TextIn.indexOf(',');                 // Find location of first ","
  d2 = TextIn.indexOf(',', d1+1 );          // Find location of second ","
  d3 = TextIn.indexOf(',', d2+1 );          // Find location of third ","

  //Create Substrings
  xPos = TextIn.substring(0, d1);           // Get String for X-Position
  yPos = TextIn.substring(d1+1, d2);        // Get String for Y-Position
  FontType = TextIn.substring(d2+1, d3);    // Get String for Font-Type
  TextOut = TextIn.substring(d3+1);         // Get String for Text
  
#ifdef XDEBUG
  Serial.printf("Created Strings: X: %s Y: %s F: %s T: %s\n", (char*)xPos.c_str(), (char*)yPos.c_str(), (char*)FontType.c_str(), (char*)TextOut.c_str());
#endif

  // Convert Strings to Integer
  x = xPos.toInt();
  y = yPos.toInt();
  f = FontType.toInt();
  
  // Parameter check
  if (x<0 || x>DispWidth-1 || y<0 || y>DispHeight-1 || f<0 || d1==-1 || d2==-1 || d3==-1) {
    x=5;
    y=40;
    f=7;
    TextOut="Parameter Error";
  }
  
  // Prepare for Text-Output
  const uint8_t *old_font = u8g2.getU8g2()->font;  // Save current Font
  //Set Font
  switch (f) {
    case 0:
      u8g2.setFont(u8g2_font_luBS08_tf);          // Transparent Font 20x12, 8 Pixel A
      break;
    case 1:
      u8g2.setFont(u8g2_font_luBS10_tf);          // Transparent Font 26x15, 10 Pixel A
      break;
    case 2:
      u8g2.setFont(u8g2_font_luBS14_tf);          // Transparent Font 35x22, 14 Pixel A
      break;
    case 3:
      u8g2.setFont(u8g2_font_luBS18_tf);          // Transparent Font 44x28, 18 Pixel A
      break;
    case 4:
      u8g2.setFont(u8g2_font_luBS24_tf);          // Transparent Font 61x40, 24 Pixel A
      break;
    case 5:
      u8g2.setFont(u8g2_font_profont12_mf);       // Non-Transparent Font 6x12, 8 Pixel A
      break;
    case 6:
      u8g2.setFont(u8g2_font_profont17_mf);       // Non-Transparent Font 9x17, 11 Pixel A
      break;
    case 7:
      u8g2.setFont(u8g2_font_profont22_mf);       // Non-Transparent Font 12x22, 14 Pixel A
      break;
    case 8:
      u8g2.setFont(u8g2_font_profont29_mf);       // Non-Transparent Font 16x29, 19 Pixel A
      break;
    case 9:
      u8g2.setFont(u8g2_font_open_iconic_all_2x_t);          // Icons 16x16 Pixel
      break;
    case 10:
      u8g2.setFont(u8g2_font_lucasarts_scumm_subtitle_o_tf); // Nice 12 Pixel Font
      break;
    default:
      u8g2.setFont(u8g2_font_profont17_mf);
      break;
  }
  // Output of Text
  u8g2.drawStr(x, y, (char*)TextOut.c_str());
  u8g2.sendBuffer();
  u8g2.setFont(old_font);                          // Set Font back
}


// --- usb2oled_readndrawgeo -- Receive and Draw some Geometrics ----
void usb2oled_readndrawgeo(void) {
  int g=0,c=0,x=0,y=0,i=0,j=0,k=0,d1=0,d2=0,d3=0,d4=0,d5=0,d6=0;
  String TextIn="",gT="",cT="",xT="",yT="",iT="",jT="",kT="";
  bool pError=false;
  
#ifdef XDEBUG
  Serial.println("Called Function GEOOUTXY");
#endif
 
  while (!Serial.available()) {                                          //
    // Just wait here for the Text
  }
  TextIn = Serial.readStringUntil('\n');                                // Read Text

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

  //Create Substrings
  gT = TextIn.substring(0, d1);           // Get String for Geometric-Type
  cT = TextIn.substring(d1+1, d2);        // Get String for Clear Flag
  xT = TextIn.substring(d2+1, d3);        // Get String for X-Position
  yT = TextIn.substring(d3+1, d4);        // Get String for Y-Position
  iT = TextIn.substring(d4+1, d5);        // Get String for Parameter i
  jT = TextIn.substring(d5+1, d6);        // Get String for Parameter j
  kT = TextIn.substring(d6+1);            // Get String for Parameter k

#ifdef XDEBUG
  Serial.printf("Part-Strings: G:%s C:%s X:%s Y:%s I:%s J:%s K:%s\n", (char*)gT.c_str(), (char*)cT.c_str(), (char*)xT.c_str(), (char*)yT.c_str(), (char*)iT.c_str(), (char*)jT.c_str(), (char*)kT.c_str() );
#endif

  // Convert Strings to Integer
  g = gT.toInt();
  c = cT.toInt();
  x = xT.toInt();
  y = yT.toInt();
  i = iT.toInt();
  j = jT.toInt();
  k = kT.toInt();

#ifdef XDEBUG
  Serial.printf("Values: G:%i C:%i X:%i Y:%i I:%i J:%i K:%i\n", g,c,x,y,i,j,k);
#endif

  // Enough Parameter given / Parameter Check
  if (g<1 || g>10 || c<0 || c>2 || x<0 || x>DispWidth-1 || y<0 || y>DispHeight-1 || d1==-1 || d2==-1 || d3==-1  || d4==-1 || d5==-1  || d6==-1) {
    pError=true;
  }

  if (!pError) {
    if (c==0) u8g2.setDrawColor(0);  // Set Draw Color to Background Color (Clear)
    if (c==2) u8g2.setDrawColor(2);  // Set Draw Color to to XOR
    switch (g) {
      case 1:  // Pixel x,y
        u8g2.drawPixel(x,y);
        break;
      case 2:  // Line x0,y0,x1,y1
        u8g2.drawLine(x,y,i,j);
        break;
      case 3:  // Frame x,y,w,h
        u8g2.drawFrame(x,y,i,j);
        break;
      case 4:  // Filled Frame/Box x,y,w,h
        u8g2.drawBox(x,y,i,j);
        break;
      case 5:  // Circle x,y,r
        u8g2.drawCircle(x,y,i);
        break;
      case 6:  // Filled Circle/Disc x,y,r
        u8g2.drawDisc(x,y,i);
        break;
      case 7:  // EllipseCircle x,y,rw,rh
        u8g2.drawEllipse(x,y,i,j);
        break;
      case 8:  // drawFilledEllipse x,y,rw,rh
        u8g2.drawFilledEllipse(x,y,i,j);
        break;
      case 9:  // Rounded Frame x,y,w,h,r
        u8g2.drawRFrame(x,y,i,j,k);
        break;
      case 10: // Rounded Box x,y,w,h,r
        u8g2.drawRBox(x,y,i,j,k);
        break;
      default:  // Just something :-)
        u8g2.drawEllipse(128,32,32,16);
        u8g2.drawDisc(128,32,8);
        break;
    }
  }
  else {
    u8g2.drawStr(5, 40, "Parameter Error");
  }

  u8g2.sendBuffer();       // Draw Display Content
  u8g2.setDrawColor(1);    // Back to normal Draw Color
}


// Draw one XBM Byte, called from the Effects in function sd2oled_readndrawlogo
void drawEightBit(int x, int y, unsigned char b) {

  for (int i=0; i<8; i++){
    if (bitRead(b, i)) {
      // Set Pixel
      u8g2.drawPixel(x+i,y);
    }
    else {
      // Clear Pixel
      u8g2.setDrawColor(0);
      u8g2.drawPixel(x+i,y);
      u8g2.setDrawColor(1);        
    }  // end bit read
  }  // end for j

#ifdef USE_NODEMCU
  yield();
#endif

}

// -----------------------------------------------------------------------------
// -------------------------- Commands V2 --------------------------------------
// ------------------- Commands Starting with "CMD"-----------------------------
// -----------------------------------------------------------------------------

// ----------------- Command Read an Set Contrast ------------------------------
void usb2oled_readnsetcontrast2(void) {
  String cT="";
#ifdef XDEBUG
  Serial.println("Called Command CMDCON");
#endif
  
  cT=newCore.substring(7);

#ifdef XDEBUG
  Serial.printf("Received Text: %s\n", (char*)cT.c_str());
#endif

  u8g2.setContrast(cT.toInt());            // Read and Set contrast  
}

// ----------------- Command Read an Set Power Save Mode ------------------------------
void usb2oled_readnopowersave(void) {
  String pT="";

#ifdef XDEBUG
  Serial.println("Called Command CMDOFF");
#endif
  
  pT=newCore.substring(7);

#ifdef XDEBUG
  Serial.printf("Received Text: %s\n", (char*)pT.c_str());
#endif

  u8g2.setPowerSave(pT.toInt());            // Set Power Save Modecontrast  
}

// ----------------- Command Read and Set Rotation ------------------------------
void usb2oled_readnsetrotation(void) {
  String rT="";
  int r=0;
  
#ifdef XDEBUG
  Serial.println("Called Command CMDROT");
#endif
  
  rT=newCore.substring(7);

#ifdef XDEBUG
  Serial.printf("Received Text: %s\n", (char*)rT.c_str());
#endif

  r=rT.toInt();
  
  switch (r) {
    case 0:
      u8g2.setDisplayRotation(U8G2_R2);
    break;
    case 1:
      u8g2.setDisplayRotation(U8G2_R0);
    break;
    default:
      u8g2.setDisplayRotation(U8G2_R2);
    break;
  }
}

// ----------------- Command Read and Set Font Direction ------------------------------
void usb2oled_readnsetfontdir(void) {
  String dT="";
  int d=0;
  
#ifdef XDEBUG
  Serial.println("Called Command CMDFOD");
#endif
  
  dT=newCore.substring(7);

#ifdef XDEBUG
  Serial.printf("Received Text: %s\n", (char*)dT.c_str());
#endif

  d=dT.toInt();
  
  switch (d) {
    case 0:
      u8g2.setFontDirection(0);
    break;
    case 1:
      u8g2.setFontDirection(1);
    break;
    case 2:
      u8g2.setFontDirection(2);
    break;
    case 3:
      u8g2.setFontDirection(3);
    break;
    default:
      u8g2.setFontDirection(0);
    break;
  }
}

// -----------------------Command Read and Draw Logo ----------------------------
int usb2oled_readndrawlogo2(int effect) {
  //const int logoBytes = DispWidth * DispHeight / 8; // Make it more universal, here 2048
  int logoByte;
  unsigned char logoByteValue;
  int w,x,y,x2;
  size_t bytesReadCount=0;
  String cN="";

// Make it universal for "CMDCOR" and "CORECHANGE"
  if (newCore.startsWith("CMDCOR,")) {
#ifdef XDEBUG
    Serial.println("Called Command CMDCOR");
#endif
    cN=newCore.substring(7);  // Cre
#ifdef XDEBUG
    Serial.printf("Received Text: %s\n", (char*)cN.c_str());
#endif
  }
  else {
#ifdef XDEBUG
    Serial.println("Called Function CORECHANGE");
#endif    
  }
  
#ifdef USE_NODEMCU
  yield();
#endif

  bytesReadCount = Serial.readBytes(logoBin, logoBytes);  // Read 2048 Bytes from Serial

#ifdef USE_NODEMCU
  yield();
#endif

  // Check if 2048 Bytes read
  if (bytesReadCount != logoBytes) {
    oled_drawlogo64h(transfererror_width, transfererror_pic);
  }
  else {
    // Draw the Picture
    // -------------------- Effects -----------------------
    switch (effect) {
    case 1:                                        // Left to Right
      for (x=0; x<DispLineBytes; x++) {
        for (y=0; y<DispHeight; y++) {
          logoByteValue = logoBin[x+y*DispLineBytes];
          drawEightBit(x*8, y, logoByteValue);
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;
    
    case 2:                                       // Top to Bottom
      for (y=0; y<DispHeight; y++) {
        for (x=0; x<DispLineBytes; x++) {
          logoByteValue = logoBin[x+y*DispLineBytes];
          drawEightBit(x*8, y, logoByteValue);
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;

    case 3:                                        // Right to Left
      for (x=DispLineBytes-1; x>=0; x--) {
        for (y=0; y<DispHeight; y++) {
          logoByteValue = logoBin[x+y*DispLineBytes];
          drawEightBit(x*8, y, logoByteValue);
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;

    case 4:                                       // Bottom to Top
      for (y=DispHeight-1; y>=0; y--) {
        for (x=0; x<DispLineBytes; x++) {
          logoByteValue = logoBin[x+y*DispLineBytes];
          drawEightBit(x*8, y, logoByteValue);
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;
    
    case 5:                                     // Even Line Left to Right / Odd Line Right to Left
      for (x=0; x<DispLineBytes; x++) {
        for (y=0; y<DispHeight; y++) {
          if ((y % 2) == 0) {
            x2 = x;
          }
          else {
            x2 = x*-1 + DispLineBytes -1;
          }
          logoByteValue = logoBin[x2+y*DispLineBytes];
          drawEightBit(x2*8, y, logoByteValue);
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;
    
    case 6:                                     // Top Part Left to Right / Bottom Part Right to Left
      for (x=0; x<DispLineBytes; x++) {
        for (y=0; y<DispHeight; y++) {
          if (y < DispLineBytes) {
            x2 = x;
          }
          else {
            x2 = x*-1 + DispLineBytes -1;
          }
          logoByteValue = logoBin[x2+y*DispLineBytes];
          drawEightBit(x2*8, y, logoByteValue);
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;
    
    case 7:                                     // Four Parts Left to Right to Left to Right...
      for (w=0; w<4; w++) {
        for (x=0; x<DispLineBytes; x++) {
          for (y=0; y<DispHeight/4; y++) {
            if ((w%2) == 0) {
              x2 = x;
            }
            else {
              x2 = x*-1 + DispLineBytes -1;
            }
            logoByteValue = logoBin[x2+(y+w*16)*DispLineBytes];
            drawEightBit(x2*8, y+w*16, logoByteValue);
          }  // end for y
          u8g2.sendBuffer();
        }  // end for x
      }
      break;

    case 8:                                      // Particle Effect
      for (w=0; w<10000; w++) {
        logoByte = random(logoBytes); // Value logoBytes = 2048 => Get 0..2047
        logoByteValue = logoBin[logoByte];
        x = (logoByte % DispLineBytes) * 8;
        y = logoByte / DispLineBytes;
        drawEightBit(x, y, logoByteValue);
        // For different speed
        // if ((w % (w/10)) == 0) u8g2.sendBuffer();
        if (w<=1000) {
          if ((w % 25)==0) u8g2.sendBuffer();
        }
        if ((w>1000) && (w<=2000)) {
          if ((w % 50)==0) u8g2.sendBuffer();
        }
        if ((w>2000) && (w<=5000)) { 
          if ((w % 200)==0) u8g2.sendBuffer();
        }
        if (w>5000) { 
          if ((w % 400)==0) u8g2.sendBuffer();
        }
      }
      // Finally overwrite the Screen with fill Size Picture
      u8g2.drawXBM(0, 0, DispWidth, DispHeight, logoBin);
      u8g2.sendBuffer();
    break;

    case 9:                                     // 4 Parts, Top-Left => Bottom-Right => Top-Right => Bottom-Left
      // Part 1 Top Left
      for (x=0; x<DispLineBytes/2; x++) {
        for (y=0; y<DispHeight/2; y++) {
          logoByteValue = logoBin[x+y*DispLineBytes];
          drawEightBit(x*8, y, logoByteValue);
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      // Part 2 Bottom Right
      for (x=DispLineBytes/2; x<DispLineBytes; x++) {
        for (y=DispHeight/2; y<DispHeight; y++) {
          logoByteValue = logoBin[x+y*DispLineBytes];
          drawEightBit(x*8, y, logoByteValue);
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      // Part 3 Top Right
      //for (x=DispLineBytes/2; x<DispLineBytes; x++) {
      for (x=DispLineBytes-1; x>=DispLineBytes/2; x--) {
        for (y=0; y<DispHeight/2; y++) {
          logoByteValue = logoBin[x+y*DispLineBytes];
          drawEightBit(x*8, y, logoByteValue);
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      // Part 4 Bottom Left
      //for (x=0; x<DispLineBytes/2; x++) {
      for (x=DispLineBytes/2-1; x>=0; x--) {
        for (y=DispHeight/2; y<DispHeight; y++) {
          logoByteValue = logoBin[x+y*DispLineBytes];
          drawEightBit(x*8, y, logoByteValue);
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;

    case 10:                                       // Left to Right Diagonally
      for (x=0; x<DispLineBytes+DispHeight; x++) {
        for (y=0; y<DispHeight; y++) {
          // x2 calculation = Angle
          //x2=x-y;                                // Long Diagonal
          //x2=x-y/2;                              // Middle Diagonal
          x2=x-y/4;                                // Short Diagonal
          if ((x2>=0) && (x2<DispLineBytes)) {
            logoByteValue = logoBin[x2+y*DispLineBytes];
            drawEightBit(x2*8, y, logoByteValue);
          }  // end for x2
          else {
#ifdef USE_NODEMCU
            yield();
#endif
          }
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;

      default:                                                 // Just overwrite the whole screen
        //u8g2.clearBuffer();
        u8g2.drawXBM(0, 0, DispWidth, DispHeight, logoBin);
        u8g2.sendBuffer();
        break;  // endcase default
    }  // end switch effect
  }                              // endif bytesCount
  //free(logoBin);
  return 1;                      // Everything OK
}  // end sd2oled_readndrawlogo2



// ------------------- Command Read an Write Text ------------------------------
void usb2oled_readnwritetext2(void) {
  int f=0,c=0,x=0,y=0,d1=0,d2=0,d3=0,d4=0;
  String TextIn="", fT="", cT="", xT="", yT="", TextOut="";
  //char *TextOutChar;
  
#ifdef XDEBUG
  Serial.println("Called Command CMDTEX");
#endif
 
  TextIn = newCore.substring(7);            // Get Command Text from "newCore"
  
#ifdef XDEBUG
  Serial.printf("Received Text: %s\n", (char*)TextIn.c_str());
#endif

  //Searching for the "," delimiter
  d1 = TextIn.indexOf(',');                 // Find location of first ","
  d2 = TextIn.indexOf(',', d1+1 );          // Find location of second ","
  d3 = TextIn.indexOf(',', d2+1 );          // Find location of third ","
  d4 = TextIn.indexOf(',', d3+1 );          // Find location of third ","

  //Create Substrings
  fT = TextIn.substring(0, d1);             // Get String for Font-Type
  cT = TextIn.substring(d1+1, d2);          // Get String for Draw Color
  xT = TextIn.substring(d2+1, d3);          // Get String for X-Position
  yT = TextIn.substring(d3+1, d4);          // Get String for Y-Position
  TextOut = TextIn.substring(d4+1);         // Get String for Text
  
#ifdef XDEBUG
  Serial.printf("Created Strings: F:%s C%s X:%s Y:%s T:%s\n", (char*)fT.c_str(), (char*)cT.c_str(), (char*)xT.c_str(), (char*)yT.c_str(), (char*)TextOut.c_str());
#endif

  // Convert Strings to Integer
  f = fT.toInt();
  c = cT.toInt();
  x = xT.toInt();
  y = yT.toInt();
  
  // Parameter check
  if (f<0 || c<0 || c>2 || x<0 || x>DispWidth-1 || y<0 || y>DispHeight-1 || d1==-1 || d2==-1 || d3==-1 || d4==-1) {
    f=7;
    x=5;
    y=40;
    TextOut="Parameter Error";
  }
  
  // Prepare for Text-Output
  const uint8_t *old_font = u8g2.getU8g2()->font;  // Save current Font
  if (c==0) u8g2.setDrawColor(0);  // Set Draw Color to Background Color (Clear)
  if (c==2) {
    u8g2.setDrawColor(2);          // Set Draw Color to to XOR/Invert Pixel
    u8g2.setFontMode(1);           // Enable Transparent Font Mode
  }
  //Set Font
  switch (f) {
    case 0:
      u8g2.setFont(u8g2_font_luBS08_tf);          // Transparent Font 20x12, 8 Pixel A
      break;
    case 1:
      u8g2.setFont(u8g2_font_luBS10_tf);          // Transparent Font 26x15, 10 Pixel A
      break;
    case 2:
      u8g2.setFont(u8g2_font_luBS14_tf);          // Transparent Font 35x22, 14 Pixel A
      break;
    case 3:
      u8g2.setFont(u8g2_font_luBS18_tf);          // Transparent Font 44x28, 18 Pixel A
      break;
    case 4:
      u8g2.setFont(u8g2_font_luBS24_tf);          // Transparent Font 61x40, 24 Pixel A
      break;
    case 5:
      u8g2.setFont(u8g2_font_profont12_mf);       // Non-Transparent Font 6x12, 8 Pixel A
      break;
    case 6:
      u8g2.setFont(u8g2_font_profont17_mf);       // Non-Transparent Font 9x17, 11 Pixel A
      break;
    case 7:
      u8g2.setFont(u8g2_font_profont22_mf);       // Non-Transparent Font 12x22, 14 Pixel A
      break;
    case 8:
      u8g2.setFont(u8g2_font_profont29_mf);       // Non-Transparent Font 16x29, 19 Pixel A
      break;
    case 9:
      u8g2.setFont(u8g2_font_open_iconic_all_2x_t);          // Icons 16x16 Pixel
      break;
    case 10:
      u8g2.setFont(u8g2_font_lucasarts_scumm_subtitle_o_tf); // Nice 12 Pixel Font
      break;
    default:
      u8g2.setFont(u8g2_font_profont17_mf);
      break;
  }
  // Output of Text
  u8g2.drawStr(x, y, (char*)TextOut.c_str());
  u8g2.sendBuffer();
  u8g2.setFont(old_font);                          // Set Font back
  u8g2.setDrawColor(1);                            // Back to normal Draw Color (1)
  u8g2.setFontMode(0);                             // Back to normale Font Mode (0)
}


// ------------------ Command Read and Draw Geometrics ------------------
void usb2oled_readndrawgeo2(void) {
  int g=0,c=0,x=0,y=0,i=0,j=0,k=0,d1=0,d2=0,d3=0,d4=0,d5=0,d6=0;
  String TextIn="",gT="",cT="",xT="",yT="",iT="",jT="",kT="";
  bool pError=false;
  
#ifdef XDEBUG
  Serial.println("Called Command CMDGEO");
#endif

  TextIn = newCore.substring(7);             // Get Command Text from "newCore"
  
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

  //Create Substrings
  gT = TextIn.substring(0, d1);           // Get String for Geometric-Type
  cT = TextIn.substring(d1+1, d2);        // Get String for Clear Flag
  xT = TextIn.substring(d2+1, d3);        // Get String for X-Position
  yT = TextIn.substring(d3+1, d4);        // Get String for Y-Position
  iT = TextIn.substring(d4+1, d5);        // Get String for Parameter i
  jT = TextIn.substring(d5+1, d6);        // Get String for Parameter j
  kT = TextIn.substring(d6+1);            // Get String for Parameter k

#ifdef XDEBUG
  Serial.printf("Part-Strings: G:%s C:%s X:%s Y:%s I:%s J:%s K:%s\n", (char*)gT.c_str(), (char*)cT.c_str(), (char*)xT.c_str(), (char*)yT.c_str(), (char*)iT.c_str(), (char*)jT.c_str(), (char*)kT.c_str() );
#endif

  // Convert Strings to Integer
  g = gT.toInt();
  c = cT.toInt();
  x = xT.toInt();
  y = yT.toInt();
  i = iT.toInt();
  j = jT.toInt();
  k = kT.toInt();

#ifdef XDEBUG
  Serial.printf("Values: G:%i C:%i X:%i Y:%i I:%i J:%i K:%i\n", g,c,x,y,i,j,k);
#endif

  // Enough Parameter given / Parameter Check
  if (g<1 || g>10 || c<0 || c>2 || x<0 || x>DispWidth-1 || y<0 || y>DispHeight-1 || d1==-1 || d2==-1 || d3==-1  || d4==-1 || d5==-1  || d6==-1) {
    pError=true;
  }

  if (!pError) {
    if (c==0) u8g2.setDrawColor(0);  // Set Draw Color to Background Color (Clear)
    if (c==2) u8g2.setDrawColor(2);  // Set Draw Color to to XOR
    switch (g) {
      case 1:  // Pixel x,y
        u8g2.drawPixel(x,y);
        break;
      case 2:  // Line x0,y0,x1,y1
        u8g2.drawLine(x,y,i,j);
        break;
      case 3:  // Frame x,y,w,h
        u8g2.drawFrame(x,y,i,j);
        break;
      case 4:  // Filled Frame/Box x,y,w,h
        u8g2.drawBox(x,y,i,j);
        break;
      case 5:  // Circle x,y,r
        u8g2.drawCircle(x,y,i);
        break;
      case 6:  // Filled Circle/Disc x,y,r
        u8g2.drawDisc(x,y,i);
        break;
      case 7:  // EllipseCircle x,y,rw,rh
        u8g2.drawEllipse(x,y,i,j);
        break;
      case 8:  // drawFilledEllipse x,y,rw,rh
        u8g2.drawFilledEllipse(x,y,i,j);
        break;
      case 9:  // Rounded Frame x,y,w,h,r
        u8g2.drawRFrame(x,y,i,j,k);
        break;
      case 10: // Rounded Box x,y,w,h,r
        u8g2.drawRBox(x,y,i,j,k);
        break;
      default:  // Just something :-)
        u8g2.drawEllipse(128,32,32,16);
        u8g2.drawDisc(128,32,8);
        break;
    }
  }
  else {
    u8g2.drawStr(5, 40, "Parameter Error");
  }

  u8g2.sendBuffer();       // Draw Display Content
  u8g2.setDrawColor(1);    // Back to normal Draw Color
}


// ---------------- Command Enable OTA ---------------------- 

#ifdef ESP32  // OTA and Reset only for ESP32
void enableOTA (void) {
  Serial.println("Connecting to Wireless..");
  u8g2.setFont(u8g2_font_profont17_mf);
  u8g2.clearBuffer();
  u8g2.setCursor(10,30);
  u8g2.print("Connecting to Wireless..");
  u8g2.sendBuffer();
  WiFi.mode(WIFI_STA);
  WiFi.begin(MySSID, MyPWD);
//  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection failed! Reboot...");;
    u8g2.clearBuffer();
    u8g2.setCursor(10,30);
    u8g2.print("Conn.failed! Reboot...");
    u8g2.sendBuffer();
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
    u8g2.setCursor(10,60);
    u8g2.printf("Progress: %u%%", (progress / (total / 100)));
    u8g2.sendBuffer();
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
  
  u8g2.clearBuffer();
  u8g2.setCursor(10,20);
  u8g2.print("OTA Active!");
  u8g2.setCursor(10,40);
  u8g2.print("IP address: ");
  u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();
  
  OTAEN = true;  // Set OTA Enabled to True for the "Handler" in "loop"
}
#endif

//========================== The end ================================
