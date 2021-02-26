dnl ######################################################################
dnl
dnl File:       txsearch.m4
dnl
dnl Purpose:    Defines a series of macros to be used to search for common
dnl		files, directores, libraries, and includes.
dnl
dnl Functions defined:
dnl   TX_PATH_FILE
dnl   TX_PATH_FILES
dnl
dnl Version:    $Id: txsearch.m4 52 2007-05-30 19:52:54Z pletzer $
dnl
dnl Copyright Tech-X Corporation, 2001-2005.  Redistribution allowed
dnl provided this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Function:	TX_PATH_FILE( variable , file-to-check-for ,
dnl	[value-if-not-found] , [path] )
dnl
dnl Purpose:	Like AC_PATH_PROG but searches for a file instead of a
dnl	program.  Calls AC_SUBST for "variable".
dnl
dnl ######################################################################

AC_DEFUN([TX_PATH_FILE],[

  AC_MSG_CHECKING([for $2])

  dnl Check for a path argument.  If none is given, set to current
  dnl directory, otherwise expand the path argument.
  if test -z "$4"; then
    tx_search_path="."
  else
    tx_search_path=`echo "$4" | tr ':' ' '`
  fi

  tx_return=""
  for tx_path in $tx_search_path; do
    ## if test -z `echo "$tx_path" | grep "/$" -`; then
    tmppath=`echo "$tx_path" | grep "/$"`
    if test -z "$tmppath"; then
      tx_path_temp="$tx_path"/
    else
      tx_path_temp=$tx_path
    fi
    if test -a "$tx_path_temp"$2; then
      tx_return="$tx_path_temp"$2
      break
    fi
  done

  if test -z "$tx_return"; then
    $1="$3"
    AC_MSG_RESULT([no])
  else
    $1="$tx_return"
    AC_MSG_RESULT([$tx_return])
  fi

  AC_SUBST([$1])

])

dnl ######################################################################
dnl
dnl Function:	TX_PATH_FILES( variable , files-to-check-for ,
dnl	[value-if-not-found] , [path] )
dnl
dnl Purpose:	Like AC_PATH_PROGS but searches for files instead of
dnl	programs.  Calls AC_SUBST for "variable".
dnl
dnl ######################################################################

AC_DEFUN([TX_PATH_FILES],[

  for tx_files in $2; do
    TX_PATH_FILE([$1],[$tx_files],[],[$4])
    if test -n "$$1"; then break; fi
  done

  if test -z "$$1"; then
    $1="$3"
    AC_SUBST([$1])
  fi

])

dnl ######################################################################
dnl
dnl Function:	TX_LOCATE_PKG( pkgname , default-path , [inc-files] ,
dnl	[lib-files] , [default-inc-path] , [default-lib-path] )
dnl
dnl Purpose:	All inclusive search for package files in the case of a
dnl	simple package.
dnl
dnl Inputs:
dnl	pkgname:
dnl		Name of the package to search for.
dnl	default-path:
dnl		Default path in which to search for the package,
dnl		colon delimited for multiple possiblities.
dnl	inc-files:
dnl		Comma separated list of critical includes required for
dnl		the package to function.  Explicitly passing [-] to
dnl             this argument will disable include file search
dnl		defaults to pkgname.h
dnl	lib-files:
dnl		Comma separated list of critical libs required for the
dnl		package to function.  Explicitly passing [-] to
dnl             this argument will disable library file search
dnl		defaults to libpkgname$SO(a,dylib)
dnl	default-inc-path:
dnl		Location of the includes for this package,
dnl		colon delimited for multiple possibilities.
dnl		defaults to default-path/include/
dnl	default-lib-path:
dnl		location of the libraries for this package,
dnl		colon delimited for multiple possibilities.
dnl		defaults to default-path/lib
dnl
dnl Outputs (via AC_SUBST):
dnl	pkgname_INCDIR:			directory of the last include
dnl	pkgname_INC:			complete, flagged, includes variable
dnl     pkgname_INC_file:		absolute path to a specific header (one for each file)
dnl	pkgname_LIBDIR:			directory of the first library
dnl	pkgname_LIBS:			complete, flagged, libraries variable
dnl	pkgname_LIB_lib:		absolute path to a specific library (one for each library)
dnl
dnl Outputs (via AC_DEFINE):
dnl	HAVE_pkgname:			defined if includes and libs are found
dnl
dnl Outputs (via AM_CONDITIONAL):
dnl     HAVE_pkgname:                   defined if includes and libs are found
dnl
dnl Variables set for later use:
dnl     FOUND_pkgname                   set to "yes" if package was found, or "no" otherwise
dnl
dnl Also defines these user override flags
dnl	--with-pkgname-dir:		Prepends default
dnl	--with-pkgname-incdir:		Prepends default
dnl	--with-pkgname-inc-file:	Overrides default (one for each file)
dnl	--with-pkgname-libdir:		Prepends default
dnl	--with-pkgname-lib-lib:		Overrides default (one for each file)
dnl
dnl ######################################################################

dnl ----------------------------------------------------------------------
dnl Definition of TX_LOCATE_PKG
dnl ----------------------------------------------------------------------

AC_DEFUN([TX_LOCATE_PKG],[

  tx_failure="no"

  m4_ifval([$1],[],[AC_DIAGNOSE([syntax],[No package name specified])])

  dnl Create the package's unique variable prepender
  PKGNM=`echo $1 | tr '[a-z./-]' '[A-Z___]'`

dnl --------------------------------------------------------------------
dnl Set the default package installation directory
dnl --------------------------------------------------------------------

  m4_ifval([$2],[],[AC_DIAGNOSE([syntax],[No default directory specified for $1 package])])

  dnl Additional user input
  AC_ARG_WITH(
    translit($1,'A-Z./','a-z__')[-dir],
    [AC_HELP_STRING([--with-]translit($1,'A-Z./','a-z__')[-dir=<dir>],[Installation directory of $1 package])],
    [eval "$PKGNM"_DIR="$withval:$2"],
    [eval "$PKGNM"_DIR="$2"])

  eval pkgdir=$`echo "$PKGNM"_DIR`
  eval "$PKGNM"_DIR=\"\"
  for pdirs in `echo $pkgdir | tr ':' ' '`; do
    pdirsubstr=`echo "$pdirs" | grep "^/"`
    if test -z "$pdirsubstr"; then                          dnl Relative path specified
      AC_MSG_WARN([$pdirs is not an absolute path, skipping])
      continue
    fi
    dnl Trailing / missing
    pdirsubstr=`echo "$pdirs" | grep "/$"`
    if test -z "$pdirsubstr"; then                          dnl Relative path specified
      eval "$PKGNM"_DIR=$pdirs/:$`echo "$PKGNM"_DIR`
    else
      eval "$PKGNM"_DIR=$pdirs:$`echo "$PKGNM"_DIR`
    fi
  done
  eval pkgdir=$`echo "$PKGNM"_DIR`
  if test -z "$pkgdir"; then
    AC_MSG_ERROR([Package $1 has no default directories specified, use --with-]translit($1,'A-Z./','a-z__')[-dir to specify one])
  fi

  dnl Write to summary file if defined
  if test -n "$config_summary_file"; then
    echo >>$config_summary_file
    eval printf \"Seeking %s package with\\n\" \"$1\"								>> $config_summary_file
    eval printf \""  "Directories searched:\\n\"								>> $config_summary_file
    for pdirs in `echo $pkgdir | tr ':' ' '`; do
      eval printf \""    "%s\\n\" \"$pdirs\"									>> $config_summary_file
    done
  fi

dnl --------------------------------------------------------------------
dnl Set the default include path search directories
dnl --------------------------------------------------------------------

  eval "$PKGNM"_INCDIR_PATH=\"\"
  dnl Default search location
  for pdirs in `echo $pkgdir | tr ':' ' '`; do
    eval "$PKGNM"_INCDIR_PATH=$pdirs\\include/:$`echo "$PKGNM"_INCDIR_PATH`
  done

  dnl Additional specified locations
  if test -n "$5"; then
    for incdir in "translit($5,':',' ')"; do
      if test -z `echo "$incdir" | grep "^/" -`; then	        dnl Relative path specified
        for pdirs in `echo $pkgdir | tr ':' ' '`; do
          eval tempvar=$pdirs\\$incdir
          if test -z `echo "$tempvar" | grep "/$" -`; then	dnl Trailing / missing
            eval tempvar="$tempvar"/
          fi
          dnl Write entry to _INCDIR_PATH
          eval "$PKGNM"_INCDIR_PATH=$tempvar:$`echo "$PKGNM"_INCDIR_PATH`
        done
      else							dnl Absolute path specified
        eval tempvar=$incdir
        if test -z `echo "$tempvar" | grep "/$" -`; then	dnl Trailing / missing
          eval tempvar="$tempvar"/
        fi
        dnl Write entry to _INCDIR_PATH
        eval "$PKGNM"_INCDIR_PATH=$tempvar:$`echo "$PKGNM"_INCDIR_PATH`
      fi
    done
  fi

dnl Additional user input
  AC_ARG_WITH(
    translit($1,'A-Z./','a-z__')[-incdir],
    [AC_HELP_STRING([--with-]translit($1,'A-Z./','a-z__')[-incdir=<dir>],[Includes directory of $1 package])],
    [tempvar=$withval],
    [tempvar=""])
  if test -n "$tempvar"; then
    if test -z `echo "$tempvar" | grep "^/" -`; then 	dnl Relative path specified
      for pdirs in `echo $pkgdir | tr ':' ' '`; do
        eval incdir=$pdirs\\$tempvar
        if test -z `echo "$incdir" | grep "/$" -`; then	dnl Trailing / missing
          eval incdir="$incdir"/
        fi
        dnl Write entry to _INCDIR_PATH
        eval "$PKGNM"_INCDIR_PATH=$incdir:$`echo "$PKGNM"_INCDIR_PATH`
      done
    else			                	dnl Absolute path specified
      eval incdir=$tempvar
      if test -z `echo "$incdir" | grep "/$" -`; then	dnl Trailing / missing
        eval incdir="$incdir"/
      fi
      dnl Write entry to _INCDIR_PATH
      eval "$PKGNM"_INCDIR_PATH=$incdir:$`echo "$PKGNM"_INCDIR_PATH`
    fi
  fi

dnl --------------------------------------------------------------------
dnl Locate and set the actual requested include files
dnl --------------------------------------------------------------------

dnl First off, check and see if we even need to look for headers
  tx_heads=""
  m4_ifval(
    [$3],
    [if test "$3" = "-"; then tx_heads="-- none --"; else tx_heads="$3"; fi],
    [tx_heads="translit($1,'A-Z./','a-z__').h"])

dnl Write to summary file if defined
  if test -n "$config_summary_file"; then
    eval printf \""  "Includes sought: %s\\n\" \"$tx_heads\"							>> $config_summary_file
  fi

dnl Skip the header check if we're not looking for any headers
  if test "$tx_heads" != "-- none --"; then

dnl Catch and save variables that are previously defined at the command prompt
    test_save=""; eval test_save=\"$`echo "$PKGNM"_INC`\"
    if test -n "$test_save"; then eval SAVE_"$PKGNM"_INC=\"$test_save\"; fi
    test_save=""; eval test_save=\"$`echo "$PKGNM"_INCDIR`\"
    if test -n "$test_save"; then eval SAVE_"$PKGNM"_INCDIR=\"$test_save\"; fi

dnl Begin the actual tests
    eval "$PKGNM"_INC=""

dnl Loop over all includes specified
    m4_ifval(
      [$3],
      [TX_LOCATE_PKG_HEADLOOP([$1],[$3])],
      [TX_LOCATE_PKG_HEADLOOP([$1],translit($1,'A-Z./','a-z__')[.h])])
    for itr in $tx_incs; do
      eval "$PKGNM"_INC=\"$`echo "$PKGNM"_INC` -I$itr\"
    done

dnl Replace variables with command prompt overrides if supplied.
dnl If this override is used, reset the failure flag
    test_save=""; eval test_save=\"$`echo SAVE_"$PKGNM"_INC`\"
    if test -n "$test_save"; then eval "$PKGNM"_INC=\"$test_save\"; tx_failure="no"; fi
    test_save=""; eval test_save=\"$`echo SAVE_"$PKGNM"_INCDIR`\"
    if test -n "$test_save"; then eval "$PKGNM"_INCDIR=\"$test_save\"; tx_failure="no"; fi

    dnl Export
    AC_SUBST(translit($1,'a-z./-','A-Z____')[_INC])
    AC_SUBST(translit($1,'a-z./-','A-Z____')[_INCDIR])

dnl Write to summary file if defined
    if test -n "$config_summary_file"; then
      if test "$tx_failure" = "no"; then
        eval printf \""    "%-30s: %s\\n\" \"$PKGNM\"\"_INCDIR\" \"$`echo "$PKGNM"_INCDIR`\"			>> $config_summary_file
        eval printf \""    "%-30s:%s\\n\" \"$PKGNM\"\"_INC\" \"$`echo "$PKGNM"_INC`\"				>> $config_summary_file
      else
        eval printf \""    "%-30s: %s\\n\" \"$PKGNM\"\"_INCDIR\" \"-- failed --\"       			>> $config_summary_file
        eval printf \""    "%-30s: %s\\n\" \"$PKGNM\"\"_INC\" \"-- failed --\"		        		>> $config_summary_file
      fi
    fi

  fi

dnl --------------------------------------------------------------------
dnl Set the default library path search directories
dnl --------------------------------------------------------------------

dnl Default search location
  eval "$PKGNM"_LIBDIR_PATH=\"\"
  for pdirs in `echo "$pkgdir" | tr ':' ' '`; do
    eval "$PKGNM"_LIBDIR_PATH=$pdirs\\lib/:$`echo "$PKGNM"_LIBDIR_PATH`
  done

dnl Additional specified locations
  if test -n "$6"; then
    for srch_libdir in " " translit($6,':',' '); do
      if test -z `echo "$srch_libdir" | grep "^/" -`; then         	dnl Relative path specified
        for pdirs in `echo $pkgdir | tr ':' ' '`; do
          eval tempvar=$pdirs\\$srch_libdir
          if test -z `echo "$tempvar" | grep "/$" -`; then	dnl Trailing / missing
            eval tempvar="$tempvar"/
          fi
          dnl Write entry to _INCDIR_PATH
          eval "$PKGNM"_LIBDIR_PATH=$tempvar:$`echo "$PKGNM"_LIBDIR_PATH`
        done
      else							dnl Absolute path specified
        eval tempvar=$srch_libdir
        if test -z `echo "$tempvar" | grep "/$" -`; then	dnl Trailing / missing
          eval tempvar="$tempvar"/
        fi
        dnl Write entry to _LIBDIR_PATH
        eval "$PKGNM"_LIBDIR_PATH=$tempvar:$`echo "$PKGNM"_LIBDIR_PATH`
      fi
    done
  fi
# JRC: how do I do this?
  # echo searching for libraries in ${"$PKGNM"_LIBDIR_PATH}

dnl Additional user input
  AC_ARG_WITH(
    translit($1,'A-Z./','a-z__')[-libdir],
    [AC_HELP_STRING([--with-]translit($1,'A-Z./','a-z__')[-libdir=<dir>],[Libraries directory of $1 package])],
    [tempvar=$withval],
    [tempvar=""])
  if test -n "$tempvar"; then
    if test -z `echo "$tempvar" | grep "^/" -`; then 	dnl Relative path specified
      for pdirs in `echo $pkgdir | tr ':' ' '`; do
        eval srch_libdir=$pdirs\\$tempvar
        if test -z `echo "$srch_libdir" | grep "/$" -`; then	dnl Trailing / missing
          eval srch_libdir="$srch_libdir"/
        fi
        dnl Write entry to _LIBDIR_PATH
        eval "$PKGNM"_LIBDIR_PATH=$srch_libdir:$`echo "$PKGNM"_LIBDIR_PATH`
      done
    else							dnl Absolute path specified
      eval srch_libdir=$tempvar
      if test -z `echo "$srch_libdir" | grep "/$" -`; then	dnl Trailing / missing
        eval srch_libdir="$srch_libdir"/
      fi
      dnl Write entry to _LIBDIR_PATH
      eval "$PKGNM"_LIBDIR_PATH=$srch_libdir:$`echo "$PKGNM"_LIBDIR_PATH`
    fi
  fi

dnl --------------------------------------------------------------------
dnl Locate and set the actual required library files
dnl --------------------------------------------------------------------

dnl First off, check and see if we even need to look for libraries
  tx_libs=""
  m4_ifval(
    [$4],
    [if test "$4" = "-"; then tx_libs="-- none --"; else tx_libs="$4"; fi],
    [tx_libs="translit($1,'A-Z./','a-z__')"])

dnl Write to summary file if defined
  if test -n "$config_summary_file"; then
    dnl if test "$tx_failure" = "no"; then
      eval printf \""  "Libraries sought: %s\\n\" \"$tx_libs\"		        				>> $config_summary_file
    dnl else
      dnl eval printf \""  "Libraries sought: %s\\n\" \"-- skipped --\"	        				>> $config_summary_file
    dnl fi
  fi

dnl Dont bother if already failed, skip the library check if we're not
dnl looking for any libraries
  dnl if test "$tx_failure" = "no" -a "$tx_libs" != "-- none --"; then
dnl JRC: continue looking for libs even if continue failed
  if test "$tx_libs" != "-- none --"; then

dnl Catch and save variables that are previously defined at the command prompt
    test_save=""; eval test_save=\"$`echo "$PKGNM"_LIBS`\"
    if test -n "$test_save"; then eval SAVE_"$PKGNM"_LIBS=\"$test_save\"; fi
    test_save=""; eval test_save=\"$`echo "$PKGNM"_LIBDIR`\"
    if test -n "$test_save"; then eval SAVE_"$PKGNM"_LIBDIR=\"$test_save\"; fi

    dnl Do the actual search
    eval "$PKGNM"_LIBS=""

    m4_ifval(
      [$4],
      [TX_LOCATE_PKG_LIBLOOP([$1],[$4])],
      [TX_LOCATE_PKG_LIBLOOP([$1],translit($1,'A-Z./','a-z__'))])
    for itr in $tx_lib_paths; do
      eval "$PKGNM"_LIBS=\"$`echo "$PKGNM"_LIBS` \\$\(RPATH_FLAG\)$itr\"
    done
    for itr in $tx_lib_paths; do
      eval "$PKGNM"_LIBS=\"$`echo "$PKGNM"_LIBS` -L$itr\"
    done
    for itr in $tx_libs; do
      eval "$PKGNM"_LIBS=\"$`echo "$PKGNM"_LIBS` -l$itr\"
    done

dnl Replace variables with command prompt overrides if supplied
dnl If this override is used, reset the failure flag
    test_save=""
    eval test_save=\"$`echo SAVE_"$PKGNM"_LIBS`\"
    if test -n "$test_save"; then
      eval "$PKGNM"_LIBS=\"$test_save\"
      tx_failure="no"
    fi
    test_save=""
    eval test_save=\"$`echo SAVE_"$PKGNM"_LIBDIR`\"
    if test -n "$test_save"; then
      eval "$PKGNM"_LIBDIR=\"$test_save\"
      tx_failure="no"
    fi

dnl Export
    AC_SUBST(translit($1,'a-z./-','A-Z____')[_LIBS])
    AC_SUBST(translit($1,'a-z./-','A-Z____')[_LIBDIR])

dnl Write to summary file if defined
    if test -n "$config_summary_file"; then
      dnl if test "$tx_failure" = "no"; then
        eval printf \""    "%-30s: %s\\n\" \"$PKGNM\"\"_LIBDIR\" \"$`echo "$PKGNM"_LIBDIR`\"			>> $config_summary_file
        eval printf \""    "%-30s:%s\\n\" \"$PKGNM\"\"_LIBS\" \"$`echo "$PKGNM"_LIBS`\"				>> $config_summary_file
      dnl else
        dnl eval printf \""    "%-30s: %s\\n\" \"$PKGNM\"\"_LIBDIR\" \"-- failed --\"	        		>> $config_summary_file
        dnl eval printf \""    "%-30s: %s\\n\" \"$PKGNM\"\"_LIBS\" \"-- failed --\"	        			>> $config_summary_file
      dnl fi
    fi

  fi

dnl --------------------------------------------------------------------
dnl Confirm that the package was found
dnl --------------------------------------------------------------------
  if test "$tx_failure" = "no"; then
    AC_DEFINE([HAVE_]translit($1,'a-z./-','A-Z____'),[],[Defined if $1 found])
    eval FOUND_"$PKGNM"="yes"
  else
    eval FOUND_"$PKGNM"="no"
  fi
  AM_CONDITIONAL([HAVE_]translit($1,'a-z./-','A-Z____'),[test "$tx_failure" = "no"])

])

dnl ----------------------------------------------------------------------
dnl Helper functions TX_LOCATE_PKG_FILELOOP used by TX_LOCATE_PKG
dnl ----------------------------------------------------------------------

dnl This is a generic function used by TX_LOCATE_PKG that on the m4 level
dnl loops through the different possible header files, adds and sets
dnl options for each file, and returns outputs accordingly
AC_DEFUN([TX_LOCATE_PKG_HEADLOOP],[

  dnl Initialize local container variables
  tx_incs=""

  m4_foreach([incfile],m4_dquote($2),[
    dnl Create this file's unique variable postpender
    FILENM=`echo "incfile" | tr '[a-z./-]' '[A-Z___]'`

    dnl Additional user input
    AC_ARG_WITH(
      translit($1,'A-Z./','a-z__')[-inc-]translit(incfile,'A-Z./','a-z__'),
      [  --with-]translit($1,'A-Z./','a-z__')[-inc-]translit(incfile,'A-Z./','a-z__')[=<file>
                          Full path location of the ]incfile[ header file],
      [eval searchdir=`dirname $withval`],
      [eval searchdir=$`echo "$PKGNM"_INCDIR_PATH`])

    dnl Find the file
    TX_PATH_FILE(translit($1,'a-z./-','A-Z____')[_INC_]translit(incfile,'a-z./-','A-Z____'),incfile,[],[$searchdir])

    dnl If the file is found, append the flag to _INC and write the directory to _INCDIR, otherwise warn
    eval tempvar="$`echo "$PKGNM"_INC_"$FILENM"`"
    if test -n "$tempvar"; then
      TX_LOCATE_PKG_UNIQUE_CAT([`dirname $tempvar`],[tx_incs])
      eval "$PKGNM"_INCDIR=`dirname $tempvar`
    else
      AC_MSG_WARN([Could not find ]incfile[ header file in $searchdir])
      tx_failure="yes"
    fi

    dnl Write to summary file if defined
    if test -n "$config_summary_file"; then
      if test -n "$tempvar"; then
        eval printf \""    "%-30s: %s\\n\" \"$PKGNM\"\"_INC_\"\"$FILENM\" \"$`echo "$PKGNM"_INC_"$FILENM"`\"	>> $config_summary_file
      else
        eval printf \""    "%-30s: %s\\n\" \"$PKGNM\"\"_INC_\"\"$FILENM\" \"-- not found --\"	                >> $config_summary_file
      fi
    fi
  ])

])

dnl This is a generic function used by TX_LOCATE_PKG that on the m4 level
dnl loops through the different possible library files, adds and sets
dnl options for each file, and returns outputs accordingly
AC_DEFUN([TX_LOCATE_PKG_LIBLOOP],[

  dnl Initialize local container variables
  tx_libs=""
  tx_lib_paths=""

  m4_foreach([libfile],m4_dquote($2),[
    dnl Create this file's unique variable postpender
    FILENM=`echo "libfile" | tr '[a-z./-]' '[A-Z___]'`

    searchfile=""
    dnl Additional user input
    AC_ARG_WITH(
      translit($1,'A-Z./','a-z__')[-lib-]translit(libfile,'A-Z./','a-z__'),
      [  --with-]translit($1,'A-Z./','a-z__')[-lib-]translit(libfile,'A-Z./','a-z__')[=<file>
                          Full path location of the lib]libfile[$SO(a) library file],
      [eval searchdir=`dirname $withval`; eval searchfile=`basename $withval`],
      [eval searchdir=$`echo "$PKGNM"_LIBDIR_PATH`])

dnl Find the library file (searches dynamic, then static)
    if test -z "$searchfile"; then
      TX_PATH_FILE(translit($1,'a-z./-','A-Z____')[_LIB_]translit(libfile,'a-z./-','A-Z____'),[lib]libfile[$SO],[],[$searchdir])
      eval tempval="$`echo "$PKGNM"_LIB_"$FILENM"`"
      if test -z "$tempval"; then
        TX_PATH_FILE(translit($1,'a-z./-','A-Z____')[_LIB_]translit(libfile,'a-z./-','A-Z____'),[lib]libfile[.dylib],[],[$searchdir])
        eval tempval="$`echo "$PKGNM"_LIB_"$FILENM"`"
      fi
      if test -z "$tempval"; then
        TX_PATH_FILE(translit($1,'a-z./-','A-Z____')[_LIB_]translit(libfile,'a-z./-','A-Z____'),[lib]libfile[.a],[],[$searchdir])
      fi
    else
      TX_PATH_FILE(translit($1,'a-z./-','A-Z____')[_LIB_]translit(libfile,'a-z./-','A-Z____'),[$searchfile],[],[$searchdir])
    fi

dnl If the file is found, append the flag to _LIB and write the directory to _LIBDIR, otherwise warn
    eval tempvar="$`echo "$PKGNM"_LIB_"$FILENM"`"
    if test -n "$tempvar"; then
      TX_LOCATE_PKG_UNIQUE_CAT([`dirname $tempvar`],[tx_lib_paths])
      TX_LOCATE_PKG_UNIQUE_CAT([libfile],[tx_libs])
      eval "$PKGNM"_LIBDIR=`dirname $tempvar`
    else
      if test -n "$searchfile"; then
        AC_MSG_WARN([Could not find $searchfile library file])
      else
        AC_MSG_WARN([Could not find lib]libfile[$SO(a) library file])
      fi
      tx_failure="yes"
    fi

    dnl Write to summary file if defined
    if test -n "$config_summary_file"; then
      if test -n "$tempvar"; then
        eval printf \""    "%-30s: %s\\n\" \"$PKGNM\"\"_LIB_\"\"$FILENM\" \"$`echo "$PKGNM"_LIB_"$FILENM"`\"	>> $config_summary_file
      else
        eval printf \""    "%-30s: %s\\n\" \"$PKGNM\"\"_LIB_\"\"$FILENM\" \"-- not found --\"           	>> $config_summary_file
      fi
    fi
  ])

])

dnl This is a generic function used by TX_LOCATE_PKG that keeps tracks of pseudo-lists
dnl and only adds to a list an item if it's a unique entity.  Effectively it is the python
dnl 	if not item in list: list.append(item)
dnl Argument 1 is "item"; argument 2 is a container containing "list".  List should be space separated.
AC_DEFUN([TX_LOCATE_PKG_UNIQUE_CAT],[

  testbool="1"
  for itr in $$2; do
    if test "$1" = "$itr"; then
      testbool="0"
      break
    fi
  done
  if test "$testbool" = "1"; then
    $2="$$2 $1"
  fi

])
