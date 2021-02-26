dnl ######################################################################
dnl
dnl Check for OpenDX
dnl
dnl ######################################################################

dnl Get the executable

DX_PATH=$HOME/$host/dx/bin:$HOME/$host/bin:/usr/local/dx/bin:/usr/local/bin:$PATH

echo Looking for OpenDX
AC_PATH_PROGS(OPENDX, dx, "", $DX_PATH)

dnl ######################################################################
dnl
dnl Check for mdf2c
dnl
dnl ######################################################################

bindir=`dirname $OPENDX`
DXAUX_BASEDIR=`dirname $bindir`/dx
DXAUX_DIRPATH=$DXAUX_BASEDIR/bin

AC_ARG_WITH(DXAUX_DIRPATH, 
[  --with-OPENDX-auxiliary=<dx-auxiliary-dir>  set location of the auxiliary directory for OpenDX],
DXAUX_DIRPATH="$withval")

AC_PATH_PROGS(MDF2C, mdf2c, "", $DXAUX_DIRPATH)
if test -z "$MDF2C"; then
  AC_MSG_ERROR(OpenDX auxiliaries not found in $DXAUX_DIRPATH.  Use --with-OPENDX-external to set the location of mdf2c.)
fi

dnl ######################################################################
dnl
dnl Get the OpenDx include directory
dnl
dnl ######################################################################

DX_INCDIRPATH=$DXAUX_BASEDIR/include

AC_ARG_WITH(DX_INCDIRPATH, 
[  --with-OPENDX-include=<dx-include-dir>  set location of the auxiliary directory for OpenDX],
DX_INCDIRPATH="$withval")

AC_PATH_PROGS(DXCONFIG_H, dxconfig.h, "", $DX_INCDIRPATH)
if test -z "$DXCONFIG_H"; then
  AC_MSG_ERROR(dxconfig.h not found in $DX_INCDIRPATH.  Use --with-OPENDX-include to set the location of dxconfig.h.)
fi
DX_INCDIR=`dirname $DXCONFIG_H`
AC_SUBST(DX_INCDIR)

dnl ######################################################################
dnl
dnl Get the OpenDx library directory
dnl
dnl ######################################################################

DX_LIBBASE=`ls -d $DXAUX_BASEDIR/lib_*`
DX_LIBDIRPATH=$DX_LIBBASE
echo Looking in $DX_LIBDIRPATH

AC_ARG_WITH(DX_LIBDIRPATH, 
[  --with-OPENDX-include=<dx-include-dir>  set location of the auxiliary directory for OpenDX],
DX_LIBDIRPATH="$withval")

AC_PATH_PROGS(DXLIB, libDX.a, "", $DX_LIBDIRPATH)
if test -z "$DXLIB"; then
  AC_MSG_ERROR(libDX.a not found in $DX_LIBDIRPATH.  Use --with-OPENDX-libdir to set the location of libDX.a.)
fi
DX_LIBDIR=`dirname $DXLIB`
AC_SUBST(DX_LIBDIR)

