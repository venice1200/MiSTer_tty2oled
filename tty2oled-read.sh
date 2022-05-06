#!/bin/bash

. /media/fat/tty2oled/tty2oled-system.ini
. /media/fat/tty2oled/tty2oled-user.ini

# [[ -c ${TTYDEV} ]] && cat ${TTYDEV} > /tmp/tty2oled-read
[[ -c ${TTYDEV} ]] && cat ${TTYDEV} > /tmp/tty2oled-read & echo $! > /run/tty2oled-read.pid
