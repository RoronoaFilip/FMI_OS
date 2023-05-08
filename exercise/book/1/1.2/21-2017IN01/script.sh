#!/bin/bash

line1=$(cat $1 | grep $2= | cut -d '=' -f 2)
line2=$(cat $1 | grep $3= | cut -d '=' -f 2)

if [[ $line2 == "" ]]; then
	exit 1;
fi

for curr in $line1; do
	line2=$(echo "$line2" | sed -E "s/($curr\s*)//g")
done

echo "$line2"

sed -E -i "s/^$3=.*$/$3=$line2/g" $1 
