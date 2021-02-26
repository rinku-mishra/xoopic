dnl ######################################################################
dnl
dnl File:       f77.m4
dnl
dnl Purpose:    Determine the F77 compiler.
dnl
dnl Version:    $Id: f77.m4 2188 2007-02-20 22:02:30Z johan $
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
dnl     F77
dnl
dnl     Notes:
dnl        o See f77flags.m4 for setting the flags.  The separation is
dnl          done to make the configure flags more intuitive
dnl        o Note that old versions of automake can screw up distinction
dnl          between FC and F77.  See README.fortran
dnl
dnl ----------------------------------------------------------------------

dnl ----------------------------------------------------------------------
dnl  allow flags or environment to overwrite variables
dnl ----------------------------------------------------------------------
AC_ARG_WITH(
F77,
[  --with-F77=MyF77   to use MyF77 instead of the default Fortran compiler],
F77="$withval")

AC_MSG_CHECKING(for F77 compiler)

dnl ----------------------------------------------------------------------

 if test "$F77" != none; then

# If F77 unspecified, try FC
  if test -z "$F77" ; then
    F77=$FC
  fi

 # Check whether an absolute path
   # echo Finding absp
   if test -n "$F77"; then
     absp=`echo "$F77" | grep ^/`
   fi
   # echo absp = $absp
   if test -n "$absp"; then
     exists=`ls $F77`
     if test -z $exists; then
       AC_MSG_ERROR($F77 does not exist.)
     fi

 # Not an absolute path, so must find
   else

 # Allow additional places to look for F77
     AC_ARG_WITH(F77_PATH,
 [  --with-F77_PATH=<F77 compiler directory>     to set root for f77 installation], F77_PATH="$withval")

 # First look in path, then other directories
     if test -z "$F77_PATH"; then
 # Look in user's path first, then system paths, then paths for compilers
 # that work on more than one platform.  Thus give user the preference,
 # then the systems (which are ignored on other systems), then the generic
       F77_PATH=$PATH:/opt/SUNWspro/bin:/usr/local/intel_fc_80/bin:/usr/local/pgi/linux86/bin:/usr/absoft/bin
     fi
     # echo
     # echo Looking for FC in $FC_PATH

 # Look for specified compiler or else any of those supported
     if test -n "$F77"; then
       F77S=$F77
     else
       F77S="xlf gfortran g77 f77 pgf77 ifort fort77 fort"
     fi
     AC_PATH_PROGS(F77, $F77S, "", $F77_PATH)

   fi

   if test -n "$F77"; then
     dnl JRC: below not needed as done by AC_PATH_PROGS
     dnl AC_MSG_RESULT(... $F77)
     AC_SUBST(F77)
     F77_BINDIR=`dirname $F77`
     echo Using F77 compiler $F77.
   else
     AC_MSG_WARN(Unable to find f77 in your path.  F77 modules will not compile.)
   fi
 else
   F77=""
   AC_MSG_WARN(F77 specified as none.  F77 modules will not compile.)
 fi

 if test -z "$F77" ; then
   if test -z "$DEFAULT_F77" ; then
 	F77="$FC"
   else
 	F77="$DEFAULT_F77"
   fi
 fi

dnl ----------------------------------------------------------------------
dnl export
dnl ----------------------------------------------------------------------
dnl JRC: below not needed as done by AC_PATH_PROGS
dnl AC_MSG_RESULT($F77)
AC_SUBST(F77)
