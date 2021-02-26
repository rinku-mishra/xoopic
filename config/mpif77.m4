dnl ######################################################################
dnl
dnl File:       fortran_mpi.m4
dnl Depends on: f77-90.m4 and fortflags.m4
dnl
dnl Purpose:    Set the right mpi include variables
dnl
dnl Version:    $Id: mpif77.m4 2334 2007-07-18 02:30:16Z kuehn $
dnl
dnl Copyright Tech-X Corporation, 2001-2005.  Redistribution allowed
dnl provided this copyright statement remains intact.
dnl
dnl ######################################################################

dnl ######################################################################
dnl
dnl Set the variables:  
dnl   MPI_INCLUDES
dnl   MPILDFLAGS
dnl   MPI_LIBS
dnl   FC_MPI_LINK
dnl
dnl     Notes:
dnl        o mpi_link allows linking of dummy routines for building serial 
dnl        o mpi_link allows linking of dummy routines for building serial 
dnl          verson of codes without building mpi (see mpi_serial.f90)
dnl        o Some people want distinction between f90 and mpif90.  We don't.
dnl          If you want FC=MPIF90, put it in the configure.in
dnl
dnl  SEK - I'm trying to maintain backward compatibility with Jim
dnl    Amundsen but this is a pain b/c Tech-X does things differently
dnl ######################################################################

dnl ----------------------------------------------------------------------
dnl Set defaults for convenience.  Perhaps irrelevent since I check
dnl several places
dnl ----------------------------------------------------------------------
case "$host" in
 *-linux-gnu)
  	DEFAULT_MPI_PREFIX=/usr/local
	DEFAULT_MPI_INCLUDE_DIR=$DEFAULT_MPI_PREFIX/include

      if test -f "$DEFAULT_MPI_INCLUDE_DIR/mpif.h"; then
          lam_count=`grep -c LAM "$DEFAULT_MPI_INCLUDE_DIR/mpif.h"`
          if test "$lam_count" -gt "0" ; then
                # Assume we have LAM
                DEFAULT_MPI_LIBS="-llammpi++ -llammpio -lpmpi -llamf77mpi -lmpi -llam -lnsl -lutil"
          else
                # Assume we have MPICH
                DEFAULT_MPI_LIBS="-lfmpich -lmpichf90 -lmpich"
          fi
    	    DEFAULT_MPILDFLAGS=-L$MPI_PREFIX/lib
	else 
  	   DEFAULT_MPI_PREFIX=""
      fi
	;;

 *apple-darwin*)
	 DEFAULT_MPI_PREFIX=/usr/local
	 DEFAULT_MPI_LIBS="-lfmpich -lmpichf90 -lmpich"
	 DEFAULT_MPI_INCLUDE_DIR=$MPI_PREFIX/include
	 DEFAULT_MPILDFLAGS=-L$MPI_PREFIX/lib
	 ;;

 *-*-aix*)
      DEFAULT_MPI_PREFIX=
      DEFAULT_MPI_INCLUDE_DIR=
      DEFAULT_MPILDFLAGS="-mpe -L/usr/lpp/ppe.poe/lib -L/usr/lpp/ppe.poe/lib/ip"
      DEFAULT_MPI_LIBS="-lmpi -lvtd -lppe"
	;;
 *)
      DEFAULT_MPI_PREFIX=
      DEFAULT_MPI_INCLUDE_DIR=
      DEFAULT_MPILDFLAGS=
      DEFAULT_MPI_LIBS=
	;;


esac

dnl ----------------------------------------------------------------------
dnl Flags
dnl ----------------------------------------------------------------------
AC_ARG_ENABLE(MPI,
     [  --disable-MPI    Disable MPI],
     [ case "${enableval}" in
       yes) MPI=true ;;
       no)  MPI=false ;;
       *) AC_MSG_ERROR(bad value ${enableval} for --enable-MPI) ;;
     esac],[MPI=false])

AM_CONDITIONAL(CONDMPI,test x$MPI = xtrue)

AC_ARG_WITH(mpi-prefix,
	[  --with-mpi-prefix=<dir>	Prefix directory for MPI.],
	[MPI_PREFIX="${withval}"],
	[MPI_PREFIX="$DEFAULT_MPI_PREFIX"]
)

AC_ARG_WITH(mpi-include-dir,
	[  --with-mpi-include-dir=<dir>	MPI include directory.
	Default is -I\$MPI_PREFIX/include],
	[MPI_INCLUDE_DIR="${withval}"],
	[MPI_INCLUDE_DIR="$DEFAULT_MPI_INCLUDE_DIR"]
)

AC_ARG_WITH(mpi-ldflags,
	[  --with-mpi-ldflags=<flags>	LDFLAGS for linking with MPI.],
	[MPILDFLAGS="${withval}"],
	[MPILDFLAGS="$DEFAULT_MPILDFLAGS"]
)
AC_ARG_WITH(mpi-libs,
	[  --with-mpi-libs=<libs>	Libraries for linking with MPI.],
	[MPI_LIBS="${withval}"],
	[MPI_LIBS="$DEFAULT_MPI_LIBS"]
)

AC_MSG_RESULT($MPI)

dnl ----------------------------------------------------------------------
dnl Set serial
dnl ----------------------------------------------------------------------
if test "$MPI" = false; then
      MPI_INCLUDES=
      MPILDFLAGS=
      MPI_LIBS=
      FC_MPI_LINK=serial
dnl ----------------------------------------------------------------------
dnl Set parallel and perform the checks
dnl ----------------------------------------------------------------------
else
      FC_MPI_LINK=parallel

	dnl On AIX, MPI includes are taken care of by the mpxlf90 script.
	MPI_PATH="$MPI_INCLUDE_DIR:$MPI_PREFIX/include:$DEFAULT_MPI_INCLUDE_DIR:/usr/local/mpich/include"
	
      AC_PATH_PROGS(MPIF_INCLUDE, mpif.h, "", $MPI_PATH)
      if test -z "$MPIF_INCLUDE"; then
		echo "MPI_INCLUDE_DIR empty. Assuming mpif.h will be found automatically."
		MPI_INCLUDES=
	else
            MPI_INCLUDE_DIR=`dirname $MPIF_INCLUDE`
		MPI_PREFIX=`dirname $MPI_INCLUDE_DIR`
		dnl note that not every compiler uses -I
		MPI_INCLUDES=$FC_INCLUDE_FLAG$MPI_INCLUDE_DIR
	fi

	dnl Set parallel compiler
      case "$host" in
	xt3-*)
		MPIF77=ftn
		;;
      	*-pc-linux-gnu)
	            if test -z "$MPILDFLAGS"; then
                        MPILDFLAGS=-L$MPI_PREFIX/lib
                  fi
	            if test -z "$MPI_LIBS"; then
            		lam_count=`grep -c LAM "$MPI_INCLUDE_DIR/mpif.h"`
            		if test "$lam_count" -gt "0" ; then
            			MPI_LIBS="-llammpi++ -llammpio -lpmpi -llamf77mpi -lmpi -llam -lnsl -lutil"
            		else
	                        MPI_LIBS="-lfmpich -lmpichf90 -lmpich"
            		fi
                  fi
                  MPIF77=mpif77 
      		;;
      	*-*-aix*)
	            if test -z "$MPILDFLAGS"; then
                        MPILDFLAGS=$DEFAULT_MPILDFLAGS
                  fi
	            if test -z "$MPI_LIBS"; then
            		MPI_LIBS=$DEFAULT_MPI_LIBS
                  fi
                  MPIF77=mpxlf77
      		;;
	     *)
	            if test -z "$MPILDFLAGS"; then
                        MPILDFLAGS=$DEFAULT_MPILDFLAGS
                  fi
	            if test -z "$MPI_LIBS"; then
            		MPI_LIBS=$DEFAULT_MPI_LIBS
                  fi
                  MPIF77=mpif77 
                  ;;
	esac
fi

dnl ----------------------------------------------------------------------
dnl Export
dnl ----------------------------------------------------------------------
AC_SUBST(MPIF77)
AC_SUBST(MPI_INCLUDES)
AC_SUBST(MPILDFLAGS)
AC_SUBST(MPI_LIBS)
AC_SUBST(FC_MPI_LINK)
