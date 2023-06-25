#!/bin/bash

[[ $# -ne 1 ]] && echo "1 param expected" && exit 1

[[ ! -f $1 ]] && echo "the 1st param must be a file that exists" && exit 2

regex="^[0-9A-Z]{1,4}\s+(enabled|disabled).*$"

devicesLines=$(egrep "$regex" $1 | tr -s ' ')

[[ -z $devicesLines ]] && echo "No Devices Found in the config file" && exit 3

exe="/proc/acpi/wakeup"
exe="exe.exe"
while read line; do 
	device=$(echo "$line" | cut -d ' ' -f 1)
	word=$(echo "$line" | cut -d ' ' -f 2)

	if setting=$(egrep "^$device" $exe); then
		
		egrep -q "\*$word" <(echo "$setting") &&  echo "Device: $device is already $word" && continue
	fi

	echo "$device > $exe"
	#echo "$device" > $exe
done < <(echo "$devicesLines")
