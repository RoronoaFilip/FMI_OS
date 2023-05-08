#!/bin/bash

state="0"
options=""
args=""
file=""
for el in $@; do
	if [[ $state -eq 0 ]]; then
		[[ $el == "-jar" ]] && state="1" && continue

		[[ $el =~ ^-[^D].+=.+$ ]] && options="$options $el"
	elif [[ $state -eq 1 ]]; then
		[[ $el =~ \.jar$ ]] && state="2" && file="$el" && continue
		
		options="$options $el"
	else
		args="$args $el"
	fi
done

[[ $state -ne 2 ]] && echo "-jar option or jar file not specified" && exit 1

comm=$(echo "java $options -jar $file $args" | sed -E -e 's/^\s+//g' -e 's/\s+$//g' -e 's/\s+/ /g')

echo "$comm"
