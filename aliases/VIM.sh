#!/bin/bash

if [[ -f "$1" ]]; then
	vim "$1";
	exit 1
fi
if [[ -f "$2" ]]; then
	vim "$2"
	exit 1
fi

file=""
if [[ $1 =~ \.sh$ ]]; then 
	file="$1"
	echo -e "#!/bin/bash\n" >> "$file"
elif [[ $1 =~ \.c$ ]]; then
	file="$1"

	#echo -e "#include <unistd.h>
	##include <stdlib.h>" >> "$file"
	echo "" >> "$file"
	echo "int main() {" >> "$file"
	echo "" >> "$file"
	echo "" >> "$file"
	echo "" >> "$file"
	echo "	close();" >> "$file"
	echo "	return 0;" >> "$file"
	echo "}" >> "$file"
fi

if [[ ! -z $file && ! $file =~ \.c$ ]]; then
	touch "$file"
	chmod u+x "$file"
else
	file="$1"
fi

vim "$file"
