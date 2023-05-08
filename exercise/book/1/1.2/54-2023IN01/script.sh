#!/bin/bash

command1="ssacli ctrl slot="
command2="pd all show detail"

if [[ $# -gt 1 ]]; then 
	echo "0 or 1 params expected"
	exit 1
fi

if [[ $1 == "autoconf" ]]; then
	echo "yes"
	exit 0
fi

if [[ $1 != "config" && ! -z $1 ]]; then
	echo "Invalid 1st param"
	exit 2
fi

CTRLSLOTS="0 1"
if [[ -z $CTRLSLOTS ]]; then
	CTRLSLOTS="0"
fi

if [[ $1 == "config" ]]; then
	echo -e "graph_title SSA drive temperatures\ngraph_vlabel Celsius\ngraph_category sensors\ngraph_info This graph shows SSA drive temp"
fi

for slot in $CTRLSLOTS; do
	#output=$(${command1}${slot} ${command2})
	output=$(cat file$slot.txt)
	echo "$output" | awk -v config="$1" -v array="" -v slot="$slot" -v model="" -v disk="" -v temp="" '
	{
		if($1=="Smart" && $2="Array") { model = $3 }
		if($1=="Array") { array = $2 }
		if($1=="Unassigned") { array = "UN" }
		if($1=="physicaldrive") { disk = $2; Disk = $2; gsub(":","",Disk) }
		if($1 == "Current" && $2 == "Temperature") {
			temp = $4;
			id = "SSA"slot""model""array""Disk;
			if(config != "") { 
				printf("%s.label SSA%s %s %s %s\n%s.type GAUGE\n", id, slot, model, array, disk, id);
			} else {
				printf "%s.value %s\n", id, temp
			}
		}
	}'
done
