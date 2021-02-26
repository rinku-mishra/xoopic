#!/bin/sh
# Shell script for submitting $FILE_NAME.inp simulation.

# Invoke this script as follows (or something similar):
# prompt> cd /work/research/bruhwile/projects/data.oopic/test
# prompt> cp /work/research/bruhwile/projects/oopic/t3batch/test.sh .
# prompt> nohup test.sh >test.out 2>test.err </dev/null &

echo
echo "XOOPIC - MPI shell script is beginning execution --"
echo

# Specify the number of processors to use
NUM_PROC=2
echo "  Number of MPI processes = " $NUM_PROC

# Specify the desired number of time steps
NUM_STEP1=400
NUM_STEP2=400
echo "  NUM_STEP1 = " $NUM_STEP1
echo "  NUM_STEP2 = " $NUM_STEP2

# Specify the base name for input/output files
FILE_NAME="beamplasmatest"

# Specify which version of xoopic to execute:
OOPIC_DIR="/work/research/bruhwile/projects/oopic"
INPUT_DIR=$OOPIC_DIR"/inp"
WORK_DIR="/work/research/bruhwile/projects/data.oopic/test"

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
echo "Now running XOOPIC with MPI..."
echo
echo

# Start job
mpirun -np $NUM_PROC xoopic -nox -i $FILE_NAME.inp -s $NUM_STEP1 -dp  $NUM_STEP1


#########################
echo
echo
echo "Now running XOOPIC with MPI..."
echo
echo

# Restart job, with timing turned on
time mpirun -np $NUM_PROC xoopic -nox -i $FILE_NAME.inp -d $FILE_NAME.dmp -s $NUM_STEP2 -dp  $NUM_STEP2

# Rename resulting output to unique name in dump directory
echo
echo
echo "Now renaming the" $FILE_NAME".dmp.* files over to unique names..."

N_DUMPS="n0_"
DUMP_FILES="`ls $FILE_NAME.dmp*`"
for i in $DUMP_FILES; do
  echo "Processing $i";
  mv $i ${N_DUMPS}$i;
done

# Delete the executable now
rm -f xoopic

##########################
echo
echo
echo "All done."
exit
