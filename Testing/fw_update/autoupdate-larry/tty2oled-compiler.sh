#!/bin/bash

SKETCHNAM=$(ls "${2}/"*.ino)
SKETCHNAM="$(basename "${SKETCHNAM%.*}")"
BBUILDPATH=$(mktemp -d)
SBUILDPATH=$(mktemp -d)
SBUILDPATH="${SBUILDPATH}/${SKETCHNAM}"
mkdir -p "${SBUILDPATH}"

cp -a "${2}/"*.ino "${2}/"*.h "${SBUILDPATH}/"
case "${1}" in
    HWESP32DE) BOARD="esp32:esp32:esp32"
    HWLOLIN32) BOARD="esp32:esp32:lolin32"
    HWESP8266) BOARD="esp8266:esp8266:nodemcuv2:xtal=160:ssl=basic:baud=921600"
esac

echo "Compiling...Please wait..."
time arduino-cli compile -b "${BOARD}" --build-path "${BBUILDPATH}" --clean "${SBUILDPATH}" --log-level info
