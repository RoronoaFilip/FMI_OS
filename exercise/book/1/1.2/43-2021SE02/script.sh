#!/bin/bash

regex="^([a-z]+\.)+ ([0-9]+ )?IN (SOA|NS|A|AAAA) (([a-zA-Z0-9]+\.)+ )+(\(|([0-9]{10}) ([0-9]+ ){3}([0-9]+))$"

for file in $@; do
	soaLine=$(egrep "$regex" $file)
	lastChar=$(echo "$soaLine" | sed -E 's/^.*(.)$/\1/g')

	serial=""
	if [[ $lastChar == "(" ]]; then
		serial=$(cat $file | egrep '^.*;\s*serial' | sed -E -e 's/\s+//g' | cut -d ';' -f 1)	
	else
		serial=$(echo "$soaLine" | sed -E "s/^.* ([0-9]{10}) ([0-9]+ ){3}([0-9]+)$/\1/g")
	fi

	date=$(echo "$serial" | sed -E "s/^(.{8})..$/\1/g")
	num=$(echo "$serial" | sed -E "s/^.{8}(..)$/\1/g")
	
	echo "Serial: ${date}${num}"

	currDate=$(date +"%Y%m%d")
	
	newSerial=""
	if [[ "$date" -lt $currDate ]]; then
		newSerial="${currDate}00"
	else
		newNum=$(echo "$num" | awk ' 
		END {
		number=sprintf("%d", $0);
		if(number<99) {number++;}
		if(number<10) {print "0"number} else {print number};
		}')
			
		newSerial="${date}${newNum}"
	fi

	echo "New Serial: $newSerial"
	if [[ $lastChar != "(" ]]; then 
		sed -i -E "s/$serial/$newSerial/g" $file
	else
		sed -i -E "s/$serial/$newSerial/g" $file
	fi
done
