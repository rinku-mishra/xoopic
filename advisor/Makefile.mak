##--------------------------------------------------------------------
#
# File:		Makefile.mak
#
# Purpose:	Create advisor library for Borland
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
BCB_COMMON_FLAGS = -w -w-hid -a8 -c  -tWR -Pcxx
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
  ..\advisor\vbemitg.obj ..\advisor\advisman.obj \
  ..\advisor\bemitg.obj ..\advisor\boundg.obj ..\advisor\conductg.obj \
  ..\advisor\controlg.obj ..\advisor\cylaxisg.obj ..\advisor\diagg.obj \
  ..\advisor\diags.obj ..\advisor\dielecg.obj ..\advisor\emitprtg.obj \
  ..\advisor\emitterg.obj ..\advisor\eportg.obj ..\advisor\eqpotg.obj \
  ..\advisor\femitg.obj ..\advisor\foilg.obj ..\advisor\gapg.obj \
  ..\advisor\gridg.obj ..\advisor\iloopg.obj ..\advisor\iregiong.obj \
  ..\advisor\lnchwag.obj ..\advisor\loadg.obj \
  ..\advisor\loadpwg.obj \
  ..\advisor\mccg.obj \
  ..\advisor\param.obj ..\advisor\part1g.obj ..\advisor\particlg.obj \
  ..\advisor\plsmsrcg.obj ..\advisor\polarg.obj ..\advisor\portg.obj \
  ..\advisor\portteg.obj ..\advisor\porttmg.obj ..\advisor\porttemmwg.obj \
  ..\advisor\rules.obj \
  ..\advisor\segmentg.obj ..\advisor\spatialg.obj ..\advisor\speciesg.obj \
  ..\advisor\spemitg.obj ..\advisor\strstuff.obj ..\advisor\varloadg.obj \
  ..\advisor\abswag.obj ..\advisor\portgaussg.obj ..\advisor\laserplasmasrcg.obj


# Defines needed for compilation (do not touch)
USERDEFINES = TXQ_GUI;OPENGL;SCALAR_DOUBLE;_RWSTD_NO_EXCEPTIONS;HAVE_STD_COMPLEX;HAVE_NAMESPACES;HAVE_STD_STREAMS;TEMPLATE_FRIENDS_NEED_BRACKETS;HAVE_NONTYPE_TEMPLATE_OPERATORS;HAVE_FSTREAM;HAVE_IOSTREAM;HAVE_STRSTREAM

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
all: advisor.lib
thread:
	make -DTHREAD
release:
	make -DRELEASE

advisor.lib: $(LIBOBJFILES)
	rm -f advisor.lib
	tlib /u $@ $(LIBFLAGS) $?

# More targets for maintenance
clean:
	rm -f *.BAK *.obj *.lib *.exe *.tds *~

tidy:
	rm -f *~

