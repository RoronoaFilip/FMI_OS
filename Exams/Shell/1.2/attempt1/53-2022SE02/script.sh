#!/bin/bash

getPercent() {
	output=$(df $1 | tail -n +2 | head -n 1 | tr -s ' ')
	percent=$(echo "$output" | cut -d ' ' -f 5 | sed -E 's/^(.+)%$/\1/g')
	echo "$percent"
}

if [[ $# -ne 2 ]]; then
	echo "2 params expected"
	exit 1
fi

if [[ ! -d $1 ]]; then
	echo "1st param must be a dir"
	exit 2
fi

if [[ ! $2 =~ ^[0-9]+$  || $2 -lt 0 || $2 -gt 100 ]]; then
	echo "2nd param must be a number in percent"
	exit 3
fi

#remove broken symlinks
tarXzRegex="[^-]+-[^-]+-[0-9]{8}\.tar\.xz"
brokenSymlinks=$(find $1 -type l -printf '%p -> %Y\n' | egrep "$tarXzRegex -> ($|N|\?)" | sed -E 's/^(.*) -> .*$/\1/g')
echo "$brokenSymlinks"
echo "$brokenSymlinks" | xargs -I @ rm @ #deleting broken symlinks

percent=$(getPercent $1)
echo "$percent"
[[ $percent -le $2 ]] && echo "Percent is already lower than $2" && exit 0

yearlyFolder="$1/0"
monthlyFolder="$1/1"
weeklyFolder="$1/2"
dailyFolder="$1/3"

deleteBackups() {
	# $1 - folder; $2 - the word yearly, monthly, weekly or daily; $3 - the number of backups needed to be a proper backup
	# $4 - the wanted %; $5 - the input dir
	echo "Processing $2 backups"
	local allBackups=$(find $1 ! -type l | egrep "$tarXzRegex$")
	local uniqueNames=$(echo "$allBackups" | cut -d '-' -f 1,2 | sort -u) #only the names distinct

	while read name; do
        	local backups=$(find $1 ! -type l | egrep "$name\-[0-9]{8}\.tar\.xz$")
    	    local backups=$(echo "$backups" | sort -n -t '-' -k 3 | head -n -$3) #everything except the latest $3 we can delete freely since we need atleast $3 left
		
        	while read backup; do
            	    rm "$backup"
        	        local percent=$(getPercent $5)
    	            [[ $percent -le $4 ]] && echo "$4 Percent reached after deleting $backup from $2 backups" && exit 0
	        done < <(echo "$backups")

	done < <(echo "$uniqueNames")
}

deleteBackups $yearlyFolder yearly 1 $2 $1
deleteBackups $monthlyFolder monthly 2 $2 $1
deleteBackups $weeklyFolder weekly 3 $2 $1
deleteBackups $dailyFolder daily 4 $2 $1

percent=$(getPercent $1)
[[ $percent -ge $2 ]] && echo "$2 Percent was not reached but as many backups as possible were deleted" && exit 0
