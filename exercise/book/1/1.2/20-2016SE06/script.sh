#!/bin/bash

a=$(mktemp)
cat "${1}" | egrep '„.*“' -o | sed -E 's/[„“]//g'| sort > $a

b=$(mktemp)
cat ${1} | sed -E 's/.*„/„/g' > $b #| awk 'BEGIN {OFS=". "} {print NR,$0}' > b

while read line; do
	line=$(egrep -n "$line" $b | sed -E 's/^([0-9]+):/\1. /g')
	echo "$line"
done < $a

rm $a $b
