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
  -"MENU" Picture moved to the SD, makes it easier to change (if you like).
  -Modified Text Position calculation (see U8G2 Documentation for Details). 
   Change text reference position from TOP to BASELINE.
   Using now "getAscent" instead of "getMaxCharHeight" for Text Y-Position calculation (see U8G2 Documentation).
  -Show a small "SD-Icon" on Startup Screen if SD Card was detected instead of the "dot".
  
*/


//#define XDEBUG          // Uncomment to get some Debugging Infos over Serial especially for SD Debugging

#include <Arduino.h>
#include <U8g2lib.h>    // Display Library
#include "logo.h"       // The Pics in XMB Format

// SD Stuff 
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// TTGO-T8 v1.7 Pins for SD Port SPI Instance
SPIClass SDSPI(HSPI);
#define SDSPI_SCLK 14
#define SDSPI_MISO 2
#define SDSPI_MOSI 15
#define SDSPI_SS   13
//#define SDSPI_SPEED 27000000

// ------------ Objects -----------------
// Display Constructor HW-SPI ESP32-Board (TTGO T8 OLED & SD Card) 180° Rotation => U8G2_R2
U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 26, /* dc=*/ 25, /* reset=*/ 27);  // Enable U8G2_16BIT in u8g2.h <= !! W I C H T I G !!

// Display Constructor HW-SPI ESP32-Board (Lolin32) 180° Rotation => U8G2_R2
// U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 12, /* dc=*/ 13, /* reset=*/ 14);  // Enable U8G2_16BIT in u8g2.h <= !! W I C H T I G !!

// Display Constructor HW-SPI Lolin32 0° Rotation => U8G2_R0
//U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 12, /* dc=*/ 13, /* reset=*/ 14);  // Enable U8G2_16BIT in u8g2.h <= !! W I C H T I G !!

// Display Constructor HW-SPI Mighty Core ATMega 1284 0° Rotation => U8G2_R0
//U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 17, /* dc=*/ 16, /* reset=*/ 18);  // Enable U8G2_16BIT in u8g2.h <= !! W I C H T I G !!

// ------------ Variables ----------------

// Strings
String newCore = "";             // Received Text, from MiSTer without "\n\r" currently (2021-01-11)
String oldCore = "";             // Buffer String for Text change detection
//char newCoreChar[30]="";        // Array of char needed for some functions, see below "newCore.toCharArray"
char *newCoreChar;

// Display Vars
u8g2_uint_t DispWidth, DispHeight;

// SD Vars
uint8_t sdCardType;
bool sdCardOK = false;
bool sdPicShown = false;

// ================ SETUP ==================
void setup(void) {
  // Init Serial
  //Serial.begin(9600);              // 9600 for MiSTer ttyUSBx Device FTDI Chip or manual set Speed
  Serial.begin(57600);           // 57600 Common Modem Speed :-)
  //Serial.begin(115200);          // 115200 for MiSTer ttyUSBx Device CP2102 Chip on ESP32
  
  //Init Random Generator with empty Analog Port value
  randomSeed(analogRead(34));

  // Init Display
  u8g2.begin();
  u8g2.setDrawColor(1);                      // Set font color to White (default value)
  u8g2.setFontDirection(0);                  // Set drawing direction of all strings or glyphs (default value)
  u8g2.setFontPosBaseline();                 // Set font Reference Position to BASELINE (default value)
  u8g2.setFontRefHeightText();               // Set the calculation method for the ascent and descent of the current font (default value)
  u8g2.setFont(u8g2_font_tenfatguys_tr);     // 10 Pixel Font

  // Get Display Dimensions
  DispWidth = u8g2.getDisplayWidth();
  DispHeight = u8g2.getDisplayHeight();

  // SD Init TTGO-T8 v1.7 specific
  // Create SPI Instance for SD Card
  SDSPI.begin(SDSPI_SCLK, SDSPI_MISO, SDSPI_MOSI, SDSPI_SS);     // Pins: SCLK, MISO, MOSI, SS(CS)

  if(SD.begin(SDSPI_SS, SDSPI)){                                 // CS(SS) Pin, SPI Instance
    sdCardOK = true;
  }
  else {
    sdCardOK = false;
  }

// **** Debugging Code ****
#ifdef XDEBUG
  if (sdCardOK) {
    Serial.println("Card Mount OK");
  }
  else {
    Serial.println("Card Mount Failed");
  }
  
  sdCardType = SD.cardType();

  if(sdCardType == CARD_NONE){
    Serial.println("No SD card attached");
  }
  
  Serial.print("SD Card Type: ");
  if(sdCardType == CARD_MMC){
    Serial.println("MMC");
  } 
  else if(sdCardType == CARD_SD){
     Serial.println("SDSC");
  } 
  else if(sdCardType == CARD_SDHC){
    Serial.println("SDHC");
  } 
  else {
    Serial.println("UNKNOWN");
  }

  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
#endif
// **** Debbuging Code ****

  // OLED Startup with Some Text
  oled_mistertext();
}

// ================ MAIN LOOP ===================
void loop(void) {

  if (Serial.available()) {
    newCore = Serial.readStringUntil('\n');                  // Read string from serial until NewLine "\n" (from MiSTer's echo command) is detected or timeout (1000ms) happens.
  }  // end serial available
    
  if (newCore!=oldCore) {                                    // Proceed only if Core Name changed
    // Many if an elses as switch/case is not working (maybe later with an array).

    // -- First Transmission --
    if (newCore.endsWith("QWERTZ")) {                        // TESTING: Process first Transmission after PowerOn/Reboot.
        // Do nothing, just receive one string to clear the buffer.
    }                    

    // -- Menu Core(s) => now part of the Logos --
    //else if (newCore=="MENU")         oled_misterlogo(random(0,7));  // MiSTer Logo Effects 0-3
    
    // Show Menu Effects manually
    else if (newCore=="MENU0")        sd2oled_readndrawlogo("MENU",0);  // Show MiSTer MENU Logo with no Effect
    else if (newCore=="MENU1")        sd2oled_readndrawlogo("MENU",1);  // Show MiSTer MENU Logo with Particle Effect
    else if (newCore=="MENU2")        sd2oled_readndrawlogo("MENU",2);  // Show MiSTer MENU Logo drawn from left to right
    else if (newCore=="MENU3")        sd2oled_readndrawlogo("MENU",3);  // Show MiSTer MENU Logo drawn from top to bottom
    else if (newCore=="MENU4")        sd2oled_readndrawlogo("MENU",4);  // Show MiSTer MENU Logo drawn from left to right but diagonally
    else if (newCore=="MENU5")        sd2oled_readndrawlogo("MENU",5);  // Show MiSTer MENU Logo drawn even line left to right / odd line right to left
    else if (newCore=="MENU6")        sd2oled_readndrawlogo("MENU",6);  // Show MiSTer MENU Logo drawn top part left to right / bottom part right to left 
    else if (newCore=="MENU7")        sd2oled_readndrawlogo("MENU",7);  // Show MiSTer MENU Logo drawn four parts left to right to left to right
    
    // -- Test Commands --
    else if (newCore=="cls")          u8g2.clear();
    else if (newCore=="sorg")         oled_mistertext();
    else if (newCore=="bye")          oled_drawlogo64h(sorgelig_icon64_width, sorgelig_icon64);
    
    // -- Unidentified Core Name , search SD  --
    else {
      if (sdCardOK) {
        sdPicShown = sd2oled_readndrawlogo(newCore, random(1,8));
      } // end if sdCardOK
      
      if (!sdCardOK || !sdPicShown) {
        // Get Font
        const uint8_t *old_font = u8g2.getU8g2()->font;
        newCoreChar = (char*)newCore.c_str();
        u8g2.clearBuffer();
        // Set font
        //u8g2.setFont(u8g2_font_bubble_tr);                        // 18 Pixel Font
        u8g2.setFont(u8g2_font_maniac_tr);                          // 23 Pixel Font
        u8g2.drawStr(DispWidth/2-(u8g2.getStrWidth(newCoreChar)/2), ( DispHeight - u8g2.getAscent() ) / 2 + u8g2.getAscent(), newCoreChar);  // Write Corename to Display
        //u8g2.setCursor(0,20);
        //u8g2.print(u8g2.getMaxCharHeight());
        u8g2.sendBuffer(); 
        // Set font back
        u8g2.setFont(old_font);
      } // end if !sdCardOK
    }  // end ifs
    oldCore=newCore;         // Update Buffer
  } // end newCore!=oldCore
} // End Main Loop

//=================== Functions =========================

// ---- oled_mistertext -- Show the Start-Up Text ----
void oled_mistertext(void) {
  u8g2.clearBuffer();
  u8g2.setCursor(DispWidth/2-(u8g2.getStrWidth("MiSTer FPGA")/2), ( DispHeight/2 - u8g2.getAscent() ) / 2 + u8g2.getAscent() );
  u8g2.print("MiSTer FPGA");
  u8g2.setCursor(DispWidth/2-(u8g2.getStrWidth("by Sorgelig")/2), ( DispHeight/2 - u8g2.getAscent() ) / 2 + u8g2.getAscent() + DispHeight/2 );
  u8g2.print("by Sorgelig");

  // If SD Card detected print "."
  if (sdCardOK) {
    u8g2.drawXBMP(239,0, sdcard_icon16_width, sdcard_icon16_height, sdcard_icon16);
  }
  u8g2.sendBuffer();
}


// ---- oled_drawlogo64h -- Draw Pictures with an height of 64 Pixel centerred ----
void oled_drawlogo64h(u8g2_uint_t w, const uint8_t *bitmap) {
  u8g2.clearBuffer();
  u8g2.drawXBMP(DispWidth/2-w/2, 0, w, 64, bitmap);
  u8g2.sendBuffer();
} // end oled_drawlogo64h


// --- sd2oled_readndrawlogo -- Read and Draw XBM Picture from SD with some effect, Picture must be sized for the display ----
int sd2oled_readndrawlogo(String corename, int effect) {
  // Returns 1 if OK or 0 in case of an Error
  // Let's try to Read the Picture first
  //u8g2.clearBuffer();
  String xbmFile = String("/" + corename + ".xbm");  // => "lander.xbm"
  char *filename = (char*)xbmFile.c_str();
  if (!filename) return 0;                     // No Filename given function end here
#ifdef XDEBUG
  Serial.print("Reading ");
  Serial.println(filename);
#endif
  if (!SD.exists(filename)) {                  // No File found on SD function end here
#ifdef XDEBUG
    Serial.println("File not found");
#endif
    return 0;
  }
  File imagefile = SD.open(filename);
  String xbm;
  char next;
  u8g2_uint_t imageWidth=0, imageHeight=0;
  uint8_t imageBits[DispWidth * DispHeight / 8];  // Make it more universal
  uint16_t pos = 0;
  const char CR = 10;
  const char comma = 44;
  const char curly = 125;
  
  while(imagefile.available()) {
    next = imagefile.read();
    if (next == CR) {
      if (xbm.indexOf("#define") == 0) {
        if (xbm.indexOf("_width ")>0) {
          xbm.remove(0,xbm.lastIndexOf(" "));
          imageWidth = xbm.toInt();
          if (imageWidth > DispWidth) {
#ifdef XDEBUG
            Serial.println("Image too large for screen");
#endif
            return 0;
          }  // end if imageWidth
        }  // end if xbm.indexOf("_width ")
        if (xbm.indexOf("_height ")>0) {
          xbm.remove(0,xbm.lastIndexOf(" "));
          imageHeight=xbm.toInt();
          if (imageHeight > DispHeight) {
#ifdef XDEBUG
            Serial.println("Image too large for screen");
#endif
            return 0;
          }  // end if imageHeight
        }  // end if xbm.indexOf("_height "
      }  // xbm.indexOf("#define")
      xbm = "";
    }  // end if (next == CR)
    else if ((next == comma) || (next == curly)) {
      imageBits[pos++] = (uint8_t) strtol(xbm.c_str(), NULL, 16);
      xbm = "";
    }  // end else if (next == CR)
    else {
      xbm += next;
    }  // end else (next == CR)
  }  // end while
  imagefile.close();  // close File
  
  // Now let's draw the Picture
  const int logoBytes = DispWidth * DispHeight / 8; // Make it more universal
  int logoByte;
  unsigned char logoByteValue;
  int a,i,x,y,x2;
  //u8g2.clearBuffer();                          // Over-Write the actual Picture or Text
  switch (effect) {
    case 1:
      for (a=0; a<10000; a++) {
        logoByte = random(logoBytes); // Value 2048 => Get 0..2047
        logoByteValue = imageBits[logoByte];
        x = (logoByte % 32) * 8;
        y = logoByte / 32;
        for (i=0; i <= 7; i++){
          if (bitRead(logoByteValue, i)) {
            // Set Pixel
            u8g2.drawPixel(x+i,y);
          }
          else {
            // Clear Pixel
            u8g2.setDrawColor(0);
            u8g2.drawPixel(x+i,y);
           u8g2.setDrawColor(1);        
          }
        }  // end for
        // For different speed
        // if ((a % (a/10)) == 0) u8g2.sendBuffer();
        if (a<=1000) {
          if ((a % 25)==0) u8g2.sendBuffer();
        }
        if ((a>1000) && (a<=2000)) {
          if ((a % 50)==0) u8g2.sendBuffer();
        }
        if ((a>2000) && (a<=5000)) { 
          if ((a % 200)==0) u8g2.sendBuffer();
        }
        if (a>5000) { 
          if ((a % 400)==0) u8g2.sendBuffer();
        }
      }
      // Finally overwrite the Screen with fill Size Picture
      u8g2.drawXBM(0, 0, imageWidth, imageHeight, imageBits);
      u8g2.sendBuffer();
	  break;  // end case 1
    case 2:                                        // Left to Right
      for (x=0; x<32; x++) {
        for (y=0; y<64; y++) {
          logoByteValue = imageBits[x+y*32];
          for (i=0; i <= 7; i++){
            if (bitRead(logoByteValue, i)) {
              // Set Pixel
              u8g2.drawPixel(x*8+i,y);
            }
            else {
              // Clear Pixel
              u8g2.setDrawColor(0);
              u8g2.drawPixel(x*8+i,y);
              u8g2.setDrawColor(1);        
            }  // end bit read
          }  // end for i
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;  // end case 2
    case 3:                                       // Top to Bottom
      for (y=0; y<64; y++) {
        for (x=0; x<32; x++) {
          logoByteValue = imageBits[x+y*32];
          for (i=0; i <= 7; i++){
            if (bitRead(logoByteValue, i)) {
              // Set Pixel
              u8g2.drawPixel(x*8+i,y);
            }
            else {
              // Clear Pixel
              u8g2.setDrawColor(0);
              u8g2.drawPixel(x*8+i,y);
              u8g2.setDrawColor(1);        
            }  // end bit read
          }  // end for i
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;  // end case 3
    case 4:                                        // Left to Right Diagonally
      for (x=0; x<96; x++) {
        for (y=0; y<64; y++) {
          // x2 calculation = Angle
          x2=x-y;
          //x2=x-y/2;
          //x2=x-y/4;
          if ((x2>=0) && (x2<32)) {
            logoByteValue = imageBits[x2+y*32];
            for (i=0; i <= 7; i++){
              if (bitRead(logoByteValue, i)) {
                // Set Pixel
                u8g2.drawPixel(x2*8+i,y);
              }
              else {
                // Clear Pixel
                u8g2.setDrawColor(0);
                u8g2.drawPixel(x2*8+i,y);
                u8g2.setDrawColor(1);        
              }  // end bit read
            }  // end if x2
          }  // end for i
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;  // end case 4
    case 5:                                     // Even Line Left to Right / Odd Line Right to Left
      for (x=0; x<=31; x++) {
        for (y=0; y<=63; y++) {
          if ((y % 2) == 0) {
            x2 = x;
          }
          else {
            x2 = x*-1 + 31;
          }
          logoByteValue = imageBits[x2+y*32];
          for (i=0; i <= 7; i++){
            if (bitRead(logoByteValue, i)) {
              // Set Pixel
              u8g2.drawPixel(x2*8+i,y);
            }
            else {
              // Clear Pixel
              u8g2.setDrawColor(0);
              u8g2.drawPixel(x2*8+i,y);
              u8g2.setDrawColor(1);        
            }  // end bit read
          }  // end for i
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;  // end case 5
    case 6:                                     // Top Part Left to Right / Bottom Part Right to Left
      for (x=0; x<=31; x++) {
        for (y=0; y<=63; y++) {
          if (y < 32) {
            x2 = x;
          }
          else {
            x2 = x*-1 + 31;
          }
          logoByteValue = imageBits[x2+y*32];
          for (i=0; i <= 7; i++){
            if (bitRead(logoByteValue, i)) {
              // Set Pixel
              u8g2.drawPixel(x2*8+i,y);
            }
            else {
              // Clear Pixel
              u8g2.setDrawColor(0);
              u8g2.drawPixel(x2*8+i,y);
              u8g2.setDrawColor(1);        
            }  // end bit read
          }  // end for i
        }  // end for y
        u8g2.sendBuffer();
      }  // end for x
      break;  // end case 6
    case 7:                                     // Four Parts Left to Right to Left to Right...
      for (a=0; a<=3; a++) {
        for (x=0; x<=31; x++) {
          for (y=0; y<=15; y++) {
            if ((a%2) == 0) {
              x2 = x;
            }
            else {
              x2 = x*-1 + 31;
            }
            logoByteValue = imageBits[x2+(y+a*16)*32];
            for (i=0; i <= 7; i++){
              if (bitRead(logoByteValue, i)) {
                // Set Pixel
                u8g2.drawPixel(x2*8+i,y+a*16);
              }
              else {
                // Clear Pixel
                u8g2.setDrawColor(0);
                u8g2.drawPixel(x2*8+i,y+a*16);
                u8g2.setDrawColor(1);        
              }  // end bit read
            }  // end for i
          }  // end for y
          u8g2.sendBuffer();
        }  // end for x
      }
      break;  // end case 7
    default:                                     // Just overwrite the whole screen
	    //u8g2.clearBuffer();
      u8g2.drawXBM(0, 0, imageWidth, imageHeight, imageBits);
      u8g2.sendBuffer();
      break;  // endcase default
  }  // end switch effect
  
  return 1;                      // Everything OK
}  // end sd2oled_readndrawlogo

//========================== The end ================================
