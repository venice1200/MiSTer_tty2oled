#!/bin/sh
. /media/fat/tty2oled/tty2oled.ini

esptool.py --chip esp32 --port ${TTYDEV} --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 boot_app0.bin 0x1000 MiSTer_SSD1322_USB.ino.bootloader.bin 0x10000 MiSTer_SSD1322_USB_Testing.ino.esp32.bin 0x8000 MiSTer_SSD1322_USB.ino.partitions.bin 
