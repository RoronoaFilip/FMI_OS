#!/bin/bash

if [[ $# -ne 3 ]]; then
	echo "3 Params expected"
	exit 1
elif [[ ! -f $1 ]]; then
	echo "1st Param must be a File"
	exit 2
elif [[ -z $2 || -z $3 ]]; then
	echo "2nd and 3rd Param must not be empty"
	exit 3
fi

user=$(whoami)
date=$(date)

regex="^\s*$|^.+=.+$|^#.*$"

lines=$(egrep $regex $1)

lines=$(echo "$lines" | gawk -v key="$2" -v val="$3" -v user="$user" -v date="$date" ' 
BEGIN { c = 0; }
{
	if($1 == key && $3 != val) {
		printf("# %s edited at %s by %s\n", $0, date, user);
		printf("%s = %s added at %s by %s\n", key, val, date, user);
		c=1;
	} else if ($1 == key && $3 == val) { c = 2; print $0} 
	else { print $0 }
}
END { if(c == 0) {
		printf("%s = %s added at %s by %s", key, val, date, user)
	}
}
')

echo "$lines"
echo "$lines" > $1
