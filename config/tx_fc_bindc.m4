#
# SYNOPSIS
#
#   TX_FC_BINDC
#
# DESCRIPTION
#
#  Check that the fortran compiler supports BIND(C)
#
#  $Id: tx_fc_bindc.m4 2395 2007-09-12 03:55:16Z pletzer $
#
AC_DEFUN([TX_FC_BINDC],[
AC_CACHE_CHECK([whether compiler supports BIND(C)],
ac_cv_have_bindc,
[AC_LANG_PUSH(Fortran)
i=0
while test \( -f tmpdir_$i \) -o \( -d tmpdir_$i \) ; do
  i=`expr $i + 1`
done
mkdir tmpdir_$i
cd tmpdir_$i
AC_COMPILE_IFELSE([
!234567
      module conftest_module
       type, bind(c) :: foo
	real :: x
       end type foo
      end module conftest_module
  ],
  [ac_cv_have_bindc="yes"],
  [ac_cv_have_bindc="no"])
cd ..
rm -fr tmpdir_$i
AC_LANG_POP(Fortran)
])
if test "$ac_cv_have_bindc" = yes; then
   AC_DEFINE(HAVE_FC_BINDC,,[define if fortran compiler has BIND(C) support])
fi
TX_FORTRAN_BINDC="$ac_cv_have_bindc"
dnl ######################################################################
dnl
dnl Print out configuration information
dnl
dnl ######################################################################

if test -n "$config_summary_file"; then
   echo                                      >> $config_summary_file
   if test "$TX_FORTRAN_BINDC" = yes; then
      echo "Fortran compiler supports BIND(C)"  >> $config_summary_file
   else
      echo "Fortran compiler does NOT support BIND(C)" >> $config_summary_file
   fi
fi
AC_SUBST(TX_FORTRAN_BINDC)
])

