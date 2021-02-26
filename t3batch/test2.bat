# Batch script for submitting $FILE_NAME.inp simulation to the T3E
# To use this test script, copy it over to $WORK_DIR (for example,
#   /usr/tmp/bruhwile/test), then cd to that directory and execute
#   the following command:
#
# prompt>  cqsub -q production -la premium test.bat

# Specify administrative details
#
#QSUB -s /bin/sh          # Specify bash shell
#QSUB -r test             # Job name
#QSUB -o test.out         # Write NQS output to specified file
#QSUB -e test.err         # Write NQS error  to specified file
#QSUB -ro -re             # Write output/error messages as they occur

# Specify general parameters of the simulation
#
#QSUB -lM 28Mw            # Specify 28 Mw of memory
#QSUB -lF 10Gb            # Specify 10 Gb of disk space

# Specify parallel specific parameters
#
#QSUB -l mpp_p=2          # Reserve required processors
#QSUB -l mpp_t=04:00:00   # Time limit per processor


set echo
echo
echo "XOOPIC - Cray T3E batch script is beginning execution --"
echo

# Specify the number of processors to use.
# This must be consistent with the  mpp_p  line above.
NUM_PROC=2

# Specify the number of time steps to use
NUM_STEP=800

# Specify the base name for input/output files
FILE_NAME="beamplasmatest3"

# Specify which version of xoopic to execute:
OOPIC_DIR="/u/mp278/bruhwile/projects/oopic"
INPUT_DIR=$OOPIC_DIR"/inp"
WORK_DIR="/usr/tmp/bruhwile/winoff7"

# copy input file and executable to local director,
# set echo, turn on accounting, time job, etc.

cd $TMPDIR
cp $OOPIC_DIR/xg/xoopic .
cp $INPUT_DIR/$FILE_NAME.inp .
cp $INPUT_DIR/$FILE_NAME.inp $WORK_DIR

# turn on job accounting
ja

#######################################################################
#######################################################################
# Run for the specified number of steps...
mpprun -n $NUM_PROC ./xoopic -i $FILE_NAME.inp -s $NUM_STEP -dp $NUM_STEP

# Copy resulting output into local area
N_DUMPS="n0_"
DUMP_FILES="`ls *.dmp*`"
for i in $DUMP_FILES; do
  cp -f $i $WORK_DIR/${N_DUMPS}$i;
done


#######################################################################
#######################################################################
# Restart job and run again for the same number of steps
mpprun -n $NUM_PROC ./xoopic -i $FILE_NAME.inp -d $FILE_NAME -s $NUM_STEP -dp $NUM_STEP

# Copy resulting output into local area
N_DUMPS="n1_"
DUMP_FILES="`ls *.dmp*`"
for i in $DUMP_FILES; do
  cp -f $i $WORK_DIR/${N_DUMPS}$i;
done


# turn off job accounting and exit
ja -s
exit
