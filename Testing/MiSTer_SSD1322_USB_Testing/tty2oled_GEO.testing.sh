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
  echo "CMDGEO,10,1,0,0,256,18,4" > ${TTYDEV}
  waitforttyack
  echo "CMDGEO,9,1,0,18,256,46,4" > ${TTYDEV}
  waitforttyack
  echo "CMDGEO,6,0,10,9,5,0,0" > ${TTYDEV}
  waitforttyack
  #echo "CMDGEO,5,0,25,9,5,0,0" > ${TTYDEV}

  echo "CMDTXT,1,0,75,14,MiSTer FPGA" > ${TTYDEV}
  waitforttyack
  

  #/etc/init.d/S60tty2oled start

  while true; do
    myhrs=$(date +"%H")
    mymin=$(date +"%M")
    mysec=$(date +"%S")
    if [[ ${mysec} != ${mysecbuf} ]]; then
      echo "CMDTXT,8,1,60,50,${myhrs}:${mymin}:${mysec}" > ${TTYDEV}
      waitforttyack
      if [[ ${secbit} -eq 1 ]]; then
        echo "CMDGEO,6,1,10,9,5,0,0" > ${TTYDEV}
        waitforttyack
        echo "CMDGEO,6,0,245,9,5,0,0" > ${TTYDEV}
        waitforttyack
        secbit=0
      else
        echo "CMDGEO,6,1,245,9,5,0,0" > ${TTYDEV}
        waitforttyack
        echo "CMDGEO,6,0,10,9,5,0,0" > ${TTYDEV}
        waitforttyack
        secbit=1
      fi
      mysecbuf=${mysec}
    fi
  done

else										# no tty detected
  echo "No ${TTYDEV} Device detected, abort."					# some output
fi										# end if tty check
# ** End Main **
