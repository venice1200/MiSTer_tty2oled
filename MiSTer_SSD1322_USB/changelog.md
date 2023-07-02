*************************** tty2oled Arduino-FW Changelog ***********************

  2023-07-02
  -Release to Stable

..2023-04-27
  -Cosmetics

  2023-04-20/2023-03-16
  -Add Command CMDSTTYACK to (set) enable(1)/disable(0) sending "ttyack". Enabled by default.
   Using no Command Delay (cDelay) if "ttyack" is not sent.

  2023-01-08
  -Add Boolean "runsTesting"

  2022-12-09..2023-01-07
  -Add more "hasXX"

  2022-12-08
  -Change 8266 NodeMCU Tilt Pin 16 (D0) to Pin 10 (SD3) to prevent false Rotation detection

  2022-11-27
  -Switch dtiv Version handling from "EEPROM" to "Preferences"

  2022-11-23
  -Removed unused Board Types

  2022-11-12..13
  -Add Suport for ESP32-S3 (ESP32-S3-DevKitC-1)
  -Removed OTA Support
  -Add EEPROM Support for d.ti Boards (IDE: ESP32* Dev Module)

  2022-10-30
  ***** Release to stable 221030 ******

  2022-10-24
  -Change Picture Array logoBin from "malloc" to "normal" defined type.

  2022-10-20
  -Add randomly chosen animated ScreenSaver after Corechange

  2022-10-15
  -Add short delay after Rotation-Pin Initialization (try to prevent false Rotation detection on 8266)
  -Add ScreenSaver Flying Toasters by Phillip Burgess/Adafruit
   https://learn.adafruit.com/animated-flying-toaster-oled-jewelry/code (MIT License) 
   Screensaver Bits: Bit 0=tty2oled Logo (Value 1), 1=MiSTer Logo (2), 2=Core Logo (4), 3=Time (8), 4=Date (16), 5=Stars (32), 6=Toast (64)

  2022-10-14
  -Changed filename logo.h to bitmaps.h
  
  2022-10-12
  -Modifying "Stars", adding Star-Depth-Color calculation

  2022-10-09
  ***** Release to stable 221009 ******

  2202-10-07
  -Fix for 8266 which don't get the Starfield Screensaver to prevent stability issues

  2022-09-30
  -Add the StarField Simulation as ScreenSaver
   Code from https://github.com/sinoia/oled-starfield (MIT License) 
   8266 must run at 160MHz!
   Screensaver Bits: Bit 0=tty2oled Logo (Value 1), 1=MiSTer Logo (=Value 2), 2=Core Logo (Value 4), 3=Time (Value 8), 4=Date (Value 16), 5=Stars (Value32)

  2022-09-25..27
  -New Command "CMDSWSAVER,[0/1]" for just Switching the Screensaver on/off
  -Workaround for the detection of PCA9536 = d.ti Board v1.2
  
  2022-09-02
  -New functions "oled_showcenterredtext" and "oled_setfont"

  2022-09-01
  -More usage of "dtiv"

  2022-08-26..28
  -New Command "CMDSHSYSHW" shows Hardware & Software Infos on Screen

  2022-09-18
  -Shorten the Date Format for the Screensaver. e.g. "19. September 2022" => "19-Sep-22".

  2022-08-07..12
  -Add RGB LED to Startup Screen (d.ti Board 1.2)
  -New Variable "dtiv" showing d.ti Board Revisions: 11=1.1 12=1.2...

  2022-08-23
  -Replace some "if (pcaAvail)" with "if (dtiv==12)"

  2022-07-31
  -SSD1322 Minor change
  -New Option "XOTA" for disable/enable the OTA functionality, Sketch consumption 65%->28%

  2022-07-23 
  ***** Release ***** to stable 220723 (based on 220714T)

  2022-07-12..14
  -Introduce ScreenSaver "Screens" which can be selected separatly (bitwise)
   Bit 0=tty2oled Logo (Value 1), 1=MiSTer Logo (=Value 2), 2=Core Logo (Value 4), 3=Time (Value 8), 4=Date (Value 16)
   Examples: 12=4+8=Core+Time, 5=1+4=tty2oled+Core, 13=1+4+8=tty2oled+Core+Time

  2022-07-09
  -Minor Update to make the Sketch compatible to ESP32 Board-Pack v2.0.4

  2022-07-08 -OBSOLETE- see 2022-07-12
  -Change ScreensaverColor to ScreeensaverMode
   Modes: 1: Show tty2oled Logo
          2: Show tty2oled Logo, Mister Logo
          3: Show tty2oled Logo, Mister Logo, Core Logo
          4: Show tty2oled Logo, Mister Logo, Core Logo, Time
          5: Show tty2oled Logo, Mister Logo, Core Logo, Time & Date

  2022-07-06
  -Adding two new fonts u8g2_font_commodore64_tr and u8g2_font_8bitclassic_tf

  2022-06-08
  -Note: Use U8g2_for_Adafruit_GFX setForegroundColor and setBackgroundColor together for inverted text
   See: https://github.com/olikraus/U8g2_for_Adafruit_GFX#command-reference

  2022-06-02
  -Add Function Prototypes for C++ Compatibilty

  2022-05-21
  -Only Cosmetics

  2022-05-07
  ** Release to stable**
  -Fix CMDPNOTE/CMDPTONE
  -Add CMDSHTIME
  -A lot Cosmetics
  -Lib Update: ESP32 Package 2.0.1 -> 2.0.3
  -Lib Update: Adafruit GFX 1.10.14 -> 1.11.1

  2022-04-28
  -Remove CMDSTEMP command and temnperature loop
  -Add CMDSHTEMP Command, show only once the actual MIC184 Temperature, for DTI Boards only
  

  2022-04-22
  -Command CMDSETTIME can be called by all MCU's but only the ESP32 is setting his internal RTC

  2022-04-18
  -Add Time to ScreenSaver but only if Time was set before, and only for ESP32

  2022-04-17
  -Add Code for new Commadn CMDSETTIME (stolen from tty2tft)
  -Add CMDNULL (Test) Command, for Troubleshooting and ACK Testing.
  
  2022-04-16
  -Fix CMDPNOTE/CMDPTONE Bug if Parameter is missing

  2022-04-10..13
  -Add Command "CMDSECD,[value]" to set the Command Delay
  -Add Command "CMDSHCD" to show the actual Command Delay "On Screen" (for Debugging)
  -Set cDelay for ESPDEV=15, Lolin32=60, 8266=60 after some manual tests with MiSTer SAM

  2022-04-07
  -Testing without "serial.flush" after "ttyack;"
  
  2022-04-07
  -Renamed Command CMDPTONE to CMDPNOTE
   >> CMDPNOTE,C,4,150,30,C,4,160,30,C,4,150,30,F,4,150,30,C,4,150,30  
  -Renamed Command CMDPFREQ to CMDPTONE

  2022-04-05
  -Wemos Lolin 32 Profile: #define cDelay 100 fixes MiSTer SAM issues with waitforack

  2022-04-01
  -Consolidate CMDSSCP/CMDSSCP2 and oled_showSmallCorePicture/oled_showSmallCorePictureV2

  2022-03-31
  ** Release **

  2022-03-23
  -Change cDelay from Variable to MCU dependent #define Value
   Fixes a response issue using "waitforack" and MiSTer_SAM with NodeMCU 8266

  2022-03-19
  -Screensaver Mod, use avarage of 2x2 Pixels for the small GSC Picture

  2022-03-16
  -Screensaver Mod, use 1/4 size Core Picture for the ScreenSaver
  -New Command CMDSSCP

  2022-03-02
  -Use more PicType "NONE" (Cosmetics)

  2022-02-17
  -Change minimum SceeenSaver Intervall from 10s to 5s

  2022-02-09 
  ** Release to Stable **

  2022-02-07
  -Modified Calculation ScreenSaverLogoTime
   First Screensaver shown now after ScreenSaverLogoTime-ScreenSaverInterval+ScreenSaverInterval

  2022-02-06
  -Change Command CMDSAVER
   CMDSAVER,Mode/Color, Interval, LogoTime
   Mode/Color: 0=Off, 1..15=On/Color, Interval: 10..600 Seconds, LogoTime: 10..600 Seconds

  2022-02-05
  -Add Command CMDSAVER to enable Screensaver
   CMDSAVER,Mode/Color, Interval
   Mode/Color: 0=Off, 1..15=On/Color, Interval: 10..600 Seconds

  2022-01-29..30
  -Enhanced Support for d.ti Board Rev 1.2
  -Modded Command CMDULED supporting d.ti Board v1.2
   d.ti Board v1.1 CMDULED,[0,1] User LED Off/On
   d.ti Board v1.2 CMDULED,[0,1..255] WS2812B User LED Off/On with Color (HSV Colors; Hue = Command Parameter, Saturation & Value = 255)
  -Add Command CMDPLED for d.ti Board v1.2
   CMDPLED,[0,1] Switch Power LED Off/On
  -Add Command CMDPTONE for d.ti Board v1.2
   CMDPTONE,[Note,Octave,Duration,Pause],[Note,Octave,Duration,Pause]... play Note/Tone using the Buzzer.
   CMDPTONE,C,4,150,30,C,4,160,30,C,4,150,30,F,4,150,30,C,4,150,30
  -Add Command CMDPFREQ for d.ti Board v1.2
   CMDPFRQ,[Frequency,Duration,Pause],[Frequency,Duration,Pause]... play Frequency using the Buzzer.

  2022-01-23
  -Add Basic Support for d.ti Board Rev 1.2 and the included WS2812B LED, Buzzer, Power(off)LED
  -Piezo Buzzer Tone Examples https://makeabilitylab.github.io/physcomp/esp32/tone.html
  -FastLED https://github.com/FastLED/FastLED/wiki, https://www.youtube.com/watch?v=FQpXStjJ4Vc

  2022-01-05
  -Cleanup unused vars, adding some cast operators

  2022-01-04
  ** Release to stable**
  -New Command CMDCLSWU
   Clear Display Buffer without Display Update (like CMDCLS) 
  -Send "ttyrdy;" after Display Start/Setup is done

  2021-12-16
  -Changed Command "CMDCOR"
   Adding optional Effect Parameter "t" after Corename, Example "CMDCOR,llander,19"
   t= -1:Random Effect, 0=No Effect, >1=see Effect List
  -Changed Command "CMDSPIC"
   Adding optional Effect Parameter "t" after Command, Example "CMDSPIC,19"
   t= -1:Random Effect, 0=No Effect, >1=see Effect List
  -Changed Command "CMDCLST"
   t= -1:Random Effect, 0=No Effect, >1=see Effect List

  2021-12-14
  -New Command "CMDCLST,t,c" (t=transition, c=color (0..15))
   Clear the Display Screen with transition and given color

  2021-12-07
  -ESP32DEV only: Adding PCA9536 availabilty check

  2021-11-29..12-06
  -New Effects 15-19 (Fade in from center to...)
  -New Effects 20 (Slightly Clockwise), 21 (Shaft), 22 (Waterfall), 23 (Chess Squares)

  2021-11-26
  ** Release ** See forums post next to https://misterfpga.org/viewtopic.php?p=38195#p38195 

  2021-11-22
  -Cleanup not needed "NA/NN" Texts

  2021-11-16
  -Removed Options for XTILT and XDTI. We use now software mechanism for determination of Hardware
  -MIC184 only available for ARDUINO_ESP32_DEV/USE_ESP32DEV Hardware and only if MIC is detected via i2c
  -Tilt available for all Hardware Platforms, ESP8266 Tilt Pin = Pin16

  2021-11-15
  -Changed Option USE_TTGOT8=USE_ESP32DEV

  2021-11-14
  -New Command "CMDHWINF"
   Send HW Info back to the MiSTer/CMD-Sender. Useful for ESP Firmware Updates.
   Currently: "HWESP32DE"=TTGO-T8 ESP32, "HWLOLIN32"=Lolin&DevKit ESP32, "HWESP8266"=ESP8266, "HWDTIPCB0"=d.ti Board ESP32

  2021-11-11
  -MIC184 uses now the Library MIC184 (modified LM75 Library)
  -New Command "CMDTZONE,z" for d.ti Board only!
   Set the Temperature Zone for the MIC184. z=0 Internal Zone, z=1 Remote Zone.

  2021-11-04/07
  -New Picture Slide-In Effects (11..14) and some effect speed adjustments

  2021-10-28
  -Adding Bool-Variable "startScreenActive"
  -Renamed function "oled_mistertext" to "oled_showStartScreen"

  2021-10-27
  -d.ti Board only: Actualize Temperature on plain boot screen if activated

  2021-10-22
  -d.ti Board only: If you sent the Text "TEP184" via Command "CMDTEXT" the Text will be replaced with the Temperature Sensor Value including "°C"

  2021-10-20
  -Added 7 Segment Font (Font 8)

  2021-10-12
  -Release "Greyscale Mode"

  2021-10-11
  -Adding Command "CMDSTEMP" for the d.ti Board (Option XDTI)
   If you send this command the Display shows and refreshs the Temperature as long as no other Commands are sent.

  2021-10-08
  -Adding Library "U8G2 for Adafruit GFX" for better Text Support
  -Adding hidden Text write and Geometric draw Modes to the Text and Geo Commands
   Font or Geo Value +100 write or draw the content only to the Display buffer without displaying it
   Good for cleaning up Display content
  -Text Command got Background Color Variable
   Write Text with given Font Color (c) and Background Color (b)
  -Adding Command "CMDDUPD"
   Update Display content (write buffer to Display)

  2021-10-01
  -Renamed Otion XMIC184 to XDTI
  -Adding USER_LED (Pin 19) for the d.ti Board (Option XDTI) 
  -Adding Command "CMDULED,0/1" for the d.ti Board (Option XDTI)

  2021-09-25/26
  -Adding Effects 1-10
  -Adding DisplayOn/Off Commands
  -Adding Geo Command, the handling is slightyl slightyl different to the U8G2 Version
  -Adding Text Command with 5 Font's, the handling is slightyl different to the U8G2 Version

  -- G R A Y S C A L E  E D I T I O N --

  2021-09-19
  -First Grayscale Test based on MiSTer_SSD1322_USB_Testing

  2021-09-10
  -Add "#define XMIC184"
   Uncomment this Option to activate the MIC184 Temperatur-Sensor included on d.ti's PCB.
   This function uses the Library "eHaJo_LM75" which is available in the Library Manager.

  2021-09-07
  -Add "#define XTILT"
   Uncomment this Option to activate Autorotation.
   Use tilt sensor connected to PIN 15 (SPI_0_CS) and GND.
   This function uses the Library "Bounce2" which is available in the Library Manager.

  2021-08-23..26
  -Moved Startscreen-Text into "defines"
  -Add "#define XSENDACK"
   Uncomment this Option to enable the Handshake with the MiSTer Daemon. !! You need the Daemon from testing !!
  -Add "#define XLOGO"
   Uncomment this Option to get the tty2oled Logo shown on Startscreen instead of the Starttext
  -Change Variable Name "newCore" into "newCommand"

  2021-08-15/16
  -Separate the "Read and Draw Picture" function into two functions
  -New Command "CMDSPIC" which just show the actual loaded Picture (SHOWPIC).
  -New String Variable "actCorename" containing the actual Corename
  -New Command "CMDSNAM" which just show the actual Corename as text (SHOWNAME).
  -New Command "CMDAPD,[corename]" which is just for sending Picture Data, nothing more (Attention Picture Data).
  -!Remove old Command Mode (att, CORECHANGE, CONTRAST, TEXTOUTXY, GEOOUTXY) so you need to use an actual Daemon

  2021-08-10
  -Reactivate System "#define USE_xxxx" set by Arduino's Board selection (Auto Mode) 
   or by uncommenting only the needed "#define USEe_XXX" for your Hardware
  -Add #define "XROTATE", uncomment it for an 180° Rotated Display.

  2021-08-07
  -New & smaller USB Icon.

  2021-08-03
  -Some of the included Logo's miss their U8X8_PROGMEM.

  2021-07-12
  -New Command to set the Font Direction. 
   "CMDFOD,[Parameter]", Parameter Format "d", d: 0=Left to Right (0°, default), 1=Top to Down (90°), 2=Right to Left (180°), 3=Down to Top (270°)

  2021-07-18 (Testing)
  -Trying Handshake with the MiSTer Daemon by sending "ttyack;" (with delimiter) after each processed command.
   !! But this need a very little delay of [cDelay]ms before "ttyack" could be send. => Need more testing !!
   In tty2oled.ini the TTYPARAM Parameter needs "-echo"

  2021-07-12
  -Combine the two separate LogoDrawing functions (usb2oled_readndrawlogo/usb2oled_readndrawlogo2) into one (usb2oled_readndrawlogo2)

  2021-07-11
  -Some more yield() fixes for ESP8266
  -Bugfix Effekt 10 (Diagonal)

  2021-07-10
  -Add Serial.flush to "Setup" to clear the transmit buffer, see https://misterfpga.org/viewtopic.php?p=29703#p29703

  2021-07-09
  -Some yield() fixes for ESP8266

  2021-07-07/08
  -New Command "CMDTEST" which just show an fullscreen test-picture
  -Count amount of transferred picture bytes and if it doesn't match show an error picture
  -Change Command processing from "if (newCommand!=oldCore)" to "if (updateDisplay)" to prevent a blank screen if multiple data packs are sent.
  -Add (Micro Font) Build Version to Start Screen

  2021-06-30
  -New Command "CMDROT"
   "CMDROT,[Parameter]" Set Display Rotation (0=180° (My Standard), 1=0 degrees) Rotation after Start
   You will see the Command Result after the next Write/Draw Command.

  2021-06-24
  -Changed location of the "yield()" Command which is needed for 8266 NodeMCU

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

  2021-06-18
  -BasicOTA only for ESP32 <= TESTING

  2021-06-18
  -Change MALLOC for Picture Data Memory Allocation into Setup for more stabilty. <= TESTING
  -Disabled "Snake" Transition, need too much time.
  -Changed "TEXTOUTXY" Command-Parameter Format to "x,y,f,[Text]"
   x = X-Position 0..255
   y = Y-Position 0..63 
   f = Font Type 0.. (see Font List above)
   Better calculation using "indexof".
   Found here: https://forum.arduino.cc/t/how-to-parse-arduino-string-with-different-delimiters/324653/3

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

  2021-05-24
  -Trying to detect the Board chosen in the Arduino IDE (ESP32 Dev Module, WEMOS LOLIN32 or NODEMCU 8266) and set the Display config.
   No need to set the Board manually in the Sketch.
   But without the correct Board you get Compiler Errors.

  2021-05-22
  -Device CleanUp 

  2021-05-05
  -Add an Option to receive the Contrast Level from the MiSTer
   At First MiSTer needs to send "att"
   As Second MiSTer needs to send "CONTRAST"
   As Third MiSTer needs to send "[Contrast Value 0..255]"

  2021-04-??
  -Add "Set Contrast" to the Code by "ojaksch"

  2021-03-29
  -USB Version Based on SD Version
   Adding USB functionality from "ojaksch" (Many Thanks) and removed SD Stuff.
   Receive Picture-Data via Serial connection instead of the Corename from the MiSTer.
   Speed up Serial Interface to 115200
   Show a small "USB-Icon" on Startup Screen

  -Using #ifdef...#endif for my different HW-Configs (LOLIN32/TTGOT8)
   Choose ONLY one USE_xxxx Board definition at the beggining of the Sketch
  -Adding more Debug Infos

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

  2021-03-07
  -Adding some Menu Effects
  -Code cleanup

  2021-03-06
  -Adding some Menu Effects
  -Code cleanup

  2021-02-16 (not released)
  -Make it more Universal 
   Add DispLineBytes , using more DispHeight & DispWidth instead of fixed Values

  2021-02-20/21
  -Using the Effects for all Picture/Logo Transistions (not only for the Menu)
  -"MENU" Picture moved from Code to the SD, makes it easier to change (if you like).
  -Modified Text Position calculation (see U8G2 Documentation for Details). 
   Change text reference position from TOP to BASELINE.
   Using now "getAscent" instead of "getMaxCharHeight" for Text Y-Position calculation.
  -Show a small "SD-Icon" on Startup Screen instead of the "dot" if SD Card was detected.

  2021-02-12
  -Adding some Menu Effects

  2021-02-11
  -Trying to make it more universal in case of used display sizes. Using more "DispHeight" and "DispWidth" instead of fixed values.
  2021-02
  -Change serial Speed to 57600
  -Adding SD Support for ESP32 using an dedicated SPI Interface
   Use now an TTGO-T8  ESP32 with integrated SD Micro Slot https://github.com/LilyGO/TTGO-T8-ESP32 .output
   All Logos or Picture have to be 256x64 Pixel (Display Size) in XBM Format.
   Place the Pictures to the root of the FAT or FAT32 formatted SD Card an give them the name like the core is named with extension (Lunar Lander = llander.xbm).
   Use Code from "XBM Read from FS" from https://github.com/lbernstone/eyes
  -Add "#define XDEBUG" for Debugging, can be Uncommented for more Infos over Serial

=====================================================================================
=====================================================================================
ToDo
-Everything I forgot

Effect List (2021/12)
01 Fade In Left to Right
02 Fade In Top to Bottom
03 Fade In Right to left
04 Fade In Bottom to Top 
05 Fade In Even Line Left to Right / Odd Line Right to Left
06 Fade In Top Part Left to Right / Bottom Part Right to Left
07 Fade In Four Parts Left to Right to Left to Right...
08 Fade In 4 Parts, Top-Left => Bottom-Right => Top-Right => Bottom-Left
09 Fade In Particle Effect
10 Fade In Left to Right Diagonally
11 Slide In left to right
12 Slide In Top to Bottom
13 Slide In Right to left
14 Slide In Bottom to Top
15 Fade In Top and Bottom to Middle
16 Fade In Left and Right to Middle
17 Fade In Middle to Top and Bottom
18 Fade In Middle to Left and Right
19 Fade In Warp, Middle to Left, Right, Top and Bottom
20 Fade In Slightly Clockwise
21 Fade In Shaft
22 Fade In Waterfall
23 Fade In Chess
24 WIP

