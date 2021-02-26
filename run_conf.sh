#!/bin/bash
# this script will run configure with some standard
# edit this for the install dir:
prefix=/usr/local

# valid precision is either float or double:
precision=double

# running dir
run_dir=`pwd`
xoopic_parent_dir=`dirname $run_dir`


# base confopts:
# note that this file is duplicated in xgrafix and xoopic, so a couple of 
# values are overspecified - ignore warnings like 'urecognized options with-SCALAR'

confopts="--prefix=$prefix  --with-XGRAFIX-lib=${prefix}/lib --with-XGRAFIX-include=${prefix}/include"

# MPI :
    # confopts="$confopts --enable-MPI"

if [ `uname -s` == 'Darwin' ]; then
  # for Mac OS (condition added by JK, 2015-11-09)
  # extra="/sw" 
  extra="/opt/local" # for macports
  confopts="$confopts --enable-debug --with-tclconfig=$extra/lib --with-tkconfig=$extra/lib --with-dfftw-incdir=$extra/include --with-sfftw-incdir=$extra/include"

elif [ `lsb_release -a | grep 'Distributor' | awk '{ print $3 '}` == 'openSUSE' ]; then
  # disable compiling with FFTW on SUSE; they ise fftw3 instead of fftw
  confopts="$confopts --disable-fftw"

elif [ `lsb_release -d | awk '{ print $2 }'` == 'Ubuntu' ]; then
  # for Ubuntu 18.04 (condition added by JK, 2018-10-22)
  libdir="/usr/lib/x86_64-linux-gnu/"
  confopts="$confopts --with-xpm=${libdir}"

#else
  # for non-OS X system, add optimisation
#  confopts="$confopts --enable-fulloptimize"
fi

# I recommend getting dependencies for xoopic through macports (macports.org)
# I find thatxgrafix will build but will crash if you link against the system
# tcl/tk.  I'm not sure why, and this may be fixed at some point, but 
# currently my best solution has been using macports for tcl/tk and other deps.
# once you have macports, you can get the dependencies
# with the following:

# sudo port install imagemagick +no_x11 bison fftw automake libpng tcl tk
# add 'hdf5' to the end if you also want hdf5

# I also recommend getting gfortran / gcc 4.2 from AT&T:
# http://r.research.att.com/tools/
# 
# changed by JK, 2015-11-09, 2018-10-05

for i in /lib64 /usr/lib64 /usr/local/lib64 /usr/lib/x86_64-linux-gnu; do
	# check for some common library paths
	if [ -d $i ]; then
	    # make sure 64-bit library directory is checked for libraries
	    export PATH=$PATH:$i
	fi
done

confopts="$confopts $@"

echo "configuring with options: $confopts"
./configure $confopts && \
echo "

Configure successful!
 if you have a multicore machine, now you can build with:
 'make -j 4' (for 4 threaded compiling)
 and install with:
 'make install' or 'sudo make install', depending on whether you have write
  permissions to $prefix"
