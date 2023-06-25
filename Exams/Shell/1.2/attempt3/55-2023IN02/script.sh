#!/bin/bash

[[ $# -ne 1 ]] && echo "1 param expected" && exit 1
[[ ! -d $1 ]] && echo "The first param must be a directory" && exit 2

DIR=$(mktemp -d)

files=$(find $1 -type f -printf '%p %i %n\n')

while read file inode count; do
	sha=$(sha256sum $file | cut -d ' ' -f 1)

	echo "$inode $count" >> $DIR/$sha
done < <(echo "$files")

files=$(find $DIR -type f)

while read sha; do
	contents=$(cat "$sha" | sort -nu)

	ones=$(echo "$contents" | egrep " 1$" | cut -d ' ' -f 1)
	notOnes=$(echo "$contents" | egrep -v " 1$" | cut -d ' ' -f 1)
	
	if [[ -z $notOnes ]]; then
		toDelete=$(echo "$ones" | tail -n +2)

		[[ -z $toDelete ]] && echo "File with inum: $ones is only 1" && continue

		while read inode; do
			find $1 -type f -inum "$inode" 
		done < <(echo "$toDelete")
	elif [[ -z $ones ]]; then
		while read inode; do
			find $1 -type f -inum "$inode" | head -n 1
		done < <(echo "$notOnes")
	else
		while read inum; do
			find $1 -type f -inum "$inode"
		done < <(echo "$ones")

		while read inum; do
			find $1 -type f -inum "$inode" | head -n 1
		done < <(echo "$notOnes")
	fi
done < <(echo "$files")
