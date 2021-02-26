dnl ######################################################################
dnl
dnl File:       petsc.m4
dnl
dnl Purpose:    Determine if PETSc is installed.
dnl
dnl Variables defined in AC_SUBST
dnl   Basics:			Include Directories:
dnl     PETSC_DIR		  PETSC_INCDIR
dnl     PETSC_ARCH		  PETSC_INCDIR_ARCH
dnl   Library:			Library Info:
dnl     PETSC_LIB		  PETSC_LIBDIR
dnl     PETSC_LIB_CONTRIB	  PETSC_LIB_TYPE
dnl     PETSC_LIB_DM		  PETSC_LIB_STATUS
dnl     PETSC_LIB_KSP           Complete Substitutions
dnl     PETSC_LIB_MAT		  PETSC_INC
dnl     PETSC_LIB_SNES		  PETSC_LIBS
dnl     PETSC_LIB_TS
dnl     PETSC_LIB_VEC
dnl
dnl Version:    $Id: petsc.m4 2399 2007-09-16 10:36:51Z cary $
dnl
dnl Copyright Tech-X Corporation, 2001-2005.  Redistribution allowed
dnl provided this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Overrides
dnl
dnl ######################################################################

AC_ARG_WITH(petsc-dir, AC_HELP_STRING([--with-petsc-dir=<petsc install dir>],
        [set the PETSc installation directory]),
        PETSC_DIR="$withval",)
if test -n "$PETSC_DIR"; then
  PETSC_INCPATH=$PETSC_DIR/include
fi
if test "$parallel" = yes; then
  PETSC_INCPATH=$PETSC_INCPATH:/usr/local/petscmpi/include
else
  PETSC_INCPATH=$PETSC_INCPATH:/usr/local/petsc/include
fi

dnl ######################################################################
dnl
dnl find petsc.h
dnl
dnl ######################################################################

TX_PATH_FILE(
  [PETSC_INC_PETSC_H],
  [petsc.h],
  [],
  [$PETSC_INCPATH])

if test -n "$PETSC_INC_PETSC_H"; then
  PETSC_INCDIR1=`dirname $PETSC_INC_PETSC_H`
  PETSC_DIR=`dirname $PETSC_INCDIR1`
# Determine other parts of installation
  AC_MSG_CHECKING(For PETSc configuration)
  PETSC_CONFFILE=$PETSC_DIR/bmake/petscconf
  AC_MSG_RESULT($PETSC_CONFFILE)
  PETSC_ARCH=`grep PETSC_ARCH= $PETSC_CONFFILE | sed 's/PETSC_ARCH=//'`
  TX_LOCATE_PKG(
    [PETSc],
    [$PETSC_DIR],
    [petscconf.h],
    [petsccontrib,petscdm,petscts,petscsnes,petscksp,petscmat,petscvec,petsc],
    [bmake/$PETSC_ARCH],
    [lib/$PETSC_ARCH])
else
  AC_MSG_WARN(PETSc not found in $PETSC_INCPATH.  Use --with-petsc-dir= to set location.)
fi

if test -n "$PETSC_INCDIR1"; then
  PETSC_ALLINC="$PETSC_INC -I$PETSC_INCDIR1"
  dnl echo PETSC_ALLINC = $PETSC_ALLINC
fi

if test "$parallel" != yes; then
  PETSC_ALLINC="$PETSC_ALLINC -I$PETSC_INCDIR1/mpiuni"
  PETSC_ALLLIBS="$PETSC_LIBS -lmpiuni"
else
  PETSC_ALLLIBS="$PETSC_LIBS"
fi

# Save
AC_SUBST(PETSC_ARCH)
AC_SUBST(PETSC_ALLINC)
AC_SUBST(PETSC_ALLLIBS)
if test -n "$config_summary_file"; then
  echo "    PETSC_ALLINC                  :$PETSC_ALLINC" >>$config_summary_file
  echo "    PETSC_ALLLIBS                 :$PETSC_ALLLIBS" >>$config_summary_file
fi
