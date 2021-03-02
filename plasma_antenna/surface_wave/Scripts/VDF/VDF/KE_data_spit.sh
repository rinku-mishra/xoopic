#!/bin/sh

echo
echo "SHELL SCRIPT FOR DATA MANIPULATION"
echo



# WRITE YOUR FILE NAME IN BETWEEN ""
#DATADIR="acc1/foc1/dispersion/cold_beam/gui_run/norm_p/vspacex"
DATADIR="data"
FILENAME="$DATADIR/"
FILES="`ls -1 $FILENAME`"

for i in $FILES; do
    #DATAFILE="$i"
    ################## SEARCH AND STORE ########################

    grep -n "#####" "$DATADIR/$i" |cut -f1 -d: > output.txt

    while read line; do
    #   echo $line
       LINEVAR=$line
    done < output.txt

    echo "$LINEVAR"

    LINEVAR1=$(( LINEVAR - 1 ))

    ################## ION DATA DELETE #######################
    echo "ELECTRON DATA"
    sed -i.bak -e "1,${LINEVAR1}!d" "$DATADIR/$i"
    mv "$DATADIR/$i" "$DATADIR"/eedf_"$i"
    mv "$DATADIR/$i.bak" "$DATADIR/$i"
    ################## ELECTRON DATA DELETE #######################
    echo "ION DATA"
    sed -i.bak -e "1,${LINEVAR}d" "$DATADIR/$i"
    mv "$DATADIR/$i" "$DATADIR"/iedf_"$i"
    mv "$DATADIR/$i.bak" "$DATADIR/$i"
    rm output.txt

done

echo "ALL DONE."
