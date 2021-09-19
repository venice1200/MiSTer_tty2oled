#!/bin/sh


. /media/fat/tty2oled/tty2oled.ini

senddata() {
  if [ "${USBMODE}" = "yes" ]; then                                             # Check the tty2xxx mode
    unset PICFNAM
    if [ -f "${picturefolder_pri}/${1}.gsc" ]; then                             # Lookup for an existing GSC in PRI and proceed
      PICFNAM="${picturefolder_pri}/${1}.gsc"
    elif [ -f "${picturefolder_pri}/${1}.xbm" ]; then                           # Lookup for an existing XBM in PRI and proceed
      PICFNAM="${picturefolder_pri}/${1}.xbm"
    elif [ -f "${picturefolder}/${1}.gsc" ]; then                           # Lookup for an existing GSC and proceed
      PICFNAM="${picturefolder}/${1}.gsc"
    elif [ -f "${picturefolder}/${1}.xbm" ]; then                               # Lookup for an existing XBM and proceed
      PICFNAM="${picturefolder}/${1}.xbm"
    fi                                                                          # End if Picture check
    if [[ -n ${PICFNAM} ]]; then
      echo "Sending: CMDCOR,${1}"
      # PICSIZE=$(stat -c "%s" "${PICFNAM}")
      echo "CMDCOR,${1}" > ${TTYDEV}                                            # Send CORECHANGE" Command and Corename
      sleep ${WAITSECS}                                                         # sleep needed here ?!
      # echo ${PICSIZE} > ${TTYDEV}                                               # Send CORECHANGE" Command and Corename
      # sleep ${WAITSECS}                                                         # sleep needed here ?!
      echo "Sending: ${PICFNAM}"
      tail -n +4 "${PICFNAM}" | xxd -r -p > ${TTYDEV}                           # /dev/shm
    else                                                                        # No Picture available!
      echo "${1}" > ${TTYDEV}                                                   # Send just the CORENAME
    fi
  else                                                                          # SD/Standard Mode ? Just send the Corename
    echo "${1}" > ${TTYDEV}                                                     # Instruct the device to load the appropriate picture from SD card
  fi
}

# Init tty2oled
stty -F ${TTYDEV} ${BAUDRATE} ${TTYPARAM}

#Just send the Data
senddata $1
