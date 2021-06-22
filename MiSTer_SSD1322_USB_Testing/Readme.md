Testing Version

New
-Over The Air Updates for ESP32
-New Command Structure for "external" Commands
   "CMDCLS"
   "CMDSORG"
   "CMDBYE"
   "CMCOR,[Corename]" followed by the Picture Data
   "CMDCON,[Contrast]"
   
   "CMDTXT,[Parameter]"   Text-Ouput,       Parameter-Format = "f,c,x,y,[Text]
   f = Font Type 0.. (see Font List above)
   c = Draw Color 0=Clear, 1=Draw, 2=XOR/Invert Pixel
   x = X-Position 0..255
   y = Y-Position 0..63 
   [Text] = Text to Draw

   "CMDGEO,[Parmeter]"    Geometric-Output, Parameter Format = "g,c,x,y,i,j,k"
   g = Geometric Type 1..10
   c = Draw Color 0=Clear, 1=Draw, 2=XOR/Invert Pixel
   x = X-Position 0..255
   y = Y-Position 0..63
   i = Parameter 1 (Depends on Geometric)
   j = Parameter 2 (Depends on Geometric)
   k = Parameter 3 (Depends on Geometric)

   "CMDRESET"   ESP32 Reset
   "CMDENOTA"   ESP32 Enable OTA Programming Mode

Examples:
echo "CMDCLS" > /dev/ttyUSB0                             Clear Screen
echo "CMDCON,211" > /dev/ttyUSB0                         Send Contrast Value 211
echo "CMDTXT,5,1,10,10,Hello World" > /dev/ttyUSB0       Write Hello World at 10,10
echo "CMDGEO,4,1,10,10,50,20,0" > /dev/ttyUSB0           Draw Box at 10,10 with 50x20 Pixel
echo "CMDENOTA" > /dev/ttyUSB0                           Enable OTA Mode, the Arduino IDE should show you an new Network "Port" for Uploads

Make Backup and Replace "/usr/bin/tty2oled" with the one in this directory. 
Make sure this version is executeable.

Report Issues here:
https://misterfpga.org/viewtopic.php?p=28397
