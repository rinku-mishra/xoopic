dnl ######################################################################
dnl
dnl find OpenGL installation
dnl
dnl ######################################################################
 
AC_CHECKING(OpenGL installation)

# Use frameworks on Mac
QT_NEEDS_X11=no
case "$host" in

  *-apple-darwin*)
    GL_LIBS="-framework OpenGL -framework AGL"
    OPENGL_INCDIR="/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers"
    HAVE_GL="yes"
    OPENGL_DEFINE="-DOPENGL"
    ;;

  *)
    if test "$QT_GUI" != "no"; then
      QT_NEEDS_X11=yes
    fi
    ;;

esac

# Get command line guess for location
AC_ARG_WITH(opengl-dir, [  --with-opengl-dir=<opengl-dir>  set the location of OpenGL distribution], OPENGL_DIR="$withval")
if test -n "$OPENGL_DIR"; then
  OPENGL_LIBDIR="$OPENGL_DIR/lib"
  OPENGL_INCDIR="$OPENGL_DIR/include"
else
  AC_ARG_WITH(opengl-libdir,[  --with-opengl-libdir=<opengl-libdir> set location of OpenGL libraries], OPENGL_LIBDIR="$withval")
  AC_ARG_WITH(opengl-incdir,[  --with-opengl-incdir=<opengl-incdir>   set location of OpenGL include files], OPENGL_INCDIR="$withval")
fi

# Remaining to find X gl libraries

if test "$QT_NEEDS_X11" = yes; then

  HAVE_MESAGL="yes"
  HAVE_GL="yes"
  OPENGL_DEFINE="-DOPENGL"
  OPENGL_LIBS=""
  OPENGL_DIR=""

# Set search path for include files
  if test -n "$OPENGL_INCDIR"; then
    PATH_CHECK="$OPENGL_INCDIR"
  else
    PATH_CHECK="/usr/include/GL:/usr/X11R6/include:/usr/openwin/include:/usr/include:/usr/X11/include:/usr/local/mesa/include:/usr/local/include:$PATH"
  fi

# Search for include files
  AC_PATH_PROGS(GL_H, gl.h, "", $PATH_CHECK)
  if test -z "$GL_H"; then
    echo " "
    echo "WARNING: Cannot find OpenGL include files."
    echo "Please compile and install OpenGL or use --with-opengl-incdir=OPENGL_INCDIR"
    echo " "
    HAVE_GL="no"
    OPENGL_DEFINE=""
    OPENGL_INCDIR=""
  else
    if test -z "$OPENGL_INCDIR"; then
      OPENGL_INCDIR="`dirname $GL_H`"
    fi
    OPENGL_DIR="`dirname $OPENGL_INCDIR`"
  fi
  
# Set search path for libraries
  if test -n "$OPENGL_LIBDIR"; then
    PATH_CHECK="$OPENGL_LIBDIR"
  else
# try to find the location of the MesaGL installation
# First check for Mesa, if not found, check for standard GL
# JRC: reversing this.
    PATH_CHECK="/usr/lib64:/usr/X11R6/lib64:/usr/X11R6/lib:/usr/openwin/lib:/usr/lib:/usr/local/lib:/lib:/usr/X11/lib:/usr/local/mesa/lib/$compiler:/usr/local/mesa/lib:$PATH"
  fi

# Now find library
  AC_PATH_PROGS(OPENGL_LIB, libGL.dylib libGL$SO libGL.a, "", $PATH_CHECK)
  if test -z "$OPENGL_LIB"; then
    echo " "
    echo "WARNING: Cannot find OpenGL installation."
    echo "Please compile and install OpenGL or use --with-opengl-dir=OPENGL_DIR"
    echo " "
    HAVE_GL="no"
    OPENGL_DEFINE=""
    OPENGL_LIBDIR=""
    OPENGL_LIBS=""
  else
    PATH_CHECK="$OPENGL_LIBDIR_PATH:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries:/usr/X11R6/lib64:/usr/lib64:/usr/X11R6/lib:/usr/openwin/lib:/usr/lib:/usr/local/lib:/lib:/usr/X11/lib:/usr/local/mesa/lib/$compiler:/usr/local/mesa/lib:$PATH"
    AC_PATH_PROGS(GLU_LIB, libGLU.dylib libGLU$SO libGLU.a, "", $PATH_CHECK)
    if test -z "$GLU_LIB"; then
      echo " "
      echo "WARNING: Cannot find OpenGL libGLU installation."
      echo "Please compile and install OpenGL or use --with-opengl-dir=OPENGL_DIR"
      echo " "
      HAVE_GL="no"
      OPENGL_DEFINE=""
      OPENGL_LIBDIR=""
      OPENGL_LIBS=""
    else 
      OPENGL_LIBDIR=`dirname $OPENGL_LIB`
      OPENGL_LIBS="-L$OPENGL_LIBDIR -lGL"
      GLU_LIBDIR=`dirname $GLU_LIB`
      GLU_LIBS="-L$GLU_LIBDIR -lGLU"
      GL_LIBS="-L$GLU_LIBDIR -lGLU -L$OPENGL_LIBDIR -lGL"
    fi
  fi

fi


if test $HAVE_GL = yes;
then
  AC_DEFINE(HAVE_OPENGL)
fi
AC_SUBST(OPENGL_DEFINE)
AC_SUBST(OPENGL_INCDIR)
AC_SUBST(OPENGL_LIBDIR)
AC_SUBST(OPENGL_LIBS)
AC_SUBST(GLU_LIBDIR)
AC_SUBST(GLU_LIBS)
AC_SUBST(GL_LIBS)
