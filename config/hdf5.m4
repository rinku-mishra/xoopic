dnl ######################################################################
dnl
dnl File:	hdf5.m4
dnl
dnl Purpose:	Determine the locations of hdf5 includes and libraries
dnl
dnl Version: $Id: hdf5.m4 2396 2007-09-13 08:19:31Z cary $
dnl
dnl Tech-X configure system
dnl
dnl need to bring this into compliance:
dnl	HDF5_LIBDIR is the directory containing -lhdf5
dnl     HDF5_LIBS should be the entire variable needed, i.e.,
dnl		-L$(HDF5_LIBDIR) -lhdf5
dnl	HDF5_LIB should be known.  It is simply -lhdf5.  No need.
dnl	same should be for INCDIR and INCS.
dnl
dnl This m4 file defines:
dnl    HAVE_HDF5
dnl    HDF5_INCDIR
dnl    HDF5_INC
dnl    HDF5_LIBS
dnl    HDF5_LIBDIR
dnl
dnl  2005-02-30 - SEK
dnl    Added hdf5 enable option for codes that do not require hdf5
dnl ######################################################################

dnl Includes functions from txsearch.m4
builtin(include, config/txsearch.m4)

dnl ######################################################################
dnl
dnl Allow the user to specify an overall hdf5 directory.  If specified,
dnl we look for include and lib under this.
dnl
dnl ######################################################################

AC_ARG_WITH(hdf5-dir, [  --with-hdf5-dir=<hdf5 installation directory>],
	HDF5_DIR="$withval", HDF5_DIR="")

dnl ######################################################################
dnl
dnl Find hdf5 includes - looking in include location if present,
dnl otherwise in dir/include if present, otherwise in default locations,
dnl first parallel, then serial.
dnl
dnl ######################################################################

AC_ARG_WITH(hdf5-incdir, [  --with-hdf5-incdir=<hdf5 include directory>],
	HDF5_INCDIR="$withval")

dnl Specified directories
if test -n "$HDF5_INCDIR"; then
  HDF5_INCPATH=$HDF5_INCDIR
elif test -n "$HDF5_DIR"; then
  HDF5_INCPATH=$HDF5_DIR/include
fi

dnl If not specified, then default.
if test -z "$HDF5_INCPATH"; then
# Store for later use
  HDF5_SERINCPATH=$HOME/$UNIXFLAVOR/hdf5/include:/usr/local/hdf5/include:/loc/hdf5/include:/usr/common/usg/hdf5_64/default/serial/include:/usr/common/usg/hdf5/64/default/serial/include:/usr/include:/usr/local/include
  HDF5_PARINCPATH=$HOME/$UNIXFLAVOR/hdf5mpi/include:/usr/local/hdf5mpi/include:/loc/hdf5mpi/include:/usr/common/usg/hdf5_64/default/parallel/include:/usr/common/usg/hdf5/64/default/parallel/include:/usr/include:/usr/local/include
  if test "$USE_PAR_HDF5" = yes; then
### JRC: $HOME paths should be first to allow users to override
    HDF5_INCPATH=$HDF5_PARINCPATH:$HDF5_SERINCPATH
  else
    HDF5_INCPATH=$HDF5_SERINCPATH:$HDF5_PARINCPATH
  fi
fi

dnl Search for main header file
TX_PATH_FILE(HDF5_H, hdf5.h, "", $HDF5_INCPATH)

dnl error conditions
if test -z "$HDF5_H"; then
  AC_MSG_WARN(hdf5.h not found in $HDF5_INCPATH.  Use --with-hdf5-incdir=)
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

# HDF5_LIBDIR=" "
# HDF5_LIBS=" "
# echo HDF5_LIBDIR = $HDF5_LIBDIR
AC_ARG_WITH(hdf5-libdir, [  --with-hdf5-libdir=<hdf5 lib directory>],
	HDF5_LIBDIR="$withval")
	# HDF5_LIBDIR="$withval", HDF5_LIBDIR="")
if test $ac_cv_have_hdf5 = yes; then
  if test -n "$HDF5_LIBDIR"; then
    HDF5_LIBPATH=$HDF5_LIBDIR
  else
    HDF5_LIBPATH=$HDF5_DIR/lib
  fi
  # echo HDF5_LIBDIR = $HDF5_LIBDIR
  # echo HDF5_LIBPATH = $HDF5_LIBPATH
  TX_PATH_FILE(LIBHDF5, libhdf5.a, "", $HDF5_LIBPATH)
  if test -z "$LIBHDF5"; then
    TX_PATH_FILE(LIBHDF5, libhdf5$SO, "", $HDF5_LIBPATH)
  fi
  if test -z "$LIBHDF5"; then
    TX_PATH_FILE(LIBHDF5, libhdf5.dylib, "", $HDF5_LIBPATH)
  fi
  if test -z "$LIBHDF5"; then
    AC_MSG_WARN(libhdf5.a(so,dylib) not found.  Use --with-hdf5-libdir=)
    ac_cv_have_hdf5=no
  else
    HDF5_LIBDIR=`dirname $LIBHDF5`
    dnl HDF5_LIBS="-L$HDF5_LIBDIR -lhdf5 $GPFS_LIB"
    HDF5_LIBS="-L$HDF5_LIBDIR "'$(RPATH_FLAG)'"$HDF5_LIBDIR -lhdf5 $GPFS_LIB"
  fi
fi
AC_SUBST(HDF5_LIBDIR)
AC_SUBST(HDF5_LIBS)

dnl ######################################################################
dnl
dnl If built parallel, may need to include parallel file system library
dnl
dnl ######################################################################

if test -n "$hdf5par"; then
  GPFS_LIBPATH=/usr/lib:/usr/local/lib:/usr/lpp/mmfs/lib
  TX_PATH_FILE(GPFS_LIB, libgpfs.a, "", $GPFS_LIBPATH)
  if test -z "$GPFS_LIB"; then
    TX_PATH_FILE(GPFS_LIB, libgpfs$SO, "", $GPFS_LIBPATH)
  fi
  if test -n "$GPFS_LIB"; then
    GPFS_LIB=-lgpfs
  fi
fi

dnl ######################################################################
dnl
dnl Define for whether hdf5 found
dnl
dnl ######################################################################

if test $ac_cv_have_hdf5 = yes; then
  AC_DEFINE([HAVE_HDF5], [], [whether the HDF5 library is present])
fi

dnl ######################################################################
dnl
dnl Determine version - as H5Sselect_hyperslab interface changed
dnl
dnl ######################################################################

if test $ac_cv_have_hdf5 = yes; then
  H5_VERS_MAJOR=`grep '#define H5_VERS_MAJOR' $HDF5_INCDIR/H5public.h | sed 's/^.*H5_VERS_MAJOR//' | sed 's/\/.*$//' | tr -d [:space:]`
  H5_VERS_MINOR=`grep '#define H5_VERS_MINOR' $HDF5_INCDIR/H5public.h | sed 's/^.*H5_VERS_MINOR//' | sed 's/\/.*$//' | tr -d [:space:]`
  H5_VERS_RELEASE=`grep '#define H5_VERS_RELEASE' $HDF5_INCDIR/H5public.h | sed 's/^.*H5_VERS_RELEASE//' | sed 's/\/.*$//' | tr -d [:space:]`
  unset new_H5Sselect_hyperslab_ifc
  if test $H5_VERS_MAJOR -gt 1; then
    new_H5Sselect_hyperslab_ifc=true
  elif test $H5_VERS_MAJOR = 1; then
    if test $H5_VERS_MINOR -gt 6; then
      new_H5Sselect_hyperslab_ifc=true
    elif test $H5_VERS_MINOR = 6; then
      if test $H5_VERS_RELEASE -ge 4; then
        new_H5Sselect_hyperslab_ifc=true
      fi
    fi
  fi
  if test -n "$new_H5Sselect_hyperslab_ifc"; then
    AC_DEFINE([NEW_H5S_SELECT_HYPERSLAB_IFC], [],
	[whether the new H5Sselect_hyperslab interface is in use])
  fi
fi

dnl ######################################################################
dnl
dnl  h5diff
dnl
dnl ######################################################################

if test $ac_cv_have_hdf5 = yes; then
  HDF5_BINPATH=`dirname $HDF5_LIBDIR`/bin
  AC_PATH_PROGS(H5DIFF, h5diff, "", $HDF5_BINPATH)
  if test -z "$H5DIFF"; then
    if test -n "$MUST_HAVE_H5DIFF"; then
      AC_MSG_ERROR(h5diff not found in HDF5 tree.  Must reinstall HDF5.)
    fi
  else
    AC_SUBST(H5DIFF)
    if test -z "$new_H5Sselect_hyperslab_ifc"; then
      if test -n "$MUST_HAVE_H5DIFF"; then
        AC_MSG_ERROR(HDF5 must be at version 1.6.4 for regression tests to work.)
      fi
    else
      echo h5diff of version 1.6.4 or greater found.
    fi
  fi
else
  if test -n "$MUST_HAVE_H5DIFF"; then
    AC_MSG_ERROR(h5diff not found.  Upgrade hdf5)
  fi
fi

dnl ######################################################################
dnl  Fortran support.
dnl  Problem is that hdf5 f90 wrappers need to be compiled with same
dnl   compiler as code compiler.  Perform check using hdf5 settings file
dnl  Note to use this functionality, one needs to set in configure.in:
dnl   ac_use_fortran_hdf5=true        # Perform fortan hdf5 tests
dnl ######################################################################

if test "$FC_BASE" != none && test -n "$FC_BASE" && test $ac_use_fortran_hdf5 = true; then
dnl test to make sure hdf5_fortran exists
  if test -f $HDF5_LIBDIR/libhdf5_fortran.a; then
dnl test to make sure hdf5_fortran exists
    hdf5_f90_full=`grep 'Fortran Compiler' $HDF5_LIBDIR/libhdf5_fortran.settings | cut -f2 -d:`
    hdf5_f90=`basename $hdf5_f90_full`
    if test "$FC_BASE" = $hdf5_f90; then
dnl This is to handle the Absoft weirdness
      HDF5_INCLUDES="$FC_INCLUDE_FLAG$HDF5_LIBDIR"
      HDF5_LIB="-L$HDF5_LIBDIR -lhdf5_fortran -lhdf5"
      AC_SUBST(HDF5_LIB)
      AC_SUBST(HDF5_INCLUDES)
    else
      AC_MSG_WARN(Fortran compiler used to compile hdf5 bindings --
	$hdf5_f90 not the same as your fortran compiler -- $FC.  Turning
	off hdf5.)
      ac_cv_have_hdf5=no
      HDF5_DIR=""
      HDF5_LIB=""
      HDF5_LIBDIR=""
      HDF5_INC=""
      HDF5_INCDIR=""
    fi
  else
    AC_MSG_WARN(hdf5_fortran.a not found in $HDF5_LIBDIR. Turning off HDF5)
    ac_cv_have_hdf5=no
    HDF5_DIR=""
    HDF5_LIB=""
    HDF5_LIBDIR=""
    HDF5_INC=""
    HDF5_INCDIR=""
  fi
fi

dnl ######################################################################
dnl
dnl Allow the user to specify an overall szip directory.  If specified,
dnl we look for include and lib under this.
dnl
dnl ######################################################################

dnl AC_ARG_WITH(szip-dir, [  --with-szip-dir=<szip installation directory> ],
dnl 	SZIP_DIR="$withval", SZIP_DIR="")

dnl ######################################################################
dnl
dnl Determine whether szip needed
dnl
dnl ######################################################################

HDF5_NEED_SZIP=`grep H5_HAVE_FILTER_SZIP $HDF5_INCDIR/H5pubconf.h | grep define`
if test -n "$HDF5_NEED_SZIP"; then
  AC_MSG_WARN(hdf5.m4 has changed.  Please be certain to include sz.m4.)
fi

dnl if test -n "$NEEDSZIP"; then
dnl   AC_ARG_WITH(szip-incdir, [  --with-szip-incdir=<szip include directory>],
dnl 	SZIP_INCDIR="$withval", )
dnl   if test -n "$SZIP_INCDIR"; then
dnl     SZIP_INCPATH=$SZIP_INCDIR
dnl   elif test -n "$SZIP_DIR"; then
dnl     SZIP_INCPATH=$SZIP_DIR/include
dnl   else
dnl     SZIP_INCPATH=/usr:$HOME/szip/include:/usr/local/szip/include:/loc/szip/include:/usr/common/usg/szip/default/include:/usr/common/usg/szip/2.0/include
dnl   fi
dnl dnl  AC_PATH_PROGS(SZLIB_H, szlib.h, "", $SZIP_INCPATH)
dnl   TX_PATH_FILE(SZLIB_H, szlib.h, "", $SZIP_INCPATH)
dnl   if test -z "$SZLIB_H"; then
dnl     AC_MSG_WARN(szlib.h not found in $SZIP_INCPATH.  Use --with-szip-incdir=)
dnl     ac_cv_have_szip=no
dnl   else
dnl     SZIP_INCDIR=`dirname $SZLIB_H`
dnl     SZIP_INC=-I$SZIP_INCDIR
dnl     SZIP_DIR=`dirname $SZIP_INCDIR`
dnl     ac_cv_have_szip=yes
dnl   fi
dnl else
dnl   ac_cv_have_szip=no
dnl fi
dnl AC_SUBST(SZIP_INCDIR)
dnl AC_SUBST(SZIP_INC)

dnl ######################################################################
dnl
dnl Find szip libraries
dnl
dnl ######################################################################

dnl AC_ARG_WITH(szip-libdir, [  --with-szip-libdir=<szip lib directory>],
dnl 	SZIP_LIBDIR="$withval")
dnl # if test $ac_cv_have_szip = yes; then
dnl   if test -n "$SZIP_LIBDIR"; then
dnl     SZIP_LIBPATH=$SZIP_LIBDIR
dnl   elif test -n "$SZIP_DIR"; then
dnl     SZIP_LIBPATH=$SZIP_DIR/lib
dnl   elif test -n "$HDF5_LIBDIR"; then
dnl     SZIP_LIBPATH=$HDF5_LIBDIR
dnl   fi
dnl   # echo SZIP_LIBPATH=$SZIP_LIBPATH
dnl   # echo SZIP_LIBDIR=$SZIP_LIBDIR
dnl   if test -n "$SZIP_LIBPATH"; then
dnl     TX_PATH_FILE(LIBSZIP, libsz.a,"", $SZIP_LIBPATH)
dnl     if test -z "$LIBSZIP"; then
dnl       TX_PATH_FILE(LIBSZIP, libsz$SO, "", $SZIP_LIBPATH)
dnl     fi
dnl     if test -z "$LIBSZIP"; then
dnl       TX_PATH_FILE(LIBSZIP, libsz.dylib, "", $SZIP_LIBPATH)
dnl     fi
dnl     if test -z "$LIBSZIP"; then
dnl       AC_MSG_WARN(libsz.a not found.  Use --with-szip-libdir= if needed.)
dnl       ac_cv_have_szip=no
dnl     else
dnl       SZIP_LIB=-lsz
dnl       SZIP_LIBDIR=`dirname $LIBSZIP`
dnl       SZIP_LIBS="-L$SZIP_LIBDIR "'$(RPATH_FLAG)'"$SZIP_LIBDIR -lsz"
dnl     fi
dnl   else
dnl     AC_MSG_WARN(libsz.a not found.  Use --with-szip-libdir= if needed.)
dnl     ac_cv_have_szip=no
dnl   fi
dnl # fi
dnl AC_SUBST(SZIP_LIB)
dnl AC_SUBST(SZIP_LIBDIR)
dnl AC_SUBST(SZIP_LIBS)
dnl AM_CONDITIONAL([HAVE_SZIP], [test -n "$SZIP_LIBS"])

dnl ######################################################################
dnl
dnl    Write to summary file if defined
dnl
dnl ######################################################################
if test -n "$config_summary_file"; then
  if test $ac_cv_have_hdf5 = yes; then
    echo                     >> $config_summary_file
    echo "Using HDF5 with"                    >> $config_summary_file
    echo "  HDF5_INC:    $HDF5_INC"           >> $config_summary_file
    echo "  HDF5_LIBDIR: $HDF5_LIBDIR"        >> $config_summary_file
    echo "  HDF5_LIBS:   $HDF5_LIBS"          >> $config_summary_file
dnl     echo "  SZIP_INC:    $SZIP_INC"           >> $config_summary_file
dnl     echo "  SZIP_LIBDIR: $SZIP_LIBDIR"        >> $config_summary_file
dnl     echo "  SZIP_LIBS:   $SZIP_LIBS"          >> $config_summary_file
  fi
fi

# For a parallel build, we'd still like vpdatanal built serially. The compiler
# is set properly in vpdatanal/Makefile.am, but even with a serial compiler
# it will try to use parallel HDF5. The vars below are used exlusively by
# vpdatanal/Makefile.am to force the use of serial HDF5. If it doesn't exist,
# the parallel version will be used, as before.

HDF5_INCDIR_SERIAL=`echo $HDF5_INCDIR | sed 's/mpi//'`
HDF5_LIBDIR_SERIAL=`echo $HDF5_LIBDIR | sed 's/mpi//'`
HDF5_LIBS_SERIAL=`echo $HDF5_LIBS | sed 's/mpi//'`

if test ! -d "$HDF5_LIBDIR_SERIAL" -o ! -d "$HDF5_INCDIR_SERIAL"; then
  cat <<END
Failed to find serial build of HDF5 in obvious location. Setting _SERIAL vars
to be the same as the parallel vars so as not to break the vpdatanal builds.
END
  HDF5_INCDIR_SERIAL=$HDF5_INCDIR
  HDF5_LIBDIR_SERIAL=$HDF5_LIBDIR
  HDF5_LIBS_SERIAL=$HDF5_LIBS
fi

AC_SUBST(HDF5_INCDIR_SERIAL)
AC_SUBST(HDF5_LIBDIR_SERIAL)
AC_SUBST(HDF5_LIBS_SERIAL)

