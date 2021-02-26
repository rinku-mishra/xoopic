## ##########################################################################
##
## File:	advisor/Makefile.am
##
## Purpose:	Running "automake" at the top level creates Makefile.in 
##		from this.
##
## ##########################################################################

##########
#
## Defines
#
##########
DEFINE_MPI=@DEFINE_MPI@
DEFINE_MPI_DEBUG=@DEFINE_MPI_DEBUG@
DEFINE_SCALAR=@DEFINE_SCALAR@
DEFINES= -DUNIX @DEFINE_MPI@ @DEFINE_MPI_DEBUG@ @DEFINE_SCALAR@ @DEFINE_NOX@

##########
#
## Define the compiler flags
#
##########
CXX	   = @CXX@
AM_CXXFLAGS   = @CXXFLAGS@ $(DEFINES)

##########
#
## Create the library
#
##########

lib_LIBRARIES = libadvisor.a

libadvisor_a_SOURCES = \
pmlportg.cpp obemitg.cpp eqpotg.cpp abswag.cpp controlg.cpp gapg.cpp particlg.cpp vbemitg.cpp advisman.cpp cylaxisg.cpp gridg.cpp portg.cpp bemitg.cpp diags.cpp lnchwag.cpp rules.cpp boundg.cpp eportg.cpp param.cpp spatialg.cpp conductg.cpp femitg.cpp part1g.cpp strstuff.cpp dielecg.cpp speciesg.cpp loadg.cpp varloadg.cpp porttmg.cpp portteg.cpp foilg.cpp emitprtg.cpp emitterg.cpp polarg.cpp diagg.cpp iloopg.cpp iregiong.cpp plsmsrcg.cpp laserplasmasrcg.cpp segmentg.cpp spemitg.cpp mccg.cpp portgaussg.cpp porttemmwg.cpp loadpwg.cpp h5diagg.cpp cursrcg.cpp fowler_nordheim_emit_g.cpp

AR              = @AR@
libadvisor_a_LIBADD = @CXX_REPOSITORY@
libadvisor_a_AR = $(AR)

lib: $(lib_LIBRARIES)
	$(AR) ../lib/$(lib_LIBRARIES) $(libadvisor_a_OBJECTS)


##########
#
## Include directories
#
##########


XGRAFIX_INCLUDE = @XGRAFIX_INCLUDE@
# Need to get at otools both ways in case of paralle directory build
XOOPIC_INCLUDES = -I$(top_srcdir)/physics -I../otools -I$(top_srcdir)/otools -I$(top_srcdir)/config
#X11_INCLUDE     = -I@X11_INCDIR@
FFTW_INC 	= @FFTW_INC@
HDF5_INCLUDE	= @HDF5_INC@
AM_CPPFLAGS     = $(XOOPIC_INCLUDES) $(XGRAFIX_INCLUDE) $(FFTW_INC) $(HDF5_INCLUDE)

include_HEADERS = \
pmlportg.h		abswag.h		foilg.h			portgaussg.h \
advisman.h		gapg.h			portteg.h \
bemitg.h		gridg.h			porttmg.h \
boundg.h		iloopg.h		rules.h \
conductg.h		iregiong.h		segmentg.h \
controlg.h		laserplasmasrcg.h	spatialg.h \
cylaxisg.h		lnchwag.h		spboundg.h \
diagg.h			loadg.h			speciesg.h \
diags.h			mccg.h			spemitg.h \
dielecg.h		param.h			symmetg.h \
emitprtg.h		part1g.h		varg.h \
emitterg.h		particlg.h		varloadg.h \
eportg.h		plsmsrcg.h		vbemitg.h \
eqpotg.h		polarg.h                femitg.h \
portg.h                 porttemmwg.h            loadpwg.h \
h5diagg.h		cursrcg.h		fowler_nordheim_emit_g.h

## #######
##
## Other targets
##
## #######

distclean-local:
	rm -f .depend

include $(top_srcdir)/config/clean-cxx.am
include $(top_srcdir)/config/depend.am
