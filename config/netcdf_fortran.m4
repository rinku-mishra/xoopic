dnl ######################################################################
dnl
dnl File:	netcdf_fortran.m4
dnl
dnl Purpose:	Determine the locations of netcdf fortran
dnl             includes and libraries
dnl
dnl Version: $Id: netcdf_fortran.m4 2207 2007-03-05 01:05:09Z pletzer $
dnl
dnl Tech-X configure system
dnl ######################################################################


dnl Includes functions from txsearch.m4
builtin(include, config/txsearch.m4)

AC_ARG_WITH(netcdf-fortran-dir, [  --with-netcdf-fortran-dir=<netcdf fortran installation directory>],
	NETCDF_FORTRAN_DIR="$withval", NETCDF_FORTRAN_DIR="")

  AC_SUBST(NETCDF_FORTRAN_DIR)

NETCDF_FORTRAN_INCDIR=$NETCDF_FORTRAN_DIR/include
TX_PATH_FILE(NETCDF_INC, netcdf.inc, "", $NETCDF_FORTRAN_INCDIR)
if test -z "$NETCDF_INC"; then
  AC_MSG_WARN(netcdf.inc not found in $NETCDF_FORTRAN_INCDIR. Use --with-netcdf-fortran-dir=)
  ac_cv_have_netcdf_fortran=no
else
  ac_cv_have_netcdf_fortran=yes
  AC_SUBST(NETCDF_FORTRAN_INCDIR)
fi

NETCDF_FORTRAN_LIBDIR=$NETCDF_FORTRAN_DIR/lib
TX_PATH_FILE(NETCDF_LIB, libnetcdf.a, "", $NETCDF_FORTRAN_LIBDIR)
if test -z "$NETCDF_LIB"; then
  AC_MSG_WARN(libnetcdf.a not found in $NETCDF_FORTRAN_LIBDIR. Use --with-netcdf-fortran-dir=)
  ac_cv_have_netcdf_fortran=no
else
  ac_cv_have_netcdf_fortran=yes
  AC_SUBST(NETCDF_FORTRAN_LIBDIR)
fi


dnl ######################################################################
dnl
dnl    Write to summary file if defined
dnl
dnl ######################################################################
if test -n "$config_summary_file"; then
  if test $ac_cv_have_netcdf_fortran = yes; then
    echo "Using NetCDF Fortran with"                    >> $config_summary_file
    echo "  NETCDF_FORTRAN_INCDIR:    $NETCDF_FORTRAN_INCDIR"           >> $config_summary_file
    echo "  NETCDF_FORTRAN_LIBDIR:    $NETCDF_FORTRAN_LIBDIR"           >> $config_summary_file
    echo                                      >> $config_summary_file
  fi
fi

