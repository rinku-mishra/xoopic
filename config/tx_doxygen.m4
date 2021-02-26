dnl ######################################################################
dnl
dnl File:	doxygen.m4
dnl
dnl Purpose:	Determine the location of doxygen and set the directory
dnl		for its output
dnl
dnl Version: $Id: tx_doxygen.m4 2356 2007-07-27 13:10:08Z cary $
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Look for the executable.
dnl
dnl ######################################################################

AC_PATH_PROGS(DOXYGEN, doxygen, "")
if test -z "$DOXYGEN"; then
  AC_MSG_WARN(The doxygen utility was not found in your path.  Combined C++ documentation will not be made.)
fi
AC_SUBST(DOXYGEN)

dnl ######################################################################
dnl
dnl Allow setting of doxygen dir.  Otherwise defaults to docs/cxxapi
dnl
dnl ######################################################################

AC_ARG_WITH(doxygen-docsdir,
	[  --with-doxygen-docsdir=<where to put doxygen output>],
	DOXYGEN_DOCSDIR="$withval", DOXYGEN_DOCSDIR="")
if test -z "$DOXYGEN_DOCSDIR"; then
  DOXYGEN_DOCSDIR=docs/cxxapi
fi
if test ! -d $DOXYGEN_DOCSDIR; then mkdir -p $DOXYGEN_DOCSDIR; fi
DOXYGEN_DOCSDIR=`(cd $DOXYGEN_DOCSDIR; pwd)`
AC_SUBST(DOXYGEN_DOCSDIR)
if test -n "$config_summary_file"; then
  echo                                            >> $config_summary_file
  echo "Documentation built with" >> $config_summary_file
  echo "  DOXYGEN:  $DOXYGEN"  >> $config_summary_file
  echo "  Doxygen docs will be in $DOXYGEN_DOCSDIR"  >> $config_summary_file
fi

