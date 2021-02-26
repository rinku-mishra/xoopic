#!/bin/tcsh

if ("$1" == "") then
  # not all params are given; exit
  exit 1
endif 

setenv ALLPARAMS "$argv:q"
setenv LOGFILE "xoopic_run.log"

while ("$1" != "" )
  if ("$1" == "-i") then
    if ("$2" != "") then
      # we have a parameters for input file
      setenv LOGFILE `basename $2`
    endif
  endif

  shift	# to next parameters
end

setenv RUNDIR `dirname $0`

echo "===== Start time: `date` =====" >&! $LOGFILE.log
$RUNDIR/xg/xoopic -nox $ALLPARAMS  >> $LOGFILE.log
echo "===== End time: `date` =====" >> $LOGFILE.log

