#!/bin/bash

. /media/fat/tty2oled/tty2oled-system.ini
. /media/fat/tty2oled/tty2oled-user.ini

echo -e "Waiting for Internet access: "
until ping -W1 -c1 8.8.8.8 >/dev/null 2>&1; do echo -n "." ; sleep 0.2; done
bash ${UPDATESCRIPT} NOINSTALLER > /dev/null 2>&1	# Auto Update after 1 week
