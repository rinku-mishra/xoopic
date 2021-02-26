dnl ######################################################################
dnl
dnl File:	wx.m4
dnl
dnl Purpose:	Determine the locations of wxWindows includes and libraries
dnl
dnl Version: 	$Id: wx.m4 896 2003-02-11 03:30:10Z cary $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Find wx-config
dnl
dnl ######################################################################

WXPATH=/usr/local/wx/bin:$PATH
AC_PATH_PROGS(WXCONFIG, wx-config, "", $WXPATH)
# echo WXCONFIG = $WXCONFIG
if test -z "$WXCONFIG"; then
  echo foo
fi

dnl ######################################################################
dnl
dnl add in flags from wx
dnl
dnl ######################################################################

CXXFLAGS="$CXXFLAGS "`$WXCONFIG --cxxflags`
# echo CXXFLAGS = $CXXFLAGS
WX_LIBS=`$WXCONFIG --libs`
AC_SUBST(WX_LIBS)
WX_GLLIBS=`$WXCONFIG --gl-libs`
AC_SUBST(WX_GLLIBS)
REZ=`$WXCONFIG --rezflags`
if test -z $REZ; then 
  REZ=:
fi
AC_SUBST(REZ)

