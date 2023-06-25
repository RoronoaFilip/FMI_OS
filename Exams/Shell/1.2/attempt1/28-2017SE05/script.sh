#!/bin/bash


if [[ $# -ne 2 ]]; then
	echo "2 Parameters expected"
	exit 1
fi

if [[ ! -d $1 ]]; then
	echo "1st Parameter must be a dir"
	exit 2
fi

if [[ -z $2 ]]; then
	echo "2nd Parameter must not be an empty string"
	exit 3
fi

files=$(find $1 -mindepth 1 -maxdepth 1 -printf '%P\n'| egrep "^vmlinuz-[0-9]+\.[0-9]+\.[0-9]+-$2")

#echo "$files"

max=$(echo "$files" | cut -d '-' -f 2 | sort -n | tail -n 1)

echo "$files" | egrep "$max"
