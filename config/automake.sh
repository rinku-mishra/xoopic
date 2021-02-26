#!/bin/sh
######################################################################
#
# File:		automake.sh
#
# Purpose:	to run automake with the appropriate arguments
#
# $Id: automake.sh 2369 2007-08-15 19:14:11Z muszala $
#
# Tech-X Corporation configure system
#
######################################################################

#
# See whether we have automake
amver=`automake --version 2>/dev/null | sed -n '1p' | sed 's/^.*automake) //'`
if test $? != 0; then
  echo automake not present on your system.  Should be fine.
  exit
fi
# amver=`echo $amver | sed 's/Written.*$//' | sed 's/ Copy.*$//'`
# echo amver is $amver
ammajver=`echo $amver | sed 's/\..*$//'`
amminver=`echo $amver | sed "s/^$ammajver\.//"`
amminver=`echo $amminver | sed 's/\..*$//' | sed 's/-.*$//'`
# echo automake minor version is $amminver

#
# Determine the Makefile's that are operated on
#
lastarg=`echo $* | sed 's/^.* //'`
mkfls=`echo $lastarg | grep -i Makefile`
if test "$mkfls"; then
  mkfls=$lastarg
  amfile=$lastarg
else
  # echo Constructing mkfls
  if test -f configure.in; then
    CONFIGFILE=configure.in
  elif test -f configure.ac; then
    CONFIGFILE=configure.ac
  else
    echo "Neither configure.in nor configure.ac found.  Quitting."
    exit
  fi
  if test -n "`grep AC_OUTPUT $CONFIGFILE`"; then
    AMFILESBEG=AC_OUTPUT
  elif test -n "`grep AC_CONFIG_FILES $CONFIGFILE`"; then
    AMFILESBEG=AC_CONFIG_FILES
  else
    echo "Cannot determine what the output files start with.  Quitting."
    exit
  fi
  # echo Looking for $AMFILESBEG through ')' in $CONFIGFILE
  AUTOMAKEFILES=`sed -n "/${AMFILESBEG}(/,/)/p" <$CONFIGFILE | sed 's/^.*(//' | sed 's/)//'`
  # echo AUTOMAKEFILES = $AUTOMAKEFILES
# Must excape the shell and the sed command
  # echo mkfls = $AUTOMAKEFILES
  amfile=""
fi

#
# If Makefile.am contains babel.make, then touch that in that directory
# echo mkfls = $mkfls
findMake=`find . -name Makefile.am`
for i in $findMake; do
  hasbabel=`grep babel.make $i`
  if test -n "$hasbabel"; then
    touch `dirname $i`/babel.make
  fi
done

#
# For version 1.4,
# --include-deps prevents the code that invokes gcc -E to create
#	dependency files.  Not done as works with gcc only.
# For version 1.5,
# --ignore-deps prevents the dependency code
# For both
# --foreign stops complaints about missing information files
# --no-force prevent automake from being invoked at every build, as
#	this would undo the final fix if automake is bad
# --add-missing recreates config.sub and config.guess
if test "$amminver" -le 4; then
  cmd="automake --include-deps --foreign --no-force --add-missing --copy $amfile"
else
  cmd="automake --ignore-deps --foreign --no-force --add-missing --copy $amfile"
fi
echo $cmd
$cmd

# Now extract any cru created by decaying automake
if test "$amminver" -le 4; then
  printed=""
  for i in $mkfls; do
    # echo working on $i
    hasar=`grep "AR = ar" $i.in`
    hascru=`grep " cru " $i.in`
    # echo hasar = $hasar
    if test -n "$hasar" -o -n "$hascru"; then
      if test ! "$printed"; then
        echo "automake not patched.  Will attempt patch of Makefile.in's."
        printed=1
      fi
      # sed -f config/automake.sed <$i.in >$i.in1
      sed '/AR = ar/s/ar/@AR@AR_FLAGS = @AR_FLAGS@/' <$i.in >$i.in1
      sed '/@AR@AR_FLAGS/G;s/\(AR_FLAGS = @AR_FLAGS@\)\(\n\)/\2\1/' <$i.in1 >$i.in2
      sed '/ cru /s/ cru//;s/\.a \$(l/.a $(AR_FLAGS) $(l/' <$i.in2 >$i.in
      rm -f $i.in1 $i.in2
    fi
  done
fi

echo automake.sh completed

