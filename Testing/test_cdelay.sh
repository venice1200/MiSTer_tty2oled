#!/bin/bash

. /media/fat/tty2oled/tty2oled-system.ini
. /media/fat/tty2oled/tty2oled-user.ini

nextwait=1
cDelay=100;
xbmpath="/media/fat/tty2oled/pics/XBM/"
xbmpic="1942.xbm"
gscpath="/media/fat/tty2oled/pics/GSC/"
gscpic="1942.gsc"

#
# cDelay gestoppt bei
# 8266: 	15,15,15
# Lolin32: 	45,45,45
# TTGO:		durchgelaufen
# d.ti 1.1: durchgelaufen
# d.to 1.2:
#

function waitforttyack() {
  ttyresponse=""
  #echo -n -e "${fyellow}Waiting for tty2oled... ${freset}"
  read -d ";" ttyresponse < ${TTYDEV}                # The "read" command at this position simulates an "do..while" loop
  while [ "${ttyresponse}" != "ttyack" ]; do
    read -d ";" ttyresponse < ${TTYDEV}              # Read Serial Line until delimiter ";"
  done
  #echo -e "${fgreen}${ttyresponse}${freset}"
}

function clearbuffer() {
  # Clear Serial input buffer
  while read -t 0 sdummy < ${TTYDEV}; do continue; done
}

# Main
clear
echo -e "Init ${TTYDEV} Interface!\n"
stty -F ${TTYDEV} ${BAUDRATE} ${TTYPARAM}
sleep ${nextwait}

echo -e "Stopping Daemon\n"
/media/fat/tty2oled/S60tty2oled stop
sleep ${nextwait}

echo -e "Disable Screensaver\n"
echo "CMDSAVER,0,0,0" > ${TTYDEV}
sleep ${nextwait}


# Loop
while [[ ${cDelay} -gt 0 ]]; do
  echo -e "${fyellow}Set cDelay to: ${fblue}${cDelay}${freset}"
  echo "CMDSECD,${cDelay}" > ${TTYDEV}
  clearbuffer
  sleep ${nextwait}
  #echo "Buffer cleared, empty ?"
  #waitforttyack  #Test to see buffer is empty
  
  echo -e "${fblue}Clear Display${freset}"
  echo "CMDCLS" > ${TTYDEV}
  waitforttyack
  echo -e "${fgreen}Show cDelay on Screen${freset}"
  #echo "CMDTXT,3,15,0,40,32, cDelay: ${cDelay}" > ${TTYDEV}
  echo "CMDSHCD" > ${TTYDEV}
  waitforttyack
  sleep ${nextwait}

  echo -e -n "${fgreen}Showing GSC Picture ${fblue}${gscpic}${freset}"
  echo "CMDCOR,${gscpic}" > ${TTYDEV}
  tail -n +4 ${gscpath}${gscpic} | xxd -r -p > ${TTYDEV}
  waitforttyack
  echo -e " ${fgreen}...done${freset}"
  #sleep ${nextwait}
  
  echo -e -n "${ffgreen}Showing XBM Picture ${fblue}${xbmpic}${freset}"
  echo "CMDCOR,${xbmpic}" > ${TTYDEV}
  tail -n +4 ${xbmpath}${xbmpic} | xxd -r -p > ${TTYDEV}
  waitforttyack
  echo -e " ${fgreen}...done${freset}"
  #sleep ${nextwait}
  echo ""
  
  if [[ ${cDelay} -gt 50 ]]; then
    cDelay=$((cDelay-10))
  #elif [[ ${cDelay} -gt 20 ]]; then
  #  cDelay=$((cDelay-5))
  else
    cDelay=$((cDelay-1))
  fi
done

exit 0