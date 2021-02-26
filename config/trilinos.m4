dnl ######################################################################
dnl
dnl File:	trilinos.m4
dnl
dnl Purpose:	Determine the locations of trilinos includes and libraries.
dnl
dnl
dnl Version: $Id: trilinos.m4 2307 2007-06-24 12:43:25Z cary $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################

dnl Include functions from txsearch.m4
builtin(include, config/txsearch.m4)

dnl ######################################################################
dnl
dnl find the location of the trilinos library
dnl
dnl ######################################################################

if test "$parallel" = yes; then
  trilinos_guess_dir=$abs_top_builddir/../txmodules/trilinosmpi:$HOME/$UNIXFLAVOR/trilinosmpi:/usr/local/trilinosmpi
else
  trilinos_guess_dir=$abs_top_builddir/../txmodules/trilinos:$HOME/$UNIXFLAVOR/trilinos:/usr/local/trilinos
fi

TX_LOCATE_PKG(
  [trilinos],
  [$trilinos_guess_dir],
  [az_aztec.h],
  [aztecoo,ml,amesos,epetraext,epetra,triutils,teuchos],
  [],
  [lib])

dnl [aztecoo,ml,amesos,epetraext,epetra,triutils,teuchos],

if test -n "$TRILINOS_INC_AZ_AZTEC_H"; then
  ac_cv_have_trilinos=yes
else
  ac_cv_have_trilinos=no
fi
if test -n "$TRILINOS_LIB_AMESOS"; then
  AC_DEFINE([HAVE_AMESOS], [], ["Whether Amesos was found"])
fi
if test -n "$TRILINOS_LIB_EPETRAEXT"; then
  AC_DEFINE([HAVE_EPETRAEXT], [], ["Whether EpetraExt was found"])
fi

dnl ######################################################################
dnl
dnl Define for whether trilinos was found.
dnl If so, determine its build variables and replace the fortran vars.
dnl
dnl ######################################################################

if test $ac_cv_have_trilinos = yes; then
  AZTECOO_F77=`grep aztecoo_F77 $TRILINOS_INCDIR/Makefile.export.aztecoo.macros | sed 's/^.* = //'`
  if test -n "$AZTECOO_F77"; then
    AZTECOO_FLIBS=`grep aztecoo_FLIBS $TRILINOS_INCDIR/Makefile.export.aztecoo.macros | sed 's/^.* = //'`
    AZTECOO_LDFLAGS=`grep aztecoo_LDFLAGS $TRILINOS_INCDIR/Makefile.export.aztecoo.macros | sed 's/^.* = //'`
    AZTECOO_LAPACK_LIBS=`grep aztecoo_LAPACK_LIBS $TRILINOS_INCDIR/Makefile.export.aztecoo.macros | sed 's/^.* = //'`
    AZTECOO_BLAS_LIBS=`grep aztecoo_BLAS_LIBS $TRILINOS_INCDIR/Makefile.export.aztecoo.macros | sed 's/^.* = //'`
  else
    AZTECOO_F77=`grep AZTECOO_F77 $TRILINOS_INCDIR/Makefile.export.aztecoo.macros | sed 's/^.* = //'`
    AZTECOO_FLIBS=`grep AZTECOO_FLIBS $TRILINOS_INCDIR/Makefile.export.aztecoo.macros | sed 's/^.* = //'`
    AZTECOO_LDFLAGS=`grep AZTECOO_LDFLAGS $TRILINOS_INCDIR/Makefile.export.aztecoo.macros | sed 's/^.* = //'`
    AZTECOO_LAPACK_LIBS=`grep AZTECOO_LAPACK_LIBS $TRILINOS_INCDIR/Makefile.export.aztecoo.macros | sed 's/^.* = //'`
    AZTECOO_BLAS_LIBS=`grep AZTECOO_BLAS_LIBS $TRILINOS_INCDIR/Makefile.export.aztecoo.macros | sed 's/^.* = //'`
  fi
fi

dnl removing -lgcc_s if building statically
if test "$enable_staticlink" = yes; then
    AZTECOO_FLIBS=`echo $AZTECOO_FLIBS | sed 's/-lgcc_s//'`
    AZTECOO_LDFLAGS=`echo $AZTECOO_LDFLAGS | sed 's/-lgcc_s//'`
    AZTECOO_LAPACK_LIBS=`echo $AZTECOO_LAPACK_LIBS | sed 's/-lgcc_s//'`
    AZTECOO_BLAS_LIBS=`echo $AZTECOO_BLAS_LIBS | sed 's/-lgcc_s//'`
fi

dnl ######################################################################
dnl
dnl If libraries found, reset variables
dnl
dnl ######################################################################

AM_CONDITIONAL(HAVE_TRILINOS, test -n "$TRILINOS_LIBS")
if test -n "$TRILINOS_LIBS"; then
AC_DEFINE(HAVE_TRILINOS, [], [Defined if trilinos was found.])
fi


dnl ######################################################################
dnl
dnl Now can compose the lapack libraries
dnl
dnl ######################################################################

TRILINOS_LIBS="$TRILINOS_LIBS $AZTECOO_LAPACK_LIBS $AZTECOO_BLAS_LIBS $AZTECOO_FLIBS $AZTECOO_LDFLAGS"
if test -n "$config_summary_file"; then
  if test $ac_cv_have_trilinos = yes; then
    echo "  "MODIFICATIONS:  >> "$config_summary_file"
    echo "    "TRILINOS_LIBS"                 ": $TRILINOS_LIBS >> $config_summary_file
  else
    echo "Trilinos not found." >> $config_summary_file
  fi
fi

AC_SUBST(TRILINOS_LIBS)

