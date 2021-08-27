**Testing-Version**  
  
Updated tty2oled Daemon Script, new Scripts and updated Arduino Code.  
Use the INI File and the tty2oled Script from this folder.  
Remove ".testing" from the filenames.  
  
Most important change is the command response back from the tty2oled ESP to the tty2oled Daemon.  
When a command is processed by the ESP a "ttyack" is sent back which the tty2oled Daemon can process.  
If you change the line "#define XSENDACK" to "//#define XSENDACK" you can use the Arduino Code with the current Daemon.  
  
The file tty2oled_cc.testing.sh (remove .testing) is the tty2oled Control Center.  
From here you can start/stop/restart the tty2oled Daemon, start an Picture Slideshow, start OTA or Reset the ESP32.  
Take a look.  
  
The file Clock01.ps1 is an PowerShell Script showing your Windows System Time.  
The file tty2oled_GEO.testing.sh (remove .testing) is the same for Linux Systems.  
  
  
Options in Testing Arduino Sketch/Program  
* // Startscreen Text  
  #define StartText1 "MiSTer FPGA"  
  #define StartText2 "by Sorgelig"  
  
* // Uncomment to get the tty2oled Logo shown on Startscreen instead of text  
  #define XLOGO  
  
* // Uncomment for 180° Rotation (Display Connector up)  
  //#define XROTATE  
  
* // Uncomment for "Send Acknowledge" from tty2oled to MiSTer, need Daemon from Testing  
  #define XSENDACK  
  
Report Issues here:  
https://misterfpga.org/viewtopic.php?p=28397  

