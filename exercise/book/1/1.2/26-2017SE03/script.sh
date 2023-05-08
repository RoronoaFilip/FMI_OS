#!/bin/bash

#if [[ ! $(whoami) == "root" ]]; then
#	echo "You are not root"
#	continue
#fi

userProcesses=$(ps -eo user= | sort | uniq -c | sed -E 's/^\s+//g')

echo "$userProcesses"

while read count user
do
	echo ""
	echo "User - $user, Count - $count"
	processes=$(ps -u "$user" -o pid=,rss= | tr -s ' ' | sed -E 's/^\s+//g' | sort -t ' ' -k 2 -n)
	sum=$(echo "$processes" | awk '{sum+=$2} END {print sum}')
	echo "For User $user: Processes Count - $count; RSS Sum - $sum"
	
	#echo "For User: $user :"
	#echo "$processes"
	#echo ""

	avg=$(echo "$sum $count" | awk 'END {print $1/$2}')

	maxPID=$(echo "$processes" | tail -n 1 | cut -d ' ' -f 1)
	maxMEM=$(echo "$processes" | tail -n 1 | cut -d ' ' -f 2)
	
	echo "AVG: $avg; BiggestPS: $maxPID $maxMEM"

	if [[ $(echo "$avg" | awk 'END {print int($1)*2}') -ne $maxMEM ]]; then 
		#kill -TERM $biggestPID
		#sleep 2
		#kill -KILL $biggestPID
		echo "To Kill - $maxPID $maxMEM"
	fi
done <<< "$userProcesses"

