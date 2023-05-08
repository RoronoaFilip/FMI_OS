#!/bin/bash

version=$(cat $2/version)

name=$(basename $2)
name="$name-$version"

tarName="$name.tar.xz"

tar -acf "$tarName" $2/tree

shaSum=$(sha256sum $tarName | cut -d ' ' -f 1)
shaSumTar="$shaSum.tar.xz"

mv $tarName $shaSumTar

db=$(cat $1/db)

samePackage=$(echo "$db" | egrep "^$name")

#echo "$samePackage"
if [[ -z $samePackage ]]; then
	echo "$name $shaSum" >> "$1/db"
	sort -V "$1/db" -o "$1/db"
else
	fileToDelete=$(echo "$samePackage" | cut -d ' ' -f 2)
	
	[[ $fileToDelete == $shaSum ]] && echo "Archive not changed" && rm $shaSumTar && exit 0

	rm "$1/packages/$fileToDelete.tar.xz"

	sed -i "s/^$name .*/$name $shaSum/g" $1/db
fi

mv $shaSumTar "$1/packages/"
