#!/bin/bash

[[ $# -ne 1 ]] && echo "1 param expected" && exit 1
[[ ! $1 =~ ^[a-zA-Z0-9]{1,4}$ ]] && echo "invalid 1st param" && exit 2

file="/proc/acpi/wakeup"

deviceLine=$( egrep "^$1" $file)
[[ -z $deviceLine ]] && echo "Device: $1  not found" && exit 3

column=$(echo "$deviceLine" | awk 'END { print $3 }')

[[ $column == "*disabled" ]] && echo "Device: $1 already disabled" && exit 0

echo "$1" > $file
