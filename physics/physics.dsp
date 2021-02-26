# Microsoft Developer Studio Project File - Name="physics" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=physics - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "physics.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "physics.mak" CFG="physics - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "physics - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "physics - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "physics - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "." /I "../../txmath/txbase" /I "../otools" /I "../advisor" /I "$(QTDIR)/include" /D "OPENGL" /D "SCALAR_DOUBLE" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_STD_STREAMS" /D "HAVE_FSTREAM" /D "HAVE_OSTREAM" /D "HAVE_NONTYPE_TEMPLATE_OPERATORS" /D "HAVE_NAMESPACES" /D "HAVE_STD_COMPLEX" /D "HAVE_SSTREAM" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Release\physics.lib"

!ELSEIF  "$(CFG)" == "physics - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "." /I "../../txmath/txbase" /I "../otools" /I "../advisor" /I "$(QTDIR)/include" /D "OPENGL" /D "SCALAR_DOUBLE" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_STD_STREAMS" /D "HAVE_FSTREAM" /D "HAVE_OSTREAM" /D "HAVE_NONTYPE_TEMPLATE_OPERATORS" /D "HAVE_NAMESPACES" /D "HAVE_STD_COMPLEX" /D "HAVE_SSTREAM" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\debug\physics.lib"

!ENDIF 

# Begin Target

# Name "physics - Win32 Release"
# Name "physics - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\absorbwa.cpp
# End Source File
# Begin Source File

SOURCE=.\beamemit.cpp
# End Source File
# Begin Source File

SOURCE=.\boltzman.cpp
# End Source File
# Begin Source File

SOURCE=.\boundary.cpp
# End Source File
# Begin Source File

SOURCE=.\cg.cpp
# End Source File
# Begin Source File

SOURCE=.\conducto.cpp
# End Source File
# Begin Source File

SOURCE=.\CsMCTI.cpp
# End Source File
# Begin Source File

SOURCE=.\cursrc.cpp
# End Source File
# Begin Source File

SOURCE=.\cylindri.cpp
# End Source File
# Begin Source File

SOURCE=.\dadi.cpp
# End Source File
# Begin Source File

SOURCE=.\dadirz.cpp
# End Source File
# Begin Source File

SOURCE=.\dadixy.cpp
# End Source File
# Begin Source File

SOURCE=.\dadixyp.cpp
# End Source File
# Begin Source File

SOURCE=.\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\dielectr.cpp
# End Source File
# Begin Source File

SOURCE=.\domain.cpp
# End Source File
# Begin Source File

SOURCE=.\electrostatic_operator.cpp
# End Source File
# Begin Source File

SOURCE=.\emitter.cpp
# End Source File
# Begin Source File

SOURCE=.\eqpot.cpp
# End Source File
# Begin Source File

SOURCE=.\exitport.cpp
# End Source File
# Begin Source File

SOURCE=.\fieldemi.cpp
# End Source File
# Begin Source File

SOURCE=.\fields.cpp
# End Source File
# Begin Source File

SOURCE=.\foil.cpp
# End Source File
# Begin Source File

SOURCE=.\fowler_nordheim_emit.cpp
# End Source File
# Begin Source File

SOURCE=.\gap.cpp
# End Source File
# Begin Source File

SOURCE=.\gmres.cpp
# End Source File
# Begin Source File

SOURCE=.\gpdist.cpp
# End Source File
# Begin Source File

SOURCE=.\grid.cpp
# End Source File
# Begin Source File

SOURCE=.\HeMCTI.cpp
# End Source File
# Begin Source File

SOURCE=.\iloop.cpp
# End Source File
# Begin Source File

SOURCE=.\iregion.cpp
# End Source File
# Begin Source File

SOURCE=.\laserplasmasrc.cpp
# End Source File
# Begin Source File

SOURCE=.\launchwa.cpp
# End Source File
# Begin Source File

SOURCE=.\LiMCTI.cpp
# End Source File
# Begin Source File

SOURCE=.\load.cpp
# End Source File
# Begin Source File

SOURCE=.\loadpw.cpp
# End Source File
# Begin Source File

SOURCE=.\mapNGDs.cpp
# End Source File
# Begin Source File

SOURCE=.\maxwelln.cpp
# End Source File
# Begin Source File

SOURCE=.\mcc.cpp
# End Source File
# Begin Source File

SOURCE=.\mg_utils.cpp
# End Source File
# Begin Source File

SOURCE=.\misc.cpp
# End Source File
# Begin Source File

SOURCE=.\multigrid.cpp
# End Source File
# Begin Source File

SOURCE=.\ngd.cpp
# End Source File
# Begin Source File

SOURCE=.\parpoi.cpp
# End Source File
# Begin Source File

SOURCE=.\periodic.cpp
# End Source File
# Begin Source File

SOURCE=.\plsmadev.cpp
# End Source File
# Begin Source File

SOURCE=.\plsmasrc.cpp
# End Source File
# Begin Source File

SOURCE=.\polar.cpp
# End Source File
# Begin Source File

SOURCE=.\port.cpp
# End Source File
# Begin Source File

SOURCE=.\portgauss.cpp
# End Source File
# Begin Source File

SOURCE=.\portsumte.cpp
# End Source File
# Begin Source File

SOURCE=.\portte.cpp
# End Source File
# Begin Source File

SOURCE=.\porttemmw.cpp
# End Source File
# Begin Source File

SOURCE=.\porttm.cpp
# End Source File
# Begin Source File

SOURCE=.\psolve.cpp
# End Source File
# Begin Source File

SOURCE=.\ptclgpes.cpp
# End Source File
# Begin Source File

SOURCE=.\ptclgpib.cpp
# End Source File
# Begin Source File

SOURCE=.\ptclgpnr.cpp
# End Source File
# Begin Source File

SOURCE=.\ptclgpsr.cpp
# End Source File
# Begin Source File

SOURCE=.\ptclgrp.cpp
# End Source File
# Begin Source File

SOURCE=.\secondary.cpp
# End Source File
# Begin Source File

SOURCE=.\spblink.cpp
# End Source File
# Begin Source File

SOURCE=.\spbound.cpp
# End Source File
# Begin Source File

SOURCE=.\sptlrgn.cpp
# End Source File
# Begin Source File

SOURCE=.\varload.cpp
# End Source File
# Begin Source File

SOURCE=.\vbmemit.cpp
# End Source File
# Begin Source File

SOURCE=.\vmaxwelln.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\absorbwa.h
# End Source File
# Begin Source File

SOURCE=.\beamemit.h
# End Source File
# Begin Source File

SOURCE=.\boltzman.h
# End Source File
# Begin Source File

SOURCE=.\boundary.h
# End Source File
# Begin Source File

SOURCE=.\cg.h
# End Source File
# Begin Source File

SOURCE=.\complex.h
# End Source File
# Begin Source File

SOURCE=.\conducto.h
# End Source File
# Begin Source File

SOURCE=.\CsMCTI.h
# End Source File
# Begin Source File

SOURCE=.\cursrc.h
# End Source File
# Begin Source File

SOURCE=.\cylindri.h
# End Source File
# Begin Source File

SOURCE=.\dadi.h
# End Source File
# Begin Source File

SOURCE=.\dadirz.h
# End Source File
# Begin Source File

SOURCE=.\dadixy.h
# End Source File
# Begin Source File

SOURCE=.\dadixyp.h
# End Source File
# Begin Source File

SOURCE=.\debug.h
# End Source File
# Begin Source File

SOURCE=.\define.h
# End Source File
# Begin Source File

SOURCE=.\dielectr.h
# End Source File
# Begin Source File

SOURCE=.\domain.h
# End Source File
# Begin Source File

SOURCE=.\electrostatic_operator.h
# End Source File
# Begin Source File

SOURCE=.\emitport.h
# End Source File
# Begin Source File

SOURCE=.\emitter.h
# End Source File
# Begin Source File

SOURCE=.\eqpot.h
# End Source File
# Begin Source File

SOURCE=.\exitport.h
# End Source File
# Begin Source File

SOURCE=.\fieldemi.h
# End Source File
# Begin Source File

SOURCE=.\fields.h
# End Source File
# Begin Source File

SOURCE=.\foil.h
# End Source File
# Begin Source File

SOURCE=.\fowler_nordheim_emit.h
# End Source File
# Begin Source File

SOURCE=.\gap.h
# End Source File
# Begin Source File

SOURCE=.\gmres.h
# End Source File
# Begin Source File

SOURCE=.\gpdist.h
# End Source File
# Begin Source File

SOURCE=.\grid.h
# End Source File
# Begin Source File

SOURCE=.\HeMCTI.h
# End Source File
# Begin Source File

SOURCE=.\iloop.h
# End Source File
# Begin Source File

SOURCE=.\inverter.h
# End Source File
# Begin Source File

SOURCE=.\iregion.h
# End Source File
# Begin Source File

SOURCE=.\laserplasmasrc.h
# End Source File
# Begin Source File

SOURCE=.\launchwa.h
# End Source File
# Begin Source File

SOURCE=.\LiMCTI.h
# End Source File
# Begin Source File

SOURCE=.\lindman.h
# End Source File
# Begin Source File

SOURCE=.\load.h
# End Source File
# Begin Source File

SOURCE=.\loadpw.h
# End Source File
# Begin Source File

SOURCE=.\lookback.h
# End Source File
# Begin Source File

SOURCE=.\mapNGDs.h
# End Source File
# Begin Source File

SOURCE=.\maxwelln.h
# End Source File
# Begin Source File

SOURCE=.\mcc.h
# End Source File
# Begin Source File

SOURCE=.\mcmcld.h
# End Source File
# Begin Source File

SOURCE=.\mg_utils.h
# End Source File
# Begin Source File

SOURCE=.\misc.h
# End Source File
# Begin Source File

SOURCE=.\multigrid.h
# End Source File
# Begin Source File

SOURCE=.\N_vector.h
# End Source File
# Begin Source File

SOURCE=.\ngd.h
# End Source File
# Begin Source File

SOURCE=.\operators.h
# End Source File
# Begin Source File

SOURCE=.\ovector.h
# End Source File
# Begin Source File

SOURCE=.\parpoi.h
# End Source File
# Begin Source File

SOURCE=.\particle.h
# End Source File
# Begin Source File

SOURCE=.\periodic.h
# End Source File
# Begin Source File

SOURCE=.\plsmadev.h
# End Source File
# Begin Source File

SOURCE=.\plsmasrc.h
# End Source File
# Begin Source File

SOURCE=.\polar.h
# End Source File
# Begin Source File

SOURCE=.\port.h
# End Source File
# Begin Source File

SOURCE=.\portgauss.h
# End Source File
# Begin Source File

SOURCE=.\portsumte.h
# End Source File
# Begin Source File

SOURCE=.\portte.h
# End Source File
# Begin Source File

SOURCE=.\porttemmw.h
# End Source File
# Begin Source File

SOURCE=.\porttm.h
# End Source File
# Begin Source File

SOURCE=.\precision.h
# End Source File
# Begin Source File

SOURCE=.\psolve.h
# End Source File
# Begin Source File

SOURCE=.\ptclgpes.h
# End Source File
# Begin Source File

SOURCE=.\ptclgpib.h
# End Source File
# Begin Source File

SOURCE=.\ptclgpnr.h
# End Source File
# Begin Source File

SOURCE=.\ptclgpsr.h
# End Source File
# Begin Source File

SOURCE=.\ptclgrp.h
# End Source File
# Begin Source File

SOURCE=.\rhbck.h
# End Source File
# Begin Source File

SOURCE=.\secondary.h
# End Source File
# Begin Source File

SOURCE=.\spacetimefunc.h
# End Source File
# Begin Source File

SOURCE=.\spblink.h
# End Source File
# Begin Source File

SOURCE=.\spbound.h
# End Source File
# Begin Source File

SOURCE=.\species.h
# End Source File
# Begin Source File

SOURCE=.\spemit.h
# End Source File
# Begin Source File

SOURCE=.\sptlrgn.h
# End Source File
# Begin Source File

SOURCE=.\symmetry.h
# End Source File
# Begin Source File

SOURCE=.\timefunc.h
# End Source File
# Begin Source File

SOURCE=.\varload.h
# End Source File
# Begin Source File

SOURCE=.\vbmemit.h
# End Source File
# Begin Source File

SOURCE=.\vmaxwelln.h
# End Source File
# End Group
# End Target
# End Project
