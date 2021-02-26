#!/bin/sh
# Shell script for submitting beamplasma.inp simulation on Unix/Linux

# Invoke this script as follows:
# prompt> nohup beamplasma.sh >beamplasma.out 2>beamplasma.err &

echo
echo "XOOPIC - MPI shell script is beginning execution --"
echo

# Specify the number of processors to use
NUM_PROC=2
echo "  Number of MPI processes = " $NUM_PROC

# Specify the number of steps
# NUM_STEP1 gets the beam fully into the simulation region
# NUM_STEP2 gets the beam 0.2 m into the plasma (using moving window)
# NUM_STEP3 gets the beam an additional 0.2 m into the plasma
# note:  NUM_STEP2 = NUM_STEP3 - NUM_STEP1
NUM_STEP1=468
NUM_STEP2=16876
NUM_STEP3=17344

# Specify the base name for input/output files
FILE_NAME="beamplasma"

# Specify which version of xoopic to execute:
OOPIC_DIR="/work/research/bruhwile/projects/oopic/oopic.mpi"
INPUT_DIR=$OOPIC_DIR"/inp"
STORE_DIR="/work/research/bruhwile/projects/data.oopic/e157"

# copy input file and executable to local directory
# set echo, turn on accounting, time job, etc.
echo
echo "Copying over the specified input file and executable..."
cd $STORE_DIR
cp $OOPIC_DIR/xg/xoopic .
cp $INPUT_DIR/$FILE_NAME.inp .


#########################
echo
echo
echo "Now running XOOPIC with MPI..."
echo
echo

# Run enough to get beam fully inside simulation region
time mpirun -np $NUM_PROC xoopic -nox -i $FILE_NAME.inp -s $NUM_STEP1 -dp $NUM_STEP1

# Copy resulting output to unique name in dump directory
echo
echo
echo "Now copying the *.dmp.* files to the dump directory with new names..."

N_DUMPS="n0_"
DUMP_FILES="`ls $FILE_NAME.dmp*`"
for i in $DUMP_FILES; do
  echo "Processing $i";
  cp -f $i ${N_DUMPS}$i;
done

#########################
echo
echo
echo "Now running XOOPIC with MPI..."
echo
echo

# Restart job and run beam from 0.0 m to 0.2 m
time mpirun -np $NUM_PROC xoopic -nox -i $FILE_NAME.inp -d $FILE_NAME.dmp -s $NUM_STEP2 -dp $NUM_STEP2

# Copy resulting output to unique name in dump directory
echo
echo
echo "Now copying the *.dmp.* files to the dump directory with new names..."

N_DUMPS="n1_"
DUMP_FILES="`ls $FILE_NAME.dmp*`"
echo $DUMP_FILES
for i in $DUMP_FILES; do
  echo "Processing $i";
  cp -f $i ${N_DUMPS}$i;
done

#########################
echo
echo
echo "Now running XOOPIC with MPI..."
echo
echo

# Restart job and run beam from 0.2 m to 0.4 m
time mpirun -np $NUM_PROC xoopic -nox -i $FILE_NAME.inp -d $FILE_NAME.dmp -s $NUM_STEP3 -dp $NUM_STEP3

# Copy resulting output to unique name in dump directory
echo
echo
echo "Now copying the *.dmp.* files to the dump directory with new names..."

N_DUMPS="n2_"
DUMP_FILES="`ls $FILE_NAME.dmp*`"
echo $DUMP_FILES
for i in $DUMP_FILES; do
  echo "Processing $i";
  cp -f $i ${N_DUMPS}$i;
done

#########################
echo
echo
echo "Now running XOOPIC with MPI..."
echo
echo

# Restart job and run beam from 0.4 m to 0.6 m
time mpirun -np $NUM_PROC xoopic -nox -i $FILE_NAME.inp -d $FILE_NAME.dmp -s $NUM_STEP3 -dp $NUM_STEP3

# Copy resulting output to unique name in dump directory
echo
echo
echo "Now copying the *.dmp.* files to the dump directory with new names..."

N_DUMPS="n3_"
DUMP_FILES="`ls $FILE_NAME.dmp*`"
echo $DUMP_FILES
for i in $DUMP_FILES; do
  echo "Processing $i";
  cp -f $i ${N_DUMPS}$i;
done

#########################
echo
echo
echo "Now running XOOPIC with MPI..."
echo
echo

# Restart job and run beam from 0.6 m to 0.8 m
time mpirun -np $NUM_PROC xoopic -nox -i $FILE_NAME.inp -d $FILE_NAME.dmp -s $NUM_STEP3 -dp $NUM_STEP3

# Copy resulting output to unique name in dump directory
echo
echo
echo "Now copying the *.dmp.* files to the dump directory with new names..."

N_DUMPS="n4_"
DUMP_FILES="`ls $FILE_NAME.dmp*`"
echo $DUMP_FILES
for i in $DUMP_FILES; do
  echo "Processing $i";
  cp -f $i ${N_DUMPS}$i;
done

#########################
echo
echo
echo "Now running XOOPIC with MPI..."
echo
echo

# Restart job and run beam from 0.8 m to 1.0 m
time mpirun -np $NUM_PROC xoopic -nox -i $FILE_NAME.inp -d $FILE_NAME.dmp -s $NUM_STEP3 -dp $NUM_STEP3

# Copy resulting output to unique name in dump directory
echo
echo
echo "Now copying the *.dmp.* files to the dump directory with new names..."

N_DUMPS="n5_"
DUMP_FILES="`ls $FILE_NAME.dmp*`"
echo $DUMP_FILES
for i in $DUMP_FILES; do
  echo "Processing $i";
  cp -f $i ${N_DUMPS}$i;
done

##########################
echo
echo
echo "All done."
exit
