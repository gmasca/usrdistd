#!/bin/bash

if [ $# -lt 1 ]; then
	echo "inserisci il nome dell'output"
	exit -1
fi

rm -f *.xpi
cd extention
zip -r ${1} *
cd ..
mv extention/${1} .
firefox ${1} &
exit 0