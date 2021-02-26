dnl ######################################################################
dnl
dnl File:	idldepend.m4
dnl
dnl Purpose:	Find the idldepend jar file
dnl
dnl Version:	$Id: idldepend.m4 1402 2004-12-05 01:10:10Z cary $
dnl
dnl ######################################################################

AC_ARG_WITH(idldepend-jarfile, [  --with-idldepend-jarfile=<idldepend-jar>         to set location of idldepend jar file], IDLDEPEND_JAR="$withval")

if test -z "$IDLDEPEND_JAR"; then
  IDLDEPEND_JAR_PATH=/usr/local/ant/lib:/cygdrive/c/idldepend/dist/lib
  AC_PATH_PROGS(IDLDEPEND_JAR, idldepend.jar ant-jdepend.jar, "", $IDLDEPEND_JAR_PATH)
fi

if test -n "$IDLDEPEND_JAR"; then
  iscygwin=`echo $IDLDEPEND_JAR | grep cygdrive`
  if test -n "$iscygwin"; then
    tempidldependjar=`cygpath -m $IDLDEPEND_JAR`
    IDLDEPEND_JAR=$tempidldependjar
  fi
  IDLDEPEND_JARDIR=`dirname $IDLDEPEND_JAR`
  AC_SUBST(IDLDEPEND_JARDIR)
  echo setting location for idldepend... $IDLDEPEND_JARDIR
else
  AC_MSG_WARN(The idldepend jar file was not found.  Use --with-idldepend-jarfile to set.)
fi
