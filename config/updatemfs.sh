#!/bin/bash
#
# File:		updatesrcs.sh
#
# Version:	$Id: updatemfs.sh 2071 2006-10-08 21:24:12Z cary $
#

DEBUG=
if test -n "$DEBUG"; then
  DECHO=echo
else
  DECHO=:
fi

# Method for replacing a group of lines starting with $replacelinestr
# and ending with a blank line.  Replace with the first line of
# that stanza plus the contents of the file, replacefile
replacelines()
{
  if test ! -f "$replacefile"; then
    echo No file named $replacefile
    echo Skipping this replacement
    return
  fi
  replaceline=`sed -n "/${replacelinestr}[ \t]*=/p" Makefile.am`
  $DECHO replaceline = $replaceline
  if test -z "$replaceline"; then
    echo No line containing $replacelinestr
    echo Skipping this replacement
    return
  fi
  numreplacelines=`grep "${replacelinestr}[ \t]*=" Makefile.am | wc -l`
  numreplacelines=`echo $numreplacelines | sed 's/Makefile.am//' | sed 's/ //g'`
  if test "$numreplacelines" != 1; then
    echo Multiple lines containing $replacelinestr
    echo Skipping this replacement
    return
  fi
  mv Makefile.am Makefile.am.orig
# Get sources line
# Count total lines
  lines=`wc -l Makefile.am.orig | sed 's/ Makefile.am.orig//' | sed 's/ //g'`
  $DECHO lines = $lines
# Count lines up to where the sources begin
  prelines=`sed "/^.*${replacelinestr}[ \t]*=/,$"d <Makefile.am.orig | wc -l`
  $DECHO prelines = $prelines
# Count source lines
  replacelines=`sed -n "/^.*${replacelinestr}[ \t]*=/,/^$/"p <Makefile.am.orig | wc -l`
  $DECHO replacelines = $replacelines
  postreplacelineno=`expr $prelines + $replacelines`
  $DECHO postreplacelineno = $postreplacelineno
# Construct new Makefile.am
  sed -n "1,${prelines}p" <Makefile.am.orig >Makefile.am
  echo $replaceline >>Makefile.am
  cat ${replacefile} >>Makefile.am
  echo >>Makefile.am
  sed "1,${postreplacelineno}d" <Makefile.am.orig >>Makefile.am
}


#
# Replace the library sources
#
replacelinestr=_a_SOURCES
replacefile=cppsrcs.txt
replacelines

#
# Replace the headers
#
replacelinestr=include_HEADERS
replacefile=hdrs.txt
replacelines

#
# Replace the extra distribution files
#
replacelinestr=EXTRA_DIST
replacefile=distfiles.txt
replacelines

