#!/bin/bash

dir="fuga"
filesPath="fuga/cfg"
validator="fuga/validate.sh"
passFile="fuga/foo.pwd"
configFile="fuga/foo.conf"

files=$(find $filesPath -type f -iname '*.cfg' -printf '%p ')
#echo $files
filesToConcat=""
for file in $files; do
	basename=$(basename $file)
	username=$(echo $basename | sed -E 's/\.cfg$//g')
	
	#echo "Basename:$basename Username:$username"

	errors=$($validator $file)
	errorCode=$?
	[[ $errorCode -eq 2 ]] && echo "Problem running validator for file $file" && continue
	
	if [[ $errorCode -eq 1  ]]; then
		echo "$errors" | awk -v basename="$basename" '{print basename":"$0}' 
		continue
	fi
	
	filesToConcat="$filesToConcat $file"

	if $(egrep -q "^$username" $passFile); then
		continue
	fi
	
	newPass=$(pwgen '10' '1')
	echo "$username:$newPass"
		
	hashedPass=$(mkpasswd $newPass)
	echo "$username:$hashedPass" >> $passFile	
	echo ""
done

[[ ! -z $filesToConcat ]] && cat $filesToConcat > $configFile
