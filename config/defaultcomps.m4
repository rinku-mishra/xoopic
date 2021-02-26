# Determine the default compilers

CXXTMP=`echo $CXX | sed 's/ .*$//'`
CXXBASE=`basename $CXXTMP`
CXXDIR=`dirname $CXXTMP`
case $CXXBASE in

  g++ | c++)
    case $CXXDIR in
      . | "")
        CC=gcc
        F77C=g77
        ;;
      *)
        CC=${CXXDIR}/gcc
        F77C=${CXXDIR}/g77
        ;;
    esac
    ;;

  mpiCC)
    CC=$CXXDIR/mpicc
    F77C=$CXXDIR/mpif77
    MPI_CXXFLAGS="-DMPI_PYTHON"
    ;;

  KCC)
    case `uname` in
      Linux)
        CC=gcc
        F77C=g77
        ;;
      *)
        CC=cc
        F77C=f77
        ;;
    esac
    ;;

  aCC)
    CC=cc
    F77C=f77
    ;;

  CC)
    CC=cc
    F77C=ftn
    ;;

  *)
    CC=cc
    F77C=f77
    ;;

esac

