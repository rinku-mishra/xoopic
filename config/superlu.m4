dnl ######################################################################
dnl
dnl File:       superlu.m4
dnl
dnl Purpose:    Determine if SuperLU is installed.
dnl
dnl Variables defined in AC_SUBST
dnl   SUPERLU_LIB
dnl   SUPERLU_LIBDIR
dnl   SUPERLU_INC
dnl   CFLAGS_SLU
dnl   SLU_LINK
dnl   SLU_DUMM
dnl
dnl Version:    $Id: superlu.m4 2281 2007-06-11 19:35:33Z johan $
dnl
dnl Copyright Tech-X Corporation, 2001-2005.  Redistribution allowed
dnl provided this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ######################################################################
dnl  First check for weird variables that the supercomputing centers define
dnl  Then check for over rides from the commandline.
dnl ######################################################################
dnl
dnl NERSC uses SUPERLU_DIST variable
if test -n "$SUPERLU_DIST"; then
   SAVE_SUPERLU_LIBS=$SUPERLU_DIST
   SAVE_SUPERLU_LIBDIR=`dirname $SUPERLU_DIST_SRC`
   if test -z "$SUPERLU_INC"; then SUPERLU_INC="-I$SAVE_SUPERLU_LIBDIR/SRC"; fi
   SAVE_CFLAGS_SLU="-O3 -DNoChange"
   SAVE_SLU_LINK="slud_link"
   SAVE_SLU_DUMM="sslu_dummy"
fi

dnl For Jaguar, it is easiest to just set out own variable.
if test -n "$SUPERLU_SCI"; then
   SAVE_SUPERLU_LIBS=$SUPERLU_SCI
   if test -z "$SUPERLU_INC"; then SUPERLU_INC="-I$HOME/../kruger/superlu_dist_includes"; fi
   CFLAGS_SLU="-O3"
   SAVE_SLU_LINK="slud_link"
   SAVE_SLU_DUMM="sslu_dummy"
fi

dnl Save specification from command-line
if test -n "$SUPERLU_LIBS";   then SAVE_SUPERLU_LIBS="$SUPERLU_LIBS"; fi
if test -n "$SUPERLU_LIBDIR"; then SAVE_SUPERLU_LIBDIR="$SUPERLU_LIBDIR"; fi
if test -n "$SUPERLU_INC";    then SAVE_SUPERLU_INC="$SUPERLU_INC"; fi
if test -n "$CFLAGS_SLU";     then SAVE_CFLAGS_SLU="$CFLAGS_SLU"; fi
if test -n "$SLU_LINK";       then SAVE_SLU_LINK="$SLU_LINK"; fi
if test -n "$SLU_DUMM";       then SAVE_SLU_DUMM="$SLU_DUMM"; fi

dnl ----------------------------------------------------------------------
dnl This section is trying to find SUPERLU_LIBS, absolute path to SuperLU
dnl ----------------------------------------------------------------------

case "$host" in
    *-linux-gnu)    PLAT=_Linux ;;
    *apple-darwin*) ;;
    hppa*-hp-hpux*) PLAT=_hppa ;;
    alpha*)         PLAT=_alpha ;;
    *-*-aix*)       PLAT=_aix ;;
    *-sgi-*)        PLAT=_sgi ;;
    *-*-solaris)    PLAT=_solaris ;;
esac

dnl Allow direct specification of library
AC_ARG_WITH(SUPERLU_LIBS,
  --with-SUPERLU_LIBS=<library>     Full path to SuperLU library,
  [SUPERLU_LIBS="${withval}"])

dnl Check whether an absolute path
absp=""
if test -n "$SUPERLU_LIBS"; then
  absp=`echo "$SUPERLU_LIBS" | grep ^/`
fi
if test -n "$absp"; then
  exists=`ls $SUPERLU_LIBS`
  if test -z $exists; then
    AC_MSG_ERROR($SUPERLU_LIBS does not exist.)
  fi

dnl Not an absolute path, so must find
else

  dnl Look in some default places
  dnl For MPI version, look for the "dist" version.
  if test "$use_mpi" = "true"; then
	SUPERLU_PATH=$SAVE_SUPERLU_LIBDIR:$SAVE_SUPERLU_LIBDIR/lib:/usr/local/lib:$HOME/lib:$HOME/lib/SuperLU_DIST:/usr/local/superlu/lib
	PLAT2=_DIST
  else
	SUPERLU_PATH=/usr/local/lib:$HOME/lib:$HOME/local/lib:$HOME/lib/SuperLU:/usr/local/superlu/lib
  fi

  dnl Allow additional places to look for SUPERLU
  AC_ARG_WITH(SUPERLU_LIBDIR,
	[  --with-SUPERLU_LIBDIR=<dir>	Prefix directory for SuperLU.
	Default is to search ${SUPERLU_PATH}],
	[SUPERLU_LIBDIR="${withval}"])

  if test -n "$SUPERLU_LIBDIR"; then
	SUPERLU_PATH=$SUPERLU_LIBDIR:$SUPERLU_PATH
  fi

  dnl Use TX_PATH_FILE to find SuperLU library
  dnl Complicated by nonstandard names
  TX_PATH_FILE(SUPERLU_LIBS,libsuperlu.a ,"", $SUPERLU_PATH)
  if test -z "$SUPERLU_LIBS"; then
    TX_PATH_FILE(SUPERLU_LIBS, libsuperlu$PLAT.a,"", $SUPERLU_PATH)
  fi
  if test -z "$SUPERLU_LIBS"; then
    if test -n "$PLAT2"; then
	 TX_PATH_FILE(SUPERLU_LIBS, "libsuperlu$PLAT2.a","", $SUPERLU_PATH)
    fi
  fi
  if test -z "$SUPERLU_LIBS"; then
    AC_MSG_WARN(Unable to find the SuperLU library in $SUPERLU_PATH.  Use SUPERLU_LIBDIR to set its directory.)
  fi
fi

dnl ----------------------------------------------------------------------
dnl Need to set up all of the variables for allowing SLU to build properly
dnl Note that there are two separate versions of SuperLU:
dnl    sequential and distributed.
dnl To get linking to work, link in a dummy version of the other one.
dnl Hence, we define SLU_LINK and SLU_DUMM to specify which one is
dnl linked and which one is dummy.  These are used to define NIMROD's
dnl wrappers to SuperLU library.
dnl ----------------------------------------------------------------------

if test -z "$SUPERLU_LIBS"; then
  AC_MSG_WARN(SuperLU not found will build without. Use
		  --with-SUPERLU_LIBS to specify library)
  SUPERLU_LIBS=""
  SUPERLU_LIBDIR=""
  if test -z "$SUPERLU_INC"; then SUPERLU_INC=""; fi
  CFLAGS_SLU=""
  SLU_LINK="sslu_dummy"
  SLU_DUMM="slud_dummy"
else
    SUPERLU_LIBDIR=`dirname $SUPERLU_LIBS`
    if test -z "$SUPERLU_INC"; then
      TX_PATH_FILE(SUPERLU_HEADER, slu_ddefs.h,"",$SUPERLU_LIBDIR/SRC:/usr/local/include:$HOME/local/include)
      if test -z "$SUPERLU_HEADER"; then
        AC_MSG_WARN(Unable to find SuperLU header files.)
      else
        SUPERLU_INC="-I`dirname $SUPERLU_HEADER`"
      fi
    fi
    SUPERLU_LIB_FLAGS="-L`dirname $SUPERLU_LIBS` -lsuperlu"
    if test "$use_mpi" = "true"; then
	SLU_LINK="slud_link"
	SLU_DUMM="sslu_dummy"
    else
	SLU_LINK="sslu_link"
	SLU_DUMM="slud_dummy"
    fi

    case "$host" in
	 x86*-linux* | i386-*-linux* | i686-*linux*)
		CFLAGS_SLU="-O3 -DAdd_ -DSuperLU_2" ;;
	 *-*-aix*)
		CFLAGS_SLU="-O3 -DNoChange" ;;
	*apple-darwin*)
		CFLAGS_SLU="-O3 -DAdd_ -DSuperLU_2" ;;
	*)
		CFLAGS_SLU="-DAdd_ -DSuperLU_2" ;;
	esac
fi
dnl ----------------------------------------------------------------------
dnl export
dnl ----------------------------------------------------------------------
if test -n "$SAVE_SUPERLU_LIBS";   then SUPERLU_LIBS="$SAVE_SUPERLU_LIBS"; fi
if test -n "$SAVE_SUPERLU_LIBDIR"; then SUPERLU_LIBDIR="$SAVE_SUPERLU_LIBDIR"; fi
if test -n "$SAVE_SUPERLU_INC";    then SUPERLU_INC="$SAVE_SUPERLU_INC"; fi
if test -n "$SAVE_CFLAGS_SLU";     then CFLAGS_SLU="$SAVE_CFLAGS_SLU"; fi
if test -n "$SAVE_SLU_LINK";       then SLU_LINK="$SAVE_SLU_LINK"; fi
if test -n "$SAVE_SLU_DUMM";       then SLU_LINK="$SAVE_SLU_DUMM"; fi
AC_SUBST(SUPERLU_LIBS)
AC_SUBST(SUPERLU_LIBDIR)
AC_SUBST(SUPERLU_INC)
AC_SUBST(SUPERLU_LIB_FLAGS)
AC_SUBST(CFLAGS_SLU)
AC_SUBST(SLU_LINK)
AC_SUBST(SLU_DUMM)

dnl ----------------------------------------------------------------------
dnl
dnl    Write to summary file if defined
dnl
dnl ----------------------------------------------------------------------
if test -n "$config_summary_file"; then
     if test -z "$SUPERLU_LIBS" ; then
       echo " NOT using SUPERLU"                   >> $config_summary_file
       echo " Set with SUPERLU_LIBS=<SUPERLU_LIBS>">> $config_summary_file
     else
       echo " Using SUPERLU"                       >> $config_summary_file
       echo "   SUPERLU_LIBDIR: $SUPERLU_LIBDIR"   >> $config_summary_file
       echo "   SUPERLU_LIBS:   $SUPERLU_LIBS"     >> $config_summary_file
       echo "   SUPERLU_INC:    $SUPERLU_INC"      >> $config_summary_file
       echo "   CFLAGS_SLU:     $CFLAGS_SLU"       >> $config_summary_file
     fi
     echo                                          >> $config_summary_file
fi
