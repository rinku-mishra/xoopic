dnl ######################################################################
dnl
dnl File:	sz.m4
dnl
dnl Purpose:	Determine the locations of sz includes and libraries
dnl
dnl Version: $Id: sz.m4 2400 2007-09-16 12:00:58Z cary $
dnl
dnl Tech-X configure system
dnl
dnl This is being split off from hdf5.m4, so it can be called
dnl after RPATH_FLAGS is defined.
dnl
dnl    SZIP_INCDIR
dnl    SZIP_INC
dnl    SZIP_LIBDIR
dnl    SZIP_LIBS
dnl
dnl ######################################################################

dnl Includes functions from txsearch.m4
builtin(include, config/txsearch.m4)

dnl ######################################################################
dnl
dnl Find szip libraries
dnl
dnl ######################################################################

if test -z "$SZ_PATH"; then
  SZ_PATH=$HOME/$UNIXFLAVOR/hdf5/include:/contrib/hdf5:/usr/local/hdf5:/usr/common/usg/hdf5_64/default/serial/include:/usr/common/usg/hdf5/64/default/serial/include
fi
# echo SZ_PATH = $SZ_PATH
TX_LOCATE_PKG(
    [SZ],
    [$SZ_PATH],
    [sz.h],
    [sz])

#, [bmake/$PETSC_ARCH], [lib/$PETSC_ARCH])

dnl ######################################################################
dnl
dnl Fix the RPATH_FLAG is possible
dnl
dnl ######################################################################

if test -z "$RPATH_FLAG"; then
  AC_MSG_WARN(RPATH_FLAG not defined.  Move sz.m4 after flags.m4.)
else
  SZ_LIBS=`echo $SZ_LIBS | sed -e 's/\$(//'  -e 's/)//' -e "s/RPATH_FLAG/$RPATH_FLAG/"`
fi
if test -n "$config_summary_file"; then
  echo "  SZ modifications"         >> $config_summary_file
  echo "    SZ_LIBS:   $SZ_LIBS"    >> $config_summary_file
fi

dnl JRC: this is wrong.  Changing to sz_*
dnl AC_SUBST(SZIP_INCDIR)
dnl AC_SUBST(SZIP_INC)
dnl AC_SUBST(SZIP_LIBDIR)
dnl AC_SUBST(SZIP_LIBS)

