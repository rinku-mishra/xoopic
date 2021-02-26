dnl ######################################################################
dnl
dnl File:       lapack.m4
dnl
dnl Purpose:    Determine if LAPACK is installed.
dnl
dnl Variables defined in AC_SUBST
dnl   LAPACK_LIBS
dnl   LAPACK_LIBDIR
dnl
dnl Version:    $Id: lapack.m4 2187 2007-02-20 05:28:02Z johan $
dnl
dnl Copyright Tech-X Corporation, 2001-2005.  Redistribution allowed
dnl provided this copyright statement remains intact.
dnl
dnl ######################################################################

AC_MSG_CHECKING(for LAPACK libraries)

dnl ######################################################################
dnl  First check for weird variables that the supercomputing centers define 
dnl  Then check for over rides from the commandline.
dnl ######################################################################
dnl
dnl jacquard uses ACML 
if test -n "$ACML"; then SAVE_LAPACK_LIBS="$ACML"; fi
dnl jaguar uses ACML_DIR
if test -n "$ACML_DIR"; then SAVE_LAPACK_LIBS="$ACML_DIR/lib/libacml.a"; fi
dnl seaborg and bassi uses LAPACK
if test -n "$LAPACK"; then SAVE_LAPACK_LIBS="$LAPACK"; fi

dnl Command-line override
if test -n "$LAPACK_LIBS"; then SAVE_LAPACK_LIBS="$LAPACK_LIBS"; fi
if test -n "$LAPACK_LIBDIR"; then SAVE_LAPACK_LIBDIR="$LAPACK_LIBDIR"; fi

dnl ----------------------------------------------------------------------
dnl This section is trying to find LAPACK_LIBS, absolute path to lapack
dnl ----------------------------------------------------------------------

dnl Allow additional places to look for lapack
AC_ARG_WITH(LAPACK_LIBS, --with-LAPACK_LIBS=<library>  Full path to LAPACK library,
            [LAPACK_LIBS="${withval}"])

dnl Check whether an absolute path
absp=""
if test -n "$LAPACK_LIBS"; then
  absp=`echo "$LAPACK_LIBS" | grep ^/`
fi

if test -n "$absp"; then
exists=`ls $LAPACK_LIBS`
  if test -z "$exists"; then
  AC_MSG_ERROR($LAPACK_LIBS does not exist.)
  fi
dnl Not an absolute path, so must find
else
dnl Search path
  LAPACK_PATH=$SAVE_LAPACK_LIBDIR:$SAVE_LAPACK_LIBDIR/lib:$HOME/lib:$HOME/local/lib:/usr/local/lib:/opt/lib

dnl Allow additional places to look for LAPACK
  AC_ARG_WITH(LAPACK_LIBDIR,
  [  --with-LAPACK_LIBDIR=<dir>	Prefix directory for LAPACK.  Default is to search ${LAPACK_PATH}],
  [LAPACK_LIBDIR="${withval}"])

  if test -n "$LAPACK_LIBDIR"; then
    LAPACK_PATH=$LAPACK_LIBDIR:$LAPACK_PATH
  fi

dnl Use TX_PATH_FILE to find LAPACK library
dnl TX_PATH_PROG doesn't work w/ autconf 2.60
  TX_PATH_FILE(LAPACK_LIBS, liblapack.a,"", $LAPACK_PATH)
  if test -z "$LAPACK_LIBS"; then
    AC_MSG_ERROR(Unable to find the LAPACK library in $LAPACK_PATH.  Use LAPACK_LIBDIR to set its directory.)
  else
    LAPACK_LIBDIR=`dirname $LAPACK_LIBS`
    LAPACK_LIB_FLAGS="-L$LAPACK_LIBDIR -llapack"
  fi
fi
dnl ----------------------------------------------------------------------
dnl export 
dnl ----------------------------------------------------------------------
dnl if test -n "$SAVE_LAPACK_LIBS"; then LAPACK_LIBS="$SAVE_LAPACK_LIBS"; fi

AC_SUBST(LAPACK_LIBS)
AC_SUBST(LAPACK_LIBDIR)
AC_SUBST(LAPACK_LIB_FLAGS)

dnl ----------------------------------------------------------------------
dnl
dnl    Write to summary file if defined
dnl
dnl ----------------------------------------------------------------------
if test -n "$config_summary_file"; then
	if test -z "$LAPACK_LIBS" ; then
	  echo " NOT using LAPACK"                   >> $config_summary_file
	  echo " Set with LAPACK_LIBS=<LAPACK_LIBS>" >> $config_summary_file
	else
	  echo " Using LAPACK"                       >> $config_summary_file
        echo "   LAPACK_LIBDIR: $LAPACK_LIBDIR"    >> $config_summary_file
        echo "   LAPACK_LIBS:   $LAPACK_LIBS"      >> $config_summary_file
	fi
      echo                                         >> $config_summary_file
fi
