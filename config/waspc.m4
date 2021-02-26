dnl ######################################################################
dnl
dnl File:	waspc.m4
dnl
dnl Purpose:	Determine where the waspc tools are.
dnl
dnl Version:	$Id: waspc.m4 1072 2003-10-08 20:59:52Z nanbor $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

AC_ARG_WITH(waspc,
[  --with-waspc=<waspc> to set location of waspc],
WASPC_HOME="$withval")
if test -n "$WASPC_HOME"; then
    echo "WASPC_HOME set by user"
else
     WASPC_HOME=/usr/local/WASP
     dnl WASPC_HOME=/home/research/sveta/WASPC_gcc3.3
fi

WASPC_INCDIR="-I$WASPC_HOME/include"
WASPC_LIBDIR=$WASPC_HOME/lib
JAVA2WSDL=$WASPC_HOME/bin/Java2WSDL
WSDLC=$WASPC_HOME/bin/wsdlc
WSDLC_FLAGS="--config=$WASPC_HOME/share/waspc/conf/wsdlc.xml"
WASPC_LIBS="-L$WASPC_LIBDIR -lwasp"
AC_DEFINE(HAVE_WASPC)
AC_SUBST(WASPC_HOME)
AC_SUBST(WSDLC)
AC_SUBST(WSDLC_FLAGS)
AC_SUBST(WASPC_LIBS)
AC_SUBST(WASPC_INCDIR)
AC_SUBST(WASPC_LIBDIR)
AC_SUBST(JAVA2WSDL)
