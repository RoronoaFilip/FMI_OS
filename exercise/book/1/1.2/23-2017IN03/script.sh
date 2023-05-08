#!/bin/bash

find /home/students/s0600041 -type f -printf '%u#%p#%TF_%TT\n' | sort -r -t '#' -k 3 | head -n 1 | cut -d '#' -f 1,2 | sed 's/#/ /g'
