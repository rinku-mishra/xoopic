# Batch script for submitting $FILE_NAME.inp simulation to the T3E
# To use this test script, copy it over to $WORK_DIR (for example,
#   /usr/tmp/bruhwile/test), then cd to that directory and execute
#   the following command:
#
# prompt>  cqsub -la premium test.bat

# Specify resource requirements
#
#QSUB -lM 40Mw            # Specify 79 Mwords of memory
#QSUB -lT 02:00:00        # Specify 28 minutes of CPU time
#QSUB -lt 02:00:00        # Per Process Max CPU Time Limit
#QSUB -lF 2Gb             # Specify 2 Gigabytes of Disk Space 
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
export NCPUS=4
NUM_PROC=$NCPUS

# Specify the number of time steps to use
NUM_STEP=100

# Specify the base name for input/output files
FILE_NAME="beamplasmatest"

# Specify which version of xoopic to execute:
OOPIC_DIR="/u/abk/bruhwile/projects/oopic"
INPUT_DIR=$OOPIC_DIR"/inp"
WORK_DIR="/u/abk/bruhwile/data/test1"

echo
echo "NCPUS     = " $NCPUS
echo "NUM_PROC  = " $NUM_PROC
echo "NUM_STEP  = " $NUM_STEP
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

echo "ls yields --"
echo "`ls`"
echo

cp $INPUT_DIR/$FILE_NAME.inp $WORK_DIR


#######################################################################
#######################################################################
# Run for the specified number of steps...
echo
echo
echo "Now running XOOPIC --"
echo
echo
mpirun -np $NUM_PROC ./xoopic -i $FILE_NAME.inp -s $NUM_STEP -dp $NUM_STEP

# Copy resulting output to a unique name
N_DUMPS="n0_"
echo
echo
echo "ls yields --"
echo "`ls`"
echo
DUMP_FILES="`ls $FILE_NAME.dmp.*`"
for i in $DUMP_FILES; do
  echo "Processing file " $i;
  cp $i ${N_DUMPS}$i;
done
echo
echo "Now, ls yields --"
echo "`ls`"
echo

#######################################################################
#######################################################################
# Run for the specified number of steps...
echo
echo
echo "Now running XOOPIC --"
echo
echo
mpirun -np $NUM_PROC ./xoopic -i $FILE_NAME.inp -d $FILE_NAME -s $NUM_STEP -dp $NUM_STEP

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

