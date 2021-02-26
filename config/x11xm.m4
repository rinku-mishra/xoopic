dnl ######################################################################
dnl
dnl Find Xm and Xt (Motif) and set flags
dnl
dnl ######################################################################

builtin(include,config/x11.m4)

dnl
dnl Find the Xm include directory
dnl

AC_ARG_WITH(XM-include,
[  --with-XM-include=<xm-include-dir>  set location of the XM/MOTIF include directory],
XM_INCDIR="$withval")

dnl If not known, check in typical directories
if test -n "$XM_INCDIR"; then
  XM_INCDIR_PATH=$XM_INCDIR
else
  XM_INCDIR_PATH=/usr/X11R6/include:/usr/X11/include:/usr/include:/usr/local/include:/loc/include:/usr/dt/include:/usr/openwin/include:$PATH
fi
AC_PATH_PROGS(ABS_SCROLLBAR_H_PATH, Xm/ScrollBar.h, "", $XM_INCDIR_PATH)
if test -z "$ABS_SCROLLBAR_H_PATH"; then
  AC_MSG_ERROR(Xm/Motif includes not found in $XM_INCDIR_PATH.  Use --with-XM-include to set the location of Xm/ScrollBar.h.)
fi
XM_INCDIR1=`dirname $ABS_SCROLLBAR_H_PATH`
XM_INCDIR=`dirname $XM_INCDIR1`
AC_SUBST(XM_INCDIR)

dnl
dnl Find the Xm library
dnl

AC_ARG_WITH(XM_LIBDIR,
[  --with-XM_LIBDIR=<xm-lib-dir>      to set location of Xm/Motif library],
XM_LIBDIR="$withval")

# Try to guess the Xm/Motif library directory
if test -n "$XM_LIBDIR"; then
  XM_LIBDIR_PATH=$XM_LIBDIR
else
  XM_LIBDIR_PATH=/usr/lib32:/usr/X11R6/lib:/usr/openwin/lib:/usr/dt/lib:/opt/SUNWmotif/lib:/usr/lib:/usr/local/lib:/lib:/usr/X11/lib:$PATH
fi
AC_PATH_PROGS(ABS_XMLIB_PATH, libXm.a libXm$SO libXm.sl, "", $XM_LIBDIR_PATH)
if test -z "$ABS_XMLIB_PATH"; then
  AC_MSG_ERROR(Unable to find libXm.a or libXm$SO in $XM_LIBDIR_PATH.  Set the appropriate directory using --with-XM_LIBDIR)
fi
XM_LIB=-lXm
XM_LIBDIR=`dirname $ABS_XMLIB_PATH`
AC_SUBST(XM_LIB)
AC_SUBST(XM_LIBDIR)

