#!/bin/sh
######################################################################
#
# File:		testinput.sh
#
# Purpose:	Test script to execute all the input files in input and
#               inp with either OOPIC Pro or XOOPIC.
#
# Version:	$Id: testinput.sh 1825 2002-01-21 18:50:54Z kgl $
#
######################################################################

##
# Find location of this file 
##
shfile=$0
srcdir=`dirname $shfile`
builddir=`pwd`
lineopts="$*"

##
# Determine the host
##
if [ -f /usr/ucb/hostname ]; then
  hostnm=`/usr/ucb/hostname`
else
  hostnm=`hostname`
fi

##
# Echo banner
##
echo " "
echo "Testing the input files on " $hostnm
echo " "

##
# Check for OOPIC Pro or XOOPIC executable.
##
exec=""
if test -f ../xg/xoopic ; then
  exec="../xg/xoopic"
else
  if test -f ../bin/oopicpro; then
    exec="../bin/oopicpro"
  fi
fi

def_exec=$exec
echo -n "Executable? [$def_exec] "
read exec
if test "$exec" = ""; then
  exec=$def_exec
fi

##
# Get the number of steps (-s)
##
def_nsteps=2
echo -n "Number of steps? [$def_nsteps] "
read nsteps
if test "$nsteps" = ""; then
  nsteps=$def_nsteps
fi

##
# Determine if testing the GUI (-nox)
##
def_ans="no"
echo -n "Display GUI? (no/yes) [no] "
read ans
if test "$ans" = ""; then
  ans=$def_ans
fi

nox="-nox"
if test "$ans" = "yes"; then
 nox=""
fi

##
# Get the output device
##
def_outdevice="term"
echo -n "Output device? (term/logfile) [term]  "
read outdevice
if test "$outdevice" = ""; then
  outdevice=$def_outdevice
fi

##
# Get a file name for logging if not term output
##
if test "$outdevice" = "term"; then
 outdevice=""
else
 echo -n "Log filename? [testinput.log]  "
 read filename
 if test "$filename" = ""; then
   filename="testinput.log"
 fi
 outdevice="$filename"
 if test -f "$outdevice"; then
   def_action="o"
   echo -n "$outdevice exists, overwrite or append? (o/a) [o] "
   read action
   if test "$action" = ""; then
     action="$def_action"
   fi
   if test "$action" = "o"; then
     /bin/rm $outdevice
   fi
   touch $outdevice
 fi
 echo "Log file is " $outdevice
fi

##
# Get input directories
##
def_dirs="../input ../inp"
echo -n "Input directories? [ $def_dirs ] "
read dirs
if test "$dirs" = ""; then
  dirs="$def_dirs"
fi

##
# Loop over the input files
##
for d in $dirs; do
  files=`ls ${d}/*.inp`
  for file in $files; do
    echo "Processing ${file}"
    echo "$exec -i ${file} -s $nsteps $nox "
    cmd="${exec} -i ${file} -s $nsteps $nox "
    if test "$outdevice" = ""; then
      $cmd
    else
      echo " " >> $outdevice
      echo " " >> $outdevice
      echo "Processing ${file}" >> $outdevice
      echo "$exec -i ${file} -s $nsteps $nox >> $outdevice" >> $outdevice
      /bin/rm -f testinput.exe
      echo "#!/bin/sh" > testinput.exe
      echo "$cmd" >> testinput.exe 
      chmod a+x testinput.exe
      ./testinput.exe >> $outdevice 2>&1
      echo "finished ${file} " >> $outdevice 
    fi
  done
done
/bin/rm -f testinput.exe

