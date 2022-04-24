#!/bin/bash
#
# /media/fat/Scripts/tty2oled_cc.sh
# by venice
# tty2oled Utilities
# v0.1
# 
# 
# Slide "Start Slideshow" \

. /media/fat/tty2oled/tty2oled-system.ini
. /media/fat/tty2oled/tty2oled-user.ini

slidewait=2
menuwait=2

function parse_cmd() {
  if [ ${#} -gt 3 ]; then # We don't accept more than 2 parameters
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
          echo -e "Enable ${fblue}${blink}OTA${freset} on ESP32"
          echo "CMDENOTA" > ${TTYDEV}
          sleep ${menuwait}
          tty_menu
          break
          ;;
        reset)
          clear
          echo -e "${fred}Reset${freset} ESP32"
          echo "CMDRESET" > ${TTYDEV}
          sleep ${menuwait}
          tty_menu
          break
          ;;
        stop)
          clear
          echo -e "${fred}Stop${freset} ${DAEMONNAME}"
          ${INITSCRIPT} stop
          sleep ${menuwait}
          tty_menu
          break
          ;;
        start)
          clear
          echo -e "${fgreen}Start${freset} ${DAEMONNAME}"
          ${INITSCRIPT} start
          sleep ${menuwait}
          tty_menu
          break
          ;;
        restart)
          clear
          echo -e "${fred}Stop${freset} ${DAEMONNAME}"
          ${INITSCRIPT} stop
          echo -e "${fgreen}Start${freset} ${DAEMONNAME}"
          ${INITSCRIPT} start
          sleep ${menuwait}
          tty_menu
          break
          ;;
        disable)
          clear
          echo -e "${fred}Disable${freset} tty2oled at boot time"
          [[ -e ${INITSCRIPT} ]] && mv ${INITSCRIPT} ${INITDISABLED}
          if [ $? -eq 0 ]; then
            echo -e "...${fgreen}done.${freset}"
          else
            echo -e "...${fred}error!${freset}"
          fi      
          sleep ${menuwait}
          tty_menu
          break
          ;;
        enable)
          clear
          echo -e "${fgreen}Enable${freset} tty2oled at boot time"
          [[ -e ${INITDISABLED} ]] && mv ${INITDISABLED} ${INITSCRIPT}
          if [ $? -eq 0 ]; then
            echo -e "...${fgreen}done.${freset}"
          else
            echo -e "...${fred}error!${freset}"
          fi
          sleep ${menuwait}
          tty_menu
          break
          ;;
        slide_menu)
          slide_menu
          #sleep ${menuwait}
          tty_menu
          break
          ;;
        slide)
          slideshow ${2}
          sleep ${menuwait}
          #tty_menu
          break
          ;;
        showpic)
          showpic ${2}
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
  echo -e ' | ${fblue}tty2oled${freset} |---[]';
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
  #echo "Calling Utility Menu..."
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
  Slide_Menu "Picture Slideshow" \
  Update "Update tty2oled" \
  Exit "Exit now" 2>"/tmp/.TTYmenu"
  menuresponse=$(<"/tmp/.TTYmenu")
  #echo "Menuresponse: ${menuresponse}"
  parse_cmd ${menuresponse}
}

function slide_menu() {
  #echo "Calling Sildeshow Menu..."
  dialog --clear --no-cancel --ascii-lines --no-tags \
  --backtitle "tty2oled" --title "[ Slideshow ]" \
  --menu "Use the arrow keys and enter \nor the d-pad and A button" 0 0 0 \
  GSC "GSC Picture Slideshow" \
  XBM "XBM Picture Slideshow" \
  Exit "Exit now" 2>"/tmp/.TTYmenu"
  menuresponse=$(<"/tmp/.TTYmenu")
  #echo "Menuresponse: ${menuresponse}"
  #sleep 2
  if ! [ ${menuresponse} = "Exit" ]; then
    slideshow ${menuresponse}
  fi
}


function slideshow() {
  counter=0
  clear
  ${INITSCRIPT} stop
  echo -e "\nShow each ${fgreen}${slidewait}${freset} second(s) a new Picture\n"
  pfolder=${picturefolder}/${1}
  #pfolder=${1}
  cd ${pfolder}
  for ppri in xbm gsc; do
    for slidepic in *.${ppri}; do
      if [ -z "$(ls -A ${pfolder}/*.${ppri} 2> /dev/null)" ]; then
        echo -e "\n${fred}No *.${ppri} Pictures found${freset} in folder ${pfolder}\n"
      else
        counter=$((counter+1))
        echo -e "Showing ${ppri}-Picture ${counter}: ${fblue}${slidepic}${freset} (Folder ${pfolder})"
        #echo "CMDAPD,${slidepic}" > ${TTYDEV}
        echo "CMDCOR,${slidepic}" > ${TTYDEV}
        tail -n +4 "${slidepic}" | xxd -r -p > ${TTYDEV}
        waitfortty
        #echo "CMDSNAM" > ${TTYDEV}
        #waitfortty
        #sleep ${slidewait}
        #echo "CMDSPIC" > ${TTYDEV}
        #waitfortty
        sleep ${slidewait}
      fi
    done
  done
  ${INITSCRIPT} start
}

function showpic() {
  basepic="`basename ${1}`"
  echo -e "${fgreen}Showing Picture ${basepic}${freset}"
  if [ -f "${1}" ]; then
    echo "CMDCOR,${basepic}" > ${TTYDEV}
    tail -n +4 ${1} | xxd -r -p > ${TTYDEV}
    waitfortty
  else
    echo -e "${fred}No Picture ${basepic} found${freset}"
  fi
  exit 0
}

function tty_update() {
  clear
  ${UPDATESCRIPT}
  tty_menu
  #exit 0
}

function waitfortty() {
  #echo -n "Waiting for tty2oled Acknowledge... "
  read -d ";" ttyresponse < ${TTYDEV}                # The "read" command at this position simulates an "do..while" loop
  while [ "${ttyresponse}" != "ttyack" ]; do
    read -d ";" ttyresponse < ${TTYDEV}              # Read Serial Line until delimiter ";"
  done
  #echo -e "${fgreen}${ttyresponse}${freset}"
  ttyresponse=""
}


#------------------- Main Part --------------------

# Clear Serial Input Buffer
#echo "Clear Serial Input Buffer for Device ${TTYDEV}"
while read -t 0 sdummy < ${TTYDEV}; do continue; done
#sleep 2

parse_cmd ${@}  # Parse command line parameters for input
exit
