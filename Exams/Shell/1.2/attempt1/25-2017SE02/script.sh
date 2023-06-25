#!/bin/bash

[[ $# -ne 3 ]] && exit 1

if [[ ! -d $1 || ! -d $2 ]]; then
	echo "Directories expected"
	exit 2
fi

isEmpty=$(find $2 -printf '%P\n')

if [[ ! -z $isEmpty ]]; then 
	echo "DST must be empty"
	exit 3
fi

if [[ ! -z $3 ]]; then
	echo "String must be not empty"
	exit 4
fi

if [[ $(whoami) != "root" ]]; then 
	echo "You are not root"
	exit 5
fi

files=$(find $1 -iname *$3*  -printf '%P\n')

for file in "$files"; do
	name=$(basename $file)
	dir=$(dirname $file)

	mkdir -p $2$dir
	mv $1$file $2$dir$name
done
