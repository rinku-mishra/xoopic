dnl ######################################################################
dnl
dnl File:	java.m4
dnl
dnl Purpose:	Determine where the java 2 compiler, tools, and
dnl             class archives are.
dnl
dnl Version:	$Id: java2.m4 733 2002-03-26 01:18:08Z alexanda $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Determine the location of java development kit
dnl
dnl ######################################################################

JDK_DIR=""
AC_ARG_WITH(jdk-dir, [ --with-jdk-dir=<location of top java JDK directory> ], JDK_DIR="$withval")
if test -n "$JDK_DIR"; then
  JAVA_PATH_TRY=$JDK_DIR/bin
else
  JAVA_PATH_TRY=/mfe/local/java2/bin:/usr/local/java2/bin:/loc/java2/bin:/usr/local/j2sdk1.4.0:/usr/java/j2sdk1.4.0/bin:/usr/local/j2sdk1.3:/usr/local/j2sdk1.3.1_02:/usr/java/j2sdk1.3:/usr/java/jdk1.3.1_02/bin:/usr/local/j2sdk1.2:/usr/java/j2sdk1.2:/mfe/local/java/bin:/usr/local/java/bin:/loc/java/bin:/usr/java/bin:/usr/bin
fi
AC_PATH_PROGS(JAVA, java, , $JAVA_PATH_TRY)
if test -z "$JAVA"; then
    AC_MSG_ERROR(Unable to find java in $JAVA_PATH_TRY.  Use --with-jdk-dir to set correct base directory of java.)
else
  JAVA_HOME_BIN=`dirname $JAVA`
#  JAVA_HOME=`dirname $JAVA_HOME_BIN`
  JDK_JAVAC=$JAVA_HOME_BIN/javac
fi
#JAVAEXEC=$JAVA
#AC_SUBST(JAVA_HOME)
#AC_SUBST(JAVA)
#AC_SUBST(JAVAEXEC)

dnl ######################################################################
dnl
dnl Determine the Java version
dnl
dnl ######################################################################

javaver=`$JAVA -version 2>&1 | sed 's/java version \"//' | sed 's/\".*//'`
javaver=`echo $javaver | sed 's/ .*$//'`
echo "... Java version is '$javaver'. "
minorversion=0
isdone=""
while test $minorversion -lt 5 -a ! "$isdone"; do
  minorversion=`expr $minorversion + 1`
  isver=`echo $javaver | grep 1\.$minorversion\.`
  if test -n "$isver"; then
    isdone=true
  fi
done
if test $minorversion -lt 2; then
  AC_MSG_ERROR("Java version must be greater than 1.2.  Use --with-jdk-dir to set correct base directory of java2.")
fi

dnl ######################################################################
dnl
dnl Determine the Java compiler
dnl
dnl ######################################################################

JAVAC=""
AC_ARG_WITH(javac,[  --with-javac=<to use an alternative java compiler (such as jikes) instead of javac>],JAVAC="$withval",JAVAC=$JDK_JAVAC)
if test "$JAVAC" != "$JDK_JAVAC"; then
  ALT_JAVAC=""
  AC_CHECK_PROG(ALT_JAVAC, $JAVAC, $JAVAC)
  if test -z "$ALT_JAVAC"; then
    AC_MSG_WARN(Unable to find $ALT_JAVAC.  Reverting to JDK compiler.)
    JAVAC=$JDK_JAVAC
  else
    JAVAC=$ALT_JAVAC
  fi
fi
echo ... Using java compiler $JAVAC
AC_SUBST(JAVAC)

dnl ######################################################################
dnl
dnl Set the location of the jar utility
dnl
dnl ######################################################################

JAR=$JAVA_HOME_BIN/jar
AC_SUBST(JAR)

dnl ######################################################################
dnl
dnl Set the location of the javadocs documentation program
dnl and setup docs directory.
dnl
dnl ######################################################################

JAVADOC=$JAVA_HOME_BIN/javadoc
AC_SUBST(JAVADOC)
JAVADOC_FLAGS="-version -author"
AC_SUBST(JAVADOC_FLAGS)
if test ! -d docs; then mkdir docs; fi
if test ! -d docs/javaapi; then mkdir docs/javaapi; fi

