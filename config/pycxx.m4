dnl ######################################################################
dnl
dnl Determine the C and C++ compilers for python
dnl
dnl ######################################################################

AC_MSG_CHECKING(for C compiler used by python)
PYVERSION=`python -c "import sys; print sys.version[[:3]]"`
installdir=`python -c "import sys; print sys.prefix"`
configdir=$installdir/lib/python$PYVERSION/config
PYMAKEFILE=$configdir/Makefile
TMPLIBS=$LIBS
MAKEVARS1="CC CCC LINKCC OPT LDFLAGS LDLAST DEFS LIBS LIBM LIBC RANLIB MACHDEP"
MAKEVARS2="SO LDSHARED CCSHARED SGI_ABI"
MAKEVARS="$MAKEVARS1 $MAKEVARS2"
echo "" >makevars
chmod a+x makevars
for i in $MAKEVARS; do
  tmpval=`grep "^${i}=" $PYMAKEFILE | sed 's/^.*=//' | sed 's/^ //'`
  val=`echo $tmpval | sed 's/^\t//'`
  echo $i=\'$val\' >> makevars
done
. ./makevars
rm makevars
AC_MSG_RESULT($CC)
CC=`echo $CC | sed 's/ .*$//'`
echo Python used C compiler $CC.  So will we.
export CC
AC_SUBST(CC)
AC_SUBST(SO)
PYTHON_BUILD_LIBS=$LIBS
AC_SUBST(PYTHON_BUILD_LIBS)
LIBS=$TMPLIBS
PYTHON_CCSHARED=$CCSHARED
AC_SUBST(PYTHON_CCSHARED)

dnl ######################################################################
dnl
dnl Determine the C++ compiler from the c compiler used by python
dnl
dnl ######################################################################


AC_MSG_CHECKING(for C++ compiler)

dnl Get recommended compiler
CC=`echo $CC | sed 's/ .*$//'`
BASECC=`echo $CC | sed 's/^.*\///g'`
# echo BASECC = $BASECC

case $BASECC in

# Native compiler
  cc | cc)
    # echo host is $host
    case "$host" in
      *-ibm-aix*)
        # All that will compile
	RECCXX=xlC
        ;;
      *-hp-hpux*)
	RECCXX=aCC
        ;;
      *-sgi-irix6*)
	RECCXX=CC
        ;;
      *-linux* | *-darwin*)
	RECCXX=g++
        ;;
      alpha*-dec-osf*)
	RECCXX=g++
	# cxx is too strict
        ;;
      *-*-solaris*)
        # RECCXX=CC
	# CC5.0 has problems with instantiation
        RECCXX=g++
        ;;
    esac
    ;;

# GCC
  gcc)
    RECCXX=g++
    ;;

# IBM xlc
  xlc)
    RECCXX=xlC
    ;;

# Default
  *)
    RECCXX=g++
    ;;

esac

if test -n "$RECCXX"; then
  AC_MSG_RESULT(recommendation is $RECCXX.)
else
  AC_MSG_RESULT(no recommendation.)
fi

dnl Allow user to override
AC_ARG_WITH(CXX,[  --with-CXX=<compiler>   to use <compiler> (CC, cxx, KCC) instead of recommendation],CXX="$withval",CXX=$RECCXX)

AC_SUBST(CXX)

dnl ######################################################################
dnl
dnl Flags needed for compiling the python libraries
dnl
dnl ######################################################################

if test -z "$PY_CXX_FLAGS"; then
  PY_CXX_FLAGS="-DRealSize8 -DIndirect_Indexing"
fi
AC_ARG_WITH(PY_CXX_FLAGS,
[  --with-PY_CXX_FLAGS=<xtra python CXX flags>      to set additional CXX flags for python dirs],
PY_CXX_FLAGS="$withval")
AC_SUBST(PY_CXX_FLAGS)
