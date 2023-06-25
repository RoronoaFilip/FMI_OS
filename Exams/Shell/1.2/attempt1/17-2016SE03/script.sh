#!/bin/bash


users=$(cat /etc/passwd | cut -d ':' -f 1,6 | sed -E 's/:/ /g')

while read user dir; do
	if [[ ! -d $dir ]]; then
		echo "$dir does not exist"
	else
		perm=$(stat $dir --printf '%a' | sed -E 's/^(.).*/\1/g')
		#echo "Permissions for folder $dir are $perm"
		[[ ! $perm =~ [7632] ]] && echo "User does not have permission to write in $dir"
	fi

done <<< "$users"
