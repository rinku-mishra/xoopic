dnl ######################################################################
dnl
dnl File:	finterop.m4
dnl
dnl Purpose:
dnl   	Determine things needed for interoperability of fortran
dnl         with other languages
dnl
dnl  The variables defined here are in two groups:
dnl     1) Things that depend on the fortran compiler used, and
dnl     2) things that depend on C compiler used.
dnl  When mixing fortan and C, one usually needs one or the other.
dnl  For example, if one is linking mixed code with fortran compiler,
dnl  then one needs the C runtime libraries.  If one is linking using
dnl  a C compiler, then one needs the fortran runtime libraries.
dnl
dnl  FC-dependent variables:
dnl	FCLIBS -- f90 runtime libraries needed for linking
dnl  F77_DEFINE  -- Flag to be used for ifdefs in C routines
dnl  FC_RTCHAR  -- Character string which can be used with grep and nm
dnl    to figure out what compiler the library was compiled with by
dnl    seeing what run-time libraries were used
dnl  FC_COMPAT_LIBS -- libraries need to link fortran into a C executable
dnl
dnl  C-dependent variables:  (Based off of Tech-X's libs.m4 file)
dnl   CXX_LIBS    -- C++ LIBRARIES TO USE
dnl   RPATH  - Pass rpath to the linker needed for making shared libraries
dnl
dnl NOTES:
dnl        o For shared libraries, FC_PIC_FLAG is set in fcflags.m4
dnl        o Note that the AC_FC_LIBRARY_LDFLAGS macro is supposed to be
dnl	       used to detect FCLIBS
dnl        o FC is the autotools awful variable name for any fortran
dnl          version beyond F77.  We use it interchangably with f90
dnl        o Testing is done on f90 executable not f77
dnl        o There is a method for dynamically determining the _
dnl          convention at the end of the file.  Currently commented out
dnl        o See autoconf info file for excellent discussion of many of
dnl          the issues
dnl
dnl Version:	$Id: fcinterop.m4 2334 2007-07-18 02:30:16Z kuehn $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ----------------------------------------------------------------------
dnl FC-DEPENDENT VARIABLES:
dnl Check on host and f90 since it is possible to have same compiler on
dnl different platforms with subtle differences
dnl ----------------------------------------------------------------------
case "$host" in

 xt3-*)
  FC_RPATH_FLAG=""
  FC_COMPAT_LIBDIR=""
  FC_COMPAT_LIBS=""
  F77_DEFINE="-DPGF"
  FC_RTCHAR=".C1"
  ;;

 *-linux-gnu)
  dnl echo ABSFC = $ABSFC
  dnl fbindir=`dirname $FC`
  fbindir=`dirname $ABSFC`
  flibdir="`dirname $fbindir`/lib"
  FC_LIBDIR="$flibdir"
  FC_RPATH_FLAG="-L"
  case $FC in
     *lf95*)
         FCLIBS="-L$flibdir -lfccx86_6a -lfj9e6 -lfj9f6 -lfj9fsse2 -lfj9i6 -lfst"
         F77_DEFINE="-DLF95"
         FC_RTCHAR="jwe_"
         FC_RPATH_FLAG="-Wl,-rpath,"
         ;;
     *pgf90)
         FCLIBS="-lpgf90 -lpgf90_rpm1 -lpgf902 -lpgf90rtl -lpghpf -lpgftnrtl -lpgc"
         F77_DEFINE="-DPGF"
	 FC_RTCHAR=".C1_"
         ;;
     *ifort)
         # n.b. we assume gcc here
         libgccfilename=`$CXX -print-libgcc-file-name`
         F77_DEFINE="-DIFORT"
         ;;
     *ifc)
         FCLIBS="-lintrins  -Qy $flibdir/lib/icrt.link -lIEPCF90 -lPEPCF90 -lF90"
         F77_DEFINE="-DIFC"
         ;;
     *bsoft)
         FCLIBS="-lfio -lf90math"
         F77_DEFINE="-DABSF"
         ;;
     *gfortran)
# This is a bit tricky, as the name of the Fortran standard library
# depends on the compiler version or Linux distribution.
        FC_LINKLINE=`gfortran -v foo.f 2>&1 | grep Driving`
        FCLIBS=""
	FC_COMPAT_LIBS=""
        for i in $FC_LINKLINE; do
	  case $i in
	    -lgfortranbegin) ;;
	    -lgf*) 
		fclibname=`echo ${i}.a | sed 's/-l/lib/g'`
		fcompatlib=`gfortran -print-file-name=$fclibname`
        	FC_COMPAT_LIB=`basename $fcompatlib`
        	FC_COMPAT_LIBDIR=`dirname $fcompatlib`
		FCLIBS="$FLIBS -L${FC_COMPAT_LIBDIR} $i"
	        FC_COMPAT_LIBS="$FC_COMPAT_LIBS $i"
		;;
	  esac
        done
	F77_DEFINE="-DGFORTRAN"
	FC_RTCHAR="_gfortran"
	;;
     *g95)
         g95libdir=`g95 -print-libgcc-file-name`
         flibdir="`dirname $g95libdir`"
         F77_DEFINE="-DG95"
         FC_COMPAT_LIBS="-lf95"
         FCLIBS="-L$flibdir -lf95"
         ;;
     *g77)
         F77_DEFINE=""
         fcompatlib=`g77 -print-file-name=libg2c.a`
         FC_COMPAT_LIB=`basename $fcompatlib`
         FC_COMPAT_LIBDIR=`dirname $fcompatlib`
         FC_COMPAT_LIBS="-lg2c"
         FCLIBS="-L$FC_COMPAT_LIBDIR -lg2c"
         FC_RTCHAR="_g77"
         ;;
     *)
         ;;
	esac
 ;;

 *apple-darwin*)
  case $FC in
     *xlf90)
         F77_DEFINE="-DOSXIBMF90"
         FCLIBS="-lxlf90 -lxlopt -lxlfmath"
	   ;;
     *Absoft*)
	 FC_LIBDIR="/Applications/Absoft/lib"
	 FC_RPATH_FLAG="-L"
         FCLIBS="-L/Applications/Absoft/lib -lfio -lf90math"
         F77_DEFINE="-DABSF"
	   ;;
     *gfortran)
        FC_LINKLINE=`gfortran -v foo.f 2>&1 | grep Driving`
        FCLIBS=""
	FC_COMPAT_LIBS=""
        for i in $FC_LINKLINE; do
	  case $i in
	    -lgfortranbegin) ;;
	    -lgf*) 
		fclibname=`echo ${i}.a | sed 's/-l/lib/g'`
		fcompatlib=`gfortran -print-file-name=$fclibname`
        	FC_COMPAT_LIB=`basename $fcompatlib`
        	FC_COMPAT_LIBDIR=`dirname $fcompatlib`
		FCLIBS="$FLIBS -L${FC_COMPAT_LIBDIR} $i"
	        FC_COMPAT_LIBS="$FC_COMPAT_LIBS $i"
		;;
	  esac
        done
	F77_DEFINE="-DGFORTRAN"
	FC_RTCHAR="_gfortran"
	;;
     *)
         ;;
	esac
 ;;

 *-*-aix*)
      FCLIBS="-lxlf90 -lxlf"
      FC_RPATH_FLAG="-L"
      F77_DEFINE="-DXLF"
 ;;
    # Sun Don't know the test here
    *-*sun*)
      # Need to autodetermine the version to get the directory
      FCLIBS="-lfai -lfai2 -lfsumai -lfprodai -lfminlai -lfmaxlai -lfminvai -lfmaxvai -lfui -lfsu -lsunmath -lm -lc"
      ;;

 alpha*-dec-osf*)
       # FCLIBS="-Wl,-qlshpf -lUfor -lfor -lFutil -lots"
       FCLIBS="-lUfor -lfor -lFutil -lots"
       if test "$OPTIMIZED" = "no"; then FC_OPTIFLAGS="-g"
       else FC_OPTIFLAGS="-O"; fi
       ;;

 hppa*-hp-hpux*)
       FCLIBS="-lF90 -lU77"
       ;;
 alpha*-cray-unicos*)
	 ;;

 *-sgi-irix6*)
       FCLIBS="-lfortran -lffio -lftn"
	 ;;
esac

dnl ----------------------------------------------------------------------
dnl
dnl Print messages and export
dnl
dnl ----------------------------------------------------------------------

AC_MSG_CHECKING(for F90 libraries )

AC_SUBST(FCLIBS)
dnl JRC: for backwards compatibility
if test -n "$FCLIBS" -a -n "$FC_LIBDIR"; then
  FC_LIBS='$(RPATH_FLAG)'"$FC_LIBDIR $FCLIBS"
fi
AC_SUBST(FC_LIBS)
AC_SUBST(FC_RTCHAR)
AC_SUBST(F77_DEFINE)
AC_SUBST(FC_LIBDIR)
AC_SUBST(FC_RPATH_FLAG)

dnl For linking fortran into C
AC_SUBST(FC_COMPAT_LIBDIR)
AC_SUBST(FC_COMPAT_LIBS)
AC_SUBST(FCLIBS)

dnl JRC: removing.  one should include libs.m4.
dnl dnl ----------------------------------------------------------------------
dnl dnl C-DEPENDENT VARIABLES:
dnl dnl This comes from config/libs.m4.  For most f90 codes, we don't need
dnl dnl all the variables in there so ignore.  THIS PROBABLY NEEDS WORK.
dnl dnl ----------------------------------------------------------------------
dnl CXX_LIBS=
dnl CC_LIBS=
dnl
dnl case "$host" in
dnl   *-darwin*)
dnl     RPATH_FLAG="-L"
dnl     case $CXX in
dnl       *g++ | *c++ | *g++3)
dnl         libgccfilename=`$CXX -print-libgcc-file-name`
dnl         libgccdir=dirname $libgccfilename
dnl         libupgccdir=dirname $libgccdir
dnl         libupdir=dirname $libupgccdir
dnl         libdir=dirname $libupgccdir
dnl         CXX_LIBS="-L$libdir -L$libgccdir -lstdc++"
dnl         CC_LIBS=$CXX_LIBS
dnl 	;;
dnl     esac
dnl     case $CC in
dnl       *gcc)
dnl         libgccfilename=`$CC -print-libgcc-file-name`
dnl         CC_LIBS="-L`dirname $libgccfilename`"
dnl 	;;
dnl     esac
dnl     ;;
dnl
dnl   *-ibm-aix*)
dnl     RPATH_FLAG=-L
dnl     case $CXX in
dnl
dnl       *g++)
dnl         libgccfilename=`$CXX -print-libgcc-file-name`
dnl         libgccdir=dirname $libgccfilename
dnl         libupgccdir=dirname $libgccdir
dnl         libupdir=dirname $libupgccdir
dnl         libdir=dirname $libupgccdir
dnl         CXX_LIBS="-L$libdir -L$libgccdir -lstdc++"
dnl         CC_LIBS=$CXX_LIBS
dnl 	;;
dnl
dnl       *xlc)
dnl 	;;
dnl
dnl     esac
dnl     case $CC in
dnl       *gcc)
dnl         libgccfilename=`$CC -print-libgcc-file-name`
dnl         CC_LIBS="-L`dirname $libgccfilename`"
dnl 	;;
dnl     esac
dnl     ;;
dnl
dnl   *-linux* | *-freebsd*)
dnl     RPATH_FLAG="-Wl,-rpath,"
dnl     case $CXX in
dnl       *g++ | *c++)
dnl   	  RPATH_FLAG="-Wl,-rpath,"
dnl         libgccfilename=`$CXX -print-libgcc-file-name`
dnl         libgccdir=`dirname $libgccfilename`
dnl         libupgccdir=`dirname $libgccdir`
dnl         libupdir=`dirname $libupgccdir`
dnl         otherlibdir=`dirname $libupdir`
dnl         CXX_LIBS="-L$otherlibdir -L$libgccdir -lstdc++"
dnl 	;;
dnl       *icc)
dnl         iccpath=`type -p icc`
dnl         iccbindir=`dirname $iccpath`
dnl         icclibdir="`dirname $iccbindir`/lib"
dnl         CXX_LIBS="-L$icclibdir -L/usr/lib -Bstatic -limf -Bdynamic -lm -Bstatic -Bdynamic -lcprts -lcxa -lunwind -Bdynamic -Bstatic -lirc -Bdynamic -lc -Bstatic -lirc $icclibdir/crtxn.o /usr/lib/crtn.o"
dnl 	;;
dnl     esac
dnl     case $CC in
dnl       *g++ | *c++)
dnl   	  RPATH_FLAG="-Wl,-rpath,"
dnl         libgccfilename=`$CC -print-libgcc-file-name`
dnl         CC_LIBS="-L`dirname $libgccfilename` -lstdc++"
dnl 	;;
dnl       *icc)
dnl         iccpath=`type -p icc`
dnl         iccbindir=`dirname $iccpath`
dnl         icclibdir="`dirname $iccbindir`/lib"
dnl         CC_LIBS="-L$icclibdir -L/usr/lib -Bstatic -limf -Bdynamic -lm -Bstatic -Bdynamic -lcprts -lcxa -lunwind -Bdynamic -Bstatic -lirc -Bdynamic -lc -Bstatic -lirc $icclibdir/crtxn.o /usr/lib/crtn.o"
dnl 	;;
dnl     esac
dnl     ;;
dnl
dnl   *-cygwin*)
dnl     RPATH_FLAG="-Wl,-rpath,"
dnl     case $CXX in
dnl       *g++)
dnl         libgccfilename=`$CXX -print-libgcc-file-name`
dnl         libgccdir=dirname $libgccfilename
dnl         libupgccdir=dirname $libgccdir
dnl         libupdir=dirname $libupgccdir
dnl         libdir=dirname $libupgccdir
dnl         CXX_LIBS="-L$libdir -L$libgccdir -lstdc++"
dnl 	;;
dnl     esac
dnl     ;;
dnl
dnl   *-sgi-irix6*)
dnl     case $CXX in
dnl       CC)
dnl 	  RPATH_FLAG="-rpath "
dnl 	;;
dnl       g++)
dnl 	  RPATH_FLAG=-Wl,-rpath,
dnl         libgccfilename=`$CXX -print-libgcc-file-name`
dnl         libgccdir=dirname $libgccfilename
dnl         libupgccdir=dirname $libgccdir
dnl         libupdir=dirname $libupgccdir
dnl         libdir=dirname $libupgccdir
dnl         CXX_LIBS="-L$libdir -L$libgccdir -lstdc++"
dnl 	;;
dnl     esac
dnl     ;;
dnl
dnl   *-*-solaris2.7* |  *-*-solaris2.8*)
dnl     case $CXX in
dnl       CC)
dnl 	  RPATH_FLAG=-Wl,-R,
dnl 	;;
dnl       g++)
dnl 	  RPATH_FLAG=-Wl,-R,
dnl         libgccfilename=`$CXX -print-libgcc-file-name`
dnl         libgccdir=dirname $libgccfilename
dnl         libupgccdir=dirname $libgccdir
dnl         libupdir=dirname $libupgccdir
dnl         libdir=dirname $libupgccdir
dnl         CXX_LIBS="-L$libdir -L$libgccdir -lstdc++"
dnl 	;;
dnl     esac
dnl
dnl     case $CC in
dnl       cc | */cc)
dnl 	;;
dnl       gcc | */gcc)
dnl         libgccfilename=`$CC -print-libgcc-file-name`
dnl         CC_LIBS="-L`dirname $libgccfilename` -lstdc++"
dnl 	;;
dnl     esac
dnl     ;;
dnl
dnl   *-cray-unicos*)
dnl     case $CXX in
dnl       KCC)
dnl         RPATH_FLAG="-L"
dnl 	;;
dnl       CC)
dnl         RPATH_FLAG="-L"
dnl 	;;
dnl     esac
dnl     ;;
dnl
dnl   alpha*-dec-osf*)
dnl     case $CXX in
dnl       cxx)
dnl         RPATH_FLAG="-rpath "
dnl 	;;
dnl       *g++)
dnl         RPATH_FLAG=-Wl,-rpath,
dnl         libgccfilename=`$CXX -print-libgcc-file-name`
dnl         libgccdir=dirname $libgccfilename
dnl         libupgccdir=dirname $libgccdir
dnl         libupdir=dirname $libupgccdir
dnl         libdir=dirname $libupgccdir
dnl         CXX_LIBS="-L$libdir -L$libgccdir -lstdc++"
dnl 	;;
dnl     esac
dnl
dnl     case $CC in
dnl       cc | */cc)
dnl         RPATH_FLAG="-rpath "
dnl 	;;
dnl       gcc | */gcc)
dnl         libgccfilename=`$CC -print-libgcc-file-name`
dnl         CC_LIBS="-L`dirname $libgccfilename` -lstdc++"
dnl 	;;
dnl     esac
dnl     ;;
dnl
dnl   *-hp-hpux10*)
dnl     case $CXX in
dnl       aCC)
dnl 	;;
dnl       g++)
dnl 	  RPATH_FLAG=-Wl,+b,
dnl         libgccfilename=`$CXX -print-libgcc-file-name`
dnl         libgccdir=dirname $libgccfilename
dnl         libupgccdir=dirname $libgccdir
dnl         libupdir=dirname $libupgccdir
dnl         libdir=dirname $libupgccdir
dnl         CXX_LIBS="-L$libdir -L$libgccdir -lstdc++"
dnl         CC_LIBS=$CXX_LIBS
dnl 	;;
dnl     esac
dnl     ;;
dnl
dnl   *-hp-hpux11*)
dnl     case $CXX in
dnl       aCC)
dnl 	  RPATH_FLAG="+b "
dnl 	  CXX_LIBS="-lm"
dnl 	;;
dnl       g++)
dnl         libgccfilename=`$CXX -print-libgcc-file-name`
dnl         libgccdir=`dirname $libgccfilename`
dnl         libupgccdir=`dirname $libgccdir`
dnl         libupdir=`dirname $libupgccdir`
dnl         otherlibdir=`dirname $libupgccdir`
dnl         CXX_LIBS="-L$otherlibdir -L$libgccdir -lstdc++"
dnl 	;;
dnl     esac
dnl     case $CC in
dnl       acc | */acc)
dnl 	  CXX_LIBS="-lm"
dnl 	;;
dnl       gcc | */gcc)
dnl         libgccfilename=`$CC -print-libgcc-file-name`
dnl         CC_LIBS="-L`dirname $libgccfilename` -lstdc++"
dnl 	;;
dnl     esac
dnl     ;;
dnl
dnl   *)
dnl     AC_MSG_WARN(Libraries unknown for host $host.  Please notify the developers.)
dnl     ;;
dnl
dnl esac
dnl
dnl AC_SUBST(CXX_LIBS)
dnl AC_SUBST(CC_LIBS)
dnl AC_SUBST(RPATH_FLAG)
dnl
