#!/bin/bash

#/media/fat/tty2oled/check_hw.sh

. /media/fat/tty2oled/tty2oled.ini

function waitforttyack() {  
  echo -n "Waiting for tty2oled Acknowledge... "  
  while [ "${ttyresponse}" != "ttyack" ]; do    
    read -d ";" ttyresponse < ${TTYDEV}              # Read Serial Line until delimiter ";"  
  done  
  echo "${fgreen}${ttyresponse}${freset}"  
  ttyresponse=""
}


function waitforttyhwinfo() {
  echo -n "Waiting for tty2oled HW-Info: "
  while [[ "${hwinfo}" != "HW"* ]]; do
    read -d ";" hwinfo < ${TTYDEV}                    # Read Serial Line until delimiter ";"
  done
  echo "${fgreen}${hwinfo}${freset}"
  hwinfo=""
}


echo "${fgreen}Send HW-Info Request \"CMDHWINF\"${freset}"  
echo "CMDHWINF" > ${TTYDEV}

waitforttyhwinfo
waitforttyack
