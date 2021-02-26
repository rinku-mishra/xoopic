# Microsoft Developer Studio Project File - Name="advisor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=advisor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "advisor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "advisor.mak" CFG="advisor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "advisor - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "advisor - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "advisor - Win32 Release"

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
MTL=midl.exe
LINK32=link.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "../otools" /I "../physics" /I "../../txmath/txbase" /D "TXQ_GUI" /D "OPENGL" /D "SCALAR_DOUBLE" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_STD_STREAMS" /D "HAVE_FSTREAM" /D "HAVE_OSTREAM" /D "HAVE_NONTYPE_TEMPLATE_OPERATORS" /D "HAVE_NAMESPACES" /D "HAVE_STD_COMPLEX" /D "HAVE_SSTREAM" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Release\advisor.lib"

!ELSEIF  "$(CFG)" == "advisor - Win32 Debug"

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
MTL=midl.exe
LINK32=link.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /Zi /Od /I "../otools" /I "../physics" /I "../../txmath/txbase" /D "TXQ_GUI" /D "OPENGL" /D "SCALAR_DOUBLE" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_STD_STREAMS" /D "HAVE_FSTREAM" /D "HAVE_OSTREAM" /D "HAVE_NONTYPE_TEMPLATE_OPERATORS" /D "HAVE_NAMESPACES" /D "HAVE_STD_COMPLEX" /D "HAVE_SSTREAM" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\debug\advisor.lib"

!ENDIF 

# Begin Target

# Name "advisor - Win32 Release"
# Name "advisor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\abswag.cpp
# End Source File
# Begin Source File

SOURCE=.\advisman.cpp
# End Source File
# Begin Source File

SOURCE=.\bemitg.cpp
# End Source File
# Begin Source File

SOURCE=.\boundg.cpp
# End Source File
# Begin Source File

SOURCE=.\conductg.cpp
# End Source File
# Begin Source File

SOURCE=.\controlg.cpp
# End Source File
# Begin Source File

SOURCE=.\cursrcg.cpp
# End Source File
# Begin Source File

SOURCE=.\cylaxisg.cpp
# End Source File
# Begin Source File

SOURCE=.\diagg.cpp
# End Source File
# Begin Source File

SOURCE=.\diags.cpp
# End Source File
# Begin Source File

SOURCE=.\dielecg.cpp
# End Source File
# Begin Source File

SOURCE=.\emitprtg.cpp
# End Source File
# Begin Source File

SOURCE=.\emitterg.cpp
# End Source File
# Begin Source File

SOURCE=.\eportg.cpp
# End Source File
# Begin Source File

SOURCE=.\eqpotg.cpp
# End Source File
# Begin Source File

SOURCE=.\femitg.cpp
# End Source File
# Begin Source File

SOURCE=.\foilg.cpp
# End Source File
# Begin Source File

SOURCE=.\fowler_nordheim_emit_g.cpp
# End Source File
# Begin Source File

SOURCE=.\gapg.cpp
# End Source File
# Begin Source File

SOURCE=.\gridg.cpp
# End Source File
# Begin Source File

SOURCE=.\h5diagg.cpp
# End Source File
# Begin Source File

SOURCE=.\iloopg.cpp
# End Source File
# Begin Source File

SOURCE=.\iregiong.cpp
# End Source File
# Begin Source File

SOURCE=.\laserplasmasrcg.cpp
# End Source File
# Begin Source File

SOURCE=.\lnchwag.cpp
# End Source File
# Begin Source File

SOURCE=.\loadg.cpp
# End Source File
# Begin Source File

SOURCE=.\loadpwg.cpp
# End Source File
# Begin Source File

SOURCE=.\mccg.cpp
# End Source File
# Begin Source File

SOURCE=.\param.cpp
# End Source File
# Begin Source File

SOURCE=.\part1g.cpp
# End Source File
# Begin Source File

SOURCE=.\particlg.cpp
# End Source File
# Begin Source File

SOURCE=.\plsmsrcg.cpp
# End Source File
# Begin Source File

SOURCE=.\polarg.cpp
# End Source File
# Begin Source File

SOURCE=.\portg.cpp
# End Source File
# Begin Source File

SOURCE=.\portgaussg.cpp
# End Source File
# Begin Source File

SOURCE=.\portteg.cpp
# End Source File
# Begin Source File

SOURCE=.\porttemmwg.cpp
# End Source File
# Begin Source File

SOURCE=.\porttmg.cpp
# End Source File
# Begin Source File

SOURCE=.\rules.cpp
# End Source File
# Begin Source File

SOURCE=.\segmentg.cpp
# End Source File
# Begin Source File

SOURCE=.\spatialg.cpp
# End Source File
# Begin Source File

SOURCE=.\speciesg.cpp
# End Source File
# Begin Source File

SOURCE=.\spemitg.cpp
# End Source File
# Begin Source File

SOURCE=.\strstuff.cpp
# End Source File
# Begin Source File

SOURCE=.\varloadg.cpp
# End Source File
# Begin Source File

SOURCE=.\vbemitg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\abswag.h
# End Source File
# Begin Source File

SOURCE=.\advisman.h
# End Source File
# Begin Source File

SOURCE=.\bemitg.h
# End Source File
# Begin Source File

SOURCE=.\boundg.h
# End Source File
# Begin Source File

SOURCE=.\conductg.h
# End Source File
# Begin Source File

SOURCE=.\controlg.h
# End Source File
# Begin Source File

SOURCE=.\cursrcg.h
# End Source File
# Begin Source File

SOURCE=.\cylaxisg.h
# End Source File
# Begin Source File

SOURCE=.\diagg.h
# End Source File
# Begin Source File

SOURCE=.\diags.h
# End Source File
# Begin Source File

SOURCE=.\dielecg.h
# End Source File
# Begin Source File

SOURCE=.\emitprtg.h
# End Source File
# Begin Source File

SOURCE=.\emitterg.h
# End Source File
# Begin Source File

SOURCE=.\eportg.h
# End Source File
# Begin Source File

SOURCE=.\eqpotg.h
# End Source File
# Begin Source File

SOURCE=.\femitg.h
# End Source File
# Begin Source File

SOURCE=.\foilg.h
# End Source File
# Begin Source File

SOURCE=.\fowler_nordheim_emit_g.h
# End Source File
# Begin Source File

SOURCE=.\gapg.h
# End Source File
# Begin Source File

SOURCE=.\gridg.h
# End Source File
# Begin Source File

SOURCE=.\h5diagg.h
# End Source File
# Begin Source File

SOURCE=.\iloopg.h
# End Source File
# Begin Source File

SOURCE=.\iregiong.h
# End Source File
# Begin Source File

SOURCE=.\laserplasmasrcg.h
# End Source File
# Begin Source File

SOURCE=.\lnchwag.h
# End Source File
# Begin Source File

SOURCE=.\loadg.h
# End Source File
# Begin Source File

SOURCE=.\loadpwg.h
# End Source File
# Begin Source File

SOURCE=.\mccg.h
# End Source File
# Begin Source File

SOURCE=.\param.h
# End Source File
# Begin Source File

SOURCE=.\part1g.h
# End Source File
# Begin Source File

SOURCE=.\particlg.h
# End Source File
# Begin Source File

SOURCE=.\plsmsrcg.h
# End Source File
# Begin Source File

SOURCE=.\polarg.h
# End Source File
# Begin Source File

SOURCE=.\portg.h
# End Source File
# Begin Source File

SOURCE=.\portgaussg.h
# End Source File
# Begin Source File

SOURCE=.\portteg.h
# End Source File
# Begin Source File

SOURCE=.\porttemmwg.h
# End Source File
# Begin Source File

SOURCE=.\porttmg.h
# End Source File
# Begin Source File

SOURCE=.\rules.h
# End Source File
# Begin Source File

SOURCE=.\segmentg.h
# End Source File
# Begin Source File

SOURCE=.\spatialg.h
# End Source File
# Begin Source File

SOURCE=.\spboundg.h
# End Source File
# Begin Source File

SOURCE=.\speciesg.h
# End Source File
# Begin Source File

SOURCE=.\spemitg.h
# End Source File
# Begin Source File

SOURCE=.\symmetg.h
# End Source File
# Begin Source File

SOURCE=.\varg.h
# End Source File
# Begin Source File

SOURCE=.\varloadg.h
# End Source File
# Begin Source File

SOURCE=.\vbemitg.h
# End Source File
# End Group
# End Target
# End Project
