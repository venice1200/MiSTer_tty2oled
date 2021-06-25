#!/bin/sh

. /media/fat/Scripts/tty2oled.ini

# ** Main **
if [ -c "${TTYDEV}" ]; then							# check for tty device
  echo "${TTYDEV} detected, setting Parameter."					# some output
  stty -F ${TTYDEV} ${TTYPARAM}							# set tty parameter

  #/etc/init.d/S60tty2oled stop


  echo "CMDCLS" > ${TTYDEV}
  echo "CMDGEO,10,1,0,0,256,18,4" > ${TTYDEV}
  echo "CMDGEO,9,1,0,18,256,46,4" > ${TTYDEV}
  echo "CMDGEO,6,0,10,9,5,0,0" > ${TTYDEV}
  #echo "CMDGEO,5,0,25,9,5,0,0" > ${TTYDEV}

  echo "CMDTXT,1,0,75,14,MiSTer FPGA" > ${TTYDEV}
  
  echo "CMDTXT,4,1,20,54,Bigger Text" > ${TTYDEV}


  #/etc/init.d/S60tty2oled start

  while true; do
    mytime=$(date +"%T")
    echo "CMDTXT,5,0,200,14,${mytime}" > ${TTYDEV}
    sleep 0.5
    echo "CMDGEO,6,1,10,9,5,0,0" > ${TTYDEV}
    sleep 0.5
    echo "CMDGEO,6,0,10,9,5,0,0" > ${TTYDEV}
  done

else										# no tty detected
  echo "No ${TTYDEV} Device detected, abort."					# some output
fi										# end if tty check
# ** End Main **
