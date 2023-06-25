#!/bin/bash

[[ $# -ne 2 ]] && echo "2 params expected" && exit 1
[[ ! -d $1 || ! -d $2 ]] && echo "the 2 params must be dirs" && exit 2

[[ ! -z $(find $2 -mindepth 1 -maxdepth 1) ]] && echo "2nd param must be an empty dir" && exit 3

files=$(find $1 -type f -printf '%P\n')

#echo "$files"

for file in $files; do
	if [[ $file =~ \..+\.swp$ ]]; then
		actualName=$(echo "$file" | sed -E 's/[^\.]*\.(.+)\.swp$/\1/g')
		realFile=$(echo "$files" | egrep "$actualName$")
		
		[[ ! -z $realFile ]] && continue
	fi
	
	dirName=$(dirname "$file")
    dirName="$2/${dirName}"
	mkdir -p "$dirName"
	
	originalFilePath="$1/${file}"
	
	cp $originalFilePath $dirName
done
