

# _TX_FC_MAIN_OBJS
# ----------------------
#
# Alexander Pletzer Tech-X (pletzer@txcorp.com)
#
# Determine the fortran object files that initialize fortran IO
# and call MAIN__ (or whatever fortran uses as an entry point to
# PROGRAM).
#
# This macro is intended to be used in those situations when it is
# necessary to mix, e.g. C++ and Fortran, source code into a single
# program or shared library.
#
# For example, if object files from a C++ and Fortran compiler must
# be linked together, then the C++ compiler/linker must be used for
# linking (since special C++-ish things need to happen at link time
# like calling global constructors, instantiating templates, enabling
# exception support, etc.).
#
# In order to be able to use fortran routines such as iargc(), for
# instance, which returns the number of command line arguments (an
# extension supported by many compilers), not only is it necessary to
# resolve MAIN__ (or equivalent), but some initialization and 
# finalization calls must be invoked prior and after calling MAIN__.
# This is done in object files that ship with each compiler. The name
# of these object files is inferred from the output of the $FC -v 
# where -v can also be -verbose or --verbose, or -V.  The retained 
# object files are those that contain the "main" symbol.
#
#
AC_DEFUN([_TX_FC_MAIN_OBJS],
[_AC_FORTRAN_ASSERT()dnl
_AC_PROG_FC_V
AC_CACHE_CHECK([Fortran object files that call PROGRAM using $[]_AC_FC[]], ac_cv_[]_AC_LANG_ABBREV[]_objs,
[if test "x$[]_AC_LANG_PREFIX[]OBJS" != "x"; then
  ac_cv_[]_AC_LANG_ABBREV[]_objs="$[]_AC_LANG_PREFIX[]OBJS" # Let the user override the test.
else

_AC_PROG_FC_V_OUTPUT

ac_cv_[]_AC_LANG_ABBREV[]_objs=

# Save positional arguments (if any)
ac_save_positional="$[@]"

set X $ac_[]_AC_LANG_ABBREV[]_v_output
while test $[@%:@] != 1; do
  shift
  ac_arg=$[1]
  case $ac_arg in
        [[\\/]]*.$OBJEXT | ?:[[\\/]]*.$OBJEXT)
	  case $ac_arg in
		# exclude these 
		/tmp*.$OBJEXT | *crtbegin.$OBJEXT | *crtend.$OBJEXT | *crtn.$OBJEXT | *crti.$OBJEXT | *crt1.$OBJEXT)
			;;
		*)
	        # only include this object file if it contains the 
		# main symbol
		out=`nm $ac_arg | grep main`
		if test "x$out" != "x"; then
          		_AC_LIST_MEMBER_IF($ac_arg, $ac_cv_[]_AC_LANG_ABBREV[]_objs, ,
              		ac_cv_[]_AC_LANG_ABBREV[]_objs="$ac_cv_[]_AC_LANG_ABBREV[]_objs $ac_arg")
		fi
          	;;
		
	  esac
  esac
done
# restore positional arguments
set X $ac_save_positional; shift

fi # test "x$[]_AC_LANG_PREFIX[]OBJS" = "x"
])
[]_AC_LANG_PREFIX[]OBJS="$ac_cv_[]_AC_LANG_ABBREV[]_objs"
AC_SUBST([]_AC_LANG_PREFIX[]OBJS)
])# _TX_FC_MAIN_OBJS



# TX_FC_FIND_MAIN_OBJS
# ----------------------
AC_DEFUN([TX_FC_FIND_MAIN_OBJS],
[AC_REQUIRE([AC_PROG_FC])dnl
AC_LANG_PUSH(Fortran)dnl
_TX_FC_MAIN_OBJS
TX_FC_MAIN_OBJS="$[]_AC_LANG_PREFIX[]OBJS"
AC_LANG_POP(Fortran)dnl
AC_SUBST(TX_FC_MAIN_OBJS)
])# TX_FC_FIND_MAIN_OBJS

# TX_F77_FIND_MAIN_OBJS
# ----------------------
AC_DEFUN([TX_F77_FIND_MAIN_OBJS],
[AC_REQUIRE([AC_PROG_F77])dnl
AC_LANG_PUSH(Fortran 77)dnl
_TX_FC_MAIN_OBJS
TX_F77_MAIN_OBJS="$[]_AC_LANG_PREFIX[]OBJS"
AC_LANG_POP(Fortran 77)dnl
AC_SUBST(TX_F77_MAIN_OBJS)
])# TX_F77_FIND_MAIN_OBJS

