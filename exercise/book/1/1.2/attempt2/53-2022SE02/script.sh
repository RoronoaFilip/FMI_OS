#!/bin/bash

[[ $# -ne 2 ]] && echo "2 params expected" && exit 1
[[ ! -d $1 || ! -d $1/1 || ! -d $1/2 || ! -d $1/3 || ! -d $1/0 ]] && echo "Backups Folders not found" && exit 2
[[ ! $2 =~ ^[0-9]+$ ]] && echo "2nd param is not a number" && exit 3

brokenLinks=$(find $1 -type l -printf '%p->%Y\n' | egrep "${regex}\->(L|N|\?)$" | sed -E 's/->.$//g') 
if [[ ! -z $brokenLinks ]]; then
	while read link; do
		rm $link
	done < <(echo "$brokenLinks")
fi

function getPercent() {
	output=$(df $1 | tail -n +2 | head -n 1 | sed -E 's/\s+/ /g')
	percent=$(echo "$output" | cut -d ' ' -f 5 | sed -E 's/%$//g')
	echo "$percent"
}

currPercent=$(getPercent $1)
[[ $currPercent -lt $2 ]] && echo "% used space is already lower that $2" && exit 0

function processDir() {
	echo "Processing $4 backups"
	local regex="^[^ \.\-]+\-[^ \.\-]+\-[0-9]{8}\.tar\.xz$"
	
	objects=$(find $1 -type f -exec basename {} \; | egrep "$regex" |  cut -d '-' -f 1,2 | sort -u)
	while read object; do
		backups=$(find $1 -type f | egrep "$object\-[0-9]{8}\.tar\.xz$" | sort -n -t '-' -k 3 | head -n -$2)
		
		while read backup; do
			rm $backup
			currPercent=$(getPercent $1)
 			[[ $currPercent -lt $3 ]] && echo "% used space is now lower than $3 after deleting backup $backup from $4 backups" && exit 0
		done < <(echo "$backups")
	done < <(echo "$objects")
}

processDir "$1/0" 1 $2 yearly
processDir "$1/1" 2 $2 monthly
processDir "$1/2" 3 $2 weekly
processDir "$1/3" 4 $2 daily

echo "% used space could not reach $2 but as many backups as possible were deleted" && exit 0
