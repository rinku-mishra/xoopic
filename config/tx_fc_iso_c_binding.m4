#
# SYNOPSIS
#
#   TX_FC_ISO_C_BINDING
#
# DESCRIPTION
#
#  Check whether the fortran compiler supports "use iso_c_binding"
#
#  $Id: tx_fc_iso_c_binding.m4 2395 2007-09-12 03:55:16Z pletzer $
#
AC_DEFUN([TX_FC_ISO_C_BINDING],[
AC_CACHE_CHECK([whether compiler has iso_c_binding],
ac_cv_have_iso_c_binding,
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
       use iso_c_binding
      end module conftest_module
  ],
  [ac_cv_have_iso_c_binding="yes"],
  [ac_cv_have_iso_c_binding="no"])
cd ..
rm -fr tmpdir_$i
AC_LANG_POP(Fortran)
])
if test "$ac_cv_have_iso_c_binding" = yes; then
   AC_DEFINE(HAVE_FC_ISO_C_BINDING,,[define if fortran compiler has iso_c_binding])
fi
TX_FORTRAN_ISO_C_BINDING="$ac_cv_have_iso_c_binding"
dnl ######################################################################
dnl
dnl Print out configuration information
dnl
dnl ######################################################################

if test -n "$config_summary_file"; then
   echo                                      >> $config_summary_file
   if test "$TX_FORTRAN_ISO_C_BINDING" = yes; then
      echo "Fortran compiler has 'use iso_c_binding'"  >> $config_summary_file
   else
      echo "Fortran compiler does NOT have 'use iso_c_binding'" >> $config_summary_file
   fi
fi
AC_SUBST(TX_FORTRAN_ISO_C_BINDING)
])
