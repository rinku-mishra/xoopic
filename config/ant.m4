dnl ######################################################################
dnl
dnl File:       ant.m4
dnl
dnl Purpose:    Determine where ant is.
dnl
dnl Version:    $Id: ant.m4 1923 2006-07-22 19:39:30Z roopa $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################
 
dnl ######################################################################
dnl
dnl Determine the location of ant
dnl
dnl ######################################################################
AC_ARG_WITH(ant-dir, [  --with-ant-dir=<ant install directory> to set location of ant], ANT_DIR="$withval")

if test -z "$ANT_DIR"; then
  ANT_DIR=/usr/local/apache-ant-1.5.3-1
fi

ANT_BINDIR=$ANT_DIR/bin
AC_SUBST(ANT_BINDIR)
ANT_LIBDIR=$ANT_DIR/lib
AC_SUBST(ANT_LIBDIR)

AC_PATH_PROGS(ANT, ant, , $ANT_BINDIR)

if test -n "$ANT"; then
  echo setting ant... $ANT
else
    AC_MSG_ERROR(Unable to find ant in $ANT_DIR.  Use --with-ant-dir to set the correct location of ant.)
fi

