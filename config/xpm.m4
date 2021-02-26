dnl ######################################################################
dnl
dnl find XPM and set flags
dnl
dnl ######################################################################


XPM_LIBDIR=""
XPM_LIB=-lXpm
AC_ARG_WITH(xpm,[  --with-xpm=<xpmlibdir>       set location of Xpm lib],
        XPM_LIBDIR="$withval")

if test -z "$XPM_LIBDIR"; then

dnl try to find the location of the library.
   AC_PATH_PROGS(XPM_LIBDIR_PATH,libXpm.a libXpm$SO,"",
/usr/X11R6/lib:/usr/openwin/lib:/usr/lib:/usr/lib64:/usr/local/lib:/usr/local/lib64:/usr/local/xgrafix/lib:/usr/local/grafix/lib64:/lib:/lib64:/usr/X11/lib:/usr/X11/lib64:$PATH)

   if test -z "$XPM_LIBDIR_PATH"; then
#      echo "ERROR: Cannot find libXpm."
      AC_MSG_WARN(Please compile and install XPM or use --with-xpm=XPMDIR to use XPM)
      XPM_LIB="" 
      XPM_LIBDIR=""
   else
      XPM_LIBDIR="`dirname $XPM_LIBDIR_PATH`"
   fi

fi
#if test -z "$XPM_LIBDIR"; then
dnl This test is not correct but is part of the original coding.
#   AC_CHECK_LIB(Xpm,xpmSetInfo,
#   AC_MSG_RESULT($XPM_LIBDIR),
#   (echo "Cant find XPM.  please compile and install XPM, or use --with-xpm=XPMDIR";exit 1))
#fi

AC_SUBST(XPM_LIBDIR)
AC_SUBST(XPM_LIB)

