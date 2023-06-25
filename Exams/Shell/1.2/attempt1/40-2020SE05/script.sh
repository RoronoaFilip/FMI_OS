#!/bin/bash


if [[ $# -ne 3 ]]; then
	echo "3 params expected"
	exit 1
fi

if [[ ! -f $1 ]]; then
	echo "1st param must be a file that exists"
	exit 2
fi
if [[ -f $2 ]]; then
    echo "2nd param must be a file that doesn't exists"
    exit 3
fi
if [[ ! -d $3 ]]; then
	echo "3rd param must be a dir"
	exit 4
fi

files=$(find $3 -iname '*.cfg')

[[ -z $files ]] && exit 5

wrongFiles=""
rightFiles=""
basenamesRightFiles=""

for file in $files; do
	linesOfFile=$(egrep -v '^#.*$|^\{ .* \};$|^\s*$' $file)
	
	if [[ ! -z "$linesOfFile" ]]; then
		wrongFiles+="$file "
	else
		rightFiles+="$file "
		basenamesRightFiles+="$(basename $file) "
	fi
done

for file in $wrongFiles; do
	echo "Error in $file"

	cat $file | gawk '!/^#.*$|^\{ .* \};$|^\s*$/ {printf("Line %d:%s\n", NR, $0)}'
	echo ""
done

cat $rightFiles > $2

usernames=$(echo "$basenamesRightFiles" | sed -E 's/\.cfg//g')

for username in $usernames; do
	if ! egrep -q $username $1; then
		pass=$(pwgen 16 1)
		echo "$username:$pass" >> $1
		echo "$username $pass"
	fi
done
