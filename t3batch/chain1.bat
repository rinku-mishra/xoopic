# Batch script for submitting beamplasma.inp simulation to the T3E
# To use this script, copy it over to /usr/tmp/bruhwile/batch then
#   cd to that directory and execute the following command:
#
# prompt>  cqsub -q production -la premium chain1.bat

# Specify administrative details
#
#QSUB -s /bin/sh          # Specify bash shell
#QSUB -r c1_bat02         # Job name
#QSUB -o chain1.out       # Write NQS output to specified file
#QSUB -e chain1.err       # Write NQS error  to specified file
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

# Specify the number of steps
NUM_STEP=14022

# Specify prefix for the file names
N_DUMPS="n1_"

# Specify the base name for input/output files
FILE_NAME="beamplasmatest"

# Specify which version of xoopic to execute:
OOPIC_DIR="/u/mp278/bruhwile/projects/oopic"
INPUT_DIR=$OOPIC_DIR"/inp"
STORE_DIR="/usr/tmp/bruhwile/batch02"

# copy input file and executable to local director,
# set echo, turn on accounting, time job, etc.

cd $TMPDIR
cp $OOPIC_DIR/xg/xoopic .
cp $INPUT_DIR/$FILE_NAME.inp .

# turn on job accounting
ja

# Copy previous dump files into local temp area
DUMP_FILES="`ls $STORE_DIR/$FILE_NAME.dmp*`"
echo
echo
for i in $DUMP_FILES; do
  echo "Processing file " $i;
  cp -f $i .;
done

#######################################################################
#######################################################################
echo
echo
echo
mpprun -n $NUM_PROC ./xoopic -i $FILE_NAME.inp -d $FILE_NAME -s $NUM_STEP -dp $NUM_STEP

# Copy resulting output into local area
DUMP_FILES="`ls *.dmp*`"
echo
echo
for i in $DUMP_FILES; do
  echo "Processing file " $i;
  cp -f $i $STORE_DIR;
  cp -f $i $STORE_DIR/${N_DUMPS}$i;
done

# Submit the next script in the chain
cd $STORE_DIR
cqsub -q production -la premium chain2.bat

# turn off job accounting and exit
ja -s
exit
