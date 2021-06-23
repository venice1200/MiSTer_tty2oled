#!/bin/sh

. /media/fat/Scripts/tty2oled.ini

# Debug function
dbug() {
  if [ "${debug}" = "true" ]; then
    if [ ! -e ${debugfile} ]; then						# log file not (!) exists (-e) create it
      echo "---------- tty2oled Debuglog ----------" > ${debugfile}
    fi 
    echo "${1}" >> ${debugfile}							# output debug text
  fi
}

if [ -c "${TTYDEV}" ]; then							# check for tty device
  echo "${TTYDEV} detected, setting Parameter."					# some output
  dbug "${TTYDEV} detected, setting Parameter."					# some debug output
  stty -F ${TTYDEV} ${TTYPARAM}							# set tty parameter
  sleep ${WAITSECS}	

  echo "Enable OTA Mode on ESP32"
  dbug "Enable OTA Mode on ESP32"
  echo "CMDENOTA" > ${TTYDEV}                                                   # Send OTA Command
fi
