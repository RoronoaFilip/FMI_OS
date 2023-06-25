#!/bin/bash

if [[ $# -ne 2 ]]; then
	echo "2 Parameters expected"
	exit 1
fi

if [[ ! -f $1 || ! $(file $1) =~ "text" ]]; then
	echo "1st Param must be a text file"
	exit 2
fi

if [[ ! -d $2 || ! -z $(find $2 -mindepth 1 -maxdepth 1) ]]; then
	echo "2nd Param must be an empty Dir"
	exit 3
fi

w="[a-zA-Z]"
lines=$(cat $1 | egrep "^${w}+\s+${w}+.*:.*")
uniqueNames=$(echo "$lines" | sed -E "s/^(${w}+\s+${w}+).+$/\1/g" | sort -u)

uniqueNamesWithNumber=$(echo "$uniqueNames" | awk -v OFS=";" '{print $0,NR}')
echo "$uniqueNamesWithNumber" > $2/dict.txt

while read nameNumber; do
	name=$(echo "$nameNumber" | cut -d ';' -f 1)
	num=$(echo "$nameNumber" | cut -d ';' -f 2)
	
	echo "$lines" | egrep "^$name" > $2/$num.txt
done < <(echo "$uniqueNamesWithNumber") 
