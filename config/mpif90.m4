dnl ######################################################################
dnl
dnl File:       fortran_mpi.m4
dnl Depends on: f77-90.m4 and fortflags.m4
dnl
dnl Purpose:    Set the right mpi include variables
dnl
dnl Version:    $Id: mpif90.m4 2334 2007-07-18 02:30:16Z kuehn $
dnl
dnl ######################################################################
dnl ----------------------------------------------------------------------
dnl
dnl Set the variables:  
dnl   MPIF90
dnl   MPI_INC
dnl   MPI_LIBS
dnl   FC_MPI_LINK
dnl
dnl     Notes:
dnl        o FC is the autotools variable name for any fortran beyond F77.
dnl          Since MPIFC isn't defined by autotools, use MPIF90 instead
dnl        o mpi_link allows linking of dummy routines for building serial 
dnl          verson of codes without building mpi (see mpi_serial.f90)
dnl        o If you want FC=MPIF90, put it in the configure.ac
dnl ----------------------------------------------------------------------

AC_MSG_CHECKING(for mpi usage)

if test -n "$MPIF90";      then SAVE_MPIF90="$MPIF90"; fi
if test -n "$MPIF90_LIBS"; then SAVE_MPIF90_LIBS="$MPIF90_LIBS"; fi
if test -n "$MPIF90_INC";  then SAVE_MPIF90_INC="$MPIF90_INC"; fi

dnl ----------------------------------------------------------------------
dnl Flags
dnl ----------------------------------------------------------------------
AC_ARG_ENABLE(parallel, [  --enable-parallel    Enable mpi],
     [ case "${enableval}" in
       yes) use_mpi=true ;;
       no)  use_mpi=false ;;
       *) AC_MSG_ERROR(bad value ${enableval} for --enable-MPI) ;;
     esac],[use_mpi=true])

if test -n "$SAVE_MPIF90"; then use_mpi="true"; fi

dnl ----------------------------------------------------------------------
dnl Find the compilers.   For LIBS: Linux allows for both lam or mpich
dnl ----------------------------------------------------------------------
if test "$use_mpi" = "true"; then
	FC_MPI_LINK=parallel
	case "$host" in
        xt3-*)
                MPIF77=ftn
		MPI_DIR=""
		MPI_INCLUDE_DIR=""
		MPI_LIBDIR=""
		MPI_LIBS=""
		AC_PATH_PROGS(MPIF_INCLUDE, mpif.h, "", $MPI_INCLUDE_DIR)
		MPI_INC=""	
                ;;
	 x86*-linux* | i386-*-linux* | i686-*linux*)
            MPI_PATH=$PATH:/usr/local/mpich:/opt/mpich:$HOME/lib/mpich:/opt2/mpich
		MPI_PATH=$MPI_DIR/bin:$PATH
		AC_PATH_PROGS(MPIF90, mpif90, "", $MPI_PATH)
		if test -n "$MPIF90"; then
			fbindir=`dirname $MPIF90`
			MPI_DIR=`dirname $fbindir`
			MPI_INCLUDE_DIR=$MPI_DIR/include
			MPI_LIBDIR="$MPI_DIR/lib"
			dnl Default is mpich.  LAM detected down below
			MPI_LIBS="-L$MPI_LIBDIR -lfmpich -lmpichf90 -lmpich"
			dnl Now make sure the include file is found are found.
			AC_PATH_PROGS(MPIF_INCLUDE, mpif.h, "", $MPI_INCLUDE_DIR)
			if test -z "$MPIF_INCLUDE"; then
			   echo "mpif.h not found.  Assume it will be included automatically"
			   MPI_INC=""
			else
			   MPI_INCLUDE_DIR=`dirname $MPIF_INCLUDE`
			   MPI_INC=-I$MPI_INCLUDE_DIR
			   lam_count=`grep -c LAM "$MPI_INCLUDE_DIR/mpif.h"`
			   if test "$lam_count" -gt 0 ; then
			     MPI_LIBS="-L$MPI_LIBDIR -llammpi++ -llammpio -lpmpi -llamf77mpi -lmpi -llam -lnsl -lutil"
			   fi
			fi
		else
		      use_mpi=false
		fi
		;;
	*apple-darwin*)
		 MPI_PATH=$PATH:/usr/local/mpich:/opt/mpich:$HOME/lib/mpich:/opt2/mpich
		  MPI_PATH=$MPI_DIR/bin:$PATH
		AC_PATH_PROGS(MPIF90, mpif90, "", $MPI_PATH)
		if test -n "$MPIF90"; then
  	      		fbindir=`dirname $MPIF90`
	  	      	MPI_DIR=`dirname $fbindir`
		      	MPI_INCLUDE_DIR=$MPI_DIR/include
			MPI_LIBDIR="$MPI_DIR/lib"
			MPI_INC=-I$MPI_INCLUDE_DIR
		  case "$MPI_DIR" in
			*openmpi*)			
				MPI_LIBS="-L$MPI_LIBDIR -lmpi -lmpi_f90"
			;;
		  	*mpich*)
				MPI_LIBS="-L$MPI_LIBDIR -lfmpich -lmpichf90 -lmpich"
			;;
		  esac
		else
		      use_mpi=false
		fi
	
	;;
	 *-*-aix*)
		MPI_PATH=$PATH:$MPI_DIR/bin
		AC_PATH_PROGS(MPIF90, mpxlf90, "", $MPI_PATH)
      	if test -n "$MPIF90"; then
		    fbindir=`dirname $MPIF90`
		    newdir=`dirname $fbindir`
	  	    MPI_DIR=$newdir
		    MPI_LIBDIR="$MPI_DIR/lib"
		    MPI_INCLUDE_DIR=$MPI_DIR/include
		    MPI_LIBS="-L$MPI_LIBDIR -lmpi -lvtd -lppe"
		else
		    use_mpi=false
		fi
		;;
	 *)   use_mpi=false
		MPIF90=""
		MPI_LIBS="" 
		;;
	esac
fi
dnl ----------------------------------------------------------------------
dnl Export
dnl ----------------------------------------------------------------------
if test -n "$SAVE_MPIF90"; then MPIF90="$SAVE_MPIF90"; fi
# if test -n "$SAVE_MPIF90"; then MPIF90="$SAVE_MPIF90"; fi
if test -n "$SAVE_MPIF90_INC"; then MPI_INC="$SAVE_MPIF90_INC"; fi
if test -n "$SAVE_MPIF90_LIBS"; then MPI_LIBS="$SAVE_MPIF90_LIBS"; fi

AM_CONDITIONAL(USE_MPI, test x$use_mpi = xtrue)
if test "$use_mpi" = false; then
      MPIF90=
      MPI_INC=
      MPILDFLAGS=
      MPI_LIBS=
      FC_MPI_LINK=serial
fi
AC_SUBST(MPIF90)
AC_SUBST(MPI_INC)
AC_SUBST(MPI_LIBS)
AC_SUBST(FC_MPI_LINK)
dnl ----------------------------------------------------------------------
dnl
dnl    Write to summary file if defined
dnl
dnl ----------------------------------------------------------------------
if test -n "$config_summary_file"; then
	if test "$use_mpi" = "true"; then
	  echo " MPIF90 =$MPIF90 "                    >> $config_summary_file
	  echo " MPI_DIR=$MPI_DIR"                    >> $config_summary_file
	  echo " MPI_LIBS=$MPI_LIBS"                  >> $config_summary_file
	  echo " MPI_INC=$MPI_INC"                    >> $config_summary_file
	else
	  echo " Do not use MPI (default)"            >> $config_summary_file
	fi
      echo                                          >> $config_summary_file
fi
