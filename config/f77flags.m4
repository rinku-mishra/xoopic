dnl ######################################################################
dnl
dnl File:	fortflags.m4
dnl
dnl Purpose:	
dnl   	Given F77 compiler, determine flags
dnl   	See also f77.m4 and f77interop.m4
dnl
dnl Variables set here include
dnl      F77_LD
dnl      F77_DBL_FLAG
dnl        Promote r4 to r8
dnl      F77_OPT_FLAG
dnl        Normal optimization
dnl      F77_MACH_FLAG
dnl        Machine dependent flags for "ultra" optimization.  NEEDS WORK!!
dnl      F77_NOOPT_FLAG
dnl        o Sometimes better to explicitly turn off optimization
dnl      F77_DEBUG_FLAG
dnl        Include check array conformance if available
dnl      F77_OTHER_FLAG
dnl        Things should probably be used, but are not strictly necessary
dnl      F77_INCLUDE_FLAG
dnl        Because Absoft does not use the -I flag for include directories
dnl      F77_PIC_FLAG
dnl        Position-Independent Code (for shared libraries)
dnl      FFLAGS
dnl
dnl Version:	$Id: f77flags.m4 2334 2007-07-18 02:30:16Z kuehn $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ----------------------------------------------------------------------
dnl  allow flags or environment to overwrite variables
dnl ----------------------------------------------------------------------
AC_ARG_WITH(
    FFLAGS,
    [  --with-FFLAGS="MyFFLAGS"   to set Fortran flags with Myf90],
    FFLAGS="$withval")
AC_ARG_ENABLE(optimize,[  --enable-optimize       turn on optimization flags],
              OPTIMIZED=yes, OPTIMIZED=no)
AC_ARG_ENABLE(fulloptimize,[  --enable-fulloptimize   turn on full optimization flags],
              OPTIMIZED=full, OPTIMIZED=$OPTIMIZED)
AC_ARG_WITH(
	DEBUG,
	[  --enable-debug  turn on debugging, turn off optimization.],
	DEBUG=yes, DEBUG=no)


dnl ----------------------------------------------------------------------
dnl Check on host and f90 since it is possible to have same compiler on
dnl different platforms with subtle differences
dnl ----------------------------------------------------------------------
case "$host" in

 xt3-*)
    F77_DBL_FLAG="-r8"
    F77_OPT_FLAG="-fastsse -tp amd64"
    F77_NOOPT_FLAG=""
    F77_DEBUG_FLAG="-g -C -trapuv -Minfo"
    F77_OTHER_FLAG="-byteswapio"
    ;;

 *-linux-gnu)

  if test -n "$F77"; then
    fbindir=`dirname $F77`
    flibdir="`dirname $fbindir`/lib"
  else
    fbindir=""
    flibdir=""
  fi
  case $F77 in
     *lf95*)
         F77_DBL_FLAG="--dbl "
         F77_OPT_FLAG="-O --prefetch 2"
         F77_NOOPT_FLAG="-O0"
         F77_DEBUG_FLAG="-g --chk"
         F77_OTHER_FLAG="--ap --pca"     dnl Preserve arithmetic precision
         F77_PIC_FLAG="-shared"
         ;;
     *pgf90)
         F77_DBL_FLAG="-r8 "
         F77_OPT_FLAG="-fast"
         dnl SEK: I think fast has all of these other optimizations but not sure
         dnl F77_OPT_FLAG="-fast -Mcache_align -Munroll -Mdalign -Minline -Mvect=prefetch"
         F77_NOOPT_FLAG=""
         F77_DEBUG_FLAG="-g -C -trapuv -Minfo"
         F77_OTHER_FLAG="-byteswapio"    dnl Use little endian for binary compatibility
         ;;
     *ifort)
         F77_DBL_FLAG="-autodouble "
         F77_OPT_FLAG="-O2"
         F77_NOOPT_FLAG="-O0"
         F77_MACH_FLAG="-O2 -tpp7 -xW"
         F77_DEBUG_FLAG="-g -inline_debug_info"
         F77_OTHER_FLAG="-cm -w -l32 -w95"    dnl Quiet
         F77_PIC_FLAG="-Kpic"
         ;;
     *ifc)
         F77_DBL_FLAG="-autodouble "
         F77_OPT_FLAG="-O2"
         F77_NOOPT_FLAG="-O0"
         dnl F77_OPT_FLAG="-O2 -tpp7 -xW"  dnl Pentium 4
         F77_DEBUG_FLAG="-g -inline_debug_info"
         F77_OTHER_FLAG="-cm -w -l32 -w95"    dnl Quiet
         F77_PIC_FLAG="-Kpic"
         ;;
     *bsoft)
         dnl SEK: Not sure about some of these flags because the naming
         dnl      scheme is so screwed up
         F77_INCLUDE_FLAG="-p"
         F77_DBL_FLAG="-N11"
         F77_OPT_FLAG="-O -B100"
         F77_NOOPT_FLAG="-O0"
         F77_DEBUG_FLAG="-g"
         F77_OTHER_FLAG=""
         F77_LD="$F77 -L$flibdir -lU77 -lfio -lf77math -lf90math"
         F77_PIC_FLAG="-fpic"
         ;;
     *g95)
         F77_DBL_FLAG="-N11"
         F77_OPT_FLAG="-O2"
         F77_NOOPT_FLAG="-O0"
         F77_DEBUG_FLAG="-g"
         F77_OTHER_FLAG="-fno-second-underscore"
         ;;
     *gfortran)
         F77_DBL_FLAG=""
         F77_OPT_FLAG="-O2"
         F77_NOOPT_FLAG="-O0"
         F77_DEBUG_FLAG="-g"
         F77_OTHER_FLAG="-fno-second-underscore"
         ;;
     *)
         F77_DBL_FLAG=""
         F77_OPT_FLAG="-O"
         F77_NOOPT_FLAG=""
         F77_DEBUG_FLAG="-g"
         F77_OTHER_FLAG=""
         ;;
	esac
	;;

 *apple-darwin*)
  case $F77 in
     *xlf90)
         F77_DBL_FLAG="-autodouble "
         F77_OPT_FLAG="-O2"
         F77_NOOPT_FLAG="-O0"
         dnl F77_OPT_FLAG="-O2 -tpp7 -xW"  dnl Pentium 4
         F77_DEBUG_FLAG="-g"
         F77_OTHER_FLAG="-cm -w -l32"    dnl Quiet
         F77_PIC_FLAG="-qpic"
         ;;
     *Absoft*)
         dnl SEK: Not sure about some of these flags because the naming
         dnl      scheme is so screwed up
         F77_INCLUDE_FLAG="-p"
         F77_DBL_FLAG="-N11"
         F77_OPT_FLAG="-O -B100 -cpu:g4"
         F77_NOOPT_FLAG="-O0"
         F77_DEBUG_FLAG="-g"
         F77_OTHER_FLAG="-YEXT_NAMES=LCS -YEXT_SFX=_"
         F77_LD="$F77 -lU77 -lfio -lf77math -lf90math"
         ;;
     *)
         F77_DBL_FLAG=""
         F77_OPT_FLAG="-O"
         F77_NOOPT_FLAG=""
         F77_DEBUG_FLAG="-g"
         F77_OTHER_FLAG=""
         ;;
	esac
	 ;;

 hppa*-hp-hpux*)
      F77_LD="$F77 +z"
	;;
 alpha*-cray-unicos*)
      ;;

 *-*-aix*)
      F77_DBL_FLAG="-qrealsize=8"
      F77_OPT_FLAG="-O3 -qstrict -qarch=pwr3 -qtune=pwr3 -qcache=auto -qmaxmem=-1"
      F77_NOOPT_FLAG="-qnoopt"
      dnl F77_OPT_FLAG="-O2 -tpp7 -xW"
      F77_DEBUG_FLAG="-g -qcheck -qsigtrap -qflttrap"
      dnl F77_OTHER_FLAG="-q32 -qfixed -qxlf90=autodealloc"
      F77_OTHER_FLAG="-qxlf90=autodealloc" dnl autodealloc default on most compilers"
      F77_PIC_FLAG="-qpic"
	;;
 *-sgi-irix6*)
      F77_DBL_FLAG="-r8 -d16"
      F77_OPT_FLAG="-O"
      F77_NOOPT_FLAG=""
      F77_DEBUG_FLAG="-g"
      F77_OTHER_FLAG=""
      F77_LDFLAGS=""
	;;


esac

dnl ----------------------------------------------------------------------
dnl Set up default flags by setting variables that haven't been defined
dnl ----------------------------------------------------------------------
if test -z "$F77_INCLUDE_FLAG"; then
    F77_INCLUDE_FLAG="-I"
fi

dnl Can't test on all platforms, so this could cause errors.
if test -z "$F77_INCLUDE_FLAG"; then
         F77_PIC_FLAG="-fpic"
fi

dnl ----------------------------------------------------------------------
dnl   Set flags based on input.  Debug flag overwrites optimization flag
dnl ----------------------------------------------------------------------

if test -n "$F77"; then
  if test -z "$F77_LD"; then
      F77_LD=$F77
  fi
  if test -z "$FFLAGS"; then
     FFLAGS="$F77_OTHER_FLAG"

     if test "$DOUBLE" = yes; then
        FFLAGS="$F77_DBL_FLAG $FFLAGS"
     fi

     case $OPTIMIZED in
       yes)
         FFLAGS="$FFLAGS $F77_OPT_FLAG"
         ;;
       full | ultra)
         AC_MSG_WARN(May generate code for this processor only.)
         FFLAGS="$FFLAGS $F77_OPT_FLAG $F77_MACH_FLAG"
         ;;
       noopt)
         FFLAGS="$FFLAGS $F77_NOOPT_FLAG"
         ;;
     esac

     if test "$DEBUG" = yes; then
        FFLAGS="$F77_DEBUG_FLAG $FFLAGS"
     fi
  fi
else
  case $F77 in
     *g77)
         F77_DBL_FLAG=""
         F77_OPT_FLAG="-O2"
         F77_NOOPT_FLAG="-O0"
         F77_DEBUG_FLAG="-g"
         F77_OTHER_FLAG="-fno-second-underscore"
         ;;
    dnl Add your own system here
     *)
         F77_DBL_FLAG=""
         F77_OPT_FLAG="-O"
         F77_NOOPT_FLAG=""
         F77_DEBUG_FLAG="-g"
         F77_OTHER_FLAG=""
         ;;
  esac
  if test -z "$LD"; then
      LD=$F77
  fi
  if test -z "$FFLAGS"; then


     FFLAGS="$F77_OTHER_FLAG"

     if test "$DOUBLE" = yes; then
        FFLAGS="$F77_DBL_FLAG $FFLAGS"
     fi

     case $OPTIMIZED in
       yes)
         FFLAGS="$FFLAGS $F77_OPT_FLAG"
         ;;
       full)
         FFLAGS="$FFLAGS $F77_OPT_FLAG $F77_FULLOPT_FLAG"
         ;;
       noopt)
         FFLAGS="$FFLAGS $F77_NOOPT_FLAG"
         ;;
     esac

     if test "$DEBUG" = yes; then
        FFLAGS="$F77_DEBUG_FLAG $FFLAGS"
     fi
  fi
fi

dnl ----------------------------------------------------------------------
dnl  AC_SUBST everything to allow fine-grained control of compilation
dnl ----------------------------------------------------------------------
AC_SUBST(FFLAGS)
AC_SUBST(F77_LD)
AC_SUBST(F77_INCLUDE_FLAG)
AC_SUBST(F77_DBL_FLAG)
AC_SUBST(F77_OPT_FLAG)
AC_SUBST(F77_MACH_FLAG)
AC_SUBST(F77_NOOPT_FLAG)
AC_SUBST(F77_DEBUG_FLAG)
AC_SUBST(F77_OTHER_FLAG)
