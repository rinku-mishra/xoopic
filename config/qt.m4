dnl ######################################################################
dnl
dnl File:	qt.m4
dnl
dnl Purpose:	Determine the locations of QT includes and libraries
dnl
dnl Version:	$Id: qt.m4 2018 2006-08-24 16:30:18Z rmtrines $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################

AC_CHECKING(Qt installation)

#
# Determine whether QT needed
#
AC_MSG_CHECKING(for enabling Qt GUI)
AC_ARG_ENABLE(gui, [  --disable-gui   disable Qt GUI], QT_GUI=no, QT_GUI=yes)
#echo gui enabling determined
#echo QT_GUI = $QT_GUI

if test "$QT_GUI" = "no"; then

    AC_MSG_RESULT(no)
    echo Qt GUI not enabled
    DEFINE_NOX="-DNOX"
    QT_DIR="."
    QT_INCDIR="."
    QT_LIBDIR="."
    QT_LIB=""
    QT_LIBFLAG=""
    QT_GLLIBS=""
    QT_GUILIBS=""
    JPEG_LIBDIR="."
    JPEG_LIB=""
    X11_LIBDIR="."
    X11_LIBS=""

else
    AC_MSG_RESULT(yes)
    echo Qt GUI enabled
    DEFINE_NOX=""

dnl ######################################################################
dnl
dnl Allow the user to specify an overall qt directory.  If specified,
dnl we look for include and lib under this.
dnl
dnl ######################################################################

  AC_ARG_WITH(qt,[  --with-qt-dir=<qtdir>       set the location of the Qt distribution], QT_DIR="$withval")

dnl ######################################################################
dnl
dnl Find qt includes - looking in include location if present,
dnl otherwise in dir/include if present, otherwise in default locations.
dnl
dnl ######################################################################


  AC_ARG_WITH(qt-incdir,[  --with-qt-incdir=<location of qt includes> ], QT_INCDIR="$withval", QT_INCDIR="")
  if test -n "$QT_INCDIR"; then
    QT_INCPATH=$QT_INCDIR
  elif test -n "$QT_DIR"; then
    QT_INCPATH=$QT_DIR/include
  else
    QT_INCPATH=$HOME/aix5.1/qt/include:$HOME/qt/include:/usr/local/qt/include:/loc/qt/include:/local/qt/include:/usr/lib/qt3/include
  fi

  AC_PATH_PROGS(QAPPLICATION_H, qapplication.h, "", $QT_INCPATH)
  if test -z "$QAPPLICATION_H"; then
    AC_MSG_ERROR(Cannot find qapplication.h in $QT_INCPATH!  Please compile and install Qt or use --with-qt-incdir=QT_INCDIR)
  fi
  QT_INCDIR=`dirname $QAPPLICATION_H`
  if test -z "$QT_DIR"; then
    QT_DIR=`dirname $QT_INCDIR`
  fi

dnl ######################################################################
dnl
dnl Determine whether threaded
dnl
dnl ######################################################################

  # QT_THREAD_FLAG=yes
  AC_ARG_ENABLE(thread,[  --disable-thread   disable threading], QT_THREAD_FLAG=no, QT_THREAD_FLAG=yes)

  case "$host" in

    *-apple-darwin*)
    QT_THREAD_FLAG_LIBTOOLBUG=yes
   ;;
    *)
    QT_THREAD_FLAG_LIBTOOLBUG=no
   ;;
  esac

  echo "QT_THREAD_FLAG = $QT_THREAD_FLAG"
  echo "QT_THREAD_FLAG_LIBTOOLBUG = $QT_THREAD_FLAG_LIBTOOLBUG"
  if test "$QT_THREAD_FLAG" = "yes"; then
    if test "$QT_THREAD_FLAG_LIBTOOLBUG" = "yes"; then
      QT_LIBNAME=libqt dnl this is the mac bug, link your libqt-mt.dylib to libqt.dylib

      QT_LIB=-lqt
      echo mac bug here.  link your libqt-mt.dylib to libqt.dylib
    else
      QT_LIBNAME=libqt-mt
      QT_LIB=-lqt-mt
    fi
      echo Threading allowed.
  else
    QT_LIBNAME=libqt
    QT_LIB=-lqt
  fi
  AC_DEFINE(QT_THREAD_SUPPORT)

dnl ######################################################################
dnl
dnl Find qt libraries - shared, then static
dnl
dnl ######################################################################

  AC_ARG_WITH(qt-libdir,[  --with-qt-libdir=<location of qt library> ], QT_LIBDIR="$withval",QT_LIBDIR="")
  if test -n "$QT_LIBDIR"; then
    QT_LIBPATH=$QT_LIBDIR
  else
    QT_LIBPATH=$QT_DIR/lib/$COMPDIR:$QT_DIR/lib
  fi

  AC_PATH_PROGS(ABSQT_LIB, ${QT_LIBNAME}${SO} $QT_LIBNAME.a, "", $QT_LIBPATH)
  echo ABSQT_LIB = $ABSQT_LIB
  dnl if test -z "$ABSQT_LIB"; then
    dnl AC_PATH_PROGS(ABSQT_LIB, $QT_LIBNAME.a, "", $QT_LIBPATH)
  dnl fi
  if test -z "$ABSQT_LIB"; then
    AC_MSG_ERROR(Cannot find libqt in $QT_LIBPATH!  Please compile and install Qt or use --with-qt-libdir=)
  fi
  QT_LIBDIR=`dirname $ABSQT_LIB`

dnl ######################################################################
dnl
dnl Check qt version number
dnl
dnl ######################################################################

  AC_MSG_CHECKING(Qt version)
  INC_FILE=$QT_INCDIR/qglobal.h
  QT_VERSION=`grep '#define QT_VERSION_STR' $INC_FILE | sed 's/^.*QT_VERSION_STR//' | sed 's/\/.*$//' | tr -d [:space:]\"`
  AC_EGREP_CPP(good_qt_version,
[#include <$INC_FILE>
#if (( QT_VERSION >= 0x30000 ))
good_qt_version
#endif], is_good_qt_version=yes,is_good_qt_version=no)
  AC_MSG_RESULT($is_good_qt_version)
  if test "$is_good_qt_version" = "no" ; then
    AC_MSG_WARN(Qt version needs to be >= 3.0 for the TxAttributeSet editor in txqeditor to function properly.  Current Qt version is $QT_VERSION.  Please upgrade to Qt 3.x!)
  fi
dnl OopicPro uses the QSplashScreen class, which only appeared in Qt 3.2, so
dnl we need to check for that separately.
  if test "$PACKAGE" = "oopicpro"; then
  AC_MSG_CHECKING(Qt version for OopicPro)
  AC_EGREP_CPP(good_oopicpro_qt_version,
[#include <$INC_FILE>
#if (( QT_VERSION >= 0x30200 ))
good_oopicpro_qt_version
#endif], is_good_oopicpro_qt_version=yes,is_good_oopicpro_qt_version=no)
  AC_MSG_RESULT($is_good_oopicpro_qt_version)
  if test "$is_good_oopicpro_qt_version" = "no" ; then
    AC_MSG_WARN(OopicPro needs the Qt version to be >= 3.2 for the QSplashScreen to function properly.  Current Qt version is $QT_VERSION.  Please upgrade to Qt 3.2 or higher!)
  fi
  fi
  echo QT_VERSION = $QT_VERSION

dnl ######################################################################
dnl
dnl Get jpeg libraries
dnl
dnl ######################################################################

  AC_CHECKING(libjpeg installation)
  AC_ARG_WITH(jpeg,[  --with-jpeg=<jpeglibdir>       set location of jpeg lib],
        JPEG_LIBDIR="$withval")

  if test -z "$JPEG_LIBDIR"; then
    JPEG_LIBPATH="/usr/lib64:/sw/lib:/usr/openwin:/usr/openwin/lib:/usr/lib:/usr/X11R6/lib:/lib:/usr/local/lib"
else
    JPEG_LIBPATH="$JPEG_LIBDIR"
  fi

  AC_PATH_PROGS(JPEG_LIBDIR_PATH,libjpeg$SO libjpeg.a,"",$JPEG_LIBPATH)
  if test -z "$JPEG_LIBDIR_PATH"; then
    AC_MSG_WARN(Cannot find libjpeg in $JPEG_LIBPATH!  JPEG output will not be enabled!)
    JPEG_LIBDIR=""
    JPEG_LIB=""
  else
    JPEG_LIBDIR=`dirname $JPEG_LIBDIR_PATH`
    JPEG_LIB="-ljpeg"
  fi

dnl ######################################################################
dnl
dnl Get png libraries
dnl
dnl ######################################################################

  AC_CHECKING(libpng installation)
  AC_ARG_WITH(png,[  --with-png=<pnglibdir>       set location of png lib],
        PNG_LIBDIR="$withval")

  if test -z "$PNG_LIBDIR"; then
    PNG_LIBPATH="/sw/lib:/usr/lib64:/usr/lib"
else
    PNG_LIBPATH="$PNG_LIBDIR"
  fi

  AC_PATH_PROGS(PNG_LIBDIR_PATH,libpng$SO libpng.a,"",$PNG_LIBPATH)
  if test -z "$PNG_LIBDIR_PATH"; then
    AC_MSG_WARN(Cannot find libpng in $PNG_LIBPATH!  PNG output will not be enabled!)
    PNG_LIBDIR=""
    PNG_LIB=""
  else
    PNG_LIBDIR=`dirname $PNG_LIBDIR_PATH`
    PNG_LIB="-lpng"
  fi

dnl ######################################################################
dnl
dnl Determine GUI libraries
dnl
dnl ######################################################################
# Determine whether X is needed
  case "$host" in

    *-apple-darwin*)
      QT_NEEDS_X11=no
      QT_GLLIBS="-framework OpenGL -framework AGL"
      QT_GUILIBS="-framework QuickTime -framework Carbon"
	dnl if libtool bug (lqt-mt barfs on mac because of '-')
      ;;

    *)
      dnl QT_NEEDS_X11=yes
      AC_PATH_X
      AC_PATH_XTRA
      dnl builtin(include, config/x11.m4)
      ;;

  esac

  if test "$QT_NEEDS_X11" = "yes"; then

    XMU_LIB="-lXmu"
    AC_PATH_PROGS(ABS_XMULIB_PATH, libXmu.a libXmu$SO, "", $X11_LIBDIR_PATH)
    if test -z "$ABS_XMULIB_PATH"; then
      XMU_LIB=""
    fi
    # echo "XMU_LIB: " $XMU_LIB

    XEXT_LIB="-lXext"
    AC_PATH_PROGS(ABS_XEXTLIB_PATH, libXext.a libXext$SO, "", $X11_LIBDIR_PATH)
    if test -z "$ABS_XEXTLIB_PATH"; then
      XEXT_LIB=""
    fi
    # echo "XEXT_LIB: " $XEXT_LIB

    XICE_LIB="-lICE"
    AC_PATH_PROGS(ABS_XICELIB_PATH, libICE.a libICE$SO, "", $X11_LIBDIR_PATH)
    if test -z "$ABS_XICELIB_PATH"; then
      XICE_LIB=""
    fi

    XSM_LIB="-lSM"
    AC_PATH_PROGS(ABS_XSMLIB_PATH, libSM.a libSM$SO, "", $X11_LIBDIR_PATH)
    if test -z "$ABS_XSMLIB_PATH"; then
      XSM_LIB=""
    fi

    X11_LIBS="$XSM_LIB $XICE_LIB $XMU_LIB $XEXT_LIB $XT_LIB $X11_LIB "
    # echo "X11_LIBS: " $X11_LIBS
    case "$host" in

      alpha*-dec-osf*)
        ;;

      *-hp-hpux*)
        ;;

      *-ibm-aix*)
        ;;

      *-linux*)
       GLIDE_LIB="-lglide3"
       AC_PATH_PROGS(ABS_GLIDELIB_PATH, libglide3.a, "", /usr/lib)
       if test -z "$ABS_GLIDELIB_PATH"; then
         GLIDE_LIB=""
       fi
       XPM_LIB="-lXpm"
       AC_PATH_PROGS(ABS_XPMLIB_PATH, libXpm.a libXpm$SO, "", $X11_LIBDIR_PATH)
       if test -z "$ABS_XPMLIB_PATH"; then
         XPM_LIB=""
       fi
       XFT_LIB="-lXft"
       AC_PATH_PROGS(ABS_XFTLIB_PATH, libXft.a libXft$SO, "", $X11_LIBDIR_PATH)
       if test -z "$ABS_XFTLIB_PATH"; then
         XFT_LIB=""
       fi
       XFREETYPE_LIB="-lfreetype"
       AC_PATH_PROGS(ABS_XFREETYPELIB_PATH, libfreetype.a libfreetype$SO, "", /usr/lib)
       if test -z "$ABS_XFREETYPELIB_PATH"; then
         XFREETYPE_LIB=""
       fi
       XRENDER_LIB="-lXrender"
       AC_PATH_PROGS(ABS_XRENDERLIB_PATH, libXrender.a libXrender$SO, "", $X11_LIBDIR_PATH)
       if test -z "$ABS_XRENDERLIB_PATH"; then
         XRENDER_LIB=""
       fi
       XINERAMA_LIB="-lXinerama"
       AC_PATH_PROGS(ABS_XINERAMALIB_PATH, libXinerama.a libXinerama$SO, "", $X11_LIBDIR_PATH)
       if test -z "$ABS_XINERAMALIB_PATH"; then
         XINERAMA_LIB=""
       fi
       X11_LIBS="$GLIDE_LIB $XSM_LIB $XICE_LIB $XPM_LIB $XFT_LIB $XFREETYPE_LIB $XRENDER_LIB $XINERAMA_LIB $XMU_LIB $XEXT_LIB $XT_LIB $X11_LIB "
        ;;

      *-sgi-irix6*)
        ;;

      *-*-solaris2.5* | *-*-solaris2.6*)
        X11_LIBS="$XMU_LIB $XEXT_LIB $XT_LIB -lX11 "
        if test -n "$QT_MT_LIBDIR_A_PATH"; then
          X11_LIBS="$XMU_LIB $XEXT_LIB $XT_LIB -lX11 -lthread -lresolv -lsocket -lnsl"
        fi
        ;;

      *-*-solaris2.7* |  *-*-solaris2.8*)
        X11_LIBS="$XICE_LIB $XSM_LIB $XMU_LIB $XEXT_LIB $XT_LIB -lX11"
        if test -n "$QT_MT_LIBDIR_A_PATH"; then
          X11_LIBS="$XICE_LIB $XSM_LIB $XMU_LIB $XEXT_LIB $XT_LIB -lX11 -lthread -lresolv -lsocket -lnsl"
        fi
        ;;

      *)
  #    AC_MSG_ERROR(Libraries unknown for host $host.)
        ;;

    esac

    QT_GUILIBS="-L$X11_LIBDIR -lXcursor -lXrandr $X11_LIBS"

  fi

fi	# End of QT_GUI

# Put into cache
AC_SUBST(DEFINE_NOX)
AC_SUBST(QT_DIR)
AC_SUBST(QT_INCDIR)
AC_SUBST(QT_LIBDIR)
AC_SUBST(QT_LIB)
AC_SUBST(ABSQT_LIB)
AC_SUBST(QT_GLLIBS)
AC_SUBST(QT_GUILIBS)
AC_SUBST(JPEG_LIBDIR)
AC_SUBST(JPEG_LIB)
AC_SUBST(PNG_LIBDIR)
AC_SUBST(PNG_LIB)
AC_SUBST(X11_LIBDIR)
AC_SUBST(X11_LIBS)

