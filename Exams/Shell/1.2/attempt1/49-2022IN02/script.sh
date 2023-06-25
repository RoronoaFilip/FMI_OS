#!/bin/bash

user=$(whoami)
[[ $user != "oracle" &&  $user != "grid" ]] && echo "you are not oracle or grid" && exit 1

[[ $# -ne 1 ]] && "1 param expected" && exit 2

[[ ! $1 =~ ^[0-9]+$ ]] && echo "the param must be a number" && exit 3

[[ -z $ORACLE_BASE || -z $ORACLE_HOME || -z $ORACLE_SID ]] && echo "some of the 3 environmental variables are not present" && exit 4

sqlplus="$ORACLE_HOME/bin/sqlplus"

[[ ! -f $sqlplus ]] && echo "sqlplus not found" && exit 5

role=""
[[ $user == "oracle" ]] && role="SYSDBA" || role="SYSASM"

if ! diagnostic_dest=$($sqlplus -SL "/ as $role" foo.sql | tail -n +4 | head -n 1); then
	echo "sqlplus could not be executed"
	exit 6
fi

[[ -z $diagnostic_dest ]] && diagnostic_dest="$ORACLE_BASE"

diag_dir="$diagnostic_dest/diag"

machine=$(hostname -s)
if [[ $user == "oracle" ]]; then
	interesting_dir="$diag_dir/rdbms"
	kilobytes=$(find $interesting_dir -mindepth 2 -maxdepth 2 -type f -mtime +$1 -printf '%s\n' | egrep "^.*_[0-9]+\.(trm|trc)$"  | awk '{count+=$0} END {print count/1024}')
	echo "rdbms: $kilobytes"
else
	crsPath="$diag_dir/crs/$machine/crs/trace"
 	kilobytes=$(find $crsPath -type f -mtime +$1 -printf '%s\n' | egrep "^.*_[0-9]+\.(trm|trc)$" | awk '{count+=$0} END {print count/1024}')
	echo "crs: $kylobytes"

	interesting_dir="$diag_dir/tnslsnr/$machine"
	kilobytes1=$(find $interesting_dir/*/alert -type f -mtime +$1 -printf '%s\n' | egrep "^.*_[0-9]+\.xml$" | awk '{count+=$0} END {print count/1024}')
	kilobytes2=$(find $interesting_dir/*/trace -type f -mtime +$1 -printf '%s\n' | egrep "^.*_[0-9]+\.log$" | awk '{count+=$0} END {print count/1024}')
	echo "tnslsnr: $((kilobytes1+kilobytes2))"
fi
