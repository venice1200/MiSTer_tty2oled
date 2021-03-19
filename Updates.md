2021-01-22/23  
Release of the MiSTer Scripts  
Release of "Standard" (Non SD) Version  
  
2021-02-07  
Release of SD Version  
**tty2oled & Standard Version:** Update to Serial Speed 57600baud  

2021-02-11  
**SD & Standard Version:** Update Menu Particle Effect, works correctly now  
  
2021-02-14  
**tty2oled Script:** Changed the timed loop from **sleep $WAITSECS** to an loop with **inotifywait -e modify "/tmp/CORENAME"**.  
This makes the system more reactive as the scripts waits for an Core change instead of polling.  
**SD Version:** Adding more Home Menu Effects (just for fun)  
  
2021-02-20/21  
**SD Version**  
Using the Effects for all Picture/Logo Transistions (not only for the Menu)  
"MENU" Picture moved from Code to the SD, makes it easier to change (if you like).  
Modified Text Position calculation (See Arduino File for Details).  
Show a small "SD-Icon" on Startup Screen instead of the "dot" if SD Card was detected.  
  
2021-03-01..04  
**Standard Version**  
Update by **ojaksch**: New Pictures added to the Arduino Code  
He uploaded the XBM Files as well so they can be used with the SD Version.  
  
2021-03-06  
**SD Version**  
Code Cleanup and 2 more Effects  
  
2021-03-14  
**SD Version**  
Update SD Access from SPI to SD Bus Mode (SD_MMC)
Depending on the used hardware 1 or 4 Bit Bus Mode is possible.  
Code Cleanup & More Debug Code added  
  
**Standard Version**  
Update by ojaksch: New Pictures added to the Arduino Code  
2 new Pictures added  
  
Adding/Changing Pictures (BMP/XBM) files from time to time  
....work in progress  
