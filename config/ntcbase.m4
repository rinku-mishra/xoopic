dnl ######################################################################
dnl
dnl File:	ntcbase.m4
dnl
dnl Purpose:	The base configure.in used for ntcdata and ntcphys.
dnl
dnl Version:	$Id: ntcbase.m4 1013 2003-08-30 00:03:27Z johan $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Make needed subdirectories
dnl
dnl ######################################################################

echo Making top level docs directories
if test ! -d docs; then mkdir docs; fi
if test ! -d docs/idlapi; then mkdir docs/idlapi; fi
if test ! -d docs/cxxapi; then mkdir docs/cxxapi; fi
if test ! -d docs/javaapi; then mkdir docs/javaapi; fi
# if test ! -d classes; then mkdir classes; fi

dnl ######################################################################
dnl
dnl Set up compiler: select and set default flags
dnl
dnl ######################################################################

# if test -z "$CXX"; then
AC_ARG_WITH(CXX,[  --with-CXX=<compiler>   to use <compiler> (CC, cxx, KCC) instead of g++],CXX="$withval",CXX=g++)
# fi
echo CXX = $CXX

# Compare with g++ to see if we use g77
CXXTMP=`echo $CXX | sed 's/ .*$//'`
CXXBASE=`basename $CXXTMP`
CXXDIR=`dirname $CXXTMP`
case $CXXBASE in

  g++ | c++)
    case $CXXDIR in
      . | "")
        CC=gcc
        ;;
      *)
        CC=${CXXDIR}/gcc
        ;;
    esac
    ;;

  mpiCC)
    CC=$CXXDIR/mpicc
    MPI_CXXFLAGS="-DMPI_PYTHON"
    ;;

  aCC)
    CC=gcc
    ;;

  *)
    CC=cc
    ;;

esac
# echo After compiler setting: pwd = $PWD

echo ... Using C++ compiler $CXX, C compiler $CC
export CXX
export CC
echo Touching .depend in C++ subdirectories
for i in $CXXSUBDIRS; do
  touch $i/.depend
done

dnl ######################################################################
dnl
dnl Check for Java and Java-dependent packages
dnl
dnl ######################################################################

builtin(include,config/java.m4)
# echo After java setting: pwd = $PWD
builtin(include,config/jchart.m4)
# echo After jchart setting: pwd = $PWD
builtin(include,config/netcdf.m4)
builtin(include,config/hdf5.m4)

dnl ######################################################################
dnl
dnl Auxiliary compilation flags
dnl
dnl ######################################################################

# Determine BSD compatible nm
builtin(include,config/nm.m4)
# Add compilie flags
builtin(include,config/flags.m4)
CXXFLAGS="$CXXFLAGS $OPTIFLAGS"
CFLAGS="$CFLAGS $C_OPTIFLAGS"
# Add link flags
builtin(include,config/libs.m4)

dnl ######################################################################
dnl
dnl Allow for extra flags
dnl
dnl ######################################################################

# Check for extra flags
AC_ARG_WITH(EXTRA_CXXFLAGS,[  --with-EXTRA_CXXFLAGS=<flags>  to add <flags> to c++ compilation], EXTRA_CXXFLAGS="$withval",EXTRA_CXXFLAGS="")
CXXFLAGS="$CXXFLAGS $EXTRA_CXXFLAGS"
AC_ARG_WITH(EXTRA_LDFLAGS,[  --with-EXTRA_LDFLAGS=<flags>   to add <flags> to linking], EXTRA_LDFLAGS="$withval",EXTRA_LDFLAGS="")
LDFLAGS="$LDFLAGS $EXTRA_LDFLAGS"

dnl ######################################################################
dnl
dnl Check for CORBA stuff and remove unneeded directories
dnl
dnl ######################################################################

USEOB3=true
builtin(include,config/corba.m4)
builtin(include,config/jcorba.m4)
rm -f classes/org classes/com

dnl ######################################################################
dnl
dnl Check for Python stuff
dnl
dnl ######################################################################

builtin(include,config/python.m4)
builtin(include,config/numpy.m4)
builtin(include,config/fnorb.m4)

dnl ######################################################################
dnl
dnl Check for MDS plus
dnl
dnl ######################################################################

builtin(include,config/mdsplus.m4)

dnl ######################################################################
dnl
dnl Select libs and flags for linking
dnl
dnl ######################################################################


dnl ######################################################################
dnl
dnl Check for header files
dnl
dnl ######################################################################

dnl AC_CHECK_HEADERS(stdlib.h stddef.h unistd.h)
dnl AC_CHECK_HEADERS(strings.h bstring.h)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_CHECK_HEADERS(iostream strstream fstream sstream)
dnl AC_CHECK_HEADERS(exception exception.h terminate.h unexpected.h)
AC_LANG_RESTORE

dnl ######################################################################
dnl
dnl Check for typedefs, structures, and compiler characteristics
dnl
dnl ######################################################################

AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

dnl ######################################################################
dnl
dnl Check for library functions
dnl
dnl ######################################################################

dnl NOT USED
dnl AC_CHECK_FUNCS(strerror)

dnl ######################################################################
dnl
dnl Find out what the library suffix is
dnl
dnl ######################################################################

AC_MSG_CHECKING(what the library suffix is)
AC_SUBST(LIBEXT)
if test -z "$LIBEXT"
then
    LIBEXT=".a"
fi
AC_MSG_RESULT($LIBEXT)

dnl ######################################################################
dnl
dnl Find out how to create libraries
dnl
dnl ######################################################################

AC_MSG_CHECKING(how to build libraries)
if test -z "$AR"; then
  AR="ar cr"
fi
if test -z "$AR_FLAGS"; then
  AR_FLAGS="cr"
fi
AC_MSG_RESULT(with $AR $AR_FLAGS)
AC_SUBST(AR)
AC_SUBST(AR_FLAGS)

dnl ######################################################################
dnl
dnl Find out how to install libraries
dnl
dnl ######################################################################

AC_MSG_CHECKING(how to install libraries)
AC_SUBST(INSTALL_LIBRARY)
if test -z "$INSTALL_LIBRARY"
then
    if test "$LIBEXT" = ".sl" # HP needs executable shared libs
    then
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
dnl C++ compiler capabilities
dnl
dnl ######################################################################

builtin(include,config/macros.m4)
builtin(include,config/cxx.m4)

dnl ######################################################################
dnl
dnl Find doxygen documentation programs
dnl
dnl ######################################################################

AC_PATH_PROGS(DOXYGEN, doxygen, "")
if test -z "$DOXYGEN"; then
  AC_MSG_WARN(The doxygen utility was not found in your path.  Combined C++ documentation will not be made.)
fi
AC_SUBST(DOXYGEN)

dnl ######################################################################
dnl
dnl Set permissions on directories
dnl
dnl ######################################################################
echo Setting permission on directories
chmod a+rx docs
chmod a+rx docs/images
chmod a+rx html 
chmod a+rx html/images
chmod -R a+r docs
chmod -R a+r html

dnl ######################################################################
dnl
dnl Check for web links to this project area
dnl
dnl ######################################################################

AC_MSG_CHECKING(for web accessible area)
if test ! -d $HOME/public_html; then
  AC_MSG_RESULT(not found.  $HOME/public_html created.)
  mkdir $HOME/public_html
  chmod 755 $HOME/public_html
else
  AC_MSG_RESULT(found.)
fi
homebase=`basename $HOME`
reldir=`pwd | sed "s/^.*${homebase}\///"`
thisdir=$PWD
if test -d $HOME/public_html; then
  chmod 755 $PWD docs html
  cd $HOME/public_html
  rm -f $HTMLDOCSDIR
  ln -s ../$reldir/docs $HTMLDOCSDIR
  rm -f $PACKAGE
  ln -s ../$reldir/html $PACKAGE
  echo Docs and html linked to public_html.
  cd $thisdir
fi

dnl ######################################################################
dnl
dnl Fix up netscape's security
dnl
dnl ######################################################################

preffile=$HOME/.netscape/preferences.js
echo $ac_n "Adding necessary lines to $preffile... " $ac_c
if test ! -f $preffile; then
  echo $preffile not found!
else
  hasline=`grep signed.applets.local_classes_have_30_powers $preffile`
  if test -z "$hasline"; then
    netscaperunning=`ps -u $USER 2>/dev/null | grep netscape`
    if test -n "$netscaperunning"; then
      "cannot because netscape is running.  Quit netscape and redo if you need to add necessary lines to netscape's preferences."
    else
      echo 'user_pref("signed.applets.codebase_principal_support", true);' \
        >> $preffile
      echo 'user_pref("signed.applets.local_classes_have_30_powers", true);' \
        >> $preffile
      echo 'user_pref("signed.applets.low_security_for_local_classes", true);' \
        >> $preffile
      echo done.
    fi
  else
    echo lines already present.
  fi
fi


