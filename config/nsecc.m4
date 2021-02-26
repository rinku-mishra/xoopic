dnl ######################################################################
dnl
dnl File:	nsecc.m4
dnl
dnl Purpose:	Determine the locations of nsecc includes and libraries. 
dnl 		Allows also for the installation of the ML library. 
dnl 
dnl Version: $Id: nsecc.m4 1582 2005-07-18 12:31:56Z cary $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################


dnl ######################################################################
dnl
dnl Allow the user to specify an overall nsecc directory.  If specified,
dnl we look for include and lib under this.
dnl
dnl ######################################################################

AC_ARG_WITH(nsecc-dir,[  --with-nsecc-dir=<location of nsecc installation> ],NSECC_DIR="$withval",NSECC_DIR="")

dnl ######################################################################
dnl
dnl Find nsecc includes - looking in include location if present,
dnl otherwise in dir/include if present, otherwise in default locations,
dnl first parallel, then serial.
dnl
dnl ######################################################################

AC_ARG_WITH(nsecc-incdir,[  --with-nsecc-incdir=<location of nsecc includes> ],
NSECC_INCDIR="$withval",NSECC_INCDIR="")
if test -n "$NSECC_INCDIR"; then
  NSECC_INCPATH=$NSECC_INCDIR
elif test -n "$NSECC_DIR"; then
  NSECC_INCPATH=$NSECC_DIR/include
else
  NSECC_INCPATH="/usr/local/txphysics/include"
fi

if test -n "$NSECC_INCPATH"; then 
  AC_PATH_PROGS(NSECC_H, nsecc.h, "", $NSECC_INCPATH)
  if test -z "$NSECC_H"; then
    AC_MSG_WARN(nsecc.h not found in $NSECC_INCPATH.  Set with --with-nsecc-incdir=)
    NSECC_INC=" "
  else
    NSECC_INCDIR=`dirname $NSECC_H`
    AC_SUBST(NSECC_INCDIR)
    NSECC_INC="-I$NSECC_INCDIR"

    AC_SUBST(NSECC_INC)
    NSECC_DIR=`dirname $NSECC_INCDIR`
  fi
fi

dnl ######################################################################
dnl
dnl Find nsecc libraries
dnl
dnl ######################################################################

AC_ARG_WITH(nsecc-libdir,[  --with-nsecc-libdir=<location of nsecc library> ],
NSECC_LIBDIR="$withval",NSECC_LIBDIR="")

if test -n "$NSECC_LIBDIR"; then
  NSECC_LIBPATH=$NSECC_LIBDIR
elif test -n "$NSECC_DIR"; then
  NSECC_LIBPATH=$NSECC_DIR/lib
else
  NSECC_LIBPATH=""
fi

if test -n "$NSECC_LIBPATH"; then
  AC_PATH_PROGS(LIBNSECC_A, libnsecc.a, "", $NSECC_LIBPATH)
  if test -z "$LIBNSECC_A"; then
    AC_MSG_WARN(libnsecc.a not found.  Set with --with-nsecc-libdir=)
    NSECC_LIBS=""
  else
    NSECC_LIBDIR=`dirname $LIBNSECC_A`
    AC_SUBST(NSECC_LIBDIR)
    NSECC_LIBS="-L$NSECC_LIBDIR -lnsecc $FLIBS"' $(RPATH_FLAG)'"$NSECC_LIBDIR"
  fi
  AC_SUBST(NSECC_LIBS)
fi

dnl ######################################################################
dnl
dnl Define for whether nsecc found
dnl
dnl ######################################################################

if test -n "$NSECC_LIBS"; then
  AC_DEFINE(HAVE_NSECC)
fi

