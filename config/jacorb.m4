dnl ######################################################################
dnl
dnl File:	jacorb.m4
dnl
dnl Purpose:	To determine location of jacorb.m4
dnl
dnl Version:	$Id: jacorb.m4 1402 2004-12-05 01:10:10Z cary $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

AC_ARG_WITH(JACORB-jarfile, [  --with-JACORB-jarfile=<jacorb-jar>         to set location of JACORB jar file],
JACORB_JAR="$withval")
if test -z "$JACORB_JAR"; then
  JACORB_JAR_PATH=$HOME/$UNIXFLAVOR/jacorb/lib:/usr/local/farsight:/usr/local/jacorb/lib:/cygdrive/c/jacorb/lib:/usr/local/codevel
  AC_PATH_PROGS(JACORB_JAR, jacorb.jar, "", $JACORB_JAR_PATH)
fi
# echo JACORB_JAR = $JACORB_JAR
if test -n "$JACORB_JAR"; then
  iscygwin=`echo $JACORB_JAR | grep cygdrive`
  if test -n "$iscygwin"; then
     tempjacorbjar=`cygpath -m $JACORB_JAR`
     JACORB_JAR=$tempjacorbjar
     echo JACORB_JAR=$JACORB_JAR
  fi

  JACORB_JARDIR=`dirname $JACORB_JAR`
  AC_SUBST(JACORB_JARDIR)
  # echo JACORB_JARDIR = $JACORB_JARDIR
  JACORB_BASE=`dirname $JACORB_JARDIR`
  AC_SUBST(JACORB_BASE)
else
  AC_MSG_WARN(The JACORB jar file was not found.  Use --with-JACORB-jarfile to set.)
fi
