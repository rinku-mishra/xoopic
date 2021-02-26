## ##########################################################################
##
## File:	physics/Makefile.am
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
CXX	      = @CXX@
AM_CXXFLAGS   = @CXXFLAGS@ $(DEFINES)

##########
#
## Create the library
#
##########

lib_LIBRARIES = libphysics.a

libphysics_a_SOURCES = \
secondary.cpp gpdist.cpp absorbwa.cpp beamemit.cpp boundary.cpp conducto.cpp cylindri.cpp dadi.cpp dadirz.cpp dadixy.cpp emitter.cpp exitport.cpp fieldemi.cpp fields.cpp gap.cpp grid.cpp launchwa.cpp vmaxwelln.cpp maxwelln.cpp port.cpp portte.cpp portsumte.cpp porttm.cpp ptclgrp.cpp sptlrgn.cpp vbmemit.cpp plsmadev.cpp eqpot.cpp dielectr.cpp load.cpp varload.cpp boltzman.cpp coulomb.cpp pmlport.cpp obemit.cpp mcc.cpp misc.cpp foil.cpp ptclgpes.cpp polar.cpp ptclgpnr.cpp ptclgpib.cpp psolve.cpp iloop.cpp iregion.cpp plsmasrc.cpp spbound.cpp multigrid.cpp mg_utils.cpp spblink.cpp periodic.cpp dadixyp.cpp electrostatic_operator.cpp cg.cpp gmres.cpp domain.cpp parpoi.cpp ptclgpsr.cpp portgauss.cpp laserplasmasrc.cpp ngd.cpp porttemmw.cpp loadpw.cpp mapNGDs.cpp HeMCTI.cpp LiMCTI.cpp CsMCTI.cpp cursrc.cpp fowler_nordheim_emit.cpp


AR              = @AR@
libphysics_a_LIBADD = @CXX_REPOSITORY@
libphysics_a_AR = $(AR)

lib: $(lib_LIBRARIES)
	$(AR) ../lib/$(lib_LIBRARIES) $(libphysics_a_OBJECTS)


##########
#
## Include directories
#
##########

# Both directories needed for parallel builds
XOOPIC_INCLUDES = -I$(top_srcdir)/advisor -I$(top_srcdir)/otools -I../otools -I$(top_srcdir)/config

XGRAFIX_INCLUDE = @XGRAFIX_INCLUDE@
#X11_INCLUDE     = -I@X11_INCDIR@
#FFTW_INCDIR 	= @FFTW_INCDIR@
FFTW_INC 	= @FFTW_INC@
HDF5_INCLUDE	= @HDF5_INC@
#INCLUDES        = $(XOOPIC_INCLUDES) $(XGRAFIX_INCLUDE) $(FFTW_INC) $(HDF5_INCLUDE) -I@QT_INCDIR@
AM_CPPFLAGS        = $(XOOPIC_INCLUDES) $(XGRAFIX_INCLUDE) $(FFTW_INC) $(HDF5_INCLUDE)

include_HEADERS = \
HeMCTI.h                        gmres.h                         port.h \
LiMCTI.h                        gpdist.h                        portgauss.h \
N_vector.h                      grid.h                          portsumte.h \
absorbwa.h                      iloop.h                         portte.h \
beamemit.h                      inverter.h                      porttemmw.h \
boltzman.h                      iregion.h                       porttm.h \
boundary.h                      laserplasmasrc.h                precision.h \
cg.h                            launchwa.h                      psolve.h \
complex.h                       lindman.h                       ptclgpes.h \
conducto.h                      load.h                          ptclgpib.h \
cylindri.h                      loadpw.h                        ptclgpnr.h \
dadi.h                          lookback.h                      ptclgpsr.h \
dadirz.h                        mapNGDs.h                       ptclgrp.h \
dadixy.h                        coulomb.h			maxwelln.h                      rhbck.h \
dadixyp.h                       pmlport.h			mcc.h                           secondary.h \
debug.h                         mcmcld.h                        spacetimefunc.h \
define.h                        mg_utils.h                      spblink.h \
dielectr.h                      misc.h                          spbound.h \
domain.h                        multigrid.h                     species.h \
electrostatic_operator.h        ngd.h                           spemit.h \
emitport.h                      operators.h                     sptlrgn.h \
emitter.h                       ovector.h                       symmetry.h \
eqpot.h                         parpoi.h                        timefunc.h \
exitport.h                      particle.h                      varload.h \
fieldemi.h                      periodic.h                      vbmemit.h \
fields.h                        plsmadev.h                      vmaxwelln.h \
foil.h                          plsmasrc.h                      fowler_nordheim_emit.h \
gap.h                           polar.h				cursrc.h \
CsMCTI.h	

## #######
##
## Other targets
##
## #######

distclean-local:
	rm -f .depend

include $(top_srcdir)/config/clean-cxx.am
include $(top_srcdir)/config/depend.am
