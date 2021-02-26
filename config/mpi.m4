dnl###################################################################
dnl
dnl File:       mpi.m4
dnl
dnl Purpose: 	Determine mpiluancher and additional options based on
dnl		platform.
dnl
dnl Version:    $Id: mpi.m4 2374 2007-08-21 21:13:17Z dws $
dnl
dnl###################################################################

# What launches mpi programs?

AC_ARG_WITH(mpi-launcher,
    AC_HELP_STRING([--with-mpi-launcher=<mpilauncher>],
        [to specify the binary which launches mpi programs]),
    MPILAUNCHER="$withval")

host=`hostname`

if test -z "$MPILAUNCHER"; then
  if which mpiexec 2>/dev/null; then
    MPILAUNCHER=mpiexec
  else
    MPILAUNCHER=mpirun
    MPIOPTS="-nolocal"
    case $host in
	facets.txcorp.com) MPIOPTS=;;
    esac
  fi


# Exceptions to the rule
  case $host in
    node1?.cipslab.colorado.edu | node*.cl.txcorp.com) MPILAUNCHER=mpirun;;
  esac

##########
#
# Deal with idiosyncrasies of systems for parallel runs
#
##########

  SYSNAME=`uname -s`
  case $SYSNAME in
    Darwin) MPILAUNCHER=mpiexec;;
     Linux) case $host in       # mpi launcher depends on the machine
	      jaccn*|jacin*) MPILAUNCHER=mpirun
	    esac;;
  esac
fi

if test -n "$PBS_NODEFILE"; then
    MPIOPTS="$MPIOPTS -machinefile $PBS_NODEFILE"
elif test -f nodes; then
    MPIOPTS="$MPIOPTS -machinefile `pwd`/nodes"
fi

AC_SUBST(MPILAUNCHER)
AC_SUBST(MPIOPTS)

