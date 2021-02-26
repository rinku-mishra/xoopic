dnl ######################################################################
dnl
dnl File:	sshtools.m4
dnl
dnl Purpose:	Find the sshtools jar file
dnl
dnl Version:	$Id: sshtools.m4 1455 2005-02-18 00:06:20Z roopa $
dnl
dnl ######################################################################

AC_ARG_WITH(sshtools-jarfile, [  --with-sshtools-jarfile=<sshtools-jar>         to set location of SSHTools jar file], SSHTOOLS_JAR="$withval")
if test -z "$SSHTOOLS_JAR"; then
  SSHTOOLS_JAR_PATH=/usr/local/codevel:/cygdrive/c/SSHTools-j2ssh/dist/lib
  AC_PATH_PROGS(SSHTOOLS_JAR, j2ssh-core.jar, "", $SSHTOOLS_JAR_PATH)
fi

if test -n "$SSHTOOLS_JAR"; then
  AC_PATH_PROGS(J2SSHCOMMON_JAR, j2ssh-common.jar, "", `dirname $SSHTOOLS_JAR`)
  if test -z "$J2SSHCOMMON_JAR"; then
    AC_MSG_WARN(The j2ssh-common jar file (used by sshtools) was not found in $SSHTOOLS_JARDIR)
  fi

  COMMONSLOGGING_JARPATH=`dirname $SSHTOOLS_JAR`:/usr/local/ant/lib
  echo Looking in $COMMONSLOGGING_JARPATH
  AC_PATH_PROGS(COMMONSLOGGING_JAR, commons-logging.jar ant-commons-logging.jar, "", $COMMONSLOGGING_JARPATH)
  if test -z "$COMMONSLOGGING_JAR"; then
    AC_MSG_WARN(The commons-logging jar file (used by sshtools) was not found in $SSHTOOLS_JARDIR)
  fi

  iscygwin=`echo $SSHTOOLS_JAR | grep cygdrive`
  if test -n "$iscygwin"; then
     tempsshtoolsjar=`cygpath -m $SSHTOOLS_JAR`
     SSHTOOLS_JAR=$tempsshtoolsjar
  fi

  SSHTOOLS_JARDIR=`dirname $SSHTOOLS_JAR`
  AC_SUBST(SSHTOOLS_JARDIR)
  echo setting location for sshtools... $SSHTOOLS_JARDIR
else
  AC_MSG_WARN(The j2ssh-core jar file was not found.  Use --with-SSHTools-jarfile to set.)
fi
