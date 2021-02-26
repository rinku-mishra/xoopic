dnl ######################################################################
dnl
dnl File:	ccfits.m4
dnl
dnl Purpose:	Determine where CCfits is.
dnl
dnl Version:	$Id: ccfits.m4 1 2007-01-29 20:44:17Z paulh $
dnl
dnl Copyright Tech-X Corporation, 2007.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

AC_ARG_WITH(CCFITS_HOME,
[  --with-CCFITS_HOME=<CCfits-home-dir>      to set location of CCfits],
CCFITS_HOME="$withval")

dnl
dnl If not known, check in typical directories
dnl
if test -n "$CCFITS_HOME"; then
  echo "CCFITS_HOME set by user"
else
  CCFITS_HOME=/usr/local/CCfits
fi

CCFITS_INCDIR=$CCFITS_HOME/include
CCFITS_LIBDIR=$CCFITS_HOME/lib
CCFITS_LIBS="-L$CCFITS_LIBDIR -lCCfits"

if test -z "$CCFITS_HOME"; then
  AC_MSG_WARN(Unable to find CCfits.)
else
  AC_DEFINE(HAVE_CCFITS, , Define if CCfits is there)
fi

AC_SUBST(CCFITS_HOME)
AC_SUBST(CCFITS_INCDIR)
AC_SUBST(CCFITS_LIBDIR)
AC_SUBST(CCFITS_LIBS)
