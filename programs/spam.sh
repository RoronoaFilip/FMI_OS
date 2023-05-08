#!/bin/bash

while true; do
	pts=$(who | egrep "$1" | tr -s ' ' | cut -d ' ' -f 2)

    echo "You have been hacked!" | write $1 $pts 1>/dev/null 2>&1

    sleep 2
done
