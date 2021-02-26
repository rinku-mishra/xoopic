dnl ######################################################################
dnl
dnl File:       blas.m4
dnl
dnl Purpose:    Determine if BLAS is installed.
dnl
dnl Variables defined in AC_SUBST
dnl   BLAS_LIBS
dnl   BLAS_LIBDIR
dnl
dnl Version:    $Id: blas.m4 2190 2007-02-20 22:26:39Z johan $
dnl
dnl Copyright Tech-X Corporation, 2001-2005.  Redistribution allowed
dnl provided this copyright statement remains intact.
dnl
dnl ######################################################################

AC_MSG_CHECKING(for BLAS libraries)

if test -n "$BLAS_LIBS"; then SAVE_BLAS_LIBS="$BLAS_LIBS"; fi
if test -n "$BLAS_LIBDIR"; then SAVE_BLAS_LIBDIR="$BLAS_LIBDIR"; fi

dnl ----------------------------------------------------------------------
dnl This section is trying to find BLAS_LIBS, absolute path to blas
dnl ----------------------------------------------------------------------

dnl Allow additional places to look for BLAS
AC_ARG_WITH(BLAS_LIBS, --with-BLAS_LIBS=<library>  Full path to BLAS library,
            [BLAS_LIBS="${withval}"])

dnl Check whether an absolute path
absp=""
if test -n "$BLAS_LIBS"; then
  absp=`echo "$BLAS_LIBS" | grep ^/`
fi
if test -n "$absp"; then
  exists=`ls $BLAS_LIBS`
  if test -z "$exists"; then
    AC_MSG_ERROR($BLAS_LIBS does not exist.)
  fi
dnl Not an absolute path, so must find
else
  dnl Search path
  BLAS_PATH=$SAVE_BLAS_LIBDIR:$SAVE_BLAS_LIBDIR/lib:$HOME/lib:$HOME/local/lib:/usr/local/lib:/opt/lib
dnl Allow additional places to look for BLAS
  AC_ARG_WITH(BLAS_LIBDIR,
	[  --with-BLAS_LIBDIR=<dir>	Prefix directory for BLAS. 
	Default is to search ${BLAS_PATH}],
	[BLAS_LIBDIR="${withval}"])

  if test -n "$BLAS_LIBDIR"; then
      BLAS_PATH=$BLAS_LIBDIR:$BLAS_PATH
  fi

dnl Use TX_PATH_FILE to find BLAS library
dnl TX_PATH_PROG doesn't work w/ autconf 2.60
  TX_PATH_FILE(BLAS_LIBS, libblas.a,"",$BLAS_PATH)
  if test -z "$BLAS_LIBS"; then
    TX_PATH_FILE(BLAS_LIBS, libatlas.a,"",$BLAS_PATH)
    if test -z "$BLAS_LIBS"; then
      AC_MSG_ERROR(Unable to find the BLAS library in $BLAS_PATH.  Use BLAS_LIBDIR to set its directory.)
    else
      BLAS_LIBDIR=`dirname $BLAS_LIBS`
      BLAS_LIB_FLAGS="-L$BLAS_LIBDIR -lcblas -lf77blas -latlas"
    fi
  else
    BLAS_LIBDIR=`dirname $BLAS_LIBS`
    BLAS_LIB_FLAGS="$-LBLAS_LIBDIR -lblas"
  fi
fi

dnl ----------------------------------------------------------------------
dnl export 
dnl ----------------------------------------------------------------------
dnl if test -n "$SAVE_BLAS_LIBS"; then BLAS_LIBS="$SAVE_BLAS_LIBS"; fi

AC_SUBST(BLAS_LIBS)
AC_SUBST(BLAS_LIBDIR)
AC_SUBST(BLAS_LIB_FLAGS)

dnl ----------------------------------------------------------------------
dnl
dnl    Write to summary file if defined
dnl
dnl ----------------------------------------------------------------------
if test -n "$config_summary_file"; then
	if test -z "$BLAS_LIBS" ; then
	  echo " NOT using BLAS"                     >> $config_summary_file
	  echo " Set with BLAS_LIBS=<BLAS_LIBS>"     >> $config_summary_file
	else
	  echo " Using BLAS"                         >> $config_summary_file
        echo "   BLAS_LIBDIR: $BLAS_LIBDIR"        >> $config_summary_file
        echo "   BLAS_LIBS:   $BLAS_LIBS"          >> $config_summary_file
	fi
      echo                                         >> $config_summary_file
fi
