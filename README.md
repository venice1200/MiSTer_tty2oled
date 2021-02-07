## tty2oled [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
Add-On for the [MiSTer FPGA](https://github.com/MiSTer-devel) showing Text or Pictures based on the loaded core on a Display driven by an Arduino which is connected via USB-Serial.  
  
**NEW** Arduino Sketch with SD Support!
  
![tty2oled connection](https://github.com/venice1200/MiSTer_tty2oled/blob/main/Pictures/OLED_Connection.jpg?raw=true)
  
### The Idea
At first I had the idea to add an Display which shows only the MiSTer Logo.  
After a bit of searching I found the possibilty to get information about the actual loaded Core out of the file `/tmp/CORENAME`.  
I tried `tail -F /tmp/CORENAME` which results in error messages so I choose `cat /tmp/CORENAME` in a timed loop.  
  
### Realizing
I tested an USB-Serial connection between an Arduino and the MiSTer  
as this is an standard communication feature on Arduino's and easy to use.  
And USB Ports are available for most of the MiSTer users is some way.  
My ATMega 1284 with an FTDI Serial Adapter was sucessfully detected as `/dev/ttyUSB0` by the MiSTer.  
Later I used an ESP32 with an CP21xx Chip because of storage and speed.  
My actual Display is an 3.12" SSD1322-OLED with 256x64 Pixel connected via SPI to the ESP32 MCU.  
  
**Arduino**  
For the Arduino Code you need to add this Library https://github.com/olikraus/u8g2 to your Arduino System.  
This Library needs the pictures in [XBM](https://en.wikipedia.org/wiki/X_BitMap) Format. I use [Krita](https://krita.org/) for converting.  
Just open your **B/W Picture** file and save it as XBM. See the Arduino Code [`logo.h`](https://github.com/venice1200/MiSTer_tty2oled/blob/main/MiSTer_SSD1322/logo.h) file for details.  
The included XBM Pictures are converted to an max Size of 256x64 Pixel.  
The Arduino Code can be easily adapted to other Displays, Display-Libraries or Arduino's.  
There is a lot of commented code in the files. I will clean it up step by step.  

**MiSTer**  
On the MiSTer you need two scripts.  
The first script `/etc/init.d/S60tty2oled` is an starter script for the second script `/usr/bin/tty2oled`  
which handles the communication to the Arduino via the USB-Serial tty device.  
  
### Demo Videos
Video Part 1: https://imgur.com/a/JOnaKPq  
Video Part 2: https://imgur.com/gallery/Ek9oFN1  
  
### How does it work?
**MiSTer**  
When the MiSTer boots up the script `/etc/init.d/S60tty2oled` is called.  
This script does nothing more than calling the script `/usr/bin/tty2oled` and sent it to the background,  
but only if `/usr/bin/tty2oled` is found and is executable.
  
The script `/usr/bin/tty2oled` checks at first the system for the device `/dev/ttyUSB0`.  
If the device is not found the scripts ends here.  
If the device is found the tty-device parameter are set with the [`stty`](https://man7.org/linux/man-pages/man1/stty.1.html) command.  
Now the script sends an **First Transmission**.  
*First Transmission?*  
During my tests I got sometimes weird display startup behavior when the MiSTer's was powerred on.  
Sometimes the text **MENU** was written instead of the MiSTer Picture shown.  
*Maybe some sleeping bits in the serial channel :smirk:*.  
After I added the **First Transmission** to the script the problem was gone :smile:.  
Now the main loop is started and checks the existence of the file `/tmp/CORENAME`.  
If this file doesn't exist, the script waits for 2 secs and tried the next loop.  
If the file exists, the file's content is read using `cat /tmp/CORENAME`,  
stored into the variable `newcore` and send to the tty Device.  
At the end of the main loop the script waits for 2 secs and starts the next loop.  
Every loop the Corename is checked **but the data are sent only if the Corename has changed**.  
  
**Arduino** (non SD Version)  
The Arduino receives the transferred Corename.  
If the Corename is within a list of known Names the corresponding picture is shown.  
![TGFX16](https://github.com/venice1200/MiSTer_tty2oled/blob/main/Pictures/TGFX16.jpg?raw=true)
  
If the Corename is unknown just the Corename is shown.  
![Astrocade](https://github.com/venice1200/MiSTer_tty2oled/blob/main/Pictures/Astrocade.jpg?raw=true)
  
The used font for the Text can be changed within the Arduino Code.  
See the [list of fonts](https://github.com/olikraus/u8g2/wiki/fntlistall) for all in the Library included fonts.  
**To prevent Display flickering the Text or Picture is only updated if the received Corename has changed.**  

**Known Corenames** (non SD Version)  
The following Corenames are (currently) identified and an dedicated or generic Picture is shown.  
**Arcade:** llander, mooncrgx ,mpatrol  
**Computers:** AO486, APPLE-I, Apple-II, AtariST, C64, Minimig, PET2001, VIC20  
**Console:** ATARI2600, Genesis, NEOGEO, NES, SMS, SNES, TGFX16  
**Other:** MEMTEST, MENU (shows the MiSTer Logo fading in with an particle effect), QWERTZ (does nothing, it's **first transmission**).  
  
**Arduino** (SD Version)  
The SD Version needs the converted XBM files stored on the root folder of the SD Card.  
The Pictures need to have the size of 256x64 Pixel for the SSD1322 Display.  
The filename must be the name of the core plus .xbm as extension.
The Arduino receives the transferred Corename and check the SD Card for an corresponding file.
Example: The Arcade Lunar Lander has the corename **llander**,  
means the filename need to be **llander.xbm**.  
If the file is found it's loaded and shown on the Display.  
If the file is not found just the Corename is shown.  
So it's easy to add and update the Pictures by adding or changing the files on the SD.  

**Known Corenames** (SD Version)  
The following Corenames are (currently) identified and an dedicated or generic Picture is shown.  
**Other:** MEMTEST, MENU (shows the MiSTer Logo fading in with an particle effect), QWERTZ (does nothing, it's **first transmission**).  
  
### Testing
You can easily test your Display by sending text from the Arduino's Serial console set to 9600 baud and LineFeed `\n` activated.  
The following Text-Commands have special functions:  
| Text | Function |
| :--- | :--- |
| cls | Clear Display |
| sorg | Show Display's Power-On Text |
| bye | Show Sorgelig's Icon :smile:|
| MENU | Show MiSTer Logo with Particle Effect |
| MENU2 | Show MiSTer Logo with Effect 2 (SD Version only) |
| MENU3 | Show MiSTer Logo with Effect 3 (SD Version only) |
| MENU4 | Show MiSTer Logo with Effect 4 (SD Version only) |

  
### Files and Folders in this Repository
| File | Description |
| :--- | :--- |
| S60tty2oled | Starter Script, must be placed on **MiSTer** in folder `/etc/init.d/`  |
| tty2oled | Communications Script, must be placed on **MiSTer** in folder `/usr/bin/` |

| Folder | Description |
| :--- | :--- |
| MiSTer_SSD1322 | The Arduino Project containing the `.ino` and `.h` file(s) |
| MiSTer_SSD1322_SD | The Arduino Project with SD Support containing the `.ino` and `.h` file(s) |
| Pictures | Just Pictures :smile: |
| Pictures/xbm | Some XBM Pictures |
| Pictures/xbm_sd | Pictures for the SD Version |


### Ideas
Use an second serial port to free up the MCU's programmer port.  
~~Use an ESP32 with an SD Micro Slot for the Pictures~~ DONE.  
  
### Links
MiSTer on Github: https://github.com/MiSTer-devel  
MiSTer Forum: https://misterfpga.org  
Add-On Thread : https://misterfpga.org/viewtopic.php?f=9&t=1887  
  
...work in progress  
  
