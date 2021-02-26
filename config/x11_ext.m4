dnl ######################################################################
dnl
dnl This file has some extra checks related to X11.
dnl 
dnl ######################################################################


dnl ######################################################################
dnl
dnl HOW TO USE THESE MACROS
dnl -----------------------
dnl
dnl You need to put this line into your Makefile.am:
dnl ACLOCAL_AMFLAGS = -I config
dnl
dnl Run "autoreconf -if" to make sure this file is being processed.
dnl
dnl Then in your configure.in you can use this macro just this way:
dnl
dnl X11_CHECK
dnl
dnl ######################################################################


dnl ######################################################################
dnl
dnl This macro performs a basic X11 check
dnl 
dnl ######################################################################

AC_DEFUN([X11_CHECK],
[

dnl
dnl The X11 binary path
dnl

AC_ARG_WITH(x-bin,
            [  --with-x-bin=<location of X11 executables>],
            [X11_BINDIR=$withval])

AC_ARG_WITH(x-directory,
            [  --with-x-directory=DIR	In this directory, the bin, include and
				lib subdirectories should exist.  The location
				of X executables becomes DIR/bin,
				of X includes becomes DIR/include,
				of X libraries becomes DIR/lib.
				Options	--x-includes, --x-libraries
				and --with-x-bin can override these
				locations.],
            [X11_DIR=$withval])

if test "${X11_DIR}" ; then
	if test -z "${X11_BINDIR}" ; then
		X11_BINDIR="${X11_DIR}/bin"
	fi

	if test "x$x_includes" = xNONE ; then
		x_includes="${X11_DIR}/include"
	fi

	if test "x$x_libraries" = xNONE ; then
		x_libraries="${X11_DIR}/lib"
	fi
fi

AC_PATH_X
AC_PATH_XTRA

# Find X11_BINDIR if none was provided
if test -z "$X11_BINDIR" ; then
	X11_BINDIR_PATH=/usr/X11R6/bin:/usr/X11/bin:/usr/bin:/usr/local/bin:/loc/bin:/usr/openwin/bin:$PATH

	if test "${x_includes}" ; then
		X11_BINDIR_PATH=`dirname ${x_includes}`/bin:${X11_BINDIR_PATH}
	fi

	if test "${x_libraries}" ; then
		X11_BINDIR_PATH=`dirname ${x_libraries}`/bin:${X11_BINDIR_PATH}
	fi

	AC_PATH_PROGS(ABS_X11BIN_PATH, xset, , $X11_BINDIR_PATH)
	if test -z "$ABS_X11BIN_PATH"; then
		AC_MSG_WARN(X11 executables not found in $X11_BIN_PATH.  You can use --with-x-bin to set the location of X11 executables.)
	fi

	if test "${ABS_X11BIN_PATH}" ; then
		X11_BINDIR=`dirname ${ABS_X11BIN_PATH}`
	fi
fi

])


dnl ######################################################################
dnl
dnl This macro checks for X11 extensions
dnl 
dnl ######################################################################

AC_DEFUN([X11_EXTENSIONS_CHECK],
[
	AC_REQUIRE([X11_CHECK])

	X11_EXTENSIONS=true

	saved_CFLAGS="${CFLAGS}"
	saved_LDFLAGS="${LDFLAGS}"
	saved_LIBS="${LIBS}"

	if test "${x_includes}"; then
		CFLAGS="${CFLAGS} -I${x_includes}"
	fi

	AC_CHECK_HEADERS(X11/extensions/Xcomposite.h,,X11_EXTENSIONS=false,
	[[#include<X11/Xlib.h>
	]])

	AC_CHECK_HEADERS(X11/extensions/Xdamage.h,,X11_EXTENSIONS=false,
	[[#include<X11/Xlib.h>
	]])

	AC_CHECK_HEADERS(X11/extensions/Xrender.h,,X11_EXTENSIONS=false,
	[[#include<X11/Xlib.h>
	]])


	if test "${x_libraries}"; then
		LDFLAGS="${LDFLAGS} -L${x_libraries} -lX11"
	fi

	AC_CHECK_LIB(Xcomposite,XCompositeQueryExtension,,X11_EXTENSIONS=false)
	AC_CHECK_LIB(Xdamage,XDamageQueryExtension,,X11_EXTENSIONS=false)
	AC_CHECK_LIB(Xrender,XRenderQueryExtension,,X11_EXTENSIONS=false)
	AC_CHECK_LIB(Xfixes,XFixesQueryExtension,,X11_EXTENSIONS=false)

	if test "${X11_EXTENSIONS}" = "false"; then
		AC_MSG_WARN([X11 extensions not found.])
	fi

	CFLAGS="${saved_CFLAGS}"
	LDFLAGS="${saved_LDFLAGS}"
	LIBS="${saved_LIBS}"
])


dnl ######################################################################
dnl
dnl This macro checks for Xvfb
dnl 
dnl ######################################################################

AC_DEFUN([XVFB_CHECK],
[
	AC_ARG_VAR(XVFB, [executable of Xvfb])
	AC_MSG_CHECKING([for executable of Xvfb])

	AC_ARG_WITH(Xvfb,
            [  --with-Xvfb=<executable of Xvfb>],
            [XVFB=$withval])

	if test -z "${XVFB}" ; then
		AC_PATH_PROG(XVFB, Xvfb, "", ${X11_BINDIR}:${PATH})
	fi

	if test -z "$XVFB" ; then
		AC_MSG_WARN([No Xvfb found.  Use the --with-Xvfb option.])
	else
		AC_MSG_RESULT([${XVFB}])
	fi
])
