#!/bin/bash

logDir="extracted"
[[ ! -d $logDir ]] && mkdir $logDir

logFile="log.txt"
[[ ! -f $logFile ]] && touch $logfile

[[ $# -ne 1 ]] && echo "1 param expected" && exit 1
[[ ! -d $1 ]] && echo "the 1st param must be a dir" && exit 2

regex="[^_]+_report-[0-9]+\.tgz$"

archives=$(find $1 -type f | egrep "$regex")

while read archive; do
	currSha=$(sha256sum $archive | cut -d ' ' -f 1)
	basename=$(basename $archive)
	foundSha=$(egrep "$basename$" $logFile | cut -d ' ' -f 1)

	if [[ $currSha == $found ]]; then
		continue
	fi

	extractedFile=$(tar -xavf $archive "meow.txt" 2>/dev/null)
	[[ -z $extractedFile ]] && echo "meow.txt not found in $archive" && echo "" && continue

	newName=$(echo "$basename $extractedFile" | sed -E 's/^([^_]+_)report-([0-9]+)\.tgz.*(\..+)$/\1\2\3/g')
	
	mv $extractedFile $newName
	mv $newName $logDir

	if [[ -z $foundSha ]]; then
		echo "Adding a new File $newName and adding $sha $basename to log File"
		echo "$currSha $basename" >> $logFile
	else 
		echo "Updating archive $basename and file $newName"
		sed -E -i "s/$foundSha/$currSha/g" $logFile
	fi
done < <(echo "$archives")
