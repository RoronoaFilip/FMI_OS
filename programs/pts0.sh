#!/bin/bash

while true; do
	pts0=$(who | egrep " pts/0 ")

	[[ -z $pts0 ]] && break

	sleep 2
done
echo "Pts/0 is free"
