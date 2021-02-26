dnl ######################################################################
dnl
dnl File:	cmee.m4
dnl
dnl Purpose:	Determine the locations of cmee includes and libraries. 
dnl 		Allows also for the installation of the ML library. 
dnl 
dnl Version: $Id: cmee.m4 1566 2005-07-11 18:26:54Z cary $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################


dnl ######################################################################
dnl
dnl Allow the user to specify an overall cmee directory.  If specified,
dnl we look for include and lib under this.
dnl
dnl ######################################################################

AC_ARG_WITH(cmee-dir,[  --with-cmee-dir=<location of cmee installation> ],CMEE_DIR="$withval",CMEE_DIR="")

dnl ######################################################################
dnl
dnl Find cmee includes - looking in include location if present,
dnl otherwise in dir/include if present, otherwise in default locations,
dnl first parallel, then serial.
dnl
dnl ######################################################################

AC_ARG_WITH(cmee-incdir,[  --with-cmee-incdir=<location of cmee includes> ],
CMEE_INCDIR="$withval",CMEE_INCDIR="")
if test -n "$CMEE_INCDIR"; then
  CMEE_INCPATH=$CMEE_INCDIR
elif test -n "$CMEE_DIR"; then
  CMEE_INCPATH=$CMEE_DIR/include
else
  CMEE_INCPATH=$HOME/$UNIXFLAVOR/cmee/include:$HOME/cmee/include:/usr/local/cmee/include
fi

if test -n "$CMEE_INCPATH"; then 
  AC_PATH_PROGS(CMEE_H, cmee.h, "", $CMEE_INCPATH)
  if test -z "$CMEE_H"; then
    AC_MSG_WARN(cmee.h not found in $CMEE_INCPATH.  Set with --with-cmee-incdir=)
    CMEE_INC=" "
  else
    CMEE_INCDIR=`dirname $CMEE_H`
    AC_SUBST(CMEE_INCDIR)
    CMEE_INC="-I$CMEE_INCDIR"
    AC_SUBST(CMEE_INC)
    CMEE_DIR=`dirname $CMEE_INCDIR`
  fi
fi

dnl ######################################################################
dnl
dnl Find cmee libraries
dnl
dnl ######################################################################

AC_ARG_WITH(cmee-libdir,[  --with-cmee-libdir=<location of cmee library> ],
CMEE_LIBDIR="$withval",CMEE_LIBDIR="")

if test -n "$CMEE_INCDIR"; then
  if test -n "$CMEE_LIBDIR"; then
    CMEE_LIBPATH=$CMEE_LIBDIR
  else
    CMEE_LIBPATH=$CMEE_DIR/lib/$COMPDIR:$CMEE_DIR/lib:$CMEE_INCDIR/.libs
  fi
  AC_PATH_PROGS(LIBCMEE_A, libcmee.a, "", $CMEE_LIBPATH)
  if test -z "$LIBCMEE_A"; then
    AC_MSG_WARN(libcmee.a not found in $CMEE_LIBPATH.  Set location with --with-txmath-libdir=)
    CMEE_LIB=""
  else
    CMEE_LIBDIR=`dirname $LIBCMEE_A`
    AC_SUBST(CMEE_LIBDIR)
    CMEE_LIB="-lcmee"
    CMEE_LIBS="-L$CMEE_LIBDIR -lcmee $FLIBS"' $(RPATH_FLAG)'"$CMEE_LIBDIR"
  fi
  AC_SUBST(CMEE_LIB)
  AC_SUBST(CMEE_LIBS)
fi

# if test -n "$CMEE_LIBDIR"; then
#   CMEE_LIBPATH=$CMEE_LIBDIR
# elif test -n "$CMEE_DIR"; then
#   CMEE_LIBPATH=$CMEE_DIR/lib
# else
#   CMEE_LIBPATH=""
# fi
# 
# if test -n "$CMEE_LIBPATH"; then
#   AC_PATH_PROGS(LIBCMEE_A, libcmee.a, "", $CMEE_LIBPATH)
#   if test -z "$LIBCMEE_A"; then
#     AC_MSG_WARN(libcmee.a not found.  Set with --with-cmee-libdir=)
#     CMEE_LIBS=""
#   else
#     CMEE_LIBDIR=`dirname $LIBCMEE_A`
#     AC_SUBST(CMEE_LIBDIR)
#     CMEE_LIBS="-L$CMEE_LIBDIR -lcmee $FLIBS"' $(RPATH_FLAG)'"$CMEE_LIBDIR"
#   fi
#   AC_SUBST(CMEE_LIBS)
# fi

dnl ######################################################################
dnl
dnl Define for whether cmee found
dnl
dnl ######################################################################

if test -n "$CMEE_LIBS"; then
  AC_DEFINE(HAVE_CMEE)
fi

