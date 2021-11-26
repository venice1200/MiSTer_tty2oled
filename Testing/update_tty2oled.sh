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
# v1.5 Beautyfication
# v1.4 Check downloadeds for errors, existence and filesize
# v1.3 Moved from /etc/init.d to /media/fat/tty2oled
# v1.2 New parameter in INI file, which selects the used tty2xxx device
# v1.1 Use of an INI file (tty2oled.ini)
# v1.0 Base updater script. Downloads and executes a second script (Main updater), which in turn completes all tasks.

freset="\e[0m\033[0m"
fblue="\e[1;34m"

#REPOSITORY_URL="https://raw.githubusercontent.com/venice1200/MiSTer_tty2oled/main"
REPOSITORY_URL="https://raw.githubusercontent.com/venice1200/MiSTer_tty2oled/main/Testing"    # Testing branch

SCRIPTNAME="/tmp/update_tty2oled_script.sh"
NODEBUG="-q -o /dev/null"

echo -e "\n +----------+";
echo -e " | ${fblue}tty2oled${freset} |---[]";
echo -e " +----------+\n";

check4error() {
  case "${1}" in
    0) ;;
    1) echo -e "${fyellow}wget: ${fred}Generic error code.${freset}" ;;
    2) echo -e "${fyellow}wget: ${fred}Parse error---for instance, when parsing command-line options, the .wgetrc or .netrc..." ;;
    3) echo -e "${fyellow}wget: ${fred}File I/O error.${freset}" ;;
    4) echo -e "${fyellow}wget: ${fred}Network failure.${freset}" ;;
    5) echo -e "${fyellow}wget: ${fred}SSL verification failure.${freset}" ;;
    6) echo -e "${fyellow}wget: ${fred}Username/password authentication failure.${freset}" ;;
    7) echo -e "${fyellow}wget: ${fred}Protocol errors.${freset}" ;;
    8) echo -e "${fyellow}wget: ${fred}Server issued an error response.${freset}" ;;
    *) echo -e "${fred}Unexpected and uncatched error.${freset}" ;;
  esac
  ! [ "${1}" = "0" ] && exit "${1}"
}

# Update the updater if neccessary
wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/update_tty2oled.sh" -O /tmp/update_tty2oled.sh
check4error "${?}"
cmp -s /tmp/update_tty2oled.sh /media/fat/Scripts/update_tty2oled.sh
if [ "${?}" -gt "0" ] && [ -s /tmp/update_tty2oled.sh ]; then
    echo -e "${fyellow}Downloading Updater-Update ${fmagenta}${PICNAME}${freset}"
    mv -f /tmp/update_tty2oled.sh /media/fat/Scripts/update_tty2oled.sh
    exec /media/fat/Scripts/update_tty2oled.sh
    exit 255
else
    rm /tmp/update_tty2oled.sh
fi

# Check and update INI files if neccessary
wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/tty2oled-system.ini" -O /tmp/tty2oled-system.ini
check4error "${?}"
. /tmp/tty2oled-system.ini
[[ -d "${TTY2OLED_PATH}" ]] || mkdir "${TTY2OLED_PATH}"
cmp -s /tmp/tty2oled-system.ini "${TTY2OLED_PATH}/tty2oled-system.ini"
if [ "${?}" -gt "0" ]; then
    mv /tmp/tty2oled-system.ini "${TTY2OLED_PATH}/tty2oled-system.ini"
    . "${TTY2OLED_PATH}/tty2oled-system.ini"
fi

wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/tty2oled-user.ini" -O /tmp/tty2oled-user.ini
check4error "${?}"
if [ -s /tmp/tty2oled-user.ini ]; then
  . /tmp/tty2oled-user.ini
  [[ -f /media/fat/Scripts/tty2oled.ini ]] && mv /media/fat/Scripts/tty2oled.ini "${TTY2OLED_PATH}/tty2oled-user.ini"
  if ! [ -f "${TTY2OLED_PATH}/tty2oled-user.ini" ]; then
    echo -e "${fyellow}Creating tty2oled-user.ini File ${fmagenta}${PICNAME}${freset}"
    cp /tmp/tty2oled-user.ini "${TTY2OLED_PATH}/tty2oled-user.ini"
  fi
  if ! [[ "$(head -n1 /tmp/tty2oled-user.ini)" = "$(head -n1 ${TTY2OLED_PATH}/tty2oled-user.ini)" ]]; then
    echo -e "${fred}There is a newer version of ${fyellow}${TTY2OLED_PATH}/tty2oled-user.ini${fred} availble.${freset}"
    echo -e "${fred}It is very likely that something will break if we continue. You should backup${freset}"
    echo -e "${fred}your INI file and move or delete the original afterwards. After re-running${freset}"
    echo -e "${fred}this updater and receiving the new INI file, compare both versions and edit${freset}"
    echo -e "${fred}the new INI file, if necessary.${freset}"
    echo -e "\n${fmagenta}If you would like that we continue and automagically doing the rest,"
    echo -en "please answer YES. Use Cursor or Joystick for ${fgreen}YES=UP${freset} / ${fred}NO=DOWN${fyellow}. Countdown: 9${freset}"
    yesno 9
    if [ "${KEY}" = "y" ]; then
      mv -f "${TTY2OLED_PATH}/tty2oled-user.ini" "${TTY2OLED_PATH}/tty2oled-user.ini.bak"
      mv -f /tmp/tty2oled-user.ini "${TTY2OLED_PATH}/tty2oled-user.ini"
      echo -e "\n${fyellow}These are the differences:${freset}\n"
      diff -u "${TTY2OLED_PATH}/tty2oled-user.ini.bak" "${TTY2OLED_PATH}/tty2oled-user.ini"
      echo -e "\n${fyellow}Please edit the new INI file and make necessary changes,"
      echo -e "then re-run this updater.${freset}\n"
    else
      echo -e "\n${fyellow}Aborting.${freset}"
    fi
    exit 1
  fi
fi

wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/update_tty2oled_script.sh" -O "${SCRIPTNAME}"
check4error "${?}"
[ -s "${SCRIPTNAME}" ] && bash "${SCRIPTNAME}" "${1}"
[ -f "${SCRIPTNAME}" ] && rm "${SCRIPTNAME}"

exit 0
