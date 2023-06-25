#!/bin/bash

input=""
for el in $@; do
	input="$input $el"
done

w="[^ ]"
input=$(echo "$input" | egrep "\s?-jar\s+.*\b$w+\.jar\b.*")
[[ -z $input ]] && echo "-jar option not found" && exit 1

before=$(echo "$input" | sed -E "s/^(.*)\s*-jar\s+.+$/\1/g")
between=$(echo "$input" | sed -E "s/^.*-jar\s+(.+\.jar)\s*.*$/\1/g")
after=$(echo "$input" | sed -E "s/^.+\.jar\s*(.*)$/\1/g")

echo "Before: $before"
echo "Between: $between"
echo "After: $after"

before=$(echo "$before" | sed -E "s/(-D$w+=$w+)//g")
between=$(echo "$between" | sed -E "s/($w+\.jar)/-jar &/")
after=$(echo "$after") # | sed -E "s/(-D${w}+=${w}+)//g")

echo "java $before $between $after" | sed -E "s/\s+/ /g"
