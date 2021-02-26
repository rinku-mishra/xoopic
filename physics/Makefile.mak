##--------------------------------------------------------------------
#
# File:		Makefile.mak
#
# Purpose:	Create physics library for Borland
#
# Version:	$Id: Makefile.mak 1830 2002-02-12 23:50:44Z kgl $
#
##--------------------------------------------------------------------

##--------------------------------------------------------------------
#
# LOCATION OF COMPILER (do not touch)
#
##--------------------------------------------------------------------

#Define compiler location if not already done
!if !$d(BCB)
BCB = $(MAKEDIR)\..
!endif


# Define Qt libraries
QT_LIBS = qt301 qtmain.lib

# Define BCB extra Link flag
BCB_LFLAGS = -v

# Define BCB cw lib
BCB_LIB = cw32.lib

# Define compiler flags, common, release and thread
# BCB_COMMON_FLAGS = -w -w-hid -a8 -c -tWR -Pcxx
BCB_COMMON_FLAGS = -w -w-hid -a8 -c -tWR  -Pcxx
BCB_DEBUG_FLAGS   =  -Od  -r-  -k -y -v -vi- $(BCB_COMMON_FLAGS)
BCB_RELEASE_FLAGS =  -O2  -Vx  -k- -vi  $(BCB_COMMON_FLAGS)
# BCB_THREAD_FLAGS = -tWM  -DQT_THREAD_SUPPORT  $(BCB_RELEASE_FLAGS)
BCB_THREAD_FLAGS =  -O2  -tWM  -DQT_THREAD_SUPPORT  $(BCB_COMMON_FLAGS)

# Obtain BCB FLAGS based on make -D ???? symbol
BCBFLAGS = $(BCB_DEBUG_FLAGS)
!if $d(RELEASE)
BCBFLAGS = $(BCB_RELEASE_FLAGS)
!endif

!if $d(THREAD)
BCBFLAGS = $(BCB_THREAD_FLAGS)
QT_LIBS = qtmt301.lib qtmain.lib
BCB_LFLAGS = -c -x
BCB_LIB = cw32mti.lib
!endif



##--------------------------------------------------------------------
#
# USER DEFINED VARIABLES
#
##--------------------------------------------------------------------

# The objects for this library
LIBOBJFILES	=  \
  ..\physics\vmaxwelln.obj ..\physics\beamemit.obj \
  ..\physics\boltzman.obj ..\physics\boundary.obj ..\physics\cg.obj \
  ..\physics\conducto.obj ..\physics\cylindri.obj ..\physics\dadi.obj \
  ..\physics\dadirz.obj ..\physics\dadixy.obj ..\physics\dadixyp.obj \
  ..\physics\dielectr.obj ..\physics\domain.obj \
  ..\physics\electrostatic_operator.obj ..\physics\emitter.obj \
  ..\physics\eqpot.obj ..\physics\exitport.obj ..\physics\fieldemi.obj \
  ..\physics\fields.obj ..\physics\foil.obj ..\physics\gap.obj \
  ..\physics\gmres.obj ..\physics\gpdist.obj ..\physics\grid.obj \
  ..\physics\iloop.obj ..\physics\iregion.obj ..\physics\launchwa.obj \
  ..\physics\load.obj ..\physics\loadpw.obj \
  ..\physics\maxwelln.obj ..\physics\mcc.obj \
  ..\physics\mg_utils.obj ..\physics\misc.obj ..\physics\multigrid.obj \
  ..\physics\ngd.obj \
  ..\physics\parpoi.obj ..\physics\periodic.obj ..\physics\plsmadev.obj \
  ..\physics\plsmasrc.obj ..\physics\polar.obj ..\physics\port.obj \
  ..\physics\portsumte.obj ..\physics\portte.obj ..\physics\porttm.obj \
  ..\physics\porttemmw.obj \
  ..\physics\psolve.obj ..\physics\ptclgpes.obj ..\physics\ptclgpib.obj \
  ..\physics\ptclgpnr.obj ..\physics\ptclgpsr.obj ..\physics\ptclgrp.obj \
  ..\physics\secondary.obj ..\physics\spblink.obj ..\physics\spbound.obj \
  ..\physics\sptlrgn.obj ..\physics\varload.obj ..\physics\vbmemit.obj \
  ..\physics\absorbwa.obj ..\physics\portgauss.obj \
  ..\physics\laserplasmasrc.obj ..\physics\mapNGDs.obj \
  ..\physics\LiMCTI.obj ..\physics\HeMCTI.obj

# Defines needed for compilation (do not touch)
USERDEFINES = OPENGL;SCALAR_DOUBLE;_RWSTD_NO_EXCEPTIONS;HAVE_STD_COMPLEX;HAVE_NAMESPACES;HAVE_STD_STREAMS;TEMPLATE_FRIENDS_NEED_BRACKETS;HAVE_NONTYPE_TEMPLATE_OPERATORS;HAVE_FSTREAM;HAVE_IOSTREAM;HAVE_STRSTREAM

##--------------------------------------------------------------------
#
# DERIVED VARIABLES (do not touch)
#
##--------------------------------------------------------------------

# Defines and command line arguments
PATHCPP=.
CXXFLAGS = -I$(BCB)\Include;..\otools;..\physics;..\advisor \
   $(BCBFLAGS) -D$(USERDEFINES)
LIBFLAGS = /P256
LFLAGS	 = -L$(BCB)\Lib -ap -Tpe  -Gn $(BCB_LFLAGS)


# Compile even for implicit dependencies
.autodepend

##--------------------------------------------------------------------
#
# DEFAULT RULES (do not touch)
#
##--------------------------------------------------------------------

.cpp.obj:
	bcc32 $(CXXFLAGS) -n$(@D) {$< }

.c.obj:
	bcc32 $(CXXFLAGS) -n$(@D) {$< }

##--------------------------------------------------------------------
#
# USER DEFINED TARGETS
#
##--------------------------------------------------------------------
all: physics.lib
thread:
	make -DTHREAD
release:
	make -DRELEASE

physics.lib: $(LIBOBJFILES)
	rm -f physics.lib
	tlib /u $@ $(LIBFLAGS) $?

# More targets for maintenance
clean:
	rm -f *.BAK *.obj *.lib *.exe *.tds *~

tidy:
	rm -f *~

