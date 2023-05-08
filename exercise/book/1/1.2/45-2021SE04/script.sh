#!/bin/bash

[[ $# -ne 1 || ! $1 =~ ^[0-9]+$ ]] && echo "1 param that is a number expected" && exit 1

user=$(whoami)

[[ $user != "oracle" && $user != "grid" ]] && echo "You are not grid or oracle" && exit 2

[[ -z $ORACLE_HOME ]] && echo "Environment Variable ORACLE_HOME doesn't exist" && exit 3

adrci="$ORACLE_HOME/bin/adrci"

[[ ! -f $adrci ]] && echo "adrci file not found" && exit 4

diag_test="/u01/app/$user"

initialCommand="SET BASE $diag_test;"

adrHomes=$($adrci exec="$initialCommand SHOW HOMES")

[[ $adrHomes =~ ^No ]] && echo "No ADR homes found" && exit 5

regex="^[^\/]*\/(crs|tnslsnr|kfod|asm|rdbms)\/.*"
regex2="^([^\/]*\/(crs|tnslsnr|kfod|asm|rdbms))\/.*"
adrHomes=$(echo "$adrHomes" | egrep "$regex" | sed -E "s/$regex2/\1/g")

minutes=$(($1*60))
for adrHome in "$adrHomes"; do
	currAdrHome="$diag_test/$adrHome"
	
	command="$initialCommand SET HOMEPATH $currAdrHome; PURGE -AGE $minutes"

	$adrci exec="$command"
done
