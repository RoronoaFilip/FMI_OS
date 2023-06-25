#!/bin/bash

[[ $# -ne 2 ]] && echo "2 params expected" && exit 1

[[ ! -d $1 || ! -d $2 ]] && echo "The 2 params must be dirs" && exit 2

repo=$(echo "$1" | sed -E 's/\/$//')
package=$(echo "$2" | sed -E 's/\/$//')

[[ ! -d $2/tree || ! -f $2/version ]] && echo "Package is invalid" && exit 3
[[ ! -d $1/packages || ! -f $1/db ]] && echo "Repo is invalid" && exit 4

version=$(cat $2/version)

files=$(find $2/tree -type f -printf '%p ')

packageVersion="${package}-${version}"

tar -acf $packageVersion.tar.xz $files
sha=$(sha256sum $packageVersion.tar.xz | cut -d ' ' -f 1)

mv $packageVersion.tar.xz $sha.tar.xz

currentVersionLine=$(egrep "^$packageVersion " $1/db)

if [[ -z $currentVersionLine ]]; then
	echo "$packageVersion $sha" >> $1/db
	sort -V -o $1/db $1/db
else
	currentSha=$(echo "$currentVersionLine" | cut -d ' ' -f 2)
	sed -i -E "s/$currentSha/$sha/" $1/db
	rm $1/packages/$currentSha.tar.xz
fi

mv $sha.tar.xz $1/packages

