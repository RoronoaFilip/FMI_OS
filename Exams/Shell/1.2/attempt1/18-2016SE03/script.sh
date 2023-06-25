#!/bin/bash

# folders reset: mv a/file1 b/file2 c/file3 . ; rm -r a b c

if [[ $# -ne 2 ]]; then
	echo "2 Parameters needed"
	exit 1
fi

if [[ ! $1 =~ ^[0-9]+$ || ! $2 =~ ^[0-9]+$ ]]; then
	echo "One of the parameters in not a number"
	exit 2
fi

mkdir a b c

files=$(find . -type f -printf '%P\n')

#files="$(find . -type f -printf '%p\n')"

[[ -z $files ]] && exit 3

while read name; do
	if [[ $0 =~ $name ]]; then
		continue
	elif [[ "$(cat $name | wc -l)" -lt $1 ]]; then 
		mv $name a
		echo "$name -> a"
	elif [[ "$(cat $name | wc -l)" -lt $2 ]]; then
		mv $name b
		echo "$name -> b"
	else
		mv $name c
		echo "$name -> c"
	fi
done <<< "$files"
