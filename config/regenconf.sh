#!/bin/sh
######################################################################
#
# File:         regenconf.sh
#
# Purpose:      to regenerate autotools files
#
# Version:      $Id: regenconf.sh 2372 2007-08-19 17:09:35Z cary $
#
# Copyright 2003, Tech-X Corporation
#
######################################################################

# Get rid of old files
echo Remaking all configuration files
mydir=`dirname $0`

# Creating new files for this platform
for i in aclocal \
	autoheader \
	"rm -rf autom4te.cache autom4te-*.cache" \
	"autoconf --force"; do
  echo $i
  $i
done

# See if libtoolize should be run
if test -f configure.in; then
  res=`grep LIBTOOL configure.in | grep -v dnl`
else
  res=`grep LIBTOOL configure.ac | grep -v dnl`
fi
if test -n "$res"; then
  echo Redoing libtoolize
  cmd="libtoolize --force --copy"
  if test "`uname`" == "Darwin"; then

# NDS: glibtoolize works on OS X 10.4 (Tiger)
# JRC: glibtoolize does NOT work on my 10.4.
# JRC: Should look for (g)libtoolize in this order:
#
# 1. .../libtoolize (i.e., NOT /usr/bin)
# 2. glibtoolize
# 3. /usr/bin/libtoolize

    lt=/usr/bin/libtoolize
    tmp=`which glibtoolize | sed 's/ /_/g'`
    if test -x "$tmp"; then
      lt=$tmp
    fi
    tmp=`which libtoolize | sed 's/ /_/g'`
    if test -x "$tmp" -a "$tmp" != /usr/bin/libtoolize; then
      lt=$tmp
    fi
    cmd="$lt --force --copy"
  fi
  echo $cmd
  $cmd
fi

# Do automake
cmd="config/automake.sh"
echo $cmd
$cmd
cp configure configure.bak
cat configure.bak | sed 's/test -x/test -r/1' > configure
rm configure.bak

