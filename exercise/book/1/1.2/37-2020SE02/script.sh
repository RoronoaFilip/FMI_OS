#!/bin/bash

regex="([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+) ([^\s]+) ([^\s]+) (\[.+\]) (GET|POST) (\/.+\/.+) ([0-9]+) ([0-9]+) (\".+\") (\".+\")"

if [[ ! -f $1 ]]; then
	echo "1st Param must be a file"
	exit 1
fi

top3Sites=$(cat $1 | sed -E "s/$regex/\2/g" | sort | uniq -c | sort -nr | sed -E 's/^\s+//g' | head -n 3 | cut -d ' ' -f 2)

#echo "$top3Sites"

while read site; do
	requests=$(cat $1 | sed -E "s/$regex/\2 \6/g" | egrep ^$site)

	http2=$(echo "$requests" | egrep "\/2\.0$" | wc -l)
	notHttp2=$(echo "$requests" | egrep -v "\/2\.0$" | wc -l)

	echo "For Site: $site HTTP2: $http2; Not HTTP2: $notHttp2"
done <<< "$top3Sites"

clientRequestsWithCodeGreaterThan302=$(cat $1 | sed -E "s/$regex/\1 \7/g" | awk '$2 > 302 {print $0}' | cut -d ' ' -f 1)
#echo "$clientRequestsWithCodeGreaterThan302"

ans=$(echo "$clientRequestsWithCodeGreaterThan302" | sort | uniq -c | sort -nr | head -n 5)
echo "$ans"
