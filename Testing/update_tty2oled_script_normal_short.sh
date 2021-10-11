#!/bin/bash

# v1.7 - Copyright (c) 2021 ojaksch, venice

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
# v1.7 Grayscale pictures
# v1.6 Move from Init based Startup to /media/fat/linux/user-startup.sh
# v1.5 Splitted script download into install and update using new Option "SCRIPT_UPDATE"
#      Check for disabled Init Script. If exists skip install.
# v1.4 New Option "USE_US_PICTURE"
# v1.3 More Text Output (Pictures)
# v1.2 New Option "USE_TEXT_PICTURE" & some Cosmetics
# v1.1 Use of an INI file (tty2oled.ini)
# v1.0 Main updater script which completes all tasks.


. /media/fat/tty2oled/tty2oled.ini

# Check and remount root writable if neccessary
if [ $(/bin/mount | head -n1 | grep -c "(ro,") = 1 ]; then
  /bin/mount -o remount,rw /
  MOUNTRO="true"
fi

# Check for and create tty2oled script folder
[[ -d ${TTY2OLED_PATH} ]] || mkdir ${TTY2OLED_PATH}

# Check for and delete old fashioned scripts to prefer /media/fat/linux/user-startup.sh
# (https://misterfpga.org/viewtopic.php?p=32159#p32159)
[[ -e /etc/init.d/S60tty2oled ]] && /etc/init.d/S60tty2oled stop && rm /etc/init.d/S60tty2oled
[[ -e /etc/init.d/_S60tty2oled ]] && rm /etc/init.d/_S60tty2oled
[[ -e /usr/bin/tty2oled ]] && rm /usr/bin/tty2oled

if [ ! -e ${USERSTARTUP} ] && [ -e /etc/init.d/S99user ]; then
  if [ -e ${USERSTARTUPTPL} ]; then
    echo "Copying ${USERSTARTUPTPL} to ${USERSTARTUP}"
    cp ${USERSTARTUPTPL} ${USERSTARTUP}
  else
    echo "Building ${USERSTARTUP}"
    echo -e "#!/bin/sh\n" > ${USERSTARTUP}
    echo -e 'echo "***" $1 "***"' >> ${USERSTARTUP}
  fi
fi
if [ $(grep -c "tty2oled" ${USERSTARTUP}) = "0" ]; then
  echo -e "Add tty2oled to ${USERSTARTUP}\n"
  echo -e "\n# Startup tty2oled" >> ${USERSTARTUP}
  echo -e "[[ -e ${INITSCRIPT} ]] && ${INITSCRIPT} \$1" >> ${USERSTARTUP}
fi

# Move old stuff to the new folder structure
if [ -d /media/fat/tty2oledpics/pri ]; then
  ! [[ -d ${picturefolder_pri} ]] && mkdir ${picturefolder_pri}
  mv /media/fat/tty2oledpics/pri/* ${picturefolder_pri}/
  rm -rf /media/fat/tty2oledpics/pri/
fi
if [ -d /media/fat/tty2oledpics ]; then
  ! [[ -d ${picturefolder} ]] && mkdir ${picturefolder}
  mv /media/fat/tty2oledpics/* ${picturefolder}/
  rm -rf /media/fat/tty2oledpics/
fi

echo -e "\e[1;32mtty2oled update script"
echo -e "----------------------\e[0m"

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
echo -e "\e[1;33mDownloading Pictures...\e[0m"
curl --progress-bar --location --continue-at - --fail --output ${TTY2OLED_PATH}/MiSTer_tty2oled_pictures.7z ${PICTURE_REPOSITORY_URL}

# Check and remount root non-writable if neccessary
[ "${MOUNTRO}" = "true" ] && /bin/mount -o remount,ro /

if [ $(pidof ${DAEMONNAME}) ]; then
  echo -e "\e[1;32mRestarting init script\n\e[0m"
  ${INITSCRIPT} restart
elif [ -c "${TTYDEV}" ]; then
  echo -e "\e[1;32mStarting init script\n\e[0m"
  ${INITSCRIPT} start
fi

[ -z "${SSH_TTY}" ] && echo -e "\e[1;32mPress any key to continue\n\e[0m"
