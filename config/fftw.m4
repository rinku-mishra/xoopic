dnl ######################################################################
dnl
dnl File:	fftw.m4
dnl
dnl Purpose:	Looks for single and double precision versions of 
dnl		FFTW libraries
dnl
dnl Version:	$Id: fftw.m4 1955 2006-07-28 21:44:41Z rmtrines $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################

dnl ######################################################################
dnl 
dnl Allow users to disable the the fftw library when building. 
dnl 
dnl ######################################################################

dnl AC_ARG_ENABLE([fftw],
dnl               AC_HELP_STRING([--disable-fftw],
dnl                              [compile without fftw (default is to use fftw)]),
dnl               [ac_cv_disable_fftw=$withval], [ac_cv_disable_fftw=no])
dnl echo "ac_cv_disable_fftw = $ac_cv_disable_fftw"

dnl ######################################################################
dnl
dnl Allow the user to specify an overall fftw directory.  If specified,
dnl we look for include and lib under this.
dnl
dnl ######################################################################

AC_ARG_WITH(dfftw-dir,[  --with-dfftw-dir=<location of dfftw installation> ],DFFTW_DIR="$withval",DFFTW_DIR="")

dnl ######################################################################
dnl
dnl Find dfftw includes - looking in include location if present,
dnl otherwise in dir/include if present, otherwise in default locations,
dnl first parallel, then serial.
dnl
dnl ######################################################################

AC_ARG_WITH(dfftw-incdir,[  --with-dfftw-incdir=<location of dfftw includes> ],
DFFTW_INCDIR="$withval",DFFTW_INCDIR="")
if test -n "$DFFTW_INCDIR"; then
  DFFTW_INCPATH=$DFFTW_INCDIR
elif test -n "$DFFTW_DIR"; then
  DFFTW_INCPATH=$DFFTW_DIR/include
else
  DFFTW_INCPATH=$HOME/include:/usr/local/fftw/include:/loc/fftw/include:/local/fftw/include:/usr/common/usg/include:/usr/include
fi
AC_PATH_PROGS(DFFTW_H, dfftw.h, "", $DFFTW_INCPATH)
if test -z "$DFFTW_H"; then
  AC_MSG_WARN(dfftw.h not found in $DFFTW_INCPATH.  Set location with --with-dfftw-incdir=)
  DFFTW_INC=" "
  ac_cv_have_dfftw=no
else
  DFFTW_INCDIR=`dirname $DFFTW_H`
  AC_SUBST(DFFTW_INCDIR)
  DFFTW_INC=-I$DFFTW_INCDIR
  AC_SUBST(DFFTW_INC)
  DFFTW_DIR=`dirname $DFFTW_INCDIR`
  ac_cv_have_dfftw=yes
fi

dnl ######################################################################
dnl
dnl Find dfftw libraries
dnl
dnl ######################################################################

AC_ARG_WITH(dfftw-libdir,[  --with-dfftw-libdir=<location of dfftw library> ], DFFTW_LIBDIR="$withval",DFFTW_LIBDIR="")
if test $ac_cv_have_dfftw = yes; then
  if test -n "$DFFTW_LIBDIR"; then
    DFFTW_LIBPATH=$DFFTW_LIBDIR
  else
    DFFTW_LIBPATH=$DFFTW_DIR/lib
  fi
  AC_PATH_PROGS(LIBDFFTW_A, libdfftw.a, "", $DFFTW_LIBPATH)
  if test -z "$LIBDFFTW_A"; then
    AC_MSG_WARN(libdfftw.a not found in $DFFTW_LIBPATH.  Set location with --with-dfftw-libdir=)
    ac_cv_have_dfftw=no
    DFFTW_LIB=" "
  else
    DFFTW_LIBDIR=`dirname $LIBDFFTW_A`
    AC_SUBST(DFFTW_LIBDIR)
    DFFTW_LIB="-L$DFFTW_LIBDIR -ldrfftw -ldfftw -lm"
  fi
  AC_SUBST(DFFTW_LIB)
  AC_DEFINE(HAVE_DFFTW, , Define if have the double fftw libraries)
fi

dnl ######################################################################
dnl
dnl Allow the user to specify an overall fftw directory.  If specified,
dnl we look for include and lib under this.
dnl
dnl ######################################################################

AC_ARG_WITH(sfftw-dir,[  --with-sfftw-dir=<location of sfftw installation> ],SFFTW_DIR="$withval",SFFTW_DIR="")

dnl ######################################################################
dnl
dnl Find sfftw includes - looking in include location if present,
dnl otherwise in dir/include if present, otherwise in default locations,
dnl first parallel, then serial.
dnl
dnl ######################################################################

AC_ARG_WITH(sfftw-incdir,[  --with-sfftw-incdir=<location of sfftw includes> ],
SFFTW_INCDIR="$withval",SFFTW_INCDIR="")
if test -n "$SFFTW_INCDIR"; then
  SFFTW_INCPATH=$SFFTW_INCDIR
elif test -n "$SFFTW_DIR"; then
  SFFTW_INCPATH=$SFFTW_DIR/include
else
  SFFTW_INCPATH=$HOME/include:/usr/local/fftw/include:/loc/fftw/include:/local/fftw/include:/usr/common/usg/include:$DFFTW_INCDIR
fi
AC_PATH_PROGS(SFFTW_H, sfftw.h, "", $SFFTW_INCPATH)
if test -z "$SFFTW_H"; then
  AC_MSG_WARN(sfftw.h not found in $SFFTW_INCPATH.  Set location with --with-sfftw-incdir=)
  SFFTW_INC=" "
  ac_cv_have_sfftw=no
else
  SFFTW_INCDIR=`dirname $SFFTW_H`
  AC_SUBST(SFFTW_INCDIR)
  SFFTW_INC=-I$SFFTW_INCDIR
  AC_SUBST(SFFTW_INC)
  SFFTW_DIR=`dirname $SFFTW_INCDIR`
  ac_cv_have_sfftw=yes
fi

dnl ######################################################################
dnl
dnl Find sfftw libraries
dnl
dnl ######################################################################

AC_ARG_WITH(sfftw-libdir,[  --with-sfftw-libdir=<location of sfftw library> ], SFFTW_LIBDIR="$withval",SFFTW_LIBDIR="")
if test $ac_cv_have_sfftw = yes; then
  if test -n "$SFFTW_LIBDIR"; then
    SFFTW_LIBPATH=$SFFTW_LIBDIR
  else
    SFFTW_LIBPATH=$SFFTW_DIR/lib:$DFFTW_LIBDIR
  fi
  AC_PATH_PROGS(LIBSFFTW_A, libsfftw.a, "", $SFFTW_LIBPATH)
  if test -z "$LIBSFFTW_A"; then
    AC_MSG_WARN(libsfftw.a not found in $SFFTW_LIBPATH.  Set location with --with-sfftw-libdir=)
    ac_cv_have_sfftw=no
    SFFTW_LIB=" "
  else
    SFFTW_LIBDIR=`dirname $LIBSFFTW_A`
    AC_SUBST(SFFTW_LIBDIR)
    SFFTW_LIB="-L$SFFTW_LIBDIR -lsrfftw -lsfftw -lm"
  fi
  AC_SUBST(SFFTW_LIB)
  AC_DEFINE(HAVE_SFFTW, Define if have the single-precision FFTW libraries)
fi



dnl ######################################################################
dnl
dnl check the options for FFTW_PRECISION
dnl 
dnl ######################################################################
FFTW_INCDIR=""
FFTW_LIB=""

if test "$FFTW_PRECISION" = "double"; then
  dnl
  dnl use the double precision version of the fftw lib
  dnl 
  if test "$DFFTW_INCDIR"; then
    FFTW_INCDIR="$DFFTW_INCDIR"
    AC_SUBST(FFTW_INCDIR)
    FFTW_INC="$DFFTW_INC"
    AC_SUBST(FFTW_INC)
  fi
  if test "$DFFTW_LIB"; then
    FFTW_LIB="$DFFTW_LIB"
    AC_SUBST(FFTW_LIB)
  fi
  if test "$FFTW_INCDIR" -a "$FFTW_LIB"; then
    AC_DEFINE(HAVE_FFT, 
	Define if the FFTW library of the appropriate precision found)
  fi
elif test "$FFTW_PRECISION" = "float"; then
  dnl
  dnl use the single precision version of the fftw lib
  dnl
  if test "$SFFTW_INCDIR"; then 
    FFTW_INCDIR="$SFFTW_INCDIR"
    AC_SUBST(FFTW_INCDIR)
    FFTW_INC="$SFFTW_INC"
    AC_SUBST(FFTW_INC)
  fi 
  if test "$SFFTW_LIB"; then 
    FFTW_LIB="$SFFTW_LIB" 
    AC_SUBST(FFTW_LIB) 
  fi 
  if test "$SFFTW_INCDIR" -a "$SFFTW_LIB"; then 
    AC_DEFINE(HAVE_FFT)
  fi
else
  AC_ARG_WITH(FFTW_PRECISION, 
  [  --with-FFTW_PRECISION=<precision> set precision double or float for FFTW],
  FFTW_PRECISION="$withval")
  if test "$FFTW_PRECISION" = "float"; then
    dnl
    dnl use the single precision version of the fftw lib
    dnl
    echo "Setting DFFTW via the --with-FFTW_PRECISION=float option"
    if test "$SFFTW_INCDIR"; then 
      FFTW_INCDIR="$SFFTW_INCDIR"
      AC_SUBST(FFTW_INCDIR)
    fi 
    if test "$SFFTW_LIB"; then 
      FFTW_LIB="$SFFTW_LIB" 
      AC_SUBST(FFTW_LIB) 
    fi 
    if test "$SFFTW_INCDIR" -a "$SFFTW_LIB"; then 
      AC_DEFINE(HAVE_FFT)
    fi
  elif test "$FFTW_PRECISION" = "double"; then
    dnl
    dnl use the double precision version of the fftw lib
    dnl 
    echo "Setting DFFTW via the --with-FFTW_PRECISION=double option"
    if test "$DFFTW_INCDIR"; then
      FFTW_INCDIR="$DFFTW_INCDIR"
      AC_SUBST(FFTW_INCDIR)
    fi
    if test "$DFFTW_LIB"; then
      FFTW_LIB="$DFFTW_LIB"
      AC_SUBST(FFTW_LIB)
    fi
    if test "$FFTW_INCDIR" -a "$FFTW_LIB"; then
      AC_DEFINE(HAVE_FFT)
    fi
  fi
fi
