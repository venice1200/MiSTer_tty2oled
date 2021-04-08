#!/bin/bash

# v1.0 - Copyright (c) 2021 Oliver Jaksch, Lars Meuser

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# You can download the latest version of this script from:
# https://github.com/venice1200/MiSTer_tty2oled



# Changelog:
# v1.0	Main updater script which completes all tasks.



REPOSITORY_URL="https://raw.githubusercontent.com/venice1200/MiSTer_tty2oled/main"
TTY2OLED_PATH="/media/fat/tty2oledpics"
NODEBUG="-q"

echo -e "\n\e[1;32mtty2oled update script"
echo -e "----------------------\e[0m"

echo -e "\e[1;32mChecking for available updates...\e[0m"
[ "${1}" = "-f" ] && echo -e "\e[1;31m-Forced update-\e[0m"

# init script
wget ${NODEBUG} "${REPOSITORY_URL}/S60tty2oled" -O /tmp/S60tty2oled
if ! cmp -s /tmp/S60tty2oled /etc/init.d/S60tty2oled || [ "${1}" = "-f" ]; then
  echo -e "\e[1;33mUpdating init script \e[1;35mS60tty2oled\e[0m"
  mv -f /tmp/S60tty2oled /etc/init.d/S60tty2oled
  chmod +x /etc/init.d/S60tty2oled
fi

# daemon
wget ${NODEBUG} "${REPOSITORY_URL}/tty2oled" -O /tmp/tty2oled
if ! cmp -s /tmp/tty2oled /usr/bin/tty2oled || [ "${1}" = "-f" ]; then
  echo -e "\e[1;33mUpdating daemon \e[1;35mtty2oled\e[0m"
  mv -f /tmp/tty2oled /usr/bin/tty2oled
  chmod +x /usr/bin/tty2oled
fi

# pictures
[[ -d /media/fat/tty2oledpics ]] || mkdir -m 777 /media/fat/tty2oledpics
wget ${NODEBUG} "${REPOSITORY_URL}/Pictures/XBM_SD/sha1.txt" -O - | grep ".xbm" | \
  while read SHA1PIC; do
    PICNAME=$(echo ${SHA1PIC} | awk '{print $2}')
    CHKSUM1=$(echo ${SHA1PIC,,} | awk '{print $1}')
    CHKSUM2=$(sha1sum /media/fat/tty2oledpics/${PICNAME} | awk '{print $1}')
    if ! [ -f /media/fat/tty2oledpics/${PICNAME} ] || [ "${CHKSUM1}" != "${CHKSUM2}" ] || [ "${1}" = "-f" ]; then
      echo -e "\e[1;33mDownloading picture \e[1;35m${PICNAME}\e[0m"
      wget ${NODEBUG} "${REPOSITORY_URL}/Pictures/XBM_SD/${PICNAME}" -O /media/fat/tty2oledpics/${PICNAME}
    fi
  done

echo -e "\e[1;32m(Re-) starting init script\n\e[0m"
/etc/init.d/S60tty2oled restart

echo -e "\e[1;32mPress any key to continue\n\e[0m"
