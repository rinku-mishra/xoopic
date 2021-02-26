dnl ######################################################################
dnl
dnl $Id: python.m4 2307 2007-06-24 12:43:25Z cary $
dnl
dnl Check for Python stuff
dnl
dnl ######################################################################

dnl Allow disabling of Python
AC_ARG_ENABLE([python],
	AC_HELP_STRING([--disable-python],
		[to not use python (default is use)]),
	[ac_enable_python=${enableval}], [ac_enable_python=yes])
# echo ac_enable_python = $ac_enable_python

dnl ######################################################################
dnl
dnl Find python
dnl
dnl ######################################################################

if test "$ac_enable_python" = yes; then
  AC_ARG_WITH(python-dir, [  --with-python-dir=<python install prefix>],
	PY_ROOT="$withval")
  if test -n "$PY_ROOT"; then
    PY_PATH=$PY_ROOT/bin
  else
    PY_PATH=$PATH:$HOME/$host/python/bin:/mfe/local/python/bin:/loc/python/bin
  fi
  AC_PATH_PROGS(PYTHON, python, "", $PY_PATH)
  if test -n "$PYTHON"; then
    PYTHON_BINDIR=`dirname $PYTHON`
    PY_ROOT=`dirname $PYTHON_BINDIR`
  else
    AC_MSG_WARN(Unable to find python in your path.  Python Makefile\'s will not be created.)
    PYTHON_MAKEFILES=""
  fi
  AC_SUBST(PYTHON_BINDIR)
  AC_SUBST(PYTHON)
  if test -n "$PYTHON"; then
    AC_DEFINE(HAVE_PYTHON, , Define if Python found on system)
  fi
fi
# echo Search for python completed.

dnl ######################################################################
dnl
dnl Find the python include files and library dir
dnl
dnl ######################################################################

if test -n "$PYTHON"; then
  AC_MSG_CHECKING(for python includes and libraries)
  PYTHON_TOPLIBDIR=`$PYTHON -c "import distutils.sysconfig; \
                dir = distutils.sysconfig.get_python_lib(1,1); \
                print dir"`
  PYTHON_INCDIR=`$PYTHON -c "import distutils.sysconfig; \
                dir = distutils.sysconfig.get_python_inc(1); \
                print dir "`
  AC_SUBST(PYTHON_INCDIR)
  PYTHON_INC=-I$PYTHON_INCDIR
  AC_SUBST(PYTHON_INC)
  AC_MSG_RESULT($PYTHON_INC)
fi

dnl ######################################################################
dnl
dnl Find the python libraries and flags
dnl
dnl ######################################################################

if test -n "$PYTHON"; then
  AC_MSG_CHECKING(for python library flags)
  PYTHON_LIBDIR=$PYTHON_TOPLIBDIR/config
  AC_SUBST(PYTHON_LIBDIR)
  if test -f $PYTHON_LIBDIR/Makefile; then
    PYTHON_XLIBS=`grep ^LIBS= $PYTHON_LIBDIR/Makefile | sed 's/^LIBS= */ /'`
  fi
  dnl echo PYTHON_XLIBS = \'$PYTHON_XLIBS\'
  PY_VERSION=`$PYTHON -c "import sys; print sys.version[[:3]]"`
  dnl echo PY_VERSION = \'$PY_VERSION\'
  PYTHON_LIB="python"$PY_VERSION
  dnl echo PYTHON_LIB = \'$PYTHON_LIB\'
  AC_SUBST(PYTHON_LIB)
  PYTHON_LINKFORSHARED=`$PYTHON -c 'import distutils.sysconfig; print distutils.sysconfig.get_config_var("LINKFORSHARED")' | sed 's/ Python.framework.*Python / /'`
dnl last bit is a patch for os x.
  dnl echo PYTHON_LINKFORSHARED = $PYTHON_LINKFORSHARED
  AC_SUBST(PYTHON_LINKFORSHARED)
  PYTHON_LINK=`echo $PYTHON_LINKFORSHARED | sed 's/-Wl,-bE:Modules\/python.exp//'`
  PYTHON_LIBS="-L$PYTHON_LIBDIR "' $(RPATH_FLAG)'"$PYTHON_LIBDIR -l$PYTHON_LIB $PYTHON_XLIBS $PYTHON_LINK"
  AC_SUBST(PYTHON_LIBS)
  AC_MSG_RESULT($PYTHON_LIBS)
fi

dnl ######################################################################
dnl
dnl Add in the python site libraries to allow tailoring of the build
dnl
dnl ######################################################################

if test "$SITE" = "llnl.gov"; then
  PY_SITE_LIBS="-Wl,-R,/opt/SUNWspro/lib -L/opt/SUNWspro/lib -lF77 -lM77 -lsunmath -L/mfe/local/Yorick/yorick-1.4/Gist -lgist -L/mfe/local/lib -R/mfe/local/lib -ltk8.3 -ltcl8.3 -Wl,-R,/usr/openwin/lib -L/usr/openwin/lib -lX11 -lpthread  -L/usr/ccs/lib -lgen"
fi
AC_SUBST(PY_SITE_LIBS)

dnl ######################################################################
dnl
dnl Write to summary file if defined
dnl SEK: Not sure what the most useful AC_SUBST variables a user
dnl needs to know.  Printing out the standard stuff
dnl
dnl ######################################################################
if test -n "$config_summary_file"; then
dnl Is there a check?
  if test -n "$PYTHON"; then
    echo                                                 >> $config_summary_file
    echo "Using Python version $PY_VERSION with"         >> $config_summary_file
    echo "  PYTHON_INC:  $PYTHON_INC"                    >> $config_summary_file
    echo "  PYTHON_LINKFORSHARED:  $PYTHON_LINKFORSHARED">> $config_summary_file
    echo "  PYTHON_LIBS: $PYTHON_LIBS"                   >> $config_summary_file
  else
    echo "NOT using Python"                     >> $config_summary_file
    echo                                        >> $config_summary_file
  fi
fi

