#!/bin/bash

if [[ $# -ne 2 ]]; then
	echo "2 Params expected"
	exit 1
fi

if [[ ! $1 =~ ^.+\.csv$ ]]; then
	echo "1st Param is not a valid csv file name"
	exit 2
fi

if [[ ! -d $2 ]]; then
	echo "2nd Param must be a directory"
	exit 3
fi

files=$(find $2 -iname '*.log')

echo "hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLAN Trunk Ports,license,SN,key" > $1

for file in $files; do
	lines=$(cat $file)
	
	baseName=$(basename $file | cut -d '.' -f 1)
	
	currLine="$baseName"
	
	currLine=$(echo "$lines" | awk -v line="$currLine" '
	{ if($1 ~ /^(Maximum|*Total|VLAN)$/){line=line","$5} } 
	{ if($1 ~ /^(VLANs|VPN-3DES-AES|Serial|Failover)$/){line=line","$3} } 
	{ if($1 ~ /^(Inside|Running)$/){line=line","$4} } 
	{ if($1 ~ /^This$/){line=line","$5" "$6} } 
	END {print line}
	')

	echo "$currLine" >> $1
	#echo "$currLine"
done
