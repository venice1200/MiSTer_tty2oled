#!/bin/bash
#
# tty2oled greyscale testbench

. /media/fat/tty2oled/tty2oled-system.ini
. /media/fat/tty2oled/tty2oled-user.ini

nextwait=2
xbmpath="/media/fat/tty2oled/pics/XBM/"
xbmpic="mpatrol.xbm"
gscpath="/media/fat/tty2oled/pics/GSC/"
gscpic="berzerk.gsc"

function waitforttyack() {
  echo -n "Waiting for tty2oled Acknowledge... "
  read -d ";" ttyresponse < ${TTYDEV}                # The "read" command at this position simulates an "do..while" loop
  while [ "${ttyresponse}" != "ttyack" ]; do
    read -d ";" ttyresponse < ${TTYDEV}              # Read Serial Line until delimiter ";"
  done
  echo -e "${fgreen}${ttyresponse}${freset}"
  ttyresponse=""
}


#------------------- Main --------------------

#${INITSCRIPT} stop

echo "tty2oled Testbench" > ${TTYDEV}
sleep ${nextwait}

echo "> CMDCOR XBM" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDCOR,${xbmpic},0" > ${TTYDEV}
tail -n +4 ${xbmpath}${xbmpic} | xxd -r -p > ${TTYDEV}
waitforttyack
sleep ${nextwait}

echo "> CMDCOR GSC" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDCOR,${gscpic},0" > ${TTYDEV}
tail -n +4 ${gscpath}${gscpic} | xxd -r -p > ${TTYDEV}
waitforttyack
sleep ${nextwait}

echo "> CMDAPD,CMDSNAM,CMDSPIC" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDAPD,${gscpic}" > ${TTYDEV}
tail -n +4 ${gscpath}${gscpic} | xxd -r -p > ${TTYDEV}
waitforttyack
echo "CMDSNAM" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDSPIC,0" > ${TTYDEV}
waitforttyack
sleep ${nextwait}

echo "> CMDCON" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDCON,10" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDCON,100" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDCON,200" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDCON,100" > ${TTYDEV}
waitforttyack
sleep ${nextwait}

echo "> CMDDOFF,CMDDON" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDDOFF" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDDON" > ${TTYDEV}
waitforttyack
sleep ${nextwait}

echo "> CMDCLS,CMDGEO" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDCLS" > ${TTYDEV}
waitforttyack
echo "CMDGEO,1,15,10,10,0,0,0,0" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDGEO,2,15,10,20,30,30,0,0" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDGEO,3,15,50,10,10,10,0,0" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDGEO,4,15,50,40,10,10,0,0" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDGEO,5,15,80,15,10,10,0,0" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDGEO,6,15,80,45,10,10,0,0" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDGEO,7,15,100,10,30,20,5,0" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDGEO,8,15,100,40,30,20,5,0" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDGEO,9,15,140,10,180,20,160,30" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDGEO,10,15,140,30,180,40,160,50" > ${TTYDEV}
waitforttyack
sleep ${nextwait}

echo "> CMDCLST" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
for i in {1..23}; do
  echo "CMDCLST,${i},15" > ${TTYDEV}
  waitforttyack
  sleep ${nextwait}
  echo "CMDCLST,${i},0" > ${TTYDEV}
  waitforttyack
  sleep ${nextwait}
  echo "CMDCLS" > ${TTYDEV}
  waitforttyack
done


echo "> CMDBYE" > ${TTYDEV}
waitforttyack
sleep ${nextwait}
echo "CMDBYE" > ${TTYDEV}
waitforttyack

#${INITSCRIPT} start
