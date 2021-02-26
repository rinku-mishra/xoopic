dnl ######################################################################
dnl
dnl File:	txphysics.m4
dnl
dnl Purpose:	Determine the locations of the txphysic component
dnl		includes and libraries. Componenets are nsecc and
dnl		txcrange.
dnl
dnl Version: $Id: txphysics.m4 2347 2007-07-24 17:33:58Z veitzer $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################

dnl Required for TX_PATH_FILE(S) to work

builtin(include,config/txsearch.m4)

dnl ######################################################################
dnl
dnl Allow the user to specify an overall txphysics directory.  If specified,
dnl we look for include and lib under this.
dnl
dnl ######################################################################

AC_ARG_WITH(txphysics-dir,
	[  --with-txphysics-dir=<txphysics installation directory> ],
	TXPHYSICS_DIR="$withval", TXPHYSICS_DIR="")

dnl ######################################################################
dnl
dnl Find txegenelec and txigenelec includes - looking in include location if present,
dnl otherwise in dir/include if present, otherwise in default locations,
dnl first parallel, then serial.
dnl
dnl ######################################################################

dnl echo Setting TXPHYSICS_INCPATH
dnl JRC 22May06: Not needed as always set below. unset TXPHYSICS_INCPATH
AC_ARG_WITH(txphysics-incdir,
	[  --with-txphysics-incdir=<txphysics include directory>],
	TXPHYSICS_INCDIR="$withval", TXPHYSICS_INCDIR="")
if test -n "$TXPHYSICS_INCDIR"; then
  TXPHYSICS_INCPATH=$TXPHYSICS_INCDIR
elif test -n "$TXPHYSICS_DIR"; then
  TXPHYSICS_INCPATH=$TXPHYSICS_DIR/include
else
  TXPHYSICS_INCPATH="$abs_top_builddir/../txmodules/txphysics/include:/usr/local/txphysics/include"
fi

dnl ######################################################################
dnl
dnl Find txegenelec.h. The other header files are in the same place. Then
dnl set TXPHYSICS_INCDIR and TXPHYSICS_INC.
dnl
dnl ######################################################################

if test -n "$TXPHYSICS_INCPATH"; then
  TX_PATH_FILES(TXEGENELEC_H, txegenelec.h, "", $TXPHYSICS_INCPATH)
  if test -z "$TXEGENELEC_H"; then
    AC_MSG_WARN(txegenelec.h not found in $TXPHYSICS_INCPATH.  Use --with-txphysics-incdir=)
    TXPHYSICS_INC=" "
  else
    TXPHYSICS_INCDIR=`dirname $TXEGENELEC_H`
    AC_SUBST(TXPHYSICS_INCDIR)
    TXPHYSICS_INC="-I$TXPHYSICS_INCDIR"
    AC_SUBST(TXPHYSICS_INC)
  fi
fi

dnl ######################################################################
dnl
dnl Find txegenelec, txigenelec, and txionpack libraries
dnl
dnl ######################################################################

AC_ARG_WITH(txphysics-libdir,
	[  --with-txphysics-libdir=<location of txphysics libraries> ],
	TXPHYSICS_LIBDIR="$withval",TXPHYSICS_LIBDIR="")

dnl echo Setting TXPHYSICS_LIBPATH
unset TXPHYSICS_LIBPATH
dnl echo TXPHYSICS_LIBDIR = $TXPHYSICS_LIBDIR
dnl echo TXPHYSICS_DIR = $TXPHYSICS_DIR
if test -n "$TXPHYSICS_LIBDIR"; then
  TXPHYSICS_LIBPATH=$TXPHYSICS_LIBDIR
elif test -n "$TXPHYSICS_DIR"; then
  TXPHYSICS_LIBPATH=$TXPHYSICS_DIR/lib
elif test -n "$TXPHYSICS_INCDIR"; then
  TXPHYSICS_LIBPATH=`dirname $TXPHYSICS_INCDIR`/lib
fi
dnl echo TXPHYSICS_LIBPATH = $TXPHYSICS_LIBPATH

if test -n "$TXPHYSICS_LIBPATH"; then
  TX_PATH_FILES(LIBTXIGENELEC_A, libtxigenelec.a, "", $TXPHYSICS_LIBPATH)
  if test -z "$LIBTXIGENELEC_A"; then
    AC_MSG_WARN(libtxigenelec.a not found.  Use --with-txphysics-libdir=)
    TXPHYSICS_LIBS=""
  else
    TXPHYSICS_LIBDIR=`dirname $LIBTXIGENELEC_A`
    AC_SUBST(TXPHYSICS_LIBDIR)
    TXPHYSICS_LIB="-ltxionpack -ltxegenelec -ltxigenelec -ltxrand"
    dnl For backwards compatibility, before libtxstopping and libtxsputter existed
    TX_PATH_FILES(LIBTXSTOPPING_A, libtxstopping.a, "", $TXPHYSICS_LIBPATH)
    if test ! -z "$LIBTXSTOPPING_A"; then
      TXPHYSICS_LIB="-ltxstopping $TXPHYSICS_LIB"
      AC_DEFINE(HAVE_TXSTOPPING, , Define if libtxstopping is found in the txphysics package)
    fi
    TX_PATH_FILES(LIBTXSPUTTER_A, libtxsputter.a, "", $TXPHYSICS_LIBPATH)
    if test ! -z "$LIBTXSPUTTER_A"; then
      TXPHYSICS_LIB="-ltxsputter $TXPHYSICS_LIB"
      AC_DEFINE(HAVE_TXSPUTTER, , Define if libtxsputter is found in the txphysics package)
    fi

    TXPHYSICS_LIBS="-L$TXPHYSICS_LIBDIR "'$(RPATH_FLAG)'"$TXPHYSICS_LIBDIR $TXPHYSICS_LIB"
  fi
  AC_SUBST(TXPHYSICS_LIBS)
fi

dnl ######################################################################
dnl
dnl Defines for whether txphysics found. Not possible to explicitly
dnl disable these. If libraries were found they will be used.
dnl
dnl ######################################################################

if test -n "$TXPHYSICS_LIBS"; then
  ac_cv_have_txphysics=yes
  AC_DEFINE(HAVE_TXPHYSICS, , Define if the txphysics libraries were found)
dnl The following macro is needed for deciding to build vpionize in the
dnl top level Makefile.am
  dnl VPIONIZE_DIR="vpionize"
  dnl AC_SUBST(VPIONIZE_DIR)
dnl The following macros are needed for building vorpal with vpionize, which
dnl is only built if txphysics is found.
  dnl VPIONIZE_LIBS="-L../vpionize -lvpionize"
  dnl VPIONIZE_DEPS="../vpionize/libvpionize.a"
  dnl AC_SUBST(VPIONIZE_LIBS)
  dnl AC_SUBST(VPIONIZE_DEPS)
else
  ac_cv_have_txphysics=no
fi

dnl ######################################################################
dnl
dnl    Write to summary file if defined
dnl
dnl ######################################################################
if test -n "$config_summary_file"; then
   echo                                      >> $config_summary_file
   if test -n "$TXPHYSICS_LIBS"; then
      echo "Using TxPhysics with"                 >> $config_summary_file
      echo "  TXPHYSICS_INC:    $TXPHYSICS_INC"   >> $config_summary_file
      echo "  TXPHYSICS_LIBDIR: $TXPHYSICS_LIBDIR">> $config_summary_file
      echo "  TXPHYSICS_LIBS:   $TXPHYSICS_LIBS"  >> $config_summary_file
   else
      echo "NOT using TxPhysics"                >> $config_summary_file
   fi
fi

