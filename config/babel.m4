dnl ######################################################################
dnl
dnl File:	babel.m4
dnl
dnl Purpose:	Determine where the babel files are.
dnl
dnl Version:	$Id: babel.m4 2369 2007-08-15 19:14:11Z muszala $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Determine the location of babel
dnl
dnl ######################################################################

AC_ARG_WITH(babel-dir, [  --with-babel-dir=<babel install directory> to set location of babel], BABEL_DIR="$withval")

if test -z "$BABEL_DIR"; then
  # Look in known directories
  BABEL_PATH=$PATH:/usr/local/babel/bin
else
  BABEL_PATH=$BABEL_DIR/bin
fi

AC_PATH_PROGS(BABEL, babel, , $BABEL_PATH)

if test -z "$BABEL"; then
  AC_MSG_ERROR(Unable to find babel in $BABEL_PATH.  Use --with-babel-dir to set location of babel.)
else
  AC_SUBST(BABEL)
  BABEL_BINDIR=`dirname $BABEL`
  AC_SUBST(BABEL_BINDIR)
  BABEL_DIR=`dirname $BABEL_BINDIR`
  AC_SUBST(BABEL_DIR)
  BABEL_INCDIR=`dirname $BABEL_BINDIR`/include
  AC_SUBST(BABEL_INCDIR)
  BABEL_LIBDIR=`dirname $BABEL_BINDIR`/lib
  AC_SUBST(BABEL_LIBDIR)
  BABEL_CONFIG=`dirname $BABEL_BINDIR`/bin/babel-config
  AC_SUBST(BABEL_CONFIG)
fi

if test -z "babelDirs.m4";then
  AC_MSG_ERROR(babelDirs.m4 not found.  No babel functionality required.)
else
  builtin(include,babelDirs.m4)
fi

dnl ######################################################################
dnl
dnl Now should set fortran compilers as set in babel
dnl
dnl ######################################################################

dnl not yet implemented.

