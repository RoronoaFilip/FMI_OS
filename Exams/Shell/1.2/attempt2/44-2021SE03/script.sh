#!/bin/bash

[[ $# -ne 2 ]] && echo "2 params expected" && exit 1

[[ ! -f $1 ]] && echo "1st param must be a file" && exit 2
[[ -f $2 || ! $2 =~ ^.+\.h$ ]] && echo "2nd param must be a C header file that does not exist" && exit 3

size=$(find $1 -printf '%s\n')
[[ $(($size%2)) -ne 0 ]] && echo "1st param must be a binary file" && exit 4
[[ $size -gt 524288 ]] && echo "The array is too large" && exit 5

arrN="$size"

echo "#include <stdio.h>" >> $2
echo "" >> $2
echo "const uint32_t arrN=$size;" >> $2
echo "" >> $2

xxd=$(xxd $1 | cut -c 11-49)
echo "$xxd"
arr=""
while read line; do
	for element in $line; do
		element=$(echo "$element" | sed -E 's/^(..)(..)$/\2\1/g')
		arr="$arr, 0x$element"
	done
done < <(echo "$xxd")

arr=$(echo "$arr" | sed -E 's/^,//g')

echo "const uint16_t arr[] = {$arr };" >> $2
