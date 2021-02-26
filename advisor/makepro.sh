#!/bin/sh
#
# Use -d for debugging
  proFile="`basename $PWD`.pro"
  echo "Making $proFile"
  qmake -project -t lib  \
   "INCLUDEPATH=..\xg ..\otools ..\advisor ..\physics  ." \
   "CONFIG+=staticlib thread release" \
   -win32 \
   "win32-borland: QMAKE_CXXFLAGS_RELEASE+=-Pcxx" \
   "win32-borland: INCLUDEPATH+=\$(BCB)\include "  \
   "win32-msvc: QMAKE_CXXFLAGS_RELEASE+=-GX -GR" \
   "DEFINES += SCALAR_DOUBLE OPENGL HAVE_OPENGL" \
   "win32-msvc: DEFINES += HAVE_STD_STREAMS HAVE_FSTREAM HAVE_OSTREAM HAVE_NONTYPE_TEMPLATE_OPERATORS HAVE_NAMESPACES HAVE_STD_COMPLEX NDEBUG" \
   -nopwd *.cpp .

  echo "Edit $proFile and remove: "
  echo "All .cxx files"
  echo " \ and end of DEPENDPATH"
