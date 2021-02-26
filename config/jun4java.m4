dnl ######################################################################
dnl
dnl Check for Jun4Java
dnl
dnl ######################################################################

AC_ARG_WITH(jun4java-jarfile, [  --with-jun4java-jarfile=<jun4java-jar>         to set location of Jun4Java jar file],
JUN4JAVA_JAR="$withval")

if test -z "$JUN4JAVA_JAR"; then
  JUN4JAVA_JAR_PATH=/usr/local/codevel/jun4java:/cygdrive/c/jun4java/lib
  AC_PATH_PROGS(JUN4JAVA_JAR, jun.jar, "", $JUN4JAVA_JAR_PATH)
fi

if test -n "$JUN4JAVA_JAR"; then
  AC_PATH_PROGS(STPL_JAR, StPL.jar, "", `dirname $JUN4JAVA_JAR`)
  if test -z "$STPL_JAR"; then
    AC_MSG_WARN(The StPL jar file (used by jun4java) was not found in $JUN4JAVA_JARDIR)
  fi

  iscygwin=`echo $JUN4JAVA_JAR | grep cygdrive`
  if test -n "$iscygwin"; then
    tempjun4javajar=`cygpath -m $JUN4JAVA_JAR`
    JUN4JAVA_JAR=$tempjun4javajar
  fi

  JUN4JAVA_JARDIR=`dirname $JUN4JAVA_JAR`
  AC_SUBST(JUN4JAVA_JARDIR)
  echo setting location for jun4java... $JUN4JAVA_JARDIR
else
  AC_MSG_WARN(The Jun4Java jar file was not found.  Use --with-Jun4Java-jarfile to set.)
fi
