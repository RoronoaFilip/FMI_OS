#!/bin/bash

user=$(whoami)
#[[ $user != "root" ]] && echo "You are not root" && exit 1

[[ $# -gt 1 ]] && echo "0 or 1 params expected" && exit 1
[[ ! -z $1 && $1 != "autoconf" && $1 != "config" ]] && echo "Invalid first param" && exit 2

command1="ssacli ctrl slot="
command2="pd all show detail"

[[ -z $CTRLSLOTS ]] && CTRLSLOTS="0 1"

[[ $1 == "autoconf" ]] && echo "yes" && exit 0

if [[ $1 == "config" ]]; then
	echo -e "graph_title SSA drive temperatures\ngraph_vlabel Celsius\ngraph_category sensors\ngraph_info This graph shows SSA drive temp"
fi

for slot in $CTRLSLOTS; do
	#output=$(${command1}${slot} ${command2})
	output=$(cat file${slot}.txt)
	echo "$output" | awk -v config="$1" -v slot="$slot" '
	{
		if($1 == "Smart") { model = $3 }
		if($1 == "Array") { array = $2 }
		if($1 == "Unassigned") { array = "UN" }
		if($1 == "physicaldrive") { disk = $2; DISK = $2; gsub(":", "", DISK) }
		if($1 == "Current" && $2 == "Temperature") { 
			temp = $4;
			id = "SSA"slot""model""array""DISK;
			if(config == "") {
				printf("%s.value %s\n", id, temp);
			} else if(config == "config"){
				printf("%s.label %s %s %s %s\n%s.type GAUGE\n", id, "SSA"slot, model, array, disk, id);
			}
		}
	}'
done
