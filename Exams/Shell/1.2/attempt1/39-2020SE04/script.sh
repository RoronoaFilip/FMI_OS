#!/bin/bash

if [[ $# -ne 2 ]]; then
	echo "2 Params expected"
	exit 1
fi

if [[ ! -d $1 ]]; then
	echo "1st Param must be a dir"
	exit 2
fi

if [[ -d $2 ]]; then
	echo "2nd dir must not exist"
	exit 3
fi

files=$(find $1 -iname '*.jpg')

basenames=$(echo "$files" | xargs -I @ basename @)

titles=$(echo "$basenames" | sed -E -e 's/\.jpg//g' -e 's/\([^\(\)]*\)//g' -e 's/\s+/ /g' -e 's/\s+$//g')

albums=$(echo "$basenames" | sed -E -e '/^.*\((.*)\)[^\(\)]*$/{ s|^.*\((.*)\)[^\(\)]*$|\1|g; b skip}; s/.*/misc/g; : skip' -e 's/\s+/ /g')

mtimes=$(echo "$files" | xargs -I @ stat @ --printf '%y\n' | cut -d ' ' -f 1)

titleAlbumMtime=$(paste -d '|' <(echo "$files") <(paste -d '|' <(echo "$titles") <(paste -d '|' <(echo "$albums") <(echo "$mtimes"))))

#while IFS='|' read -r file title album mtime; do
while read line; do
	file=$(echo "$line" | cut -d '|' -f 1)
	title=$(echo "$line" | cut -d '|' -f 2)
	album=$(echo "$line" | cut -d '|' -f 3)
	date=$(echo "$line" | cut -d '|' -f 4)

	sha=$(sha256sum "$file" | cut -d ' ' -f 1)

	mkdir -p "$2"/images
	mkdir -p "$2"/by-date/"$date"/by-album/"$album"/by-title
	mkdir -p "$2"/by-date/"$date"/by-title
	mkdir -p "$2"/by-album/"$album"/by-date/"$date"/by-title
	mkdir -p "$2"/by-album/"$album"/by-title
	mkdir -p "$2"/by-title

	cp "$file" "$2/images/"
	basename=$(basename "$file")
	mv $2/images/"$basename" $2/images/"$sha.jpg"
	pwd=$(pwd)
	
	#cd $2/by-date/"$date"/by-album/"$album"/by-title
	#ln -s "../../../../../images/$sha.jpg" "$title".jpg
	#cd $pwd 

	ln -s "../../../../../images/$sha.jpg" $2/by-date/"$date"/by-album/"$album"/by-title/"$title".jpg

	#cd $2/by-date/"$date"/by-title
	#ln -s "../../../images/$sha.jpg" "$title".jpg
	#cd $pwd

	ln -s "../../../images/$sha.jpg" $2/by-date/"$date"/by-title/"$title".jpg

	#cd $2/by-album/"$album"/by-date/"$date"/by-title
	#ln -s "../../../../../images/$sha.jpg" "$title".jpg
	#cd $pwd
	
	ln -s "../../../../../images/$sha.jpg" $2/by-album/"$album"/by-date/"$date"/by-title/"$title".jpg

	#cd $2/by-album/"$album"/by-title/
	#ln -s "../../../images/$sha.jpg" "$title".jpg
	#cd $pwd

	ln -s "../../../images/$sha.jpg" $2/by-album/"$album"/by-title/"$title".jpg

	#cd $2/by-title/
	#ln -s "../images/$sha.jpg" "$title".jpg
	#cd $pwd
	ln -s "../images/$sha.jpg" $2/by-title/"$title".jpg
done < <(echo "$titleAlbumMtime")
