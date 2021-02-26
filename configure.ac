dnl ######################################################################
dnl
dnl File:       XOOPIC configure.in
dnl
dnl Purpose:    input file for autoconf for making configure
dnl
dnl Version:    $Id: configure.in 2301 2007-01-05 21:57:20Z hyunkim $
dnl             changes, JK, 2013-08-07
dnl
dnl ######################################################################

dnl Process this file with autoconf to produce a configure script.

AC_INIT(xg/xgmain.cpp, 2.70)
AC_CONFIG_AUX_DIR(./config)
AM_INIT_AUTOMAKE()
AM_CONFIG_HEADER(./config.h)
abs_top_builddir=`pwd`
AC_SUBST(abs_top_builddir)
abs_top_srcdir=`dirname $0`
cd $abs_top_srcdir
abs_top_srcdir=`pwd`
OOPIC_TOP_SRCDIR=$abs_top_srcdir
AC_SUBST(OOPIC_TOP_SRCDIR)
cd $abs_top_builddir

# try to turn off auto-invocation of autotools in the top-level Makefile
AM_MAINTAINER_MODE

# Default prefix
AC_PREFIX_DEFAULT(/usr/local/xoopic)


# Define stuff from former acconfig.in
AH_TEMPLATE([HAVE_NO_CONST_TYPE_CONVERSION_OVERLOAD],[Define if your c++ compiler can't overload const type conversions.])
AH_TEMPLATE([HAVE_NO_MUTABLE],[Define if your c++ compiler doesn't know mutable.])
AH_TEMPLATE([HAVE_NAMESPACES],[Define if your c++ compiler has namespaces.])
AH_TEMPLATE([HAVE_NO_RTTI],[Define if your c++ compiler does not have RTTI])
AH_TEMPLATE([HAVE_STD_COMPLEX],[Define if your c++ compiler has complex in the namespace std])
AH_TEMPLATE([HAVE_STD_STREAMS],[Define if your c++ compiler has streams in the namespace std])
AH_TEMPLATE([HAVE_SSTREAM],[Define if you use the <sstream> header file.])
AH_TEMPLATE([HOSTTYPE],[Define if you need the host type])
AH_TEMPLATE([TEMPLATE_FRIENDS_NEED_BRACKETS],
			[Define if your c++ compiler requires brackets in declarations 
			of template friends.])
AH_TEMPLATE([HAVE_NONTYPE_TEMPLATE_OPERATORS],[Define if your c++ compiler can handle nontyped templated operators])
AH_TEMPLATE([HAVE_HDF5],[Define if you have HDF5])
AH_TEMPLATE([HAVE_FFT],[Define if you have installed an fft lib])
AH_TEMPLATE([HAVE_DFFTW],[Define if you have the dfftw.h header file.])
AH_TEMPLATE([HAVE_SFFTW],[Define if you have the sfftw.h header file.])

dnl ######################################################################
dnl
dnl Use wrapped automake
dnl
dnl ######################################################################

# See whether we have automake
amver=`automake --version 2>/dev/null`
if test $? != 0; then
  echo automake not present in your path.
  echo Modifications to Makefile.am\'s will not propagate.
else
  AUTOMAKE=$abs_top_srcdir/config/automake.sh
fi
# For backware compatibility
EXEEXT=""
AC_SUBST(EXEEXT)

dnl ######################################################################
dnl
dnl Standard tests
dnl
dnl ######################################################################

AC_CANONICAL_HOST
hosttype=$host
AC_DEFINE_UNQUOTED(HOSTTYPE, "$host")

dnl ######################################################################
dnl
dnl Using hdf5?  If so, then find it.
dnl
dnl ######################################################################

dnl For now, make using hdf5 an option

AC_ARG_ENABLE(hdf5,[  --disable-hdf5   disable HDF5], hdf5="$enableval", hdf5=yes)
echo using HDF5? ... $hdf5
if test $hdf5 = yes; then
  builtin(include,config/hdf5.m4)
  if test $ac_cv_have_hdf5 = no; then 
dnl    AC_MSG_ERROR(hdf5 not found -- must install it or use flag --disable-hdf5.) 
    AC_MSG_WARN(hdf5 not properly installed -- Turning off hdf5.)
    HDF5_DIR=""
    HDF5_LIB=""
    HDF5_LIBDIR="."
    HDF5_INC=""
    HDF5_INCDIR="."

    AC_SUBST(HDF5_DIR)
    AC_SUBST(HDF5_INC)
    AC_SUBST(HDF5_LIB)
    AC_SUBST(HDF5_INCDIR)
    AC_SUBST(HDF5_LIBDIR)

  fi 
else
  HDF5_INC=""
  HDF5_LIB=""
  HDF5_INCDIR="."
  HDF5_LIBDIR="."

  AC_SUBST(HDF5_INC)
  AC_SUBST(HDF5_LIB)
  AC_SUBST(HDF5_INCDIR)
  AC_SUBST(HDF5_LIBDIR)
fi

dnl ######################################################################
dnl
dnl Choose C and C++ compilers
dnl
dnl ######################################################################

# If the user specifies nothing, then we default to g++/gcc, allowing the
# user's path to determine which version.
CXX=g++
CC=gcc

# Allow user to override choice of compiler
AC_ARG_WITH(CXX,[  --with-CXX=<compiler>   to use <compiler> (CC, cxx, KCC) instead of g++],CXX="$withval",CXX=$CXX)
echo Using C++ compiler $CXX
export CXX
AC_ARG_WITH(CC,[  --with-CC=<compiler>    to use <compiler> (cc) instead of gcc],CC="$withval",CC=$CC)
echo Using C compiler $CC
export CC

dnl ######################################################################
dnl
dnl Standard tests
dnl
dnl ######################################################################

AC_ISC_POSIX
AC_HEADER_TIME

dnl ######################################################################
dnl
dnl Auxiliary compilation flags
dnl
dnl ######################################################################

# Need BSD compatible nm
builtin(include,config/nm.m4)
builtin(include,config/flags.m4)
# AC_ARG_WITH(CXXOPTIFLAGS,[  --with-CXXOPTIFLAGS=<optimization flags> (e.g. -O)],CXXOPTIFLAGS="$withval",CXXOPTIFLAGS=$CXXOPTIFLAGS)
CXXFLAGS="$CXXFLAGS $CXX_OPTIFLAGS"
CFLAGS="$CFLAGS $C_OPTIFLAGS"
# Add link flags
builtin(include,config/libs.m4)

dnl ######################################################################
dnl
dnl Allow for extra flags
dnl
dnl ######################################################################

# Check for extra flags
AC_ARG_WITH(EXTRA_CXXFLAGS,[  --with-EXTRA_CXXFLAGS=<flags>   to add <flags> to c++ compilation], EXTRA_CXXFLAGS="$with
val",EXTRA_CXXFLAGS="$EXTRA_CXXFLAGS")
CXXFLAGS="$CXXFLAGS $EXTRA_CXXFLAGS $MPI_CXXFLAGS"
# echo CXXFLAGS= $CXXFLAGS
CFLAGS="$CFLAGS $MPI_CXXFLAGS"
AC_ARG_WITH(EXTRA_LDFLAGS,[  --with-EXTRA_LDFLAGS=<flags>    to add <flags> to linking], EXTRA_LDFLAGS="$withval",EXTRA_LDFLAGS="$EXTRA_LDFLAGS")
LDFLAGS="$LDFLAGS $EXTRA_LDFLAGS"
dnl AC_SUBST(LDFLAGS)

dnl ######################################################################
dnl
dnl Check for C, C++ compilers
dnl
dnl ######################################################################

AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CXXCPP

dnl ######################################################################
dnl
dnl Check for header files
dnl
dnl ######################################################################

AC_LANG_SAVE
AC_LANG_CPLUSPLUS
dnl AC_CHECK_HEADERS(iostream strstream fstream sstream)
AC_CHECK_HEADERS
AC_LANG_RESTORE

dnl ######################################################################
dnl
dnl Check for typedefs, structures, and compiler characteristics
dnl
dnl ######################################################################

AC_TYPE_SIZE_T
AC_STRUCT_TM

dnl ######################################################################
dnl
dnl C++ compiler capabilities
dnl
dnl ######################################################################

echo Calling config/macros.m4
builtin(include,config/macros.m4)
echo Calling config/cxx.m4
builtin(include,config/cxx.m4)
echo config/cxx.m4 finished

dnl ######################################################################
dnl
dnl MPI capabilities
dnl Get compilers used by mpi, then flags
dnl
dnl ######################################################################

AC_ARG_ENABLE(MPI,[  --enable-MPI          to use MPI], MPI=yes, MPI=no)

if test "$MPI" = yes; then
  HAS_F77=NO
  HAS_F90=NO
  builtin(include,config/mpicompilers.m4)
fi

dnl ######################################################################
dnl
dnl Get compiler to use based on whether mpi requested
dnl
dnl ######################################################################

DEFINE_MPI=""
if test $MPI = yes; then
  CXXCOMP="$MPICXX"
  DEFINE_MPI=-DMPI_VERSION;
else
  CXXCOMP="$CXX"
fi
AC_SUBST(CXXCOMP)
AC_SUBST(DEFINE_MPI)

dnl ######################################################################
dnl
dnl Check if debugging should be turned on for MPI runs
dnl
dnl ######################################################################

DEFINE_MPI_DEBUG=""
if test $MPI = yes; then
  AC_ARG_ENABLE(MPI_DEBUG,[  --enable-MPI_DEBUG      to enable debugging output in MPI from different processes to go to different files], MPI_DEBUG=yes, MPI_DEBUG=no)
  echo "MPI_DEBUG = $MPI_DEBUG"
  if test "$MPI_DEBUG" = yes; then
    DEFINE_MPI_DEBUG=-DMPI_DEBUG
  fi
fi
AC_SUBST(DEFINE_MPI_DEBUG)

dnl ######################################################################
dnl
dnl Check for purify
dnl
dnl ######################################################################
# Check for purify flags

AC_ARG_WITH(purify,[  --with-purify=<flags>  to create a purify executable ], 
       PURIFY_DEFINE="-DPURIFY_DEFINE",
       PURIFY_DEFINE="")
AC_SUBST(PURIFY_DEFINE)
# echo PURIFY_DEFINE=$PURIFY_DEFINE
if test ! -z "$PURIFY_DEFINE"; then
  AC_CHECKING(purify installation)
  AC_PATH_PROGS(PURIFY_PATH,purify,"",$PATH)
  if test -z "$PURIFY_PATH"; then
    echo " " 
    echo "WARNING: Cannot find purify"
    echo "Please add purify to your path"
  fi
fi

dnl ######################################################################
dnl
dnl Check for programs
dnl
dnl ######################################################################

AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB

dnl ######################################################################
dnl
dnl Find out what the library suffix is
dnl
dnl ######################################################################

AC_MSG_CHECKING(what the library suffix is)
AC_SUBST(LIBEXT)
if test -z "$LIBEXT"; then
    LIBEXT=".a"
fi
AC_MSG_RESULT($LIBEXT)

dnl ######################################################################
dnl
dnl Find out how to install libraries
dnl
dnl ######################################################################

AC_MSG_CHECKING(how to install libraries)
AC_SUBST(INSTALL_LIBRARY)
if test -z "$INSTALL_LIBRARY"; then
    if test "$LIBEXT" = ".sl"; then # HP needs executable shared libs
        INSTALL_LIBRARY="$INSTALL_PROGRAM"
    else
        INSTALL_LIBRARY="$INSTALL_DATA"
    fi
else
    INSTALL_LIBRARY="$INSTALL_DATA"
fi
AC_MSG_RESULT(with $INSTALL_LIBRARY)


dnl ######################################################################
dnl
dnl Check XGmini option, set NOX, set X11 flags
dnl
dnl ######################################################################

AC_ARG_WITH(XGmini,[  --with-XGmini=yes               to configure without X],
NOX=1,NOX=0)
export NOX
DEFINE_NOX=""
if test "$NOX" = "1"; then
  DEFINE_NOX="-DNOX"
fi
AC_SUBST(DEFINE_NOX)

dnl ######################################################################
dnl
dnl X11 
dnl
dnl ######################################################################

if test "$NOX" = "0"; then
  builtin(include,config/x11.m4)
fi

dnl ######################################################################
dnl
dnl find XPM and set flags
dnl
dnl ######################################################################

if test "$NOX" = "0"; then
  builtin(include,config/xpm.m4)
fi

dnl ######################################################################
dnl
dnl find XGrafix 
dnl
dnl ######################################################################
builtin(include,config/xgrafix.m4)

dnl ######################################################################
dnl
dnl find Tcl/TK
dnl
dnl ######################################################################

if test "$NOX" = "0"; then
  builtin(include,config/tcltk.m4)
fi

dnl ######################################################################
dnl
dnl FFTW capabilities
dnl
dnl ######################################################################

ac_cv_disable_fftw="no"
AC_ARG_ENABLE([fftw],
              [  --disable-fftw compile without fftw (default is to use fftw)],
              [ac_cv_disable_fftw=yes], [ac_cv_disable_fftw=no])
if test "$ac_cv_disable_fftw" = "yes"; then
  echo "Building with fftw is disabled"
fi
if test "$ac_cv_disable_fftw" = "no"; then
  echo "Checking for fftw..."
  if test "$SCALAR_DOUBLE"; then
    FFTW_PRECISION="double"
  else
    FFTW_PRECISION="float"
  fi
  builtin(include,config/fftw.m4)
fi

dnl ######################################################################
dnl
dnl Make all the Makefiles
dnl
dnl ######################################################################

echo Making Makefiles

AC_CONFIG_FILES(
 Makefile
 otools/Makefile
 advisor/Makefile
 physics/Makefile
 xg/Makefile
 config/Makefile
)

AC_OUTPUT

dnl ######################################################################
dnl
dnl Add in the .depend files.
dnl
dnl ######################################################################

touch otools/.depend
touch physics/.depend
touch advisor/.depend
touch xg/.depend
