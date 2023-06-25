#!/bin/bash

N="10"
args="$@"
if [[ $1 == "-n" ]]; then
	N=$2
	args=$(echo "$@" | cut -d ' ' -f 2-)
fi

a=$(mktemp)
for file in $args; do
	[[ ! -f $file ]] && echo "File $file does not exist" && continue

	lines=$(cat $file | tail -n $N | awk -v file="$file" '{
		end="";
		for(i=3; i<=NF; ++i) {end=end" "$i};
		printf("%s %s %s %s\n", $1, $2, file, end);
	}')
	echo "$lines" >> $a
done

cat $a | sort -n

rm $a
