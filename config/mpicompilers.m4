dnl ######################################################################
dnl
dnl File:	mpicompilers.m4
dnl
dnl Purpose:	Determine where the MPI compilers are.  This can be used
dnl		for setting the default compiler when one has some
dnl		reason for believing it should be parallel.
dnl
dnl Version:	$Id: mpicompilers.m4 1820 2006-05-23 13:52:54Z cary $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Start with C++
dnl
dnl ######################################################################

AC_ARG_WITH(MPICXX, [  --with-MPICXX=<mpi C++ complier>],
	MPICXX="$withval")
MPIPATH=/usr/local/mpi/bin:/usr/local/mpich/bin:/usr/local/lam/bin
if test -n "$MPICXX"; then
  MPICXXTMP=`echo $MPICXX | sed 's/ .*$//'`
  # echo MPICXXTMP = $MPICXXTMP
  MPICXXBASE=`basename $MPICXXTMP`
  MPICXXDIR=`dirname $MPICXXTMP`
  # echo MPICXXDIR = $MPICXXDIR
  MPIPATH="$MPICXXDIR:$PATH:$MPIPATH"
  # echo MPIPATH = $MPIPATH
  AC_PATH_PROGS(ABSMPICXX, $MPICXXBASE, "", $MPIPATH)
else
  MPIPATH="$PATH:$MPIPATH"
  AC_PATH_PROGS(ABSMPICXX, mpicxx mpic++ mpiCC, "", $MPIPATH)
  if test -n "$ABSMPICXX"; then
    MPICXXBASE=`basename $ABSMPICXX`
    MPICXX=$MPICXXBASE
  fi
fi

dnl JRC 22May06: No need for REALMPI*.  All determined by flags.m4.
if test -z "$ABSMPICXX"; then
  AC_MSG_WARN(No mpi cxx compiler in $MPIPATH.)
  if test "$PACKAGE" = ntcphys; then
    AC_MSG_WARN(mpipython will not be compiled.)
  fi
else
  case $MPICXXBASE in
    KCC)
      dnl REALMPICXX=KCC
      MPICC=cc
      CC=cc
      ;;
    dnl *)
      dnl REALMPICXX=`grep ^CCCBASE $ABSMPICXX | sed 's/^.*=\"//' | sed 's/\".*$//'`
      dnl REALMPICXX=`echo $REALMPICXX | sed 's/ //g'`
      dnl ;;
  esac
fi
AC_SUBST(MPICXX)
AC_SUBST(ABSMPICXX)
dnl AC_SUBST(REALMPICXX)

dnl ######################################################################
dnl
dnl MPI C compiler
dnl
dnl ######################################################################

AC_ARG_WITH(MPICC,
[  --with-MPICC=<mpi C compiler>           to set MPI C compiler],
MPICC="$withval")
if test -n "$MPICC"; then
  MPICCTMP=`echo $MPICC | sed 's/ .*$//'`
  MPICCBASE=`basename $MPICCTMP`
  AC_PATH_PROGS(ABSMPICC, $MPICCBASE, "", $MPIPATH)
else
  AC_PATH_PROGS(ABSMPICC, mpicc, "", $MPIPATH)
  MPICCBASE=mpicc
  MPICC=mpicc
fi

if test -z "$ABSMPICC"; then
  AC_MSG_WARN(No mpi C compiler.)
  if test "$PACKAGE" = ntcphys; then
    AC_MSG_WARN(... mpipython will not be compiled.)
  fi
else
  MPI_TEMP=`dirname $ABSMPICC`
  MPI_INCDIR="`dirname $MPI_TEMP`/include"
  AC_SUBST(MPI_INCDIR)
  AC_SUBST(MPICC)
  AC_SUBST(ABSMPICC)
  dnl REALMPICC=`grep ^CCBASE $ABSMPICC | sed 's/^.*=\"//' | sed 's/\".*$//'`
  dnl REALMPICC=`echo $REALMPICC | sed 's/ //g'`
  dnl AC_SUBST(REALMPICC)
fi

dnl ######################################################################
dnl
dnl MPI f77 compiler
dnl
dnl ######################################################################

if test "$HAS_F77" != "NO"; then

  AC_ARG_WITH(MPIF77, [  --with-MPIF77=<mpi FORTRAN77 compiler>],
	MPIF77="$withval")
  if test -n "$MPIF77"; then
    MPIF77TMP=`echo $MPIF77 | sed 's/ .*$//'`
    MPIF77BASE=`basename $MPIF77TMP`
    AC_PATH_PROG(ABSMPIF77, $MPIF77BASE)
  else
    AC_PATH_PROGS(ABSMPIF77, mpif77, "", $PATH:$MPIPATH)
    MPIF77BASE=mpif77
    MPIF77=mpif77
  fi

  if test -z "$ABSMPIF77"; then
    AC_MSG_WARN(No mpi F77 compiler.)
    if test "$PACKAGE" = ntcphys; then
      AC_MSG_WARN(... mpipython will not be compiled.)
    fi
  dnl else
    dnl REALMPIF77=`grep ^F77BASE $ABSMPIF77 | sed 's/^.*=\"//' | sed 's/\".*$//'`
    dnl REALMPIF77=`echo $REALMPIF77 | sed 's/ //g'`
    dnl AC_SUBST(REALMPIF77)
  fi
  AC_SUBST(MPIF77)
  AC_SUBST(ABSMPIF77)

fi

dnl ######################################################################
dnl
dnl f90 mpi compiler
dnl
dnl ######################################################################

if test "$HAS_F90" != "NO"; then

  AC_ARG_WITH(MPIF90, [  --with-MPIF90=<mpi FORTRAN90 compiler>],
	MPIF90="$withval")
  if test -n "$MPIF90"; then
    MPIF90TMP=`echo $MPIF90 | sed 's/ .*$//'`
    MPIF90BASE=`basename $MPIF90TMP`
    AC_PATH_PROG(ABSMPIF90, $MPIF90BASE)
  else
    AC_PATH_PROGS(ABSMPIF90, mpif90, "", $PATH:$MPIPATH)
    MPIF90BASE=mpif90
    MPIF90=mpif90
  fi

  if test -z "$ABSMPIF90"; then
    AC_MSG_WARN(No mpi F90 compiler.)
    if test "$PACKAGE" = ntcphys; then
      AC_MSG_WARN(... glf23f90 will not be compiled.)
    fi
  dnl else
    dnl REALMPIF90=`grep ^F90BASE $ABSMPIF90 | sed 's/^.*=\"//' | sed 's/\".*$//'`
    dnl REALMPIF90=`echo $REALMPIF90 | sed 's/ //g'`
    dnl AC_SUBST(REALMPIF90)
  fi
  AC_SUBST(MPIF90)
  AC_SUBST(ABSMPIF90)

fi

