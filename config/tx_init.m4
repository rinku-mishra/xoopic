dnl###################################################################
dnl
dnl File:	tx_init.m4
dnl
dnl Purpose: Do standard initialization of autocnf projects including
dnl  the collection of data for keeping track of configuration metadata
dnl
dnl Version:	$Id: txtest.m4 203 2007-08-25 14:14:47Z dws $
dnl
dnl###################################################################

dnl ######################################################################
dnl Set sourcedir and builddir for keeping track of the separation
dnl ######################################################################
abs_top_builddir=`pwd`
AC_SUBST(abs_top_builddir)
abs_top_srcdir=`dirname $0`
cd $abs_top_srcdir
abs_top_srcdir=`pwd`
cd $abs_top_builddir
AC_PREFIX_DEFAULT($abs_top_builddir)

dnl ######################################################################
dnl
dnl Get host
dnl
dnl ######################################################################

echo Getting canonical host
AC_CANONICAL_HOST
hosttype=$host
echo hosttype = $hosttype
AC_DEFINE_UNQUOTED(HOSTTYPE, "$host", [The host type])
echo HOSTTYPE = $HOSTTYPE
hnm=`hostname`
AC_DEFINE_UNQUOTED(HOSTNAME, "$hnm", [The host name])
echo HOSTNAME defined.

dnl ######################################################################
dnl
dnl Start configuration summary
dnl
dnl ######################################################################

builtin(include, config/tx_ac_summary.m4)
TX_AC_SUMMARY_INIT(config.summary, $0)

dnl ######################################################################
dnl
dnl Use wrapped automake
dnl
dnl ######################################################################

dnl # See whether we have automake
amver=`automake --version 2>/dev/null`
if test $? != 0; then
  echo automake not present in your path.
  echo Modifications to Makefile.am\'s will not propagate.
  AUTOMAKE=$abs_top_srcdir/config/automake.sh
fi

dnl ######################################################################
dnl
dnl Get version information
dnl
dnl ######################################################################

builtin(include, config/tx_svn_info.m4)
TX_SVN_INFO([PROJTESTS_URL], [PROJTESTS_REV], .)
TX_SVN_INFO([TXTEST_URL], [TXTEST_REV], txtest)
TX_SVN_INFO([CONFIG_URL], [CONFIG_REV], config)
echo  >> $config_summary_file

