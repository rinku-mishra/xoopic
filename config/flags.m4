dnl ######################################################################
dnl
dnl File:	flags.m4
dnl
dnl Purpose:	To determine the flags for C and C++ compilation.  In the
dnl 		case where the C or C++ compiler is mpiCC, one must
dnl		first parse the name of the real compiler out of the
dnl		mpiCC script.
dnl
dnl Version:	$Id: flags.m4 2386 2007-09-05 22:58:04Z cary $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

dnl echo $ac_n "setting the flags per system and compiler... $ac_c" 1>&6
echo "Setting the flags per system and C++ compiler:" $CXX

#
# JRC 22May06: This section needs some rework.  It should key
# off of MPICXX and MPICC only.
#
# Version info found from http://www.fortran-2000.com/ArnaudRecipes/Version.html
#

#default values
SHAREDLIBFLAGS="-shared"
# Extract the real compiler
if test -n "$CXX"; then
  isabs=`echo $CXX | grep '^/'`
  if test -n "$isabs"; then
    ABSCXX=$CXX
  else
    AC_PATH_PROG(ABSCXX, $CXX)
  fi
  CXXBASE=`basename $ABSCXX`
  case $CXXBASE in

    mpiCC | mpicxx | mpic++)
      # echo "Looking for compiler in" $ABSCXX
      # SERIALCXX=`grep CCCBASE= $ABSCXX | sed 's/^.*=//' | sed 's/\"//g'`
      MPI_OPTS=`$ABSCXX -show`
      SERIALCXX=`echo $MPI_OPTS | sed 's/ .*$//'`
      dnl echo SERIALCXX = $SERIALCXX
      if test -z "$SERIALCXX"; then
	AC_MSG_ERROR(Unable to determine the real compiler inside of $ABSCXX)
      fi
      MPI_LIBDIR=`echo $MPI_OPTS | sed 's/^.*-L//' | sed 's/ .*$//'`
      dnl MPI_LIBDIR = $MPI_LIBDIR
      if test "$MPICHG2" = yes; then
        SERIALCXX=`grep TEST_GC= $SERIALCXX | sed 's/^.*=//' | sed 's/\"//g'`
      fi
      ;;

    mpCC)
      SERIALCXX=xlC
      ;;

    mpCC_r)
      SERIALCXX=xlC_r
      ;;

    mpiicpc)
      SERIALCXX=icpc
      ;;

    mpipathCC)
      SERIALCXX=pathCC
      ;;

    CC)
      SERIALCXX=CC
      ;;

    *)
      SERIALCXX=$CXXBASE
      ;;

  esac
  echo Serial C++ compiler is \`$SERIALCXX\'
else
  echo No C++ compiler specified
  SERIALCXX=nocxx
fi

# The following is used to make sure that e.g. pathscale is found
ABSSERIALCXX=`which $SERIALCXX`
SERIALCXX=`basename $SERIALCXX`

# Check for optimization
# OPTIMIZED=yes
AC_ARG_ENABLE(debug,
	AC_HELP_STRING([--enable-debug], [turn on debugging]),
	OPTIMIZED=debug, OPTIMIZED=yes)
AC_ARG_ENABLE(fulloptimize,
	AC_HELP_STRING([--enable-fulloptimize], [turn on full optimization]),
	OPTIMIZED=full, OPTIMIZED=$OPTIMIZED)
AC_ARG_ENABLE(ultraoptimize,
	AC_HELP_STRING([--enable-ultraoptimize], [turn on ultra optimization]),
	OPTIMIZED=ultra, OPTIMIZED=$OPTIMIZED)
dnl echo OPTIMIZED = $OPTIMIZED

# Check for other options
AC_ARG_ENABLE(profiling,
	AC_HELP_STRING([--enable-profiling], [turn on profiling]),
	PROFILING=yes, PROFILING=no)
AC_ARG_ENABLE(libhpm,
	AC_HELP_STRING([--enable-libhpm], [use libhpm performance counters]),
	LIBHPM=yes, LIBHPM=no)
AC_ARG_ENABLE(assert,
	AC_HELP_STRING([--enable-assert], [turn on assertion checking]),
	ASSERT=yes, ASSERT=no)

CRAY=""
CRAY_INSTANTIATE=""
export CRAY
case "$SERIALCXX" in

  *g++ | *g++3 | g++* | c++ | path* )
    CPPFLAGS=""
    CXXFLAGS="-Wall -Wno-unused"
    CXXDEPFLAGS="-M"
    CXXDEP="$CXX -M >depend.u"
    CXXPROFILE_FLAG=-pg
    if test "$PROFILING" = "yes"; then CXX_PROFILE_FLAG="-pg"; fi
    AR="ar cr"
    AR_FLAGS=" "
    ARFLAGS=" "

    case "$host" in
      rs6000-ibm-aix*)
        CXX_PIC_FLAG=" "
        ;;
      hppa*-hp-hpux*)
        CXX_PIC_FLAG="-fpic"
        ;;
      *)
        CXX_PIC_FLAG="-fPIC"
        ;;
    esac

# Get version to set compiler directory
    AC_MSG_CHECKING(g++ version)

#
# Detect the version of gcc by matching to an arbitrary prefix string,
# followed by a space and the sequence Digit[.Digit]+ and arbitrary trailing
# characters. The main problem is the version name of GCC for pathCC,
# which is sent as a multi line text to stderr(!) and is specified as
# GNU gcc version 3.3.1 (PathScale 2.1 driver).
# Note that we need the double '[[' in order to prevent m4 substitution
# of the '['. Use \{N,\} instead of \+ to be portable -- PM / 09/15/05
    CXX_VERSION=`$SERIALCXX --version 2>/dev/null | grep -i gcc |  sed -n 's/^\([[^.]]* \)*\([[0-9]]\{1,\}\(\.[[0-9]]\{1,\}\)\{1,\}\).*/\2/p'`
# keep the old method to determine the version, in case the new version fails
    if test -z "$CXX_VERSION"; then
      CXX_VERSION=`$SERIALCXX --version 2>/dev/null | sed -n '1p' | sed 's/^.*GCC. //'  | sed 's/ .*$//'`
    fi
# For pathscale
    if test -z "$CXX_VERSION"; then
      CXX_VERSION=`$SERIALCXX -v 2>&1 1>/dev/null | grep gcc | sed 's/^.*version //'`
      CXX_VERSION=`echo $CXX_VERSION | sed 's/ .*$//'`
    fi
    dnl echo CXX_VERSION = $CXX_VERSION
    AC_MSG_RESULT($CXX_VERSION)
    case $CXX_VERSION in
# JRC: These lines are in error.  All from version 4 has the same ABI.
      # 4.2*)
	# COMPDIR=gcc4.2
	# ;;
      # 4.1*)
	# COMPDIR=gcc4.1
	# ;;
      # 4.0*)
	# COMPDIR=gcc4.0
	# ;;
      4.*)
	COMPDIR=gcc4
	;;
      3.4*)
	COMPDIR=gcc3.4
	;;
      3.3*)
	COMPDIR=gcc3.3
# removed as it does not work with pathCC and is kind of arbitrary anyway
# (if we want to keep it, it should be architecture specific) -- PM 09/14/05
#	CXX_OPTIFLAGS="$CXX_OPTIFLAGS --param max-unrolled-insns=10"
	;;
      3.2*)
        COMPDIR=gcc3.2
        ;;
      3.1*)
        COMPDIR=gcc3.1
        ;;
      3.0*)
        COMPDIR=gcc3.0
        ;;
      2.96*)
        COMPDIR=gcc2.96
        ;;
      2.95* | pgcc-2.95*)
        COMPDIR=gcc2.95
	case $host in
	  *-*-solaris*)
	  AC_MSG_WARN(gcc2.95-solaris may not work. In that case try egcs1.1.2.)
	  ;;
	esac
        ;;
      egcs-2.91*)
        COMPDIR=egcs1.1
        ;;
      *)
        if test "$PACKAGE" = "ntcdata" -o "$PACKAGE" = "ntcphys" ; then
	  AC_MSG_ERROR(Ancient gcc.  Must be egcs1.1.1 or gcc2.95 or later.)
        else
          AC_MSG_WARN(Caution: version $ver is not known to work.)
          COMPDIR=gcc3.1
        fi
        ;;
    esac

# Set optimization flags
    dnl echo OPTIMIZED = $OPTIMIZED
    processor=`uname -p`
    if test "$processor" = unknown; then processor=`uname -m`; fi
    sys=`uname -s`
    case $OPTIMIZED in

      yes)
        CXX_OPTIFLAGS="-g -O2 -pipe"
        case "$processor" in
	  athlon)
            CXX_OPTIFLAGS="$CXX_OPTIFLAGS -march=athlon"
	    ;;
        esac
        dnl echo CXX_OPTIFLAGS = $CXX_OPTIFLAGS
        ;;

      full | ultra)
        CXX_OPTIFLAGS="-O3 -pipe"
        AC_MSG_WARN(C++ compiler may generate code for this processor only.)
	# echo sys = $sys
        case "$SERIALCXX" in
          path*)
# -O3 hasn't worked for several months (ICE), throttling down. JAC 01/25/06
            CXX_OPTIFLAGS="-O2 -OPT:ro=1:Olimit=0 -ffast-math"
            ;;
          *g++ | *g++3)
            CXX_OPTIFLAGS="$CXX_OPTIFLAGS -funroll-loops"
            ;;
        esac
        case "$sys" in
	  Linux)
	    case $processor in
	      athlon)
	        ismp=`cat /proc/cpuinfo | grep "model name" | grep " MP "`
	        if test -n "$ismp"; then processor="${processor}-mp"; fi
                CXX_OPTIFLAGS="$CXX_OPTIFLAGS -march=$processor"
		;;
	      i686)
	        isintel=`cat /proc/cpuinfo | grep "model name" | grep " Intel"`
	        if test -n "$isintel"; then
		  thisproc="pentium4"
	        else
		  isathlon=`cat /proc/cpuinfo | grep "model name" | grep " Athlon"`
	          if test -n "$isathlon"; then
		    thisproc="athlon"
	            ismp=`cat /proc/cpuinfo | grep "model name" | grep " MP "`
	            if test -n "$ismp"; then thisproc="${thisproc}-mp"; fi
		  fi
		fi
		if test -n "$thisproc"; then
                  CXX_OPTIFLAGS="$CXX_OPTIFLAGS -march=$thisproc"
		fi
		;;
	    esac
	    ;;
	  Darwin)
	    machine=`machine`
	    SHAREDLIBFLAGS="-dynamiclib"
	    case "$machine" in
	      ppc7450)
                CXX_OPTIFLAGS="$CXX_OPTIFLAGS -mcpu=7450"
	        ;;
	      ppc970)
                CXX_OPTIFLAGS="$CXX_OPTIFLAGS -mcpu=970"
	        ;;
	    esac
	    ;;
        esac
        case $ver in
          3.3*)
# removed, as it breaks pathCC and is kind of architecture specific anyway
# -- PM 09/14/05
#            CXX_OPTIFLAGS="$CXX_OPTIFLAGS --param max-unrolled-insns=10"
            case "$processor" in
	      athlon)
		# Since this is not working I am commenting it out - CPN 2/21/05
                # CXX_OPTIFLAGS="$CXX_OPTIFLAGS -mfpmath=sse" # sse not working?
	        ;;
            esac
            ;;
        esac
# Add options for ultra or fastmath
        if test $OPTIMIZED = ultra; then
          AC_MSG_WARN(May generate code that is not IEEE math compliant.)
          CXX_OPTIFLAGS="$CXX_OPTIFLAGS -ffast-math"
        fi
# Full optimize generates cpu specific code
        ;;

      *)
        CXX_OPTIFLAGS="-g -DDEBUG"
        ;;

    esac

# Check for -fsquangle
    AC_MSG_CHECKING(for -fsquangle)
    case "$host" in

      *-cray-unicos*)
        AC_MSG_RESULT(no)
        ;;

      *)
	# AC_PATH_PROG(gxxtmp, g++)
        absgxx=`echo $ABSCXX | sed "s/^.* //g"`
        # echo absgxx = $absgxx
        bincxxdir=`dirname "$absgxx" | sed "s/ //g"`
        topcxxdir=`dirname "$bincxxdir" | sed "s/ //g"`
        libcxxdir=$topcxxdir/lib
        # echo libcxxdir = $libcxxdir
        # sqngtst="$NM $libcxxdir/libstdc++.a* 2>/dev/null | grep t12basic_string3ZX01ZX11ZX21_RB2"
        # echo sqngtst = $sqngtst
        sqng=`$NM $libcxxdir/libstdc++.a* 2>/dev/null | grep t12basic_string3ZX01ZX11ZX21_RB2`
        # echo sqng = $sqng
        if test -n "$sqng"; then
          CXXFLAGS="$CXXFLAGS -fsquangle"
          AC_MSG_RESULT(yes)
        else
          AC_MSG_RESULT(no)
        fi
        ;;

    esac

    ;;

  *aCC)

    case "$host" in
      hppa*-hp-hpux*)
	CXX_VERSION=`aCC -V 2>&1 | sed "s/^.*A\.//"`
	minver=`echo $CXX_VERSION | sed "s/^.*\.//"`
	majver=`echo $CXX_VERSION | sed "s/\..*$//"`
        if test $majver -lt 3 -o $majver -eq 3 -a $minver -lt 13; then
          AC_MSG_WARN(aCC not tested for versions less than A.03.13 on HPUX11.00)
	fi
        CPPFLAGS=""
        CXX_PIC_FLAG="+z"
        CXXFLAGS="-Aa +w +W152,169,361,392,495,431,641,655,5004"
	# Tried adding +p, but this caused incorrect compilation failures
        CXXDEPFLAGS="+M"
        CXXDEP="$CXX +M >depend.u"
        AR="ar cq"
        AR_FLAGS=" "
        ARFLAGS=" "
        if test "$OPTIMIZED" = "debug"; then CXX_OPTIFLAGS="-g -DDEBUG"
        else CXX_OPTIFLAGS="-O"; fi
        COMPDIR=aCC
        ;;

      *)
        AC_MSG_ERROR(Compiler $CXX not supported for host $host.)
        ;;

    esac
    ;;

  KCC | mpKCC)
    CPPFLAGS=""
    CXX_PIC_FLAG=" "
    case "$host" in
      *86-pc-linux-*)
        CXXFLAGS="--one_instantiation_per_object --display_error_number --diag_suppress 837 -DHAS_BOOL_TYPE -D_OSF_SOURCE"
        CXXDEP="$CXX -M >depend.u"
        ;;
      *-cray-unicos*)
        CXXFLAGS="--one_instantiation_per_object --display_error_number --diag_suppress 837 -DHAS_BOOL_TYPE -D_XOPEN_SOURCE -D_OSF_SOURCE"
        CXXDEP="$CXX -M >depend.u"
        ;;
      *)
        CXXFLAGS="--one_instantiation_per_object --display_error_number --diag_suppress 837 -DHAS_BOOL_TYPE -D_XOPEN_SOURCE -D_OSF_SOURCE"
        CXXDEP="$CXX -M >depend.u"
        ;;
    esac
    CXXDEPFLAGS="-M"
    AR="KCC -o"
    AR_FLAGS=" "
    ARFLAGS=" "
    if test "$OPTIMIZED" = "debug"; then CXX_OPTIFLAGS="-g -DDEBUG"
    else CXX_OPTIFLAGS="+K3"; fi
    COMPDIR=KCC
    ;;

  cxx)

    CPPFLAGS=""
    CXX_PIC_FLAG=" "
    CXXDEPFLAGS="-M -noimplicit_include"
    CXXDEP="$CXX -M -noimplicit_include >depend.u"
    AR="ar cr"
    AR_FLAGS=" "
    ARFLAGS=" "
    COMPDIR=cxx

    case "$host" in
      alpha*-dec-osf*)
        CXXFLAGS="-msg_disable 846 -D__USE_STD_IOSTREAM -msg_disable narrowptr"
        CXX_REPOSITORY="cxx_repository/*.o"
        if test "$OPTIMIZED" = "debug"; then CXX_OPTIFLAGS="-g -DDEBUG"
        else CXX_OPTIFLAGS="-O3"; fi
	# Optimization does not work
        ;;

      alphaev6*-linux-*)
        CXXFLAGS=" -arch ev6 -tune ev6"
#        CXX_REPOSITORY="cxx_repository/*.o"
        if test "$OPTIMIZED" = "debug"; then CXX_OPTIFLAGS="-g -DDEBUG"
        else CXX_OPTIFLAGS="-O3"; fi
        ;;

      *)
        AC_MSG_ERROR(Compiler $CXX not supported for host $host.)
        ;;

    esac
    ;;

  CC | */CC )

    case "$host" in

      *-sgi-irix6*)
        CPPFLAGS=""
	CXX_PIC_FLAG=" "
	# changes 9 Oct 01 to get oopic to compile
	# CXXFLAGS="-n32 -exceptions -no_auto_include -D_PTHREADS -LANG:ansi-for-init-scope=ON -LANG:std -OPT:Olimit_opt=on"
	# CXXFLAGS="-n32 -exceptions -no_auto_include -D_PTHREADS -LANG:ansi-for-init-scope=ON -OPT:Olimit_opt=on"
	# Change from Irek for dxhdf5.  If causes problems elsewhere,
	# we need to test on the project
	CXXFLAGS="-exceptions -no_auto_include -LANG:std -LANG:ansi-for-\init-scope=ON -OPT:Olimit_opt=on -woff 1174,1552"
        CXXDEPFLAGS="-M"
        CXXDEP="$CXX -M"
        CXXDEP="$CXX -M >depend.u"
	AR="CC -ar -o"
	AR_FLAGS=" "
        ARFLAGS=" "
        if test "$OPTIMIZED" = "debug"; then CXX_OPTIFLAGS="-g -DDEBUG"
        else CXX_OPTIFLAGS="-O"; fi
        COMPDIR=CC
        ;;

      *-*-solaris*)
        CPPFLAGS=" "
	CXX_PIC_FLAG="-KPIC"
	CXXFLAGS="-D__EXTENSIONS__ -D_POSIX_SOURCE"
        CXXDEPFLAGS="-xM1"
        CXXDEP="$CXX -xM1 >depend.u"
        AR="CC -xar -o"
	AR_FLAGS=" "
        ARFLAGS=" "
        if test "$OPTIMIZED" = "debug"; then
	  CXX_OPTIFLAGS="-g -DDEBUG"
        else
	  CXX_OPTIFLAGS="-g -DDEBUG -O"
	  echo "Solaris CC may not work with optimization.  Also needs -g -DDEBUG"
	fi
        COMPDIR=CC
        ;;

      *-cray-unicos*)
        CXXDEPFLAGS="-M"
        CXXDEP="$CXX -M >depend.u"
        AR="ar cru"
        AR_FLAGS=" "
        ARFLAGS=" "
        if test "$OPTIMIZED" = "debug"; then CXX_OPTIFLAGS="-Gn"
        else CXX_OPTIFLAGS="-O3"
        fi
      	CRAY_INSTANTIATE="-h instantiate=all"
        CRAY=1
        ;;

      xt3-*)
        CXXDEPFLAGS="-M"
        CXXDEP="$CXX -M >depend.u"
	CXXFLAGS="-tp amd64"
        AR="ar cr"
        AR_FLAGS=" "
        ARFLAGS=" "
        if test "$OPTIMIZED" = "debug"
        then
                CXX_OPTIFLAGS="-g -DDEBUG"
        else
                CXX_OPTIFLAGS="-fastsse"
        fi
        ;;

      *)
        AC_MSG_ERROR(Compiler $CXX not supported for host $host.)
        ;;

    esac
    ;;

  pgCC)
    CPPFLAGS=""
    CXX_PIC_FLAG=" "
    case "$host" in
      *86-pc-linux-*)
        CXXFLAGS=""
        CXXDEP="$CXX -M >depend.u"
        ;;
      *)
        AC_MSG_ERROR(Compiler $SERIALCXX not supported for host $host.)
        ;;
    esac
    CXXDEPFLAGS="-M"
    AR="ar cr"
    AR_FLAGS=" "
    ARFLAGS=" "
    if test "$OPTIMIZED" = "debug"; then
      CXX_OPTIFLAGS="-g -DDEBUG"
    else
      CXX_OPTIFLAGS="-O -fast -Minfo -Mcache_align -Munroll -Mdalign -Minline -Mvect=prefetch"
    fi
    COMPDIR=pgCC
    ;;

  xlC | xlC_r | xlc++ | xlc++_r)
    SHAREDLIBFLAGS="-G"
    case "$SERIALCXX" in
      xlC)
        COMPDIR=xlC
        ;;
      xlC_r)
        COMPDIR=xlC_r
        ;;
      xlc++)
        COMPDIR=xlc++
        ;;
      xlc++_r)
        COMPDIR=xlc++_r
        ;;
    esac

# Universal flags
    CPPFLAGS=""
    CXX_PIC_FLAG=" "
    CXXDEPFLAGS="-M"
    CXXDEP="$CXX -M -E >/dev/null"
    AR="ar -X32_64 cr"
    AR_FLAGS=" "
    ARFLAGS=" "
    if test "$PROFILING" = "yes"; then CXX_PROFILE_FLAG="-g -pg"; fi
    dnl echo OPTIMIZED = $OPTIMIZED
    processor=`uname -p`
    verline=`lslpp -l | grep vacpp.cmp.core | grep COMMITTED`
    CXX_VERSION=`echo $verline | sed 's/ *vacpp.cmp.core *//' | sed 's/ .*$//'`

    case "$host" in

      rs6000-ibm-aix* | powerpc-ibm-aix4* | powerpc-ibm-aix5* )
	# CXXFLAGS="-qrtti -bmaxdata:0x80000000 -bmaxstack:0x80000000 -q64"
	CXXFLAGS="-qrtti -q64"
	# LDFLAGS="$LDFLAGS -bmaxdata:0x80000000 -bmaxstack:0x80000000 -q64 -L/usr/common/usg/zlib64/1.2.1/lib"
	LDFLAGS="$LDFLAGS -q64 -bbigtoc -L/usr/common/usg/zlib64/1.2.1/lib"
	if test "$LIBHPM" = "yes"; then CXXFLAGS="$CXXFLAGS -DLIBHPM -I/usr/common/usg/hpmtoolkit/2.4.2/include"; LDFLAGS="$LDFLAGS -L/usr/common/usg/hpmtoolkit/2.4.2/lib -lhpm -lpmapi"; fi

# Set optimization flags
        case $OPTIMIZED in

          yes)
            CXX_OPTIFLAGS="-O2 -qarch=auto -qtune=auto"
            ;;

          full)
            CXX_OPTIFLAGS="-O3 -qarch=auto -qtune=auto -qcache=auto"
            ;;

          ultra)
            CXX_OPTIFLAGS="-O5"
            ;;

          *)
            CXX_OPTIFLAGS="-g -DDEBUG -qflttrap"
            ;;

        esac
        ;;

      *)
        AC_MSG_ERROR(Compiler $CXX not supported for host $host.)
        ;;

      powerpc-*-darwin*)
	CXXFLAGS=""
	LDFLAGS="$LDFLAGS"

# Set optimization flags
        case $OPTIMIZED in

          yes)
            CXX_OPTIFLAGS="-O2"
            ;;

          full)
            CXX_OPTIFLAGS="-O4 -qaltivec"
            ;;

          ultra)
            CXX_OPTIFLAGS="-O5"
            ;;

          *)
            CXX_OPTIFLAGS="-g -DDEBUG -qflttrap"
            ;;

        esac
        ;;

      *)
        AC_MSG_ERROR(Compiler $CXX not supported for host $host.)
        ;;

    esac
    ;;

  ecc)

    CPPFLAGS=""
    CXXFLAGS="-mp"
    CXX_PIC_FLAG=" "
    CXXDEPFLAGS="-M"
    CXXDEP="$CXX -M >depend.u"
    AR="ar cr "
    AR_FLAGS=" "
    ARFLAGS=" "
    COMPDIR=ecc
    if test "$OPTIMIZED" = "debug"; then CXX_OPTIFLAGS="-O0 -g -DDEBUG"
    elif test "$OPTIMIZED" = "yes";  then CXX_OPTIFLAGS="-O2"
    elif test "$OPTIMIZED" = "full";  then CXX_OPTIFLAGS="-O3"; fi

    ;;

  icpc)
    CPPFLAGS=""
    CXXFLAGS="-Wall"
    CXXDEPFLAGS="-M"
    CXXDEP="$CXX -M >depend.u"
    CXXPROFILE_FLAG=-pg
    if test "$PROFILING" = "yes"; then CXX_PROFILE_FLAG="-pg"; fi
    icpcbindir=`dirname $ABSSERIALCXX`
    if test -f $icpcbindir/xiar; then
      AR="$icpcbindir/xiar cr"
    else
      AR="/usr/local/intel/bin/xiar cr"
    fi
    AR_FLAGS=" "
    ARFLAGS=" "
    COMPDIR=icpc
    AC_MSG_CHECKING(icpc version)
    CXX_VERSION=`$SERIALCXX --version 2>/dev/null | sed -n '1p' | sed 's/^.*ICC. //'  | sed 's/ .*$//'`
    AC_MSG_RESULT($CXX_VERSION)

# Set optimization flags
    processor=`uname -p`
    case $OPTIMIZED in
      yes)
        CXX_OPTIFLAGS="-O2 -pipe"
        ;;
      full | ultra)
        AC_MSG_WARN(May generate code for processor $processor only.)
	case $processor in
	  athlon)
            CXX_OPTIFLAGS="-fast -prefetch"
 	    ;;
	  *)
            CXX_OPTIFLAGS="-O3 -ip -static"
 	    ;;
	esac
        ;;
      *)
        CXX_OPTIFLAGS="-g -DDEBUG"
        ;;
    esac
    ;;

  nocxx)
    AC_MSG_WARN(No C++ compiler.)
    ;;

  *)
    AC_MSG_ERROR(Compiler $SERIALCXX not supported.)
    ;;

esac

if test $SERIALCXX != nocxx; then
  AC_SUBST(CRAY_INSTANTIATE)
  AC_SUBST(CXXDEPFLAGS)
  AC_SUBST(CXXDEP)
  AC_SUBST(CXX_PIC_FLAG)
  AC_SUBST(CXX_REPOSITORY)
  AC_SUBST(CXX_OPTIFLAGS)
  AC_SUBST(COMPDIR)
  CXXFLAGS="$CXXFLAGS $CXX_PROFILE_FLAG $CXX_OPTIFLAGS"
  AC_SUBST(CXX_FLAGS)
  AC_SUBST(AR)
  AC_SUBST(AR_FLAGS)
  AC_SUBST(ARFLAGS)
  AC_MSG_CHECKING(how to build libraries)
  if test -z "$AR"; then
    AC_MSG_ERROR(unknown!)
  else
    AC_MSG_RESULT(with $AR $AR_FLAGS)
  fi
fi
AC_SUBST(SERIALCXX)
AC_SUBST(MPI_LIBDIR)

##########
#
# Now find the flags for the C compiler
#
##########

# Extract the real compiler
# ABSCC=`$WHICH $CC | sed 's/ .*$//'`
isabs=`echo $CC | grep '^/'`
if test -n "$isabs"; then
  ABSCC=$CC
else
  AC_PATH_PROG(ABSCC, $CC)
fi
dnl echo ABSCC = $ABSCC
CCBASE=`basename $ABSCC`
dnl echo CCBASE = $CCBASE

case $CCBASE in

  mpicc)
    MPI_OPTS=`$ABSCC -show`
    dnl echo MPI_OPTS=$MPI_OPTS
    SERIALCC=`echo $MPI_OPTS | sed 's/ .*$//'`
    # echo SERIALCC = $SERIALCC
    if test "$MPICHG2" = yes; then
        SERIALCC=`grep GC= $SERIALCC | sed 's/^.*=//' | sed 's/\"//g'`
    fi

    ;;

  mpcc_r)
    SERIALCC=xlc_r
    ;;

  mpiicc)
    SERIALCC=icc
    ;;

  mpipathcc)
    SERIALCC=pathcc
    ;;

  *)
    SERIALCC=$CC
    ;;

esac
#The following is used to make sure that e.g. pathscale gets detected
SERIALCC=`basename $SERIALCC`

echo Serial C compiler is \`$SERIALCC\'

case "$SERIALCC" in

  */cc | cc | xlc_r | cc_r )

    case "$host" in

      rs6000-ibm-aix* )
        C_PIC_FLAG=" "
        CFLAGS="-bmaxdata:0x80000000 -bmaxstack:0x80000000"
        if test "$OPTIMIZED" = "debug"; then C_OPTIFLAGS="-g -DDEBUG"
        else C_OPTIFLAGS="-O"; fi
        ;;

      powerpc-ibm-aix4.* | powerpc-ibm-aix5.* )
        C_PIC_FLAG=" "
        CFLAGS=" -qcpluscmt "
        if test "$OPTIMIZED" = "debug"; then C_OPTIFLAGS="-g -DDEBUG"
        else C_OPTIFLAGS="-O2"; fi
        ;;

      hppa*-hp-hpux*)
	C_PIC_FLAG="+z"
	CFLAGS="-Aa -D_HPUX_SOURCE"
        if test "$OPTIMIZED" = "debug"; then C_OPTIFLAGS="-g -DDEBUG"
        else C_OPTIFLAGS="-O"; fi
        ;;

      *-linux*)

        C_PIC_FLAG="-fPIC"
        CFLAGS="-Wall -Wno-unused"
        # if test "$OPTIMIZED" = "debug"; then C_OPTIFLAGS="-g -DDEBUG"
        # else C_OPTIFLAGS="-O"; fi

        case $OPTIMIZED in
          yes)
            C_OPTIFLAGS="-O2 -g -pipe"
            ;;
          full | ultra)
            OPTIFLAGS="-O3 -pipe"
            ;;
          *)
            C_OPTIFLAGS="-g -DDEBUG"
            ;;
        esac
        ;;

      *-sgi-irix6*)
	C_PIC_FLAG=" "
	CFLAGS="-n32 -common -fullwarn -woff 1174,1552,1209,1506,3201"
        if test "$OPTIMIZED" = "debug"; then C_OPTIFLAGS="-g -DDEBUG"
        else C_OPTIFLAGS="-O"; fi
        ;;

      alpha*-dec-osf*)
	C_PIC_FLAG=" "
	CFLAGS=" "
        if test "$OPTIMIZED" = "debug"; then C_OPTIFLAGS="-g -DDEBUG"
        else C_OPTIFLAGS="-O"; fi
        ;;

      powerpc-*-darwin*)
        C_PIC_FLAG="-fpic"
        CFLAGS="-Wall -Wno-unused"
        if test "$OPTIMIZED" = "debug"; then C_OPTIFLAGS="-g -DDEBUG"
        else C_OPTIFLAGS="-O"; fi
        ;;

      *-*-solaris*)
	C_PIC_FLAG="-KPIC"
	CFLAGS="-D__EXTENSIONS__ -D_POSIX_SOURCE"
        if test "$OPTIMIZED" = "debug"; then C_OPTIFLAGS="-g -DDEBUG"
        else C_OPTIFLAGS="-O"; fi
        ;;

      *-cray-unicos*)
        CRAY=1
	C_PIC_FLAG=" "
	CFLAGS=" "
        if test "$OPTIMIZED" = "debug"; then C_OPTIFLAGS="-g -DDEBUG"
        else C_OPTIFLAGS="-O2"; fi
        ;;

      xt3-*)
        CRAY=
	C_PIC_FLAG=""
	CFLAGS="-tp amd64"
        if test "$OPTIMIZED" = "debug"
	then
		C_OPTIFLAGS="-g -DDEBUG"
        else
		C_OPTIFLAGS="-fastsse"
	fi
        ;;

      *)
        AC_MSG_ERROR(Compiler $SERIALCC not supported for host $host.)
        ;;

    esac
    ;;

  */gcc | gcc | gcc3 | gcc*| pathcc )
    case "$host" in
      rs6000-ibm-aix*)
        C_PIC_FLAG=" "
        ;;
      hppa*-hp-hpux*)
        C_PIC_FLAG="-fpic"
        ;;
      *-linux*)
        C_PIC_FLAG="-fPIC"
        ;;
      *)
        C_PIC_FLAG="-fPIC"
        ;;
    esac
    CFLAGS="-Wall -Wno-unused -Wno-uninitialized"
    if test "$PROFILING" = "yes"; then C_PROFILE_FLAG="-pg"; fi
    dnl if test "$OPTIMIZED" = "debug"; then C_OPTIFLAGS="-g -DDEBUG"
    dnl else C_OPTIFLAGS="-O2"; fi
    case $OPTIMIZED in
      yes)
        C_OPTIFLAGS="-O2 -g -pipe"
        ;;
      full | ultra)
        OPTIFLAGS="-O3 -pipe"
        ;;
      *)
        C_OPTIFLAGS="-g -DDEBUG"
        ;;
    esac
    ;;

  *icc)
    C_PIC_FLAG="-fPIC"
    case $OPTIMIZED in
      yes)
        C_OPTIFLAGS="-O2 -g -pipe"
        ;;
      full | ultra)
        OPTIFLAGS="-O3 -pipe"
        ;;
      *)
        C_OPTIFLAGS="-g -DDEBUG"
        ;;
    esac
    ;;

  *ccc)
    CFLAGS="-O2"
    ;;

  */pgcc | pgcc)
    case "$host" in

      *-linux*)
        C_PIC_FLAG="-fPIC"
        CFLAGS=""
        if test "$OPTIMIZED" = "debug"; then
	  C_OPTIFLAGS="-g -DDEBUG"
        else
	  C_OPTIFLAGS="-O"
  	fi
        ;;

      *)
        AC_MSG_ERROR(Compiler $SERIALCC not supported for host $host.)
        ;;

    esac
    CFLAGS=""
    if test "$PROFILING" = "yes"; then C_PROFILE_FLAG="-pg"; fi
    ;;

  *)
    AC_MSG_ERROR(Compiler $SERIALCC not supported.)
    ;;

esac

if test "$ASSERT" = "yes"; then CXXFLAGS="$CXXFLAGS -DASSERT"; fi

AC_SUBST(SERIALCC)
AC_SUBST(C_PIC_FLAG)
AC_SUBST(C_OPTIFLAGS)
CFLAGS="$CFLAGS $C_PROFILE_FLAG $C_OPTIFLAGS"
AC_SUBST(C_FLAGS)

dnl Following for runs to print
AC_DEFINE_UNQUOTED([CXX], "$SERIALCXX", "C++ Compiler to use")
if test -z "$CXX_VERSION"; then
  CXX_VERSION=unknown
  AC_MSG_WARN(C++ compiler version unknown.  Update flags.m4.)
fi
AC_DEFINE_UNQUOTED([CXX_VERSION], "$CXX_VERSION", "C++ Compiler version")
AC_DEFINE_UNQUOTED([CXX_OPTIFLAGS], "$CXX_OPTIFLAGS", "C++ Optimization flags")
AC_DEFINE_UNQUOTED([CXX_SHAREDLIBFLAGS],"$SHAREDLIBFLAGS","shared library flag")
AC_DEFINE_UNQUOTED([TOP_BUILDDIR], "$abs_top_builddir","top build dir")
AC_DEFINE_UNQUOTED([TOP_SRCDIR], "$abs_top_srcdir","top src dir")

# echo "finished setting the flags per system and compiler."
