#!/bin/bash

echo "//
//
const unsigned char _bits[8192] PROGMEM = {" > "${1%.*}".gsc

tail -n+2 "${1}" >> "${1%.*}".gsc
sha1sum "${1%.*}".gsc | awk '{print $1" "$2}'
rm "${1}"
