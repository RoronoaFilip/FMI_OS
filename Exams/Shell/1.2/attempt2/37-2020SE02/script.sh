#!/bin/bash

[[ $# -ne 1 || ! -f $1 ]] && echo "1 param expected that is a file" && exit 1

regex="^([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3})"
regex="$regex ([^\s]+)"
regex="$regex ([^\s]+)"
regex="$regex (\[.+\])"
regex="$regex (GET|POST)"
regex="$regex (\/.*HTTP\/[12]\.[01])"
regex="$regex ([0-9]{3})"
regex="$regex ([0-9]+)"
regex="$regex (\".+\")"
regex="$regex (\".+\")$"

lines=$(egrep "$regex" $1)

top3Sites=$(echo "$lines" | sed -E "s/$regex/\2/g" | sort | uniq -c | sort -nr | head -n 3 | sed -E 's/^\s+//g' | cut -d ' ' -f 2)

while read site; do
	requests=$(echo "$lines" | sed -E "s/$regex/\2 \1 \7 \6/g" | egrep "^$site")

	http2=$(echo "$requests" | egrep -c "2\.0$")
	notHttp2=$(echo "$requests" | egrep -vc "2\.0$")
	echo "Site: $site; HTTP2: $http2; Not HTTP2: $notHttp2"

	requests=$(echo "$requests" | cut -d ' ' -f 2,3)
	requests=$(echo "$requests" | awk '{if($2 > 302) {print $0};}')

	users=$(echo "$requests" | cut -d ' ' -f 1 | sort | uniq -c | sort -nr | head -n 5)

	echo "$users"
	echo ""
done < <(echo "$top3Sites")
