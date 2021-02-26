dnl ######################################################################
dnl
dnl File:       linlibs.m4
dnl
dnl Purpose:    Determine library flags for lapack and blas
dnl
dnl Version: $Id: linlibs.m4 2305 2007-06-24 11:46:12Z cary $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################

dnl AC_LANG_SAVE
dnl AC_LANG_FORTRAN77

# Determine absolute path of linear algebra libraries
AC_ARG_WITH(lapack-libdir, AC_HELP_STRING([--with-lapack-libdir],
        [lib directory for LAPACK]), LAPACK_LIBDIR="$withval", LAPACK_LIBDIR="")

dnl Default - no use to trying again
ac_cv_try_lapack=no
ac_cv_have_lapack=no

case "$host" in

  *-apple-darwin*)
    # LAPACK_LIBS='-framework Accelerate -llapack -lblas -lgcc_s.10.4 -lSystemStubs -lSystem'
    # LAPACK_LIBS='-framework Accelerate -lgcc_s.10.4 -lSystemStubs -lSystem'
    LAPACK_LIBS='-framework Accelerate'
dnl Ensure later testing
    ac_cv_try_lapack=yes
    ;;

  *)
dnl JRC: 23Jul06: this may be broken
    LAPACK_LIBS="-llapack -lblas"
    if test -n "$FC"; then
      LAPACK_LIBS="$LAPACK_LIBS $FC_COMPAT_LIBS"
    elif test -n "$F77"; then
      LAPACK_LIBS="$LAPACK_LIBS $F77_COMPAT_LIBS"
    else
      AC_MSG_WARN(No Fortran found on your system. Linkin lapack may not work.)
    fi

    if test -n "$LAPACK_LIBDIR"; then
      LAPACK_LIBS="-L$LAPACK_LIBDIR $LAPACK_LIBS"
    fi
dnl Try the link without a libdir first
    if test -n "$SERIALCC"; then
      CCSAV=$CC
      CC=$SERIALCC
    fi

    LIBS_SAVE=$LIBS
    LIBS="$LIBS_SAVE $LAPACK_LIBS"
    AC_LINK_IFELSE([ AC_LANG_PROGRAM([[#include <stdio.h>]], [[return 0;]]) ],
	ac_cv_have_lapack=yes, ac_cv_have_lapack=no)
    LIBS=$LIBS_SAVE
    if test -n "$SERIALCC"; then
      CC=$CCSAV
    fi
dnl If did not link, try adding in the libdir
    if test $ac_cv_have_lapack = no -a -z "$LAPACK_LIBDIR"; then
      LAPACK_LIBPATH=$abs_top_builddir/../txmodules/lapack/lib:$HOME/$UNIXFLAVOR/lapack/lib:/usr/local/lapack/lib:/usr/local/lib:/usr/lib
      AC_PATH_PROGS(ABS_LAPACK_LIB, liblapack.a liblapack$SO liblapack.dylib, "", $LAPACK_LIBPATH)
      if test -n "$ABS_LAPACK_LIB"; then
        LAPACK_LIBDIR=`dirname $ABS_LAPACK_LIB`
        LAPACK_LIBS="-L$LAPACK_LIBDIR $LAPACK_LIBS"
        ac_cv_try_lapack=yes
      fi
    fi
    ;;

esac

dnl Either has not been tried, or parameters changed, so try again
if test $ac_cv_have_lapack = no -a $ac_cv_try_lapack = yes; then
  if test -n "$SERIALCC"; then
    CCSAV=$CC
    CC=$SERIALCC
  fi
  LIBS_SAVE=$LIBS
  LIBS="$LIBS_SAVE $LAPACK_LIBS"
  AC_LINK_IFELSE([ AC_LANG_PROGRAM([[#include <stdio.h>]], [[return 0;]]) ],
	ac_cv_have_lapack=yes, ac_cv_have_lapack=no)
  LIBS=$LIBS_SAVE
  if test -n "$SERIALCC"; then
    CC=$CCSAV
  fi
fi

if test $ac_cv_have_lapack = no; then
  AC_MSG_WARN(Could not link with the lapack library.  Use --with-lapack-libdir to set its directory.)
  unset LAPACK_LIBS
fi

# Put in the variables
AC_SUBST(ABS_LAPACK_LIB)
AC_SUBST(LAPACK_LIBDIR)
AC_SUBST(LAPACK_LIBS)

dnl AC_LANG_RESTORE

dnl ######################################################################
dnl
dnl Write to summary file if defined
dnl
dnl ######################################################################
if test -n "$config_summary_file"; then
  if test "$ac_cv_have_lapack" = yes; then
    echo                                    >> $config_summary_file
    echo "Using LAPACK with"                >> $config_summary_file
    echo "  LAPACK_LIBS:   $LAPACK_LIBS"    >> $config_summary_file
  else
    echo                                    >> $config_summary_file
    echo "NOT using LAPACK"                 >> $config_summary_file
  fi
fi
