#!/bin/bash

filePath="/proc/acpi/wakeup"
testFilePath="testFile"

[[ $# -ne 1 ]] && echo "1 param expected" && exit 1

[[ ! $1 =~ ^[A-Z0-9]{1,4} ]] && echo "The Param is not a valid device name" && exit 2

#if line=$(egrep "^$1" $filePath); then
if line=$(egrep "^$1" $testFilePath); then
	[[ $line =~ ^$1.*\*enabled.*$ ]] && echo $1 || echo "Already Disabled"
	#[[ $line =~ ^$1\s+[a-zA-Z0-9]+\s+\*enabled.* ]] && echo $1 > $filePath
else
	echo "Device not found"
	exit 3
fi
