#!/bin/sh

# /media/fat/Scripts/tty2oled_cmd.sh
# By venice
# Send Hardware-Commands to the tty2oled's ESP32
#
# Example
# /media/fat/Scripts/tty2oled_cmd.sh [PARAMETER]
# Valid Parameter see Variable "PARAM"
#

. /media/fat/Scripts/tty2oled.ini

#Colors
fblink=`tput blink`
fbold=`tput bold`
freset=`tput sgr0`
finvers=`tput rev`
fgreen=`tput setf 2`
fred=`tput setf 4`
fblue=`tput setf 1`

PARAM="espota espreset dispoff dispon ttystop ttystart ttybooton ttybootoff dispron disproff"

# Debug function
dbug() {
  if [ "${debug}" = "true" ]; then
    if [ ! -e ${debugfile} ]; then						# log file not (!) exists (-e) create it
      echo "---------- tty2oled Debuglog ----------" > ${debugfile}
    fi 
    echo "${1}" >> ${debugfile}							# output debug text
  fi
}

# ** Main **
if [ -c "${TTYDEV}" ]; then                         # check for tty device  
  echo "${TTYDEV} detected, setting Parameter."					
  dbug "${TTYDEV} detected, setting Parameter."					
  stty -F ${TTYDEV} ${TTYPARAM}	                    # set tty parameter
else
  echo "${TTYDEV} not detected, exit."
  dbug "${TTYDEV} not detected, exit."					
  exit 1
fi
  
 
if [ "${#}" -ge 1 ]; then                           # At least one Command Line Parameter given
  case "${1}" in
    "espota")
      echo "Enable ${fblue}${blink}OTA${freset} on ESP32"
      echo "CMDENOTA" > ${TTYDEV}
    ;;
    "espreset")
      echo "${fred}Reboot${freset} ESP32"
      echo "CMDRESET" > ${TTYDEV}
    ;;
    "dispoff")
      echo "Power Display ${fred}OFF${freset}"
      echo "CMDOFF,1" > ${TTYDEV}
    ;;
    "dispon")
      echo "Power Display ${fgreen}ON${freset}"
      echo "CMDOFF,0" > ${TTYDEV}
    ;;
    "disproff")
      echo "Display Rotation ${fred}OFF${freset}"
      echo "CMDROT,0" > ${TTYDEV}
      echo "CMDSORG" > ${TTYDEV}
    ;;
    "dispron")
      echo "Display Rotation ${fgreen}ON${freset}"
      echo "CMDROT,1" > ${TTYDEV}
      echo "CMDSORG" > ${TTYDEV}
    ;;
    "ttystop")
      echo "${fred}Stop${freset} ${DAEMONNAME}"
      ${INITSCRIPT} stop
    ;;
    "ttystart")
      echo "${fgreen}Start${freset} ${DAEMONNAME}"
      ${INITSCRIPT} start
    ;;
    "ttybooton")
      echo "${fgreen}Enable${freset} tty2oled at boot time"
      [[ -e ${INITDISABLED} ]] && mv ${INITDISABLED} ${INITSCRIPT}
      if [ $? -eq 0 ]; then
        echo "...${fgreen}done.${freset}"
      else
        echo "...${fred}error!${freset}"
      fi
    ;;
    "ttybootoff")
      echo "${fred}Disable${freset} tty2oled at boot time"
      [[ -e ${INITSCRIPT} ]] && mv ${INITSCRIPT} ${INITDISABLED}
      if [ $? -eq 0 ]; then
        echo "...${fgreen}done.${freset}"
      else
        echo "...${fred}error!${freset}"
      fi      
    ;;
    *)
      echo -e "${fred}${fblink}Wrong Commandline Parameter given!${freset}"
      echo "${fgreen}Valid Parameter: ${fblue}${blink}${PARAM}${freset}"
    ;;
  esac
else
  echo -e "${fred}${fblink}No Commandline Parameter given!${freset}"
  echo "${fgreen}Valid Parameter: ${fblue}${blink}${PARAM}${freset}"
fi
# ** End Main **
