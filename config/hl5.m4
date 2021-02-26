dnl ######################################################################
dnl
dnl File:	hl5.m4
dnl
dnl Purpose:	Determine where the hdf5 lite is.
dnl
dnl Version:	$Id: hl5.m4 1433 2005-01-28 21:25:19Z sveta $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

AC_ARG_WITH(hl5,
[  --with-hl5=<hl5> to set location of gsoap],
HDF5L_DIR="$withval")
if test -n "$HDF5L_DIR"; then
    echo "HDF5L_DIR set by user"
else
    HDF5L_DIR=/usr/local/hl5
fi

HDF5L_INCDIR="$HDF5L_DIR/include"
HDF5L_LIBDIR=$HDF5L_DIR/lib
HDF5L_LIBS="-L$HDF5L_LIBDIR -lhdf5_hl"

AC_DEFINE(HAVE_HDF5L)

AC_SUBST(HDF5L_DIR)
AC_SUBST(HDF5L_LIBS)
AC_SUBST(HDF5L_INCDIR)
AC_SUBST(HDF5L_LIBDIR)

    
