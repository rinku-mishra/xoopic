dnl ######################################################################
dnl
dnl Check for Visual Numerics' pv-wave
dnl
dnl ######################################################################

dnl Get the executable

WAVE_PATH=$HOME/$host/vni/wave/bin:/usr/local/vni/wave/bin:$PATH

echo Looking for PV-WAVE
AC_PATH_PROGS(PVWAVE, wave, "", $WAVE_PATH)

dnl Get the location of the external header

bindir=`dirname $PVWAVE`
pvwavedir=`dirname $bindir`
PVWAVEEXT_DIRPATH=$pvwavedir/util/variables

AC_ARG_WITH(PVWAVEEXT_DIRPATH, 
[  --with-PVWAVE-external=<pvwave-external-dir>  set location of the external directory for PVWAVE],
PVWAVEEXT_DIRPATH="$withval")

AC_PATH_PROGS(PVWAVEEXT_PATH, wavevars.h, "", $PVWAVEEXT_DIRPATH)
if test -z "$PVWAVEEXT_PATH"; then
  AC_MSG_ERROR(PVWAVE includes not found in $PVWAVEEXT_DIRPATH.  Use --with-PVWAVE-external to set the location of wavevars.h.)
fi
PVWAVEEXT_DIR=`dirname $PVWAVEEXT_PATH`
AC_SUBST(PVWAVEEXT_DIR)

