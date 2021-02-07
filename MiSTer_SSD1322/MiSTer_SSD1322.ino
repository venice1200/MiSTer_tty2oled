/*
  By Venice
  Get CORENAME from MiSTer via Serial TTY Device and show CORENAME related text, Pictures or Logos
  Using the universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Problem: 
  Sometimes after Power-On "MENU" is written instead of the logo shown.
  So i added an "first transmission" without doing anything.
*/

#include <Arduino.h>
#include <U8g2lib.h>    // Display Library
#include "logo.h"       // The Pics in XMB Format

/*
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
*/

// ------------ Objects -----------------
// Display Constructor HW-SPI ESP32-Board (Lolin32) 180° Rotation => U8G2_R2
U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 12, /* dc=*/ 13, /* reset=*/ 14);  // Enable U8G2_16BIT in u8g2.h <= !! W I C H T I G !!

// Display Constructor HW-SPI Lolin32 0° Rotation => U8G2_R0
//U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 12, /* dc=*/ 13, /* reset=*/ 14);  // Enable U8G2_16BIT in u8g2.h <= !! W I C H T I G !!

// Display Constructor HW-SPI Mighty Core ATMega 1284 0° Rotation => U8G2_R0
//U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 17, /* dc=*/ 16, /* reset=*/ 18);  // Enable U8G2_16BIT in u8g2.h <= !! W I C H T I G !!

// ------------ Variables ----------------
// For Particle effect
const int logoBytes = 2048; // 0..2047
int logoByte;
unsigned char logoByteValue;

// Strings
String newCore = "";             // Received Text, from MiSTer without "\n\r" currently (2021-01-11)
String oldCore = "";             // Buffer String for Text change detection
char newCoreArray[30]="";        // Array of char needed for some functions, see below "newCore.toCharArray"

// Display Vars
u8g2_uint_t DispWidth, DispHeight;

// ================ SETUP ==================
void setup(void) {
  // Init Serial
  //Serial.begin(9600);              // 9600 for MiSTer ttyUSBx Device FTDI Chip or manual set Speed
  Serial.begin(57600);           // 57600 Common Modem Speed :-)
  //Serial.begin(115200);          // 115200 for MiSTer ttyUSBx Device CP2102 Chip on ESP32
  
  //Init Random Generator
  randomSeed(analogRead(0));

  // Init Display
  u8g2.begin();
  u8g2.setFont(u8g2_font_tenfatguys_tr);     // 10 Pixel Font
  u8g2.setDrawColor(1);  // White (Standard)
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

  //u8g2.setFontRefHeightExtendedText();
  //u8g2.setFont(u8g2_font_tenthinguys_tf);
  //u8g2.setFont(u8g2_font_6x10_tf);
  //u8g2.setFont(u8g2_font_ncenB08_tr);
  //u8g2.setDrawColor(0); // Black

  // Get Display Dimensions
  DispWidth = u8g2.getDisplayWidth();
  DispHeight = u8g2.getDisplayHeight();
  
  // Startup with Some Text
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
    
    // -- Menu Core --
    else if (newCore=="MENU")         oled_misterlogo();     // TESTING! Sometimes after Power On "MENU" is written instead of the logo shown. Maybe one character in the serial buffer?
    
    // -- Arcade Cores --
    else if (newCore=="llander")      oled_drawlogo64h(arcade_logo_width, arcade_logo);
    else if (newCore=="mooncrgx")     oled_drawlogo64h(arcade_logo_width, arcade_logo);
    else if (newCore=="mpatrol")      oled_drawlogo64h(arcade_logo_width, arcade_logo);
    
    // -- Computer Cores --
    else if (newCore=="AO486")        oled_drawlogo64h(ao486_logo_width, ao486_logo);
    else if (newCore=="APPLE-I")      oled_drawlogo64h(apple_logo_width, apple_logo);
    else if (newCore=="Apple-II")     oled_drawlogo64h(apple2_logo_width, apple2_logo);
    else if (newCore=="AtariST")      oled_drawlogo64h(atarist_logo_width, atarist_logo);
    else if (newCore=="C64")          oled_drawlogo64h(c64_logo_width, c64_logo);
    else if (newCore=="Minimig")      oled_drawlogo64h(amiga_logo_width, amiga_logo);
    else if (newCore=="PET2001")      oled_drawlogo64h(commodore_logo_width, commodore_logo);
    else if (newCore=="VIC20")        oled_drawlogo64h(commodore_logo_width, commodore_logo);
    
    // -- Console Cores --
    else if (newCore=="ATARI2600")    oled_drawlogo64h(atari2600_logo_width, atari2600_logo);
    else if (newCore=="Genesis")      oled_drawlogo64h(sega_logo_width, sega_logo);
    else if (newCore=="NEOGEO")       oled_drawlogo64h(neogeo_logo_width, neogeo_logo);
    else if (newCore=="NES")          oled_drawlogo64h(nintendo_logo_width, nintendo_logo);
    else if (newCore=="SMS")          oled_drawlogo64h(sega_logo_width, sega_logo);
    else if (newCore=="SNES")         oled_drawlogo64h(nintendo_logo_width, nintendo_logo);
    else if (newCore=="TGFX16")       oled_drawlogo64h(pcengine_logo_width, pcengine_logo);

    // -- Other --
    else if (newCore=="MEMTEST")      oled_drawlogo64h(memtest_logo_width, memtest_logo);
    
    // -- Test Commands --
    else if (newCore=="cls")          u8g2.clear();
    else if (newCore=="sorg")         oled_mistertext();
    else if (newCore=="bye")          oled_drawlogo64h(sorgelig_icon64_width, sorgelig_icon64);
    
    // -- Unidentified Core Name --
    else {
      // Get Font
      const uint8_t *old_font = u8g2.getU8g2()->font;
      
      // Set font
      //u8g2.setFont(u8g2_font_bubble_tr);                        // 18 Pixel Font
      u8g2.setFont(u8g2_font_maniac_tr);                          // 23 Pixel Font
      newCore.toCharArray(newCoreArray, newCore.length()+1);      // Convert String to Array of Char, +1 is needed
      u8g2.clearBuffer();
      u8g2.drawStr(DispWidth/2-(u8g2.getStrWidth(newCoreArray)/2), DispHeight/2 - u8g2.getMaxCharHeight()/2, newCoreArray);  // Write Corename to Display
      u8g2.sendBuffer(); 
 
      // Set font back
      u8g2.setFont(old_font);
    }  // end ifs
    oldCore=newCore;  // Update Buffer
  } // end newCore!=oldCore
} // End Main Loop

//=================== Functions =========================

void oled_mistertext(void) {
  //u8g2.setFont(u8g2_font_tenfatguys_tr); // 10 Pixel Font
  u8g2.clearBuffer();
  u8g2.setCursor(DispWidth/2-(u8g2.getStrWidth("MiSTer FPGA")/2), DispHeight/2 - u8g2.getMaxCharHeight()/2 - 10);
  u8g2.print("MiSTer FPGA");
  u8g2.setCursor(DispWidth/2-(u8g2.getStrWidth("by Sorgelig")/2), DispHeight/2 - u8g2.getMaxCharHeight()/2 + 10);
  u8g2.print("by Sorgelig");
  u8g2.sendBuffer();
}

void oled_misterlogo(void) {
  // Particle Effect Random Based
  // A Loop of 10000 is enough for Arduino
  // An ESP32 works much faster
  int a,i,x,y;
  for (a=0; a<20000; a++) {
    logoByte = random(logoBytes); // Value 2048 => Get 0..2047
    logoByteValue = mister_logo[logoByte];
    x = (logoByte % 32) * 8;
    y = logoByte / 32;
    if (logoByteValue > 0) {
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
      }
    }
    // For different speed 
    if (a<=1000) {
      if ((a % 25)==0) u8g2.sendBuffer();
    }
    if ((a>1000) && (a<=5000)) {
      if ((a % 100)==0) u8g2.sendBuffer();
    }
    if ((a>5000) && (a<=10000)) { 
      if ((a % 200)==0) u8g2.sendBuffer();
    }
    if (a>10000) { 
      if ((a % 400)==0) u8g2.sendBuffer();
    }

  }
  // Finally overwrite the Screen with fill Size Picture
  u8g2.drawXBM(0, 0, mister_logo_width, mister_logo_height, mister_logo);
  u8g2.sendBuffer();
} // end oled_misterlogo

// Draw Pictures with an height of 64 Pixel centerred
void oled_drawlogo64h(u8g2_uint_t w, const uint8_t *bitmap) {
  u8g2.clearBuffer();
  u8g2.drawXBMP(DispWidth/2-w/2, 0, w, 64, bitmap);
  u8g2.sendBuffer();
} // end oled_drawlogo64h

//========================== The end ================================
