**Testing-Version**  
  
Updated tty2oled Daemon Script, new Scripts and updated Arduino Code.  
Use the INI File from the main folder.  
  
-tty2oled Daemon (use the one from the Root folder)  
* Make a Backup and replace "/usr/bin/tty2oled" with the one from this directory.  
* Make sure this version is "executeable".  
* ! An Update after using the Testing-Version downloads the "old" tty2oled Daemon again !  
  
-Over The Air (OTA) Updates for ESP32  
* Copy the folder "MiSTer_SSD1322_USB_Testing" to your local Arduino Project Folder  
 You need only the .ino  File and all .h files.  
 You need to add your Wireless Credentials to the "cred.h" file.  
 Update your ESP32 with the new Program. After this Version was applied you can use OTA for future Updates.  
 Copy the Scripts "tty2oled_enable_OTA.sh" and "tty2oled_RESET.sh" from this folder to "media/fat/Scripts".  
  
* Using OTA  
 Enable OTA from the command line (see Command Examples below) or run the Script "tty2oled_control.sh espota".  
 The Arduino IDE should show you an new Network "Port" for Uploads after OTA is active.  
 Use the new Port for the upload of the new Program.  
 See https://misterfpga.org/viewtopic.php?p=28423#p28423  
 If you like to stop OTA, Reset the ESP32 (see Command Examples below) or run the Script "tty2oled_control.sh espreset".  
  
-New Command Structure for "external" Commands  
* "CMDCLS"  
* "CMDSORG"  
* "CMDBYE"  
* "CMCOR,[Corename]" followed by the Picture Data  
* "CMDCON,[Contrast]"  
  
* "CMDTXT,[Parameter]" Text-Ouput  
 Parameter-Format = "f,c,x,y,[Text]  
 f = Font Type 0.. see https://github.com/venice1200/MiSTer_tty2oled/wiki/Command
 c = Draw Color 0=Clear, 1=Draw, 2=XOR/Invert Pixel  
 x = X-Position 0..255  
 y = Y-Position 0..63  
 [Text] = Text to Draw  
  
* "CMDGEO,[Parmeter]" Geometric-Output, 
 Parameter Format = "g,c,x,y,i,j,k"  
 g = Geometric Type see https://github.com/venice1200/MiSTer_tty2oled/wiki/Command  
 c = Draw Color 0=Clear, 1=Draw, 2=XOR/Invert Pixel  
 x = X-Position 0..255  
 y = Y-Position 0..63  
 i = Parameter 1 (Depends on Geometric)  
 j = Parameter 2 (Depends on Geometric)  
 k = Parameter 3 (Depends on Geometric)  
  
* "CMDRESET"   ESP32 Reset  
* "CMDENOTA"   ESP32 Enable OTA Programming Mode  
* "CMDOFF,[Parameter]" Set Power Save Mode  
 Parameter-Format = "p"  
 p = Power Save Mode 0=Disabled/Display ON (Default), 1=Enabled/Display OFF  
  
* "CMDROT,[Parameter]" Enable Rotation  
 Parameter-Format = "r"  
 r = Rotation 180° 0=Disabled (Default), 1=Enabled  
  
Examples Command Line (CLI/SSH):  
`echo "CMDCLS" > /dev/ttyUSB0`                             Clear Screen  
`echo "CMDCON,211" > /dev/ttyUSB0`                         Send Contrast Value 211  
`echo "CMDTXT,5,1,10,10,Hello World" > /dev/ttyUSB0`       Write/Draw "Hello World" at 10,10 with Font 5  
`echo "CMDGEO,4,1,10,10,50,20,0" > /dev/ttyUSB0`           Draw Box at 10,10 with 50x20 Pixel  
`echo "CMDGEO,4,0,10,10,50,20,0" > /dev/ttyUSB0`           Clear Box-Area at 10,10 with 50x20 Pixel  
`echo "CMDGEO,4,2,10,10,50,20,0" > /dev/ttyUSB0`           Invert Box-Area at 10,10 with 50x20 Pixel (not for Circle/Disc/Ellipse)  
`echo "CMDENOTA" > /dev/ttyUSB0`                           Enable ESP32 OTA Mode  
`echo "CMDRESET" > /dev/ttyUSB0`                           Reset/Reboot ESP32  
`echo "CMDOFF,1" > /dev/ttyUSB0`                           Enable Power Save/Display OFF  
`echo "CMDOFF,0" > /dev/ttyUSB0`                           Disable Power Save/Display ON  
`echo "CMDROT,1" > /dev/ttyUSB0`                           Rotate Display 180 degrees  
`echo "CMDROT,0" > /dev/ttyUSB0`                           Do not Rotate Display  



  
Report Issues here:  
https://misterfpga.org/viewtopic.php?p=28397  

