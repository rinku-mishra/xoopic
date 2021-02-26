dnl ######################################################################
dnl
dnl File:	libs.m4
dnl
dnl Purpose:	Determine how to build libraries, esp. shared
dnl
dnl Version:	$Id: libs.m4 2367 2007-08-14 13:52:25Z cary $
dnl
dnl Copyright 2003-2005 Tech-X Corporation.  This file may be freely
dnl redistributed and modified provided the above copyright remains.
dnl
dnl ######################################################################

dnl echo $ac_n "setting the methods for building libraries... $ac_c" 1>&6
# echo libs.m4: host = $host

# Needed by automake 1.5.  Should be done on
# individual basis
OBJEXT=o

case "$host" in
  xt3-*)
    NET_LIBS=""
    THREAD_LIB=""
    DB_LIB=""
    DYNLINK_LIB=""
    ZLIB_DIR=""
    LINK_STATIC=""
    LINK_DYNAMIC=""
    RPATH_FLAG="-L"
    HAVE_BUNDLES=false
    LINKCMODULE="$CC"
    LINKCMODULE_FLAGS=""
    LINKCXXMODULE="$CXX"
    LINKCXXMODULE_FLAGS=""
    LDWITHMODULE="$CXX"
    LDWITHMODULE_FLAGS=""
    ;;
  alpha*-dec-osf*)
    NET_LIBS="-lbsd"
    THREAD_LIB="-lpthread"
    DB_LIB="-ldb"
    DYNLINK_LIB=""
    ZLIB_DIR=""
    SO=".so"
    HAVE_BUNDLES=false

    case $SERIALCXX in
      cxx)
	PIC_FLAG=" "
        RPATH_FLAG="-rpath "
        LINKCXXMODULE="$CXX -shared -expect_unresolved \"*\""
        LDWITHMODULE="$CXX "
        LDWITHMODULE_FLAGS=" "
	;;
      g++)
	PIC_FLAG="-fPIC"
        RPATH_FLAG=-Wl,-rpath,
        LINKCXXMODULE="$CXX -shared -Wl,-expect_unresolved,\"*\""
        LINKCXXMODULE_FLAGS="-shared -Wl,-expect_unresolved,\"*\""
        LDWITHMODULE="$CXX"
        LDWITHMODULE_FLAGS=" "
	;;
      KCC)
	PIC_FLAG=" "
        RPATH_FLAG="-rpath "
        LINKCXXMODULE="$CXX -shared "
        LDWITHMODULE="$CXX "
        LDWITHMODULE_FLAGS=" "
	;;
    esac

    case $SERIALCC in
      cc | */cc)
        LINKCMODULE="$CC -shared -expect_unresolved \"*\""
	;;
      gcc | */gcc)
        LINKCMODULE="$CC -shared -Wl,-expect_unresolved,\"*\""
        LINKCMODULE_FLAGS="-shared -Wl,-expect_unresolved,\"*\""
	;;
    esac
    ;;

  *-darwin*)
    NET_LIBS=""
    THREAD_LIB="-lpthread"
    DB_LIB="-ldb"
    DYNLINK_LIB="-ldl"
    ZLIB_DIR=""
    SO=".dylib"
    LINK_STATIC="-Wl,-static"
    LINK_DYNAMIC="-Wl,-dynamic"
    RPATH_FLAG="-L"
    HAVE_BUNDLES=true
    case $SERIALCXX in
      *g++ | *c++ | *g++3)
	PIC_FLAG="-fpic"
        LINKCMODULE="$CC -shared "
        LINKCMODULE_FLAGS="-shared "
        LINKCXXMODULE="$CXX -shared "
        LINKCXXMODULE_FLAGS="-shared "
# The following flags are unknown as of 12 July 02
        # LDWITHMODULE="$CXX -Wl,-export-dynamic"
        # LDWITHMODULE_FLAGS="-Wl,-export-dynamic"
        LDWITHMODULE="$CXX "
        LDWITHMODULE_FLAGS=""
	;;
    esac
    ;;

  *-hp-hpux10*)
    NET_LIBS="-lnsl -ldld"
    THREAD_LIB="-lpthread"
    DB_LIB="-ldbm"
    DYNLINK_LIB="-lnsl -ldld"
    SO=".sl"
    HAVE_BUNDLES=false

    case $SERIALCXX in
      aCC)
        ver=`aCC -V 2>&1 | sed "s/^.*A\.//"`
        minver=`echo $ver | sed "s/^.*\.//"`
        majver=`echo $ver | sed "s/\..*$//"`
        if test $majver -lt 3 -o $majver -eq 3 -a $minver -lt 13; then
          AC_MSG_WARN(aCC not tested for versions less than A.03.13)
        fi
	;;
      g++)
	PIC_FLAG="-fpic"
	RPATH_FLAG=-Wl,+b,
        gccspecdir=`$CC -v 2>&1 | grep specs | sed -e "s/^.* //"`
        gcclibdir=`echo $gccspecdir | sed -e "s/\/specs//"`
	CXXMODULELIBS="-L$gcclibdir -lstdc++"
        LINKCXXMODULE="ld -b"
        LDWITHMODULE="$CXX -Wl,-E -Wl,+s"
        LDWITHMODULE_FLAGS="-Wl,-E -Wl,+s"
	;;
    esac
    ;;

  *-hp-hpux11*)
    NET_LIBS="-lnet"
    THREAD_LIB=" "
    # No working thread lib on hp?
    DB_LIB="-ldbm"
    DYNLINK_LIB="-lnsl -ldld"
    ZLIB_DIR=""
    SO=".sl"
    HAVE_BUNDLES=false

    case $SERIALCXX in
      aCC)
        ver=`aCC -V 2>&1 | sed "s/^.*A\.//"`
        minver=`echo $ver | sed "s/^.*\.//"`
        majver=`echo $ver | sed "s/\..*$//"`
        if test $majver -lt 3 -o $majver -eq 3 -a $minver -lt 13; then
          AC_MSG_WARN(aCC not tested for versions less than A.03.13)
        fi
	PIC_FLAG="+z"
	RPATH_FLAG="+b "
        LINKCXXMODULE="$CXX -b"
	CXXMODULELIBS="-lm"
        LDWITHMODULE="$CXX -E"
        LDWITHMODULE_FLAGS="-E"
	;;
      g++)
	AC_MSG_ERROR($SERIALCXX not supported on $host)
	;;
    esac

    case $SERIALCC in
      acc | */acc)
        LINKCMODULE="$CC -b"
	CMODULELIBS="-lm"
	;;
      gcc | */gcc)
        # LINKCMODULE="$CC -Wl,-b"
	# Above fails trying to link in position dependent code in /usr/ccs/lib/crt0.o
        gccspecdir=`$CC -v 2>&1 | grep specs | sed -e "s/^.* //"`
        gcclibdir=`echo $gccspecdir | sed -e "s/\/specs//"`
	CMODULELIBS="-L$gcclibdir -lgcc -lm"
        LINKCMODULE="ld -b"
	;;
    esac
    ;;

  *-ibm-aix*)
    RPATH_FLAG=-L
    PIC_FLAG=""
    NET_LIBS=" "
    THREAD_LIB="-lpthreads"
    DB_LIB="-ldb"
    DYNLINK_LIB="-lld -ldl"
    ZLIB_DIR="-L/usr/common/usg/gnu/lib"
    SO=".so"
    HAVE_BUNDLES=false
    # echo SERIALCXX = $SERIALCXX
    case $SERIALCXX in

      *g++)
	LDFLAGS="$LDFLAGS -Wl,-bbigtoc"
        LINKCMODULE='$(PYTHON_LIBDIR)/ld_so_aix $(CC) -bI:$(PYTHON_LIBDIR)/python.exp'
        LINKCXXMODULE='$(PYTHON_LIBDIR)/ld_so_aix $(CXX) -bI:$(PYTHON_LIBDIR)/python.exp'
        LDWITHMODULE='$(PYTHON_LIBDIR)/makexp_aix python.exp "" $(PYTHON_LIBDIR)/lib$(PYTHON_LIB).a; $(CXX) -Wl,-bE:python.exp'
	;;

      *xlC)
	AC_MSG_WARN(You must add the -C option to nm in $PYTHON_LIBDIR/makexp_aix)
        LINKCMODULE='$(PYTHON_LIBDIR)/ld_so_aix $(CC) -bI:$(PYTHON_LIBDIR)/python.exp'
	# Note that we use C compiler, not C++ compiler, below
        LINKCXXMODULE='$(PYTHON_LIBDIR)/ld_so_aix $(CXX) -bI:$(PYTHON_LIBDIR)/python.exp'
        LDWITHMODULE='$(PYTHON_LIBDIR)/makexp_aix python.exp "" $(PYTHON_LIBDIR)/lib$(PYTHON_LIB).a; $(CXX) -bE:python.exp'
	;;

      *KCC)
	;;

    esac
    ;;

  *-cygwin*)
    RPATH_FLAG="-Wl,-rpath,"
    ;;

  *-linux* | *-freebsd*)
    linuxver=`uname -r | sed 's/-.*$//' | sed 's/\.[0-9]*$//'`
    linuxminver=`echo $linuxver | sed 's/^.*\.//'`
    linuxmajver=`echo $linuxver | sed 's/\..*$//'`
    NET_LIBS=""
    if test "$linuxmajver" -eq 2 -a "$linuxminver" -le 2; then
      NET_LIBS="-lbsd"
    fi
    THREAD_LIB="-lpthread"
    DB_LIB="-ldb"
    DYNLINK_LIB="-ldl"
    ZLIB_DIR=""
    SO=".so"
    LINK_STATIC="-Wl,-Bstatic"
    LINK_DYNAMIC="-Wl,-Bdynamic"
    RPATH_FLAG="-Wl,-rpath,"
    HAVE_BUNDLES=false
    case $SERIALCXX in
      *g++ | *c++)
	PIC_FLAG="-fpic"
	RPATH_FLAG="-Wl,-rpath,"
        LINKCMODULE="$CC -shared "
        LINKCMODULE_FLAGS="-shared "
        LINKCXXMODULE="$CXX -shared "
        LINKCXXMODULE_FLAGS="-shared "
        LDWITHMODULE="$CXX -Wl,-export-dynamic"
        LDWITHMODULE_FLAGS="-Wl,-export-dynamic"
	;;
    esac
    ;;

  *-sgi-irix6*)
    NET_LIBS="-lbsd"
    THREAD_LIB="-lpthread"
    DB_LIB="-ldb"
    DYNLINK_LIB=""
    ZLIB_DIR=""
    SO=".so"
    HAVE_BUNDLES=false

    case $SERIALCXX in
      CC)
	PIC_FLAG=" "
	RPATH_FLAG="-rpath "
        LINKCMODULE="$CC -shared"
        LINKCXXMODULE="$CXX -shared"
        LDWITHMODULE="$CXX"
        LDWITHMODULE_FLAGS=" "
	;;
      g++)
	PIC_FLAG="-fpic"
	RPATH_FLAG=-Wl,-rpath,
        LINKCMODULE="$CC -shared"
        LINKCXXMODULE="$CXX -shared"
        LDWITHMODULE="$CXX "
        LDWITHMODULE_FLAGS=" "
	;;
    esac
    ;;

  *-*-solaris2.5* | *-*-solaris2.6*)
    NET_LIBS="-lsocket -lnsl"
    THREAD_LIB="-lpthread"
    DB_LIB="-ldb"
    DYNLINK_LIB="-ldl"
    ZLIB_DIR=""
    SO=".so"
    HAVE_BUNDLES=false

    case $SERIALCXX in
      CC)
	PIC_FLAG="-PIC"
	RPATH_FLAG=-Wl,-R,
        LINKCXXMODULE="$CXX -G"
        LDWITHMODULE="$CXX"
        LDWITHMODULE_FLAGS=" "
	;;
      g++)
	PIC_FLAG="-fpic"
	RPATH_FLAG=-Wl,-R,
	LDFLAGS="$LDFLAGS -Wl,-z,muldefs "
        LINKCXXMODULE="$CXX -G -nostartfiles -Wl,-z,muldefs"
        LDWITHMODULE="$CXX -Wl,-z,muldefs"
        LDWITHMODULE_FLAGS="-Wl,-z,muldefs"
	;;
    esac

    case $SERIALCC in
      cc | */cc)
        LINKCMODULE="$CC -G"
	;;
      gcc | */gcc)
        LINKCMODULE="$CC -G -nostartfiles -Wl,-z,muldefs"
	;;
    esac
    ;;

  *-*-solaris2.7* |  *-*-solaris2.8*)
    NET_LIBS="-lsocket -lnsl"
    THREAD_LIB="-lpthread"
    DB_LIB="-ldb"
    DYNLINK_LIB="-ldl"
    ZLIB_DIR=""
    SO=".so"
    HAVE_BUNDLES=false

    case $SERIALCXX in
      CC)
	PIC_FLAG="-PIC"
	RPATH_FLAG=-Wl,-R,
        LINKCXXMODULE="$CXX -G"
        LDWITHMODULE="$CXX"
        LDWITHMODULE_FLAGS=" "
	;;
      g++)
	PIC_FLAG="-fpic"
	RPATH_FLAG=-Wl,-R,
        LINKCXXMODULE="$CXX -G -nostartfiles -Wl,-z,muldefs"
        # LDWITHMODULE="$CXX -Wl,-z,muldefs -Wl,-export-dynamic"
        # LDWITHMODULE_FLAGS="-Wl,-z,muldefs -Wl,-export-dynamic"
        LDWITHMODULE="$CXX -Wl,-z,muldefs"
        LDWITHMODULE_FLAGS="-Wl,-z,muldefs"
	;;
    esac

    case $SERIALCC in
      cc | */cc)
        LINKCMODULE="$CC -G"
	;;
      gcc | */gcc)
        LINKCMODULE="$CC -G -nostartfiles -Wl,-z,muldefs"
	;;
    esac
    ;;

  *-cray-unicos*)
    NET_LIBS=""
    THREAD_LIB="-lpthread"
    DB_LIB="-ldb"
    DYNLINK_LIB=""
    ZLIB_DIR=""
    SO=".so"
    PIC_FLAG="-fpic"
    LINKCMODULE="$CC -shared "
    LINKCMODULE_FLAGS="-shared "
    LINKCXXMODULE="$CXX -shared "
    LINKCXXMODULE_FLAGS="-shared "
    LDWITHMODULE="$CXX -Wl,-export-dynamic"
    LDWITHMODULE_FLAGS="-Wl,-export-dynamic"
    HAVE_BUNDLES=false
    case $SERIALCXX in
      KCC)
        RPATH_FLAG="-L"
	;;
      CC)
        RPATH_FLAG="-L"
	;;
    esac
    ;;

  *)
    AC_MSG_WARN(Libraries unknown for host $host.  Please notify the developers.)
    ;;

esac

# For g++, add in location of C++ libraries
case $SERIALCXX in
  *g++)
    gcclibplatform=`$CXX -dumpmachine`          #osx 10.4 + gcc4.0.1 changed libstdc++.a to libstdc++-static.a
    gcclibversion=`$CXX -dumpversion`
    mysystype="${gcclibplatform}-${gcclibversion}"
    case ${mysystype} in
      *-apple-darwin8-4.0.*)
        gcclibextention='-static'
        echo 'mac-gcc 4.0.1: we are looking for libstdc++-static.a'
        ;;
      *)
        gcclibextention=''
        ;;
    esac
    gcclibfilename=`$CXX -print-file-name=libstdc++${gcclibextention}.a`
    # echo gcclibfilename = $gcclibfilename
    gcclibsdir=`dirname $gcclibfilename`
    gcclibsdir=`(cd $gcclibsdir; pwd -P)`
    # echo gcclibsdir = $gcclibsdir
    CXX_LIBFLAG=${RPATH_FLAG}${gcclibsdir}
    CXX_LTLIBFLAG="-rpath ${gcclibsdir}"
    CXX_LIBDIR=${gcclibsdir}
    ;;

esac

# Put into cache

AC_SUBST(OBJEXT)
AC_SUBST(PIC_FLAG)
AC_SUBST(NET_LIBS)
AC_SUBST(THREAD_LIB)
AC_SUBST(DB_LIB)
AC_SUBST(DYNLINK_LIB)
AC_SUBST(ZLIB_DIR)
# JRC: For conformance to convention
Z_LIBDIR=$ZLIB_DIR
AC_SUBST(Z_LIBDIR)
AC_SUBST(SO)
AC_SUBST(LINK_STATIC)
AC_SUBST(LINK_DYNAMIC)
AC_SUBST(RPATH_FLAG)
dnl In case wrapper compiler uses shared libraries without setting rpath:
if test -n "$MPI_LIBDIR"; then
  MPI_RUNLIBFLAG=${RPATH_FLAG}$MPI_LIBDIR
  LDFLAGS="$LDFLAGS $MPI_RUNLIBFLAG"
fi
AC_SUBST(MPI_RUNLIBFLAG)
AC_SUBST(CXX_LIBFLAG)
AC_SUBST(CXX_LTLIBFLAG)
AC_SUBST(LINKCMODULE)
AC_SUBST(LINKCMODULE_FLAGS)
AC_SUBST(LINKCXXMODULE)
AC_SUBST(LINKCXXMODULE_FLAGS)
AC_SUBST(CMODULELIBS)
AC_SUBST(CXXMODULELIBS)
AC_SUBST(LDWITHMODULE)
AC_SUBST(LDWITHMODULE_FLAGS)
AC_SUBST(HAVE_BUNDLES)
AC_SUBST(CXX_LIBDIR)

dnl echo done
