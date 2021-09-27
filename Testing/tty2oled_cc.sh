#!/bin/bash
#
# /media/fat/Scripts/tty2oled_cc.sh
# by venice
# tty2oled Utilities
# v0.1
# 

. /media/fat/tty2oled/tty2oled.ini


slidewait=2
menuwait=2

function parse_cmd() {
  if [ ${#} -gt 2 ]; then # We don't accept more than 2 parameters
    echo "Too much parameter given"
  elif [ ${#} -eq 0 ]; then # Show Main
    #tty_main
    tty_menu
  else
    while [ ${#} -gt 0 ]; do
      case ${1,,} in
        menu)
          tty_menu
          break
          ;;
        update)
          tty_update
          break
          ;;
        ota)
          clear
          echo "Enable ${fblue}${blink}OTA${freset} on ESP32"
          echo "CMDENOTA" > ${TTYDEV}
          sleep ${menuwait}
          tty_menu
          break
          ;;
        reset)
          clear
          echo "${fred}Reset${freset} ESP32"
          echo "CMDRESET" > ${TTYDEV}
          sleep ${menuwait}
          tty_menu
          break
          ;;
        stop)
          clear
          echo "${fred}Stop${freset} ${DAEMONNAME}"
          ${INITSCRIPT} stop
          sleep ${menuwait}
          tty_menu
          break
          ;;
        start)
          clear
          echo "${fgreen}Start${freset} ${DAEMONNAME}"
          ${INITSCRIPT} start
          sleep ${menuwait}
          tty_menu
          break
          ;;
        restart)
          clear
          echo "${fred}Stop${freset} ${DAEMONNAME}"
          ${INITSCRIPT} stop
          echo "${fgreen}Start${freset} ${DAEMONNAME}"
          ${INITSCRIPT} start
          sleep ${menuwait}
          tty_menu
          break
          ;;
        disable)
          clear
          echo "${fred}Disable${freset} tty2oled at boot time"
          [[ -e ${INITSCRIPT} ]] && mv ${INITSCRIPT} ${INITDISABLED}
          if [ $? -eq 0 ]; then
            echo "...${fgreen}done.${freset}"
          else
            echo "...${fred}error!${freset}"
          fi      
          sleep ${menuwait}
          tty_menu
          break
          ;;
        enable)
          clear
          echo "${fgreen}Enable${freset} tty2oled at boot time"
          [[ -e ${INITDISABLED} ]] && mv ${INITDISABLED} ${INITSCRIPT}
          if [ $? -eq 0 ]; then
            echo "...${fgreen}done.${freset}"
          else
            echo "...${fred}error!${freset}"
          fi
          sleep ${menuwait}
          tty_menu
          break
          ;;
        slide)
          tty_slideshow
          sleep ${menuwait}
          tty_menu
          break
          ;;
        showpic)
          tty_showpic ${2}
          break
          ;;
        exit)
          clear
          echo "See you, bye...."
          exit 0
          break
          ;;
        *)
          echo "ERROR! ${1} is unknown."
          break
          ;;
      esac
    done
  fi
}

function tty_main() {
  clear
  echo -e ' +----------+';
  echo -e ' | \e[1;34mtty2oled\e[0m |---[]';
  echo -e ' +----------+';
  echo ""
  echo " Press UP for Utility Menu"
  echo " Press DOWN or wait for Update"
  echo ""

  for i in {10..1}; do
    echo -ne " Start tty2oled Update in ${i}...\033[0K\r"
    premenu="Update"
    read -r -s -N 1 -t 1 key
    if [[ "${key}" == "A" ]]; then
      premenu="Menu"
      break
    elif [[ "${key}" == "B" ]]; then
      premenu="Update"
      break
    fi
  done
  parse_cmd ${premenu}
}

function tty_menu() {
  echo "Calling Utility Menu"
  dialog --clear --no-cancel --ascii-lines --no-tags \
  --backtitle "tty2oled" --title "[ Utilities ]" \
  --menu "Use the arrow keys and enter \nor the d-pad and A button" 0 0 0 \
  Ota "Start ESP OTA (ESP32 only)" \
  Reset "Reset ESP (ESP32 only)" \
  Stop "Stop tty2oled Daemon" \
  Start "Start tty2oled Daemon" \
  Restart "Restart tty2oled Daemon" \
  Disable "Disable tty2oled at boot" \
  Enable "Enable tty2oled at boot" \
  Slide "Start Slideshow" \
  Update "Update tty2oled" \
  Exit "Exit now" 2>"/tmp/.TTYmenu"
  menuresponse=$(<"/tmp/.TTYmenu")
  #echo "Menuresponse: ${menuresponse}"
  parse_cmd ${menuresponse}
}

function tty_slideshow() {
  counter=0
  clear
  ${INITSCRIPT} stop
  echo -e "\nShow each ${fgreen}${slidewait}${freset} second(s) a new Picture\n"

  if [ -z "$(ls -A ${picturefolder_pri})" ]; then
    echo "" 
    echo "${fred}No Pictures found${freset} in your Private Picture Folder (pri) "
    echo ""
  else
    # Private Pictures
    cd ${picturefolder_pri}
    for slidepic in *.xbm; do
      counter=$((counter+1))
      echo "Showing Pri-Picture ${counter}: ${fblue}${slidepic}${freset} (pri Folder)"
      echo "CMDAPD,${slidepic}" > ${TTYDEV}
      tail -n +4 "${slidepic}" | xxd -r -p > ${TTYDEV}
      waitforttyack
      echo "CMDSNAM" > ${TTYDEV}
      waitforttyack
      sleep ${slidewait}
      echo "CMDSPIC" > ${TTYDEV}
      waitforttyack
      sleep ${slidewait}
    done
  fi

  # General Greyscale Pictures
  cd ${picturefolder}
  for slidepic in *.gsc; do
    counter=$((counter+1))
    echo "Showing Grayscale-Picture ${counter}: ${fyellow}${slidepic}${freset}"
    echo "CMDAPD,${slidepic}" > ${TTYDEV}
    tail -n +4 "${slidepic}" | xxd -r -p > ${TTYDEV}
    waitforttyack
    echo "CMDSNAM" > ${TTYDEV}
    waitforttyack
    sleep ${slidewait}
    echo "CMDSPIC" > ${TTYDEV}
    waitforttyack
    sleep ${slidewait}
  done

  # General Pictures
  cd ${picturefolder}
  for slidepic in *.xbm; do
    counter=$((counter+1))
    echo "Showing Picture ${counter}: ${fyellow}${slidepic}${freset}"
    echo "CMDAPD,${slidepic}" > ${TTYDEV}
    tail -n +4 "${slidepic}" | xxd -r -p > ${TTYDEV}
    waitforttyack
    echo "CMDSNAM" > ${TTYDEV}
    waitforttyack
    sleep ${slidewait}
    echo "CMDSPIC" > ${TTYDEV}
    waitforttyack
    sleep ${slidewait}
  done

  ${INITSCRIPT} start
}

function tty_showpic() {
  echo "${fgreen}Showing Picture ${1}${freset}"
  if [ -f "${picturefolder_pri}/${1}.xbm" ]; then
    echo "CMDCOR,${1}" > ${TTYDEV}
    tail -n +4 "${picturefolder_pri}/${1}.xbm" | xxd -r -p > ${TTYDEV}
    waitforttyack
  elif [ -f "${picturefolder}/${1}.xbm" ]; then
    echo "CMDCOR,${1}" > ${TTYDEV}
    tail -n +4 "${picturefolder}/${1}.xbm" | xxd -r -p > ${TTYDEV}
    waitforttyack
  else
    echo "${fred}No Picture ${1} found${freset}"
  fi
  exit 0
}

function tty_update() {
  clear
  ${UPDATESCRIPT}
  tty_menu
  #exit 0
}

function waitforttyack() {
  echo -n "Waiting for tty2oled..."
  while [ "${ttyresponse}" != "ttyack" ]; do
    read -d ";" ttyresponse < ${TTYDEV}              # Read Serial Line until delimiter ";"
  done
  echo "${fgreen}${ttyresponse}${freset}"
  ttyresponse=""
}

#------------------- Main Part --------------------

# Clear Serial Input Buffer
#echo "Clear Serial Input Buffer for Device ${TTYDEV}"
while read -t 0 sdummy < ${TTYDEV}; do continue; done
#sleep 2

parse_cmd ${@}  # Parse command line parameters for input
exit
