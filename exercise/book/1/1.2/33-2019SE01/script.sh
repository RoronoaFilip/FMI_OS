#!/bin/bash


text=""

while read line; do
	text=$(echo -e "${text}\n${line}")
done

nums=$(echo "$text" | egrep '^-?[0-9]+$' | sort -n | uniq)

#echo "$nums"

moduled=$(echo "$nums" | sed -E 's/^-//g' | sort -n | uniq)

#echo "$moduled"

max=$(echo "$moduled" | awk -v max="0" '{if(max<$0){max=$0}} END {print max}')

outputA=$(echo "$nums" | egrep "^-?$max$")

echo ""
echo "A)" 
echo "$outputA"
echo ""

digitsSum=$(echo "$moduled" | awk -v FS="" '{sum=0; for(i=1;i<=NF;i++){sum+=$i}; print sum,$0}')

maxSum=$(echo "$digitsSum" | head -n 1 | cut -d ' ' -f 1)

allMaxNums=$(echo "$digitsSum" | egrep "^$maxSum" | cut -d ' ' -f 2)

file=$(mktemp)
echo "$nums" > $file
minNums=$(echo "$allMaxNums" | xargs -I @ egrep '^-?@$' $file | sort -n | head -n 1)
rm $file

echo ""
echo "B)"
echo $minNums
