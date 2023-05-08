#!/bin/bash

baseFile="base.csv"
prefixFile="prefix.csv"

[[ $# -ne 3 ]] && echo "3 params expected" && exit 1

[[ ! $1 =~ ^-?[0-9]+$ && ! $1 =~ ^-?[0-9]+\.[0-9]+$ ]] && echo "1st param must be a number" && exit 2

[[ ! $2 =~ ^[a-zA-Z]+$ || ! $3 =~ ^[a-zA-Z]+$ ]] && echo "2nd and 3rd params must be non empty strings" && exit 3

units=$(cat $baseFile | tail -n +2)
prefixes=$(cat $prefixFile | tail -n +2)

prefixLine=$(echo "$prefixes" | egrep "^.*,$2,.*$")
unitLine=$(echo "$units" | egrep "^.*,$3,.*$")

[[ -z $prefixLine ]] && echo "Prefix Not Found" && exit 4
[[ -z $unitLine ]] && echo "Unit Not Found" && exit 4

unitName=$(echo "$unitLine" | cut -d ',' -f 1)
measure=$(echo "$unitLine" | cut -d ',' -f 3)
unitOutput=" $3 ($measure, $unitName)"

multiplier=$(echo "$prefixLine" | cut -d ',' -f 3)

echo "awk:"
echo "$multiplier" | gawk -v back="$unitOutput" -v num="$1" 'END {number=num*$0; print number""back}'
echo ""

echo "bc:"
finalNum=$(echo "$1*$multiplier" | bc)
echo "${finalNum}${unitOutput}"
