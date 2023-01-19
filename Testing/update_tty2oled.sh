#!/bin/bash

# v1.9 - Copyright (c) 2021/2022 ojaksch, venice

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
# v1.9 Removed obsolete "INI merge" for very old setups
# v1.8 Create tty2oled-user.ini if it's missing to avoid ugly errors
# v1.7 Merging of tty2oled-user.ini to tty2oled-system.ini to leave a user-managed tty2oled-user.ini
# v1.5 Beautyfication
# v1.4 Check downloadeds for errors, existence and filesize
# v1.3 Moved from /etc/init.d to /media/fat/tty2oled
# v1.2 New parameter in INI file, which selects the used tty2xxx device
# v1.1 Use of an INI file (tty2oled.ini)
# v1.0 Base updater script. Downloads and executes a second script (Main updater), which in turn completes all tasks.

freset="\e[0m\033[0m"
fblue="\e[1;34m"

REPOSITORY_URL="https://raw.githubusercontent.com/venice1200/MiSTer_tty2oled/main"

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
[ -e /tmp/TTY2OLED_TESTING  ] && REPOSITORY_URL="https://raw.githubusercontent.com/venice1200/MiSTer_tty2oled/main/Testing"
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
! [ -e /media/fat/tty2oled/tty2oled-user.ini ] && touch /media/fat/tty2oled/tty2oled-user.ini
wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/tty2oled-system.ini" -O /tmp/tty2oled-system.ini
check4error "${?}"
. /tmp/tty2oled-system.ini
[[ -d "${TTY2OLED_PATH}" ]] || mkdir "${TTY2OLED_PATH}"
cmp -s /tmp/tty2oled-system.ini "${TTY2OLED_PATH}/tty2oled-system.ini"
if [ "${?}" -gt "0" ]; then
    mv /tmp/tty2oled-system.ini "${TTY2OLED_PATH}/tty2oled-system.ini"
    . "${TTY2OLED_PATH}/tty2oled-system.ini"
fi

wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/update_tty2oled_script.sh" -O "${SCRIPTNAME}"
check4error "${?}"
[ -s "${SCRIPTNAME}" ] && bash "${SCRIPTNAME}" "${@}"
[ -f "${SCRIPTNAME}" ] && rm "${SCRIPTNAME}"

date +%s > /media/fat/tty2oled/last_update
exit 0
