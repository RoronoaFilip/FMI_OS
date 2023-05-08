#!/bin/bash

#if [[ $(whoami) != "root" ]]; then
#	echo "You are not root"
#	exit 1
#fi

users=$(ps -eo user= | egrep -v "root" | sort -u)
rootSum=$(ps -u "root" -o rss= | awk '{count+=$0} END {print count}')

echo "Root Rss sum: $rootSum"

echo $users
echo ""

for user in $users; do
	userHomeDir=$(cat /etc/passwd | egrep "$user" | cut -d ':' -f 6)

	dirOwner=$(stat $userHomeDir --printf '%U')

	dirUserPerms=$(stat $userHomeDir --print '%a' | sed -E 's/^(.).*/\1/g')

	#echo "$dirUserPerms"

	final=""
	if [[ ! -d $userHomeDir || $dirOwner != $user || !  $dirUserPerms =~ ^[2367]$ ]]; then
		final=$user
	fi

	if [[ ! -z $final ]]; then
		rssSum=$(ps -u $final -o rss= | awk '{count+=$0} END {print count}')
		echo "User: $user HomeDir: $userHomeDir DirOwner: $dirOwner DirUserPerms: $diruserPerms RssSum: $rssSum"
		
		if [[ $rootSum -lt $rssSum ]]; then
			pids=$(ps -u $user -o pid=)
			#kill -TERM $pids
			#sleep 2
			#kill -KILL $pids
		fi
	fi
done

