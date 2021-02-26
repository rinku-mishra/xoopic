#
# SYNOPSIS
#
#   TX_FC_ALLOCATABLE_COMPONENT
#
# DESCRIPTION
#   
#   Check that the fortran compiler supports allocatable components
#   in derived types.
#
#   $Id: tx_fc_allocatable_component.m4 2394 2007-09-11 23:06:22Z pletzer $
#
AC_DEFUN([TX_FC_ALLOCATABLE_COMPONENT],[
AC_CACHE_CHECK([allocatable component in derived type],
ac_cv_have_allocatable_component,
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
       type foo
	real, allocatable :: x(:)
       end type foo
      end module conftest_module
  ],
  [ac_cv_have_allocatable_component="yes"],
  [ac_cv_have_allocatable_component="no"])
cd ..
rm -fr tmpdir_$i
AC_LANG_POP(Fortran)
])
if test "$ac_cv_have_allocatable_component" = yes; then
   AC_DEFINE(HAVE_FC_ALLOCATABLE_COMPONENT,,[define if fortran compiler supports allocatable components])
fi
TX_FORTRAN_ALLOCATABLE_COMPONENT="$ac_cv_have_allocatable_component"
AC_SUBST(TX_FORTRAN_ALLOCATABLE_COMPONENT)
])
