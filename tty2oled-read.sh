#!/bin/bash

. /media/fat/tty2oled/tty2oled-system.ini
. /media/fat/tty2oled/tty2oled-user.ini

if [[ -c ${TTYDEV} ]]; then
  cat ${TTYDEV} > /tmp/tty2oled-read & echo $! > /run/tty2oled-read.pid
fi

# kill $(cat /run/tty2oled-read.pid) && rm /run/tty2oled-read.pid && > /tmp/tty2oled-read
