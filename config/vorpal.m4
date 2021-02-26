dnl ######################################################################
dnl
dnl File:	vorpal.m4
dnl
dnl Purpose:	Check for VORPAL defines, including VDIM and
dnl		MAX_PARTICLE_GROUP_SIZE
dnl
dnl Version: $Id: vorpal.m4 2387 2007-09-06 10:51:56Z cary $
dnl
dnl Tech-X configure system
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Check for VDIM=9
dnl
dnl ######################################################################

AC_ARG_WITH(vdim,[  --with-vdim=<value of VDIM>     set max # of non spatial particle variables], VDIM="$withval",VDIM="")
AC_MSG_CHECKING(values of VDIM)

# Extract the vdim values
# echo VDIM = \'$VDIM\'
if test -n "$VDIM"; then
  vdimvals=`echo $VDIM | sed 's/,/ /g'`
else
  vdimvals="3 4 5 9"
fi
# echo vdimvals = $vdimvals

vdimmax=3
for i in $vdimvals; do
  if test $i -gt $vdimmax; then vdimmax=$i; fi
  case $i in
    1)
      AC_DEFINE([VDIM1], 1, [Define to have VDIM=1 particles])
      ;;
    2)
      AC_DEFINE([VDIM2], 1, [Define to have VDIM=2 particles])
      ;;
    3)
      AC_DEFINE([VDIM3], 1, [Define to have VDIM=3 particles])
      ;;
    4)
      AC_DEFINE([VDIM4], 1, [Define to have VDIM=4 particles])
      ;;
    5)
      AC_DEFINE([VDIM5], 1, [Define to have VDIM=5 particles])
      ;;
    9)
      AC_DEFINE(VDIM9, 1, [Define to have VDIM=9 particles])
      AC_DEFINE([HAVE_VAR_DT_PTCLS], 1,
	[Define to have variable time step particles])
      VPVARDT_LIBFLAG="-L../vpvardt -lvpvardt"
      ;;
    10)
      AC_DEFINE([VDIM10], 1, [Define to have VDIM=10 particles])
      ;;
    *)
      AC_MSG_ERROR(VDIM set to invalid value of $i. Currently must set to 1, 2, 3, 4, 5 or 9.)
      ;;
  esac
done
AC_MSG_RESULT($vdimvals)

AC_MSG_CHECKING(values of NDIM)
# Extract the ndim values
# echo NDIM = \'$NDIM\'
if test -n "$NDIM"; then
  ndimvals=`echo $NDIM | sed 's/,/ /g'`
else
  ndimvals="1 2 3"
fi

for i in $ndimvals; do
  case $i in
    1)
      AC_DEFINE([NDIM1], 1, [Define to have NDIM=1 simulations])
      ;;
    2)
      AC_DEFINE([NDIM2], 1, [Define to have NDIM=2 simulations])
      ;;
    3)
      AC_DEFINE([NDIM3], 1, [Define to have NDIM=3 simulations])
      ;;
    *)
      AC_MSG_ERROR(NDIM set to invalid value of $i. Currently must set to 1, 2, or 3.)
      ;;
  esac
done
AC_MSG_RESULT($ndimvals)

AC_SUBST(VPVARDT_LIBFLAG)
AC_DEFINE_UNQUOTED([VDIMMAX], ${vdimmax},
	[Define maximum allowed value of VDIM])
if test -n "$config_summary_file"; then
  echo >> $config_summary_file
  echo Building with ${vdimmax} maximum internal particle variables \
	>> $config_summary_file
fi

# Get the maximum particle group size
AC_MSG_CHECKING(maximum size of a particle group)
AC_ARG_WITH(max-ptcl-grp-size,
	AC_HELP_STRING([--with-max-ptcl-grp-size=<value of MAX_PARTICLE_GROUP_SIZE>],
        [set max size of a particle group]),
	MAX_PARTICLE_GROUP_SIZE=$withval,
	MAX_PARTICLE_GROUP_SIZE=1024)
AC_DEFINE_UNQUOTED([MAX_PARTICLE_GROUP_SIZE], $MAX_PARTICLE_GROUP_SIZE,
[Define the maximum size of a particle group])
AC_MSG_RESULT($MAX_PARTICLE_GROUP_SIZE)

# Set the compiler for vpdatanal
if test "$HDF5_INCDIR" = "$HDF5_INCDIR_SERIAL"; then
    VPDATANAL_CXX=$CXX
else
    VPDATANAL_CXX=$SERIALCXX
fi
AC_SUBST(VPDATANAL_CXX)

# Set maximum number of cells for some features
AC_MSG_CHECKING(maximum number of cells for some features)
AC_ARG_WITH(max-num-cells,
	AC_HELP_STRING([--with-max-num-cells=<max number of cells for some features>],
        [set max number of cells for some features]),
	MAX_NUM_CELLS=$withval,
	MAX_NUM_CELLS=-1)
AC_DEFINE_UNQUOTED([MAX_NUM_CELLS], $MAX_NUM_CELLS,
[Define the maximum number of cells for some features])
AC_MSG_RESULT($MAX_NUM_CELLS)

