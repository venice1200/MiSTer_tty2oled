# MiSTer_tty2oled [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
Add-On for the [MiSTer](https://github.com/MiSTer-devel) showing Text or Pictures on a Display driven by an Arduino which is connected via USB-Serial.<br>
  
![tty2oled connection](https://github.com/venice1200/MiSTer_tty2oled/blob/main/Pictures/OLED_Connection.jpg?raw=true)
  
Files and Folders in this Repository  

| File | Description |
| :--- | :--- |
| S60tty2oled | Starter Script, must be placed on **MiSTer** in folder `/etc/init.d/`  |
| tty2oled | Communications Script, must be placed on **MiSTer** in folder `/usr/bin/` |

| Folder | Description |
| :--- | :--- |
| MiSTer_SSD1322 | The Arduino Project containing the `.ino` and `.h` file(s) |
| Pictures | Pictures :-) |

  
At first I had the idea to add an Display only showing the MiSTer Logo.  
After a bit of searching I found the possibilty to get information about the actual loaded Core out of the file `/tmp/CORENAME`.  
I tried `tail -F /tmp/CORENAME` which results in error messages so I choose `cat /tmp/CORENAME` in a timed loop.  
  
I tried an USB-Serial connection between an Arduino and the MiSTer  
as this is an standard communication feature on Arduino's and easy to use.  
And USB Ports are available for most of the MiSTer users is some way.  
My ATMega 1284 with an FTDI Serial Adapter was sucessfully detected as `/dev/ttyUSB0` by the MiSTer.  
Later I used an ESP32 with an CP21xx Chip because of storage and speed.  
My actual Display is an SSD1322-OLED with 256x64 Pixel connected via SPI to the ESP32 MCU.  

**Arduino**  
For the Arduino Code you need to add this Library https://github.com/olikraus/u8g2 to your Arduino System.  
This Library needs the pictures in [XBM](https://en.wikipedia.org/wiki/X_BitMap) Format. I use [Krita](https://krita.org/) for converting.  
Just open your **B/W Picture** file and save it as XBM. See the Arduino Code `logo.h` file for details.  
The included XBM Pictures are converted to an max Size of 256x64 Pixel.  
The Arduino Code can be easily adapted to other Displays, Display-Libraries or Arduino's.  
There is a lot of commented code in the files. I will clean it up step by step.  

**MiSTer**  
On the MiSTer you need two scripts.  
The first script `S60tty2oled` is an starter script for the second script `tty2oled`  
which handles the communication to the Arduino via the USB-Serial tty device.  
  
**Demo Videos**  
Video Part 1: https://imgur.com/a/JOnaKPq  
Video Part 2: https://imgur.com/gallery/Ek9oFN1  
  
**How does it work?**  
  
**MiSTer**  
When the MiSTer boots up the Script `/etc/init.d/S60tty2oled` is called.  
This script does nothing more than calling another script `/usr/bin/tty2oled` and sent it to background.  
  
The script `tty2oled` checks at first the device `/dev/ttyUSB0`.  
If it's not found the scripts ends here.  
If it's found the tty-device parameter are set with the [`stty`](https://man7.org/linux/man-pages/man1/stty.1.html) command.  
Then a **first transmission** is send.  
During my tests I got somtimes weird startup behavior on power-ups with different lenghth of downtimes.  
Sometimes the text **Menu** was written instead of the MiSTer Picture shown.  
*Maybe some strange bits in the serial channel :smirk:*.  
After I added the **first transmission** the problem was gone.  
Then the main loop starts and checks the existence of the file `/tmp/CORENAME`.  
If this file doesn't exist the script waits for 2 secs and tried the next loop.  
If the file exists the content is read using `cat /tmp/CORENAME` to an variable and send to the tty Device.  
Now the script waits for 2 secs and runs the next loop.  
Every loop the Corename is checked **but the data are sent only if the Corename in** `/tmp/CORENAME` **has changed**.  
  
**Arduino**
The Arduino receives the transfered Corename.  
The Text or Picture on the Display is only updated if the received Corename has changed.  
  
If the Corename is within a list of known Names the correspondig picture is shown.  
![TGFX16](https://github.com/venice1200/MiSTer_tty2oled/blob/main/Pictures/TGFX16.jpg?raw=true)
  
If the Corename is unknown just the corename is shown.  
![Astrocade](https://github.com/venice1200/MiSTer_tty2oled/blob/main/Pictures/Astrocade.jpg?raw=true)
  
The used font for the Text can be chosen from a [list of fonts](https://github.com/olikraus/u8g2/wiki/fntlistall).  
  
The following Corenames are known and showing an dedicated or generic Picture:  
**Arcade:** llander, mooncrgx ,mpatrol  
**Computers:** AO486, APPLE-I, Apple-II, AtariST, C64, Minimig, PET2001, VIC20  
**Console:** ATARI2600, Genesis, NEOGEO, NES, SMS, SNES, TGFX16  
  
  
  
...work in progress  
  
