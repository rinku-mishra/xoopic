dnl ######################################################################
dnl
dnl Get host tag for binary distribution
dnl
dnl ######################################################################

un=`uname`
osr=`uname -r`
case "$un" in
  Linux)
    # echo "Working on Linux"
    # os_tag=`echo $osr | sed 's/-.*$//'`
    os_tag=`echo $osr | sed 's/-.*$//' | sed 's/\.[0-9]*$//'`
    ;;
  *)
    AC_MSG_WARN(tag abbreviation not know for system $un)
    os_tag=$osr
    ;;
esac
os_tag=${un}-$os_tag
AC_SUBST(os_tag)


