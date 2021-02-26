dnl ######################################################################
dnl
dnl Find txqplot
dnl
dnl ######################################################################

dnl Check for the distribution
AC_ARG_WITH(txqplot,[  --with-txqplot=<txqdir>       set the location of the TxqPlot distribution],
        TXQ_DIR="$withval")

AC_CHECKING(TxqPlot installation)
if test -n "$TXQ_DIR"; then
   TXQ_INC_PATH_CHECK="$TXQ_DIR/include:$TXQ_DIR/txqplot/include"
   TXQ_LIB_PATH_CHECK="$TXQ_DIR/lib:$TXQ_DIR/lib/$COMPDIR:$TXQ_DIR/txqplot/lib:$TXQ_DIR/txqplot/lib/$COMPDIR"
fi


dnl The txqplot includes

if test -z "$TXQ_INC_PATH_CHECK"; then
  TXQPLOT_INCPATH=$HOME/$host/txqplot/include:/usr/local/txqplot/include:/loc/txqplot/include:$HOME/txqplot/include
else
  TXQPLOT_INCPATH=$TXQ_INC_PATH_CHECK
fi

AC_ARG_WITH(txqplot-incdir,[  --with-txqplot-incdir=<location of txqplot includes> ],TXQPLOT_INCDIR="$withval",TXQPLOT_INCDIR="")

if test -n "$TXQPLOT_INCDIR"; then
  TXQPLOT_INCPATH=$TXQPLOT_INCDIR
fi
AC_PATH_PROGS(TXQPLOT_FRAME, TxqFrame.h, "", $TXQPLOT_INCPATH)
if test -z "$TXQPLOT_FRAME"; then
  AC_MSG_ERROR(TxqFrame.h not found in $TXQPLOT_INCPATH.  Set with --with-txqplot-incdir=)
fi
TXQPLOT_INCDIR=`dirname $TXQPLOT_FRAME`
AC_SUBST(TXQPLOT_INCDIR)
#echo "TXQPLOT_INCDIR = $TXQPLOT_INCDIR"

dnl The txqplot libraries
dnl COMPDIR must have already been defined (by flags.m4)

# echo TXQ_LIB_PATH_CHECK = \'$TXQ_LIB_PATH_CHECK\'
if test -z "$TXQ_LIB_PATH_CHECK"; then
  libpath=`dirname $TXQPLOT_INCDIR`/lib
  TXQPLOT_LIBPATH=$libpath/$COMPDIR:$HOME/$host/txqplot/lib/$COMPDIR:/usr/local/txqplot/lib/$COMPDIR:$HOME/txqplot/lib
else
  TXQPLOT_LIBPATH=$TXQ_LIB_PATH_CHECK
fi

AC_ARG_WITH(txqplot-libdir,[  --with-txqplot-libdir=<location of txqplot libs>   ],
TXQPLOT_LIBDIR="$withval",TXQPLOT_LIBDIR="")
if test -n "$TXQPLOT_LIBDIR"; then
  TXQPLOT_LIBPATH=$TXQPLOT_LIBDIR
fi
AC_PATH_PROGS(TXQPLOT_BASE, libtxqbase.a, "", $TXQPLOT_LIBPATH)
if test -z "$TXQPLOT_BASE"; then
  AC_MSG_ERROR(libtxqbase.a not found.  Set with --with-txqplot-libdir=)
fi
TXQPLOT_LIBDIR=`dirname $TXQPLOT_BASE`
AC_SUBST(TXQPLOT_LIBDIR)
#echo "TXQPLOT_LIBDIR = $TXQPLOT_LIBDIR"

