#!/bin/bash

[[ ! -d $1 ]] && echo "1st param is not a dir" && exit 1

DIR=$(mktemp -d)
fileInodeHardlinks=$(find $1 -type f -printf '%i %n %p\n')

while read inode count name; do
	#name=$(echo "$file" | awk '{ print $3 }')
	#inode=$(echo "$file" | awk '{ print $1 }')
	#count=$(echo "$file" | awk '{ print $2 }')
	
	sha=$(sha256sum "$name" | cut -d ' ' -f 1)

	echo "$inode $count" >> "$DIR/$sha"
done < <(echo "$fileInodeHardlinks")

hashFiles=$(find $DIR -type f)

while read hash; do
	file=$(cat $hash | sort -u)

	ones=$(echo "$file" | egrep " 1$" | cut -d ' ' -f 1)
	notOnes=$(echo "$file" | egrep -v " 1$" | cut -d ' ' -f 1)
		
	if [[ -z "$notOnes" ]]; then
		toDelete=$(echo "$ones" | tail -n +2)

		[[ -z $toDelete ]] && echo "File with Inode: $ones is just 1" && continue
		
		while read inode; do
			find $1 -inum "$inode"
		done < <(echo "$toDelete")
	elif [[ -z "$ones" ]]; then

		while read inode; do
			find $1 -inum "$inode" | head -n 1
		done < <(echo "$notOnes")

	else
        while read inode; do
            find $1 -inum "$inode"
        done < <(echo "$ones")

		while read inode; do
            find $1 -inum "$inode" | head -n 1
        done < <(echo "$notOnes")
	fi
done < <(echo "$hashFiles")

rm -r $DIR
