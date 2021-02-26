## ##########################################################################
##
## File:	otools/Makefile.am
##
## Purpose:	Running "automake" at the top level creates Makefile.in 
##		from this.
##
## ##########################################################################

EXTRA_DIST = evaluator.y 

##########
#
## Defines
#
##########
DEFINE_MPI=@DEFINE_MPI@
DEFINE_SCALAR=@DEFINE_SCALAR@
DEFINES= -DUNIX @DEFINE_MPI@ @DEFINE_SCALAR@ @DEFINE_NOX@ 

##########
#
## Define the compiler flags
#
##########
CXX	      = @CXX@
AM_CXXFLAGS   = @CXXFLAGS@ $(DEFINES)
AM_CFLAGS    = @CFLAGS@ $(DEFINES)

##########
#
## Create the library
# 
##########

AR              = @AR@
lib_LIBRARIES = libotools.a
libotools_a_AR = $(AR)

libotools_a_SOURCES = \
	c_utils.c \
	diagn.cpp \
	diag2file.cpp \
	derivDiag.cpp \
	dump.cpp \
	dumpHDF5.cpp \
	evaluator.tab.c \
	fft.c \
	gridprob.cpp \
	history.cpp \
	initwin.cpp \
	laserspotdiag.cpp \
	newdiag.cpp \
	oops.cpp \
	physics.cpp \
	powerspectrum1d.cpp \
	probergn.cpp \
	psdFieldDiag1d.cpp \
	psdFieldDiag2d.cpp \
	wavesplitdiag.cpp \
	winset.c \
	wrapFFTW.cpp

 
lib: $(lib_LIBRARIES)
	$(AR) ../lib/$(lib_LIBRARIES) $(libotools_a_OBJECTS)


##########
##
# Bison objects
##
##########

evaluator.tab.c: $(srcdir)/evaluator.y
	rm -f evaluator.tab.c evaluator.tab.h
	bison -d $(srcdir)/evaluator.y

#	if test ! -f evaluator.tab.c; then \
#	        mv evaluator.tab.c .; fi

evaluator.tab.h: evaluator.tab.c

#	if test ! -f evaluator.tab.h; then \
#	        mv evaluator.tab.h .; fi
#	rm -f evaluator.tab.c
#	bison -d $(srcdir)/evaluator.y
#	$(srcdir)/make_e_tab_h

diagn.o:   evaluator.tab.h


##########
#
## Include directories
#
##########

MPI_INCLUDE     = 
XGRAFIX_INCLUDE = @XGRAFIX_INCLUDE@
XOOPIC_INCLUDES = -I$(top_srcdir)/advisor -I$(top_srcdir)/physics -I$(top_srcdir)/otools -I$(top_builddir)/otools -I$(top_srcdir)/config
FFTW_INC 	= @FFTW_INC@
X11_INCLUDE     = -I@X11_INCDIR@
HDF5_INCLUDE	= @HDF5_INC@
AM_CPPFLAGS        = $(XOOPIC_INCLUDES) $(MPI_INCLUDE) $(XGRAFIX_INCLUDE) $(X11_INCLUDE) $(FFTW_INC)  $(HDF5_INCLUDE)


include_HEADERS = \
	diagn.h  \
	evaluator.h \
	globalvars.h  \
	laserspotdiag.h \
	oopiclist.h \
	wavesplitdiag.h \
	dump.h \
	diag2file.h \
	derivDiag.h \
	dumpHDF5.h \
	evaluator.tab.h \
	gridprob.h \
	newdiag.h \
	oops.h \
	ostring.h \
	xgdefs.h \
	eval.h \
	fft.h \
	history.h \
	numbers.h \
	probergn.h \
	baseFFT.h \
	wrapFFTW.h \
	powerspectrum1d.h \
	baseWindow.h \
	blackmanWindow.h \
	hammingWindow.h \
	welchWindow.h \
	bartlettWindow.h \
	hannWindow.h	\
	psdFieldDiag1d.h \
	psdFieldDiag2d.h

## #######
##
## Other targets
##
## #######

#uninstall-local:
#clean-local:
#	chmod a+w $(srcdir)/evaluator.tab.h $(srcdir)/evaluator.tab.c && \
#	rm -f $(srcdir)/evaluator.tab.h $(srcdir)/evaluator.tab.c
# mindgame: comment to avoid duplication and warning
#depend:
#	  $(srcdir)/evaluator.tab.c

distclean-local:
	rm -f .depend

include $(top_srcdir)/config/clean-cxx.am
include $(top_srcdir)/config/depend.am
