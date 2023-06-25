#!/bin/bash


[[ $# -ne 2 ]] && echo "2 params expected" && exit 1
[[ ! -d $1 ]] && echo "1st param must be a dir" && exit 2

[[ ! -d $1/packages ]] && echo "packages folder not found" && exit 3
[[ ! -f $1/db ]] && echo "db file not found" && exit 3

[[ ! -d $2/tree ]] && echo "tree folder not found" && exit 4
[[ ! -f $2/version ]] && echo "version file not found" && exit 3

version=$(cat $2/version)
name=$(basename $2)
name="$name-$version"
tarName="$name.tar.xz"

mv $2/tree .
tar -acvf $tarName tree
mv tree $2

sha=$(sha256sum $tarName | cut -d ' ' -f 1)

dbLine="$name $sha"

actualLine=$(egrep "^$name" $1/db)

[[ $actualLine == $dbLine ]] && rm $tarName && echo "Archive is already up-to date"  && exit 0

if [[ -z $actualLine ]]; then
	echo "$dbLine" >> $1/db
	sort -V -o "$1/db" "$1/db"
else
	currSha=$(echo "$actualLine" | cut -d ' ' -f 1)
	sed -i -E "s/$currSha/$sha/g" $1/db	
fi

mv $tarName $sha.tar.xz
mv $sha.tar.xz $1/packages
