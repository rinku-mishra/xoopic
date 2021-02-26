dnl ######################################################################
dnl
dnl File:       txsuperlu.m4
dnl
dnl Purpose:    Determine if SuperLU is installed.
dnl
dnl Version:    $Id: txsuperlu.m4 2314 2007-07-03 13:40:04Z cary $
dnl
dnl Copyright Tech-X Corporation, 2001-2005.  Redistribution allowed
dnl provided this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ##################################################################
dnl
dnl Include functions from txsearch.m4
dnl
dnl ##################################################################
builtin(include, config/txsearch.m4)

dnl ##################################################################
dnl
dnl Locate the sequential package, verify it, set conditional
dnl
dnl ##################################################################

if test "$WANT_SUPERLU_SEQ" = yes; then

  SUPERLU_SEQ_PATH=$abs_top_builddir/../txmodules/superlu:$HOME/superlu_seq:$HOME/$UNIXFLAVOR/superlu:$HOME/local:/usr/local/superlu_seq:/usr/local/superlu
  TX_LOCATE_PKG(
	[SUPERLU_SEQ],
	[$SUPERLU_SEQ_PATH],
	[colamd.h,old_colamd.h,slu_Cnames.h,slu_ddefs.h,slu_util.h,supermatrix.h],
	[superlu_3.0])

  if test -n "$SUPERLU_SEQ_PATH_LIBDIR"; then
    symbol="superlu_free"
    AC_CHECK_LIB(
	[superlu_3.0],
	[$symbol],,
	AC_MSG_ERROR($symbol not found in libsuperlu_3.0.),
    )
  fi
fi

AM_CONDITIONAL([HAVE_SUPERLU_SEQ], [test "$FOUND_SUPERLU_SEQ" != "no"])

dnl ##################################################################
dnl
dnl Locate the distributed package, verify it, set conditional
dnl
dnl ##################################################################

if test "$WANT_SUPERLU_DIST" = yes; then

  SUPERLU_DIST_PATH=$abs_top_builddir/../txmodules/superlumpi:$HOME/superlu_dist:$HOME/$UNIXFLAVOR/superlumpi:/usr/local/superlu_dist:/usr/local/superlumpi
  TX_LOCATE_PKG(
	[SUPERLU_DIST],
	[$SUPERLU_DIST_PATH],
	[colamd.h,old_colamd.h,slu_Cnames.h,slu_ddefs.h,slu_util.h,supermatrix.h],
	[superlu_3.0])

  if test -n "$SUPERLU_DIST_PATH_LIBDIR"; then
    symbol="superlu_free"
    AC_CHECK_LIB(
	[superlu_3.0],
	[$symbol],,
	AC_MSG_ERROR($symbol not found in libsuperlu_3.0.),
    )
  fi
fi

AM_CONDITIONAL([HAVE_SUPERLU_DIST], [test "$FOUND_SUPERLU_DIST" != "no"])


