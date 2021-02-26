dnl ######################################################################
dnl
dnl Check for Numeric Python
dnl
dnl ######################################################################

if test -n "$PY_ROOT"; then
  NUMERIC_PY_INCDIR=$PYTHON_INCDIR/numeric:$PYTHON_INCDIR/Numeric:/mfe/local/python/NumPy/Include
  AC_ARG_WITH(NUMERIC_PY_INCDIR,
  [  --with-NUMERIC_PY_INCDIR=<numeric python incdir> to set directory of numeric python includes],
  NUMERIC_PY_INCDIR="$withval")
  AC_PATH_PROGS(NUMPY_IDR, arrayobject.h, "", $NUMERIC_PY_INCDIR)
  if test -n "$NUMPY_IDR"; then
    NUMERIC_PY_INCDIR=`dirname $NUMPY_IDR`
    AC_SUBST(NUMERIC_PY_INCDIR)
  else
    AC_MSG_WARN(Unable to find Numeric Python include dir.  Python subdirs will not compile.  Set --with-NUMERIC_PY_INCDIR to the location of the Numeric python includes.)
  fi

  NUMERIC_PY_LIBPATH=$PYTHON_SITEDIR/Numeric:$PYTHON_SITEDIR/numeric:/mfe/local/python/NumPy/NumPyLib:/mfe/local/python/NumPy/NumPyLib
  AC_ARG_WITH(NUMERIC_PY_LIBPATH,
  [  --with-NUMERIC_PY_LIBPATH=<numeric python libdir> to set directory of numeric python libraries],
  NUMERIC_PY_LIBPATH="$withval")
  AC_PATH_PROGS(NUMPY_LDR, _numpy$SO, "", $NUMERIC_PY_LIBPATH)
  if test -z "$NUMPY_LDR"; then
    AC_PATH_PROGS(NUMPY_LDR, _numpymodule$SO, "", $NUMERIC_PY_LIBPATH)
  fi
  if test -z "$NUMPY_LDR"; then
    AC_PATH_PROGS(NUMPY_LDR, _numpymodule.sl, "", $NUMERIC_PY_LIBPATH)
  fi
  if test -z "$NUMPY_LDR"; then
    AC_PATH_PROGS(NUMPY_LDR, _numpy.sl, "", $NUMERIC_PY_LIBPATH)
  fi
  if test -n "$NUMPY_LDR"; then
    NUMERIC_PY_LIBDIR=`dirname $NUMPY_LDR`
    AC_SUBST(NUMERIC_PY_LIBDIR)
  else
    AC_MSG_WARN(Unable to find Numeric Python library directory in $NUMERIC_PY_LIBPATH.  Python code will not run.  Set --with-NUMERIC_PY_LIBPATH to the location of the Numeric python includes.)
  fi
fi

