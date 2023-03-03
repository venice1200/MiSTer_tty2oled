#!/bin/bash

. /media/fat/tty2oled/tty2oled-system.ini
. /media/fat/tty2oled/tty2oled-user.ini

echo -e "Waiting for Internet access: "
for WAIT4NET in {1..50}; do ping -W1 -c1 192.168.9.9 >/dev/null 2>&1; echo -n "." ; sleep 0.2; done
bash ${UPDATESCRIPT} NOINSTALLER > /dev/null 2>&1	# Auto Update after 1 week
