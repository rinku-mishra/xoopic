# Batch script for submitting beamplasma.inp simulation to the T3E
# To use this script, copy it over to /usr/tmp/bruhwile/batch then
#   cd to that directory and execute the following command:
#
# prompt>  cqsub -q production -la premium beamplasma.bat

# Specify administrative details
#
#QSUB -s /bin/sh          # Specify bash shell
#QSUB -r beamplasma       # Job name
#QSUB -o beamplasma.out   # Write NQS output to specified file
#QSUB -e beamplasma.err   # Write NQS error  to specified file
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
NUM_STEP1=361
NUM_STEP2=14222

# Specify the base name for input/output files
FILE_NAME="beamplasmatest"

# Specify which version of xoopic to execute:
OOPIC_DIR="/u/mp278/bruhwile/projects/oopic"
INPUT_DIR=$OOPIC_DIR"/inp"
STORE_DIR="/usr/tmp/bruhwile/batch"

# copy input file and executable to local director,
# set echo, turn on accounting, time job, etc.

cd $TMPDIR
cp $OOPIC_DIR/xg/xoopic .
cp $INPUT_DIR/$FILE_NAME.inp .
cp $INPUT_DIR/$FILE_NAME.inp $STORE_DIR

# turn on job accounting
ja

#######################################################################
#######################################################################
# Run enough to get beam fully inside simulation region
mpprun -n $NUM_PROC ./xoopic -i $FILE_NAME.inp -s $NUM_STEP1 -dp $NUM_STEP1

# Copy resulting output into local area
N_DUMPS="n0_"
DUMP_FILES="`ls *.dmp*`"
for i in $DUMP_FILES; do
  cp -f $i $STORE_DIR/${N_DUMPS}$i;
done


#######################################################################
#######################################################################
# Restart job and run beam from 0.0 m to 0.2 m
mpprun -n $NUM_PROC ./xoopic -i $FILE_NAME.inp -d $FILE_NAME -s $NUM_STEP2 -dp $NUM_STEP2

# Copy resulting output into local area
N_DUMPS="n1_"
DUMP_FILES="`ls *.dmp*`"
for i in $DUMP_FILES; do
  cp -f $i $STORE_DIR/${N_DUMPS}$i;
done

#######################################################################
#######################################################################
# Restart job and run beam from 0.2 m to 0.4 m
mpprun -n $NUM_PROC ./xoopic -i $FILE_NAME.inp -d $FILE_NAME -s $NUM_STEP2 -dp $NUM_STEP2

# Copy resulting output into local area
N_DUMPS="n2_"
DUMP_FILES="`ls *.dmp*`"
for i in $DUMP_FILES; do
  cp -f $i $STORE_DIR/${N_DUMPS}$i;
done

#######################################################################
#######################################################################
# Restart job and run beam from 0.2 m to 0.4 m
mpprun -n $NUM_PROC ./xoopic -i $FILE_NAME.inp -d $FILE_NAME -s $NUM_STEP2 -dp $NUM_STEP2

# Copy resulting output into local area
N_DUMPS="n3_"
DUMP_FILES="`ls *.dmp*`"
for i in $DUMP_FILES; do
  cp -f $i $STORE_DIR/${N_DUMPS}$i;
done

#######################################################################
#######################################################################
# Restart job and run beam from 0.2 m to 0.4 m
mpprun -n $NUM_PROC ./xoopic -i $FILE_NAME.inp -d $FILE_NAME -s $NUM_STEP2 -dp $NUM_STEP2

# Copy resulting output into local area
N_DUMPS="n4_"
DUMP_FILES="`ls *.dmp*`"
for i in $DUMP_FILES; do
  cp -f $i $STORE_DIR/${N_DUMPS}$i;
done

#######################################################################
#######################################################################
# Restart job and run beam from 0.2 m to 0.4 m
mpprun -n $NUM_PROC ./xoopic -i $FILE_NAME.inp -d $FILE_NAME -s $NUM_STEP2 -dp $NUM_STEP2

# Copy resulting output into local area

# Copy resulting output into local area
N_DUMPS="n5_"
DUMP_FILES="`ls *.dmp*`"
for i in $DUMP_FILES; do
  cp -f $i $STORE_DIR/${N_DUMPS}$i;
done


# turn off job accounting and exit
ja -s
exit
