#!/bin/bash

. /media/fat/tty2oled/tty2oled-system.ini
. /media/fat/tty2oled/tty2oled-user.ini

# Start
start() {
	echo "Starting tty2oled-read Daemon..."
	if [[ -c ${TTYDEV} ]]; then
		cat ${TTYDEV} > /tmp/tty2oled-read & echo $! > /run/tty2oled-read.pid
	fi
}

# Stop
stop() {
	echo "Stopping tty2oled-read Daemon..."
	kill $(cat /run/tty2oled-read.pid) && rm /run/tty2oled-read.pid && > /tmp/tty2oled-read
	#ps | grep "cat ${TTYDEV}" | head -n 1 | awk '{print $1}' | xargs kill
}

clear() {
	echo "Clearing tty2oled-read buffer file..."
	> /tmp/tty2oled-read
}

case "$1" in
	start)
        start
        ;;
	stop) 
        stop
        ;;
	clear)
		clear
		;;
	restart)
        stop
        start
        ;;
	*)
        echo "Usage: $0 {start|stop|restart|clear}"
        exit 1
esac
