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
# v1.1 Use of an INI file (tty2oled.ini)
# v1.0 Main updater script which completes all tasks.


. /media/fat/Scripts/tty2oled.ini

# Check and remount root writable if neccessary
if [ $(/bin/mount | head -n1 | grep -c "(ro,") = 1 ]; then
  /bin/mount -o remount,rw /
  MOUNTRO="true"
fi

echo -e "\n\e[1;32mtty2oled update script"
echo -e "----------------------\e[0m"

echo -e "\e[1;32mChecking for available updates...\e[0m"

# init script
wget ${NODEBUG} "${REPOSITORY_URL}/S60tty2oled" -O /tmp/S60tty2oled
if ! cmp -s /tmp/S60tty2oled ${INITSCRIPT}; then
  echo -e "\e[1;33mUpdating init script \e[1;35mS60tty2oled\e[0m"
  mv -f /tmp/S60tty2oled ${INITSCRIPT}
  chmod +x ${INITSCRIPT}
else
  rm /tmp/S60tty2oled
fi

# daemon
wget ${NODEBUG} "${REPOSITORY_URL}/tty2oled" -O /tmp/tty2oled
if ! cmp -s /tmp/tty2oled ${DAEMONSCRIPT}; then
  echo -e "\e[1;33mUpdating daemon \e[1;35mtty2oled\e[0m"
  mv -f /tmp/tty2oled ${DAEMONSCRIPT}
  chmod +x ${DAEMONSCRIPT}
else
  rm /tmp/tty2oled
fi

# pictures
[[ -d ${picturefolder} ]] || mkdir -m 777 ${picturefolder}
wget ${NODEBUG} "${REPOSITORY_URL}/Pictures/XBM_SD/sha1.txt" -O - | grep ".xbm" | \
  while read SHA1PIC; do
    PICNAME=$(echo ${SHA1PIC} | awk '{print $2}')
    CHKSUM1=$(echo ${SHA1PIC,,} | awk '{print $1}')
    [ -f ${picturefolder}/${PICNAME} ] && CHKSUM2=$(sha1sum ${picturefolder}/${PICNAME} | awk '{print $1}')
    if ! [ -f ${picturefolder}/${PICNAME} ] || ([ "${CHKSUM1}" != "${CHKSUM2}" ] && [ "${OVERWRITE}" = "yes" ]); then
      echo -e "\e[1;33mDownloading picture \e[1;35m${PICNAME}\e[0m"
      wget ${NODEBUG} "${REPOSITORY_URL}/Pictures/XBM_SD/${PICNAME}" -O ${picturefolder}/${PICNAME}
    fi
  done
sync

# Check and remount root non-writable if neccessary
[ "${MOUNTRO}" = "true" ] && /bin/mount -o remount,ro /

if [ $(pidof tty2oled) ]; then
  echo -e "\e[1;32m(Re-) starting init script\n\e[0m"
  ${INITSCRIPT} restart
fi

[ -z "${SSH_TTY}" ] && echo -e "\e[1;32mPress any key to continue\n\e[0m"
