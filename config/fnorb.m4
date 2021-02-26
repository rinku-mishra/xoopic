dnl ######################################################################
dnl
dnl Check for FNORB
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Find Fnorb modules
dnl
dnl ######################################################################

if test -n "$PY_ROOT"; then
  AC_ARG_WITH(FNORBPATH,
  [  --with-FNORBPATH=<fnorb-path>            to set location of fnorb modules],
  FNORBPATH="$withval")
  if test -z "$FNORBPATH"; then
    FNORBPATH=$PYTHON_SITEDIR:$PYTHON_SITEDIR/Fnorb:/$host/python/lib/python/Fnorb:/opt/python/lib/python/Fnorb:/usr/lib/python/Fnorb:/mfe/local/python/lib/python/Fnorb:/usr/local/python/lib/python/Fnorb:/loc/python/lib/python/Fnorb:/usr/local/Python/lib/python/Fnorb
  fi
  AC_PATH_PROGS(CDR_MODULE, cdr$SO cdr.sl cdrmodule$SO cdrmodule.sl, "", $FNORBPATH)
  if test -n "$CDR_MODULE"; then
      FNORB_LIBDIR=`dirname $CDR_MODULE`
      PYLIBPATH=`dirname $FNORB_LIBDIR`
      FNORBPATH=$PYLIBPATH:$FNORB_LIBDIR
      AC_SUBST(FNORBPATH)
      AC_SUBST(FNORB_LIBDIR)
  else
      AC_MSG_WARN(Unable to find FNORB modules.  Python Makefile's will not be created.)
      PYTHON_MAKEFILES=""
  fi
fi

dnl Look for fnidl as needed for FNORB

if test -n "$FNORBPATH"; then
  FNIDLPATH=$PYTHON_TOPDIR/bin:$PATH
  AC_PATH_PROGS(FNIDL, fnidl, "", $FNIDLPATH)
  if test -z "$FNIDL"; then
    AC_MSG_WARN(Unable to find fnidl in $FNIDLPATH.  Python Makefile\'s will not be created.)
    PYTHON_MAKEFILES=""
  fi
fi
