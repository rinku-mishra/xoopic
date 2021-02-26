# XOOPIC (X11-based Object-Oriented Particle-In-Cell) 
XOOPIC is a two-dimensional 3-velocity particle-in-cell simulator. It can handle electrostatic and electromagnetic models, has a large variety of boundary conditions, supports multiple gasses and gas chemistry, and is easily reconfigurable via an input file.

This is a standard ./configure, make, make install system.
Start with the script 'run_conf.sh' - it is a nicer way to pass arguments to configure and it is the same for xoopic and xgrafix (you need to install xgrafix first)
once you have edited run_conf.sh to fit your needs, use it to configure xgrafix, then install it:
```console
$> sh run_conf.sh
$> make && sudo make install # note only sudo if you need root privileges to write the destination
```
then once that completes, use _the same_ run_conf.sh to configure xoopic:
```console
$> sh run_conf.sh
$> make && cp xg/xoopic /path/to/where/I/want/xoopic
```
That should do it!

# COMPLETE BUILD on new SYSTEM

To build XOOPIC on new system do the folowing:
```console
 - make distclean
 - aclocal ; autoconf ; automake
 - ./run_conf.sh
 - make 
```
The compiled XOOPIC file is in xg/xoopic

Optionally you could do:
```console
 - make install    # require root privileges to install to /usr/local/....
```
or
```console
 - cp xg/xoopic /path/to/where/I/want/xoopic
```

# Dependencies:
### building and running xgrafix/xoopic depend on the following headers/libraries/programs:
X11, Xpm, Tcl/Tk, gcc, imagemagick, bison
    
### optional xoopic dependencies:
Fortran
MPI for parallel runs
HDF5 for parallel or high performance dump files
libpng for png diagnostics
fftw
fftw3


### dependencies can be installed on Ubuntu (or likely any Debian based distro) with the command:
```console
sudo aptitude install gcc g++ gfortran build-essential automake \
    tk-dev imagemagick bison libx11-dev libxpm-dev libpng-dev \
    fftw-dev  libfftw3-dev h5utils hdf5-tools libhdf5-serial-*\
```
the first two lines are necessary, the last installs optional fftw/hdf5 dependencies

On Fedora, these dependencies can be installed with:
```console
sudo yum install gcc gcc-gfortran gcc-c++ automake libX11-devel libXpm-devel \
        ImageMagick bison tk-devel libpng-devel \
        hdf5 hdf5-devel
```
On SUSE, install the following packages with your package manager:
        gcc gcc-c++ gcc-fortran automake xorg-x11-devel make
        tk-devel bison libpng-devel imagemagick hdf5 hdf5-devel








For more information refer to [https://ptsg.egr.msu.edu](https://ptsg.egr.msu.edu)

Copyright © 2012-2016 Plasma Theory and Simulation Group
Electrical Computer Engineering Department
Michigan State University

Copyright © 2008 Plasma Theory and Simulation Group
Nuclear Engineering Department
University of California, Berkeley
