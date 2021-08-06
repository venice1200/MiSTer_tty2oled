#!/bin/sh

. /media/fat/Scripts/tty2oled.ini


function waitforttyack() {
  echo -n "Waiting for tty2oled..."
  while [ "${ttyresponse}" != "ttyack" ]; do
    read -d ";" ttyresponse < ${TTYDEV}              # Read Serial Line until delimiter ";"
  done
  echo "${fgreen}${ttyresponse}${freset}"
  ttyresponse=""
}

# ** Main **
if [ -c "${TTYDEV}" ]; then							# check for tty device
  echo "${TTYDEV} detected, setting Parameter."					# some output
  stty -F ${TTYDEV} ${TTYPARAM}							# set tty parameter

  #/etc/init.d/S60tty2oled stop

  echo "CMDCLS" > ${TTYDEV}
  waitforttyack
  echo "CMDTXT,0,1,70,10,Text 0 Degrees" > ${TTYDEV}
  waitforttyack
  sleep 3
  echo "CMDFOD,1" > ${TTYDEV}
  waitforttyack
  echo "CMDTXT,0,1,180,10,Text 90" > ${TTYDEV}
  waitforttyack
  sleep 3
  echo "CMDFOD,2" > ${TTYDEV}
  waitforttyack
  echo "CMDTXT,0,1,170,50,Text 180 Degrees" > ${TTYDEV}
  waitforttyack
  sleep 3
  echo "CMDFOD,3" > ${TTYDEV}
  waitforttyack
  echo "CMDTXT,0,1,50,60,Text 270" > ${TTYDEV}
  waitforttyack
  echo "CMDFOD,0" > ${TTYDEV}
  waitforttyack

  #/etc/init.d/S60tty2oled restart

else										# no tty detected
  echo "No ${TTYDEV} Device detected, abort."					# some output
fi										# end if tty check
# ** End Main **
