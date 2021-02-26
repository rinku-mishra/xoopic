dnl ######################################################################
dnl
dnl File:	netcdf.m4
dnl
dnl Purpose:	Determine where the ucar netcdf package jar file is.
dnl             Must be included after java.m4.
dnl
dnl Version:	$Id: netcdf.m4 664 2001-12-26 18:38:06Z cary $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

AC_ARG_WITH(ucar-jarfile,
[  --with-ucar-jarfile=<ucar-jar>     to set location of ucar (netcdf) jarfile],
UCAR_JAR="$withval")
dnl Look in the usual path
if test -z "$UCAR_JAR"; then
  AC_PATH_PROGS(UCAR_JAR, ucar.jar, "", $JAR_PATH)
fi
if test -n "$UCAR_JAR"; then
  AC_SUBST(UCAR_JAR)
  NETCDF_DIRS='$(NETCDF_DIRS)'
  AC_SUBST(NETCDF_DIRS)
else
  AC_MSG_WARN(The ucar jar file was not found.  Java dirs using netcdf will not compile. Use --with-ucar-jarfile to set.)
fi

if test -n "$UCAR_JAR"; then
  fname=`basename $UCAR_JAR`
  if test -n "$fname"; then
    rm -f classes/$fname
    ln -s $UCAR_JAR classes/$fname
  fi
fi

dnl ######################################################################
dnl
dnl Link the ucar dirs in the java area into classes
dnl
dnl ######################################################################

# jlibdir=`dirname $UCAR_JAR`
# for i in ucar; do
#   if test -d $jlibdir/$i; then
#     rm -f classes/$i
#     ln -s $jlibdir/$i classes
#   else
#     echo "$jlibdir/$i not found. "
#     echo "Unpack $i.jar in $jlibdir if you want signed applets!"
#   fi
# done
