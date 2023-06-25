#!/bin/bash

[[ $# -ne 2 ]] && echo "2 params expected" && exit 1
[[ ! -d $2 ]] && echo "The 2nd param must be a dir" && exit 2
[[ -f $1 || ! $1 =~ ^.+\.csv$ ]] && echo "The 1st param must be a .csv file that doesn't exist" && exit 3

echo "hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLAN Trunk Ports,license,SN,key" > $1

files=$(find $2 -type f -name '*.log')

hostname=""
phy=""
vlans=""
hosts=""
failover=""
VPN=""
peers=""
VLAN=""
license=""
SN=""
key=""
while read file; do
	lines=$(cat $file | sed -E -e 's/^\s+//g' -e 's/\s+$//g' -e 's/\s+/ /g')
	hostname=$(basename $file | sed -E 's/^(.+)\.log/\1/g')

	while read line; do
		[[ $line =~ ^Maximum ]] && phy=$(echo "$line" | cut -d ' ' -f 5) && continue
		[[ $line =~ ^VLANs ]] && vlans=$(echo "$line" | cut -d ' ' -f 3) && continue
		[[ $line =~ ^Inside ]] && hosts=$(echo "$line" | cut -d ' ' -f 4) && continue
		[[ $line =~ ^Failover ]] && failover=$(echo "$line" | cut -d ' ' -f 3) && continue
		[[ $line =~ ^VPN-3DES-AES ]] && VPN=$(echo "$line" | cut -d ' ' -f 3) && continue
		[[ $line =~ ^\*Total ]] && peers=$(echo "$line" | cut -d ' ' -f 5) && continue
		[[ $line =~ ^VLAN\ Trunk ]] && VLAN=$(echo "$line" | cut -d ' ' -f 5) && continue
		[[ $line =~ ^This\ platform ]] && license=$(echo "$line" | cut -d ' ' -f 5,6) && continue
		[[ $line =~ ^Serial ]] && SN=$(echo "$line" | cut -d ' ' -f 3) && continue
		[[ $line =~ ^Running ]] && key=$(echo "$line" | cut -d ' ' -f 4) && continue
	done < <(echo "$lines")

	csvLine="${hostname},${phy},${vlans},${hosts},${failover},${VPN},${peers},${VLAN},${license},${SN},${key}"

	echo "$csvLine" >> $1
done < <(echo "$files")
