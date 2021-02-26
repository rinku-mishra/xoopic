dnl ######################################################################
dnl
dnl File:	ionpack.m4
dnl
dnl Purpose:	Looks for the ionpack library. 
dnl
dnl Version:	$Id: ionpack.m4 1577 2005-07-16 14:35:51Z cary $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################

IONPACK_INCDIR=""
IONPACK_INC=""
IONPACK_LIBS=""
IONPACK_LIBDIR=""
dnl ######################################################################
dnl
dnl Allow the user to specify an overall ionpack directory.  If specified,
dnl we look for include and lib under this.
dnl
dnl ######################################################################

AC_ARG_WITH(ionpack-dir,[  --with-ionpack-dir=<location of ionpack installation> ],IONPACK_DIR="$withval",IONPACK_DIR="")

dnl ######################################################################
dnl
dnl Find ionpack includes - looking in include location if present,
dnl otherwise in dir/include if present, otherwise in default locations,
dnl first parallel, then serial.
dnl
dnl ######################################################################

AC_ARG_WITH(ionpack-incdir,[  --with-ionpack-incdir=<location of ionpack includes> ],
IONPACK_INCDIR="$withval",IONPACK_INCDIR="")
if test -n "$IONPACK_INCDIR"; then
  IONPACK_INCPATH=$IONPACK_INCDIR
elif test -n "$IONPACK_DIR"; then
  IONPACK_INCPATH=$IONPACK_DIR/include
else
  IONPACK_INCPATH=$abs_top_builddir/../txmodules/ionpack/include:$HOME/$UNIXFLAVOR/ionpack/include:$HOME/ionpack/include:$HOME/opt/ionpack/include:$HOME/$host/ionpack/include:/usr/local/ionpack/include:/opt/ionpack/include:/loc/ionpack/include:/local/ionpack/include:/usr/local/cmee/include:/usr/local/txphysics/include
fi
AC_PATH_PROGS(IONPACK_H, ionpack.h, "", $IONPACK_INCPATH)
if test -z "$IONPACK_H"; then
  AC_MSG_WARN([ionpack.h not found in $IONPACK_INCPATH. Set location with --[with-ionpack-incdir] to use ionpack. Building without ionpack.])
else
  IONPACK_INCDIR=`dirname $IONPACK_H`
  IONPACK_INC=-I$IONPACK_INCDIR
  IONPACK_DIR=`dirname $IONPACK_INCDIR`
fi

dnl ######################################################################
dnl
dnl Find ionpack (static for now, even if the dynamic exists) libraries.
dnl
dnl ######################################################################

AC_ARG_WITH(ionpack-libdir,[  --with-ionpack-libdir=<location of ionpack library> ], IONPACK_LIBDIR="$withval",IONPACK_LIBDIR="")
if test -n "$IONPACK_INCDIR"; then
  if test -n "$IONPACK_LIBDIR"; then
    IONPACK_LIBPATH=$IONPACK_LIBDIR
  elif test -n "$IONPACK_DIR"; then
    IONPACK_LIBPATH=$IONPACK_DIR/lib:$IONPACK_DIR/lib/$COMPDIR
  else 
    IONPACK_LIBPATH="$abs_top_builddir/../txmodules/ionpack/lib:$HOME/ionpack/lib/$COMPDIR:$HOME/ionpack/lib:$HOME/opt/ionpack/lib:$HOME/$host/ionpack/lib/$COMPDIR:$HOME/$host/ionpack/lib:/usr/local/ionpack/lib/$COMPDIR:/usr/local/ionpack/lib:/loc/ionpack/lib/$COMPDIR:/opt/ionpack/lib:/loc/ionpack/lib:/usr/local/txphysics/lib"
  fi
  AC_PATH_PROGS(LIBIONPACK_A, libionpack.a, "", $IONPACK_LIBPATH)
  if test -z "$LIBIONPACK_A"; then
    AC_MSG_WARN([libionpack.a not found in $IONPACK_LIBPATH. Use --[with-ionpack-libdir] to set location. Building without ionpack.])
  else
    IONPACK_LIBDIR=`dirname $LIBIONPACK_A`
    IONPACK_LIBS="-L$IONPACK_LIBDIR -lionpack"' $(RPATH_FLAG)'"$IONPACK_LIBDIR"
  fi
fi
AC_SUBST(IONPACK_INCDIR)      
AC_SUBST(IONPACK_INC)
AC_SUBST(IONPACK_LIBDIR)
AC_SUBST(IONPACK_LIBS)
