#!/bin/bash

echo "-------- gut pull of unofficial libs"
git -C ~/Arduino/libraries/MIC184_Temperature_Sensor pull --ff-only
git -C ~/Arduino/libraries/SSD1322_for_Adafruit_GFX pull --ff-only
echo "-------- update of cores"
arduino-cli core update-index
arduino-cli core upgrade
echo "-------- update of libs"
arduino-cli lib update-index
arduino-cli lib upgrade

WWWPATH="/var/www/tty2tft.de/htdocs/MiSTer_tty2oled-installer"
BBUILDPATH=$(mktemp -d)

acompile() {
    BUILDNAM=${BLA,,} ; BUILDNAM=${BUILDNAM:2}
    BUILDVER=$(grep "#define BuildVersion" ${SKETCHNAME} | awk '{print $3}' | tr -d "\"")
    echo "-------- Compiling ${BLA} - BUILDVER ${BUILDVER} ...Please wait..."
    arduino-cli compile -b "${BOARD}" --build-path "${BBUILDPATH}" --clean "${SKETCHPATH}" --log-level error
    cp -a "${BBUILDPATH}/$(basename ${SKETCHNAME}).bin" "${WWWPATH}/${BUILDNAM}_${BUILDVER}.bin"
}

acompcopy() {
    cp -a "${BBUILDPATH}/$(basename ${SKETCHNAME}).partitions.bin" "${WWWPATH}/partitions.bin"
}

ahardware() {
    for BLA in HWESP32DE HWLOLIN32 HWESP8266; do
	case "${BLA}" in
	    HWESP32DE)
		BOARD="esp32:esp32:esp32:FlashMode=dio"
		acompile && acompcopy
		;;
	    HWLOLIN32)
		BOARD="esp32:esp32:lolin32"
		acompile && acompcopy
		;;
	    HWESP8266)
		BOARD="esp8266:esp8266:nodemcuv2:xtal=160,ssl=basic,baud=921600"
		acompile
		;;
	esac
    done
}

if [ "${1}" = "" ] || [ "${1}" = "stable" ]; then
    SKETCHNAME="/var/www/tty2tft.de/git/MiSTer_tty2oled/MiSTer_SSD1322_USB/MiSTer_SSD1322_USB.ino"
    SKETCHPATH="$(dirname "${SKETCHNAME%.*}")"
    ahardware
    echo "${BUILDVER}" > "${WWWPATH}/buildver"
fi

if [ "${1}" = "" ] || [ "${1}" = "testing" ]; then
    SKETCHNAME="/var/www/tty2tft.de/git/MiSTer_tty2oled/Testing/MiSTer_SSD1322_USB_Testing/MiSTer_SSD1322_USB_Testing.ino"
    SKETCHPATH="$(dirname "${SKETCHNAME%.*}")"
    ahardware
    echo "${BUILDVER}" > "${WWWPATH}/buildverT"
fi

cp -a ${HOME}/.arduino15/packages/esp32/hardware/esp32/*/tools/partitions/boot_app0.bin "${WWWPATH}/"
cp -a ${HOME}/.arduino15/packages/esp32/hardware/esp32/*/tools/sdk/esp32/bin/bootloader_dio_80m.bin "${WWWPATH}/"
cp -a ${HOME}/.arduino15/packages/esp32/hardware/esp32/*/tools/sdk/esp32/bin/bootloader_qio_80m.bin "${WWWPATH}/"

[ -d ${BBUILDPATH} ] && rm -rf ${BBUILDPATH}
