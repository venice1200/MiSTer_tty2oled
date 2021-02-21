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
  
Adding/Changing Pictures (BMP/XBM) files from time to time  
....work in progress  
