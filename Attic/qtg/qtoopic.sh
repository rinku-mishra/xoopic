#!/bin/sh
LD_LIBRARY_PATH="/usr/openwin/lib:/usr/local/qt/lib/egcs1.1:/usr/local/mesa3.0/lib"
export LD_LIBRARY_PATH
file=$1
if [ $file ]; then
  echo "Using input file $file"
else
  file="../input/dring.inp"
  echo "Using input file $file"
fi
echo "../qtg/qtoopic -i $file"
../qtg/qtoopic -i $file
