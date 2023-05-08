#!/bin/bash

[[ $# -ne 1 ]] && echo "1 param expected" && exit 1
[[ ! -f $1 ]] && echo "The param must be a file" && exit 2

regex="^[a-zA-Z0-9]{1,4}\s+(disabled|enabled)(\s*#.*)?$"

lines=$(cat $1 | egrep "$regex")
[[ -z $lines ]] && echo "No valid config lines found" && exit 3

file="/proc/acpi/wakeup"
file="exe.exe"
while read device setting; do
	#device=$(echo "$line" | awk 'END { print $1 }')
	#setting=$(echo "$line" | awk 'END { print $2 }')

	deviceLine=$(cat $file | egrep "^$device")
	[[ -z $deviceLine ]] && echo "Device $device not found in the wakeup file" && continue

	currSetting=$(echo "$deviceLine" | awk 'END { print $3 }' | sed -E 's/^\*//')

	[[ $currSetting == $setting ]] && echo "Device $device is already $setting" && continue

	#echo "$device" > $file
	echo "$device > $file"
	echo "Device $device is now $setting"
done < <(echo "$lines" | awk '{ print $1,$2 }')
