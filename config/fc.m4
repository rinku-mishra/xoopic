dnl ######################################################################
dnl
dnl File:       fc.m4
dnl
dnl Purpose:    Determine F90 (a.k.a. FC) compilers.
dnl
dnl Version:    $Id: fc.m4 2385 2007-08-31 22:59:23Z cary $
dnl
dnl Copyright Tech-X Corporation, 2001-2005.  Redistribution allowed
dnl provided this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ----------------------------------------------------------------------
dnl
dnl  See README.fortran
dnl  Relevant files:
dnl   f77.m4, f77flags.m4, f77interop.m4, mpif77.m4
dnl   fc.m4, fcflags.m4, fcinterop.m4, mpif90.m4
dnl
dnl Set the variable:
dnl     FC
dnl
dnl     Notes:
dnl        o See fcflags.m4 for setting the flags.  The separation is
dnl          done to make the configure flags more intuitive
dnl
dnl ----------------------------------------------------------------------

dnl ----------------------------------------------------------------------
dnl  allow flags or environment to overwrite variables
dnl ----------------------------------------------------------------------
AC_ARG_WITH(FC, AC_HELP_STRING([--with-FC], [fortran compiiler]), FC="$withval")
AC_MSG_CHECKING(for F90 compilers )


dnl ----------------------------------------------------------------------
dnl Set up default values
dnl ----------------------------------------------------------------------

if test -z "$FC"; then

  dnl Check whether an absolute path
  if test -n "$FC"; then
    absp=`echo "$FC" | grep ^/`
  fi
  if test -n "$absp"; then
    exists=`ls $FC`
    if test -z $exists; then
      AC_MSG_ERROR($FC does not exist.)
    fi

  dnl Not an absolute path, so must find
  else

  dnl Allow additional places to look for FC
    AC_ARG_WITH(FC_PATH,
[  --with-FC_PATH=<F90 compiler directory>     to set root for f90 installation], F90_PATH="$withval")

dnl First look in path, dnl  then system paths, then paths for compilers
dnl that work on more than one platform.  Thus give user the preference,
dnl then the systems (which are ignored on other systems), then the generic
    if test -z "$FC_PATH"; then
      FC_PATH=$PATH:/usr/local/lf9562/bin:/opt/SUNWspro/bin:/usr/local/intel_fc_80/bin:/usr/local/pgi/linux86/bin:/usr/absoft/bin:/Applications/Absoft/bin
    fi

dnl Look for specified compiler or else any of those supported
    if test -n "$FC"; then
      FCS=$FC
    else
      FCS="lf95 pgf90 pathf90 xlf90 ifort ifc f90 gfortran"
    fi
    AC_PATH_PROGS(ABSFC, $FCS, "", $FC_PATH)

  fi

  if test -n "$ABSFC"; then
    dnl AC_MSG_RESULT(... $ABSFC)
    FC=$ABSFC
dnl JRC - in case overwritten
    AC_SUBST(FC)
    FC_BINDIR=`dirname $FC`
    echo Using F90 compiler $FC.
  else
    AC_MSG_WARN(Unable to find f90 in your path.  F90 modules will not compile.)
  fi

fi

dnl
dnl JRC: if a relative path, find absolute for use by fcinteropt.m4
dnl
if test -z "$ABSFC"; then
  AC_PATH_PROGS(ABSFC, $FC, "", $PATH)
fi

dnl ----------------------------------------------------------------------
dnl export compiler and its base name
dnl ----------------------------------------------------------------------
AC_SUBST(FC)
FCBASE=`basename $FC`
AC_SUBST(FCBASE)

dnl ----------------------------------------------------------------------
dnl FC_BASE is in case the FC flag gets overwritten by mpif90
dnl ----------------------------------------------------------------------
if test -n "$FC"; then
  FC_BASE=`basename $FC`
fi
AC_SUBST(FC_BASE)
