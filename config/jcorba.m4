dnl ######################################################################
dnl
dnl Check for JOB CORBA (Java ORB) stuff
dnl
dnl ######################################################################

dnl This m4 file assumes that 
dnl       corba.m4
dnl       java.m4
dnl are included before it, so that
dnl       IDL_PATH must is defined
dnl       obv4 may be defined
dnl       classes directory is set up

IDL_PATH=$IDL_PATH:`dirname $IDL`:/usr/local/JOB3/bin
AC_PATH_PROGS(JIDL, jidl, "", $IDL_PATH)
JIDLVER=`$JIDL --version 2>&1`
isorbacus=`echo $JIDLVER | grep ORBacus`
if test -n "$isorbacus"; then
  echo ... Using the ORBacus IDL to Java translator
  JAVAORB=ORBacus
else
  if test -n "$isomni"; then
    echo Originally using omniORB for C++ mapping.
    echo But it does not not have Java translator.
    echo Will set Java translator to be ORBacus.
    JAVAORB=ORBacus
  else
    #echo Could not find the version number: will do ORBacus eval 4.1.1! 
    #JIDLVER=ORBacus 4.1.1
    #JAVAORB=ORBacus
    echo Unknown orb: quitting!
    exit
  fi
fi

dnl ######################################################################
dnl
dnl Get the CORBA jar file
dnl
dnl ######################################################################

AC_ARG_WITH(CORBA-jarfile,
[  --with-CORBA-jarfile=<corba-jar>         to set location of CORBA jar file],
CORBA_JAR="$withval")
dnl Look in the usual path
if test -z "$CORBA_JAR"; then
  if test $JAVAORB = ORBacus; then
# Find equivalent of usr/local
    LOCDIR2=`dirname $JIDL`
    LOCDIR1=`dirname $LOCDIR2`
    LOCDIR=`dirname $LOCDIR1`
    if test -n "$obv4"; then
      CORBA_JAR_PATH=$HOME/$host/JOB4:/usr/local/JOB4:/loc/JOB4:/mfe/local/JOB4
      CORBA_JAR_PATH=$CORBA_JAR_PATH:/local/JOB4
      CORBA_JAR_PATH=$LOCDIR/JOB4:$CORBA_JAR_PATH:/usr/local/JOB4/lib
    else
      CORBA_JAR_PATH=$HOME/$host/JOB3:/usr/local/JOB3:/loc/JOB3:/mfe/local/JOB3
      CORBA_JAR_PATH=$CORBA_JAR_PATH:$HOME/$host/jlib:/usr/local/jlib:/loc/jlib
      CORBA_JAR_PATH=$CORBA_JAR_PATH:/mfe/local/jlib
      CORBA_JAR_PATH=$LOCDIR/JOB3:$CORBA_JAR_PATH
    fi
    echo looking for OB.jar in $CORBA_JAR_PATH
    AC_PATH_PROGS(CORBA_JAR, OB.jar, "", $CORBA_JAR_PATH)
  fi
fi
if test -z "$CORBA_JAR"; then
  AC_MSG_ERROR(The CORBA jar file was not found.  Use --with-CORBA-jarfile to set.)
fi
CORBA_JAR_DIR=`dirname $CORBA_JAR`
AC_SUBST(CORBA_JAR_DIR)
CORBA_BIDIR_JAR=$CORBA_JAR_DIR/OBBiDir.jar
AC_SUBST(CORBA_BIDIR_JAR)
if test -n "$CORBA_JAR"; then
  fname=`basename $CORBA_JAR`
  if test -n "$fname"; then
    rm -f classes/$fname
    ln -s $CORBA_JAR classes/$fname
  fi
fi

dnl ######################################################################
dnl
dnl Find CORBA documentation program
dnl
dnl ######################################################################

AC_PATH_PROGS(HIDL, hidl, "", $IDL_PATH)
if test -z "$HIDL"; then
  AC_MSG_WARN(The hidl utility was not found in your path.  IDL documentation will not be built.)
fi


