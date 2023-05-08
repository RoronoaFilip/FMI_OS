#!/bin/bash

if [[ $# -ne 1 ]]; then
	echo "1 param expected"
	exit 1
fi

if [[ ! -d $1 ]]; then
	echo "the first param must be a dir"
	exit 2
fi

friends=$(find $1 -type f -iname '*.txt' -exec dirname {} \; | xargs -I @ basename @ | sort -u)
tempFile=$(mktemp)
while read friend; do
	files=$(find $1 -ipath "*/$friend/*" -type f | egrep "[0-9]{4}(-[0-9]{2}){5}\.txt$")
	lines=$(echo "$files" | xargs -I @ wc -l @ | cut -d ' ' -f 1)
	
	echo "Friend: $friend Files: $files"

	sum=$(echo "$lines" | awk '{count+=$0} END {print count}')
	echo -e "$friend $sum\n" >> $tempFile
done < <(echo "$friends")

top10=$(cat $tempFile | sort -nr -t ' ' -k 2 | head -n 10)
echo "$top10"
rm $tempFile
