#!/bin/bash

REPOSITORY_URL1="https://raw.githubusercontent.com/venice1200/MiSTer_tty2oled/main"
REPOSITORY_URL2="https://www.tty2tft.de//MiSTer_tty2oled-installer"
BUILDVER=$(wget -q ${REPOSITORY_URL2}/buildver -O -)
TMPDIR=$(mktemp -d)
cd ${TMPDIR}
wget -q ${REPOSITORY_URL1}/tty2oled.ini -O ${TMPDIR}/tty2oled.ini
. ${TMPDIR}/tty2oled.ini

mkdir -p ${TTY2OLED_PATH}
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
DUSB="/dev/ttyUSB0"
DBAUD="921600"
#DSTD="--before default_reset --after hard_reset write_flash --compress --flash_mode dio --flash_freq 80m --flash_size detect"
DSTD="--before default_reset --after hard_reset write_flash --compress --flash_size detect"
echo "cls" > /tmp/CORENAME

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
echo -en "${CNON}Checking for device at ${DUSB}${CNON}: "
if [[ -c ${DUSB} ]]; then
    stty -F ${DUSB} ${TTYPARAM}
    echo -e "${CGRN}available${CNON}"
    echo -en "${CNON}Trying to identify device... ${CNON}"
    echo "CMDHWINF" > ${DUSB} ; read -t5 BLA < ${DUSB}
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
    echo "Updating tty2oled sketch" > /dev/ttyUSB0
    echo -e "${CYEL}Version of your tty2oled device is ${SWver}, but BUILDVER is ${BUILDVER}. Updating!${CNON}"
    echo "------------------------------------------------------------------------"
    case "${MCUtype}" in
	HWESP32DE)
	    wget -q ${REPOSITORY_URL2}/boot_app0.bin ${REPOSITORY_URL2}/bootloader_qio_80m.bin ${REPOSITORY_URL2}/partitions.bin ${REPOSITORY_URL2}/esp32de_${BUILDVER}.bin
	    ${TMPDIR}/esptool.py --chip esp32 --port ${DUSB} --baud ${DBAUD} ${DSTD} 0xe000 ${TMPDIR}/boot_app0.bin 0x1000 ${TMPDIR}/bootloader_qio_80m.bin 0x10000 ${TMPDIR}/esp32de_${BUILDVER}.bin 0x8000 ${TMPDIR}/partitions.bin
	    ;;
	HWLOLIN32 | HWDTIPCB0 | HWDTIPCB1)
	    wget -q ${REPOSITORY_URL2}/boot_app0.bin ${REPOSITORY_URL2}/bootloader_dio_80m.bin ${REPOSITORY_URL2}/partitions.bin ${REPOSITORY_URL2}/lolin32_${BUILDVER}.bin
	    ${TMPDIR}/esptool.py --chip esp32 --port ${DUSB} --baud ${DBAUD} ${DSTD} 0xe000 ${TMPDIR}/boot_app0.bin 0x1000 ${TMPDIR}/bootloader_dio_80m.bin 0x10000 ${TMPDIR}/lolin32_${BUILDVER}.bin 0x8000 ${TMPDIR}/partitions.bin
	    ;;
	HWESP8266)
	    wget -q ${REPOSITORY_URL2}/esp8266_${BUILDVER}.bin
	    ${TMPDIR}/esptool.py --chip esp8266 --port ${DUSB} --baud ${DBAUD} ${DSTD} 0x00000 ${TMPDIR}/esp8266_${BUILDVER}.bin
	    ;;
    esac
    echo "------------------------------------------------------------------------"
    echo -en "${CYEL}${CBLNK}...waiting for reboot of device...${CNON}" ; sleep 4 ; echo -e "\033[2K"
    stty -F ${DUSB} ${TTYPARAM} ; sleep 1
    echo "Updating tty2oled software" > /dev/ttyUSB0
    echo -e "${CGRN}Downloading, checking and (maybe) installing and updating ${CYEL}tty2oled${CNON}"
    #wget -q ${REPOSITORY_URL1}/update_tty2oled.sh -O - | bash
    echo "MENU" > /tmp/CORENAME
    echo -e "\n${CGRN}Install/Update completed. Have fun!${CNON}"
fi

if [[ "${SWver}" = "${BUILDVER}" ]]; then
    echo -e "${CYEL}Good boy, you're hardware is up-to-date!${CNON}"
    [ "${INITSTOPPED}" = "yes" ] && ${INITSCRIPT} start
    sleep 0.5
    echo "MENU" > /tmp/CORENAME
fi

cd - ; rm -rf ${TMPDIR}
exit 0
# esptool.py --port ${DUSB} --baud 115200 erase_flash
# rm /lib/python3.9/site-packages/easy-install.pth /lib/python3.9/site-packages/pyserial-3.5-py3.9.egg
