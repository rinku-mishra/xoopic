dnl ######################################################################
dnl
dnl File:	ccafe.m4
dnl
dnl Purpose:	Determine the locations of CCAfeine toolkits and the
dnl             associating includes and libraries
dnl
dnl Version: $Id: ccafe.m4 1453 2005-02-14 22:08:16Z nanbor $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################


dnl @@ What the heck are the following 3 lines for? -nw
# Check for libutil.a (needed on Fedore Core 2 for python 2.3.4)
EXTRA_PYTHON_LIBS=""
AC_CHECK_LIB(util,openpty,[EXTRA_PYTHON_LIBS="$EXTRA_PYTHON_LIBS -lutil"])

dnl Determining where the ccafe toolkits are installed via the location of
dnl ccafe-config 
AC_ARG_WITH(ccafe,
	[  
  --with-ccafe=/path/to/ccafe           full path to ccafe installation],
	CCAFE_ROOT=$with_ccafe,[ccafe_config=`which ccafe-config`])
if test "$CCAFE_ROOT" = "" ; then  # if ccafe not defined
# then we assume
  if test "$ccafe_config" = "" ; then
    AC_MSG_ERROR([Can not determine where Ccafeine is installed.  Use --with-ccafe=ccafe_path to specify the path.])
  fi
  CCAFE_CONFIG=$ccafe_config
  ccafe_temp=`dirname $ccafe_config`
  CCAFE_ROOT=`dirname $ccafe_temp`
  AC_MSG_NOTICE([CCAFE_ROOT=$CCAFE_ROOT is determined from $ccafe_config])
else
  if ! test -d $CCAFE_ROOT ; then
	AC_MSG_ERROR([--with-ccafe must be given the directory to the Ccafeine installation.])
  fi
  CCAFE_CONFIG="$CCAFE_ROOT/bin/ccafe-config"
  if ! test -f $CCAFE_CONFIG ; then
    AC_MSG_ERROR([$CCAFE_CONFIG executable not found, make sure --with-ccafe to specify the root directory of Ccafeine installation.])
  fi
  if ! test -x $CCAFE_CONFIG ; then
    AC_MSG_ERROR([$CCAFE_CONFIG not executable.])
  fi
fi
AC_MSG_RESULT([$CCAFE_CONFIG found.])

CCAFE_MAKEINCL=`$CCAFE_CONFIG --var CCAFE_BABEL_CCA_CONFIG`
CCASPEC_BABEL_LANGUAGES=`grep CCASPEC_BABEL_LANGUAGES $CCAFE_MAKEINCL | sed -e 's/^CCASPEC_BABEL_LANGUAGES=//'`
CCAFE_BINDIR=`$CCAFE_CONFIG  --var CCAFE_bindir`

AC_ARG_WITH(languages, 
	[  --with-languages="LANG1 LANG2 LANG3 ..."        list of languages (e.g., "c c++ f90 python java"); 
                                                  by default all Babel-supported languages are available],
        [
   CCA_LANGUAGES=""
   for i in $with_languages ; do
     for j in $CCASPEC_BABEL_LANGUAGES ; do
       if test "x$i" = "x$j" ; then
         CCA_LANGUAGES="$CCA_LANGUAGES $i";
       fi
     done
   done], [CCA_LANGUAGES="$CCASPEC_BABEL_LANGUAGES"])

AC_MSG_RESULT([CCA will support the following languages: $CCA_LANGUAGES.])

dnl The following scripts are a hack to determine the subdirectory name containing
dnl the legacy Fortran 90 code for component implementation.  We should probably 
dnl treat it as a configurable argument.
CCA_LEGACY_90_DIR=""
for i in $CCA_LANGUAGES ; do
  if test "$i" = "f90" ; then
    CCA_LEGACY_90_DIR="legacy/f90"
  fi
done

# Ccaffeine executable
CCAFE_EXEC=$CCAFE_BINDIR/ccafe-single

AC_SUBST(CCAFE_ROOT)
AC_SUBST(CCAFE_CONFIG)
AC_SUBST(CCAFE_MAKEINCL)
AC_SUBST(CCA_LANGUAGES)
AC_SUBST(EXTRA_PYTHON_LIBS)
AC_SUBST(CCAFE_BINDIR)
AC_SUBST(CCA_LEGACY_90_DIR)
AC_SUBST(CCAFE_EXEC)
