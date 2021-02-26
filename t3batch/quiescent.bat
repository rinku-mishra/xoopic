# Batch script for submitting quiescent.inp simulation to the T3E
# To use this script, copy it over to /usr/tmp/bruhwile/batch then
#   cd to that directory and execute the following command:
#
# prompt>  cqsub -q long quiescent.bat

# Specify administrative details
#
#QSUB -s /bin/sh          # Specify bash shell
#QSUB -r quie064          # Job name
#QSUB -o quie064.out      # Write NQS output to specified file
#QSUB -e quie064.err      # Write NQS error  to specified file
#QSUB -ro -re             # Write output/error messages as they occur

# Specify general parameters of the simulation
#
#QSUB -lM 28Mw            # Specify 28 Mw of memory
#QSUB -lF 10Gb            # Specify 10 Gb of disk space

# Specify parallel specific parameters
#
#QSUB -l mpp_p=64         # Reserve required processors
#QSUB -l mpp_t=12:00:00   # Time limit per processor


set echo
echo
echo "XOOPIC - Cray T3E batch script is beginning execution --"
echo

# Specify the number of processors to use.
# This must be consistent with the  mpp_p  line above.
NUM_PROC=64

# Specify the number of steps
NUM_STEPS=100

# Specify the base name for input/output files
FILE_NAME="quiescent"

# Specify which version of xoopic to execute:
OOPIC_DIR="/u/b/bruhwile/projects/oopic"
INPUT_DIR=$OOPIC_DIR"/inp"
STORE_DIR="/usr/tmp/bruhwile/test064"

# copy input file and executable to local directory,
# set echo, turn on accounting, time job, etc.

cd $TMPDIR
cp $OOPIC_DIR/xg/xoopic .
cp $INPUT_DIR/$FILE_NAME.inp .
cp $INPUT_DIR/$FILE_NAME.inp $STORE_DIR

# turn on job accounting
ja

#######################################################################
#######################################################################
# Run for the specified number of steps
mpprun -n $NUM_PROC ./xoopic -i $FILE_NAME.inp -s $NUM_STEPS -dp $NUM_STEPS

# Copy resulting output into local area
DUMP_FILES="`ls *.dmp*`"
for i in $DUMP_FILES; do
  cp -f $i $STORE_DIR/$i;
done


# turn off job accounting and exit
ja -s
exit
