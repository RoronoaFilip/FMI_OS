#!/bin/bash

logFile="log.txt"
logDir="extracted"

if [[ ! -f $logFile ]]; then
	touch $logFile
fi

if [[ ! -d $logDir ]]; then
	mkdir $logDir
fi

if [[ $# -ne 1 || ! -d $1 ]]; then
	echo "1 Parameter expected that must be a dir"
	exit 1
fi

archives=$(find $1 -iname '*_report-*.tgz')
logs=$(cat $logFile)

for archive in $archives; do
	echo "Archive: $archive"
	
	sha=$(sha256sum $archive | cut -d ' ' -f 1)
	echo "SHA: $sha"	
	
	basename=$(basename $archive)
	echo "BASENAME: $basename"

	if  line=$(egrep $basename $logFile); then
		foundSha=$(echo $line | cut -d ' ' -f 1)
		echo "FOUNDSHA: $foundSha"	
		
		[[ $sha == $foundSha ]] && echo "Archive $archive is the same" && echo "Aborting" && echo "" && echo "" && continue

		echo "Updating File $newFileName from archive $archive"
		sed -i -E "s/$line/$sha $basename/g" $logFile
	else 
		echo "Creating File $newFileName from archive $archive"
		echo "$sha $basename" >> $logFile
	fi

	extractedFile=$(tar -xavf $archive)
	newFileName=$(echo "$basename" | sed -E -e 's/report-//g' -e 's/\.tgz//g')
	newFileName=$(echo "$extractedFile" | sed -E "s/^.*(\..*)$/$newFileName\1/g")	
	
	echo "Extracted: $extractedFile; NewFileName: $newFileName"

	mv $extractedFile $newFileName
	mv $newFileName $logDir

	echo ""
	echo ""
done
