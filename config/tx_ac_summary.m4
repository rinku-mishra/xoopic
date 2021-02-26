# ######################################################################
#
# File:		tx_ac_summary.m4
#
# Purpose:	To initialize the configuaration summary file
#
# Version:	$Id: tx_ac_summary.m4 2304 2007-06-24 11:27:10Z cary $
#
# Copyright Tech-X Corporation, 2007.  Redistribution allowed provided
# this copyright statement remains intact.
#
# ######################################################################

dnl ######################################################################
dnl
dnl Function:   TX_AC_SUMMARY_INIT(summary-file-name, calling-program)
dnl
dnl Purpose:    Initialize the configuration summary.
dnl
dnl ######################################################################

AC_DEFUN([TX_AC_SUMMARY_INIT], [

  config_summary_file=$1
  echo Configuration summary file is $config_summary_file
  top_config_summary_file=$abs_top_builddir/$config_summary_file
  AC_SUBST(top_config_summary_file)
  echo ""                                           >  $config_summary_file
  echo "Config line:"                               >> $config_summary_file
  echo $2 $ac_configure_args                        >> $config_summary_file
  echo ""                                           >> $config_summary_file
  echo "Directory: $PWD"                            >> $config_summary_file
  if test -z "$host"; then
    AC_MSG_WARN([You must define host before setting up config_summary_file])
  else
    echo "Host: $host"                                >> $config_summary_file
  fi

])

dnl ######################################################################
dnl
dnl Function:   TX_AC_SUMMARY_FINALIZE
dnl
dnl Purpose:    Finalize the configuration summary.
dnl
dnl ######################################################################

AC_DEFUN([TX_AC_SUMMARY_FINALIZE], [

  if test -n "$config_summary_file"; then
    echo
    echo "-----------------------------------"
    echo "Configure summary (from file $config_summary_file):"
    cat $config_summary_file
    echo "-----------------------------------"
    echo
  else
    echo config_summary_file not defined.
  fi

])

