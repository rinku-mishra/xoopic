# ######################################################################
#
# File:	svninfo.m4
#
# Purpose:	To get svn URL and revision number for a project.
#
# Version:	$Id: tx_svn_info.m4 2363 2007-08-08 22:00:40Z dws $
#
# Copyright Tech-X Corporation, 2007.  Redistribution allowed provided
# this copyright statement remains intact.
#
# ######################################################################

dnl ######################################################################
dnl
dnl Function:   TX_SVN_INFO(url-variable, revision-variable, directory)
dnl
dnl Purpose:    To get the URL and REVISION variables for svn
dnl     Calls AC_SUBST for those variables.
dnl
dnl ######################################################################

AC_DEFUN([TX_SVN_INFO], [

  AC_MSG_CHECKING([for svn information for the subdir [$3] ])
  if test -z "$abs_top_srcdir"; then
    echo
    AC_MSG_ERROR(\$abs_top_srcdir not defined!)
  fi
  if test -d "$abs_top_srcdir/$3"; then
    svndir=`(cd $abs_top_srcdir/$3; pwd -P)`
  else
    AC_MSG_ERROR($abs_top_srcdir/$3 not found!)
  fi
  svninfofile=$svndir/svninfo.txt

# Refresh svninfo.txt if a checkout

  if test -d $svndir/.svn; then
    (cd $svndir; svn info > $svninfofile 2>/dev/null)
  fi
  if test ! -f $svninfofile; then
    AC_MSG_ERROR($svninfofile not found!)
  fi

  TX_PROJ_URL=`grep ^URL: $svninfofile | sed 's/^.* //'`
  $1=$TX_PROJ_URL
  TX_PROJ_REV=`grep ' Rev:' $svninfofile | sed 's/^.*Rev: //'`
  $2=$TX_PROJ_REV
  AC_SUBST([$1])
  AC_DEFINE_UNQUOTED([$1], "$TX_PROJ_URL", "SVN Project URL for the subdir $3")
  AC_SUBST([$2])
  AC_DEFINE_UNQUOTED([$2], "$TX_PROJ_REV", "SVN Project REV for the subdir $3")
  AC_MSG_RESULT([$TX_PROJ_URL, Revision $TX_PROJ_REV])

  if test -n "$config_summary_file"; then
    echo >>$config_summary_file
    echo $3 from svn with >>$config_summary_file
    eval printf  \""  "$1 = %s\\n\" \"$TX_PROJ_URL\" >>$config_summary_file
    eval printf  \""  "$2 = %s\\n\" \"$TX_PROJ_REV\" >>$config_summary_file
  fi

])
