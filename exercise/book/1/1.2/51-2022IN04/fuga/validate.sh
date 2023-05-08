#!/bin/bash


[[ $1 =~ \.invalid.*$ ]] && echo "Simulated Invalid Lines"  && exit 1
[[ $1 =~ \.error.*$ ]] && exit 2

exit 0
