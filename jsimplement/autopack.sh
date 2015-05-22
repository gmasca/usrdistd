#!/bin/bash

name=pack.xpi

rm -f *.xpi
rm -f extention/*.zip
cd extention
zip -r pack.xpi *
cd ..
mv extention/pack.xpi .
firefox pack.xpi &
exit 0
