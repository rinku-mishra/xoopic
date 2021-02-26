dnl ######################################################################
dnl
dnl File:	jmflibs.m4
dnl
dnl Purpose:	Find the sshtools jar file
dnl
dnl Version:	$Id: jmflibs.m4 1504 2005-04-20 22:14:04Z chuli $
dnl
dnl ######################################################################

AC_ARG_WITH(jmflibs-jarfile, [ --with-jmflibs-jarfile=<jmflibs-jar> to set location of JMF jar file], JMFLIBS_JAR="$withval")

if test -z "$JMFLIBS_JAR"; then
  JMFLIBS_JAR_PATH=/usr/local/codevel/jmf-linux:/cygdrive/c/JMF-2.1.1e/lib 
  AC_PATH_PROGS(JMFLIBS_JAR, jmf.jar, "", $JMFLIBS_JAR_PATH)
fi

if test -n "$JMFLIBS_JAR"; then
  AC_PATH_PROGS(JMFMEDIAPLAYER_JAR, mediaplayer.jar, "", `dirname $JMFLIBS_JAR`)
  if test -z "$JMFMEDIAPLAYER_JAR"; then
    AC_MSG_WARN(The JMF mediaplayer jar file (used by jmf) was not found in $JMFLIBS_JARDIR)
  fi

  AC_PATH_PROGS(JMFMULTIPLAYER_JAR, multiplayer.jar, "", `dirname $JMFLIBS_JAR`)
  if test -z "$JMFMULTIPLAYER_JAR"; then
    AC_MSG_WARN(The JMF multiplayer jar file (used by jmf) was not found in $JMFLIBS_JARDIR)
  fi

  iscygwin=`echo $JMFLIBS_JAR | grep cygdrive`
  if test -n "$iscygwin"; then
     tempjmfjar=`cygpath -m $JMFLIBS_JAR`
     JMFLIBS_JAR=$tempjmfjar
  fi

  JMFLIBS_JARDIR=`dirname $JMFLIBS_JAR`
  AC_SUBST(JMFLIBS_JARDIR)
  echo setting location for jmf libraries ... $JMFLIBS_JARDIR
else
  AC_MSG_WARN(The jmf jar file was not found.  Use --with-jmflibs-jarfile to set.)
fi
