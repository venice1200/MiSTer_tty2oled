#!/bin/bash

git -C ~/Arduino/libraries/MIC184_Temperature_Sensor pull --ff-only
git -C ~/Arduino/libraries/SSD1322_for_Adafruit_GFX pull --ff-only
arduino-cli core update-index
arduino-cli core upgrade
arduino-cli lib update-index
arduino-cli lib upgrade

SKETCHNAME="${HOME}/Arduino/MiSTer_SSD1322_USB/MiSTer_SSD1322_USB.ino"
SKETCHPATH="$(dirname "${SKETCHNAME%.*}")"
BBUILDPATH=$(mktemp -d)
WWWPATH="/var/www/tty2tft.de/htdocs/MiSTer_tty2oled-installer"

acompile() {
    echo "Compiling ${BLA}...Please wait..."
    time arduino-cli compile -b "${BOARD}" --build-path "${BBUILDPATH}" --clean "${SKETCHPATH}" --log-level info
    BUILDNAM=${BLA,,} ; BUILDNAM=${BUILDNAM:2}
    BUILDVER=$(grep "#define BuildVersion" ${BBUILDPATH}/sketch/*.ino.cpp | awk '{print $3}' | tr -d "\"")
    cp -a "${BBUILDPATH}/$(basename ${SKETCHNAME}).bin" "${WWWPATH}/${BUILDNAM}_${BUILDVER}.bin"
}

acompcopy() {
    cp -a "${BBUILDPATH}/$(basename ${SKETCHNAME}).partitions.bin" "${WWWPATH}/partitions.bin"
}

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

echo "${BUILDVER}" > "${WWWPATH}/buildver"
cp -a ${HOME}/.arduino15/packages/esp32/hardware/esp32/*/tools/partitions/boot_app0.bin "${WWWPATH}/"
cp -a ${HOME}/.arduino15/packages/esp32/hardware/esp32/*/tools/sdk/esp32/bin/bootloader_dio_80m.bin "${WWWPATH}/"
cp -a ${HOME}/.arduino15/packages/esp32/hardware/esp32/*/tools/sdk/esp32/bin/bootloader_qio_80m.bin "${WWWPATH}/"
