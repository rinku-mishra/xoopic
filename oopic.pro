######################################################################
#
# File:         oopic.pro
#
# Purpose:      Defines the top level oopic Makefile
#
# Version:      $Id: oopic.pro 1294 2000-07-06 16:07:33Z bruhwile $
#
# Copyright Tech-X Corporation 1999.  All rights reserved.
#
######################################################################

##########
#
# tmake stuff
#
##########
TEMPLATE	= oopic
CONFIG		= cxx develop topdir 

##########
#
# Other directories in the hierarchy
#
##########
TOP_DIR		= .
THIS_DIR	= oopic
TXCORP_DIR	=
PROJ_DIR	= $(HOME)/projects
RULES_DIR	= $(PROJ_DIR)/projrules
RULES_FILE	= $(RULES_DIR)/$(PROJ_ENV)/rules
CLASSES_DIR	=
# Where to put documentation
DOCS_DIR	= docs
DOCS_PACKAGES	=
THIS_PACKAGE	=
# Where to build libraries
PROJ_LIB_DIR	= lib

##########
#
# The various subdirectories
#
##########
IDL_DIRS	=
PURE_JAVA_DIRS	=
PURE_CXX_DIRS	= physics advisor xg qtg
LINKED_DIRS	=

##########
#
# Distribution variables.
#
# Below is a complete list of distribution variables
#
##########
# The DIST_* variables denote items for all distributions

DIST_DIR	= ./txmath
DIST_DIRS	= physics advisor xg
DIST_CXX_DIRS   = physics advisor xg
DIST_EXTS	= 
DIST_ITEMS	= LICENSE.TXT

DIST_MSVC_DIRS	= 

# The variables describing extra files needed by each of
# the DIST_TYPE's: UNIX_EQ, BORLAND, WIN32_MWERKS, MACOS_MWERKS, MSVC

UNIX_EQ_ITEMS	= README README.BENCHMARK README.INSTALL README.PARALLEL SOLARIS.NOTES
UNIX_EQ_EXTS	=
BORLAND_ITEMS	= 
BORLAND_EXTS	=
MSVC_ITEMS	= 
MSVC_EXTS	=
WIN32_MWERKS_ITEMS	= 
WIN32_MWERKS_EXTS	=
MACOS_MWERKS_ITEMS	= 
MACOS_MWERKS_EXTS	=

