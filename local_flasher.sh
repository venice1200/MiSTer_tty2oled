#!/bin/bash

REPOSITORY_URL2="https://www.tty2tft.de//MiSTer_tty2oled-installer"
TTYDEV="/dev/ttyUSB0"
DBAUD="921600"
DSTD="--before default_reset --after hard_reset write_flash --compress --flash_mode dio --flash_freq 80m --flash_size detect"

cd /tmp

if [ -z "${1}" ]; then
  echo -e "Need at least name of device (esp8266, lolin32, esp32de).\nOptional: stable or testing\nExiting."
  exit 255
fi

if ! [ "${1}" = "esp8266" ] && ! [ "${1}" = "lolin32" ] && ! [ "${1}" = "esp32de" ]; then
  echo "Need at least name of device (esp8266, lolin32, esp32de). Exiting."
  exit 255
fi

[ "${2}" = "" ] && BUILDVER=$(wget -q ${REPOSITORY_URL2}/buildver -O -)
[ "${2}" = "stable" ] && BUILDVER=$(wget -q ${REPOSITORY_URL2}/buildver -O -)
[ "${2}" = "testing" ] && BUILDVER=$(wget -q ${REPOSITORY_URL2}/buildverT -O -)
echo "Using BuildVer ${BUILDVER}"

wget -Nq ${REPOSITORY_URL2}/esptool.py ${REPOSITORY_URL2}/boot_app0.bin ${REPOSITORY_URL2}/bootloader_dio_80m.bin ${REPOSITORY_URL2}/partitions.bin ${REPOSITORY_URL2}/${1}_${BUILDVER}.bin
exit
if [ "${1}" = "esp8266" ]; then
  python ./esptool.py --chip esp8266 --port ${TTYDEV} --baud ${DBAUD} ${DSTD} 0x00000 esp8266_${BUILDVER}.bin
else
  python ./esptool.py --chip esp32 --port ${TTYDEV} --baud ${DBAUD} ${DSTD} 0xe000 boot_app0.bin 0x1000 bootloader_dio_80m.bin 0x10000 "${1}" 0x8000 partitions.bin
fi
