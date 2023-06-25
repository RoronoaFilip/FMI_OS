#!/bin/bash

[[  $(echo "$1" | egrep -q '\-jar') ]] && echo "no -jar" && exit 1

beforeJar=$(echo "$1" | sed -E -e 's/^java(.*)-jar.*$/\1/g' -e 's/(-D[^ ]+=[^ ]+)//g' ) 
echo "Before: $beforeJar"

between=$(echo "$1" | sed -E -e 's/^.*-jar(.*\.jar).*$/\1/g' -e 's/([^ ]+\.jar)/-jar \1/g')
echo "Between: $between"

after=$(echo "$1" | sed -E -e 's/.*\.jar(.*)/\1/g' -e 's/(-D[^ ]+=[^ ]+)//g')
echo "After: $after"

echo "java $beforeJar $between $after" | sed -E 's/\s+/ /g'
