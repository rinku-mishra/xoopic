dnl ######################################################################
dnl
dnl File:	txmath.m4
dnl
dnl Purpose:	Determine the locations of txmath includes and libraries
dnl
dnl Version: 	$Id: txmath.m4 2201 2007-02-27 15:40:54Z dws $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Allow the user to specify an overall txmath directory.  If specified,
dnl we look for include and lib under this.
dnl
dnl ######################################################################

AC_ARG_WITH(txmath-dir,[  --with-txmath-dir=<location of txmath installation> ],TXMATH_DIR="$withval",TXMATH_DIR="")

dnl ######################################################################
dnl
dnl Find txmath includes - looking in include location if present,
dnl otherwise in dir/include if present, otherwise in default locations,
dnl first parallel, then serial.
dnl
dnl ######################################################################

AC_ARG_WITH(txmath-incdir,[  --with-txmath-incdir=<location of txmath includes> ],
TXMATH_INCDIR="$withval",TXMATH_INCDIR="")
if test -n "$TXMATH_INCDIR"; then
  TXMATH_INCPATH=$TXMATH_INCDIR
elif test -n "$TXMATH_DIR"; then
  TXMATH_INCPATH=$TXMATH_DIR/include
else
  TXMATH_INCPATH=$HOME/$UNIXFLAVOR/optsolve++/include:$HOME/optsolve++/include:/usr/local/optsolve++/include:/loc/optsolve++/include:/local/optsolve++/include:$HOME/$UNIXFLAVOR/txmath/include:$HOME/txmath/include:/usr/local/txmath/include:/loc/txmath/include:/local/txmath/include
fi
TX_PATH_FILE(TXMATH_H, txc_math.h, "", $TXMATH_INCPATH)
if test -z "$TXMATH_H"; then
  AC_MSG_WARN(txc_math.h not found in $TXMATH_INCPATH.  Set location with --with-txmath-incdir=)
  TXMATH_INC=""
else
  TXMATH_INCDIR=`dirname $TXMATH_H`
  AC_SUBST(TXMATH_INCDIR)
  TXMATH_INC=-I$TXMATH_INCDIR
  AC_SUBST(TXMATH_INC)
  TXMATH_DIR=`dirname $TXMATH_INCDIR`
fi

dnl ######################################################################
dnl
dnl Find txmath libraries
dnl
dnl ######################################################################

AC_ARG_WITH(txmath-libdir,[  --with-txmath-libdir=<location of txmath library> ], TXMATH_LIBDIR="$withval",TXMATH_LIBDIR="")
if test -n "$TXMATH_INCDIR"; then
  if test -n "$TXMATH_LIBDIR"; then
    TXMATH_LIBPATH=$TXMATH_LIBDIR
  else
    TXMATH_LIBPATH=$TXMATH_DIR/lib/$COMPDIR:$TXMATH_DIR/lib
  fi
  TX_PATH_FILE(LIBTXBASE_A, libtxbase.a, "", $TXMATH_LIBPATH)
  if test -z "$LIBTXBASE_A"; then
    AC_MSG_WARN(libtxbase.a not found in $TXMATH_LIBPATH.  Set location with --with-txmath-libdir=)
    TXBASE_LIB=""
  else
    TXMATH_LIBDIR=`dirname $LIBTXBASE_A`
    AC_SUBST(TXMATH_LIBDIR)
    TXBASE_LIB="-ltxbase"
  fi
  AC_SUBST(TXBASE_LIB)
fi

dnl ######################################################################
dnl
dnl Find txmath and verify that version is sufficiently new
dnl
dnl ######################################################################

dnl Check OptSolve++ version number
TX_PATH_FILE(TXMATH_VERSION_CHECK, txmath_version.h, "", $TXMATH_INCDIR)
if test -z "$TXMATH_VERSION_CHECK"; then
  AC_MSG_ERROR(Cannot find txmath_version.h in $TXMATH_INCDIR!
  Please install OptSolve++ or use --with-txmath-dir=TXMATH_INSTALL_DIR)
fi

AC_MSG_CHECKING(OptSolve++ version)
TXMATH_INC_FILE="`dirname $TXMATH_INCDIR`/include/txmath_version.h"
TXMATH_VERSION=`grep VERSION $TXMATH_INC_FILE | sed 's/^.* \"//' | sed 's/\"//g'`
AC_MSG_RESULT($TXMATH_VERSION)
TXMATH_MAJOR_VERSION=`echo $TXMATH_VERSION | sed 's/\..*$//'`
# echo TXMATH_MAJOR_VERSION = $TXMATH_MAJOR_VERSION

if test -z "$TXMATH_OK_MAJOR_VERSION"; then
  TXMATH_OK_MAJOR_VERSION=1
fi
if test -z "$TXMATH_OK_MINOR_VERSION"; then
  TXMATH_OK_MINOR_VERSION=0
fi
if test -z "$TXMATH_OK_RELEASE"; then
  TXMATH_OK_RELEASE=0
fi
if test -z "$TXMATH_OK_VERSION"; then
  TXMATH_OK_VERSION=$TXMATH_OK_MAJOR_VERSION.$TXMATH_OK_MINOR_VERSION.$TXMATH_OK_RELEASE
fi

if test $TXMATH_MAJOR_VERSION -lt $TXMATH_OK_MAJOR_VERSION; then
  echo Major version must be at least $TXMATH_OK_MAJOR_VERSION
  exit
fi
if test $TXMATH_MAJOR_VERSION -eq $TXMATH_OK_MAJOR_VERSION; then
  TXMATH_MINOR_VERSION=`echo $TXMATH_VERSION | sed "s/^$TXMATH_MAJOR_VERSION\.//" | sed 's/\..*$//'`
  # echo TXMATH_MINOR_VERSION = $TXMATH_MINOR_VERSION
  if test $TXMATH_MINOR_VERSION -lt $TXMATH_OK_MINOR_VERSION; then
    echo Minor version must be at least $TXMATH_OK_MINOR_VERSION
    exit
  fi
  if test $TXMATH_MINOR_VERSION -eq $TXMATH_OK_MINOR_VERSION; then
    TXMATH_RELEASE=`echo $TXMATH_VERSION | sed 's/^.*\.//'`
    # echo TXMATH_RELEASE = $TXMATH_RELEASE
    if test $TXMATH_RELEASE -lt $TXMATH_OK_RELEASE; then
      echo Minor version must be at least $TXMATH_OK_RELEASE
      exit
    fi
  fi
fi
echo OptSolve of sufficiently high version

