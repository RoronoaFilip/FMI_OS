#!/bin/bash

find ~ -printf '%s %p\n' | sort -n -r | head -n 5 | cut -d ' ' -f 2 > file.txt

cat file.txt | xargs -I {} rm -i {}

rm file.txt
