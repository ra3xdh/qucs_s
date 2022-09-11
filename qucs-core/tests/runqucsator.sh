#!/bin/sh
# run qucsator in directory dst on file

qucsator="$1"
dst="$2"
simuldir=`
	(dirname "$dst") 2>/dev/null ||
	expr X"$dst" : 'X\(.*[^/]\)//*[^/][^/]*/*$' \| \
	     X"$dst" : 'X\(//\)[^/]' \| \
	     X"$dst" : 'X\(//\)$' \| \
	     X"$dst" : 'X\(/\)' \| . 2>/dev/null ||
	echo X"$dst" |
	    sed '/^X\(.*[^/]\)\/\/*[^/][^/]*\/*$/{
		   s//\1/
		   q
		 }
		 /^X\(\/\/\)[^/].*/{
		   s//\1/
		   q
		 }
		 /^X\(\/\/\)$/{
		   s//\1/
		   q
		 }
		 /^X\(\/\).*/{
		   s//\1/
		   q
		 }
		 s/.*/./; q'`
simulfile=`basename $dst`
cd "$simuldir"
echo $qucsator
"$qucsator" -i "$simulfile"
