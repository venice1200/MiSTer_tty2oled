#!/bin/bash

# /media/fat/Scripts/tty2oled_
# by venice
# tty2oled Updater/Utility Menu
# v0.1

. /media/fat/Scripts/tty2oled.ini

#Colors
fblink=`tput blink`
fbold=`tput bold`
freset=`tput sgr0`
finvers=`tput rev`
fblue=`tput setf 1`
fgreen=`tput setf 2`
fyellow=`tput setf 5`
fred=`tput setf 4`

slidewait=6
menuwait=2
counter=0

function parse_cmd() {
  if [ ${#} -gt 2 ]; then # We don't accept more than 2 parameters
    echo "Too much parameter given"
  elif [ ${#} -eq 0 ]; then # Show Main
    tty_main
  else
    while [ ${#} -gt 0 ]; do
      case ${1,,} in
        main)
          tty_main
          break
          ;;
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
          echo "${fred}Reboot${freset} ESP32"
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
  Disable "Disable tty2oled Daemon at boot" \
  Enable "Enable tty2oled Daemon at boot" \
  Slide "Start Slideshow" \
  Update "Update tty2oled" \
  Main "Back to Main Menu/Updater" \
  Exit "Exit now" 2>"/tmp/.TTYmenu"
  menuresponse=$(<"/tmp/.TTYmenu")
  #echo "Menuresponse: ${menuresponse}"
  parse_cmd ${menuresponse}
}

function tty_slideshow() {
  clear
  ${INITSCRIPT} stop
  echo "Show each ${slidewait} seconds a new Picture"
  if [ -z "$(ls -A ${picturefolder_pri})" ]; then
    echo "" 
    echo "${fred}No Pictures found${freset} in your Private Picture Folder (pri) "
    echo ""
  else
    # Private Pictures
    cd ${picturefolder_pri}
    for slidepic in *.xbm; do
      counter=$((counter+1))
      echo "Showing Picture ${counter}: ${fgreen}${slidepic}${freset} (pri Folder)"
      echo "CMDCOR,${slidepic}" > ${TTYDEV}
      sleep ${WAITSECS}
      tail -n +4 "${slidepic}" | xxd -r -p > ${TTYDEV}
      sleep ${slidewait}
    done
  fi

  # General Pictures
  cd ${picturefolder}
  for slidepic in *.xbm; do
    counter=$((counter+1))
    echo "Showing Picture ${counter}: ${fyellow}${slidepic}${freset}"
    echo "CMDCOR,${slidepic}" > ${TTYDEV}
    sleep ${WAITSECS}
    tail -n +4 "${slidepic}" | xxd -r -p > ${TTYDEV}
    sleep ${slidewait}
  done
  ${INITSCRIPT} start
}

function tty_showpic() {
  echo "${fgreen}Showing Picture ${1}${freset}"
  if [ -f "${picturefolder_pri}/${1}.xbm" ]; then
    echo "CMDCOR,${1}" > ${TTYDEV}
    sleep ${WAITSECS}
    tail -n +4 "${picturefolder_pri}/${1}.xbm" | xxd -r -p > ${TTYDEV}
  elif [ -f "${picturefolder}/${1}.xbm" ]; then
    echo "CMDCOR,${1}" > ${TTYDEV}
    sleep ${WAITSECS}
    tail -n +4 "${picturefolder}/${1}.xbm" | xxd -r -p > ${TTYDEV}
  else
    echo "${fred}No Picture ${1} found${freset}"
  fi
  exit 0
}

function tty_update() {
  clear
  ${UPDATESCRIPT}
  exit 0
}

parse_cmd ${@}  # Parse command line parameters for input
exit
