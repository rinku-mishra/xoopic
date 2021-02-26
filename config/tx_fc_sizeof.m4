#
# SYNOPSIS
#
#   TX_FC_SIZEOF
#
# DESCRIPTION
#
#  Determine size of Fortran types
#
#  $Id: tx_fc_sizeof.m4 2398 2007-09-14 22:56:34Z pletzer $
#
AC_DEFUN([TX_FC_SIZEOF],[
AS_VAR_PUSHDEF([type_var], [ac_cv_sizeof_$1])
AC_CACHE_CHECK([size of Fortran $1],
ac_cv_sizeof_$1,[
cat >conftestf.f90<<EOF
!234567
      program test
	$1 x(2)
	integer sz
	call getsize(x(1), x(2), sz)
	print *, sz
      end
EOF
cat > conftestc.c<<EOF
void GetSize(void *e1, void *e2, int *sz) {
    *sz = (int)(e2 - e1);
}
void getsize(void *e1, void *e2, int *sz) {
    GetSize(e1, e2, sz);
}
void getsize_(void *e1, void *e2, int *sz) {
    GetSize(e1, e2, sz);
}
void GETSIZE(void *e1, void *e2, int *sz) {
    GetSize(e1, e2, sz);
}
EOF
$FC $FCFLAGS -c conftestf.f90 -o conftestf.o
$CC $CFLAGS conftestc.c conftestf.o -o conftest $FCLIBS
rm -f size.txt
./conftest > size.txt
rm -f ./conftest
size=`cat size.txt`
ac_cv_sizeof_$1="$size"
])
TX_FORTRAN_SIZEOF_$1="$ac_cv_sizeof_$1"
AC_SUBST(TX_FORTRAN_SIZEOF_$1)
])
