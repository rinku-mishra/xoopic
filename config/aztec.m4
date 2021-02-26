dnl ######################################################################
dnl
dnl File:	aztec.m4
dnl
dnl Purpose:	Determine the locations of aztec includes and libraries.
dnl 		Allows also for the installation of the ML library.
dnl
dnl Assumptions:
dnl             there are some assumptions about the libraries:
dnl             ML:
dnl               it is assumed that ml is compiled with SUPERLU support.
dnl               recompile superlu if it does not support it.
dnl             AZTEC:
dnl               it seems that aztec requires the library g2c/gfortran on
dnl               some systems. It is not clear yet, if other OS require a
dnl               similar library. If your OS requires other libraries to
dnl               built aztec, add them in the OS branch below.
dnl
dnl
dnl Version: $Id: aztec.m4 1952 2006-07-28 15:09:45Z cary $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################


dnl ######################################################################
dnl
dnl First find if the user supplies also the ml library.
dnl
dnl ######################################################################


dnl ######################################################################
dnl
dnl Find ml includes - looking in include location if present,
dnl otherwise in dir/include if present, otherwise in default locations,
dnl first parallel, then serial.
dnl
dnl ######################################################################

AC_ARG_WITH(ml-dir, [  --with-ml-dir=<location of ml installation> ],
  ML_DIR = "$withval", ML_DIR="")
if test -n "$ML_DIR"; then
  ML_DIR=$ML_DIR
else
dnl do not try to guess the ml location. Either the user provides it or
dnl we ignore it.
  ML_DIR=""
fi

AC_ARG_WITH(ml-incdir,[  --with-ml-incdir=<location of ml includes> ],
  ML_INCDIR="$withval",ML_INCDIR="")
if test -n "$ML_INCDIR"; then
  ML_INCPATH=$ML_INCDIR
elif test -n "$ML_DIR"; then
dnl ML has carzy locations for include files and libraries
  ML_INCPATH=$ML_DIR/lib
else
  ML_INCPATH=""
fi

if test -n "$ML_INCPATH"; then
 AC_PATH_PROGS(ML_H, ml_include.h, "", $ML_INCPATH)
 if test -z "$ML_H"; then
   AC_MSG_WARN(ml_include.h not found in $ML_INCPATH.  Set with --with-ml-incdir=)
   ML_INC=" "
   ac_cv_have_ml=no
 else
   ML_INCDIR=`dirname $ML_H`
   AC_SUBST(ML_INCDIR)
   ML_INC=-I$ML_INCDIR
   AC_SUBST(ML_INC)
   ML_DIR=`dirname $ML_INCDIR`
   ac_cv_have_ml=yes
 fi
else
 ac_cv_have_ml=no
fi

dnl ######################################################################
dnl
dnl Find ml libraries
dnl
dnl ######################################################################

AC_ARG_WITH(ml-libdir,[  --with-ml-libdir=<location of ml library> ],
  ML_LIBDIR="$withval",ML_LIBDIR="")
if test $ac_cv_have_ml = yes; then
  if test -n "$ML_LIBDIR"; then
    ML_LIBPATH=$ML_LIBDIR
  else
    ML_LIBPATH=$ML_DIR/lib
  fi
  AC_PATH_PROGS(LIBML_A, libml.a, "", $ML_LIBPATH)
  if test -z "$LIBML_A"; then
    AC_MSG_WARN(libml.a not found.  Set with --with-ml-libdir=)
    ac_cv_have_ml=no
    ML_LIB=" "
  else
    ML_LIBDIR=`dirname $LIBML_A`
    AC_SUBST(ML_LIBDIR)
    ML_LIB="-L$ML_LIBDIR -lml -lsuperlu"
  fi
  AC_SUBST(ML_LIB)
fi

dnl ######################################################################
dnl
dnl Define for whether ml found
dnl
dnl ######################################################################

if test $ac_cv_have_ml = yes; then
   AC_DEFINE(HAVE_ML, , Define if the Aztec multilevel library present)
fi


dnl ######################################################################
dnl
dnl Now here comes the actual AZTEC stuff.
dnl If AZTEC is not supported, the provided ML library will be ignored
dnl
dnl ######################################################################


dnl ######################################################################
dnl
dnl Allow the user to specify an overall aztec directory.  If specified,
dnl we look for include and lib under this.
dnl
dnl ######################################################################

AC_ARG_WITH(aztec-dir,[  --with-aztec-dir=<location of aztec installation> ],AZTEC_DIR="$withval",AZTEC_DIR="")

dnl ######################################################################
dnl
dnl Find aztec includes - looking in include location if present,
dnl otherwise in dir/include if present, otherwise in default locations,
dnl first parallel, then serial.
dnl
dnl ######################################################################

# echo parallel = $parallel
# echo AZTEC_INCDIR = $AZTEC_INCDIR
AC_ARG_WITH(aztec-incdir,[  --with-aztec-incdir=<location of aztec includes> ],
AZTEC_INCDIR="$withval",AZTEC_INCDIR="")
# echo AZTEC_INCDIR = $AZTEC_INCDIR
if test -n "$AZTEC_INCDIR"; then
  AZTEC_INCPATH=$AZTEC_INCDIR
elif test -n "$AZTEC_DIR"; then
dnl AZTEC has some crazy default paths for include files
  AZTEC_INCPATH=$AZTEC_DIR/lib
elif test "$parallel" = yes; then
  AZTEC_INCPATH=$abs_top_builddir/../txmodules/aztecmpi/include:$HOME/$UNIXFLAVOR/aztecmpi/include:$HOME/aztecmpi/include:$HOME/opt/aztecmpi/include:$HOME/$host/aztecmpi/include:/usr/local/aztecmpi/include:/usr/local/aztecmpi/lib:/opt/aztecmpi/include:/loc/aztecmpi/include:/local/aztecmpi/include:$abs_top_builddir/../txmodules/aztec/include:$HOME/$UNIXFLAVOR/aztec/lib:$HOME/aztec/lib:$HOME/opt/aztec/include:$HOME/$host/aztec/include:/usr/local/aztec/lib:/opt/aztec/include:/loc/aztec/lib:/local/aztec/lib
else
  AZTEC_INCPATH=$abs_top_builddir/../txmodules/aztec/include:$HOME/$UNIXFLAVOR/aztec/include:$HOME/aztec/include:$HOME/opt/aztec/include:$HOME/$host/aztec/include:/usr/local/aztec/include:/opt/aztec/include:/loc/aztec/include:/local/aztec/include:/usr/local/aztec/lib
fi
# echo AZTEC_INCPATH = $AZTEC_INCPATH

if test -n "$AZTEC_INCPATH"; then
  AC_PATH_PROGS(AZTEC_H, az_aztec.h, "", $AZTEC_INCPATH)
  if test -z "$AZTEC_H"; then
    AC_MSG_WARN(az_aztec.h not found in $AZTEC_INCPATH.  Set with --with-aztec-incdir=)
    AZTEC_INC=" "
    ac_cv_have_aztec=no
  else
    AZTEC_INCDIR=`dirname $AZTEC_H`
    AC_SUBST(AZTEC_INCDIR)
dnl JRC: Put in trilinos vars also to allow switching in vorpal
    TRILINOS_INCDIR=$AZTEC_INCDIR
    AC_SUBST(TRILINOS_INCDIR)
    AZTEC_INC="$ML_INC -I$AZTEC_INCDIR"
    AC_SUBST(AZTEC_INC)
    TRILINOS_INC=$AZTEC_INC
    AC_SUBST(TRILINOS_INC)
    AZTEC_DIR=`dirname $AZTEC_INCDIR`
    ac_cv_have_aztec=yes
  fi
else
  ac_cv_have_aztec=no
fi

dnl ######################################################################
dnl
dnl Find aztec libraries
dnl
dnl ######################################################################

AC_ARG_WITH(aztec-libdir,[  --with-aztec-libdir=<location of aztec library> ],
AZTEC_LIBDIR="$withval",AZTEC_LIBDIR="")

if test $ac_cv_have_aztec = yes; then
  if test -n "$AZTEC_LIBDIR"; then
    AZTEC_LIBPATH=$AZTEC_LIBDIR
  else
    AZTEC_LIBPATH=$AZTEC_DIR/lib
  fi
  AC_PATH_PROGS(LIBAZTEC_A, libaztec.a, "", $AZTEC_LIBPATH)
  if test -z "$LIBAZTEC_A"; then
    AC_MSG_WARN(libaztec.a not found.  Set with --with-aztec-libdir=)
    ac_cv_have_aztec=no
    AZTEC_LIBS=" "
  else
    AZTEC_LIBDIR=`dirname $LIBAZTEC_A`
    AC_SUBST(AZTEC_LIBDIR)
    TRILINOS_LIBDIR=$AZTEC_LIBDIR
    AC_SUBST(TRILINOS_LIBDIR)

dnl ######################################################################
dnl
dnl Additional libraries used to link AZTEC. Modify if required!!
dnl
dnl ######################################################################

dnl JRC; Add in compatibility libs.  Must have run fcinteropt.m4.
    AZTEC_LIBS="$ML_LIB -L$AZTEC_LIBDIR -laztec $LAPACK_LIBS $FC_LIBS"

dnl    AC_MSG_CHECKING(full aztec library set)
dnl
dnl    case "$host" in
dnl
dnl      *-linux* | *-freebsd* )
dnl
dnl It seems libg2c$SO/libgfortran$SO is always in the same directory as
dnl libstdc++$SO. JAC 02/09/06
dnl        g2clibfilename=`$CXX -print-file-name=libstdc++$SO`
dnl        g2clibdir=`dirname $g2clibfilename`
dnl
dnl ver should get set in flags.m4
dnl        case "$ver" in
dnl          4.*)
dnl            AZTEC_LIBS="$ML_LIB -L$AZTEC_LIBDIR -laztec -L$g2clibdir -lgfortran"
dnl            ;;
dnl          *)
dnl            AZTEC_LIBS="$ML_LIB -L$AZTEC_LIBDIR -laztec -L$g2clibdir -lg2c"
dnl            ;;
dnl        esac
dnl        ;;
dnl      s00* | powerpc-ibm-aix4.* | powerpc-ibm-aix5.* )
dnl        AZTEC_LIBS="$ML_LIB -L$AZTEC_LIBDIR -laztec -lblas -lxlf90"
dnl        ;;
dnl      *)
dnl        AZTEC_LIBS="$ML_LIB -L$AZTEC_LIBDIR -laztec"
dnl        ;;
dnl    esac
dnl    AC_MSG_RESULT($AZTEC_LIBS)

  fi

fi
AC_SUBST(AZTEC_LIBS)
TRILINOS_LIBS=$AZTEC_LIBS
AC_SUBST(TRILINOS_LIBS)

dnl ######################################################################
dnl
dnl Define for whether aztec found
dnl
dnl ######################################################################

if test $ac_cv_have_aztec = yes; then
  AC_DEFINE(HAVE_AZTEC, , Define if have the Aztec solver library)
fi

dnl ######################################################################
dnl
dnl    Write to summary file if defined
dnl
dnl ######################################################################
if test -n "$config_summary_file"; then
   if test $ac_cv_have_aztec = yes; then
      echo "Using Aztec with"                     >> $config_summary_file
      echo "  AZTEC_INC:    $AZTEC_INC"           >> $config_summary_file
      echo "  AZTEC_LIBDIR: $AZTEC_LIBDIR"        >> $config_summary_file
      echo "  AZTEC_LIBS:   $AZTEC_LIBS"          >> $config_summary_file
      echo "  TRILINOS_INC:    $TRILINOS_INC"           >> $config_summary_file
      echo "  TRILINOS_LIBDIR: $TRILINOS_LIBDIR"        >> $config_summary_file
      echo "  TRILINOS_LIBS:   $TRILINOS_LIBS"          >> $config_summary_file
      echo                                        >> $config_summary_file
   else
      echo "NOT using Aztec"                      >> $config_summary_file
      echo                                        >> $config_summary_file
   fi
fi

