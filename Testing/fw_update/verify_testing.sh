#!/bin/sh
. /media/fat/tty2oled/tty2oled.ini

esptool.py --chip esp32 --port ${TTYDEV} --baud 921600 verify_flash 0x10000 MiSTer_SSD1322_USB_Testing.ino.esp32.bin
