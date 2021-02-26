# Batch script for submitting beamplasma.inp simulation to the T3E
# To use this script, copy it over to /usr/tmp/bruhwile/batch then
#   cd to that directory and execute the following command:
#
# prompt>  cqsub -q serial -la premium chain5.bat

# Specify administrative details
#
#QSUB -s /bin/sh          # Specify bash shell
#QSUB -r c5_bat01         # Job name
#QSUB -o chain5.out       # Write NQS output to specified file
#QSUB -e chain5.err       # Write NQS error  to specified file
#QSUB -ro -re             # Write output/error messages as they occur

# Specify general parameters of the simulation
#
#QSUB -lT 04:00:00        # Specify  4 hours of CPU time
#QSUB -lM 28Mw            # Specify 28 Mw of memory
#QSUB -lF  2Gb            # Specify  2 Gb of disk space


set echo
echo
echo "XOOPIC - Cray T3E batch script is beginning execution --"
echo

# Specify the number of steps
NUM_STEP=14222

# Specify prefix for the file names
N_DUMPS="n5_"

# Specify the base name for input/output files
FILE_NAME="beamplasmatest"

# Specify which version of xoopic to execute:
OOPIC_DIR="/u/mp278/bruhwile/projects/oopic.serial"
INPUT_DIR=$OOPIC_DIR"/inp"
STORE_DIR="/usr/tmp/bruhwile/batch01"

# copy input file and executable to local director,
# set echo, turn on accounting, time job, etc.

cd $TMPDIR
cp $OOPIC_DIR/xg/xoopic .
cp $INPUT_DIR/$FILE_NAME.inp .

# turn on job accounting
ja

# Copy previous dump files into local temp area
echo
echo
echo "Processing file " $STORE_DIR/$FILE_NAME.dmp
cp -f $STORE_DIR/$FILE_NAME.dmp .


#######################################################################
#######################################################################
echo
echo
echo
./xoopic -i $FILE_NAME.inp -d $FILE_NAME.dmp -s $NUM_STEP -dp $NUM_STEP

# Copy resulting output into local area
echo
echo
echo "Processing file " $FILE_NAME.dmp
cp -f $FILE_NAME.dmp $STORE_DIR
cp -f $FILE_NAME.dmp $STORE_DIR/${N_DUMPS}$FILE_NAME.dmp


# turn off job accounting and exit
ja -s
exit
