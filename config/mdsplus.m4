dnl ######################################################################
dnl
dnl File:	mdsplus.m4
dnl
dnl Purpose:	Determine where the mdsplus data base tools are.
dnl
dnl Version:	$Id: mdsplus.m4 1107 2003-11-12 19:13:16Z nanbor $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

AC_ARG_WITH(MDS_PLUS,
[  --with-MDS_PLUS=<mds plus dir>           to set location of mds plus],
MDS_PLUS="$withval")
if test -n "$MDS_PLUS"; then
  if test "$MDS_PLUS" = "none"; then
    MDS_PLUS_INCDIR=""
    MDS_PLUS_LIBDIR=""
  else
    MDS_PLUS_INCDIR="-I$MDS_PLUS/include -I$MDS_PLUS/mdstcpip"
    MDS_PLUS_LIBDIR=$MDS_PLUS/lib
  fi
else
  AC_PATH_PROGS(MDS_PLUS_LIB, libMdsIpShr.a libMdsIpShr$SO, "", /mfe/local/mdsplus/shlib:/loc/mdsplus/shlib:/usr/local/mdsplus/shlib:/usr/local/mdsplus/lib)
  if test -n "$MDS_PLUS_LIB"; then
    MDS_PLUS_LIBDIR=`dirname $MDS_PLUS_LIB`
    MDS_PLUS_DIR=`dirname $MDS_PLUS_LIBDIR`
    MDS_PLUS_INCDIR="-I${MDS_PLUS_DIR}/include"
  fi
fi
if test -z "$MDS_PLUS_LIBDIR"; then
  AC_MSG_WARN(Unable to find MDSplus, the dataserver will not be able to access MDSplus data.)
else
  AC_DEFINE(HAVE_MDS_PLUS)
dnl Link with MDS_PLUS_IPLIB if you are using C++
  MDS_PLUS_IPLIB="-L$MDS_PLUS_LIBDIR -lMdsIpShr"
dnl Otherwise, you should link with MDS_PLUS_LIB
  MDS_PLUS_LIB="-L$MDS_PLUS_LIBDIR -lMdsLib"
  MDS_PLUS_SHLIBDIR='$(RPATH_FLAG)'$MDS_PLUS_LIBDIR
  MDS_PLUS_LIBDIR=-L$MDS_PLUS_LIBDIR
fi
AC_SUBST(MDS_PLUS_IPLIB)
AC_SUBST(MDS_PLUS_LIB)
AC_SUBST(MDS_PLUS_INCDIR)
dnl AC_SUBST(MDS_PLUS_LIBDIR)
dnl echo MDS_PLUS_SHLIBDIR = $MDS_PLUS_SHLIBDIR
AC_SUBST(MDS_PLUS_SHLIBDIR)
