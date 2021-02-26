# Microsoft Developer Studio Project File - Name="otools" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=otools - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "otools.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "otools.mak" CFG="otools - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "otools - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "otools - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "otools - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
LINK32=link.exe
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "." /I "../../txmath/txbase" /I "../physics" /I "../advisor" /I "../xg" /D "TXQ_GUI" /D "OPENGL" /D "SCALAR_DOUBLE" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_STD_STREAMS" /D "HAVE_FSTREAM" /D "HAVE_OSTREAM" /D "HAVE_NONTYPE_TEMPLATE_OPERATORS" /D "HAVE_NAMESPACES" /D "HAVE_STD_COMPLEX" /D "HAVE_SSTREAM" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Release\otools.lib"

!ELSEIF  "$(CFG)" == "otools - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
LINK32=link.exe
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "." /I "../../txmath/txbase" /I "../physics" /I "../advisor" /I "../xg" /D "TXQ_GUI" /D "OPENGL" /D "SCALAR_DOUBLE" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_STD_STREAMS" /D "HAVE_FSTREAM" /D "HAVE_OSTREAM" /D "HAVE_NONTYPE_TEMPLATE_OPERATORS" /D "HAVE_NAMESPACES" /D "HAVE_STD_COMPLEX" /D "HAVE_SSTREAM" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\debug\otools.lib"

!ENDIF 

# Begin Target

# Name "otools - Win32 Release"
# Name "otools - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\c_utils.c
# End Source File
# Begin Source File

SOURCE=.\diagn.cpp
# End Source File
# Begin Source File

SOURCE=.\dump.cpp
# End Source File
# Begin Source File

SOURCE=.\dumpHDF5.cpp
# End Source File
# Begin Source File

SOURCE=.\evaluator.tab.c
# End Source File
# Begin Source File

SOURCE=.\fft.c
# End Source File
# Begin Source File

SOURCE=.\gridprob.cpp
# End Source File
# Begin Source File

SOURCE=.\half.cpp
# End Source File
# Begin Source File

SOURCE=.\history.cpp
# End Source File
# Begin Source File

SOURCE=.\initwin.cpp
# End Source File
# Begin Source File

SOURCE=.\laserspotdiag.cpp
# End Source File
# Begin Source File

SOURCE=.\newdiag.cpp
# End Source File
# Begin Source File

SOURCE=.\physics.cpp
# End Source File
# Begin Source File

SOURCE=.\powerspectrum1d.cpp
# End Source File
# Begin Source File

SOURCE=.\probergn.cpp
# End Source File
# Begin Source File

SOURCE=.\psdFieldDiag1d.cpp
# End Source File
# Begin Source File

SOURCE=.\psdFieldDiag2d.cpp
# End Source File
# Begin Source File

SOURCE=.\wavesplitdiag.cpp
# End Source File
# Begin Source File

SOURCE=.\winset.c
# End Source File
# Begin Source File

SOURCE=.\wrapFFTW.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\bartlettWindow.h
# End Source File
# Begin Source File

SOURCE=.\baseFFT.h
# End Source File
# Begin Source File

SOURCE=.\baseWindow.h
# End Source File
# Begin Source File

SOURCE=.\blackmanWindow.h
# End Source File
# Begin Source File

SOURCE=.\diagn.h
# End Source File
# Begin Source File

SOURCE=.\dump.h
# End Source File
# Begin Source File

SOURCE=.\dumpHDF5.h
# End Source File
# Begin Source File

SOURCE=.\eval.h
# End Source File
# Begin Source File

SOURCE=.\evaluator.h
# End Source File
# Begin Source File

SOURCE=.\fft.h
# End Source File
# Begin Source File

SOURCE=.\globalvars.h
# End Source File
# Begin Source File

SOURCE=.\gridprob.h
# End Source File
# Begin Source File

SOURCE=.\hammingWindow.h
# End Source File
# Begin Source File

SOURCE=.\hannWindow.h
# End Source File
# Begin Source File

SOURCE=.\history.h
# End Source File
# Begin Source File

SOURCE=.\laserspotdiag.h
# End Source File
# Begin Source File

SOURCE=.\newdiag.h
# End Source File
# Begin Source File

SOURCE=.\numbers.h
# End Source File
# Begin Source File

SOURCE=.\oopiclist.h
# End Source File
# Begin Source File

SOURCE=.\ostring.h
# End Source File
# Begin Source File

SOURCE=.\powerspectrum1d.h
# End Source File
# Begin Source File

SOURCE=.\probergn.h
# End Source File
# Begin Source File

SOURCE=.\psdFieldDiag1d.h
# End Source File
# Begin Source File

SOURCE=.\psdFieldDiag2d.h
# End Source File
# Begin Source File

SOURCE=.\wavesplitdiag.h
# End Source File
# Begin Source File

SOURCE=.\welchWindow.h
# End Source File
# Begin Source File

SOURCE=.\wrapFFTW.h
# End Source File
# Begin Source File

SOURCE=.\xgdefs.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\evaluator.y

!IF  "$(CFG)" == "otools - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\evaluator.y
InputName=evaluator

BuildCmds= \
	%CYGWINDIR%\bin\bison.exe -d $(InputDir)\$(InputName).y \
	%CYGWINDIR%\bin\bison.exe -d $(InputDir)\$(InputName).y \
	

".\$(InputName).tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\$(InputName).tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "otools - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputDir=.
InputPath=.\evaluator.y
InputName=evaluator

BuildCmds= \
	c:\cygwin\bin\bison.exe -d $(InputDir)\$(InputName).y

".\$(InputName).tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\$(InputName).tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
