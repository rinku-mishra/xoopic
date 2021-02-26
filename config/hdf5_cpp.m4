dnl ######################################################################
dnl
dnl Purpose: Determine the locations of C++ Interface for 
dnl          the hdf5 includes and libraries
dnl
dnl Version: $Id: hdf5_cpp.m4 1794 2006-05-09 20:44:38Z swsides $
dnl
dnl NOTE: This is a modified version of the hdf5.m4 file.
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Allow the user to specify an overall hdf5 directory.  If specified,
dnl we look for include and lib under this.
dnl
dnl ######################################################################

AC_ARG_WITH(hdf5-dir,[  --with-hdf5-dir=<location of hdf5 installation> ],HDF5_DIR="$withval",HDF5_DIR="")

dnl ######################################################################
dnl
dnl Find hdf5 includes - looking in include location if present,
dnl otherwise in dir/include if present, otherwise in default locations,
dnl first parallel, then serial.
dnl
dnl ######################################################################

AC_ARG_WITH(hdf5-incdir,[  --with-hdf5-incdir=<location of hdf5 includes> ],
HDF5_INCDIR="$withval",HDF5_INCDIR="")
if test -n "$HDF5_INCDIR"; then
  HDF5_INCPATH=$HDF5_INCDIR
elif test -n "$HDF5_DIR"; then
  HDF5_INCPATH=$HDF5_DIR/include
elif test "$parallel" = yes; then
  HDF5_INCPATH=$HOME/hdf5mpi/include:/usr/local/hdf5mpi/include:/loc/hdf5mpi/include:$HOME/hdf5/include:/usr/local/hdf5/include:/loc/hdf5/include:/usr/common/usg/hdf5/default/parallel/include
else
  HDF5_INCPATH=$HOME/hdf5/include:/usr/local/hdf5/include:/loc/hdf5/include:$HOME/hdf5mpi/include:/usr/local/hdf5mpi/include:/loc/hdf5mpi/include:/usr/common/usg/hdf5/default/serial/include
fi
AC_PATH_PROGS(HDF5_H, H5Cpp.h, "", $HDF5_INCPATH)
if test -z "$HDF5_H"; then
  AC_MSG_WARN(H5Cpp.h not found in $HDF5_INCPATH.  Set with --with-hdf5-incdir=)
  HDF5_INC=" "
  ac_cv_have_hdf5=no
else
  HDF5_INCDIR=`dirname $HDF5_H`
  AC_SUBST(HDF5_INCDIR)
  HDF5_INC=-I$HDF5_INCDIR
  AC_SUBST(HDF5_INC)
  HDF5_DIR=`dirname $HDF5_INCDIR`
  ac_cv_have_hdf5=yes
fi

dnl ######################################################################
dnl
dnl See if built parallel
dnl
dnl ######################################################################

if test $ac_cv_have_hdf5 = yes; then
  if test -f $HDF5_INCDIR/H5config.h; then
    hdf5par=`grep "HAVE_PARALLEL 1" $HDF5_INCDIR/H5config.h`
  elif test -f $HDF5_INCDIR/H5pubconf.h; then
    hdf5par=`grep "HAVE_PARALLEL 1" $HDF5_INCDIR/H5pubconf.h`
  fi
fi

dnl ######################################################################
dnl
dnl Find hdf5 libraries
dnl
dnl ######################################################################

AC_ARG_WITH(hdf5-libdir,[  --with-hdf5-libdir=<location of hdf5 library> ],
HDF5_LIBDIR="$withval",HDF5_LIBDIR="")
if test $ac_cv_have_hdf5 = yes; then
  if test -n "$HDF5_LIBDIR"; then
    HDF5_LIBPATH=$HDF5_LIBDIR
  else
    HDF5_LIBPATH=$HDF5_DIR/lib
  fi
  AC_PATH_PROGS(LIBHDF5_A, libhdf5_cpp.a, "", $HDF5_LIBPATH)
  if test -z "$LIBHDF5_A"; then
    AC_MSG_WARN(libhdf5_cpp.a not found.  Set with --with-hdf5-libdir=)
    ac_cv_have_hdf5=no
    HDF5_LIB=" "
  else
    HDF5_LIBDIR=`dirname $LIBHDF5_A`
    AC_SUBST(HDF5_LIBDIR)
    HDF5_LIB="-L$HDF5_LIBDIR -lhdf5"
  fi
  AC_SUBST(HDF5_LIB)
fi

dnl ######################################################################
dnl
dnl Define for whether hdf5 found
dnl
dnl ######################################################################

if test $ac_cv_have_hdf5 = yes; then
  AC_DEFINE([HAVE_HDF5], [], [tells if the HDF5 library is present])
fi

