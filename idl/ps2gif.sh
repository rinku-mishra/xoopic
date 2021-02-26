#!/bin/sh
#
# ps2gif script by Ian Hutchinson 1998; use at your own risk.
# You need Ghostscript and the pbmplus utilities installed. 
#
# Renamed to ps2gif.sh by David Bruhwiler (Tech-X Corporation), May 10, 2001
# Replaced use of Ghostscript with the linux utility "convert"
#
# This script is used in conjunction with IDL, because IDL generates good
# postscript (but lousy gif) files and gif is needed for making the images
# web-accessible.
#
# The last line scp's the resulting gif to a hard-wired directory.
# This line will probably have to be altered or deleted.
#
if [ $# != 2 ] ; then
       echo " Usage: ps2gif.sh <file.ps> <file.gif>" 1>&2
       exit 1
else 
	echo "Converting from postscript to gif.  Please wait ..." >&2
#
# I've commented out the original Ghostscript code...
#
#	gs -sDEVICE=ppmraw -sOutputFile=- -sNOPAUSE -q $1 -c showpage -c quit | pnmcrop| pnmmargin 10 | ppmtogif >$2
#
# Here's the new line, which invokes "convert":
	convert $1 $2
	scp $2 tech-x.txcorp.com:projects/fusion_html/xoopic/loasis2/$2
fi



