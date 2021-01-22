# MiSTer_tty2oled [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
Add-On for the [MiSTer](https://github.com/MiSTer-devel) showing Text or Pictures on a Display driven by an Arduino which is connected via USB-Serial.<br/>
<br/>
![tty2oled connection](https://github.com/venice1200/MiSTer_tty2oled/blob/main/Pictures/OLED_Connection.jpg?raw=true)
<br/>
<br/>
Files and Folders in this Repository<br/>

| File | Description |
| :--- | :--- |
| S60tty2oled | Starter Script, must be placed on **MiSTer** in folder **/etc/init.d/**  |
| tty2oled | Communications Script, must be placed on **MiSTer** in folder **/usr/bin/** |

| Folder | Description |
| :--- | :--- |
| MiSTer_SSD1322 | The Arduino Project |
| Pictures | Pictures :-) |

<br/>
At first I had the idea to add an Display only showing the MiSTer Logo.<br/>
After a bit of searching I found the possibilty to get information about the actual loaded Core out of the file /tmp/CORENAME.<br/>
I tried tail -F /tmp/CORENAME which results in error messages so I choose cat /tmp/CORENAME in a timed loop.<br/>
<br/>
I tried an USB-Serial connection between an Arduino and the MiSTer as this is an standard communication feature on Arduino's and easy to use.<br/>
And USB Ports are available for most of the MiSTer users is some way.<br/>
My ATMega 1284 with an FTDI Serial Adapter was sucessfully detected as **/dev/ttyUSB0** by the MiSTer.<br/>
Later I used an ESP32 with an CP Communication Chip because of storage and speed.<br/>
My actual Display is an SSD1322-OLED with 256x64 Pixel connected via SPI to the ESP32 MCU.<br/>
<br/><br/>
...work in progress<br/>
<br/>
