#!/bin/sh
######################################################################
#
# File:		runconfig.sh
#
# Purpose:	Script to determine most options.  User specifies rest.
#
# Version:	$Id: runconfig.sh 1781 2001-11-07 20:08:01Z cary $
#
# XGRAFIX
#
######################################################################

#
# Find location of this file and, hence, configure
#
shfile=$0
srcdir=`dirname $shfile`
builddir=`pwd`
lineopts="$*"
haveprefix=`echo $lineopts | grep -- --prefix=`
# echo lineopts = $lineopts

#
# Determine the host
#
if [ -f /usr/ucb/hostname ]; then
  hostnm=`/usr/ucb/hostname`
else
  hostnm=`hostname`
fi

#
# Look for seaborg
#
isseaborg=`echo $hostnm | grep s00`
if test "$isseaborg"; then hostnm=seaborg.nersc.gov; fi
echo host is $hostnm

#
# Offer parallel choice on some platforms
#
case $hostnm in

  mcurie | mcurie.nersc.gov)
    prl="y"
    ;;

  *)
    echo "Build parallel (y/n)?"
    read prl
    ;;

esac

#
# Offer to fix Makefile.in's
#
echo "Fix Makefile.in's? (y/n)"
read fixit
if test "$fixit" = yes -o "$fixit" = y; then
  cd $srcdir
  config/automake.sh
  cd $builddir
fi

#
# Known options
#
case $hostnm in

  seaborg | seaborg.nersc.gov)
    echo assuming host is seaborg.nersc.gov
    confopts="--with-CC=cc --with-CXX=mpCC_r --with-MPICC=mpcc_r --with-MPICXX=mpCC_r --enable-MPI --with-XGmini=yes"
    if test ! "$haveprefix"; then 
      confopts="$confopts --prefix=$HOME/unicos2.0"
    fi
    ;;

  mcurie | mcurie.nersc.gov)
    echo assuming host is mcurie.nersc.gov
    confopts="--with-CC=cc --with-CXX=CC --enable-MPI --with-MPICXX=CC --with-XGmini=yes"
    if test ! "$haveprefix"; then 
      confopts="$confopts --prefix=$HOME/unicos2.0"
    fi
    ;;

  quad.txcorp.com | tech-x.txcorp.com | minotaur.colorado.edu | penguin.colorado.edu)
    if test "$prl" = y -o "$prl" = yes; then
      confopts="--enable-MPI --with-xpm=/usr/X11R6/lib --with-XGmini=yes"
    else
      confopts="--with-xpm=/usr/X11R6/lib"
    fi
    if test ! "$haveprefix"; then 
      confopts="$confopts --prefix=/usr/local/xoopic"
    fi
    ;;

  *)
    echo Machine not known.  Will try on basis of OS.
    case `uname` in

      Linux)
        if test "$prl" = y -o "$prl" = yes; then
          confopts="--enable-MPI --with-xpm=/usr/X11R6/lib --with-XGmini=yes"
        else
          confopts="--with-xpm=/usr/X11R6/lib"
        fi
        if test ! "$haveprefix"; then 
          confopts="$confopts --prefix=/usr/local/xoopic"
        fi
        ;;

      *)
        echo OS not known.  Will try fallback.
        if test "$prl" = y -o "$prl" = yes; then
          confopts="--enable-MPI --with-xpm=/usr/X11R6/lib"
        else
          confopts="--with-xpm=/usr/X11R6/lib"
        fi
        if test ! "$haveprefix"; then 
          confopts="$confopts --prefix=/usr/local/xoopic"
        fi
        ;;

  esac

esac
confopts="$confopts $lineopts"

#
# Allow additional options
#
echo configure options: $confopts
echo
echo "Other options (e.g., --enable-optimize)?  Or type \"quit\" to quit."
read otheropts
if test "$otheropts" = quit; then exit; fi
confopts="$confopts $otheropts"

#
# Will now run configure
#
cmd="$srcdir/configure $confopts"
echo $cmd
$cmd

