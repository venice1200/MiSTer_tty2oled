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

REPOSITORY_URL="https://raw.githubusercontent.com/venice1200/MiSTer_tty2oled/main"
#REPOSITORY_URL="https://raw.githubusercontent.com/venice1200/MiSTer_tty2oled/main/Testing"    # Testing branch

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

# The merge of the INI files (2022/02)
if [ "$(head -n1 ${TTY2OLED_PATH}/tty2oled-user.ini | awk '{print $2}')" = "Version" ]; then
  mv -f "${TTY2OLED_PATH}/tty2oled-user.ini" "${TTY2OLED_PATH}/tty2oled-user.ini.bak"
  touch "${TTY2OLED_PATH}/tty2oled-user.ini"
  echo -e "${fred} ${freset}"
  echo -e "${fred}We made very important changes to the INI files and merged them"
  echo -e "${fred}together. In case you missed the announcement, have a look at"
  echo -e "${fyellow}https://misterfpga.org/viewtopic.php?p=44358#p44358${freset}\n"
  echo -e "${fred}There is a copy of your old ${fyellow}${TTY2OLED_PATH}/tty2oled-user.ini"
  echo -e "${fred}named ${fyellow}${TTY2OLED_PATH}/tty2oled-user.ini.bak${fred} in case you want"
  echo -e "${fred}to review it. If you have edited some settings before this merge,"
  echo -e "${fred}take these changes to the new ${fyellow}${TTY2OLED_PATH}/tty2oled-user.ini"
  echo -e "${fred}and re-run this updater.${freset}\n"
  echo -e "${fred}This newly created tty2oled-user.ini will never be touched by"
  echo -e "${fred}tty2oled again. ${fwhite}You alone are responsible for this file in "
  echo -e "the future!${freset}"
  exit 1
fi

[ -f "${TTY2OLED_PATH}/tty2oled.ini" ] && mv "${TTY2OLED_PATH}/tty2oled.ini" "${TTY2OLED_PATH}/tty2oled.ini.bak"
[ -f "/media/fat/Scripts/tty2oled.ini" ] && mv "/media/fat/Scripts/tty2oled.ini" "${TTY2OLED_PATH}/tty2oled-user.ini.bak"

wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/update_tty2oled_script.sh" -O "${SCRIPTNAME}"
check4error "${?}"
[ -s "${SCRIPTNAME}" ] && bash "${SCRIPTNAME}" "${1}"
[ -f "${SCRIPTNAME}" ] && rm "${SCRIPTNAME}"

exit 0
