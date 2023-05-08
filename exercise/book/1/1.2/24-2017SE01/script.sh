#!/bin/bash

if [[ "$2" =~ ^[0-9]+$ ]]; then
	find "$1" -type f -links +$2 -o -links $2 2> /dev/null
else
	find "$1" -type l -printf '%p -> %l\n' 2> /dev/null | egrep '^.* -> $' | sed -E 's/ -> //g'
fi
