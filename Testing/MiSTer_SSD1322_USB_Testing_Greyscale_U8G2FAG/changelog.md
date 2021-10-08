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
  -Change Command processing from "if (newCommand!=oldCore)" to "if (updateDisplay)" to prevent a blank screen if multiple data packs are sent.
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
  -New & smaller USB Icon.

  2021-08-10
  -Reactivate System "#define USE_xxxx" set by Arduino's Board selection (Auto Mode) 
   or by uncommenting only the needed "#define USEe_XXX" for your Hardware
  -Add #define "XROTATE", uncomment it for an 180° Rotated Display.

  2021-08-15/16
  -Separate the "Read and Draw Picture" function into two functions
  -New Command "CMDSPIC" which just show the actual loaded Picture (SHOWPIC).
  -New String Variable "actCorename" containing the actual Corename
  -New Command "CMDSNAM" which just show the actual Corename as text (SHOWNAME).
  -New Command "CMDAPD,[corename]" which is just for sending Picture Data, nothing more (Attention Picture Data).
  -!Remove old Command Mode (att, CORECHANGE, CONTRAST, TEXTOUTXY, GEOOUTXY) so you need to use an actual Daemon

  2021-08-23..26
  -Moved Startscreen-Text into "defines"
  -Add "#define XSENDACK"
   Uncomment this Option to enable the Handshake with the MiSTer Daemon. !! You need the Daemon from testing !!
  -Add "#define XLOGO"
   Uncomment this Option to get the tty2oled Logo shown on Startscreen instead of the Starttext
  -Change Variable Name "newCore" into "newCommand"

  2021-09-07
  -Add "#define XTILT"
   Uncomment this Option to activate Autorotation.
   Use tilt sensor connected to PIN 15 (SPI_0_CS) and GND.
   This function uses the Library "Bounce2" which is available in the Library Manager.

  2021-09-10
  -Add "#define XMIC184"
   Uncomment this Option to activate the MIC184 Temperatur-Sensor included on d.ti's PCB.
   This function uses the Library "eHaJo_LM75" which is available in the Library Manager.

  -- G R A Y S C A L E  E D I T I O N --

  2021-09-19
  -First Grayscale Test based on MiSTer_SSD1322_USB_Testing

  2021-09-25/26
  -Adding Effects 1-10
  -Adding DisplayOn/Off Commands
  -Adding Geo Command, the handling is slightyl slightyl different to the U8G2 Version
  -Adding Text Command with 5 Font's, the handling is slightyl different to the U8G2 Version

  2021-10-01
  -Renamed Otion XMIC184 to XDTI
  -Adding USER_LED (Pin 19) for the  XDTI Option (d.ti Board) and 
  -Adding Command "CMDULED,0/1" (nothing happens without the XDTI Option)
  
  2021-10-03
  -Modify Text Output
   -Font 1.. = Write Text
   -Font 1.. +100 = 101.. Clear Text without Output

  ToDo
  -XMIC T-Sensor enhanced (Command)
  -Everything I forgot
