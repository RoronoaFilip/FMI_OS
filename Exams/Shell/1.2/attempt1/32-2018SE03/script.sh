#!/bin/bash

#lines=$(cat $1 | awk -v FS="," '{i=substr($0, length($1)+2); if(arr[i]>$1 || arr[i]==""){arr[i]=$1; answer[i]=$0}} END {for (var in answer){print answer[var]}}')
#echo "$lines"

if [[ ! -f $1 || ! -f $2 ]]; then
	echo "2 Files expected"
	exit 1
fi

if [[ ! $1 =~ ^.*\.csv$ || ! $2 =~ ^.*\.csv$ ]]; then
	echo "2 CSV files expected"
	exit 2
fi

lines=$(cat $1)
uniqLines=$(echo "$lines" | cut -d ',' -f 2- | sort -u)

echo "$uniqLines"

while read line; do
	#echo $line

	currLinesSorted=$(echo "$lines" | egrep "^[0-9]+,$line$" | sort -n -t ',' -k 1)
	#echo "$currLinesSorted"
	echo "$currLinesSorted" | head -n 1 >> $2
done <<< "$uniqLines"
