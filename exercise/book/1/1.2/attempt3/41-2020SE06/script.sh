#!/bin/bash

[[ $# -ne 3 ]] && echo "3 params expected" && exit 1

[[ ! -f $1 ]] && echo "The 1st param must be a file" && exit 2

[[ -z $2 || -z $3 ]] && echo "The 2nd and 3rd params must be non empty strings" && exit 3

line=$(egrep "^\s*\b$2\b\s+=.*" $1)
user=$(whoami)
date=$(date)

if [[ -z $line ]]; then
	echo "$2 = $3 # added at $date by $user" >> $1
else
	key=$(echo "$line" | cut -d '=' -f 1 | sed -E -e "s/^\s+//" -e "s/\s+$//g" -e "s/\s+/ /g")
	value=$(echo "$line" | cut -d '#' -f 1 | cut -d '=' -f 2 | sed -E -e "s/^\s+//" -e "s/\s+$//g" -e "s/\s+/ /g")
	
	[[ $value == $3 ]] && exit 0

	sed -i -E "/$line/{
		s/^/# /; 
		s/$/ # edited at $date by $user/; 
		a $2 = $3 # added at $date by $user
	}" $1
fi
