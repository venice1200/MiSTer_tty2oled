#!/bin/bash

# v1.4 - Copyright (c) 2021 ojaksch, venice

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
# v1.4 Check downloadeds for errors, existence and filesize
# v1.3 Moved from /etc/init.d to /media/fat/tty2oled
# v1.2 New parameter in INI file, which selects the used tty2xxx device
# v1.1 Use of an INI file (tty2oled.ini)
# v1.0 Base updater script. Downloads and executes a second script (Main updater), which in turn completes all tasks.


#REPOSITORY_URL="https://raw.githubusercontent.com/venice1200/MiSTer_tty2oled/main"
#REPOSITORY_URL="https://raw.githubusercontent.com/venice1200/MiSTer_tty2oled/testing"    # Testing branch
REPOSITORY_URL="https://raw.githubusercontent.com/ojaksch/MiSTer_tty2oled/master"

SCRIPTNAME="/tmp/update_tty2oled_script.sh"
NODEBUG="-q -o /dev/null"

echo -e '\n +----------+';
echo -e ' | \e[1;34mtty2oled\e[0m |---[]';
echo -e ' +----------+\n';

check4error() {
  case "${1}" in
    0) ;;
    1) echo -e "\e[1;33mwget: \e[1;31mGeneric error code.\e[0m" ;;
    2) echo -e "\e[1;33mwget: \e[1;31mParse error---for instance, when parsing command-line options, the .wgetrc or .netrc..." ;;
    3) echo -e "\e[1;33mwget: \e[1;31mFile I/O error.\e[0m" ;;
    4) echo -e "\e[1;33mwget: \e[1;31mNetwork failure.\e[0m" ;;
    5) echo -e "\e[1;33mwget: \e[1;31mSSL verification failure.\e[0m" ;;
    6) echo -e "\e[1;33mwget: \e[1;31mUsername/password authentication failure.\e[0m" ;;
    7) echo -e "\e[1;33mwget: \e[1;31mProtocol errors.\e[0m" ;;
    8) echo -e "\e[1;33mwget: \e[1;31mServer issued an error response.\e[0m" ;;
    *) echo -e "\e[1;31mUnexpected and uncatched error.\e[0m" ;;
  esac
  ! [ "${1}" = "0" ] && exit "${1}"
}

# Update the updater if neccessary
wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/update_tty2oled.sh" -O /tmp/update_tty2oled.sh
check4error "${?}"
cmp -s /tmp/update_tty2oled.sh /media/fat/Scripts/update_tty2oled.sh
if [ "${?}" -gt "0" ] && [ -s /tmp/update_tty2oled.sh ]; then
    echo -e "\e[1;33mDownloading Updater-Update \e[1;35m${PICNAME}\e[0m"
    mv -f /tmp/update_tty2oled.sh /media/fat/Scripts/update_tty2oled.sh
    exec /media/fat/Scripts/update_tty2oled.sh
    exit 255
else
    rm /tmp/update_tty2oled.sh
fi

# Check and update INI file if neccessary
wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/tty2oled.ini" -O /tmp/tty2oled.ini
check4error "${?}"
if [ -s /tmp/tty2oled.ini ]; then
  . /tmp/tty2oled.ini
  [[ -d "${TTY2OLED_PATH}" ]] || mkdir "${TTY2OLED_PATH}"
  [[ -f /media/fat/Scripts/tty2oled.ini ]] && mv /media/fat/Scripts/tty2oled.ini "${TTY2OLED_PATH}/tty2oled.ini"
  if ! [ -f "${TTY2OLED_PATH}/tty2oled.ini" ]; then
    echo -e "\e[1;33mCreating tty2oled.ini File \e[1;35m${PICNAME}\e[0m"
    cp /tmp/tty2oled.ini "${TTY2OLED_PATH}/tty2oled.ini"
  fi
  if ! [[ "$(head -n1 /tmp/tty2oled.ini)" = "$(head -n1 ${TTY2OLED_PATH}/tty2oled.ini)" ]]; then
    echo -e "\e[1;31mThere is a newer version of \e[1;33m${TTY2OLED_PATH}/tty2oled.ini\e[1;31m availble.\e[0m"
    echo -e "\e[1;31mIt is very likely that something will break if we continue. You should backup\e[0m"
    echo -e "\e[1;31myour INI file and move or delete the original afterwards. After re-running\e[0m"
    echo -e "\e[1;31mthis updater and receiving the new INI file, compare both versions and edit\e[0m"
    echo -e "\e[1;31mthe new INI file, if necessary.\e[0m"
    echo -e "\n\e[1;35mIf you would like that we continue and automagically doing the rest,"
    echo -e "please answer YES\e[0m"
    read ANSWER
    if [ "${ANSWER}" = "YES" ]; then
      mv -f "${TTY2OLED_PATH}/tty2oled.ini" "${TTY2OLED_PATH}/tty2oled.ini.bak"
      mv -f /tmp/tty2oled.ini "${TTY2OLED_PATH}/tty2oled.ini"
      echo -e "\n\e[1;33mThese are the differences:\e[0m\n"
      diff -u "${TTY2OLED_PATH}/tty2oled.ini.bak" "${TTY2OLED_PATH}/tty2oled.ini"
      echo -e "\n\e[1;33mPlease edit the new INI file and make necessary changes,"
      echo -e "then re-run this updater.\e[0m\n"
    else
      echo -e "\n\e[1;33mAborting.\e[0m"
    fi
    exit 1
  fi
fi

wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/update_tty2oled_script.sh" -O "${SCRIPTNAME}"
check4error "${?}"
[ -s "${SCRIPTNAME}" ] && bash "${SCRIPTNAME}" "${1}"
[ -f "${SCRIPTNAME}" ] && rm "${SCRIPTNAME}"

exit 0
