dnl ######################################################################
dnl
dnl File:	txbase.m4
dnl
dnl Purpose:	Determine the locations of txbase includes and libraries
dnl
dnl Version: 	$Id: txbase.m4 2383 2007-08-28 09:54:56Z cary $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Allow the user to specify an overall txbase directory.  If specified,
dnl we look for include and lib under this.
dnl
dnl ######################################################################

AC_ARG_WITH(txbase-dir, [  --with-txbase-dir=<txbase installation directory>],
	TXBASE_DIR="$withval", TXBASE_DIR="")

dnl Enable Command-line override
if test -n "$TXBASE_LIBS"; then SAVE_TXBASE_LIBS="$TXBASE_LIBS"; fi
if test -n "$TXBASE_INC"; then SAVE_TXBASE_INC="$TXBASE_INC"; fi
if test -n "$TXBASE_LIBDIR"; then SAVE_TXBASE_LIBDIR="$TXBASE_LIBDIR"; fi
dnl if given txbase_libdir from commandline then look there
if test -n "$SAVE_TXBASE_LIBDIR"; then
     TXBASE_DIR=`dirname $SAVE_TXBASE_LIBDIR`
fi
dnl ######################################################################
dnl
dnl Find txbase includes - looking in include location if present,
dnl otherwise in dir/include if present, otherwise in default locations,
dnl first parallel, then serial.
dnl
dnl ######################################################################

AC_ARG_WITH(txbase-incdir, [  --with-txbase-incdir=<txbase include directory>],
	TXBASE_INCDIR="$withval",TXBASE_INCDIR="")
if test -n "$TXBASE_INCDIR"; then
  TXBASE_INCPATH=$TXBASE_INCDIR
elif test -n "$TXBASE_DIR"; then
  TXBASE_INCPATH=$TXBASE_DIR/include
else
dnl JRC: The $HOME part should be first to be chosen if available.
  TXBASE_INCPATH=$HOME/$UNIXFLAVOR/txbase/include:$HOME/txbase/include:/internal/txbase/include:/usr/local/txbase/include:$HOME/$UNIXFLAVOR/optsolve/include:$HOME/optsolve/include:/usr/local/optsolve/include:/loc/optsolve/include:/local/optsolve/include:/opt/optsolve/include
fi

TX_PATH_FILE(TXC_MATH_H, txc_math.h, "", $TXBASE_INCPATH)

if test -z "$TXC_MATH_H"; then
  AC_MSG_WARN(txc_math.h not found in $TXBASE_INCPATH.  Set location with --with-txbase-incdir=)
  TXBASE_INC=""
else
  TXBASE_INCDIR=`dirname $TXC_MATH_H`
  AC_SUBST(TXBASE_INCDIR)
  TXBASE_INC=-I$TXBASE_INCDIR
  AC_SUBST(TXBASE_INC)
  TXBASE_DIR=`dirname $TXBASE_INCDIR`
  inoptsolve=`echo $TXBASE_INCDIR | grep optsolve`
  if test -n "$inoptsolve"; then
    AC_MSG_WARN(Warning: installing txbase into optsolve is deprecated.)
  fi
fi

dnl ######################################################################
dnl
dnl Find txbase libraries
dnl
dnl ######################################################################

AC_ARG_WITH(txbase-libdir,[  --with-txbase-libdir=<location of txbase library> ], TXBASE_LIBDIR="$withval",TXBASE_LIBDIR="")
if test -n "$TXBASE_INCDIR"; then
  if test -n "$TXBASE_LIBDIR"; then
    TXBASE_LIBPATH=$TXBASE_LIBDIR
  else
    TXBASE_LIBPATH=$TXBASE_DIR/lib/$COMPDIR:$TXBASE_DIR/lib
  fi
  TX_PATH_FILE(LIBTXBASE_A, libtxbase.a, "", $TXBASE_LIBPATH)
  if test -z "$LIBTXBASE_A"; then
    AC_MSG_WARN(libtxbase.a not found in $TXBASE_LIBPATH.  Set location with --with-txbase-libdir=)
    TXBASE_LIB=""
    TXBASE_LIBS=""
  else
    TXBASE_LIBDIR=`dirname $LIBTXBASE_A`
    TXBASE_LIB="-ltxbase"
    TXBASE_LIBS="-L$TXBASE_LIBDIR "'$(RPATH_FLAG)'"$TXBASE_LIBDIR $TXBASE_LIB"
  fi
fi
AC_SUBST(TXBASE_LIBDIR)
AC_SUBST(TXBASE_LIB)
AC_SUBST(TXBASE_LIBS)

dnl ######################################################################
dnl
dnl Find txbase version file
dnl
dnl ######################################################################

dnl Check TxBase version number
if test -n "$TXBASE_INCDIR"; then
  TX_PATH_FILE(TXBASE_VERSION_H, txbase_version.h, "", $TXBASE_INCDIR)
  if test -z "$TXBASE_VERSION_H"; then
dnl JRC: changed to warning to that configure.ac does the kill.
    AC_MSG_WARN(Cannot find txbase_version.h in $TXBASE_INCDIR!
    Please install TxBase or use --with-txbase-dir=)
  fi
fi

dnl ######################################################################
dnl
dnl Verify that version is sufficiently new
dnl
dnl ######################################################################

if test -n "$TXBASE_VERSION_H"; then
  AC_MSG_CHECKING(TxBase version)
  TXBASE_VERSION=`grep TXBASE_VERSION $TXBASE_VERSION_H | sed 's/^.* \"//' | sed 's/\"//g'`
  if test -z "$TXBASE_VERSION"; then
    AC_MSG_RESULT(not found.)
    AC_MSG_WARN(TXBASE_VERSION not present in $TXBASE_VERSION_H.  Please reinstall.)
  else
    AC_MSG_RESULT($TXBASE_VERSION)
    TXBASE_MAJOR_VERSION=`echo $TXBASE_VERSION | sed 's/\..*$//'`

    if test -z "$TXBASE_OK_MAJOR_VERSION"; then
      TXBASE_OK_MAJOR_VERSION=1
    fi
    if test -z "$TXBASE_OK_MINOR_VERSION"; then
      TXBASE_OK_MINOR_VERSION=0
    fi
    if test -z "$TXBASE_OK_RELEASE"; then
      TXBASE_OK_RELEASE=0
    fi
    if test -z "$TXBASE_OK_VERSION"; then
      TXBASE_OK_VERSION=$TXBASE_OK_MAJOR_VERSION.$TXBASE_OK_MINOR_VERSION.$TXBASE_OK_RELEASE
    fi

    if test $TXBASE_MAJOR_VERSION -lt $TXBASE_OK_MAJOR_VERSION; then
      echo Major version must be at least $TXBASE_OK_MAJOR_VERSION
      exit
    fi
    if test $TXBASE_MAJOR_VERSION -eq $TXBASE_OK_MAJOR_VERSION; then
      TXBASE_MINOR_VERSION=`echo $TXBASE_VERSION | sed "s/^$TXBASE_MAJOR_VERSION\.//" | sed 's/\..*$//'`
      # echo TXBASE_MINOR_VERSION = $TXBASE_MINOR_VERSION
      if test $TXBASE_MINOR_VERSION -lt $TXBASE_OK_MINOR_VERSION; then
        echo Minor version must be at least $TXBASE_OK_MINOR_VERSION
        exit
      fi
      if test $TXBASE_MINOR_VERSION -eq $TXBASE_OK_MINOR_VERSION; then
        TXBASE_RELEASE=`echo $TXBASE_VERSION | sed 's/^.*\.//'`
        # echo TXBASE_RELEASE = $TXBASE_RELEASE
        if test $TXBASE_RELEASE -lt $TXBASE_OK_RELEASE; then
          echo Minor version must be at least $TXBASE_OK_RELEASE
          exit
        fi
      fi
    fi
    echo TxBase of sufficiently high version
  fi

dnl JRC, 22May06: Why define this?  Have substituted it.
  AC_DEFINE_UNQUOTED([TXBASE_INCDIR], "$TXBASE_INCDIR",
	"txbase include directory")

fi

dnl ######################################################################
dnl Enable Command-line override
if test -n "$SAVE_TXBASE_INC";  then TXBASE_INC="$SAVE_TXBASE_INC"; fi
if test -n "$SAVE_TXBASE_LIBS"; then TXBASE_LIBS="$SAVE_TXBASE_LIBS"; fi
if test -n "$SAVE_TXBASE_LIBDIR"; then TXBASE_LIBDIR="$SAVE_TXBASE_LIBDIR"; fi

dnl ######################################################################
dnl
dnl    Write to summary file if defined
dnl
dnl ######################################################################
if test -n "$config_summary_file"; then
      echo                                        >> $config_summary_file
      echo "Using TxBase with"                    >> $config_summary_file
      echo "  TXBASE_INC:    $TXBASE_INC"         >> $config_summary_file
      echo "  TXBASE_LIBDIR: $TXBASE_LIBDIR"      >> $config_summary_file
      echo "  TXBASE_LIBS:   $TXBASE_LIBS"        >> $config_summary_file
fi

