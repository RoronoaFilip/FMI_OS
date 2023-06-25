#!/bin/bash

numsRegex="([0-9]{10})(\s+[0-9]+){4}"
regex="^([a-zA-Z0-9]+\.)+"
regex="$regex\s+([0-9]+\s+)?"
regex="${regex}IN"
regex="$regex\s+(SOA)"
regex="$regex\s+(([a-zA-Z0-9]+\.)+\s+)+"
regex="$regex(\(|$numsRegex)$"

for file in $@; do
	line=$(cat $file | egrep "$regex")
	
	[[ -z $line ]] && echo "Invalid SOA File" && continue

	lastChar=$(echo "$line" | sed -E 's/.+(.)$/\1/')

	serial=""
	if [[ $lastChar != "(" ]]; then
		serial=$(echo "$line" | sed -E "s/.+$numsRegex$/\1/")
	else
		serial=$(egrep "\s*[0-9]{10}\s*;\s*serial" $file | cut -d ';' -f 1 | sed -E -e 's/^\s+//g' -e 's/\s+$//g')
	fi

	echo "Serial: $serial"

	date=$(echo "$serial" | cut -c 1-8)
	num=$(echo "$serial" | cut -c 9-10)
	currDate=$(date +"%Y%m%d")

	newSerial=""
	if [[ $currDate -gt $date ]]; then
		newSerial="${currDate}00"
	else
		num=$(printf "%02d" $((num+1)))
		newSerial="${date}${num}"
	fi
	
	echo "NewSerial: $newSerial"

	sed -E -i "s/$serial/$newSerial/" $file
done
