#!/bin/bash

files=$*
N="10"

if [[ $1 == '-n' ]]; then
	N=$2
	files=$(echo $* | cut -d ' ' -f 2-)
fi

output=""
for file in $files; do
	add=$(cat $file | tail -n $N | awk -v file="$file" '{rest=""; for(i=3; i<=NF; ++i) {rest=rest" "$i}; printf("%s %s %s %s\n", $1, $2, file, rest)}')

	output+="\n$add"
done

answer=$(echo "$output"| tail -n +2  | sort -n)
echo -e "$answer"
