dnl ######################################################################
dnl
dnl Find Xm and Xt (Motif) and set flags
dnl 
dnl Changed: JK, 2013-08-08 -- removed message that is depricated and changed
dnl                            to search for files (not programs - difference in 
dnl                            executable flag for programs)
dnl                         -- also add/change to predefined paths for include/library
dnl
dnl ######################################################################

dnl
dnl The X11 include directory
dnl

# commented out by JK, 2013-08-08
#AC_MSG_WARN([x11.m4 is obsolete.  Please use AC@&t@_PATH_X or AC@&t@_PATH_XTRA.])

AC_ARG_WITH(X11-include,
[  --with-X11-include=<x11-include-dir>  set location of the X11 include directory],
X11_INCDIR="$withval")

dnl If not known, check in typical directories
if test -n "$X11_INCDIR"; then
  X11_INCDIR_PATH=$X11_INCDIR
else
  X11_INCDIR_PATH=/usr/X11R6/include:/usr/X11/include:/usr/include:/usr/local/include:/loc/include:/usr/openwin/include:$PATH
fi

tmp_IFS=$IFS ; IFS=$PATH_SEPARATOR

for tmp_path in $X11_INCDIR_PATH; do
  if test -f "${tmp_path}/X11/Xlib.h"; then
    ABS_XLIB_H_PATH=${tmp_path}/X11/Xlib.h
    break
  fi
done

IFS=$tmp_IFS

if test -z "$ABS_XLIB_H_PATH"; then
  if test "$PACKAGE" = "xoopic"; then
     AC_MSG_ERROR(X11 includes not found in $X11_INCDIR_PATH.  Use --with-X11-include to set the location of X11/Xlib.h
  or configure without X11 using --with-XGmini )
  else
     AC_MSG_ERROR(X11 includes not found in $X11_INCDIR_PATH.  Use --with-X11-include to set the location of X11/Xlib.h.)
  fi
fi
X11_INCDIR1=`dirname $ABS_XLIB_H_PATH`
X11_INCDIR=`dirname $X11_INCDIR1`
AC_SUBST(X11_INCDIR)

dnl
dnl Find the X11 library
dnl

AC_ARG_WITH(X11_LIBDIR,
[  --with-X11_LIBDIR=<x11-lib-dir>      to set location of X11 library],
X11_LIBDIR="$withval")

if test -n "$X11_LIBDIR"; then
  X11_LIBDIR_PATH=$X11_LIBDIR
else
  X11_LIBDIR_PATH=/usr/lib:/usr/lib32:/usr/lib64:/usr/X11R6/lib:/usr/X11R6/lib64:/usr/local/lib:/usr/local/lib64:/lib:/lib64:/usr/X11/lib:/usr/X11/lib64:/usr/openwin/lib:/usr/dt/lib:$PATH
fi

tmp_IFS=$IFS ; IFS=$PATH_SEPARATOR

for tmp_path in $X11_LIBDIR_PATH; do
	for tmp_file in libX11.a libX11$SO; do
		if test -f "${tmp_path}/${tmp_file}"; then
			ABS_X11LIB_PATH=${tmp_path}/${tmp_file}
			break 2
		fi
	done
done

IFS=$tmp_IFS

if test -z "$ABS_X11LIB_PATH"; then
  AC_MSG_ERROR(Unable to find libX11.a or libX11$SO in $X11_LIBDIR_PATH.  Set the appropriate directory using --with-X11_LIBDIR)
fi
X11_LIB=-lX11
XT_LIB=-lXt
X11_LIBDIR=`dirname $ABS_X11LIB_PATH`
AC_SUBST(X11_LIB)
AC_SUBST(X11_LIBS)
AC_SUBST(XT_LIB)
AC_SUBST(X11_LIBDIR)


