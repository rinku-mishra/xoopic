#!/bin/bash
#
# File:		cleanemacs.sh
#
# Purpose:	Replace lines containing only whitespace with empty lines.
#		Emacs tends to make such lines, and they ruin the paragraph
#		feature of vi
#
# Version:	$Id: cleanemacs.sh 1849 2006-06-23 14:26:33Z cary $
#
######################################################################

DEBUG=
for i in .h .cpp .cxx Makefile.am; do
  files=`ls *$i 2>/dev/null`
  if test -n "$files"; then
    for i in $files; do
      # echo Testing $i
      # haswsl=`grep '^[ \t]\+*$' $i`
      # haswsl=`grep '[ \t]\+*$' $i`
      haswsl=`grep '[ \t]\{1,\}$' $i`
      if test -n "$haswsl"; then
        echo File $i has whitespace-only lines!
        echo Result = \'$haswsl\'
        if test -z "$DEBUG"; then
          mv $i $i.orig
          # sed 's/^[ \t]\+*$//' <$i.orig >$i
# sed has different matching from grep
          # sed 's/^[ \t]\{1,\}$//' <$i.orig >$i
          sed 's/[ \t]\{1,\}$//' <$i.orig >$i
        fi
      fi
    done
  fi
done

