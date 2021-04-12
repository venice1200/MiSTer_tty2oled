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
# v1.0 Base updater script. Downloads and executes a second script (Main updater), which in turn completes all tasks.


#REPOSITORY_URL="https://raw.githubusercontent.com/venice1200/MiSTer_tty2oled/main"
REPOSITORY_URL="https://raw.githubusercontent.com/ojaksch/MiSTer_tty2oled/master"
SCRIPTNAME="/tmp/update_tty2oled_script.sh"
NODEBUG="-q -o /dev/null"

if [ -f /media/fat/Scripts/tty2oled.ini ] && [ "${1}" = "-f" ]; then wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/tty2oled.ini" -O /media/fat/Scripts/tty2oled.ini; fi
! [[ -f /media/fat/Scripts/tty2oled.ini ]] && wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/tty2oled.ini" -O /media/fat/Scripts/tty2oled.ini

wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/update_tty2oled.sh" -O /tmp/update_tty2oled.sh
if ! cmp -s /tmp/update_tty2oled.sh /media/fat/Scripts/update_tty2oled.sh; then
    mv /tmp/update_tty2oled.sh /media/fat/Scripts/update_tty2oled.sh
    exec /media/fat/Scripts/update_tty2oled.sh
    exit 255
else
    rm /tmp/update_tty2oled.sh
fi

echo -e "\n\e[1;32mIf you want to FORCE an update, please re-run with parameter -f\e[0m"

wget ${NODEBUG} --no-cache "${REPOSITORY_URL}/update_tty2oled_script.sh" -O ${SCRIPTNAME}
case  ${?} in
    0) bash ${SCRIPTNAME} ${1} ;;
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

[[ -f ${SCRIPTNAME} ]] && rm ${SCRIPTNAME}

exit 0
