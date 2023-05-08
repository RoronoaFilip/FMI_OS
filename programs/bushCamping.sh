#!/bin/bash

while ! who | egrep "$1"; do
	sleep 5
done

echo "$1 is online"

./spam.sh $(who | egrep "$1" | tr -s ' ' |  cut -d ' ' -f 1) &
