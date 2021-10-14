#!/bin/sh

# By venice & ojaksch
#
# 2021-01-12 First release
# 2021-01-15 Adding debug to /tmp/tty2oled, device check else part
# 2021-01-16 Change send command "echo -ne ${newcore} >" to "echo ${newcore} >" without -ne to send "\n" (newline)
#            Arduino uses now "Serial.readStringUntil('\n');"
#            Feels more responsive as Serial Read on Arduino is not waitung for the timeout (1000ms)
#            Add "raw" to tty Parameter (see stty manpage)
# 2021-01-17 Add check for readable file "/tmp/CORENAME"
# 2021-01-19 Add "First Transmission" to clear send buffer (Preventing  weird issues after PowerOn)
# 2021-02-07 Changed Speed from 9600 to 57600
# 2021-02-14 Change Timed Loop to " inotifywait -e modify "/tmp/CORENAME" ".
#            Makes it much more responsive :-)
# 2021-03-29 USB Transfer realized by ojaksch, many thanks.
#            Modified Arduino Sketch and "tty2oled" for the USB Version of tty2oled.
#            All XBM must be stored on the MiSTer in the folder "/media/fat/tty2oledpics".
#            Instead of sending the Corename the Content of the XBM is send if an XBM exists. Done with the function "senddata".
#            "senddata" checks for an existing Picture-Folder and if the Folder exists the Picture-Data (if found) or the Corename are sent.
#            If the Folder does not exist, it's assumed the "SD Version" is used and only the corename is sent. 
#            Serial Interface set to 115200 Baud now.
# 2021-04-12 Added an INI file
# 2021-05-05 Added sending Contrast Value
# 2021-05-14 Adding Command Line Parameter for Testing
#            Parameter 1: /dev/ttyUSBx Device 
#            Parameter 2: SD or USB Mode 
#            Parameter 3: Baudrate
#            Example: /usr/bin/tty2oled /dev/ttyUSB1 
#            Example: /usr/bin/tty2oled /dev/ttyUSB1 SD 
#            Example: /usr/bin/tty2oled /dev/ttyUSB1 USB 921600
# 2021-06-22 New Command Mode (Testing)
#            "CMCOR,[Corename]", "CMDCON,[Contrast]",  "CMDTEX,[Parameter]", "CMDGEO,[Parmeter]", "CMDRST", "CMDOTA"
# 2021-06-24 Adding folder "/media/fat/Scripts/.tty2oled/pics_pri"  
#            Pictures found in this folder will be used before the "common" pictures.
#            The User himself is responsible for the content in this Folder.
# 2021-06-30 Added new INI Option/Command for Display Rotation ("CMDROT,[Parameter]")
# 2021-07-10 Fix Baudrate CLI Parameter handling
#            Change CLI Parameter order
#            Parameter 1: /dev/ttyUSBx Device
#            Parameter 2: Baudrate
#            Parameter 3: SD or USB Mode
#            Example: /usr/bin/tty2oled /dev/ttyUSB1 
#            Example: /usr/bin/tty2oled /dev/ttyUSB1 115200
#            Example: /usr/bin/tty2oled /dev/ttyUSB1 921600 USB
# 2021-07-14 Clean up Script
# 2021-09-09 Moved from /etc/init.d to /media/fat/tty2oled 
# 2021-09    Grayscale pictures implemented
# 2021-10-02 Complete rework of senddata
# 2021-10-05 USE_RANDOM_ALT to choose between _altX pictures
#
#

. /media/fat/tty2oled/tty2oled.ini

# Debug function
dbug() {
  if [ "${debug}" = "true" ]; then
    if [ ! -e ${debugfile} ]; then						# log file not (!) exists (-e) create it
      echo "---------- tty2oled Debuglog ----------" > ${debugfile}
    fi 
    echo "${1}" >> ${debugfile}							# output debug text
  fi
}

# Send-Contrast-Data function
sendcontrast() {
  if [ "${USBMODE}" = "yes" ]; then						# Check the tty2xxx mode
    dbug "Sending: CMDCON,${CONTRAST}"
    echo "CMDCON,${CONTRAST}" > ${TTYDEV}					# Send Contrast Command and Value
  else
    echo "att" > ${TTYDEV}							# Send an "att" to the MiSTer annoucing another Command
    sleep ${WAITSECS}								# sleep needed here ?!
    echo "CONTRAST" > ${TTYDEV}							# Send "CONTRAST" annoucing the OLED Contrast Data as next
    sleep ${WAITSECS}								# sleep needed here ?!
    echo ${CONTRAST} > ${TTYDEV}						# Send Contrast Value
  fi
}

# Rotate Display function
sendrotation() {
  if [ "${USBMODE}" = "yes" ]; then						# Check the tty2xxx mode
    if [ "${ROTATE}" = "yes" ]; then
      dbug "Sending: CMDROT,1"
      echo "CMDROT,1" > ${TTYDEV}						# Send Rotation if set to "yes"
      sleep ${WAITSECS}
      echo "CMDSORG" > ${TTYDEV}						# Show Start Screen rotated
      sleep 4
    #else
    #  ebug "Sending: CMDROT,1" > ${TTYDEV}
    #  echo "CMDROT,0" > ${TTYDEV}						# No Rotation
    fi
  fi
}

# USB Send-Picture-Data function
senddata() {
  . /media/fat/tty2oled/tty2oled.ini						# ReRead INI for changes
  newcore="${1}"
  unset picfnam
  if [ "${USBMODE}" = "yes" ]; then						# Check the tty2xxx mode
    if [ -e "${picturefolder_pri}/${newcore}.gsc" ]; then			# Check for _pri pictures
       picfnam="${picturefolder_pri}/${newcore}.gsc"
    elif [ -e "${picturefolder_pri}/${newcore}.xbm" ]; then
       picfnam="${picturefolder_pri}/${newcore}.xbm"
    else
      picfolders="gsc_us xbm_us gsc xbm xbm_text"				# If no _pri picture found, try all the others
      [ "${USE_US_PICTURE}" = "no" ] && picfolders="${picfolders//gsc_us xbm_us/}"
      [ "${USE_GSC_PICTURE}" = "no" ] && picfolders="${picfolders//gsc_us/}" && picfolders="${picfolders//gsc/}"
      [ "${USE_TEXT_PICTURE}" = "no" ] && picfolders="${picfolders//xbm_text/}"
      for picfolder in ${picfolders}; do
	picfnam="${picturefolder}/${picfolder^^}/${newcore}.${picfolder:0:3}"
	[ -e "${picfnam}" ] && break
      done
    fi
    if [ -e "${picfnam}" ]; then						# Exist?
      if [ "${USE_RANDOM_ALT}" = "yes" ]; then					# Use _altX pictures?
	SAVEIFS="${IFS}"
	IFS=$'\n'
	ALTPICNUM=$(find $(dirname "${picfnam}") -name $(basename "${picfnam%.*}_alt")* | wc -l)
	IFS="${SAVEIFS}"
	if [ "${ALTPICNUM}" -gt "0" ]; then					# If more than 0 _altX pictures
	  ALTPICRND=$(( ${RANDOM} % $((ALTPICNUM+1)) ))				# then dice between 0 and count of found _altX pictures
	  [ "${ALTPICRND}" -gt 0 ] && picfnam="${picturefolder}/${picfolder^^}/${newcore}_alt"${ALTPICRND}".${picfolder:0:3}"
	fi									# If 0 then original picture, otherwise _altX
      fi
      dbug "Sending: CMDCOR,${1}"
      echo "CMDCOR,${1}" > ${TTYDEV}						# Send CORECHANGE" Command and Corename
      sleep ${WAITSECS}								# sleep needed here ?!
      tail -n +4 "${picfnam}" | xxd -r -p > ${TTYDEV}				# The Magic, send the Picture-Data up from Line 4 and proces
    else									# No Picture available!
      echo "${1}" > ${TTYDEV}							# Send just the CORENAME
    fi										# End if Picture check
  else										# SD/Standard Mode ? Just send the Corename
    echo "${1}" > ${TTYDEV}							# Instruct the device to load the appropriate picture from SD card
  fi
}

# ** Main **
# Check for Command Line Parameter
if [ "${#}" -ge 1 ]; then							# Command Line Parameter given, override Parameter
  echo -e "\nUsing Command Line Parameter"
  TTYDEV=${1}									# Set TTYDEV with Parameter 1
  if [ -n "${2}" ]; then							# Parameter 2 Baudrate
    BAUDRATE=${2}								# Set Baudrate
  fi										# end if Parameter 3
  if [ -n "${3}" ]; then							# Parameter 3 SD or USB Mode
    if [ "${3}" = "SD" ]; then							# Parameter 3 = "SD" ?
      USBMODE="no"								# Un-Set USBMODE
    elif [ "${3}" = "USB" ]; then						# Parameter 3 = "USB" ?
      USBMODE="yes"								# Set USBMODE
    else									# Parameter not "USB" or "SD"!
      echo "Parameter 3 invalid"						# Parameter 3 wrong
    fi										# end if USB Mode
  fi										# end if Parameter 3
  echo "Using Interface: ${TTYDEV} with ${BAUDRATE} Baud"			# Device Output
  echo "USBMODE: ${USBMODE}"							# Mode Output
fi										# end if command line Parameter 

# Let's go
if [ -c "${TTYDEV}" ]; then							# check for tty device
  echo -e "\n${TTYDEV} detected, setting Parameter: ${BAUDRATE} ${TTYPARAM}."
  dbug "${TTYDEV} detected, setting Parameter: ${BAUDRATE} ${TTYPARAM}."
  stty -F ${TTYDEV} ${BAUDRATE} ${TTYPARAM}					# set tty parameter
  sleep ${WAITSECS}
  echo "QWERTZ" > ${TTYDEV}							# First Transmission to clear serial send buffer
  dbug "Send QWERTZ as first transmission"
  sleep ${WAITSECS}
  sendcontrast									# Set Contrast
  sendrotation									# Set Display Rotation
  while true; do								# main loop
    if [ -r ${corenamefile} ]; then						# proceed if file exists and is readable (-r)
      newcore=$(cat ${corenamefile})						# get CORENAME
      echo "Read CORENAME: -${newcore}-"
      dbug "Read CORENAME: -${newcore}-"
      if [ "${newcore}" != "${oldcore}" ]; then					# proceed only if Core has changed
        echo "Send -${newcore}- to ${TTYDEV}."
        dbug "Send -${newcore}- to ${TTYDEV}."
        senddata "${newcore}"							# The "Magic"
        oldcore="${newcore}"							# update oldcore variable
      fi									# end if core check
      inotifywait -e modify "${corenamefile}"					# wait here for next change of corename
    else									# CORENAME file not found
     echo "File ${corenamefile} not found!"
     dbug "File ${corenamefile} not found!"
    fi										# end if /tmp/CORENAME check
  done										# end while
else										# no tty detected
  echo "No ${TTYDEV} Device detected, abort."
  dbug "No ${TTYDEV} Device detected, abort."
fi										# end if tty check
# ** End Main **
