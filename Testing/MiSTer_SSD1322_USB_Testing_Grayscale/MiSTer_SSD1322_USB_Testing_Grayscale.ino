/*
  By Venice
  Get CORENAME from MiSTer via Serial TTY Device and show CORENAME related text, Pictures or Logos
  Using Forked Adafruit SSD1327 Library https://github.com/adafruit/Adafruit_SSD1327for the SSD1322
 
  2021-09-19 
  - First Grayscale Test
   
*/

#include <Arduino.h>
#include <SSD1322_for_Adafruit_GFX.h>             // Display Library
#include "logo.h"                                 // The Pics in XMB Format
#include <Fonts/Picopixel.h>

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
// ---------------------------------------------------------------------------------------------------------------------

// Uncomment (remove "//" in front ofd the Option) to get some Debugging Infos over Serial especially for SD Debugging
//#define XDEBUG

// Version
#define BuildVersion "210918GT"    // "T" for Testing, "G" for Grayscale

// Uncomment to get the tty2oled Logo shown on Startscreen instead of text
#define XLOGO

// Uncomment for 180° StartUp Rotation (Display Connector up)
//#define XROTATE

// Uncomment for "Send Acknowledge" from tty2oled to MiSTer, need Daemon from Testing
#define XSENDACK

// Uncomment for Tilt-Sensor based Display-Auto-Rotation. The Sensor is connected to Pin 15 (SPI_0_CS) and GND.
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
uint16_t DispWidth, DispHeight, DispLineBytes;
unsigned int logoBytes1bpp=0;
unsigned int logoBytes4bpp=0;
const int cDelay=25;                // Command Delay in ms for Handshake
size_t bytesReadCount=0;
//unsigned char *logoBin;           // <<== For malloc in Setup
uint8_t *logoBin;                   // <<== For malloc in Setup
//uint8_t logoBin[8192];

#ifdef XTILT
// Input/Output
#define RotationPin 15
#define DebounceTime 23
Bounce RotationDebouncer = Bounce();     // Instantiate a Bounce object
#endif

// =============================================================================================================
// ================================================ SETUP ======================================================
// =============================================================================================================
void setup(void) {
  // Init Serial
  Serial.begin(115200);                      // 115200 for MiSTer ttyUSBx Device CP2102 Chip on ESP32
  Serial.flush();                            // Wait for empty Send Buffer 

  randomSeed(analogRead(34));                // Init Random Generator with empty Port Analog value

#ifdef XTILT
  // Buttons
  RotationDebouncer.attach(RotationPin,INPUT);     // Attach the debouncer to a pin with INPUT mode
  RotationDebouncer.interval(25);                 // Use a debounce interval of 25 milliseconds
#endif

#ifdef XMIC184
  Wire.begin(xmic_SDA, xmic_SCL, 100000);
  Serial.print("Temperature = ");
  Serial.print(tSensor.getTemp());
  Serial.print("°C");
#endif

  // Init Display
  oled.begin();
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1322_WHITE);
  oled.setContrast(contrast);                // Set contrast of display

  // Get Display Dimensions
  DispWidth = oled.width();
  DispHeight = oled.height();
  DispLineBytes = DispWidth / 8;                       // How many Bytes each Dipslay Line (SSD1322: 256Pixel/8Bit = 32Bytes each Line)

  // Create Picture Buffer, better than create (malloc) & destroy (free)
  logoBytes1bpp = DispWidth * DispHeight / 8;              // 2048
  logoBytes4bpp = DispWidth * DispHeight / 2;              // 8192
  logoBin = (uint8_t *) malloc(logoBytes4bpp);             // Reserve Memory for Picture-Data

  oled_mistertext();                                       // OLED Startup with Some Text
}

// =============================================================================================================
// =============================================== MAIN LOOP ===================================================
// =============================================================================================================
void loop(void) {

#ifdef ESP32  // OTA and Reset only for ESP32
  if (OTAEN) ArduinoOTA.handle();                            // OTA active?
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

/*
    else if (newCommand=="CMDSNAM") {                                       // Show actual loaded Corename
      usb2oled_showcorename();
    }
*/

    else if (newCommand=="CMDSPIC") {                                       // Show actual loaded Picture with Transition
      usb2oled_drawlogo(random(1,11));
    }

    else if (newCommand.startsWith("CMDAPD,")) {                            // Command from Serial to receive Picture Data via USB Serial from the MiSTer
      usb2oled_readlogo();                                               // ESP32 Receive Picture Data... 
    }

    else if (newCommand.startsWith("CMDCOR,")) {                            // Command from Serial to receive Picture Data via USB Serial from the MiSTer
      if (usb2oled_readlogo()==1) {                                      // ESP32 Receive Picture Data... 
        usb2oled_drawlogo(random(1,11));                                 // ..and show them on the OLED with Transition Effect 1..10
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
  oled.setTextSize(1);
  //oled.setFont(&Picopixel);
  oled.drawXBitmap(82, 0, tty2oled_logo, tty2oled_logo_width, tty2oled_logo_height, SSD1322_WHITE);
  oled.setCursor(0,0);
  oled.print(BuildVersion);
  oled.setCursor(101,55);
  oled.print("Grayscale");
  oled.display();
  delay(2000);
   for (int i=0; i<DispWidth; i+=16) {
    oled.fillRect(i,55,16,8,color);
    color++;
    oled.display();
    delay(100);
  }
  //oled.setFont();
} // end mistertext


// ---- oled_drawlogo64h -- Draw Pictures with an height of 64 Pixel centerred ----
void oled_drawlogo64h(uint16_t w, const uint8_t *bitmap) {
  oled.clearDisplay();
  oled.drawXBitmap(DispWidth/2-w/2, 0, bitmap, w, DispHeight, SSD1322_WHITE);
  oled.display();
} // end oled_drawlogo64h

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

#ifdef USE_NODEMCU
  yield();
#endif

  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.print(bytesReadCount);
  oled.display();
  delay(1000);

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
void usb2oled_drawlogo(int effect) {
  int w,x,y,x2;
  unsigned char logoByteValue;
  int logoByte;

#ifdef XDEBUG
  Serial.println("Called Command CMDLOGO");
#endif

  if (bytesReadCount == 2048) {
    oled.clearDisplay();
    oled.setCursor(0,0);
    oled.print("drawXBitmap");
    oled.display();
    delay(1000);
    
    oled.clearDisplay();
    oled.drawXBitmap(0, 0, logoBin, DispWidth, DispHeight, SSD1322_WHITE);
    oled.display();
  }
  if (bytesReadCount == 8192) {
    oled.clearDisplay();
    oled.setCursor(0,0);
    oled.print("draw4bppBitmap");
    oled.display();
    delay(1000);

    oled.clearDisplay();
    oled.draw4bppBitmap(logoBin);
    oled.display();
  }
}  // end sd2oled_drawlogo


// ---------------- Command Enable OTA ---------------------- 

#ifdef ESP32  // OTA and Reset only for ESP32
void enableOTA (void) {
  Serial.println("Connecting to Wireless..");
  oled.setTextSize(1);
  oled.clearDisplay();
  oled.setCursor(10,30);
  oled.print("Connecting to Wireless..");
  oled.display();
  WiFi.mode(WIFI_STA);
  WiFi.begin(MySSID, MyPWD);
//  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection failed! Reboot...");;
    oled.clearDisplay();
    oled.setCursor(10,30);
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
    oled.setCursor(10,60);
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
  oled.setCursor(10,20);
  oled.print("OTA Active!");
  oled.setCursor(10,40);
  oled.print("IP address: ");
  oled.print(WiFi.localIP());
  oled.display();
  
  OTAEN = true;  // Set OTA Enabled to True for the "Handler" in "loop"
}
#endif

//========================== The end ================================
