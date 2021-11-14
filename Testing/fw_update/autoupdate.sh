#!/bin/bash

CRED="\e[1;31m"
CGRN="\e[1;32m"
CYEL="\e[1;33m"
CNON="\e[0m"
DUSB="/dev/ttyUSB0"
DBAUD="921600"
DSTD="--before default_reset --after hard_reset"

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
if [[ -c ${DUSB} ]]; then
    echo -en "${CNON}Checking for device at ${DUSB}${CNON}: "
    MCUtype=$(esptool.py --port ${DUSB} --baud 115200 chip_id)
else
    echo -e "${CRED}Could not connect to interface. Please check USB connection and run script again.${CNON}"
    exit 1
fi

#Check for MCU
if [[ $(grep "Chip is ESP32" <<< "${MCUtype}") ]]; then
    echo -e "${CGRN}ESP32 detected.${CNON}"
    echo -en "${CNON}Checking if MCU firmware update is required: ${CNON}"

    if [[ $(esptool.py --chip esp32 --port ${DUSB} --baud ${DBAUD} ${DSTD} verify_flash 0xe000 ./binaries/boot_app0.bin 0x1000 ./binaries/bootloader_qio_80m.bin 0x10000 ./binaries/firmware.bin 0x8000 ./binaries/partitions.bin | grep fail) ]]; then
	echo -e "${CYEL}MCU firmware outdated. Performing update.${CNON}"
	echo "------------------------------------"
	esptool.py --chip esp32 --port ${DUSB} --baud ${DBAUD} ${DSTD} write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 ./binaries/boot_app0.bin 0x1000 ./binaries/bootloader_qio_80m.bin 0x10000 ./binaries/firmware.bin 0x8000 ./binaries/partitions.bin
	echo "------------------------------------"
	echo -en "${CYEL}MCU firmware upload complete. Verifying data: "
	if [[ $(esptool.py --chip esp32 --port ${DUSB} --baud ${DBAUD} ${DSTD} verify_flash 0xe000 ./binaries/boot_app0.bin 0x1000 ./binaries/bootloader_qio_80m.bin 0x10000 ./binaries/firmware.bin 0x8000 ./binaries/partitions.bin | grep fail) ]]; then
	    echo -e "${CRED}Data verification failed. Please run script again. Possible sources of error: USB-serial converter / MCU flash memory.${CNON}"
	    exit 1
	else
	    echo -e "${CGRN}Data verification successful. Will exit script.${CNON}"
	    exit 0
	fi
    else
	echo -e "${CGRN}MCU firmware up to date. Will exit script.${CNON}"
	exit 0
    fi
elif [[ $(grep "Chip is ESP8266" <<< "${MCUtype}") ]]; then
    echo -e "${CGRN}ESP8266 detected.${CNON}"
else
    echo -e "${CRED}No compatible MCU attached to ttyUSB0. Please check USB connection and run script again.${CNON}"
fi
