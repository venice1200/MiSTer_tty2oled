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
   Use now an TTGO-T8  ESP32 with integrated SD Micro Slot https://github.com/LilyGO/TTGO-T8-ESP32 .
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
  -Adding "TEXTOUTXY" Function
   With this additional function you can send text to the Display without using the tty2oled scripts.
   ! The Serial Interface needs to be correctly configured !
   The Format is "xxx,yy,f,[Text]"
   xxx = 3 Digits X-Position 000..255
   yy  = 2 Digits Y-Position 00..63 
   f   = Font Type 0..9
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
   10: u8g2_font_lucasarts_scumm_subtitle_o_tf (Nice 12 Pixel Fon, Transparent)

*/

// Uncomment to get some Debugging Infos over Serial especially for SD Debugging
//#define XDEBUG

// USE Parameter are currently unused !! 
// Uncomment ONLY one Board !!
//#define USE_TTGOT8             // TTGO-T8, Arduino: ESP32 Dev Module, xx MB Flash, def. Part. Schema
//#define USE_LOLIN32            // Wemos LOLIN32, Arduino: WEMOS LOLIN32
//#define USE_DEVKIT4            // Az-Delivery Devkitc_V4, Arduino: LOLIN32D or ESP32 Dev Module
//#define USE_NODEMCU            // NODEMCU, Arduino: ESP8266 NodeMCU v3

#include <Arduino.h>
#include <U8g2lib.h>    // Display Library
#include "logo.h"       // The Pics in XMB Format

// ------------ Display Objects Automatic Mode -----------------

// TTGO-T8 Display Constructor HW-SPI OLED & integrated SD Card, 180° Rotation => U8G2_R2
// Using VSPI SCLK = 18, MISO = 19, MOSI = 23 and...
#ifdef ARDUINO_ESP32_DEV
U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 26, /* dc=*/ 25, /* reset=*/ 27);
#endif

// WEMOS LOLIN32 Display Constructor HW-SPI & Adafruit SD_MMC Adapter 180° Rotation => U8G2_R2
// Devkitc V4 works as well with these settings.
// Using VSPI SCLK = 18, MISO = 19, MOSI = 23, SS = 5 and...
#ifdef ARDUINO_LOLIN32
U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 5, /* dc=*/ 16, /* reset=*/ 17);  // Better because original SPI SS = 5
#endif

// ESP8266-Board (NodeMCU v3) Display Constructor HW-SPI 180° Rotation => U8G2_R2
#if defined(ARDUINO_ESP8266_NODEMCU) || defined(ARDUINO_ESP8266_NODEMCU_ESP12E)
U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 15, /* dc=*/ 4, /* reset=*/ 5);
#endif

// ------------ Display Objects Manual Mode -----------------

// Display Constructor HW-SPI ESP32-Board TTGO T8 OLED & integrated SD Card, 180° Rotation => U8G2_R2
// Using VSPI SCLK = 18, MISO = 19, MOSI = 23 and...
//U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 26, /* dc=*/ 25, /* reset=*/ 27);

// WEMOS LOLIN32 Display Constructor HW-SPI & Adafruit SD_MMC Adapter, 180° Rotation => U8G2_R2
// Using VSPI SCLK = 18, MISO = 19, MOSI = 23, SS = 5 and...
//U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 5, /* dc=*/ 16, /* reset=*/ 17);

// Devkitc V4 Display Constructor HW-SPI, 180° Rotation => U8G2_R2
// Using VSPI SCLK = 18, MISO = 19, MOSI = 23 and...
//U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 5, /* dc=*/ 16, /* reset=*/ 17);

// ESP8266-Board (NodeMCU v3) Display Constructor HW-SPI, 180° Rotation => U8G2_R2
//U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 15, /* dc=*/ 4, /* reset=*/ 5);

// ------------ Variables ----------------

// Strings
String newCore = "";             // Received Text, from MiSTer without "\n\r" currently (2021-01-11)
String oldCore = "";             // Buffer String for Text change detection
uint8_t contrast = 200;          // Contrast (brightness) of display, range: 0 (no contrast) to 255 (maximum)
char *newCoreChar;

// Display Vars
u8g2_uint_t DispWidth, DispHeight, DispLineBytes;


// ================ SETUP ==================
void setup(void) {
  // Init Serial
  //Serial.begin(57600);                       // 57600 for MiSTer ttyUSBx Device CP2102 Chip on ESP32
  Serial.begin(115200);                      // 115200 for MiSTer ttyUSBx Device CP2102 Chip on ESP32
  //Serial.begin(921600);                     // 921600 for MiSTer ttyUSBx Device CP2102 Chip on ESP32

  randomSeed(analogRead(34));                // Init Random Generator with empty Analog Port value

  // Init Display
  u8g2.begin();
  u8g2.setContrast(contrast);                // Set contrast of display
  u8g2.setDrawColor(1);                      // Set font color to White (default value)
  u8g2.setFontDirection(0);                  // Set drawing direction of all strings or glyphs (default value)
  u8g2.setFontPosBaseline();                 // Set font Reference Position to BASELINE (default value)
  u8g2.setFontRefHeightText();               // Set the calculation method for the ascent and descent of the current font (default value)
  u8g2.setFont(u8g2_font_tenfatguys_tr);     // 10 Pixel Font

  // Get Display Dimensions
  DispWidth = u8g2.getDisplayWidth();
  DispHeight = u8g2.getDisplayHeight();
  DispLineBytes = DispWidth / 8;             // How many Bytes each Dipslay Line (SSD1322: 256Pixel/8Bit = 32Bytes each Line)

  // OLED Startup with Some Text
  oled_mistertext();
}

// ================ MAIN LOOP ===================
void loop(void) {

  if (Serial.available()) {
    newCore = Serial.readStringUntil('\n');                  // Read string from serial until NewLine "\n" (from MiSTer's echo command) is detected or timeout (1000ms) happens.

#ifdef XDEBUG
    Serial.printf("Received Corename or Command: %s\n", (char*)newCore.c_str());
#endif
  }  // end serial available
    
  if (newCore!=oldCore) {                                    // Proceed only if Core Name changed
    // Many if an elses as switch/case is not working (maybe later with an array).

    // -- First Transmission --
    if (newCore.endsWith("QWERTZ")) {                        // TESTING: Process first Transmission after PowerOn/Reboot.
        // Do nothing, just receive one string to clear the buffer.
    }                    

    // -- Test Commands --
    else if (newCore=="cls")          u8g2.clear();
    else if (newCore=="sorg")         oled_mistertext();
    else if (newCore=="bye")          oled_drawlogo64h(sorgelig_icon64_width, sorgelig_icon64);
    
    // -- Get "att" = Attention an Command follows
    else if (newCore=="att") {                                           // Do nothing but needed to get an following Command (CONTRAST/CORECHANGE/TEXTOUTXY) working
      // Do nothing (actually)
    }

    // -- Get Data via USB from the MiSTer and show them
    else if (newCore=="CORECHANGE") {                                    // Annoucement to receive Data via USB Serial from the MiSTer
      usb2oled_readndrawlogo(random(1,11));                              // Receive Picture Data and show them on the OLED, Transition Effect Random Number 1..10
    }
 
    // -- Get Contrast Data via USB from the MiSTer and set them
    else if (newCore=="CONTRAST") {                                     // Annoucement to receive Contrast-Level Data from the MiSTer
      usb2oled_readnsetcontrast();                                      // Read and Set contrast                                   
    }

    // -- Get Text Data via USB from the MiSTer and write it
    else if (newCore=="TEXTOUTXY") {                                     // Annoucement to receive Contrast-Level Data from the MiSTer
#ifdef XDEBUG
      Serial.println("Call Function TEXTOUTXY");
#endif
      usb2oled_readnwritetext();                                         // Read and Write Text
    }

    // -- Unidentified Core Name, just write it on screen
    else {
      // Get Font
      const uint8_t *old_font = u8g2.getU8g2()->font;
      newCoreChar = (char*)newCore.c_str();
      u8g2.clearBuffer();
      // Set font
      //u8g2.setFont(u8g2_font_bubble_tr);                        // 18 Pixel Font
      u8g2.setFont(u8g2_font_maniac_tr);                          // 23 Pixel Font
      u8g2.drawStr(DispWidth/2-(u8g2.getStrWidth(newCoreChar)/2), ( DispHeight - u8g2.getAscent() ) / 2 + u8g2.getAscent(), newCoreChar);  // Write Corename to Display
      u8g2.sendBuffer(); 
      // Set font back
      u8g2.setFont(old_font);
    }  // end ifs
    oldCore=newCore;         // Update Buffer
  } // end newCore!=oldCore
} // End Main Loop

//=================== Functions =========================

// ---- oled_mistertext -- Show the Start-Up Text ----
void oled_mistertext(void) {
#ifdef XDEBUG
  Serial.println("Show Startscreen");
#endif
  u8g2.clearBuffer();
  u8g2.setCursor(DispWidth/2-(u8g2.getStrWidth("MiSTer FPGA")/2), ( DispHeight/2 - u8g2.getAscent() ) / 2 + u8g2.getAscent() );
  u8g2.print("MiSTer FPGA");
  u8g2.setCursor(DispWidth/2-(u8g2.getStrWidth("by Sorgelig")/2), ( DispHeight/2 - u8g2.getAscent() ) / 2 + u8g2.getAscent() + DispHeight/2 );
  u8g2.print("by Sorgelig");

  u8g2.drawXBMP(DispWidth-usb_icon16_width-1, 0, usb_icon16_width, usb_icon16_height, usb_icon16);
  
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
  int x=0,y=0,f=0;
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

  xPos = TextIn.substring(0, 3);
  yPos = TextIn.substring(4, 6);
  FontType = TextIn.substring(7, 8);
  TextOut = TextIn.substring(9, TextIn.length());
#ifdef XDEBUG
  Serial.printf("Received= X: %s Y: %s S: %s T: %s\n", (char*)xPos.c_str(), (char*)yPos.c_str(), (char*)TextSize.c_str(), (char*)TextOut.c_str());
#endif

  x = xPos.toInt();
  y = yPos.toInt();
  f = FontType.toInt();
  
  // Parameter check
  if (x<0 || x>DispWidth-1 || y<0 || y>DispHeight-1 || f<0 || f>20) {
    x=5;
    y=40;
    f=8;
    TextOut="Parameter Error";
  }
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
  u8g2.drawStr(x, y, (char*)TextOut.c_str());
  u8g2.sendBuffer();
  u8g2.setFont(old_font);                          // Set Font back
}

// --- usb2oled_readndrawlogo -- Read and Draw XBM Picture from SD with some effect, Picture must be sized for the display ----
int usb2oled_readndrawlogo(int effect) {
  const int logoBytes = DispWidth * DispHeight / 8; // Make it more universal, here 2048
  int logoByte;
  unsigned char logoByteValue;
  int w,x,y,x2;
  unsigned char *logoBin;

#ifdef XDEBUG
    Serial.println("Called Function CORECHANGE");
#endif
  
  logoBin = (unsigned char *) malloc(logoBytes);    // Reserve Memory for Picture-Data
  Serial.readBytes(logoBin, logoBytes);             // Read 2048 Bytes from Serial
  
  // Draw the Picture
  // -------------------- Effects -----------------------
  switch (effect) {
    case 1:                                      // Particle Effect
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
	  break;  // end case 1
  
    case 2:                                        // Left to Right
      for (x=0; x<DispLineBytes; x++) {
        for (y=0; y<DispHeight; y++) {
          logoByteValue = logoBin[x+y*DispLineBytes];
          drawEightBit(x*8, y, logoByteValue);
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;  // end case 2
    
    case 3:                                       // Top to Bottom
      for (y=0; y<DispHeight; y++) {
        for (x=0; x<DispLineBytes; x++) {
          logoByteValue = logoBin[x+y*DispLineBytes];
          drawEightBit(x*8, y, logoByteValue);
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;  // end case 3

    case 4:                                        // Right to Left
      for (x=DispLineBytes-1; x>=0; x--) {
        for (y=0; y<DispHeight; y++) {
          logoByteValue = logoBin[x+y*DispLineBytes];
          drawEightBit(x*8, y, logoByteValue);
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;  // end case 4

    case 5:                                       // Bottom to Top
      for (y=DispHeight-1; y>=0; y--) {
        for (x=0; x<DispLineBytes; x++) {
          logoByteValue = logoBin[x+y*DispLineBytes];
          drawEightBit(x*8, y, logoByteValue);
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;  // end case 5

    case 6:                                        // Left to Right Diagonally
      for (x=0; x<DispWidth+DispHeight; x++) {
        for (y=0; y<DispHeight; y++) {
          // x2 calculation = Angle
          //x2=x-y;                                // Long Diagonal
          //x2=x-y/2;                              // Middle Diagonal
          x2=x-y/4;                                // Short Diagonal
          if ((x2>=0) && (x2<DispLineBytes)) {
            logoByteValue = logoBin[x2+y*DispLineBytes];
            drawEightBit(x2*8, y, logoByteValue);
          }  // end for x2
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;  // end case 6
    
    case 7:                                     // Even Line Left to Right / Odd Line Right to Left
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
      break;  // end case 7
    
    case 8:                                     // Top Part Left to Right / Bottom Part Right to Left
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
      break;  // end case 8
    
    case 9:                                     // Four Parts Left to Right to Left to Right...
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
      break;  // end case 9

    case 10:                                     // 4 Parts, Top-Left => Bottom-Right => Top-Right => Bottom-Left
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
      break;  // end case 10
      
      case 11:                                     // Circles(Snake)
      for (w=0; w<4; w++) {
        // To the right
        for (x=0+w; x<DispLineBytes-w; x++) {
          for (y=0; y<8; y++) {
            logoByteValue = logoBin[x+(y+w*8)*DispLineBytes];
            drawEightBit(x*8, y+w*8, logoByteValue);
          }
        u8g2.sendBuffer();
        } 
        // Down
        x=DispLineBytes-1-w;
        for (y=(1+w)*8; y<DispHeight-(1+w)*8; y++) {
          logoByteValue = logoBin[x+y*DispLineBytes];
          drawEightBit(x*8, y, logoByteValue);
          if ((y+1) % 4 == 0) u8g2.sendBuffer();              // Modulo Factor = Speed, 2(slow), 4(middle), 8(fast), none(Hyperspeed)
        }
        // To the left
        for (x=DispLineBytes-1-w; x>=0+w; x--) {
          for (y=0; y<8; y++) {
            logoByteValue = logoBin[x+(DispHeight-1-y-w*8)*DispLineBytes];
            drawEightBit(x*8, DispHeight-1-y-w*8, logoByteValue);
          }
        u8g2.sendBuffer();
        } 
        // Up
        x=0+w;
        for (y=DispHeight-1-(1+w)*8; y>=0+(1+w)*8; y--) {
          logoByteValue = logoBin[x+y*DispLineBytes];
          drawEightBit(x*8, y, logoByteValue);
          if (y % 4 == 0) u8g2.sendBuffer();                  // Modulo Factor = Speed, 2(slow), 4(middle), 8(fast), none(Hyperspeed)
        }
      }  // end for w
      break;  // end case 11

    default:                                     // Just overwrite the whole screen
	    //u8g2.clearBuffer();
      u8g2.drawXBM(0, 0, DispWidth, DispHeight, logoBin);
      u8g2.sendBuffer();
      break;  // endcase default
  }  // end switch effect
  free(logoBin);
  return 1;                      // Everything OK
}  // end sd2oled_readndrawlogo

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
#if defined(ARDUINO_ESP8266_NODEMCU) || defined(ARDUINO_ESP8266_NODEMCU_ESP12E)
  yield();
#endif
}

//========================== The end ================================
