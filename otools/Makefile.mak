##--------------------------------------------------------------------
#
# File:		Makefile.mak
#
# Purpose:	Create otools library for Borland
#
# Version:    $Id: Makefile.mak 1918 2002-06-18 22:30:16Z kgl $
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


# Define BCB extra Link flag
BCB_LFLAGS = -v

# Define BCB cw lib
BCB_LIB = cw32.lib

# Define compiler flags, common, release and thread
# BCB_COMMON_FLAGS = -w -w-hid -a8 -c -tWR -Pcxx
BCB_COMMON_FLAGS = -w -a8 -c -tWR
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
  probergn.obj physics.obj \
  dump.obj gridprob.obj history.obj \
  newdiag.obj diagn.obj fft.obj \
  evaluator.tab.obj c_utils.obj \
  laserspotdiag.obj wavesplitdiag.obj \
  initwin.obj winset.obj

# Defines needed for compilation (do not touch)
USERDEFINES = OPENGL;SCALAR_DOUBLE;_RWSTD_NO_EXCEPTIONS;HAVE_STD_COMPLEX;HAVE_NAMESPACES;HAVE_STD_STREAMS;TEMPLATE_FRIENDS_NEED_BRACKETS;HAVE_NONTYPE_TEMPLATE_OPERATORS;HAVE_FSTREAM;HAVE_IOSTREAM;HAVE_STRSTREAM;TXQ_GUI

##--------------------------------------------------------------------
#
# DERIVED VARIABLES (do not touch)
#
##--------------------------------------------------------------------

# Defines and command line arguments
PATHCPP=.
CXXFLAGS = -I$(BCB)\Include;..\xg;..\otools;..\physics;..\advisor \
  $(BCBFLAGS)  \
  -D$(USERDEFINES)
LIBFLAGS = /P256
LFLAGS	 = -L$(BCB)\Lib -ap -Tpe -Gn $(BCB_LFLAGS)


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
all: eval otools.lib
release:
	make -DRELEASE
thread:
	make -DTHREAD

msvc:
   nmake -f Makefile.msvc

eval: evaluator.tab.c

evaluator.tab.c: evaluator.y
        echo "Making evaluator.tab.c"
        bison evaluator.y
        echo "Making evaluator.tab.h"
        sh make_e_tab_h /bin/grep


otools.lib: $(LIBOBJFILES)
	rm -f otools.lib
	tlib /u $@ $(LIBFLAGS) $?

# More targets for maintenance
clean:
	rm -f *.BAK *.obj *.lib *.exe *.tds *~
   rm -f evaluator.tab.h evaluator.tab.c

tidy:
	rm -f *~

