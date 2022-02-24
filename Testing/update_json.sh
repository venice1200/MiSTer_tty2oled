#!/bin/bash

MD5=$(md5sum update_tty2oled.sh | awk '{print $1}')
SIZE=$(ls -o update_tty2oled.sh | awk '{print $4}')
DATE=$(date +%s -r update_tty2oled.sh)

cp tty2oleddb.json-empty tty2oleddb.json
sed -i 's/"hash": "XXX"/"hash": "'${MD5}'"/' tty2oleddb.json
sed -i 's/"size": XXX/"size": '${SIZE}'/' tty2oleddb.json
sed -i 's/"timestamp": XXX/"timestamp": '${DATE}'/' tty2oleddb.json
