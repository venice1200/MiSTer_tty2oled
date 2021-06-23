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

# ** Main **
if [ -c "${TTYDEV}" ]; then							# check for tty device
  echo "${TTYDEV} detected, setting Parameter."					# some output
  stty -F ${TTYDEV} ${TTYPARAM}							# set tty parameter


  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,0,1,0,30,I'am Font No. 0" > ${TTYDEV}
  echo "CMDTXT,5,1,0,60,I'am Font No. 5" > ${TTYDEV}
  
  sleep 3
  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,1,1,0,30,I'am Font No. 1" > ${TTYDEV}
  echo "CMDTXT,6,1,0,60,I'am Font No. 6" > ${TTYDEV}
 
  sleep 3
  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,2,1,0,30,I'am Font No. 2" > ${TTYDEV}
  echo "CMDTXT,7,1,0,60,I'am Font No. 7" > ${TTYDEV}

  sleep 3
  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,3,1,0,30,I'am Font No. 3" > ${TTYDEV}
  echo "CMDTXT,8,1,0,60,I'am Font No. 8" > ${TTYDEV}

  sleep 3
  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,4,1,0,60,I'am Font 4" > ${TTYDEV}
 
  sleep 3
  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,9,1,0,30,I'am Font No. 9" > ${TTYDEV}
  echo "CMDTXT,10,1,0,60,I'am Font No. 10" > ${TTYDEV}

  sleep 3
  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,5,1,0,10,Frame" > ${TTYDEV}
  echo "CMDGEO,3,1,100,20,60,30,0" > ${TTYDEV}

  sleep 3
  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,5,1,0,10,Box" > ${TTYDEV}
  echo "CMDGEO,4,1,100,20,60,30,0" > ${TTYDEV}

  sleep 3
  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,5,1,0,10,Circle" > ${TTYDEV}
  echo "CMDGEO,5,1,128,32,20,0,0" > ${TTYDEV}

  sleep 3
  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,5,1,0,10,Disc" > ${TTYDEV}
  echo "CMDGEO,6,1,128,32,20,0,0" > ${TTYDEV}
  
  sleep 3
  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,5,1,0,10,Ellipse" > ${TTYDEV}
  echo "CMDGEO,7,1,128,32,40,20,0" > ${TTYDEV}

  sleep 3
  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,5,1,0,10,Filled Ellipse" > ${TTYDEV}
  echo "CMDGEO,8,1,128,32,40,20,0" > ${TTYDEV}

  sleep 3
  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,5,1,0,10,Rounded Frame" > ${TTYDEV}
  echo "CMDGEO,9,1,100,20,60,30,3" > ${TTYDEV}

  sleep 3
  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,5,1,0,10,Rounded Box" > ${TTYDEV}
  echo "CMDGEO,10,1,100,20,60,30,3" > ${TTYDEV}

  sleep 3
  echo "CMDCLS" > ${TTYDEV}
  echo "CMDTXT,5,1,0,10,Cleared Text in Box" > ${TTYDEV}
  echo "CMDGEO,3,1,98,18,64,34,0" > ${TTYDEV}
  sleep 1
  echo "CMDGEO,4,1,100,20,60,30,0" > ${TTYDEV}
  sleep 1
  echo "CMDTXT,5,0,118,40,Text" > ${TTYDEV}
  sleep 3
  echo "CMDTXT,5,1,0,10,Invert the Box & Text" > ${TTYDEV}
  sleep 2
  echo "CMDGEO,4,2,100,20,60,30,0" > ${TTYDEV}

  sleep 3
  for ((i=0; i<5; i++ )); do
    echo "CMDCON,200" > ${TTYDEV}
    sleep 0.2
    echo "CMDCON,0" > ${TTYDEV}
    sleep 0.2
  done

  echo "CMDCON,200" > ${TTYDEV}

else										# no tty detected
  echo "No ${TTYDEV} Device detected, abort."					# some output
fi										# end if tty check
# ** End Main **
