#!/bin/bash

REPOSITORY_URL1="https://raw.githubusercontent.com/venice1200/MiSTer_tty2oled/main"
REPOSITORY_URL2="https://www.tty2tft.de//MiSTer_tty2oled-installer"
TMPDIR=$(mktemp -d)
cd ${TMPDIR}

# When started with parameter "T" use testing sketch
if [ "${1}" = "T" ]; then
    BUILDVER=$(wget -q ${REPOSITORY_URL2}/buildverT -O -)
else
    BUILDVER=$(wget -q ${REPOSITORY_URL2}/buildver -O -)
fi

# If there's an existing ini, use it
if [ -r /media/fat/tty2oled/tty2oled.ini ]; then
    . /media/fat/tty2oled/tty2oled.ini
else
    wget -q ${REPOSITORY_URL1}/tty2oled.ini -O ${TMPDIR}/tty2oled.ini
    . ${TMPDIR}/tty2oled.ini
fi

# If there is no existing path of tty2oled, create it
! [ -d ${TTY2OLED_PATH} ] && mkdir -p ${TTY2OLED_PATH}

# Clear the display by setting this as CORENAME, which keeps the display while updating
echo "cls" > /tmp/CORENAME

# Stop an already running daemon
if [ $(pidof ${DAEMONNAME}) ] && [ -f ${INITSCRIPT} ] ; then
    ${INITSCRIPT} stop
    INITSTOPPED="yes"
    sleep 0.5
fi

CRED="\e[1;31m"
CGRN="\e[1;32m"
CYEL="\e[1;33m"
CNON="\e[0m\033[0m"
CBLNK="\033[5m"
DBAUD="921600"
DSTD="--before default_reset --after hard_reset write_flash --compress --flash_size detect"

#Install pySerial (if it is missing)
if ! python -c "import serial" &> /dev/null; then
  ! [ -f /lib/python3.9/site-packages/pyserial-3.5-py3.9.egg ] && wget -q ${REPOSITORY_URL2}/pyserial-3.5-py3.9.egg -O /lib/python3.9/site-packages/pyserial-3.5-py3.9.egg
  echo "./pyserial-3.5-py3.9.egg" >> /lib/python3.9/site-packages/easy-install.pth
fi

#Install esptool (if it is missing)
if ! [ -f ${TMPDIR}/esptool.py ]; then
    wget -q ${REPOSITORY_URL2}/esptool.py -O ${TMPDIR}/esptool.py
    chmod +x ${TMPDIR}/esptool.py
fi

#Check if interface ttyUSB0 is present
echo -en "${CNON}Checking for device at ${TTYDEV}${CNON}: "
if [[ -c ${TTYDEV} ]]; then
    stty -F ${TTYDEV} ${BAUDRATE} ${TTYPARAM}
    echo -e "${CGRN}available${CNON}"
    echo -en "${CNON}Trying to identify device... ${CNON}"
    echo "CMDHWINF" > ${TTYDEV} ; read -t5 BLA < ${TTYDEV}
    MCUtype=${BLA:0:9}
    SWver=${BLA%;*} && SWver=${SWver:10}
    if [ "${MCUtype:0:2}" = "HW" ]; then
	echo -e "${CGRN}${MCUtype} with sketch version ${SWver}${CNON}"
    else
	echo -e "${CRED}Unknown MCU and/or not a tty2oled sketch. Please select.${CNON}" ; sleep 3
	exec 3>&1
	MCUtype=$(dialog --clear --no-cancel --ascii-lines --no-tags \
	--backtitle "tty2oled" --title "[ Flash tool for ESP devices ]" \
	--menu "Use the arrow keys and enter \nor the d-pad and A button" 0 0 0 \
	HWESP32DE "TTGO/DTI (ESP32)" \
	HWLOLIN32 "Wemos/Lolin/DevKit_V4 (ESP32)" \
	HWESP8266 "NodeMCU v3 (ESP8266)" \
	Exit "Exit now" 2>&1 1>&3)
	exec 3>&-;
	clear
	SWver="0"
    fi
else
    echo -e "${CRED}Could not connect to interface. Please check USB connection and run script again.${CNON}"
    exit 1
fi

#Check for MCU
case "${MCUtype}" in
    Exit)	exit 0 ;;
    HWNONEXXX)	echo -e "${CRED}Unknown hardware, can't continue.${CNON}" ; exit 1 ;;
    HWESP32DE)	echo -e "${CYEL}ESP32 selected/detected (TTGO/DTI).${CNON}" ;;
    HWLOLIN32)	echo -e "${CYEL}ESP32 selected/detected (Wemos/Lolin/DevKit_V4).${CNON}" ;;
    HWDTIPCB0)	echo -e "${CYEL}ESP32 selected/detected (DTI v1.0).${CNON}" ;;
    HWDTIPCB1)	echo -e "${CYEL}ESP32 selected/detected (DTI n/a).${CNON}" ;;
    HWESP8266)	echo -e "${CYEL}ESP8266 selected/detected.${CNON}" ;;
esac

if [[ "${SWver}" < "${BUILDVER}" ]]; then
    # Called by updater?
    if [ "${2}" = "UPDATER" ]; then
	echo -e "${CYEL}Version of your tty2oled device is ${SWver}, but BUILDVER is ${BUILDVER}.${CNON}"
	echo -en "${CYEL}Do you want to update (y/n)?${CNON} " ; read -t5 -n1 BLA
	echo
    fi
    # Not called by updater - or called by updater and answered YES
    if ! [ "${2}" = "UPDATER" ] || [ "${BLA}" = "y" ]; then
    echo "Updating tty2oled sketch" > /dev/ttyUSB0
    echo -e "${CYEL}Version of your tty2oled device is ${SWver}, but BUILDVER is ${BUILDVER}. Updating!${CNON}"
    echo "------------------------------------------------------------------------"
    case "${MCUtype}" in
	HWESP32DE)
	    wget -q ${REPOSITORY_URL2}/boot_app0.bin ${REPOSITORY_URL2}/bootloader_dio_80m.bin ${REPOSITORY_URL2}/partitions.bin ${REPOSITORY_URL2}/esp32de_${BUILDVER}.bin
	    ${TMPDIR}/esptool.py --chip esp32 --port ${TTYDEV} --baud ${DBAUD} ${DSTD} 0xe000 ${TMPDIR}/boot_app0.bin 0x1000 ${TMPDIR}/bootloader_dio_80m.bin 0x10000 ${TMPDIR}/esp32de_${BUILDVER}.bin 0x8000 ${TMPDIR}/partitions.bin
	    ;;
	HWLOLIN32 | HWDTIPCB0 | HWDTIPCB1)
	    wget -q ${REPOSITORY_URL2}/boot_app0.bin ${REPOSITORY_URL2}/bootloader_dio_80m.bin ${REPOSITORY_URL2}/partitions.bin ${REPOSITORY_URL2}/lolin32_${BUILDVER}.bin
	    ${TMPDIR}/esptool.py --chip esp32 --port ${TTYDEV} --baud ${DBAUD} ${DSTD} 0xe000 ${TMPDIR}/boot_app0.bin 0x1000 ${TMPDIR}/bootloader_dio_80m.bin 0x10000 ${TMPDIR}/lolin32_${BUILDVER}.bin 0x8000 ${TMPDIR}/partitions.bin
	    ;;
	HWESP8266)
	    wget -q ${REPOSITORY_URL2}/esp8266_${BUILDVER}.bin
	    ${TMPDIR}/esptool.py --chip esp8266 --port ${TTYDEV} --baud ${DBAUD} ${DSTD} 0x00000 ${TMPDIR}/esp8266_${BUILDVER}.bin
	    ;;
    esac
    echo "------------------------------------------------------------------------"
    echo -en "${CYEL}${CBLNK}...waiting for reboot of device...${CNON}" ; sleep 4 ; echo -e "\033[2K"
    stty -F ${TTYDEV} ${BAUDRATE} ${TTYPARAM} ; sleep 1
    fi

    # Called by updater?
    if ! [ "${2}" = "UPDATER" ]; then
	echo "Updating tty2oled software" > /dev/ttyUSB0
	echo -e "${CGRN}Downloading, checking and (maybe) installing and updating ${CYEL}tty2oled${CNON}"
	echo "MENU" > /tmp/CORENAME
    fi
    echo -e "\n${CGRN}Install/Update completed. Have fun!${CNON}"

elif [[ "${SWver}" > "${BUILDVER}" ]]; then
    echo -e "${CYEL}Your version ${SWver} is newer than the available stable build ${BUILDVER}!${CNON}"
elif [[ "${SWver}" = "${BUILDVER}" ]]; then
    echo -e "${CYEL}Good boy, you're hardware is up-to-date!${CNON}"
    [ "${INITSTOPPED}" = "yes" ] && ${INITSCRIPT} start
    sleep 0.5
    echo "MENU" > /tmp/CORENAME
fi

cd - ; rm -rf ${TMPDIR}
exit 0
# esptool.py --port ${TTYDEV} --baud 115200 erase_flash
# rm /lib/python3.9/site-packages/easy-install.pth /lib/python3.9/site-packages/pyserial-3.5-py3.9.egg
