/*
  By Venice
  Get CORENAME from MiSTer via Serial TTY Device and show CORENAME related text, Pictures or Logos
  Using Forked Adafruit SSD1327 Library https://github.com/adafruit/Adafruit_SSD1327for the SSD1322
 
  2021-09-19 (based on MiSTer_SSD1322_USB_Teseting)
  -First Grayscale Test
  2021-09-25
  -Adding Effects 1-7

  ToDo
  -Text & Geo Commands
  -Particle Effect
  -Four-Part Effect
  -Everything I forgot
   
*/

#include <Arduino.h>
#include <SSD1322_for_Adafruit_GFX.h>             // Display Library
#include "logo.h"                                 // The Pics in XMB Format
#include <Fonts/Picopixel.h>
//#include <Fonts/Org_01.h>

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
#define BuildVersion "210925GT"    // "T" for Testing, "G" for Grayscale

// Uncomment to get the tty2oled Logo shown on Startscreen instead of text
#define XLOGO

// Uncomment for 180° StartUp Rotation (Display Connector up)
//#define XROTATE

// Uncomment for "Send Acknowledge" from tty2oled to MiSTer, need Daemon with "waitfortty"
#define XSENDACK

// Uncomment for Tilt-Sensor based Display-Auto-Rotation. The Sensor is connected to Pin 32 and GND.
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
// ------------------------------------------------ Manual-Board-Config ------------------------------------------------
// ------------------------------------ Make sure the Auto-Board-Config is not active ----------------------------------
// ---------------------------------------------------------------------------------------------------------------------

#define OLED_CS 26
#define OLED_DC 25
#define OLED_RESET 27

// hardware SPI
Adafruit_SSD1322 oled(256, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS);

// -------------------------------------------------------------
// ------------------------- Variables -------------------------
// -------------------------------------------------------------

// Strings
String newCommand = "";             // Received Text, from MiSTer without "\n" currently (2021-01-11)
String prevCommand = "";
String actCorename = "No Core";     // Actual Received Corename
uint8_t contrast = 5;               // Contrast (brightness) of display, range: 0 (no contrast) to 255 (maximum)
char *newCommandChar;
bool updateDisplay = false;

// Display Vars
uint16_t DispWidth, DispHeight, DispLineBytes1bpp, DispLineBytes4bpp;
unsigned int logoBytes1bpp=0;
unsigned int logoBytes4bpp=0;
const int cDelay=25;                // Command Delay in ms for Handshake
size_t bytesReadCount=0;
//unsigned char *logoBin;           // <<== For malloc in Setup
uint8_t *logoBin;                   // <<== For malloc in Setup
enum picType {NONE, XBM, GSC};
int actPicType=0;
//uint8_t logoBin[8192];

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

#ifdef XTILT
  RotationDebouncer.attach(RotationPin,INPUT_PULLUP);     // Attach the debouncer to a pin with INPUT mode
  RotationDebouncer.interval(DebounceTime);               // Use a debounce interval of 25 milliseconds
#endif

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
  oled.setTextSize(1);
  oled.setTextColor(SSD1322_WHITE, SSD1322_BLACK);  // White foreground, black background
  oled.setContrast(contrast);                       // Set contrast of display
  oled.setRotation(0);

  // Get Display Dimensions
  DispWidth = oled.width();
  DispHeight = oled.height();
  DispLineBytes1bpp = DispWidth / 8;                       // How many Bytes each Dipslay Line at 1bpp
  DispLineBytes4bpp = DispWidth / 2;                       // How many Bytes each Dipslay Line at 4bpp
  logoBytes1bpp = DispWidth * DispHeight / 8;              // 2048
  logoBytes4bpp = DispWidth * DispHeight / 2;              // 8192
  logoBin = (uint8_t *) malloc(logoBytes4bpp);             // Create Picture Buffer, better than create (malloc) & destroy (free)

#ifdef XTILT
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
      usb2oled_drawlogo(random(1,11));
    }

    else if (newCommand.startsWith("CMDAPD,")) {                            // Command from Serial to receive Picture Data via USB Serial from the MiSTer
      usb2oled_readlogo();                                                  // ESP32 Receive Picture Data... 
    }

    else if (newCommand.startsWith("CMDCOR,")) {                            // Command from Serial to receive Picture Data via USB Serial from the MiSTer
      if (usb2oled_readlogo()==1) {                                         // ESP32 Receive Picture Data... 
        //usb2oled_drawlogo(5);                                               // ..and show them on the OLED with Transition Effect 1..10
        usb2oled_drawlogo(random(1,8));                                     // ..and show them on the OLED with Transition Effect 1..10
        //usb2oled_drawlogo(random(1,11));                                    // ..and show them on the OLED with Transition Effect 1..10
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
      newCommandChar = (char*)newCommand.c_str();
      oled.clearDisplay();
      oled.setTextSize(2);
      oled.setCursor(0,30);
      oled.print(newCommandChar);
      oled.display(); 
      // Set font back
      oled.setTextSize(1);
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

// ---- oled_mistertext -- Show the Start-Up Text ----
void oled_mistertext(void) {
  uint8_t color = 0;
#ifdef XDEBUG
  Serial.println("Show Startscreen");
#endif
  oled.clearDisplay();
  oled.drawXBitmap(82, 0, tty2oled_logo, tty2oled_logo_width, tty2oled_logo_height, SSD1322_WHITE);
  oled.setTextSize(1);
  oled.setFont(&Picopixel);
  //oled.setFont(&Org_01);
  oled.setCursor(0,5);
  //oled.setCursor(0,0);
  oled.print(BuildVersion);
  oled.display();
  delay(2000);
   for (int i=0; i<DispWidth; i+=16) {
    oled.fillRect(i,55,16,8,color);
    color++;
    oled.display();
    delay(100);
  }
  oled.setFont();
} // end mistertext


// ---- oled_drawlogo64h -- Draw Pictures with an height of 64 Pixel centerred ----
void oled_drawlogo64h(uint16_t w, const uint8_t *bitmap) {
  oled.clearDisplay();
  oled.drawXBitmap(DispWidth/2-w/2, 0, bitmap, w, DispHeight, SSD1322_WHITE);
  oled.display();
} // end oled_drawlogo64h

// ----------------- Just show the Corname ------------------------
void usb2oled_showcorename() {
#ifdef XDEBUG
  Serial.println("Called Command CMDSNAM");
#endif

  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setCursor(0,30);
  oled.print(actCorename);
  oled.display();
  oled.setTextSize(1);
}

// ----------------- Command Read an Set Contrast ------------------------------
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

// -----------------------Command Read Logo ----------------------------
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

// -----------------------Command Draw Logo ----------------------------
void usb2oled_drawlogo(int e) {
  int w,x,y,x2;
  unsigned char logoByteValue;
  int logoByte;
  int DispLineBytes;

#ifdef XDEBUG
  Serial.println("Called Command CMDLOGO");
#endif

  // PreLoad the Loop Value "DispLineBytes" based on Pic Type (XBM/GSC)
  if (actPicType == XBM) DispLineBytes = DispLineBytes1bpp;
  if (actPicType == GSC) DispLineBytes = DispLineBytes4bpp;

  switch (e) {
    case 1:                                  // Left to Right
      for (x=0; x<DispLineBytes; x++) {
        for (y=0; y<DispHeight; y++) {
          drawOLEDByte(x, y, logoBin[x+y*DispLineBytes], actPicType);
        }
        if (actPicType == XBM) {
          oled.display();
        }
        if (actPicType == GSC) {
          if ((x+1)%4==0) oled.display();
        }
      }
    break;

    case 2:                                  // Top to Bottom
      for (y=0; y<DispHeight; y++) {
        for (x=0; x<DispLineBytes; x++) {
          drawOLEDByte(x, y, logoBin[x+y*DispLineBytes], actPicType);
        }
        oled.display();
      }
    break;

    case 3:                                  // Right to left
      for (x=DispLineBytes-1; x>=0; x--) {
        for (y=0; y<DispHeight; y++) {
          drawOLEDByte(x, y, logoBin[x+y*DispLineBytes], actPicType);
        }
        if (actPicType == XBM) {
          oled.display();
        }
        if (actPicType == GSC) {
          if (x%4==0) oled.display();
        }
      }
    break;

    case 4:                                  // Bottom to Top
      for (y=DispHeight-1; y>=0; y--) {
        for (x=0; x<DispLineBytes; x++) {
          drawOLEDByte(x, y, logoBin[x+y*DispLineBytes], actPicType);
        }
        oled.display();
      }
    break;

    case 5:                                  // Even Line Left to Right / Odd Line Right to Left
      for (x=0; x<DispLineBytes; x++) {
        for (y=0; y<DispHeight; y++) {
          if ((y % 2) == 0) {
            x2 = x;
          }
          else {
            x2 = x*-1 + DispLineBytes -1;
          }
          drawOLEDByte(x2, y, logoBin[x2+y*DispLineBytes], actPicType);
        }  // end for y
        if (actPicType == XBM) {
          oled.display();
        }
        if (actPicType == GSC) {
          if ((x+1)%4==0) oled.display();
        }
      }  // end for x
    break;

    case 6:                                     // Top Part Left to Right / Bottom Part Right to Left
      for (x=0; x<DispLineBytes; x++) {
        for (y=0; y<DispHeight; y++) {
          //if (y < DispLineBytes) {
          if (y < DispHeight/2) {
            x2 = x;
          }
          else {
            x2 = x*-1 + DispLineBytes -1;
          }
          drawOLEDByte(x2, y, logoBin[x2+y*DispLineBytes], actPicType);
        }  // end for y
        if (actPicType == XBM) {
          oled.display();
        }
        if (actPicType == GSC) {
          if ((x+1)%4==0) oled.display();
        }
      }  // end for x
    break;  // End 6

    case 7:                                     // Four Parts Left to Right to Left to Right...
      for (w=0; w<4; w++) {
        for (x=0; x<DispLineBytes; x++) {
          for (y=0; y<DispHeight/4; y++) {
            if (w%2==0) {
              x2 = x;
            }
            else {
              x2 = x*-1 + DispLineBytes -1;
            }
            drawOLEDByte(x2, y+w*16, logoBin[x2+(y+w*16)*DispLineBytes], actPicType);
          }  // end for y
          if (actPicType == XBM) {
            oled.display();
          }
          if (actPicType == GSC) {
            if ((x+1)%4==0) oled.display();
          }
        }  // end for x
      }
    break;  // End 7


    default:
      // XBM
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
      // 4bpp Picture
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

// -------- Draw one Data Byte to Display Buffer ------------
// x,y: Display Coordinates on the Display
// b:   Data Byte
// t:   Picture Type (GSC/XBM) 
// Depending on the Picture Type 2 or 8 Pixels are written
// ---------------------------------------------------------- 
void drawOLEDByte(int x, int y, unsigned char b, int t) {
  switch (t) {
    case XBM:
      for (int i=0; i<8; i++){
        if (bitRead(b, i)) {
          // Set Pixel
          oled.drawPixel(x*8+i,y,SSD1322_WHITE);
        }
        else {
          // Clear Pixel
          oled.drawPixel(x*8+i,y,SSD1322_BLACK);
        }
      }
    break;
    case GSC:
      oled.drawPixel(x*2, y, (0xF0 & b) >> 4);  // Draw Left Pixel with Grey-Value
      oled.drawPixel(x*2+1, y, 0x0F & b);       // Draw Right Pixel with Grey-Value
    break;
  }
#ifdef USE_NODEMCU
  yield();
#endif
}

// ---------------- Command Enable OTA ---------------------- 

#ifdef ESP32  // OTA and Reset only for ESP32
void enableOTA (void) {
  Serial.println("Connecting to Wireless..");
  oled.setTextSize(1);
  oled.clearDisplay();
  oled.setCursor(10,0);
  oled.print("Connecting to Wireless..");
  oled.display();
  WiFi.mode(WIFI_STA);
  WiFi.begin(MySSID, MyPWD);
//  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection failed! Reboot...");;
    oled.clearDisplay();
    oled.setCursor(10,0);
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
    oled.setCursor(10,40);
    oled.printf("Progress: %u%%", (progress / (total / 100)));
    oled.display();
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
  oled.setCursor(10,0);
  oled.print("OTA Active!");
  oled.setCursor(10,20);
  oled.print("IP address: ");
  oled.print(WiFi.localIP());
  oled.display();
  
  OTAEN = true;  // Set OTA Enabled to True for the "Handler" in "loop"
}
#endif

//========================== The end ================================
