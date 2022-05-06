#!/bin/bash

. /media/fat/tty2oled/tty2oled-system.ini
. /media/fat/tty2oled/tty2oled-user.ini

[[ -c ${TTYDEV} ]] && cat ${TTYDEV} > /tmp/tty2oled-read
