#!/bin/bash

if [[ $# -ne 2 && $# -ne 1 ]]; then
	echo "1 or 2 Paramters expected"
	exit 1
fi

if [[ -z $1 || ! -d $1 ]]; then
	echo "The 1st Parameter must not be empty and must be a directory"
	exit 2
fi

if [[ ! -z $2 && ! -f $2 ]]; then
	echo "The 2nd Parameter must be a file"
	exit 3
fi

symlinks=$(find $1 -type l -printf '%f -> %l\n' 2>/dev/null)

validSymlinks=$(echo "$symlinks" | egrep '^.* -> .+$')
brokenSymlinks=$(echo "$symlinks" | egrep ' -> $')
brokenSymlinksCount=$(echo "$brokenSymlinks" | wc -l)

#validSymlinks=$(echo "$symlinks" | egrep -v "$brokenSymlinks")

if [[ -z $2 ]]; then
	echo "$validSymlinks"
	echo "Broken Symlinks Count: $brokenSymlinksCount"
else
	echo "$validSymlinks" > $2
	echo "Broken Symlinks Count: $brokenSymlinksCount" >> $2
fi
