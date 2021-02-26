dnl ######################################################################
dnl
dnl Find qscimpl
dnl
dnl ######################################################################

dnl Check for the distribution
AC_ARG_WITH(qscimpl-dir,[  --with-qscimpl-dir=<qscimpl-dir> set the location of the QScimpl distribution],
        TXQ_DIR="$withval")

AC_CHECKING(QScimpl installation)
if test -n "$TXQ_DIR"; then
   TXQ_INC_PATH_CHECK="$TXQ_DIR/include:$TXQ_DIR/qscimpl/include"
   TXQ_LIB_PATH_CHECK="$TXQ_DIR/lib:$TXQ_DIR/lib/$COMPDIR:$TXQ_DIR/qscimpl/lib:$TXQ_DIR/qscimpl/lib/$COMPDIR"
fi


dnl The qscimpl includes

if test -z "$TXQ_INC_PATH_CHECK"; then
  TXQPLOT_INCPATH=$HOME/$host/qscimpl/include:/usr/local/qscimpl/include:/loc/qscimpl/include:$HOME/qscimpl/include
else
  TXQPLOT_INCPATH=$TXQ_INC_PATH_CHECK
fi

AC_ARG_WITH(qscimpl-incdir,[  --with-qscimpl-incdir=<location of qscimpl includes> ],TXQPLOT_INCDIR="$withval",TXQPLOT_INCDIR="")

if test -n "$TXQPLOT_INCDIR"; then
  TXQPLOT_INCPATH=$TXQPLOT_INCDIR
fi
AC_PATH_PROGS(TXQPLOT_FRAME, TxqFrame.h, "", $TXQPLOT_INCPATH)
if test -z "$TXQPLOT_FRAME"; then
  AC_MSG_ERROR(TxqFrame.h not found in $TXQPLOT_INCPATH.  Set with --with-qscimpl-incdir=)
fi
TXQPLOT_INCDIR=`dirname $TXQPLOT_FRAME`
AC_SUBST(TXQPLOT_INCDIR)
TXQPLOT_INC=" "
if test -n "$TXQPLOT_FRAME"; then
  TXQPLOT_INC=-I$TXQPLOT_INCDIR
  AC_SUBST(TXQPLOT_INC)
fi

echo "TXQPLOT_INCDIR = $TXQPLOT_INCDIR"

dnl The qscimpl libraries
dnl COMPDIR must have already been defined (by flags.m4)

# echo TXQ_LIB_PATH_CHECK = \'$TXQ_LIB_PATH_CHECK\'
if test -z "$TXQ_LIB_PATH_CHECK"; then
  libpath=`dirname $TXQPLOT_INCDIR`/lib
  TXQPLOT_LIBPATH=$libpath/$COMPDIR:$HOME/$host/qscimpl/lib/$COMPDIR:/usr/local/qscimpl/lib/$COMPDIR:$HOME/qscimpl/lib
else
  TXQPLOT_LIBPATH=$TXQ_LIB_PATH_CHECK
fi

AC_ARG_WITH(qscimpl-libdir,[  --with-qscimpl-libdir=<location of qscimpl libs>   ],
TXQPLOT_LIBDIR="$withval",TXQPLOT_LIBDIR="")
if test -n "$TXQPLOT_LIBDIR"; then
  TXQPLOT_LIBPATH=$TXQPLOT_LIBDIR
fi
AC_PATH_PROGS(TXQPLOT_BASE, libtxqbase.a, "", $TXQPLOT_LIBPATH)
if test -z "$TXQPLOT_BASE"; then
  AC_MSG_ERROR(libtxqbase.a not found.  Set with --with-qscimpl-libdir=)
fi
TXQPLOT_LIBDIR=`dirname $TXQPLOT_BASE`
AC_SUBST(TXQPLOT_LIBDIR)
#echo "TXQPLOT_LIBDIR = $TXQPLOT_LIBDIR"

