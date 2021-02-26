dnl ######################################################################
dnl
dnl File:	rsiidl.m4
dnl
dnl Purpose:	Check for RSI's IDL
dnl
dnl Version:	$Id: rsiidl.m4 2371 2007-08-19 15:59:49Z cary $
dnl
dnl Tech-X configuration files, all rights reserved.
dnl
dnl ######################################################################

dnl Get the executable

dnl AC_ARG_ENABLE(rsiidl,[  --disable-rsiidl       turn off rsiidl features],,enable_rsiidl=yes)
dnl echo "RSIIDL enable flag is..." $enable_rsiidl
dnl if test "$enable_rsiidl" = "yes"; then

# Determine whether RSIIDL_NOT_REQUIRED
if test "$RSIIDL_NOT_REQUIRED" = no; then
  RSIIDL_NOT_REQUIRED=""
fi
# echo RSIIDL_NOT_REQUIRED = \'$RSIIDL_NOT_REQUIRED\'

AC_ARG_WITH(rsiidl-bindir,
[  --with-rsiidl-bindir=<rsiidl-bindir>  set location of the RSI-IDL idl binary],
RSIIDL_BINDIR="$withval")

AC_ARG_WITH(rsiidl-libdir,
[  --with-rsiidl-libdir=<rsiidl-libdir>  set location of the RSI-IDL idl libraries],
RSIIDL_LIBBIN="$withval")

if test -z "$RSIIDL_BINDIR"; then
  IDL_PATH=/usr/local/rsi/idl/bin:/usr/local/pkg/graphics/rsi/idl/bin:/Applications/rsi/idl/bin
  AC_PATH_PROGS(RSIIDL_BIN, idl, "", $IDL_PATH)
  if test -z "$RSIIDL_BIN"; then
    if test -n "$RSIIDL_NOT_REQUIRED"; then
      AC_MSG_WARN(RSI-IDL not found in $IDL_PATH.  Use --with-rsiidl-dir to set the location of the rsi-idl binary)
    else
      AC_MSG_ERROR(RSI-IDL not found in $IDL_PATH.  Use --with-rsiidl-dir to set the location of the rsi-idl binary)
    fi
  else
    RSIIDL_BINDIR=`dirname $RSIIDL_BIN`
  fi
fi
# echo RSIIDL_BINDIR = $RSIIDL_BINDIR

dnl Get the location of the external header, if executable found
if test -n "$RSIIDL_BINDIR"; then
  RSIIDL_DIR=`dirname $RSIIDL_BINDIR`

# if this test succeeds then using version 5.6 or greater
  RSIIDL_INCDIR="$RSIIDL_DIR/external/include"
  OSMESA_LIB="-lOSMesa4_0 -lMesaGLU4_0 -lMesaGL4_0"
  AC_PATH_PROGS(RSIIDL_HASINC, idl_export.h, "", $RSIIDL_INCDIR)

# This is for version 5.5 or earlier
  if test -z "$RSIIDL_HASINC"; then
    RSIIDL_INCDIR=$RSIIDL_DIR/external
    OSMESA_LIB="-lMesaGLU -lMesaGL"
    AC_PATH_PROGS(RSIIDL_HASINC, export.h, "", $RSIIDL_INCDIR)
    if test -z "$RSIIDL_HASINC"; then
      AC_MSG_ERROR(RSI-IDL header file not found in $RSIIDL_INCDIR.  Use --with-RSIIDL to set the location of the rsi-idl binary)
    fi
  fi

  # Keeping these two lines for backwards compatability
  IDLEXT_DIR=$RSIIDL_INCDIR

  # Check for path to libidl$SO
  case "$host" in

    *-linux*)
      RSIIDL_LIBBIN="bin.linux.x86"
      ;;

    *-sgi*)
      RSIIDL_LIBBIN="bin.sgi"
      ;;

    i686-*-darwin*)
      RSIIDL_LIBBIN="bin.darwin.i386"
      ;;

    *-darwin*)
      RSIIDL_LIBBIN="bin.darwin.ppc"
      ;;

  esac

  if test -z "$RSIIDL_LIBBIN"; then
     AC_MSG_ERROR(Can't find RSI-IDL libidl$SO on this platform)
  fi
  # end check for libidl$SO

fi	# end check assuming RSIIDL_DIR found

dnl fi
# end check for disable-rsiidl
AC_SUBST(RSIIDL_BINDIR)
AC_SUBST(RSIIDL_DIR)
AC_SUBST(OSMESA_LIB)
AC_SUBST(RSIIDL_INCDIR)
AC_SUBST(RSIIDL_LIBBIN)
AC_SUBST(IDLEXT_DIR)
