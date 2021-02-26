dnl ######################################################################
dnl
dnl File:	gsoap.m4
dnl
dnl Purpose:	Determine where the gsoap tools are.
dnl
dnl Version:	$Id: gsoap.m4 2062 2006-10-01 20:46:44Z nanbor $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

AC_ARG_WITH(gsoap,
[  --with-gsoap=<gsoap> to set location of gsoap],
GSOAP_HOME="$withval")
if test -n "$GSOAP_HOME"; then
    echo "GSOAP_HOME set by user"
else
    GSOAP_HOME=/usr/local/gsoap
fi
AC_MSG_NOTICE([Using gSOAP instlllation at: $GSOAP_HOME])

GSOAP_INCDIR="-I$GSOAP_HOME/include"
GSOAP_LIBDIR=$GSOAP_HOME/lib
SOAPCPP2=$GSOAP_HOME/bin/soapcpp2
GSOAP_LIBS="-L$GSOAP_LIBDIR -lgsoap"
GSOAP_LIBSPP="-L$GSOAP_LIBDIR -lgsoap++"
GSOAP_FLAGS=""
AC_DEFINE(HAVE_GSOAP, [], [Specify GSOAP is available])
AC_SUBST(GSOAP_HOME)
AC_SUBST(GSOAP_FLAGS)
AC_SUBST(SOAPCPP2)
AC_SUBST(GSOAP_LIBS)
AC_SUBST(GSOAP_LIBSPP)
AC_SUBST(GSOAP_INCDIR)
AC_SUBST(GSOAP_LIBDIR)

    
