#!/bin/bash

# v2.1 - Copyright (c) 2022 ojaksch, venice

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
# v2.1 Removed obsolete "INI merge" for very old setups
# v2.0 Bugfixes, new daemon for feedback from ESP
# v1.9 Create tty2oled-user.ini if it's missing to avoid ugly errors
# v1.8 Beautyfication and Installer
# v1.7 Grayscale pictures and new download technics
# v1.6 Move from Init based Startup to /media/fat/linux/user-startup.sh
# v1.5 Splitted script download into install and update using new Option "SCRIPT_UPDATE"
#      Check for disabled Init Script. If exists skip install.
# v1.4 New Option "USE_US_PICTURE"
# v1.3 More Text Output (Pictures)
# v1.2 New Option "USE_TEXT_PICTURE" & some Cosmetics
# v1.1 Use of an INI file (tty2oled.ini)
# v1.0 Main updater script which completes all tasks.


! [ -e /media/fat/tty2oled/tty2oled-user.ini ] && touch /media/fat/tty2oled/tty2oled-user.ini
. /media/fat/tty2oled/tty2oled-system.ini
. /media/fat/tty2oled/tty2oled-user.ini

sendtext() {
  echo "${1}" > ${TTYDEV}
  sleep ${WAITSECS}
}

# Check for and create tty2oled script folder
[[ -d ${TTY2OLED_PATH} ]] && cd ${TTY2OLED_PATH} || mkdir ${TTY2OLED_PATH}

# Check and remount root writable if neccessary
if [ $(/bin/mount | head -n1 | grep -c "(ro,") = 1 ]; then
  /bin/mount -o remount,rw /
  MOUNTRO="true"
fi

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

# Get pv (Pipe Viewer) / rsyncy (progress bar for rsync)
#wget ${NODEBUG} -Nq "${PICTURE_REPOSITORY_URL}/MiSTer_tty2oled-installer/pv"
wget ${NODEBUG} -Nq "${PICTURE_REPOSITORY_URL}/MiSTer_tty2oled-installer/rsyncy.py"


echo -e "${fgreen}tty2oled update script"
echo -e "----------------------${freset}"
echo -e "${fgreen}Checking for available tty2oled updates...${freset}"


# init script
wget ${NODEBUG} "${REPOSITORY_URL}/S60tty2oled" -O /tmp/S60tty2oled
if  ! [ -f ${INITSCRIPT} ]; then
  if  [ -f ${INITDISABLED} ]; then
    echo -e "${fyellow}Found disabled init script, skipping Install${freset}"
  else
    echo -e "${fyellow}Installing init script ${fmagenta}S60tty2oled${freset}"
    [ -s /tmp/S60tty2oled ] && mv -f /tmp/S60tty2oled ${INITSCRIPT}
    chmod +x ${INITSCRIPT}
  fi
elif ! cmp -s /tmp/S60tty2oled ${INITSCRIPT}; then
  if [ "${SCRIPT_UPDATE}" = "yes" ]; then
    echo -e "${fyellow}Updating init script ${fmagenta}S60tty2oled${freset}"
    [ -s /tmp/S60tty2oled ] && mv -f /tmp/S60tty2oled ${INITSCRIPT}
    chmod +x ${INITSCRIPT}
  else
    echo -e "${fblink}Skipping${fyellow} available init script update because of the ${fcyan}SCRIPT_UPDATE${fyellow} INI-Option${freset}"
  fi
fi
[[ -f /tmp/S60tty2oled ]] && rm /tmp/S60tty2oled


# daemon
wget ${NODEBUG} "${REPOSITORY_URL}/${DAEMONNAME}" -O /tmp/${DAEMONNAME}
if  ! [ -f ${DAEMONSCRIPT} ]; then
  echo -e "${fyellow}Installing daemon script ${fmagenta}tty2oled${freset}"
  [ -s /tmp/${DAEMONNAME} ] && mv -f /tmp/${DAEMONNAME} ${DAEMONSCRIPT}
  chmod +x ${DAEMONSCRIPT}
elif ! cmp -s /tmp/${DAEMONNAME} ${DAEMONSCRIPT}; then
  if [ "${SCRIPT_UPDATE}" = "yes" ]; then
    echo -e "${fyellow}Updating daemon script ${fmagenta}tty2oled${freset}"
    [ -s /tmp/${DAEMONNAME} ] && mv -f /tmp/${DAEMONNAME} ${DAEMONSCRIPT}
    chmod +x ${DAEMONSCRIPT}
  else
    echo -e "${fblink}Skipping${fyellow} available daemon script update because of the ${fcyan}SCRIPT_UPDATE${fyellow} INI-Option${freset}"
  fi
fi
[[ -f /tmp/${DAEMONNAME} ]] && rm /tmp/${DAEMONNAME}

# pictures
if ! [ -d ${picturefolder}/GSC ];then
  if [ -d ${picturefolder} ];then
    mv "${picturefolder}" "${picturefolder}.old"	# ExFAT bug?
    rm -rf "${picturefolder}.old"
    sync
  fi
  ! [ -d ${picturefolder} ] && mkdir -p ${picturefolder}
  echo -e "${fyellow}Downloading Picture Archive (initial)...${freset}"
  wget -qN --show-progress --ca-certificate=/etc/ssl/certs/cacert.pem ${PICTURE_REPOSITORY_URL}/MiSTer_tty2oled_pictures.7z -O /tmp/MiSTer_tty2oled_pictures.7z
  echo -e "${fyellow}Decompressing Pictures Archive...${freset}"
  7zr x -bsp0 -bso0 /tmp/MiSTer_tty2oled_pictures.7z -o${picturefolder}
  rm /tmp/MiSTer_tty2oled_pictures.7z
else
  echo -e "${fyellow}Downloading Pictures...${freset}"
  [ "${OVERWRITE_PICTURE}" = "no" ] && RSYNCOPTS="--ignore-existing" || RSYNCOPTS="--delete"
  ${TTY2OLED_PATH}/rsyncy.py -crlzzP --modify-window=1 ${RSYNCOPTS} rsync://tty2oled-update-daemon@tty2tft.de/tty2oled-pictures/ ${picturefolder}/
fi

# Download tty2oled Utilities
wget ${NODEBUG} "${REPOSITORY_URL}/tty2oled_cc.sh" -O /tmp/tty2oled_cc.sh
if ! cmp -s /tmp/tty2oled_cc.sh ${CCSCRIPT}; then
  if [ "${SCRIPT_UPDATE}" = "yes" ]; then
    echo -e "${fyellow}Updating tools script ${fmagenta}tty2oled_cc.sh${freset}"
    [ -s /tmp/tty2oled_cc.sh ] && mv -f /tmp/tty2oled_cc.sh ${CCSCRIPT}
    chmod +x ${CCSCRIPT}
  else
    echo -e "${fblink}Skipping${fyellow} available tools script update because of the ${fcyan}SCRIPT_UPDATE${fyellow} INI-Option${freset}"
  fi
fi

# Download Read/Buffer Daemon
wget ${NODEBUG} -Nq "${REPOSITORY_URL}/tty2oled-read.sh"

# Download the installer to check esp firmware
if [ "${1}" != "NOINSTALLER" ]; then
  cd /tmp
  [ "${TTY2OLED_UPDATE}" = "yes" ] && bash <(wget -qO- ${REPOSITORY_URL}/installer.sh) UPDATER
elif [ "${1}" = "NOINSTALLER" ]; then
  sendtext "CMDHWINF" ; read -t5 HWINF < ${TTYDEV}
  LBUILDVER=$(echo ${HWINF} | cut -d ";" -f 2)
  [ "${TTY2OLED_FW_TESTING}" = "yes" ] && BUILDVER=$(wget -q ${REPOSITORY_URL2}/buildverT -O -) || BUILDVER=$(wget -q ${REPOSITORY_URL2}/buildver -O -)
  echo ${LBUILDVER} ${BUILDVER}
  if [ ${LBUILDVER} -lt ${BUILDVER} ]; then
    sendtext "CMDCLS"
    sendtext "CMDTXT,1,15,0,40,20,Firmware Update Available!"
    sendtext "CMDTXT,1,15,0,30,40,You: ${LBUILDVER} / Server: ${BUILDVER}"
    sendtext "CMDTXT,1,15,0,1,60,Please run update_tty2oled.sh on MiSTer"
    for FLASH in {1..5}; do sendtext "CMDDOFF"; sendtext "CMDDON"; done
    sleep 5
  fi
fi

# Check and remount root non-writable if neccessary
[ "${MOUNTRO}" = "true" ] && /bin/mount -o remount,ro /

if [ "${1}" != "NOINSTALLER" ]; then
  if [ $(pidof ${DAEMONNAME}) ]; then
    echo -e "${fgreen}Restarting init script\n${freset}"
    ${INITSCRIPT} restart
  elif [ -c "${TTYDEV}" ]; then
    echo -e "${fgreen}Starting init script\n${freset}"
    ${INITSCRIPT} start
  fi
fi

[ -z "${SSH_TTY}" ] && echo -e "${fgreen}Press any key to continue\n${freset}"
