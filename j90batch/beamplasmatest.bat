# Batch script for submitting $FILE_NAME.inp simulation to the T3E
# To use this test script, copy it over to $WORK_DIR (for example,
#   /usr/tmp/bruhwile/test), then cd to that directory and execute
#   the following command:
#
# prompt>  cqsub -la premium test.bat

# Specify resource requirements
#
#QSUB -lM 100Mw           # Specify 100 Mwords of memory
#QSUB -lT 08:00:00        # Specify 8 hours of CPU time
#QSUB -lt 08:00:00        # Per Process Max CPU Time Limit
#QSUB -lF 10Gb            # Specify 10 Gigabytes of Disk Space 
#QSUB -J y                # Keep job log

# Specify administrative details
#
#QSUB -s /bin/sh          # Specify bash shell
#QSUB -r test             # Job name
#QSUB -o test.out         # Write NQS output to specified file
#QSUB -e test.err         # Write NQS error  to specified file
#QSUB -ro -re             # Write output/error messages as they occur

# turn on job accounting
ja

set echo
echo
echo "XOOPIC - Cray J90 batch script is beginning execution --"
echo


# Specify the number of processors to use.
export NCPUS=2
NUM_PROC=$NCPUS

# Specify the number of time steps to use
NUM_STEP1=361
NUM_STEP2=14222

# Specify the base name for input/output files
FILE_NAME="beamplasmatest"

# Specify which version of xoopic to execute:
OOPIC_DIR="/u/abk/bruhwile/projects/oopic"
INPUT_DIR=$OOPIC_DIR"/inp"
WORK_DIR="/u/abk/bruhwile/data/test2"

echo
echo "NCPUS     = " $NCPUS
echo "NUM_PROC  = " $NUM_PROC
echo "NUM_STEP1 = " $NUM_STEP1
echo "NUM_STEP2 = " $NUM_STEP2
echo "FILE_NAME = " $FILE_NAME
echo "OOPIC_DIR = " $OOPIC_DIR
echo "INPUT_DIR = " $INPUT_DIR
echo "WORK_DIR  = " $WORK_DIR
echo

# copy input file and executable to local director,
# set echo, turn on accounting, time job, etc.

cd $TMPDIR
echo
echo "cd " $TMPDIR
echo "pwd yields:  `pwd`"

cp $OOPIC_DIR/xg/xoopic .
cp $INPUT_DIR/$FILE_NAME.inp .

echo "ls -l yields --"
echo "`ls -l`"
echo

cp $INPUT_DIR/$FILE_NAME.inp $WORK_DIR


#######################################################################
#######################################################################
# Run for NUM_STEP1 time steps...
echo
echo
echo "Now running XOOPIC --"
echo
echo
mpirun -np $NUM_PROC ./xoopic -i $FILE_NAME.inp -s $NUM_STEP1 -dp $NUM_STEP1

# Copy resulting output to a unique name
N_DUMPS="n0_"
echo
echo
echo "ls -l yields --"
echo "`ls -l`"
echo
DUMP_FILES="`ls $FILE_NAME.dmp.*`"
for i in $DUMP_FILES; do
  echo "Processing file " $i;
  cp $i ${N_DUMPS}$i;
done
echo
echo "Now, ls -l yields --"
echo "`ls -l`"
echo

#######################################################################
#######################################################################
# Read in the dump files and then run for NUM_STEP2 time steps...
echo
echo
echo "Now running XOOPIC --"
echo
echo
mpirun -np $NUM_PROC ./xoopic -i $FILE_NAME.inp -d $FILE_NAME -s $NUM_STEP2 -dp $NUM_STEP2

# Copy resulting output to a unique name
N_DUMPS="n1_"
echo
echo
echo "ls -l yields --"
echo "`ls -l`"
echo
DUMP_FILES="`ls $FILE_NAME.dmp.*`"
for i in $DUMP_FILES; do
  echo "Processing file " $i;
  cp $i ${N_DUMPS}$i;
done
echo
echo "Now, ls -l yields --"
echo "`ls -l`"
echo
echo


#######################################################################
#######################################################################
# Read in the dump files and then run for NUM_STEP2 time steps...
echo
echo
echo "Now running XOOPIC --"
echo
echo
mpirun -np $NUM_PROC ./xoopic -i $FILE_NAME.inp -d $FILE_NAME -s $NUM_STEP2 -dp $NUM_STEP2

# Copy resulting output to a unique name
N_DUMPS="n2_"
echo
echo
echo "ls -l yields --"
echo "`ls -l`"
echo
DUMP_FILES="`ls $FILE_NAME.dmp.*`"
for i in $DUMP_FILES; do
  echo "Processing file " $i;
  cp $i ${N_DUMPS}$i;
done
echo
echo "Now, ls -l yields --"
echo "`ls -l`"
echo
echo


#######################################################################
#######################################################################
# Read in the dump files and then run for NUM_STEP2 time steps...
echo
echo
echo "Now running XOOPIC --"
echo
echo
mpirun -np $NUM_PROC ./xoopic -i $FILE_NAME.inp -d $FILE_NAME -s $NUM_STEP2 -dp $NUM_STEP2

# Copy resulting output to a unique name
N_DUMPS="n3_"
echo
echo
echo "ls -l yields --"
echo "`ls -l`"
echo
DUMP_FILES="`ls $FILE_NAME.dmp.*`"
for i in $DUMP_FILES; do
  echo "Processing file " $i;
  cp $i ${N_DUMPS}$i;
done
echo
echo "Now, ls -l yields --"
echo "`ls -l`"
echo
echo


#######################################################################
#######################################################################
# Read in the dump files and then run for NUM_STEP2 time steps...
echo
echo
echo "Now running XOOPIC --"
echo
echo
mpirun -np $NUM_PROC ./xoopic -i $FILE_NAME.inp -d $FILE_NAME -s $NUM_STEP2 -dp $NUM_STEP2

# Copy resulting output to a unique name
N_DUMPS="n4_"
echo
echo
echo "ls -l yields --"
echo "`ls -l`"
echo
DUMP_FILES="`ls $FILE_NAME.dmp.*`"
for i in $DUMP_FILES; do
  echo "Processing file " $i;
  cp $i ${N_DUMPS}$i;
done
echo
echo "Now, ls -l yields --"
echo "`ls -l`"
echo
echo


#######################################################################
#######################################################################
# Read in the dump files and then run for NUM_STEP2 time steps...
echo
echo
echo "Now running XOOPIC --"
echo
echo
mpirun -np $NUM_PROC ./xoopic -i $FILE_NAME.inp -d $FILE_NAME -s $NUM_STEP2 -dp $NUM_STEP2

# Copy resulting output to a unique name
N_DUMPS="n5_"
echo
echo
echo "ls -l yields --"
echo "`ls -l`"
echo
DUMP_FILES="`ls $FILE_NAME.dmp.*`"
for i in $DUMP_FILES; do
  echo "Processing file " $i;
  cp $i ${N_DUMPS}$i;
done
echo
echo "Now, ls -l yields --"
echo "`ls -l`"
echo
echo


#######################################################################
#######################################################################
# Copy all output into the work area
DUMP_FILES="`ls n*_$FILE_NAME.dmp.*`"
for i in $DUMP_FILES; do
  echo "Copying " $i " over to " $WORK_DIR;
  cp $i $WORK_DIR;
done


# turn off job accounting and exit
ja -s
echo
echo
echo "All done!"
echo
echo
exit

