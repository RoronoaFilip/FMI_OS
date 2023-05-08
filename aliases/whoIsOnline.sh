#!/bin/bash

a=mktemp
b=mktemp

grep "$(who | cut -d ' ' -f 1)" /etc/passwd | cut -d ':' -f 1,5 | cut -d ',' -f 1 | sort -nt ':' -k 1 | cat -n > a

who | sort -t ' ' -k 1 | uniq -w 8 | tr -s ' ' | cut -d ' ' -f 1,2 | cat -n > b

join a b

rm a b
