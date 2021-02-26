#!/bin/sh
# Shell script for submitting $FILE_NAME.inp simulation in serial mode.

# Invoke this script as follows (or something similar):
# prompt> cd /work/research/bruhwile/projects/data.oopic/test_serial
# prompt> cp /work/research/bruhwile/projects/oopic.serial/t3batch/test_serial.sh .
# prompt> nohup test_serial.sh >test_serial.out 2>test_serial.err </dev/null &

echo
echo "XOOPIC - MPI shell script is beginning execution --"
echo

# Specify the desired number of time steps
NUM_STEP1=400
NUM_STEP2=400
echo "  NUM_STEP1 = " $NUM_STEP1
echo "  NUM_STEP2 = " $NUM_STEP2

# Specify the base name for input/output files
FILE_NAME="beamplasmatest"

# Specify which version of xoopic to execute:
OOPIC_DIR="/work/research/bruhwile/projects/oopic.serial"
INPUT_DIR=$OOPIC_DIR"/inp"
WORK_DIR="/work/research/bruhwile/projects/data.oopic/test_serial"

# Make sure shell is running commands in the right place
cd $WORK_DIR

# Copy over executable and input file
echo
echo "Copying over the specified input file..."
echo "Input file is:    " $INPUT_DIR"/"$FILE_NAME
cp $INPUT_DIR/$FILE_NAME.inp .
echo
echo "Copying over the specified XOOPIC executable..."
echo "Executable is:    " $OOPIC_DIR"/xg/xoopic"
cp $OOPIC_DIR/xg/xoopic .

#########################
echo
echo
echo "Now running XOOPIC in serial mode..."
echo
echo

# Start job
xoopic -nox -i $FILE_NAME.inp -s $NUM_STEP1 -dp  $NUM_STEP1


#########################
echo
echo
echo "Now running XOOPIC in serial mode..."
echo
echo

# Restart job, with timing turned on
time xoopic -nox -i $FILE_NAME.inp -d $FILE_NAME.dmp -s $NUM_STEP2 -dp  $NUM_STEP2

# Rename resulting output to unique name in dump directory
echo
echo
echo "Now renaming the" $FILE_NAME".dmp.* files over to unique names..."

N_DUMPS="n0_"
mv $FILE_NAME.dmp ${N_DUMPS}$FILE_NAME.dmp;

# Delete the executable now
rm -f xoopic

##########################
echo
echo
echo "All done."
exit
