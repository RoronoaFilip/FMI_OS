#!/bin/bash


if [[ $# -ne 2 ]]; then
	echo "2 params expected"
	exit 1
fi

if [[ ! -f $1 ]]; then
	echo "$1 must be a file"
	exit 2
fi

size=$(stat $1 --printf '%s\n')

if [[ $size -gt 528439 ]]; then
	echo "File is too big"
	exit 5
fi

sizeCheck=$((size%2))
if [[ $sizeCheck -ne 0 ]]; then
	echo "$1 is not a binary file"
	exit 3
fi

if [[ ! -f $2 ]]; then
	echo "File doesn't exist"
	exit 4
fi

echo -e '#include <stdio.h>\n' >> $2

arrN=$((size/2))
echo -e "const uint32_t arrN = $arrN;\n" >> $2
echo -n "const uint16_t arr[] = {" >> $2

file=$(xxd $1)

string=""
while read line; do
	line=$(echo "$line" | sed -E 's/^(.*:)(.*) \s+(.*)$/\2/g')
	
	for element in $line; do
		string="$string, $(echo $element | sed -E 's/^(..)(..)$/0x\2\1/g')"
	done
done < <(echo "$file")
string=$(echo "$string" | sed -E 's/^, (.*)$/\1/g')

echo "$string"
echo "${string}};" >> $2

