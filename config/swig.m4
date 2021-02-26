dnl ######################################################################
dnl
dnl Check for SWIG stuff
dnl
dnl ######################################################################

dnl Find location of swig

AC_ARG_WITH(SWIG_ROOT,
[  --with-SWIG_ROOT=<swig install root>     to set root for python installation],
SWIG_ROOT="$withval")
if test -n "$SWIG_ROOT"; then
  SWIG_PATH=$SWIG_ROOT/bin
else
  SWIG_PATH=$PATH:$HOME/$host/swig/bin:/usr/local/swig/bin
fi
AC_PATH_PROGS(SWIG, swig, "", $SWIG_PATH)
if test -n "$SWIG"; then
  SWIG_BINDIR=`dirname $SWIG`
  SWIG_ROOT=`dirname $SWIG_BINDIR`
else
  AC_MSG_WARN(Unable to find swig in your path.)
fi
AC_SUBST(SWIG)

dnl ######################################################################
dnl
dnl Find the swig libraries
dnl
dnl ######################################################################

AC_MSG_CHECKING(for swig library)
SWIG_LIBDIR=$SWIG_ROOT/lib
AC_SUBST(SWIG_LIBDIR)
SWIG_VERSION=`$SWIG -version 2>&1| grep Version | cut -b 14-16`
SWIG_LIB="swig"$SWIG_VERSION
AC_SUBST(SWIG_LIB)
AC_MSG_RESULT(-L$SWIG_LIBDIR -l$SWIG_LIB)
