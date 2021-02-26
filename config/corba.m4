dnl ######################################################################
dnl
dnl File:	corba.m4
dnl
dnl Purpose:	Find CORBA stuff for Makefiles
dnl
dnl Version:	$Id: corba.m4 2272 2007-05-22 17:53:34Z roopa $
dnl
dnl Copyright 2001, Tech-X Corporation.  This file may be freely
dnl distributed provided copyright statement remains in place.
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Get the IDL to C++ translator
dnl
dnl ######################################################################

AC_MSG_WARN(If this hangs - you may have RSI's idl (Interactive Data Language) in your path ahead of the Interface Definition Language translator for CORBA.  You will need to change your path.)

##########
#
# CORBA_IDL_PROG is the name of the idl translator.  It can be
# set to tao_idl to require use of TAO
#
##########

if test -z "$CORBA_IDL_PROG"; then
  CORBA_IDL_PROG=idl
fi

##########
#
# Allow user to set absolute path to the idl translator.
# Otherwise look in standard places
#
##########

AC_ARG_WITH(corba-idl,
[  --with-corba-idl=<corba-idl>  set location of the CORBA idl translator],
CORBA_IDL="$withval")
if test -n "$CORBA_IDL"; then
  IDL=$CORBA_IDL
else
  if test -n "$USEOB3"; then
    IDL_PATH=$HOME/$host/OB3-${COMPDIR}/bin:$HOME/$host/OB3/bin:/usr/local/OB3-${COMPDIR}/bin:/usr/local/OB3/bin:/loc/OB3-${COMPDIR}/bin:/local/OB3-${COMPDIR}/bin:/loc/OB3/bin:/mfe/local/OB3-${COMPDIR}/bin:/mfe/local/OB3/bin:$PATH
  else
    IDL_PATH=$HOME/$host/OB4-${COMPDIR}/bin:$HOME/$host/OB4/bin:/usr/local/OB4-${COMPDIR}/bin:/usr/local/OB4/bin:/usr/local/ACE+TAO/TAO/TAO_IDL:/local/OB4-${COMPDIR}/bin:/loc/OB4-${COMPDIR}/bin:/loc/OB4/bin:/mfe/local/OB4-${COMPDIR}/bin:/mfe/local/OB4/bin:$PATH
  fi
  AC_PATH_PROGS(IDL, $CORBA_IDL_PROG, "", $IDL_PATH)
fi

IDL_DIR=`dirname $IDL`

# Try to get version
# Test whether omniORB, TAO or MICO is being used
isomni=`echo $IDL | grep omni`
istao=`echo $IDL | grep tao`
ismico=`echo $IDL | grep mico`

if test -n "$isomni" -o -n "$istao"; then
# This command works for tao and omniORB
  IDLVER=`$IDL -V 2>&1`
else
# This command is for orbacus and mico
  IDLVER=`$IDL --version 2>&1`
fi

echo IDLVER = $IDLVER

# Distinguish between orbacus and mico
if test -n "$ismico"; then
  echo ... Using the MICO IDL to C++ Translator
  CXXORB=MICO
  TESTINC=CORBA.h
else
  isorbacus=`echo $IDLVER | grep ORBacus`
  if test -n "$isorbacus"; then
    echo ... Using the ORBacus IDL to C++ translator, $IDL
    obv4=`echo $IDLVER | grep " 4"`
    obv4_1=`echo $IDLVER | grep " 4.1"`
    echo $IDLVER
    #echo obv4 = \"$obv4\"
    if test -n "$obv4"; then
      if test -n "$obv4_1"; then
        echo Using Orbacus 4.1
      else
        echo Using Orbacus 4.0
      fi
    else
      echo Using Orbacus 3
    fi
    CXXORB=ORBacus
    TESTINC=OB/CORBA.h
  else
    if test -n "$isomni"; then
       echo ... Using the omniORB IDL to C++ Translator
       CXXORB=omniORB
       TESTINC=CORBA.h
    else
	if test -n "$istao"; then
	  echo ... Using the TAO IDL to C++ Translator
	  CXXORB=tao
	  TESTINC=corba.h
	else
          echo Unknown ORB: quitting!
          exit
        fi
    fi
  fi
fi

dnl ######################################################################
dnl
dnl Get the CORBA include files
dnl
dnl ######################################################################

AC_ARG_WITH(corba-incdir,
[  --with-corba-incdir=<corba-incdir-dir>  set location of the CORBA include directory],
CORBA_INCDIR="$withval")

dnl If not known, check in typical directories

if test -n "$CORBA_INCDIR"; then
  CORBA_INCPATH=$CORBA_INCDIR
else
  if test -n "$obv4"; then
    CORBA_INCPATH=$HOME/$host/OB4-${COMPDIR}/include:$HOME/$host/OB4/include
    CORBA_INCPATH=$CORBA_INCPATH:/usr/local/OB4-${COMPDIR}/include:
    CORBA_INCPATH=$CORBA_INCPATH:/usr/local/OB4/include:
    CORBA_INCPATH=$CORBA_INCPATH:/loc/OB4-${COMPDIR}/include:/loc/OB4/include
    CORBA_INCPATH=$CORBA_INCPATH:/mfe/local/OB4-${COMPDIR}/include:/mfe/local/OB4/include
    CORBA_INCPATH=$CORBA_INCPATH:/local/OB4-${COMPDIR}/include:/local/OB4/include
  else
    CORBA_INCPATH=$HOME/$host/OB3-${COMPDIR}/include:$HOME/$host/OB3/include
    CORBA_INCPATH=$CORBA_INCPATH:/usr/local/OB3-${COMPDIR}/include:
    CORBA_INCPATH=$CORBA_INCPATH:/usr/local/OB3/include:
    CORBA_INCPATH=$CORBA_INCPATH:/loc/OB3-${COMPDIR}/include:/loc/OB3/include
    CORBA_INCPATH=$CORBA_INCPATH:/mfe/local/OB3-${COMPDIR}/include:
    CORBA_INCPATH=$CORBA_INCPATH:/mfe/local/OB3/include
    CORBA_INCPATH=$CORBA_INCPATH:$HOME/$host/OB-${COMPDIR}/include:$HOME/$host/OB/include
    CORBA_INCPATH=$CORBA_INCPATH:/usr/local/OB-${COMPDIR}/include:
    CORBA_INCPATH=$CORBA_INCPATH:/usr/local/OB/include:
    CORBA_INCPATH=$CORBA_INCPATH:/loc/OB-${COMPDIR}/include:/loc/OB/include
    CORBA_INCPATH=$CORBA_INCPATH:/mfe/local/OB-${COMPDIR}/include:
    CORBA_INCPATH=$CORBA_INCPATH:/mfe/local/OB/include
  fi
fi

CORBA_INCPATH=`dirname $IDL_DIR`/include:$CORBA_INCPATH

# If omniORB, this is the include directory
if test -n "$isomni"; then
    CORBA_INCPATH=/usr/local/omniORB4/include/omniORB4
fi

# If MICO, there are several include directories
if test -n "$ismico"; then
    MICO_ROOT=`dirname $IDL_DIR`
    CORBA_INCPATH=$MICO_ROOT/include
    AC_SUBST(MICO_VERSION)
    IDL="MICO_ROOT=$MICO_ROOT LD_LIBRARY_PATH=$MICO_ROOT/lib:$LD_LIBRARY_PATH $IDL"
fi

if test -n "$istao"; then
  if test "x$TAO_ROOT" = "x"; then
    AC_MSG_NOTICE([Environment variable TAO_ROOT is not defined.])
    TAO_ROOT=`dirname $IDL_DIR`
  elif test "$TAO_ROOT/TAO_IDL" != "$IDL_DIR"; then
    AC_MSG_ERROR([Ennvironment variable TAO_ROOT($TAO_ROOT) does not match the $IDL used])
  fi

  dnl We will trust the ACE_ROOT definition if there is one
  if test "x$ACE_ROOT" = "x"; then
    ACE_ROOT=`dirname $TAO_ROOT`
    if test ! -e "$ACE_ROOT/ace/config.h"; then
      dnl Newer version of TAO could use different directory hierarchy
      ACE_ROOT=$ACE_ROOT/ACE
    fi
  fi

  CORBA_INCPATH=$TAO_ROOT/tao

  if test -e "$ACE_ROOT/lib/libTAO$SO"; then
    ACE_LIB_PATH=$ACE_ROOT/lib
  else
    ACE_LIB_PATH=$ACE_ROOT/ace
  fi

    case "$host" in
      powerpc-*-darwin*)
        IDL="ACE_ROOT=$ACE_ROOT TAO_ROOT=$TAO_ROOT DYLD_LIBRARY_PATH=$IDL_DIR:$ACE_LIB_PATH:$DYLD_LIBRARY_PATH $IDL"
        ;;
      i686-*-linux* | i386-*-linux*)
        IDL="ACE_ROOT=$ACE_ROOT TAO_ROOT=$TAO_ROOT LD_LIBRARY_PATH=$IDL_DIR:$ACE_LIB_PATH:$LD_LIBRARY_PATH $IDL "
        case "$SERIALCXX" in
          *++ | *g++3)
	    CORBA_CXXFLAGS="-D_POSIX_THREAD -D_POSIX_THREAD_SAFE_FUNCTIONS -D_REENTRANT -DACE_HAS_AIO_CALLS"
          ;;
        esac
        ;;
      *-sgi-irix6*)
        # IDL="ACE_ROOT=$ACE_ROOT TAO_ROOT=$TAO_ROOT LD_LIBRARY_PATH=$IDL_DIR:$ACE_ROOT/ace:$LD_LIBRARY_PATH $IDL -Ge 1 -Sc"
        CORBA_CXXFLAGS="-DACE_HAS_EXCEPTIONS -diag_suppress 3284 -ptused -prelink"
        ;;
      *)
        ;;
   esac

#  echo TAO_ROOT=$TAO_ROOT
#  echo ACE_ROOT=$ACE_ROOT

  echo IDL = \"$IDL\"
  CORBA_INCDIR2=$ACE_ROOT
else CORBA_INCDIR=
  CORBA_INCDIR2=.
fi
AC_SUBST(TAO_ROOT)
AC_SUBST(ACE_ROOT)
AC_SUBST(ACE_LIB_PATH)
AC_SUBST(MICO_ROOT)
AC_SUBST(CORBA_CXXFLAGS)

echo CORBA_INCPATH = $CORBA_INCPATH
AC_PATH_PROGS(ABS_CORBA_INCDIR, $TESTINC,"", $CORBA_INCPATH)

if test -z "$ABS_CORBA_INCDIR"; then
  AC_MSG_ERROR(CORBA includes not found in $CORBA_INCPATH.  Use --with-corba-incdir to set the location of $TESINC.)
fi

CORBA_INCSUBDIR=`dirname $ABS_CORBA_INCDIR`
#echo CORBA_INCSUBDIR=$CORBA_INCSUBDIR
CORBA_INCDIR=`dirname $CORBA_INCSUBDIR`
if test -n "$ismico"; then
    CORBA_INCDIR=$CORBA_INCSUBDIR
fi
CORBA_DIR=`dirname $CORBA_INCDIR`

echo Corba include directory is $CORBA_INCDIR
echo CORBA_DIR=$CORBA_DIR

AC_SUBST(CORBA_INCDIR)
AC_SUBST(CORBA_INCDIR2)

dnl ######################################################################
dnl
dnl Get the JThreads++ include files if using Orbacus 4
dnl
dnl ######################################################################

#echo "######################################################################"
#echo "obv4 = $obv4"
#echo "######################################################################"

if test -z "$obv4"; then

  JTC_INCDIR=.

else

  AC_ARG_WITH(jtc-incdir,
  [  --with-jtc-incdir=<jthreads include directory>      to set location of JThreads/C++ headers],
  JTC_INCDIR="$withval")

  if test -n "$JTC_INCDIR"; then
    JTC_INCPATH=$JTC_INCDIR
  else
    JTC_INCPATH=$HOME/$host/jtc-${COMPDIR}/include:$HOME/$host/jtc/include
    JTC_INCPATH=$JTC_INCPATH:/usr/local/jtc-${COMPDIR}/include:
    JTC_INCPATH=$JTC_INCPATH:/usr/local/jtc/include:
    JTC_INCPATH=$JTC_INCPATH:/loc/jtc-${COMPDIR}/include:/loc/jtc/include
    JTC_INCPATH=$JTC_INCPATH:/mfe/local/jtc-${COMPDIR}/include:/mfe/local/jtc/include
    JTC_INCPATH=$JTC_INCPATH:/local/jtc-${COMPDIR}/include:/local/jtc/include
  fi

  AC_PATH_PROGS(ABS_JTC_INCDIR, JTC/JTC.h,"", $JTC_INCPATH)
  if test -z "$ABS_JTC_INCDIR"; then
    AC_MSG_ERROR(JTC includes not found in $JTC_INCPATH.  Use --with-jtc-incdir to set the location of JTC/JTC.h.)
  fi

  JTC_INCDIR1=`dirname $ABS_JTC_INCDIR`
  JTC_INCDIR=`dirname $JTC_INCDIR1`
  JTC_DIR=`dirname $JTC_INCDIR`

fi

AC_SUBST(JTC_INCDIR)
AC_SUBST(JTC_DIR)

dnl ######################################################################
dnl
dnl Determine the correct CORBA library
dnl
dnl ######################################################################

AC_ARG_WITH(corba-libdir,
[  --with-corba-libdir=<corba library directory>      to set location of corba libraries],
CORBA_LIBDIR="$withval")

# Set in case not found or used

# Try to guess the corba library directory
case $CXXORB in

  ORBacus)
    if test -n "$CORBA_LIBDIR"; then
      CORBA_LIBPATH=$CORBA_LIBDIR
    else
      CORBA_LIBPATH=$CORBA_DIR/lib/${COMPDIR}:$CORBA_DIR/lib
    fi
    AC_PATH_PROGS(ABS_CORBA_LIB, libOB.a libOB$SO, "", $CORBA_LIBPATH)
    if test -z "$ABS_CORBA_LIB"; then
      AC_MSG_ERROR(Unable to find libOB.a or libOB$SO in $CORBA_LIBPATH.  Set the appropriate directory using --with-corba-libdir)
    fi
    CORBA_LIBDIR=`dirname $ABS_CORBA_LIB`
    if test -n "$obv4"; then
      if test -n "$obv4_1"; then
        CORBA_LIB="-lOB"
      else
        CORBA_LIB="-lOB -lOBBiDir"
      fi
    else
      CORBA_LIB="-lOB"
    fi
    CORBA_LIBS="-L$CORBA_LIBDIR $CORBA_LIB"

dnl Find the JThreads/C++ libraries
    if test -n "$obv4"; then
      AC_ARG_WITH(jtc-libdir,
      [  --with-jtc-libdir=<jthreads libdir>      to set location of JThreads/C++ libraries],
      JTC_LIBDIR="$withval")
      if test -n "$JTC_LIBDIR"; then
        JTC_LIBPATH=$JTC_LIBDIR
      else
        JTC_LIBPATH=$JTC_DIR/lib/${COMPDIR}:$JTC_DIR/lib
	# =$HOME/$host/jtc-${COMPDIR}/lib:$HOME/$host/jtc/lib/$COMPDIR:/usr/local/jtc-${COMPDIR}/lib:/usr/local/jtc/lib/$COMPDIR:/loc/jtc-${COMPDIR}/lib:/loc/jtc/lib/$COMPDIR:/local/jtc-${COMPDIR}/lib:/mfe/local/jtc-${COMPDIR}/lib:/mfe/local/jtc/lib/$COMPDIR
      fi
      AC_PATH_PROGS(ABS_JTC_LIB, libJTC.a libJTC$SO, "", $JTC_LIBPATH)
      if test -z "$ABS_JTC_LIB"; then
        AC_MSG_WARN(Unable to find libJTC.a or libJTC$SO in $JTC_LIBPATH.  Set the appropriate directory using --with-jtc-libdir if needed)
      else
        JTC_LIBDIR=`dirname $ABS_JTC_LIB`
        JTC_LIB="-lJTC"
        JTC_LIBS="-L$JTC_LIBDIR $JTC_LIB"
        CORBA_LIB="$CORBA_LIB $JTC_LIB"
        CORBA_LIBS="$CORBA_LIBS $JTC_LIBS"
      fi
    fi
#   Minimal libraries for server and client apps.
    CORBA_SVR_LIBS="$CORBA_LIBS"
    CORBA_CLT_LIBS="$CORBA_LIBS"

    ;;

  MICO)
    if test -n "$CORBA_LIBDIR"; then
      CORBA_LIBPATH=$CORBA_LIBDIR
    else
      CORBA_LIBPATH=$CORBA_DIR/lib:$HOME/$host/mico-${COMPDIR}/lib:$HOME/$host/mico/lib/$COMPDIR:/usr/local/mico-${COMPDIR}/lib:/usr/local/mico/lib/$COMPDIR:/loc/mico-${COMPDIR}/lib:/loc/mico/lib/$COMPDIR:/mfe/local/mico-${COMPDIR}/lib:/mfe/local/mico/lib/$COMPDIR
    fi
    AC_PATH_PROGS(ABS_CORBA_LIB, libmico2.3.11.a libmicocoss2.3.11$SO, "", $CORBA_LIBPATH)
    if test -z "$ABS_CORBA_LIB"; then
      AC_MSG_ERROR(Unable to find libmico.a or libmico$SO in $CORBA_LIBPATH.  Set the appropriate directory using --with-corba-libdir)
    fi
    CORBA_LIBDIR=`dirname $ABS_CORBA_LIB`
    CORBA_LIB="-lmico2.3.11 -lmicocoss2.3.11"
    CORBA_LIBS="-L$CORBA_LIBDIR $CORBA_LIB"
#   Minimal libraries for server and client apps.
    CORBA_SVR_LIBS="$CORBA_LIBS"
    CORBA_CLT_LIBS="$CORBA_LIBS"
    ;;

  omniORB)
    if test -n "$CORBA_LIBDIR"; then
      CORBA_LIBPATH=$CORBA_LIBDIR
    else
      CORBA_LIBPATH=/usr/local/omniORB4/lib/i586_linux_2.0_glibc2.1
    fi
    AC_PATH_PROGS(ABS_CORBA_LIB, libomniORB4.a libomniORB4$SO, "", $CORBA_LIBPATH)
    if test -z "$ABS_CORBA_LIB"; then
      AC_MSG_ERROR(Unable to find libomniORB4.a or libomniORB4$SO in $CORBA_LIBPATH.  Set the appropriate directory using --with-corba-libdir)
    fi
    CORBA_LIBDIR=`dirname $ABS_CORBA_LIB`
    CORBA_LIB="-lomniORB4 -lbsd -lpthread -lomnithread -lomniDynamic"
    CORBA_LIBS="-L$CORBA_LIBDIR $CORBA_LIB"
#   Minimal libraries for server and client apps.
    CORBA_SVR_LIBS="$CORBA_LIBS"
    CORBA_CLT_LIBS="$CORBA_LIBS"
    ;;

  tao)
    if test -n "$CORBA_LIBDIR"; then
      CORBA_LIBPATH=$CORBA_LIBDIR
    else
      CORBA_LIBPATH=$TAO_ROOT/tao
    fi
    AC_PATH_PROGS(ABS_CORBA_LIB, libTAO.a libTAO$SO libTAO.dylib, "", 
	$CORBA_LIBPATH)
    if test -z "$ABS_CORBA_LIB"; then
      AC_MSG_ERROR(Unable to find libTAO.a or libTAO$SO or libTAO.dylib in $CORBA_LIBPATH.  Set the appropriate directory using --with-corba-libdir)
    fi
#    CORBA_LIBDIR=`dirname $ABS_CORBA_LIB`
    CORBA_LIBDIR=$ACE_LIB_PATH
    CORBA_LIB="-lTAO_CosNaming -lTAO_IFR_Client -lTAO_Svc_Utils -lTAO_IORTable -lTAO_BiDirGIOP -lTAO_DynamicInterface -lTAO_DynamicAny -lTAO_TypeCodeFactory -lTAO_DynamicAny -lTAO_Strategies -lTAO_PortableServer -lTAO -lACE"
#   Minimal libraries for server and client applications.
    CORBA_SVR_LIBS="-L$CORBA_LIBDIR -lTAO_PortableServer -lTAO -lACE -lTAO_Strategies"
    CORBA_CLT_LIBS="-L$CORBA_LIBDIR -lTAO -lACE"
    CORBA_LIBS="-L$CORBA_LIBDIR $CORBA_LIB"
    ;;

esac

dnl make substitutions
AC_SUBST(CORBA_LIBDIR)
AC_SUBST(CORBA_LIB)
AC_SUBST(CORBA_SVR_LIBS)
AC_SUBST(CORBA_CLT_LIBS)
AC_SUBST(CORBA_LIBS)
if test -z "$JTC_LIBDIR"; then JTC_LIBDIR=.; fi
AC_SUBST(JTC_LIBDIR)
AC_SUBST(JTC_LIB)
