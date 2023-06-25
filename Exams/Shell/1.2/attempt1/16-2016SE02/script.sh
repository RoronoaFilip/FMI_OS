#!/bin/bash
: '
if [[ $(whoami) != "root" ]]; then
	echo "You are not root"
	exit 1
fi
'
if [[ $# -ne 1 ]]; then 
	exit 2
fi

if [[ ! $1 =~ ^[0-9]+$ ]]; then 
	exit 3
fi


for user in $(ps -e -o user= | sort  | uniq); do
	#echo "$user"
	process=$(ps -u "$user" -o pid=,rss= | sort -t ' ' -k 2 -n | awk '{count+=$2} END {print count,$1}')

	pid=$(echo $process | cut -d ' ' -f 2)
	mem=$(echo $process | cut -d ' ' -f 1)

	[[ $mem -gt $1 ]] && echo "$pid" #kill $pid
done
