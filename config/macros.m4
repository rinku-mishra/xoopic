dnl
dnl
dnl

dnl ----------------------------------------------------------------------
dnl Macro like AC_TRY_COMPILE, but generate code outside a function body
dnl ----------------------------------------------------------------------

dnl
dnl AC_TRY_COMPILE_GLOBAL(INCLUDES, CODE-SEGMENT,
dnl                       [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])

AC_DEFUN([AC_TRY_COMPILE_GLOBAL],
[cat > conftest.$ac_ext <<EOF
[#]line __oline__ "configure"
#include "confdefs.h"
[$1]
[$2]
int main() { return 0; }
EOF
if AC_TRY_EVAL(ac_compile)
then
    ifelse([$3], , :, [rm -rf conftest*
    $3])
ifelse([$4], , , [else
    rm -rf conftest*
    $4
])dnl
fi
rm -f conftest*]
)
