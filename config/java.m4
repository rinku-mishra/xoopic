dnl ######################################################################
dnl
dnl File:	java.m4
dnl
dnl Purpose:	Determine where the java tools, base classes, and
dnl             swing extension classes are.
dnl
dnl Version:	$Id: java.m4 2124 2006-12-15 16:52:01Z karipid $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Determine the location of java
dnl
dnl ######################################################################

AC_ARG_WITH(java-home,
[  --with-java-home=<location of java>      to set base directory of java],
JAVA_HOME="$withval")
dnl Look in the usual path
if test -n "$JAVA_HOME"; then
  JAVA_PATH=$JAVA_HOME/bin
else
  JAVA_PATH=/usr/java/java/bin:/mfe/local/java/bin:/usr/local/java2/bin:/usr/local/java/bin:/loc/java/bin:/usr/java/bin:/usr/bin:/usr/lib/j2sdk1.3/bin:/usr/lpp/J1.1.6/bin:/usr/IBMVJava/ide/program/bin
fi
AC_PATH_PROGS(JAVA, java, , $JAVA_PATH)
if test -z "$JAVA"; then
    AC_MSG_ERROR(Unable to find java in $JAVA_PATH.  Use --with-java-home to set correct base directory of java.)
else
  JAVA_BINDIR=`dirname $JAVA`
  JAVA_BINDIR=`(cd $JAVA_BINDIR; pwd)`
  JAVA_HOME=`dirname $JAVA_BINDIR`
  JAVA_HOME=`(cd $JAVA_HOME; pwd)`
fi
JAVAEXEC=$JAVA
AC_SUBST(JAVA_BINDIR)
AC_SUBST(JAVA_HOME)
AC_SUBST(JAVA)
AC_SUBST(JAVAEXEC)

dnl ######################################################################
dnl
dnl Determine the Java version
dnl
dnl ######################################################################

javaver=`$JAVA_BINDIR/java -version 2>&1 | sed 's/java version \"//' | sed 's/\".*//'`
javaver=`echo $javaver | sed 's/ .*$//'`
# echo javaver  = \'$javaver\'
minorversion=0
isdone=""
while test $minorversion -lt 5 -a ! "$isdone"; do
  minorversion=`expr $minorversion + 1`
  # echo minorversion = $minorversion
  isver=`echo $javaver | grep 1\.$minorversion\.`
  if test -n "$isver"; then
    JAVA_MINOR_VERSION=$minorversion
    isdone=true
  fi

done
# echo JAVA_MINOR_VERSION is $JAVA_MINOR_VERSION
AC_SUBST(JAVA_MINOR_VERSION)

dnl ######################################################################
dnl
dnl Determine the Java tools
dnl
dnl ######################################################################

if test -z "$JAVAC"; then
  AC_ARG_WITH(javac,[  --with-javac=<java cmp> to use <java compiler> (jikes) instead of javac],JAVAC="$withval",JAVAC=javac)
fi
if test "$JAVAC" = javac; then
  AC_PATH_PROGS(JAVAC, javac, "", $JAVA_HOME/bin)
else
  AC_CHECK_PROG(JAVAC, $JAVAC, $JAVAC)
fi

if test -z "$JAVAC"; then
  AC_MSG_WARN(Unable to find $JAVAC in your path.  Java classes will not be created.)
  JAVA_MAKEFILES=""
else
  JAVA=""
fi

echo ... Using java compiler $JAVAC
export JAVAC
AC_SUBST(JAVAC)

dnl ######################################################################
dnl
dnl Determine the location of classes.zip
dnl
dnl ######################################################################

JAVA_LIBPATH=$JAVA_HOME/lib
if test $JAVA_MINOR_VERSION -lt 2; then
  AC_PATH_PROGS(CLASSES_ZIP, classes.zip, "", $JAVA_LIBPATH)
  if test -z "$CLASSES_ZIP"; then
    AC_MSG_WARN(Unable to find classes.zip in $JAVA_LIBPATH.  Use --with-java-home to set correct base directory of java.)
  fi
fi
AC_SUBST(CLASSES_ZIP)

dnl ######################################################################
dnl
dnl Determine location of the capsapi zip file
dnl
dnl ######################################################################

dnl JAR_PATH=$HOME/$UNIXFLAVOR/jlib:/usr/local/jlib:/local/jlib:/mfe/local/jlib:/loc/jlib:$JAVA_LIBPATH
dnl AC_ARG_WITH(capsapi-zipfile,
dnl [  --with-capsapi-zip=<capsapi-zip file>     to set location of Netscape's capsapi zip file],
dnl CAPSAPI_ZIP="$withval")
dnl dnl Look in the usual path
dnl if test -z "$CAPSAPI_ZIP"; then
dnl   AC_PATH_PROGS(CAPSAPI_ZIP, capsapi_classes.zip, "", $JAR_PATH)
dnl fi
dnl 
dnl if test -z "$CAPSAPI_ZIP"; then
dnl   AC_MSG_WARN(The capsapi zip file was not found.  It is needed for projects to support Netscape 4.7x.  Use --with-capsapi-zipfile to set.)
dnl fi
dnl AC_SUBST(CAPSAPI_ZIP)

dnl ######################################################################
dnl
dnl Determine the locations of the jar utility
dnl
dnl ######################################################################

dnl Look in the usual path
AC_PATH_PROGS(JAR, jar, "", $JAVA_HOME/bin:$PATH)
if test -z "$JAR"; then
  AC_MSG_ERROR(The jar utility was not found in the Java home or the path.  Use --with-java-home to set base directory of java.)
fi
AC_SUBST(JAR)

dnl ######################################################################
dnl
dnl Determine location of Swing
dnl
dnl ######################################################################

dnl JRC: No need for swing anymore.
dnl AC_ARG_WITH(swing-jarfile,
dnl [  --with-swing-jarfile=<swing-jar>         to set location of the swing jarfile],
dnl SWING_JAR="$withval")
dnl if test -n "$SWING_JAR"; then
dnl   if ! test -f $SWING_JAR; then
dnl     AC_MSG_ERROR(File $SWING_JAR not found.  Use --with-swing-jarfile to set correctly)
dnl   fi
dnl else
dnl   AC_PATH_PROGS(SWING_JAR, swing.jar, "", $JAR_PATH)
dnl   if test -z "$SWING_JAR"; then
dnl     AC_MSG_WARN(The swing jar file was not found.  This is okay if you are using Java1.2 or later.  Use --with-swing-jarfile to set.)
dnl   fi
dnl fi
dnl AC_SUBST(SWING_JAR)

dnl ######################################################################
dnl
dnl Find javadocs documentation programs and setup docs directory
dnl
dnl ######################################################################

AC_PATH_PROGS(JAVADOC, javadoc, "", $JAVA_HOME/bin)
if test -z "$JAVADOC"; then
 AC_MSG_WARN(The javadoc utility was not found in the Java home.  Use --with-java-home to set base directory of java.)
fi
AC_SUBST(JAVADOC)
if test ! -d docs; then mkdir docs; fi
if test ! -d docs/javaapi; then mkdir docs/javaapi; fi
 
dnl ######################################################################
dnl
dnl Link the classes into the html subdirectory and the jars into the 
dnl classes subdirectory
dnl
dnl ######################################################################

echo $ac_n creating the links for Java to work... $ac_c
if test -d html/classes; then 
  for i in "$CLASSES_ZIP" "$SWING_JAR" "$CAPSAPI_ZIP"; do
    if test -n "$i"; then
      fname=`basename $i`
      # echo working on $i
      rm -f html/classes/$fname
      ln -s $i html/classes/$fname
    fi
  done
elif test ! -d classes; then 
  mkdir classes
  cd classes
  for i in "$CLASSES_ZIP" "$SWING_JAR" "$CAPSAPI_ZIP"; do
    if test -n "$i"; then
      fname=`basename $i`
      # echo working on $i
      rm -f $fname
      ln -s $i $fname
    fi
  done
  cd ..
fi

if test ! -d html; then 
  mkdir html
  cd html
  rm -f classes
  ln -s ../classes .
  cd ..
  echo " done."
fi

dnl ######################################################################
dnl
dnl Determine the location of the include and machine dependant include directories.
dnl
dnl ######################################################################


dnl ### everything is different on a mac

case $host in
  *-darwin*)
    JAVA_INCPATH="/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers"
    JAVA_MD_INCPATH=$JAVA_INCPATH
    ;;
  *)
    JAVA_INCPATH=$JAVA_HOME/include
    
    dnl ##### to do: fix this for more hosts
    case "$host" in
      *-linux*)
        JAVA_MD_INCPATH=$JAVA_INCPATH/linux
        ;;
      *-cygwin*)
        JAVA_MD_INCPATH=$JAVA_INCPATH/win32
        ;;
    esac
    ;;
esac

AC_PATH_PROGS(JNI_H, jni.h, "", $JAVA_INCPATH)
if test -z "$JNI_H"; then
  AC_MSG_WARN(Unable to find jni.h in $JAVA_INCPATH.)
fi

AC_PATH_PROGS(JNI_MD_H, jni_md.h, "", $JAVA_MD_INCPATH)
if test -z "$JNI_MD_H"; then
  AC_MSG_WARN(Unable to find jni_md.h in $JAVA_MD_INCPATH.)
fi

AC_SUBST(JAVA_INCPATH)
AC_SUBST(JAVA_MD_INCPATH)

