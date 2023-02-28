#!/bin/bash

REPOSITORY_URL="https://raw.githubusercontent.com/venice1200/MiSTer_tty2oled/main"
REPOSITORY_URL2="https://www.tty2tft.de//MiSTer_tty2oled-installer"
[ -e /media/fat/tty2oled/TTY2OLED_TESTING ] && REPOSITORY_URL="https://raw.githubusercontent.com/venice1200/MiSTer_tty2oled/main/Testing"
DBAUD="921600"
DSTD="--before default_reset --after hard_reset write_flash --compress --flash_mode dio --flash_freq 80m --flash_size detect"
TMPDIR=$(mktemp -d)
MCUtype="${2,,}"
cd ${TMPDIR}

flash() {
    echo "------------------------------------------------------------------------"
    case "${MCUtype}" in
	hwesp32de | hwlolin32)
	    BOOTSWITCH="0xe000 ${TMPDIR}/boot_app0.bin"
	    BOOTLOADER="0x1000 ${TMPDIR}/bootloader_esp32_dio_80m.bin"
	    PARTITION="0x8000 ${TMPDIR}/partitions.bin"
	    APP="0x10000 ${TMPDIR}/${MCUtype:2}_${BUILDVER}.bin"
	    wget -q ${REPOSITORY_URL2}/MAC.html?${MAC} ${REPOSITORY_URL2}/${BOOTSWITCH##*/} ${REPOSITORY_URL2}/${BOOTLOADER##*/} ${REPOSITORY_URL2}/${PARTITION##*/} ${REPOSITORY_URL2}/${APP##*/}
	    ${TMPDIR}/esptool.py --chip esp32 --port ${TTYDEV} --baud ${DBAUD} ${DSTD} ${BOOTSWITCH} ${BOOTLOADER} ${PARTITION} ${APP}
	    ;;
	hwesp32s3)
	    BOOTSWITCH="0xe000 ${TMPDIR}/boot_app0.bin"
	    BOOTLOADER="0x0 ${TMPDIR}/bootloader_esp32s3_dio_80m.bin"
	    PARTITION="0x8000 ${TMPDIR}/partitions.bin"
	    APP="0x10000 ${TMPDIR}/${MCUtype:2}_${BUILDVER}.bin"
	    wget -q ${REPOSITORY_URL2}/MAC.html?${MAC} ${REPOSITORY_URL2}/${BOOTSWITCH##*/} ${REPOSITORY_URL2}/${BOOTLOADER##*/} ${REPOSITORY_URL2}/${PARTITION##*/} ${REPOSITORY_URL2}/${APP##*/}
	    ${TMPDIR}/esptool.py --chip esp32s3 --port ${TTYDEV} --baud ${DBAUD} ${DSTD} ${BOOTSWITCH} ${BOOTLOADER} ${PARTITION} ${APP}
	    ;;
	hwesp8266)
	    wget -q ${REPOSITORY_URL2}/MAC.html?${MAC} ${REPOSITORY_URL2}/esp8266_${BUILDVER}.bin
	    ${TMPDIR}/esptool.py --chip esp8266 --port ${TTYDEV} --baud ${DBAUD} ${DSTD} 0x0 ${TMPDIR}/esp8266_${BUILDVER}.bin
	    ;;
    esac
    echo "------------------------------------------------------------------------"
    echo -en "${fyellow}${fblink}...waiting for reboot of device...${freset}" ; sleep 4 ; echo -e "\033[2K"
    stty -F ${TTYDEV} ${BAUDRATE} ${TTYPARAM} ; sleep 1
    echo -e "\n${fgreen}Flash progress completed. Have fun!${freset}"
}

checkesp() {
  MAC=$(${TMPDIR}/esptool.py --chip auto --port ${TTYDEV} --baud ${DBAUD} flash_id | grep MAC | awk '{print $2}')
  MAC=${MAC^^}
}

# If there's an existing ini, use it
if [ -r /media/fat/tty2oled/tty2oled-system.ini ]; then
    . /media/fat/tty2oled/tty2oled-system.ini
else
    wget -q ${REPOSITORY_URL}/tty2oled-system.ini -O ${TMPDIR}/tty2oled-system.ini
    . ${TMPDIR}/tty2oled-system.ini
fi
if [ -r /media/fat/tty2oled/tty2oled-user.ini ]; then
    . /media/fat/tty2oled/tty2oled-user.ini
else
    wget -q ${REPOSITORY_URL}/tty2oled-user.ini -O ${TMPDIR}/tty2oled-user.ini
    . ${TMPDIR}/tty2oled-user.ini
fi

# When started with parameter "T" use testing sketch
[ "${TTY2OLED_FW_TESTING}" = "yes" ] && BUILDVER=$(wget -q ${REPOSITORY_URL2}/buildverT -O -) || BUILDVER=$(wget -q ${REPOSITORY_URL2}/buildver -O -)

# Stop an already running daemon
if [ $(pidof ${DAEMONNAME}) ] && [ -f ${INITSCRIPT} ] ; then
    ${INITSCRIPT} stop
    INITSTOPPED="yes"
    sleep 0.5
fi

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
if [ "${MCUtype}" = "" ]; then
    echo -en "${freset}Checking for device at ${TTYDEV}${freset}: "
    if [[ -c ${TTYDEV} ]]; then
	stty -F ${TTYDEV} ${BAUDRATE} ${TTYPARAM}
	echo -e "${fgreen}available${freset}"
	echo -en "${freset}Trying to identify device... ${freset}"
	echo "CMDHWINF" > ${TTYDEV} ; read -t5 HWINF < ${TTYDEV}
	MCUtype=${HWINF:0:9}
	SWver=${HWINF%;*} && SWver=${SWver:10}
	checkesp ; sleep 2.5
	if [ "${MCUtype:0:2}" = "HW" ]; then
	    echo -e "${fgreen}${MCUtype} with sketch version ${SWver}${freset}"
	else
	    echo -e "${fred}Unknown MCU and/or not a tty2oled sketch. Please select.${freset}" ; sleep 3
	    exec 3>&1
	    MCUtype=$(dialog --clear --no-cancel --ascii-lines --no-tags \
	    --backtitle "tty2oled" --title "[ Flash tool for ESP devices ]" \
	    --menu "Use the arrow keys and enter \nor the d-pad and A button" 0 0 0 \
	    hwesp32de "TTGO/DTI (ESP32)" \
	    hwesp32s3 "ESP32-S3 DevKitC (ESP32-S3)" \
	    hwlolin32 "Wemos/Lolin/DevKit_V4 (ESP32)" \
	    hwesp8266 "NodeMCU v3 (ESP8266)" \
	    Exit "Exit now" 2>&1 1>&3)
	    exec 3>&-;
	    clear
	    SWver="0"
	fi
    else
	echo -e "${fred}Could not connect to interface. Please check USB connection and run script again.${freset}"
	exit 1
    fi
fi

MCUtype=${MCUtype,,}

#Check for MCU
case "${MCUtype}" in
    Exit)	exit 0 ;;
    hwnonexxx)	echo -e "${fred}Unknown hardware, can't continue.${freset}" ; exit 1 ;;
    hwesp32de)	echo -e "${fyellow}ESP32 selected/detected (TTGO/DTI).${freset}" ;;
    hwesp32s3)	echo -e "${fyellow}ESP32-S3 selected/detected (ESP32-S3 DevKitC/DTI).${freset}" ;;
    hwlolin32)	echo -e "${fyellow}ESP32 selected/detected (Wemos/Lolin/DevKit_V4).${freset}" ;;
    hwesp8266)	echo -e "${fyellow}ESP8266 selected/detected.${freset}" ;;
esac

if [ "${1}" = "FORCE" ]; then
    echo -e "${fred}FORCED UPDATE${freset}"
    echo -e "${fyellow}Version of your tty2oled device is ${fblue}${SWver}${fyellow}, forced BUILDVER is ${fgreen}${BUILDVER}${fyellow}.${freset}"
    echo -e "${fyellow}MCUtype is set to ${fblue}${MCUtype}${freset}"
    flash
    [ "${INITSTOPPED}" = "yes" ] && ${INITSCRIPT} start
else
    if [[ "${SWver}" < "${BUILDVER}" ]]; then
	echo -e "${fyellow}Version of your tty2oled device is ${fblue}${SWver}${fyellow}, but BUILDVER is ${fgreen}${BUILDVER}${fyellow}.${freset}"
	echo -en "${fyellow}Do you want to Update? Use Cursor or Joystick for ${fgreen}YES=UP${freset} / ${fred}NO=DOWN${fyellow}. Countdown: 9${freset}"
	yesno 9
	if [ "${KEY}" = "y" ]; then
	    echo "Updating tty2oled" > ${TTYDEV}
	    flash
	fi
    elif [[ "${SWver}" > "${BUILDVER}" ]]; then
	if [ "${SWver: -1}" = "T" ]; then
	    echo -e "${fyellow}Your version ${fblue}${SWver}${fyellow} is newer than the available stable build ${fgreen}${BUILDVER}${fyellow}!${freset}"
	    echo -en "${fyellow}Do you want to Downgrade? Use Cursor or Joystick for ${fgreen}YES=UP${freset} / ${fred}NO=DOWN${fyellow}. Countdown: 9${freset}"
	    yesno 9
	    if [ "${KEY}" = "y" ]; then
		echo "Downgrading tty2oled" > ${TTYDEV}
		flash
	    fi
	fi
	[ "${INITSTOPPED}" = "yes" ] && ! [ "${1}" = "UPDATER" ] && ${INITSCRIPT} start
    elif [[ "${SWver}" = "${BUILDVER}" ]]; then
	echo -e "${fyellow}Good job, your hardware is up-to-date!${freset}"
	[ "${INITSTOPPED}" = "yes" ] && ! [ "${1}" = "UPDATER" ] && ${INITSCRIPT} start
    fi
fi

rm -rf ${TMPDIR}
exit 0
