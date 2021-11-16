#!/bin/bash

# https://arduino.github.io/arduino-cli/0.19/commands/arduino-cli_compile/

# arduino-cli compile -b BOARD --build-path PATH --clean SKETCHPATH
#
#BOARD ESP8266 NodeMCU v3
#"esp8266:esp8266:nodemcuv2:xtal=160,vt=flash,exception=disabled,stacksmash=disabled,ssl=all,mmu=3232,non32xfer=fast,eesz=4M2M,led=2,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=921600"
#
#BOARD Wemos LOLIN32, LOLIN32, DevKit_V4
#"esp32:esp32:lolin32:FlashFreq=80,PartitionScheme=default,CPUFreq=240,UploadSpeed=921600"
#
#BOARD TTGo-T8, d.ti
#"esp32:esp32:esp32:PSRAM=disabled,PartitionScheme=default,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,LoopCore=1,EventsCore=1,DebugLevel=none"

SKETCHNAM=$(ls "${2}/"*.ino)
SKETCHNAM="$(basename "${SKETCHNAM%.*}")"
BBUILDPATH=$(mktemp -d)
SBUILDPATH=$(mktemp -d)
SBUILDPATH="${SBUILDPATH}/${SKETCHNAM}"
mkdir -p "${SBUILDPATH}"

cp -a "${2}/"*.ino "${2}/"*.h "${SBUILDPATH}/"
case "${1}" in
    HWESP32DE) BOARD="esp32:esp32:esp32:PSRAM=disabled,PartitionScheme=default,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,LoopCore=1,EventsCore=1,DebugLevel=none" ;;
    HWESP8266) BOARD="esp8266:esp8266:nodemcuv2:xtal=160,vt=flash,exception=disabled,stacksmash=disabled,ssl=all,mmu=3232,non32xfer=fast,eesz=4M2M,led=2,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=921600" ;;
    HWLOLIN32 | HWDTIPCB0 | HWDTIPCB1) BOARD="esp32:esp32:lolin32:FlashFreq=80,PartitionScheme=default,CPUFreq=240,UploadSpeed=921600" ;;
esac

echo "Compiling...Please wait..."
arduino-cli compile -b "${BOARD}" --build-path "${BBUILDPATH}" --clean "${SBUILDPATH}" --log-level info