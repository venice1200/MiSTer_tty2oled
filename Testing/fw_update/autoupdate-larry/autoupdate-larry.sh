#!/bin/bash

[ -f ./tty2oled.ini ] && source ./tty2oled.ini
[ -f /media/fat/tty2oled/tty2oled.ini ] && source /media/fat/tty2oled/tty2oled.ini

BUILDVER="2112115T"

CRED="\e[1;31m"
CGRN="\e[1;32m"
CYEL="\e[1;33m"
CNON="\e[0m\033[0m"
CBLNK="\033[5m"
DUSB="/dev/ttyUSB0"
DBAUD="921600"
DSTD="--before default_reset --after hard_reset"
TTYPARAM="${BAUDRATE} cs8 raw -parenb -cstopb -hupcl -echo"
stty -F ${DUSB} ${TTYPARAM}

#Check if Python is installed
echo -en "${CNON}Checking for Python installation: ${CGRN}"
python --version || (echo -e "\n${CRED}Python installation missing. Please install Python and run script again.${CGRN}" && exit 1)

#Check if PIP is installed
echo -en "${CNON}Checking for PIP installation: ${CGRN}"
pip --version || (echo -e "\nPIP installation missing. Will install.${CNON}" && (wget -N https://bootstrap.pypa.io/get-pip.py || (echo -e "${CRED}Unable to download PIP. Please check network connection an run script again.${CNON}" && exit 1)) && python ./get-pip.py)
rm get-pip.py &> /dev/null

#Install pySerial (if it is missing)
echo -en "${CNON}Checking for pySerial installation: ${CGRN}"
pip_list="$(pip list)"
if [[ $(grep "pyserial" <<< "${pip_list}") ]]; then
    echo $(grep "pyserial" <<< "${pip_list}")
else
    pip install pyserial
fi

#Install esptool (if it is missing)
echo -en "${CNON}Checking for ESPtool installation: ${CGRN}"
if [[ $(grep "esptool" <<< "${pip_list}") ]]; then
    echo $(grep "esptool" <<< "${pip_list}")
else
    pip install esptool
fi

#Check if interface ttyUSB0 is present
echo -en "${CNON}Checking for device at ${DUSB}${CNON}: "
if [[ -c ${DUSB} ]]; then
    echo -e "${CGRN}available${CNON}"
    echo "CMDHWINF" > ${DUSB} ; read BLA < ${DUSB}
    MCUtype=${BLA:0:6}
    SWver=${BLA%;*} && SWver=${SWver:7}
    echo "debug: device is a ${MCUtype} with sketch version ${SWver}"
else
    echo -e "${CRED}Could not connect to interface. Please check USB connection and run script again.${CNON}"
    exit 1
fi

#Check for MCU
case "${MCUtype}" in
    HWNONE) echo -e "${CRED}Unknown hardware, can't continue.${CNON}" ; exit 1 ;;
    HWESP32DE)	echo -e "${CYEL}ESP32 detected (TTGO/DTI).${CNON}"
		MCU="espdti" ;;
    HWLOLIN32)	echo -e "${CYEL}ESP32 detected (Wemos/Lolin/DevKit_V4).${CNON}"
		MCU="esp32" ;;
    HWDTIPCB0)	echo -e "${CYEL}ESP32 detected (DTI v1.0).${CNON}"
		MCU="esp32" ;;
    HWDTIPCB1)	echo -e "${CYEL}ESP32 detected (DTI n/a).${CNON}"
		MCU="esp32" ;;
    HWESP8266)	echo -e "${CYEL}ESP8266 detected.${CNON}"
		MCU="esp8266" ;;
esac

if [[ "${SWver}" < "${BUILDVER}" ]]; then
    echo -e "${CYEL}Version of your tty2oled device is ${SWver}, but BUILDVER is ${BUILDVER}. Updating!${CNON}"
    echo "------------------------------------"
	[ "${MCU}" = "esp32" ] && esptool.py --chip esp32 --port ${DUSB} --baud ${DBAUD} ${DSTD} write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 ./binaries/boot_app0.bin 0x1000 ./binaries/bootloader_qio_80m.bin 0x10000 ./binaries/firmware.bin 0x8000 ./binaries/partitions.bin
	[ "${MCU}" = "espdti" ] && esptool.py --chip esp32 --port ${DUSB} --baud ${DBAUD} ${DSTD} write_flash -z --flash_mode qio --flash_freq 80m --flash_size detect 0xe000 ./binaries/boot_app0.bin 0x1000 ./binaries/bootloader_qio_80m.bin 0x10000 ./binaries/firmware.bin 0x8000 ./binaries/partitions.bin
	[ "${MCU}" = "esp8266" ] && esptool.py --chip esp8266 --port ${DUSB} --baud ${DBAUD} ${DSTD} write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0x00000 ./binaries/MiSTer_SSD1322_USB.ino.esp8266.bin
    echo "------------------------------------"
    echo -en "${CYEL}${CBLNK}...waiting for reboot of device...${CNON}" ; sleep 3 ; echo -e "\033[2K"
    echo -en "${CYEL}"
    read -t5 -n1 -p "Do you want to verify the flash (y/n)? - Waiting 5 seconds for input..." BLA
    echo -e "${CNON}"
    if [ "${BLA}" = "y" ]; then
	[ "${MCU}" = "esp32" ] && esptool.py --chip esp32 --port ${DUSB} --baud ${DBAUD} ${DSTD} verify_flash --flash_size detect 0xe000 ./binaries/boot_app0.bin 0x1000 ./binaries/bootloader_qio_80m.bin 0x10000 ./binaries/firmware.bin 0x8000 ./binaries/partitions.bin
	[ "${MCU}" = "esp8266" ] && esptool.py --chip esp8266 --port ${DUSB} --baud ${DBAUD} ${DSTD} verify_flash --flash_size detect 0x00000 ./binaries/MiSTer_SSD1322_USB.ino.esp8266.bin
    fi
fi
