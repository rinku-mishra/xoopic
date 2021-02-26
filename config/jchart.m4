dnl ######################################################################
dnl
dnl File:	jchart.m4
dnl
dnl Purpose:	Determine where the JCCHART package jar file is.
dnl             Must be included after java.m4.
dnl
dnl Version:	$Id: jchart.m4 673 2001-12-29 19:13:16Z cary $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

AC_ARG_WITH(jcchart-jarfile,
[  --with-jcchart-jarfile=<jcchart-jar>     to set location of JCChart jarfile],
JCCHART_JAR="$withval")
dnl Look in the usual path
if test -z "$JCCHART_JAR"; then
  AC_PATH_PROGS(JCCHART_JAR, jcchart362J.jar, "", $JAR_PATH)
fi
if test -z "$JCCHART_JAR"; then
  AC_PATH_PROGS(JCCHART_JAR, jcchart361-classes.zip, "", $JAR_PATH)
fi
if test -z "$JCCHART_JAR"; then
  AC_PATH_PROGS(JCCHART_JAR, jcchart360-classes.zip, "", $JAR_PATH)
fi
if test -z "$JCCHART_JAR"; then
  AC_MSG_WARN(The jcchart jar file was not found.  Some java dirs may not compile.  Use --with-jcchart-jarfile to set.)
fi
AC_SUBST(JCCHART_JAR)

if test -n "$JCCHART_JAR"; then
  fname=`basename $JCCHART_JAR`
  if test -n "$fname"; then
    rm -f classes/$fname
    ln -s $JCCHART_JAR classes/$fname
  fi
fi

dnl ######################################################################
dnl
dnl Link the jclass dirs in the java area into classes
dnl
dnl ######################################################################

echo $ac_n linking jclass into classes... $ac_c
jlibdir=`dirname $JCCHART_JAR`
for i in jclass; do
  if test -d $jlibdir/$i; then
    rm -f classes/$i
    ln -s $jlibdir/$i classes
  else
    echo "$jlibdir/$i not found. "
    echo "....Unpack jcchart362J.jar in $jlibdir if you want complete applets!"
  fi
done
echo " done."

