######################################################################
#
# File:	        qtg.pro
#
# Purpose:	tmake project file for qtg
#
# Version:	$Id: qtg.pro 1294 2000-07-06 16:07:33Z bruhwile $
#
# Copyright 1999, Tech-X Corporation
#
######################################################################

##########
#
# tmake stuff
#
##########
TEMPLATE	= qtg
TARGET		= xgmain
CONFIG		= cxx qt opengl staticlib develop 

##########
#
# Other directories in the hierarchy
#
##########
THIS_DIR        = qtg
TOP_DIR         = ..
TXCORP_DIR      =
PROJ_DIR        = $(HOME)/projects
RULES_DIR       = $(PROJ_DIR)/projrules
RULES_FILE      = $(RULES_DIR)/$(PROJ_ENV)/rules
DOCS_DIR        = $(TOP_DIR)/docs/cxxapi/qtg
DOCS_PACKAGES   =
THIS_PACKAGE    =

##########
#
# Locations for libraries, class files, and archives
#
##########
PROJ_LIB_DIR	= $(TOP_DIR)/lib
PROJ_JSRCS_DIR	=
PROJ_CLASSES_DIR= $(TOP_DIR)/classes

##########
#
# Other libraries that this depends on and compilation defines
#
##########
PROJ_DEPS       = advisor physics
PROJ_DEFINES	= SCALAR_FLOAT OPENGL 
PROJ_LIBS       = advisor physics xgmain

##########
#
# The various subdirectories
#
##########
IDL_DIRS        =
PURE_JAVA_DIRS  =
PURE_CXX_DIRS   =
LINKED_DIRS     = 

# All headers

HEADERS      =	\
		fft.h \
		globalvars.h \
		gridprob.h \
		history.h \
		list.h \
		newdiag.h \
		ostring.h \
		probergn.h \
		physics.h \
		ButtonCmds.h \
		ButtonList.h \
		CreateWindows.h \
		DiagnosticList.h \
		GLPlot3D.h \
		Information.h \
		MainButtons.h \
		Plot3D.h \
		Plot3DWindow.h \
		PlotWindow.h \
		TxqList.h \
		TxqMainWin.h \
		TxqPlot.h \
		TxqPlot3DWindow.h \
		TxqPlotBorder.h \
		TxqPlotWindow.h \
		WindowList.h \
		windowtype.h \
		xgdatamacros.h \
		xgmini.h \
		xgminint.h \
		xgnox.h \
		xgprintgif.h \
		xgrafix.h \
		xgrafixint.h \
		xpoint.h


		
# Source files for creation of main target (library)

SOURCES     =	\
		c_utils.c \
		evaluator.tab.c \
		fft.c \
		xgnox.c \
		xgreadrit.c \
		xginitvar.c \
		xginitc.c \
		xgsetup.c \
		xgrescale.c \
		diagn.cpp \
		dump.cpp \
		gridprob.cpp \
		history.cpp \
		initwin.cpp \
		main.cpp \
		newdiag.cpp \
		probergn.cpp \
		physics.cpp \
		printf.cpp \
		ButtonCmds.cpp \
		TxqMainWin.cpp \
		TxqPlot.cpp \
		TxqPlotBorder.cpp \
		TxqPlotWindow.cpp \
		ButtonList.cpp \
		MainButtons.cpp \
		DiagnosticList.cpp \
		Information.cpp \
		CreateWindows.cpp \
		WindowList.cpp \
		TxqPlot3DWindow.cpp \
		GLPlot3D.cpp \
		moc_ButtonList.cpp \
		moc_DiagnosticList.cpp \
		moc_Information.cpp \
		moc_TxqPlot.cpp \
		moc_TxqPlotBorder.cpp \
		moc_TxqPlotWindow.cpp \
		moc_TxqPlot3DWindow.cpp \
		moc_GLPlot3D.cpp \
		moc_ButtonCmds.cpp \
		moc_MainButtons.cpp

# Additional testing executables

EXECS = xgmain

# Additional files to remove with "make clean"

CLEAN_FILES  =

##########
#
# Distribution variables
#
##########

# The DIST_* variables denote items for all distributions
DIST_DIR	= ../oopic/qtg
DIST_DIRS	=
DIST_CXX_DIRS	=
DIST_EXTS	= .h .cpp
DIST_ITEMS	= 

# The variables describing extra files needed by each of
# the DIST_TYPE's: UNIX_EQ, BORLAND, WIN32_MWERKS, MACOS_MWERKS, MSVC

UNIX_EQ_ITEMS	= 
UNIX_EQ_EXTS	=
BORLAND_ITEMS	=
BORLAND_EXTS	=
MSVC_ITEMS	=
MSVC_EXTS	=
WIN32_MWERKS_ITEMS	=
WIN32_MWERKS_EXTS	=
MACOS_MWERKS_ITEMS	=
MACOS_MWERKS_EXTS	=
