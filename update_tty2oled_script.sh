#!/bin/bash

# v1.5 - Copyright (c) 2021 ojaksch, venice

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
# v1.5 Splitted script download into install and update using new Option "SCRIPT_UPDATE"
#      Check for disabled Init Script. If exists skip install.
# v1.4 New Option "USE_US_PICTURE"
# v1.3 More Text Output (Pictures)
# v1.2 New Option "USE_TEXT_PICTURE" & some Cosmetics
# v1.1 Use of an INI file (tty2oled.ini)
# v1.0 Main updater script which completes all tasks.


. /media/fat/Scripts/tty2oled.ini

# Check and remount root writable if neccessary
if [ $(/bin/mount | head -n1 | grep -c "(ro,") = 1 ]; then
  /bin/mount -o remount,rw /
  MOUNTRO="true"
fi

# Currently disabled, delete old Work-Folders, move Deamon
#[[ -v oldpicturefolder ]] && [[ -d ${oldpicturefolder} ]] && mv ${oldpicturefolder}/* ${picturefolder} && rm -R ${oldpicturefolder}
#[[ -v OLDDAEMONSCRIPT ]] && [[ -e ${OLDDAEMONSCRIPT} ]] && mv ${OLDDAEMONSCRIPT} ${DAEMONSCRIPT} 

# Check for and create tty2oled script folder
[[ -d /media/fat/Scripts/tty2oled ]] || mkdir /media/fat/Scripts/tty2oled

# Check for and delete old fashioned scripts to prefer /media/fat/linux/user-startup.sh
# (https://misterfpga.org/viewtopic.php?p=32159#p32159)
[[ -e /etc/init.d/S60tty2oled ]] && rm /etc/init.d/S60tty2oled
[[ -e /etc/init.d/_S60tty2oled ]] && rm /etc/init.d/_S60tty2oled
[[ -e /usr/bin/tty2oled ]] && rm /usr/bin/tty2oled
if [ $(grep -c "tty2oled" /media/fat/linux/user-startup.sh) = "0" ]; then
  echo "${INITSCRIPT} \$1" >> /media/fat/linux/user-startup.sh
fi

echo -e '\n +----------+';
echo -e ' | \e[1;34mtty2oled\e[0m |---[]';
echo -e ' +----------+\n';
echo -e "\e[1;32m Update Script"
echo -e "---------------\e[0m"

#echo -e "\n\e[1;34mtty\e[1;31m2\e[1;33moled\e[1;32m update script"
#echo -e "----------------------\e[0m"

echo -e "\e[1;32mChecking for available tty2oled updates...\e[0m"

# init script
wget ${NODEBUG} "${REPOSITORY_URL}/S60tty2oled" -O /tmp/S60tty2oled
if  ! [ -f ${INITSCRIPT} ]; then
  if  [ -f ${INITDISABLED} ]; then
    echo -e "\e[1;33mFound disabled init script, skipping Install\e[0m"
  else
    echo -e "\e[1;33mInstalling init script \e[1;35mS60tty2oled\e[0m"
    mv -f /tmp/S60tty2oled ${INITSCRIPT}
    chmod +x ${INITSCRIPT}
  fi
elif ! cmp -s /tmp/S60tty2oled ${INITSCRIPT}; then
  if [ "${SCRIPT_UPDATE}" = "yes" ]; then
    echo -e "\e[1;33mUpdating init script \e[1;35mS60tty2oled\e[0m"
    mv -f /tmp/S60tty2oled ${INITSCRIPT}
    chmod +x ${INITSCRIPT}
  else
    echo -e "\e[5;31mSkipping\e[25;1;33m available init script update because of the \e[1;36mSCRIPT_UPDATE\e[1;33m INI-Option\e[0m"
  fi
fi
[[ -f /tmp/S60tty2oled ]] && rm /tmp/S60tty2oled


# daemon
wget ${NODEBUG} "${REPOSITORY_URL}/${DAEMONNAME}" -O /tmp/${DAEMONNAME}
if  ! [ -f ${DAEMONSCRIPT} ]; then
  echo -e "\e[1;33mInstalling daemon script \e[1;35mtty2oled\e[0m"
  mv -f /tmp/${DAEMONNAME} ${DAEMONSCRIPT}
  chmod +x ${DAEMONSCRIPT}
elif ! cmp -s /tmp/${DAEMONNAME} ${DAEMONSCRIPT}; then
  if [ "${SCRIPT_UPDATE}" = "yes" ]; then
    echo -e "\e[1;33mUpdating daemon script \e[1;35mtty2oled\e[0m"
    mv -f /tmp/${DAEMONNAME} ${DAEMONSCRIPT}
    chmod +x ${DAEMONSCRIPT}
  else
    echo -e "\e[5;31mSkipping\e[25;1;33m available daemon script update because of the \e[1;36mSCRIPT_UPDATE\e[1;33m INI-Option\e[0m"
  fi
fi
[[ -f /tmp/${DAEMONNAME} ]] && rm /tmp/${DAEMONNAME}

# pictures
if [ "${USBMODE}" = "yes" ]; then
  [[ -d ${picturefolder} ]] || mkdir -p -m 777 ${picturefolder}
  [[ -d ${picturefolder_pri} ]] || mkdir -p -m 777 ${picturefolder_pri}
  # Text-Based Pictures download
  if [ "${USE_TEXT_PICTURE}" = "yes" ]; then
    echo -e "\e[1;32mChecking for available Text-Pictures...\e[0m"
    wget ${NODEBUG} "${REPOSITORY_URL}/Pictures/XBM_Text/sha1.txt" -O - | grep ".xbm" | dos2unix | \
    while read SHA1PIC; do
      PICNAME=$(echo ${SHA1PIC} | cut -d " " -f 2-)
      CHKSUM1=$(echo ${SHA1PIC,,} | cut -d " " -f 1)
      [ -f "${picturefolder}/${PICNAME}" ] && CHKSUM2=$(sha1sum ${picturefolder}/${PICNAME} | awk '{print $1}')
      if ! [ -f "${picturefolder}/${PICNAME}" ] || ([ "${CHKSUM1}" != "${CHKSUM2}" ] && [ "${OVERWRITE_PICTURE}" = "yes" ]); then
        echo -e "\e[1;33mDownloading Picture \e[1;35m${PICNAME}\e[0m"
        wget ${NODEBUG} "${REPOSITORY_URL}/Pictures/XBM_Text/${PICNAME}" -O "${picturefolder}/${PICNAME}"
      fi
    done
  else
    echo -e "\e[1;31mSkipping\e[1;33m Text-Based Picture download because of the \e[1;36mUSE_TEXT_PICTURE\e[1;33m INI-Option\e[0m"
  fi

  # Graphic-Based Pictures (as Second = Higher Priority)
  echo -e "\e[1;32mChecking for available Graphic-Pictures...\e[0m"
  wget ${NODEBUG} "${REPOSITORY_URL}/Pictures/XBM/sha1.txt" -O - | grep ".xbm" | dos2unix | \
  while read SHA1PIC; do
    PICNAME=$(echo ${SHA1PIC} | cut -d " " -f 2-)
    CHKSUM1=$(echo ${SHA1PIC,,} | cut -d " " -f 1)
    [ -f "${picturefolder}/${PICNAME}" ] && CHKSUM2=$(sha1sum "${picturefolder}/${PICNAME}" | awk '{print $1}')
    if ! [ -f "${picturefolder}/${PICNAME}" ] || ([ "${CHKSUM1}" != "${CHKSUM2}" ] && [ "${OVERWRITE_PICTURE}" = "yes" ]); then
      echo -e "\e[1;33mDownloading Picture \e[1;35m${PICNAME}\e[0m"
      wget ${NODEBUG} "${REPOSITORY_URL}/Pictures/XBM/${PICNAME}" -O "${picturefolder}/${PICNAME}"
    fi
  done

  # Checking for US version of Graphic-Based Pictures (Genesis = MegaDrive ; Sega CD = Mega CD ; TurboGrafx16 = PCEngine)
  if [ "${USE_US_PICTURE}" = "yes" ]; then 
    echo -e "\e[1;32mChecking for available Graphic-Pictures US-Version...\e[0m"
    wget ${NODEBUG} "${REPOSITORY_URL}/Pictures/XBM_US/sha1.txt" -O - | grep ".xbm" | dos2unix | \
    while read SHA1PIC; do
      PICNAME=$(echo ${SHA1PIC} | cut -d " " -f 2-)
      CHKSUM1=$(echo ${SHA1PIC,,} | cut -d " " -f 1)
      [ -f "${picturefolder}/${PICNAME}" ] && CHKSUM2=$(sha1sum ${picturefolder}/${PICNAME} | awk '{print $1}')
      if ! [ -f "${picturefolder}/${PICNAME}" ] || ([ "${CHKSUM1}" != "${CHKSUM2}" ] && [ "${OVERWRITE_PICTURE}" = "yes" ]); then
        echo -e "\e[1;33mDownloading Picture \e[1;35m${PICNAME}\e[0m"
        wget ${NODEBUG} "${REPOSITORY_URL}/Pictures/XBM_US/${PICNAME}" -O "${picturefolder}/${PICNAME}"
      fi
    done
  else
    echo -e "\e[1;31mSkipping\e[1;33m US-Version Picture download because of the \e[1;36mUSE_US_PICTURE\e[1;33m INI-Option\e[0m"
  fi
else
  echo -e "\e[5;31mSkipping\e[25;1;33m Picture download because of the \e[1;36mUSBMODE\e[1;33m INI-Option\e[0m"
fi

sync

# Check and remount root non-writable if neccessary
[ "${MOUNTRO}" = "true" ] && /bin/mount -o remount,ro /

if [ $(pidof tty2oled) ]; then
  echo -e "\e[1;32m(Re-) starting init script\n\e[0m"
  ${INITSCRIPT} restart
elif [ -c "${TTYDEV}" ]; then
  echo -e "\e[1;32mStarting init script\n\e[0m"
  ${INITSCRIPT} start
fi

[ -z "${SSH_TTY}" ] && echo -e "\e[1;32mPress any key to continue\n\e[0m"
