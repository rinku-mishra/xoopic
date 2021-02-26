## ##########################################################################
##
## File:	xg/Makefile.am
##
## Purpose:	Running "automake" at the top level creates Makefile.in 
##		from this.
##
## ##########################################################################

# Needed for later use
AR		= @AR@
AR_FLAGS	= @AR_FLAGS@
OBJEXT          = @OBJEXT@
CXX_REPOSITORY  = @CXX_REPOSITORY@
RPATH_FLAG      = @RPATH_FLAG@
EXEEXT          = @EXEEXT@

##########
#
## Some definitions
#
##########

DEFINES	= -DUNIX @DEFINE_MPI@ @DEFINE_MPI_DEBUG@ @DEFINE_SCALAR@ @DEFINE_NOX@

##########
#
## Define the compiler flags
#
##########
CXX_LIBFLAG	= @CXX_LIBFLAG@
AM_CXXFLAGS	= @CXXFLAGS@ $(DEFINES) -fno-inline
CFLAGS		= @CFLAGS@ $(DEFINES)

##########
#
## Create the library
#
##########
lib_LIBRARIES = libxg.a

libxg_a_SOURCES = xgmain.cpp
libxg_a_LIBADD = @CXX_REPOSITORY@
libxg_a_AR = $(AR)

lib: $(lib_LIBRARIES)
	$(AR) ../lib/$(lib_LIBRARIES) $(libxg_a_OBJECTS)

##########
#
## Libraries
#
##########

#
# X11
#
X11_LIBDIR = @X11_LIBDIR@
X11_LIBS   = -L@X11_LIBDIR@ @RPATH_FLAG@@X11_LIBDIR@ @X11_LIBS@                                                    
# X11_LIBDIR = @X11_LIBDIR@
# X11_LIB    = -L$(X11_LIBDIR) $(RPATH_FLAG)$(X11_LIBDIR) @X11_LIB@ @XT_LIB@

XPM_LIBDIR =@XPM_LIBDIR@
XPM_LIB = -L$(XPM_LIBDIR) $(RPATH_FLAG)$(XPM_LIBDIR) @XPM_LIB@

#
# SYSTEM
#
SYS_LIBS = @DYNLINK_LIB@ -L/usr/lib
# DYNLINK_LIB=@DYNLINK_LIB@

#
# physics and parsing
#
OOPIC_LIBS = -L../otools -lotools -L../advisor -ladvisor -L../physics -lphysics -lotools -ladvisor

#
# FFTW
#
FFTW_INC = @FFTW_INC@
FFTW_LIB = @FFTW_LIB@

#
# HDF5
#
HDF5_INC    = @HDF5_INC@
HDF5_LIBDIR = @HDF5_LIBDIR@
HDF5_LIBS    = @HDF5_LIBS@ @RPATH_FLAG@@HDF5_LIBDIR@

#
# XGrafix
#
XGRAFIX_LIBDIR = @XGRAFIX_LIBDIR@
XGRAFIX_LIB    = -L$(XGRAFIX_LIBDIR) $(RPATH_FLAG)$(XGRAFIX_LIBDIR) @XGRAFIX_LIB@

#
# Tcl
#
TCL_LIB =@TCL_LIB@

#
# ?
#
NET_LIBS =@NET_LIBS@


##########
#
## Include directories
#
##########

XGRAFIX_INCLUDE = @XGRAFIX_INCLUDE@
XOOPIC_INCLUDES = -I$(top_srcdir)/otools -I../otools -I$(top_srcdir)/physics -I$(top_srcdir)/advisor -I$(top_srcdir)/config
AM_CPPFLAGS        = $(XOOPIC_INCLUDES) $(XGRAFIX_INCLUDE) $(HDF5_INC) $(FFTW_INC)

include_HEADERS = 


##########
#
## bin programs
#
##########

bin_PROGRAMS        = xoopic
xoopic_SOURCES      = main.cpp
xoopic_DEPENDENCIES = ../advisor/libadvisor.a ../physics/libphysics.a ../otools/libotools.a
xoopic_LDADD        = xgmain.o $(OOPIC_LIBS) $(HDF5_LIBS) \
		      $(XGRAFIX_LIB) $(XPM_LIB) $(X11_LIB) $(TCL_LIB) $(NET_LIBS) $(SYS_LIBS) $(FFTW_LIB) \
		       @ZLIB_DIR@ -lz $(CXX_LIBFLAG) -lm

clean-local:
	$(RM) $(OBJECTS)

#########
##
## Other targets
##
#########

include ../config/clean-cxx.am
include ../config/depend.am
