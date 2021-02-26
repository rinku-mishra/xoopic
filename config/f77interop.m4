dnl ######################################################################
dnl
dnl File:	f77interop.m4
dnl
dnl Purpose:
dnl   	Determine things needed for interoperability of fortran
dnl         with other languages
dnl
dnl  The variables defined here are in two groups:
dnl     1) Things that depend on the fortran compiler used, and
dnl     2) things that depend on C compiler used.
dnl  When mixing fortran and C, one usually needs one or the other.
dnl  For example, if one is linking mixed code with fortran compiler,
dnl  then one needs the C runtime libraries.  If one is linking using
dnl  a C compiler, then one needs the fortran runtime libraries.
dnl
dnl  F77-dependent variables:
dnl	FLIBS -- f77 runtime libraries needed for linking
dnl	F77_PYFORT -- F77 compiler name using conventions of PYFORT
dnl   F77_DEFINE  -- Flag to be used for ifdefs in C routines
dnl   F77_RTCHAR  -- Character string which can be used with grep and nm
dnl    to figure out what compiler the library was compiled with by
dnl    seeing what run-time libraries were used
dnl
dnl  C-dependent variables:  (Based off of Tech-X's libs.m4 file)
dnl   CXX_LIBS    -- C++ LIBRARIES OT USE
dnl   RPATH  - Pass rpath to the linker needed for making shared libraries
dnl
dnl
dnl NOTES:
dnl        o Note that the AC_F77_LIBRARY_LDFLAGS macro is supposed to be
dnl	       used to detect FLIBS
dnl        o There is a method for dynamically determining the _
dnl          convention at the end of the file.  Currently commented out
dnl          See autoconf manual for pre-defined methods for doing this
dnl
dnl Version:	$Id: f77interop.m4 2334 2007-07-18 02:30:16Z kuehn $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ----------------------------------------------------------------------
dnl F77-DEPENDENT VARIABLES:
dnl Check on host and f90 since it is possible to have same compiler on
dnl different platforms with subtle differences
dnl ----------------------------------------------------------------------

case "$host" in

  xt3-*)
    F77_PYFORT="pgf90"
    F77_DEFINE="-DPGF"
    F77_RTCHAR=".C1_"
    ;;

  *-linux-gnu)
    fbindir=`dirname $F77`
    flibdir="`dirname $fbindir`/lib"
    case $F77 in
      *lf95*)
        F77_PYFORT=Lahey-Fujitsu
        FLIBS="-L$flibdir -lfccx86_6a -lfj9e6 -lfj9f6 -lfj9fsse2 -lfj9i6 -lfst"
        F77_DEFINE="-DLF95"
        F77_RTCHAR="jwe_"
        ;;
      *pgf90)
        F77_PYFORT=pgf90
        FLIBS="-L$flibdir -lpgf90 -lpgf90_rpm1 -lpgf902 -lpgf90rtl -lpghpf -lpgftnrtl -lpgc"
        F77_DEFINE="-DPGF"
	F77_RTCHAR=".C1_"
        ;;
      *pgf77)
        F77_PYFORT=pgf77
        FLIBS="-L$flibdir -lpgftnrtl -lpghpf -lpgftnrtl -lpgc"
        F77_DEFINE="-DPGF"
	F77_RTCHAR=".C1_"
        ;;
      *ifort)
        # n.b. we assume gcc here
        libgccfilename=`$CXX -print-libgcc-file-name`
        F77_DEFINE="-DIFORT"
        ;;
      *ifc)
        FLIBS="-L$flibdir -lintrins  -Qy $flibdir/lib/icrt.link -lIEPCF90 -lPEPCF90 -lF90"
        F77_DEFINE="-DIF77"
        ;;
      *bsoft)
        F77_PYFORT=Absoft
        FLIBS="-L$flibdir -lfio -lf90math"
        F77_DEFINE="-DABSF"
        ;;
      *g77 | *g95)
        F77_DEFINE="-DG77"
        g2clib=`$F77 -print-file-name=libg2c.a`
        g2clibdir=`dirname $g2clib`
        FLIBS="-L$g2clibdir -lg2c"
	F77_RTCHAR="_g77"
        ;;
      *gfortran)
        F77_LINKLINE=`gfortran -v foo.f 2>&1 | grep Driving`
        FLIBS=""
	F77_COMPAT_LIBS=""
        for i in $F77_LINKLINE; do
	  case $i in
	    -lgfortranbegin) ;;
	    -lgf*)
		f77libname=`echo ${i}.a | sed 's/-l/lib/g'`
		f77compatlib=`gfortran -print-file-name=$f77libname`
        	F77_COMPAT_LIB=`basename $f77compatlib`
        	F77_COMPAT_LIBDIR=`dirname $f77compatlib`
		FLIBS="$FLIBS -L${F77_COMPAT_LIBDIR} "'$(RPATH_FLAG)'"${F77_COMPAT_LIBDIR} $i"
        	F77_COMPAT_LIBS="$F77_COMPAT_LIBS $i"
		;;
	  esac
        done
	F77_DEFINE="-DGFORTRAN"
	F77_RTCHAR="_gfortran"
	;;
      *)
        ;;
    esac
    ;;

  *apple-darwin*)
    case $F77 in
      *xlf90)
        F77_DEFINE="-DXLF"
	;;
      *Absoft*)
        F77_PYFORT=Absoft
        FLIBS="-L/Applications/Absoft/lib -lfio -lf90math"
        F77_DEFINE="-DABSF"
	;;
      *gfortran)
# This is a bit tricky, as the name of the Fortran standard library
# depends on the compiler version or Linux distribution.
        F77_LINKLINE=`gfortran -v foo.f 2>&1 | grep Driving`
        FLIBS=""
	F77_COMPAT_LIBS=""
        for i in $F77_LINKLINE; do
	  case $i in
	    -lgfortranbegin) ;;
	    -lgf*)
		f77libname=`echo ${i}.a | sed 's/-l/lib/g'`
		f77compatlib=`gfortran -print-file-name=$f77libname`
        	F77_COMPAT_LIB=`basename $f77compatlib`
        	F77_COMPAT_LIBDIR=`dirname $f77compatlib`
		FLIBS="$FLIBS -L${F77_COMPAT_LIBDIR} $i"
        	F77_COMPAT_LIBS="$F77_COMPAT_LIBS $i"
		;;
	  esac
        done
	F77_DEFINE="-DGFORTRAN"
	F77_RTCHAR="_gfortran"
	;;
      *)
        FLIBS="/sw/lib/libg2c.a"
        ;;
    esac
    ;;

  *-*-aix*)
    fbindir=`dirname $F77`
    flibdir="`dirname $fbindir`/lib"
    F77_PYFORT=xlf90
    # FLIBS="-L$flibdir -lxlf90 -lxlf"
    FLIBS="-L$flibdir -lxlf90"
    F77_DEFINE="-DXLF"
    ;;

# Sun Don't know the test here
  *-*sun*)
    F77_PYFORT=solaris
    # Need to autodetermine the version to get the directory
    FLIBS="-L$flibdir -lfai -lfai2 -lfsumai -lfprodai -lfminlai -lfmaxlai -lfminvai -lfmaxvai -lfui -lfsu -lsunmath -lm -lc"
    ;;

  alpha*-dec-osf*)
    F77_PYFORT=alpha
    # FLIBS="-Wl,-qlshpf -lUfor -lfor -lFutil -lots"
    FLIBS="-L$flibdir -lUfor -lfor -lFutil -lots"
    if test "$OPTIMIZED" = "no"; then F77_OPTIFLAGS="-g"
    else F77_OPTIFLAGS="-O"; fi
    ;;

  hppa*-hp-hpux*)
    F77_PYFORT=hp
    FLIBS="-L$flibdir -lF90 -lU77"
    ;;

  alpha*-cray-unicos*)
    ;;

  *-sgi-irix6*)
    F77_PYFORT=irix
    FLIBS="-L$flibdir -lfortran -lffio -lftn"
    ;;

esac

dnl ----------------------------------------------------------------------
dnl
dnl Print messages and export
dnl
dnl ----------------------------------------------------------------------

AC_MSG_CHECKING(for F77 libraries)

AC_SUBST(FLIBS)
AC_SUBST(F77_PYFORT)
AC_SUBST(F77_RTCHAR)
AC_SUBST(F77_DEFINE)
AC_SUBST(F77_COMPAT_LIBDIR)

dnl AC_MSG_RESULT($FLIBS -- $F77_PYFORT)
AC_MSG_RESULT($FLIBS)
