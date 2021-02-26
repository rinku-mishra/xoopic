dnl #####################################################################
dnl
dnl Find xerces
dnl
dnl #####################################################################

builtin(include, config/txsearch.m4)
TX_LOCATE_PKG(
    [XERCES],
    [/contrib/xerces:/usr/local/xerces],
    [XercesDOMParser.hpp],
    [xerces-c],
    [include/xercesc/parsers],
)
xdir=`dirname $XERCES_INCDIR`
XERCES_TOPINCDIR=`dirname $xdir`
AC_SUBST(XERCES_TOPINCDIR)
XERCES_INCS="$XERCES_INC -I$XERCES_TOPINCDIR"
AC_SUBST(XERCES_INCS)

# Append needs for OS X

XERCES_LIBS="-L$XERCES_LIBDIR "'$(RPATH_FLAG)'"$XERCES_LIBDIR -lxerces-c"
# echo host = $host
case $host in
  *darwin*)
    XERCES_XLIBS="-framework CoreServices"
    XERCES_LIBS="$XERCES_LIBS $XERCES_XLIBS"
    ;;
esac
AC_SUBST(XERCES_XLIBS)
AC_SUBST(XERCES_LIBS)
if test -z "$XERCES_LIBDIR"; then
  AC_ERROR("Xerces not found!")
fi

# Print out results
if test -n "$config_summary_file"; then
  echo "  XERCES modifications"                >> $config_summary_file
  echo "    XERCES_LIBS:   $XERCES_LIBS"       >> $config_summary_file
  echo "    XERCES_XLIBS:  $XERCES_XLIBS"      >> $config_summary_file
fi

