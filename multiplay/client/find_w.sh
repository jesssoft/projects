#!/bin/bash
#
# 2014.07.14 jesssoft.
#

if [ -z $1 ]; then
	echo "usage: cmd key-word"
	exit 1;
fi

echo "---------------------------------[ Start ]------------------------------------" 
grep --color=auto -n $1 *.h *.c
echo "----------------------------------[ End ]-------------------------------------" 


