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
dnl  F90-dependent variables:
dnl	F90_LIBS -- f90 runtime libraries needed for linking
dnl	F90_PYFORT -- F90 compiler name using conventions of PYFORT
dnl   F77_DEFINE  -- Flag to be used for ifdefs in C routines
dnl   F90_RTCHAR  -- Character string which can be used with grep and nm
dnl    to figure out what compiler the library was compiled with by
dnl    seeing what run-time libraries were used
dnl 
dnl  C-dependent variables:  (Based off of Tech-X's libs.m4 file)
dnl   CXX_LIBS    -- C++ LBIRARIES OT USE
dnl   RPATH  - Pass rpath to the linker needed for making shared libraries
dnl
dnl
dnl NOTES: 
dnl        o Testing is done on f90 executable not f77
dnl        o There is a method for dynamically determining the _
dnl          convention at the end of the file.  Currently commented out
dnl
dnl Version:	$Id: finterop.m4 2334 2007-07-18 02:30:16Z kuehn $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ----------------------------------------------------------------------
dnl F90-DEPENDENT VARIABLES:
dnl Check on host and f90 since it is possible to have same compiler on 
dnl different platforms with subtle differences
dnl ----------------------------------------------------------------------
case "$host" in
 xt3-*)
  F90_PYFORT="ftn"
  F77_DEFINE="-DPGF"
  FC_RPATH_FLAG=""
  FC_COMPAT_LIBDIR=""
  FC_COMPAT_LIBS=""
  F77_DEFINE="-DPGF"
  FC_RTCHAR=".C1"
  ;;

 *-linux-gnu)
  fbindir=`dirname $F90`
  flibdir="`dirname $fbindir`/lib"
  case $F90 in
     *lf95*)
         F90_PYFORT=Lahey-Fujitsu
         F90_LIBS="-L$flibdir -lfccx86_6a -lfj9e6 -lfj9f6 -lfj9fsse2 -lfj9i6 -lfst"
         F77_DEFINE="-DLF95"
         F90_RTCHAR="jwe_"
         ;;
     *pgf90)
         F90_PYFORT=pgf90
         F90_LIBS="-L$flibdir -lpgf90 -lpgf90_rpm1 -lpgf902 -lpgf90rtl -lpghpf -lpgftnrtl -lpgc"
         F77_DEFINE="-DPGF"
         ;;
     *ifort)
         # n.b. we assume gcc here
         libgccfilename=`$CXX -print-libgcc-file-name`
         F77_DEFINE="-DIFORT"
         ;;
     *ifc)
          F90_LIBS="-L$flibdir -lintrins  -Qy $flibdir/lib/icrt.link -lIEPCF90 -lPEPCF90 -lF90"
          F77_DEFINE="-DIFC"
         ;;
     *bsoft)
         F90_PYFORT=Absoft
         F90_LIBS="-L$flibdir -lfio -lf90math"
         F77_DEFINE="-DABSF"
         ;;
     *g95)
         F77_DEFINE="-DG95"
         F90_LIBS="-L$flibdir -lf95"
         ;;
     *)
         ;;
	esac
 ;;

 *apple-darwin*)
  case $F90 in
     *xlf90)
         F77_DEFINE="-DXLF"
	   ;;
     *Absoft*)
         F90_PYFORT=Absoft
         F90_LIBS="-L/Applications/Absoft/lib -lfio -lf90math"
         F77_DEFINE="-DABSF"
	   ;;
     *)
         ;;
	esac
 ;;

 *-*-aix*)
      F90_PYFORT=xlf90
      F90_LIBS="-L$flibdir -lxlf90 -lxlf"
      F77_DEFINE="-DXLF"
 ;;
    # Sun Don't know the test here
    *-*sun*)
      F90_PYFORT=solaris
      # Need to autodetermine the version to get the directory
      F90_LIBS="-L$flibdir -lfai -lfai2 -lfsumai -lfprodai -lfminlai -lfmaxlai -lfminvai -lfmaxvai -lfui -lfsu -lsunmath -lm -lc"
      ;;

 alpha*-dec-osf*)
       F90_PYFORT=alpha
       # F90_LIBS="-Wl,-qlshpf -lUfor -lfor -lFutil -lots"
       F90_LIBS="-L$flibdir -lUfor -lfor -lFutil -lots"
       if test "$OPTIMIZED" = "no"; then F90_OPTIFLAGS="-g"
       else F90_OPTIFLAGS="-O"; fi
       ;;

 hppa*-hp-hpux*)
       F90_PYFORT=hp
       F90_LIBS="-L$flibdir -lF90 -lU77"
       ;;
 alpha*-cray-unicos*)
	 ;;

 *-sgi-irix6*)
       F90_PYFORT=irix
       F90_LIBS="-L$flibdir -lfortran -lffio -lftn"
	 ;;
esac

dnl ----------------------------------------------------------------------
dnl  
dnl Just set the F77 equivalents to F90
dnl
dnl ----------------------------------------------------------------------

F77_LIBS=$F90_LIBS
F77_PYFORT=$F90_PYFORT
FORT_LIBS=$F77_LIBS

dnl ----------------------------------------------------------------------
dnl  
dnl Print messages and export
dnl
dnl ----------------------------------------------------------------------

AC_MSG_CHECKING(for F90 libraries )

AC_SUBST(F90_LIBS)
AC_SUBST(F90_PYFORT)
AC_SUBST(F90_RTCHAR)
AC_SUBST(F77_DEFINE)

AC_MSG_RESULT($F90_LIBS -- $F90_PYFORT)
dnl ----------------------------------------------------------------------
dnl C-DEPENDENT VARIABLES:
dnl This comes from config/libs.m4.  For most f90 codes, we don't need
dnl all the variables in there so ignore.  THIS PROBABLY NEEDS WORK.
dnl ----------------------------------------------------------------------
CXX_LIBS=
CC_LIBS=

case "$host" in
  *-darwin*)
    RPATH_FLAG="-L"
    case $CXX in
      *g++ | *c++ | *g++3)
        libgccfilename=`$CXX -print-libgcc-file-name`
        CXX_LIBS="-L`dirname $libgccfilename` -lstdc++"
        CC_LIBS=$CXX_LIBS
	;;
    esac
    case $CC in
      *gcc)
        libgccfilename=`$CC -print-libgcc-file-name`
        CC_LIBS="-L`dirname $libgccfilename`"
	;;
    esac
    ;;

  *-ibm-aix*)
    RPATH_FLAG=-L
    case $CXX in

      *g++)
        libgccfilename=`$CXX -print-libgcc-file-name`
        CXX_LIBS="-L`dirname $libgccfilename` -lstdc++"
	;;

      *xlc)
	;;

    esac
    case $CC in
      *gcc)
        libgccfilename=`$CC -print-libgcc-file-name`
        CC_LIBS="-L`dirname $libgccfilename`"
	;;
    esac
    ;;

  *-linux* | *-freebsd*)
    RPATH_FLAG="-Wl,-rpath,"
    case $CXX in
      *g++ | *c++)
  	  RPATH_FLAG="-Wl,-rpath,"
        libgccfilename=`$CXX -print-libgcc-file-name`
        CXX_LIBS="-L`dirname $libgccfilename` -lstdc++"
	;;
      *icc)
        iccpath=`type -p icc`
        iccbindir=`dirname $iccpath`
        icclibdir="`dirname $iccbindir`/lib"
        CXX_LIBS="-L$icclibdir -L/usr/lib -Bstatic -limf -Bdynamic -lm -Bstatic -Bdynamic -lcprts -lcxa -lunwind -Bdynamic -Bstatic -lirc -Bdynamic -lc -Bstatic -lirc $icclibdir/crtxn.o /usr/lib/crtn.o"
	;;
    esac
    case $CC in
      *g++ | *c++)
  	  RPATH_FLAG="-Wl,-rpath,"
        libgccfilename=`$CC -print-libgcc-file-name`
        CC_LIBS="-L`dirname $libgccfilename` -lstdc++"
	;;
      *icc)
        iccpath=`type -p icc`
        iccbindir=`dirname $iccpath`
        icclibdir="`dirname $iccbindir`/lib"
        CC_LIBS="-L$icclibdir -L/usr/lib -Bstatic -limf -Bdynamic -lm -Bstatic -Bdynamic -lcprts -lcxa -lunwind -Bdynamic -Bstatic -lirc -Bdynamic -lc -Bstatic -lirc $icclibdir/crtxn.o /usr/lib/crtn.o"
	;;
    esac
    ;;

  *-cygwin*)
    RPATH_FLAG="-Wl,-rpath,"
    case $CXX in
      *g++)
        libgccfilename=`$CXX -print-libgcc-file-name`
        CXX_LIBS="-L`dirname $libgccfilename` -lstdc++"
	;;
    esac
    ;;

  *-sgi-irix6*)
    case $CXX in
      CC)
	  RPATH_FLAG="-rpath "
	;;
      g++)
	  RPATH_FLAG=-Wl,-rpath,
        libgccfilename=`$CXX -print-libgcc-file-name`
        CXX_LIBS="-L`dirname $libgccfilename` -lstdc++"
	;;
    esac
    ;;

  *-*-solaris2.7* |  *-*-solaris2.8*)
    case $CXX in
      CC)
	  RPATH_FLAG=-Wl,-R,
	;;
      g++)
	  RPATH_FLAG=-Wl,-R,
        libgccfilename=`$CXX -print-libgcc-file-name`
        CXX_LIBS="-L`dirname $libgccfilename` -lstdc++"
	;;
    esac

    case $CC in
      cc | */cc)
	;;
      gcc | */gcc)
        libgccfilename=`$CC -print-libgcc-file-name`
        CC_LIBS="-L`dirname $libgccfilename` -lstdc++"
	;;
    esac
    ;;

  *-cray-unicos*)
    case $CXX in
      KCC)
        RPATH_FLAG="-L"
	;;
      CC)
        RPATH_FLAG="-L"
	;;
    esac
    ;;

  alpha*-dec-osf*)
    case $CXX in
      cxx)
        RPATH_FLAG="-rpath "
	;;
      *g++)
        RPATH_FLAG=-Wl,-rpath,
        libgccfilename=`$CXX -print-libgcc-file-name`
        CXX_LIBS="-L`dirname $libgccfilename` -lstdc++"
	;;
    esac

    case $CC in
      cc | */cc)
        RPATH_FLAG="-rpath "
	;;
      gcc | */gcc)
        libgccfilename=`$CC -print-libgcc-file-name`
        CC_LIBS="-L`dirname $libgccfilename` -lstdc++"
	;;
    esac
    ;;

  *-hp-hpux10*)
    case $CXX in
      aCC)
	;;
      g++)
	  RPATH_FLAG=-Wl,+b,
        libgccfilename=`$CXX -print-libgcc-file-name`
        CXX_LIBS="-L`dirname $libgccfilename` -lstdc++"
        CC_LIBS=$CXX_LIBS
	;;
    esac
    ;;

  *-hp-hpux11*)
    case $CXX in
      aCC)
	  RPATH_FLAG="+b "
	  CXX_LIBS="-lm"
	;;
      g++)
        libgccfilename=`$CXX -print-libgcc-file-name`
        CXX_LIBS="-L`dirname $libgccfilename` -lstdc++"
	;;
    esac
    case $CC in
      acc | */acc)
	  CXX_LIBS="-lm"
	;;
      gcc | */gcc)
        libgccfilename=`$CC -print-libgcc-file-name`
        CC_LIBS="-L`dirname $libgccfilename` -lstdc++"
	;;
    esac
    ;;

  *)
    AC_MSG_WARN(Libraries unknown for host $host.  Please notify the developers.)
    ;;

esac

AC_SUBST(CXX_LIBS)
AC_SUBST(CC_LIBS)
AC_SUBST(RPATH_FLAG)

dnl ----------------------------------------------------------------------
dnl
dnl Determine conventions for names in compiled libraries.  We assume that
dnl compilers can handle underscores in names.
dnl 
dnl ----------------------------------------------------------------------

dnl COMMENTING OUT FOR NOW!!!
dnl 
dnl AC_LANG_FORTRAN77
dnl cat <<EOF >dummy.f
dnl       subroutine dummyfcn()
dnl       return
dnl       end
dnl       subroutine dummy_fcn()
dnl       return
dnl       end
dnl EOF
dnl # echo Test compiling.
dnl # echo $F77 -o dummy.o -c dummy.f 2>/dev/null
dnl # $F77 -o dummy.o -c dummy.f 
dnl $F77 -o dummy.o -c dummy.f 2>/dev/null
dnl dummyfcn_entry=`$NM dummy.o | grep -i dummyfcn`
dnl dummy_fcn_entry=`$NM dummy.o | grep -i dummy_fcn`
dnl 
dnl AC_MSG_CHECKING(for capitalization of fortran routine names)
dnl lc=`echo $dummy_fcn_entry 2>/dev/null | grep dummy`
dnl if test -n "$lc"; then
dnl   DUMMYFCN=dummyfcn
dnl   DUMMY_FCN=dummy_fcn
dnl   capf77names=""
dnl   AC_MSG_RESULT(no)
dnl else
dnl   DUMMYFCN=DUMMYFCN
dnl   DUMMY_FCN=DUMMY_FCN
dnl   capf77names=yes
dnl   AC_DEFINE(F77_NMS_CAPITALIZED)
dnl   AC_MSG_RESULT(yes)
dnl fi
dnl 
dnl AC_MSG_CHECKING(for underscore in fortran routine names)
dnl res=`echo "$dummy_fcn_entry" | grep ${DUMMY_FCN}__`
dnl if test -n "$res"; then
dnl   AC_DEFINE(F77_NMS_HAVE_DOUBLE_POST_USCORE)
dnl   AC_MSG_RESULT(doublepost)
dnl   underscore=doublepost
dnl else
dnl   res=`echo "$dummy_fcn_entry" | grep ${DUMMY_FCN}_ `
dnl   if test -n "$res"; then
dnl     AC_DEFINE(F77_NMS_HAVE_POST_USCORE)
dnl     AC_MSG_RESULT(post)
dnl     underscore=post
dnl   else
dnl     srch=_${DUMMY}
dnl     res=`echo $dummy_fcn_entry 2>/dev/null | grep _${DUMMY_FCN} `
dnl     if test -n "$res"; then
dnl       AC_DEFINE(F77_NMS_HAVE_PRE_USCORE)
dnl       AC_MSG_RESULT(pre)
dnl       underscore=pre
dnl     else
dnl       AC_DEFINE(F77_NMS_HAVE_NO_USCORE)
dnl       AC_MSG_RESULT(none)
dnl       underscore=none
dnl     fi
dnl   fi
dnl fi
dnl rm dummy.o
dnl 
dnl # Put it all together in a single variable - the F77_COMPILER_TYPE
dnl # Not clear whether this is correct when strings are present
dnl AC_MSG_CHECKING(F77 compiler type)
dnl if test -n "$capf77names"; then
dnl   case $underscore in
dnl     doublepost)
dnl       AC_MSG_ERROR(Unknown fortran name convention: capitalized-pre.)
dnl       ;;
dnl     post)
dnl       AC_MSG_ERROR(Unknown fortran name convention: capitalized-post.)
dnl       ;;
dnl     pre)
dnl       AC_MSG_ERROR(Unknown fortran name convention: capitalized-pre.)
dnl       ;;
dnl     none)
dnl       F77_COMPILER_TYPE=T3E
dnl       ;;
dnl   esac
dnl else
dnl   case $underscore in
dnl     doublepost)
dnl       F77_COMPILER_TYPE=GNU
dnl       ;;
dnl     post)
dnl       F77_COMPILER_TYPE=SOL
dnl       ;;
dnl     pre)
dnl       AC_MSG_ERROR(Unknown fortran name convention: not capitalized-pre.)
dnl       ;;
dnl     none)
dnl       F77_COMPILER_TYPE=HP700
dnl       ;;
dnl   esac
dnl fi
dnl 
dnl AC_MSG_RESULT($F77_COMPILER_TYPE)
dnl AC_SUBST(F77_COMPILER_TYPE)
dnl rm -f dummy.f
