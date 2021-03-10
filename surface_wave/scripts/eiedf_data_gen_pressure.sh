#!/bin/sh

echo
echo "SHELL SCRIPT FOR DATA MANIPULATION"
echo



# WRITE YOUR FILE NAME IN BETWEEN ""
DATADIR="/home/rinku/Documents/Plasma_antenna/New_data/den_1e15"
DATAFILE="antenna_1e15_ux_y.txt"
################## SEARCH AND STORE ########################

grep -n "#####" "$DATADIR/$DATAFILE" |cut -f1 -d: > output.txt

while read line; do
#   echo $line
   LINEVAR=$line
done < output.txt

echo "$LINEVAR"

LINEVAR1=$(( LINEVAR - 1 ))

################## ION DATA DELETE #######################
echo "ELECTRON DATA"
sed -i.bak -e "1,${LINEVAR1}!d" "$DATADIR/$DATAFILE"
mv "$DATADIR/$DATAFILE" "$DATADIR"/eedf_ux_y.txt
cp "$DATADIR/$DATAFILE.bak" "$DATADIR/$DATAFILE"
################## ELECTRON DATA DELETE #######################
echo "ION DATA"
sed -i.bak -e "1,${LINEVAR}d" "$DATADIR/$DATAFILE"
mv "$DATADIR/$DATAFILE" "$DATADIR"/iedf_ux_y.txt
cp "$DATADIR/$DATAFILE.bak" "$DATADIR/$DATAFILE"
rm output.txt

