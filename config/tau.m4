dnl ######################################################################
dnl
dnl Find tau wrappers
dnl
dnl ######################################################################

unset TAU_BINDIR
AC_ARG_WITH(tau-cxx, AC_HELP_STRING([--with-tau-cxx=<tau wrapper>],
	[to use the Tau C++ wrapper <tau wrapper>]),
	[TAU_WRAPPER="${withval}"], )
# If answer simply yes, then try to locate
if test "$TAU_WRAPPER" = yes; then
  myproc=`uname -p`
  TAU_PATH=$PATH:/contrib/tau/$myproc/bin:/usr/local/tau/$myproc/bin
  AC_PATH_PROG(TAU_WRAPPER, tau_cxx.sh, , $TAU_PATH)
  if test -z "$TAU_WRAPPER"; then
    AC_MSG_ERROR(tau_cxx.sh not found in $TAU_PATH.)
  fi
fi
# Have a candication
if test -n "$TAU_WRAPPER"; then
# If not an executable, bail
  if test ! -x $TAU_WRAPPER; then
    AC_MSG_ERROR($TAU_WRAPPER not an executable.)
  fi
# Find other dirs
  TAU_BINDIR=`dirname $TAU_WRAPPER `
  TAU_LIBDIR=`dirname $TAU_BINDIR`/lib
  echo TAU_LIBDIR = \'$TAU_LIBDIR\'
# Locate tau makefile
  AC_ARG_WITH(tau-makefile,
	AC_HELP_STRING([--with-tau-makefile=<tau makefile>],
	[to use the Tau makefile <tau makefile>]),
	[TAU_MAKEFILE="${withval}"], )
# If not absolute, try libdir
  if test -n "$TAU_MAKEFILE"; then
    absmakefile=`echo $TAU_MAKEFILE | grep ^/`
    if test -z "$absmakefile"; then
      TAU_MAKEFILE=$TAU_LIBDIR/$TAU_MAKEFILE
    fi
  fi
# Not found so pick first consistent
  if test -z "$TAU_MAKEFILE"; then
    if test $parallel = yes; then
      taumakefiles=`\ls $TAU_LIBDIR/Makefile*mpi*pdt*`
    else
      taumakefiles=`\ls $TAU_LIBDIR/Makefile*pdt* | grep -v mpi`
    fi
    TAU_MAKEFILE=`echo $taumakefiles | sed 's/ .*$//'`
  else
    if test ! -f $TAU_MAKEFILE; then
      AC_MSG_ERROR($TAU_MAKEFILE does not exist.)
    fi
  fi
  echo TAU_MAKEFILE =\'$TAU_MAKEFILE\'
  if test $parallel = yes; then
    goodmakefile=`echo $TAU_MAKEFILE | grep mpi`
    if test -z "$goodmakefile"; then
      AC_MSG_ERROR($TAU_MAKEFILE not appropriate for parallel build.)
    fi
  else
    goodmakefile=`echo $TAU_MAKEFILE | grep -v mpi`
    if test -z "$goodmakefile"; then
      AC_MSG_ERROR($TAU_MAKEFILE not appropriate for serial build.)
    fi
    tauopts='-optMpi'
  fi
dnl Should test for which of the above has the same compiler
  EXPCXX=`$TAU_WRAPPER -tau_makefile=$TAU_MAKEFILE $tauopts -show | sed 's/ .*$//'`
  AC_MSG_WARN(Tau expects you to be using $EXPCXX)
  CXX="$TAU_WRAPPER -tau_makefile=$TAU_MAKEFILE $tauopts"
  AC_DEFINE(HAVE_TAU, , Define if have the Tau performance tools.)
fi
AC_SUBST(TAU_WRAPPER)
AC_SUBST(TAU_MAKEFILE)
AM_CONDITIONAL(HAVE_TAU, test -n "$TAU_WRAPPER")

dnl ######################################################################
dnl
dnl Print out configuration information
dnl
dnl ######################################################################

if test -n "$config_summary_file"; then
   echo                                      >> $config_summary_file
   if test -n "$TAU_WRAPPER"; then
      echo "Using Tau with"                 >> $config_summary_file
      echo "  TAU_WRAPPER:    $TAU_WRAPPER"   >> $config_summary_file
      echo "  TAU_MAKEFILE:   $TAU_MAKEFILE"  >> $config_summary_file
      echo "  TAU_CXX:        $CXX"  >> $config_summary_file
   else
      echo "NOT using Tau"                >> $config_summary_file
   fi
fi

