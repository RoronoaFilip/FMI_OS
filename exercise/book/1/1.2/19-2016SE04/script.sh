#!/bin/bash

[[ $# -ne 2 ]] && exit 1

if [[ ! -f $1 || ! -f $2 ]]; then
	echo "1 of the files does not exist"
	exit 2
fi

count1=$(cat $1 | egrep $1 | wc -l)
count2=$(cat $2 | egrep $2 | wc -l)

fileName=$1

if [[ $count2 -gt $count1 ]]; then
	fileName=$2
fi

cat $fileName | sed -E 's/^[0-9]{1,4}г\. \w* - //g' | sort > $fileName.songs
