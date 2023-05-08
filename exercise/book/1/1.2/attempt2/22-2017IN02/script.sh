#!/bin/bash

[[ $# -ne 1 ]] && echo "1 param expected" && exit 1

processes=$(ps -e -o user=,etimes=,pid= | sed -E -e 's/^\s+//g' -e 's/\s+$//g' -e 's/\s+/ /g')

fooProcesses=$(echo "$processes" | egrep "^$1")
fooProcessesCount=$(echo "$processes" | wc -l)

otherProcessesCount=$(echo "$processes" | cut -d ' ' -f 1 | sort | uniq -c | sed -E 's/^\s+//g')
echo "$otherProcessesCount" | awk -v fooCount="$fooProcessesCount" '{ if($1 > fooCount) { print $2 }}'

etimesAvgSec=$(echo "$processes" | cut -d ' ' -f 2 | awk '{ count += $0 } END {print int(count/NR)}')

hhmmss=$(echo "$etimesAvgSec" | awk 'END { printf("%02d:%02d:%02d", ($0/3600), ($0/3600%60), ($0%60)) }')

echo "HH:MM:SS - $hhmmss"

doubleAvgSec=$((2*$etimesAvgSec))

while read line; do
	pid=$(echo "$line" | cut -d ' ' -f 3)
	time=$(echo "$line" | cut -d ' ' -f 2)
	
	[[ $time -gt $doubleAvgSec ]] && echo "User: $1 pid: $pid; TIME: $time" #&& kill -KILL $pid
done < <(echo "$fooProcesses") 
