#!/bin/bash

me=$(whoami)

processesByUser=$(ps -eo user=)

processesCount=$(echo "$processesByUser" | sort | uniq -c | sed -E 's/^\s+//g')

echo "User $1 has $userPrCount Processes active"

processesTotalRuntimeInSeconds=$(ps -eo times= | awk '{count+=$0} END {print count}')
#echo $processesTotalRuntimeInSeconds
avgProcessesTotalRuntimeInSeconds=$(ps -eo times= | awk '{count+=$0} END {print count/NR}')
#echo $avgProcessesTotalRuntimeInSeconds

avgRuntime=$(echo $avgProcessesTotalRuntimeInSeconds |  awk '{printf("%02d:%02d:%02d",($0/3600),($0/3600%60),($0%60))}')

echo "Average runtime in HH:MM:SS -> $avgRuntime"

FOOProcesses=$(ps -eo user=,pid=,times= | egrep $1)
#echo "$FOOProcesses"

doubleAvgRuntime=$(echo $avgProcessesTotalRuntimeInSeconds | awk '{print $0*2}')
#echo $doubleAvgRuntime

echo "$FOOProcesses" | awk -v time="$doubleAvgRuntime" '{if(time>$3){print $0}}'
#system("kill" $2)
