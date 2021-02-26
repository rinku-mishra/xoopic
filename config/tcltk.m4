dnl ######################################################################
dnl
dnl File:	tcltk.m4
dnl
dnl Purpose:	Find Tcl/Tk, tclsh and setup flags
dnl
dnl Version:	$Id: tcltk.m4 1516 2005-04-29 15:24:53Z cary $
dnl             changes: JK, 2013-08-08 -- checking for existance of tclConfig.sh
dnl
dnl Copyright 2003-2005 Tech-X Corporation.  This file may be freely
dnl redistributed and modified provided the above copyright remains.
dnl
dnl ######################################################################

dnl AC_CHECKING(Tcl/Tk installation)
legal_tk_versions=" 8.5 8.4 8.3 8.2 8.1 8.0 4.9 4.8 4.7 4.6 4.5 4.4 4.3 4.2 4.1"
legal_tcl_versions="8.5 8.4 8.3 8.2 8.1 8.0 7.9 7.8 7.7 7.6 7.5"

#### Find tclsh
AC_ARG_WITH(tclsh,[  --with-tclsh=<path>     full path name of tclsh],
        tclsh="$withval")
if test -n "$tclsh"; then
    AC_MSG_CHECKING(for tclsh)
    AC_MSG_RESULT($tclsh)
else
    tclsh_list=tclsh
    for v in $legal_tcl_versions; do
        tclsh_list="$tclsh_list tclsh$v"
    done
    # Look for tclsh variants in $prefix/bin, and then in $PATH.
    AC_PATH_PROGS(tclsh, $tclsh_list, "", $PATH:/usr/lib)
fi
if test -z "$tclsh"; then
    AC_MSG_ERROR(I could not find tclsh in your search PATH.

Please supply the location of a tclsh executable by
running configure with the following option
        --with-tclsh=<full path name of tclsh>)
fi

#### Run tclsh to find tclConfig.sh
## AC_MSG_CHECKING(for tclConfig.sh)
AC_ARG_WITH(tclconfig,
[  --with-tclconfig=<path> directory that contains tclConfig.sh],
tclconfig="$withval")
# if test -z "$tclconfig"; then
    # changequote(,)
    # tclconfig=`echo 'puts stdout [file dirname [info library]]' | $tclsh`
    # changequote([,])
# fi

# JRC: This file should be rewritten to follow standard rules:
# Check for specification.  If not specified, then look
# in a set of places.
if test -z "$tclconfig"; then
  tclconfig=$PATH:/usr/lib:/usr/lib64:/usr/local/lib:/usr/local/lib64:/lib:/lib64
fi

# original: test for tclConfig.sh for existance and with executable flag (which is not necessary!)
#AC_PATH_PROGS(tclconfigsh, tclConfig.sh, "", $tclconfig)
tmp_IFS=$IFS ; IFS=$PATH_SEPARATOR

for tmp_path in $tclconfig; do
  if test -f "${tmp_path}/tclConfig.sh"; then
    tclconfigsh=${tmp_path}/tclConfig.sh
    break
  fi
done

IFS=$tmp_IFS

if test -z "$tclconfigsh"; then
  AC_MSG_ERROR(I could not find tclConfig.sh.


Please specify the directory that contains tclConfig.sh
by running configure with the following option
        --with-tclconfig=<path name of directory containing tclConfig.sh>

Warning: Only Tcl versions 7.5 and later provide a tclConfig.sh file.
Make sure you have a new enough version installed on your
system.)
fi
tclconfig=`dirname $tclconfigsh`
dnl AC_MSG_RESULT($tclconfig/tclConfig.sh)

#### Look for tkConfig.sh (in same directory as tclConfig.sh?)
AC_MSG_CHECKING(for tkConfig.sh)
AC_ARG_WITH(tkconfig,
[  --with-tkconfig=<path>  directory that contains tkConfig.sh],
tkconfig="$withval")
if test -z "$tkconfig"; then
    tkconfig="$tclconfig"
fi
if test ! -f "$tkconfig/tkConfig.sh"; then
    AC_MSG_ERROR(I could not find tkConfig.sh.

Please specify the directory that contains tkConfig.sh
by running configure with the following option
        --with-tkconfig=<path name of directory containing tkConfig.sh>

Warning: Only Tk versions 4.1 and later provide a tkConfig.sh file.
Make sure you have a new enough version installed on your system.)
fi
AC_MSG_RESULT($tkconfig/tkConfig.sh)

#### Load the Tcl/Tk configuration files
. $tclconfig/tclConfig.sh
. $tkconfig/tkConfig.sh

if test -z "$TCL_EXEC_PREFIX"; then TCL_EXEC_PREFIX="$TCL_PREFIX"; fi
if test -z "$TK_EXEC_PREFIX"; then TK_EXEC_PREFIX="$TK_PREFIX"; fi

#### Check that the library files exist
# AC_MSG_CHECKING(for installed Tcl/Tk library files)
# lib1="${TCL_EXEC_PREFIX}/lib/${TCL_LIB_FILE}"
AC_PATH_PROGS(TCL_LIBDIR_PATH,libtcl${TCL_VERSION}.a,"",${TCL_EXEC_PREFIX}/lib:${TCL_EXEC_PREFIX}/lib64)
if test -z "$TCL_LIBDIR_PATH"; then
  AC_PATH_PROGS(TCL_LIBDIR_PATH,libtcl${TCL_VERSION}$SO,"",${TCL_EXEC_PREFIX}/lib:${TCL_EXEC_PREFIX}/lib64)
fi
if test -z "$TCL_LIBDIR_PATH"; then
        AC_MSG_ERROR(Make sure Tcl/Tk are installed correctly on your
 system and then rerun configure.)
fi

# lib2="${TK_EXEC_PREFIX}/lib/${TK_LIB_FILE}"
AC_PATH_PROGS(TK_LIBDIR_PATH,libtk${TK_VERSION}.a,"",${TK_EXEC_PREFIX}/lib:${TK_EXEC_PREFIX}/lib64)
if test -z "$TK_LIBDIR_PATH"; then
  AC_PATH_PROGS(TK_LIBDIR_PATH,libtk${TK_VERSION}$SO,"",${TK_EXEC_PREFIX}/lib:${TK_EXEC_PREFIX}/lib64)
fi
if test -z "$TK_LIBDIR_PATH"; then
        AC_MSG_ERROR(Make sure Tcl/Tk are installed correctly on your
 system and then rerun configure.)
fi

# for f in "$lib1" "$lib2"; do
#     if test ! -f "$f"; then
#         AC_MSG_RESULT($f not found)
#         AC_MSG_ERROR(Make sure Tcl/Tk are installed correctly on your
# system and then rerun configure.)
#     fi
# done
# AC_MSG_RESULT(ok)

#### Find the flags for linking with the Tcl/Tk libraries
if test "$TCL_LIB_VERSIONS_OK" = "ok"; then TCL_LIB_VERSIONS_OK=yes; fi
if test "$TCL_LIB_VERSIONS_OK" = "1";  then TCL_LIB_VERSIONS_OK=yes; fi

if test -z "$TCL_LIB_SPEC"; then
    if test "$TCL_LIB_VERSIONS_OK" = "yes"; then
        TCL_LIB_SPEC="-L${TCL_EXEC_PREFIX}/lib -ltcl${TCL_VERSION}"
    else
        TCL_LIB_SPEC="${TCL_EXEC_PREFIX}/lib/${TCL_LIB_FILE}"
    fi
fi

if test -z "$TK_LIB_SPEC"; then
    if test "$TCL_LIB_VERSIONS_OK" = "yes"; then
        TK_LIB_SPEC="-L${TK_EXEC_PREFIX}/lib -ltk${TK_VERSION}"
    else
        TK_LIB_SPEC="${TK_EXEC_PREFIX}/lib/${TK_LIB_FILE}"
    fi
fi

#### Check Tcl/Tk versions

AC_REQUIRE_CPP()

AC_MSG_CHECKING(for up-to-date Tcl version in tcl.h)
AC_ARG_WITH(tclhdir,
[  --with-tclhdir=<path>   directory that contains tcl.h],
        tclhdir="$withval")
if test -z "$tclhdir"; then
    # Search for tcl.h
    tclhdir="${TCL_PREFIX}/include"
    for dir in "${TCL_PREFIX}/include" /usr/include /usr/include/tcl; do
        if test -f "$dir/tcl.h"; then
            tclhdir="$dir"
            break
            fi
        done
fi

# Find tk.h
AC_ARG_WITH(tkhdir,
[  --with-tkhdir=<path>    directory that contains tk.h],
        tkhdir="$withval")
if test -z "$tkhdir"; then
    # Search for tk.h
    tkhdir="$tclhdir"
    for dir in "${TK_PREFIX}/include" /usr/include /usr/include/tcl; do
        if test -f "$dir/tk.h"; then
            tkhdir="$dir"
            break
            fi
        done
fi

AC_EGREP_CPP(good_tcl_version,
[#include "$tclhdir/tcl.h"
#if ((TCL_MAJOR_VERSION == $TCL_MAJOR_VERSION) && (TCL_MINOR_VERSION == $TCL_MINOR_VERSION))
    good_tcl_version
#endif],is_good_tcl_version=yes,is_good_tcl_version=no)
AC_MSG_RESULT($TCL_MAJOR_VERSION.$TCL_MINOR_VERSION)
tcl_version="NONE"
for v in $legal_tcl_versions; do
   if test "$v" = "$TCL_MAJOR_VERSION.$TCL_MINOR_VERSION"; then
      tcl_version="$v"
   fi
done
if test "$tcl_version" = "NONE"; then
   echo "Warning: The Tcl version in $tclhdir/tcl.h is not in the legal Tcl versions list!"
   echo "tcl_version: " $tcl_version
   echo "TCL_MAJOR_VERSION: " $TCL_MAJOR_VERSION
   echo "TCL_MINOR_VERSION: " $TCL_MINOR_VERSION
fi


AC_MSG_CHECKING(for up-to-date Tk version in tk.h)
AC_EGREP_CPP(good_tk_version,
[#include "$tkhdir/tk.h"
#if ((TK_MAJOR_VERSION == $TK_MAJOR_VERSION) && (TK_MINOR_VERSION == $TK_MINOR_VERSION))
    good_tk_version
#endif],is_good_tk_version=yes,is_good_tk_version=no)

AC_MSG_RESULT($TK_MAJOR_VERSION.$TK_MINOR_VERSION)
tk_version="NONE"
for v in $legal_tk_versions; do
   if test "$v" = "$TK_MAJOR_VERSION.$TK_MINOR_VERSION"; then
      tk_version="$v"
   fi
done
if test "$tk_version" = "NONE"; then
   echo "Warning: The Tk version in $tclhdir/tcl.h is not in the legal Tk versions list!"
   echo "tk_version: " $tk_version
   echo "TK_MAJOR_VERSION: " $TK_MAJOR_VERSION
   echo "TK_MINOR_VERSION: " $TK_MINOR_VERSION
fi


TCL_INCDIR="$tclhdir"
TK_INCDIR="$tkhdir"
TCL_LIB="$TK_LIB_SPEC $TCL_LIB_SPEC"

export TCL_MAJOR_VERSION
export TCL_MINOR_VERSION
AC_SUBST(TCL_INCDIR)
AC_SUBST(TK_INCDIR)
AC_SUBST(TCL_LIB)


#### End TCL/TK

