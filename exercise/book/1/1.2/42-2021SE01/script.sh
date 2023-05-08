#!/bin/bash


user=$(whoami)

if [[ $user != "oracle" || $user != "grid" ]]; then
	echo "Only grid and oracle can execute this script"
	exit 1
fi

rootPath="u01/app/$user"
pathToDir=$ORACLE_HOME

pathToExe="$rootPath/$pathToDir/bin/adrci"

output=$($pathToExe exec="show homes")

if [[ $output =~ ^No ]]; then
	echo "No ADR homes are set"
	exit 2
fi

relativePaths=$(echo "$output" | egrep '^\s*(\w+\/)+\w*$' | sed -E 's/^\s+|\s+$|^\s+$//g')

while read path; do
	fullPath="$rootPath/$path"

	find $fullPath -maxdepth 0 -printf '%s %p\n'

done <<< $relativePaths 
