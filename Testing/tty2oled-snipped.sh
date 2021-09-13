# USB Send-Picture-Data function
senddata() {
  if [ "${USBMODE}" = "yes" ]; then                                             # Check the tty2xxx mode
    if [ -f "${picturefolder_pri}/${1}.xbm" ]; then                             # Lookup for an existing XBM in PRI and proceed
      PICFNAM="${picturefolder_pri}/${1}.xbm"
      SENDPIC="yes"
    elif [ -f "${picturefolder_16gray}/${1}.xbm" ]; then                        # Lookup for an existing XBM and proceed
      PICFNAM="${picturefolder_16gray}/${1}.xbm"
      SENDPIC="yes"
    elif [ -f "${picturefolder}/${1}.xbm" ]; then                               # Lookup for an existing XBM and proceed
      PICFNAM="${picturefolder}/${1}.xbm"
      SENDPIC="yes"
    fi                                                                          # End if Picture check
    if [ "${SENDPIC}" = "yes" ]; then
      dbug "Sending: CMDCOR,${1}"
      tail -n +4 "${PICFNAM}" | xxd -r -p > /dev/shm/corepic
      PICSIZE=$(stat -c "%s" /dev/shm/corname)
      echo "CMDCOR,${1}" > ${TTYDEV}                                            # Send CORECHANGE" Command and Corename
      sleep ${WAITSECS}                                                         # sleep needed here ?!
      echo ${PICSIZE} > ${TTYDEV}                                               # Send CORECHANGE" Command and Corename
      sleep ${WAITSECS}                                                         # sleep needed here ?!
      cat /dev/shm/corepic > ${TTYDEV}                                          # The Magic, send the Picture-Data up from Line 4 and proces$
      rm /dev/shm/corepic
      unset SENDPIC
    else                                                                        # No Picture available!
      echo "${1}" > ${TTYDEV}                                                   # Send just the CORENAME
    fi
  else                                                                          # SD/Standard Mode ? Just send the Corename
    echo "${1}" > ${TTYDEV}                                                     # Instruct the device to load the appropriate picture from SD card
  fi
}
