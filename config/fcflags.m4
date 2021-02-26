dnl ######################################################################
dnl
dnl File:	fcflags.m4
dnl
dnl Purpose:	
dnl   	Given FC (f90) compiler, determine flags
dnl   	See also fc.m4 and fcinterop.m4
dnl
dnl Variables set here include
dnl      FC_LD
dnl      FC_DBL_FLAG
dnl        Promote r4 to r8
dnl 	 FC_WARN_FLAG
dnl	   Turn on additional warnings. NEED WORK: defined only for lf95!	
dnl      FC_OPT_FLAG
dnl        Normal optimization
dnl      FC_MACH_FLAG
dnl        Machine dependent flags for "ultra" optimization.  NEEDS WORK!!
dnl      FC_NOOPT_FLAG
dnl        o Sometimes better to explicitly turn off optimization
dnl      FC_DEBUG_FLAG
dnl        Include check array conformance if available
dnl      FC_OTHER_FLAG
dnl        Things should probably be used, but are not strictly necessary
dnl      FC_INCLUDE_FLAG
dnl        Because Absoft does not use the -I flag for include directories
dnl      FC_PIC_FLAG
dnl        Position-Independent Code (for shared libraries)
dnl      FCFLAGS
dnl
dnl Description of differences between levels of optimization: 
dnl   Standard: basic optimizations that are low-risk and do not lead to
dnl   very long compilation times or code bloat. Full compliance with the
dnl   IEEE-754 standard for floating point accuracy. No fancy tricks like
dnl   loop unrolling, inlining of non-inline functions, instruction reordering,
dnl   rewriting floating point expressions, or inter-procedural analysis. No
dnl   cpu-specific code. This generally corresponds to '-O2' or so.
dnl   
dnl   Full: aggressive optimizations that might lead to longer compile
dnl   times and/or code size increase. Compliance with the IEEE standard
dnl   is relaxed. Function inlining permitted. Other fancy tricks also allowed
dnl   to some extend. No cpu-specific code. This generally corresponds to '-O3'.
dnl   
dnl   Ultra: all caution to the wind. Compliance with IEEE goes out of the
dnl   window. Any trick the compiler provides may be used. Code can be
dnl   tailored to a specific cpu. Compiler options are highly platform-dependent
dnl 
dnl NOTES: 
dnl        o FC is the autotools awful variable name for any fortran
dnl          version beyond F77.  We use it interchangably with f90
dnl
dnl Version:	$Id: fcflags.m4 2366 2007-08-13 22:27:28Z srinath $
dnl
dnl Copyright Tech-X Corporation, 2001.  Redistribution allowed provided
dnl this copyright statement remains intact.
dnl
dnl ######################################################################
if test -n "$FC_INCLUDE_FLAG"; then SAVE_FC_INCLUDE_FLAG="$FC_INCLUDE_FLAG"; fi
if test -n "$FC_DBL_FLAG"; then SAVE_FC_DBL_FLAG="$FC_DBL_FLAG"; fi
if test -n "$FC_OPT_FLAG"; then SAVE_FC_OPT_FLAG="$FC_OPT_FLAG"; fi
if test -n "$FC_MACH_FLAG"; then SAVE_FC_MACH_FLAG="$FC_MACH_FLAG"; fi
if test -n "$FC_FREE_FLAG"; then SAVE_FC_FREE_FLAG="$FC_FREE_FLAG"; fi
if test -n "$FC_FIXED_FLAG"; then SAVE_FC_FIXED_FLAG="$FC_FIXED_FLAG"; fi
if test -n "$FC_NOOPT_FLAG"; then SAVE_FC_NOOPT_FLAG="$FC_NOOPT_FLAG"; fi
if test -n "$FC_DEBUG_FLAG"; then SAVE_FC_DEBUG_FLAG="$FC_DEBUG_FLAG"; fi
if test -n "$FC_ALWAY_FLAG"; then SAVE_FC_ALWAY_FLAG="$FC_ALWAY_FLAG"; fi
if test -n "$FC_PIC_FLAG"; then SAVE_FC_PIC_FLAG="$FC_PIC_FLAG"; fi
if test -n "$FC_LDFLAGS"; then SAVE_FC_LDFLAGS="$FC_LDFLAGS"; fi

dnl ----------------------------------------------------------------------
dnl  allow flags or environment to overwrite variables
dnl ----------------------------------------------------------------------
AC_ARG_WITH(FCFLAGS,
    [  --with-FCFLAGS="MyFCFLAGS"   to set Fortran flags with Myf90],
    FCFLAGS="$withval")

AC_ARG_ENABLE(debug,[  --enable-debug   turn on debug and turn off optimizaton],
              DEBUG=yes, DEBUG=no)
AC_ARG_ENABLE(optimize,[  --enable-optimize       turn on optimization flags],
              OPTIMIZED=yes, OPTIMIZED=no)
AC_ARG_ENABLE(fulloptimize,[  --enable-fulloptimize   turn on full optimization flags],
              OPTIMIZED=full, OPTIMIZED=$OPTIMIZED)
AC_ARG_ENABLE(ultraoptimize,[  --enable-ultraoptimize  turn on ultraoptimization flags -- O5],
              OPTIMIZED=ultra, OPTIMIZED=$OPTIMIZED)

dnl NOT ENABLED YET
dnl AC_ARG_ENABLE(profiling,[  --enable-profiling      turn on profiling],
dnl PROFILING=yes, PROFILING=no)

dnl ----------------------------------------------------------------------
dnl Check on host and f90 since it is possible to have same compiler on 
dnl different platforms with subtle differences
dnl ----------------------------------------------------------------------
case "$host" in
 x86*-linux* | i386-*-linux* | i686-*linux*)
  processor=`uname -p`
  case $FC_BASE in
     *lf95*)
         FC_DBL_FLAG="--dbl "
         case $OPTIMIZED in
           yes)  FC_OPT_FLAG="-O --prefetch 2" ;;
           full) FC_OPT_FLAG="-O3" ;;
           ultra) FC_OPT_FLAG="-O5" ;;
         esac
         FC_NOOPT_FLAG="-O0"
         FC_DEBUG_FLAG="-g --chk --trap --trace"
         FC_ALWAY_FLAG="--ap --pca"
         FC_PIC_FLAG="-shared"
	   FC_WARN_FLAG="--f95" 
           dnl turn on Fortran 95 confirmation warning
	   case $processor in 
	      athlon)  FC_MACH_FLAG="" ;;
	      i686)  FC_MACH_FLAG="" ;;
	   esac
      ;;
     *pgf90)
          FC_DBL_FLAG="-r8 "
          dnl SEK: I think fast has all of these other optimizations but not sure
          dnl  FC_OPT_FLAG="-fast -Mcache_align -Munroll -Mdalign -Minline -Mvect=prefetch"
          case $OPTIMIZED in
            yes)  FC_OPT_FLAG="-O2" ;;
            full) FC_OPT_FLAG="-fast" ;;
            ultra) FC_OPT_FLAG="-fast" ;;
          esac
          FC_NOOPT_FLAG=""
          FC_DEBUG_FLAG="-g -C -Minfo"
	   dnl Use little endian for binary compatibility
          FC_ALWAY_FLAG="-byteswapio"
	   case $processor in 
	      athlon)  FC_MACH_FLAG="" ;;
	      i686)  FC_MACH_FLAG="" ;;
	   esac
      ;;
     *pathf90)
          FC_DBL_FLAG="-r8 "
          dnl O3 seems to cause problems with NIMROD
          case $OPTIMIZED in
            yes)  FC_OPT_FLAG="-O2" ;;
            full) FC_OPT_FLAG="-O2" ;;
            ultra) FC_OPT_FLAG="-O2" ;;
          esac
          FC_NOOPT_FLAG=""
          FC_DEBUG_FLAG="-g -C"
          FC_ALWAY_FLAG=""
	    FC_LDFLAGS="-Wl,--warn-unresolved-symbols"
     	   case $processor in 
	      athlon) FC_MACH_FLAG="" ;;
	      x86_64) FC_MACH_FLAG="-march=opteron -mtune=opteron -msse3" ;;
	   esac
         ;;
     *ifort)
          FC_DBL_FLAG="-autodouble "
          case $OPTIMIZED in
            yes)  FC_OPT_FLAG="-O2" ;;
            full) FC_OPT_FLAG="-O3" ;;
            ultra) FC_OPT_FLAG="-O5" ;;
          esac
          FC_NOOPT_FLAG="-O0"
          FC_DEBUG_FLAG="-g -inline_debug_info"
          FC_ALWAY_FLAG="-cm -w -w95"
          FC_PIC_FLAG="-Kpic"
          FC_MACH_FLAG="-O2 -tpp7 -xW"
         ;;
     *ifc)
          FC_DBL_FLAG="-autodouble "
          case $OPTIMIZED in
            yes)  FC_OPT_FLAG="-O2" ;;
            full) FC_OPT_FLAG="-O3" ;;
            ultra) FC_OPT_FLAG="-O5" ;;
          esac
          FC_NOOPT_FLAG="-O0"
         dnl  FC_OPT_FLAG="-O2 -tpp7 -xW"
	   dnl Pentium 4
          FC_DEBUG_FLAG="-g -inline_debug_info"
          FC_ALWAY_FLAG="-cm -w -l32 -w95"
          FC_PIC_FLAG="-Kpic"
         ;;
     *bsoft)
         dnl SEK: Not sure about some of these flags because the naming
         dnl      scheme is so screwed up
          FC_INCLUDE_FLAG="-p"
          FC_DBL_FLAG="-N11"
          case $OPTIMIZED in
            yes)   FC_OPT_FLAG="-O -B100" ;;
            full)  FC_OPT_FLAG="-O3" ;;
            ultra) FC_OPT_FLAG="-O5" ;;
          esac
          FC_NOOPT_FLAG="-O0"
          FC_DEBUG_FLAG="-g"
          FC_ALWAY_FLAG=""
          FC_LD="$FC -L$flibdir -lU77 -lfio -lf77math -lf90math"
          FC_PIC_FLAG="-fpic"
         ;;
     *g95)
          FC_DBL_FLAG=""
          case $OPTIMIZED in
            yes)  FC_OPT_FLAG="-O2" ;;
            full) FC_OPT_FLAG="-O3" ;;
            ultra) FC_OPT_FLAG="-O5" ;;
          esac
          FC_NOOPT_FLAG="-O0"
          FC_DEBUG_FLAG="-g"
          FC_ALWAY_FLAG="-fno-second-underscore"
	    FC_MACH_FLAG=""
         ;;
     *gfortran)
          FC_DBL_FLAG=""
          case $OPTIMIZED in
            yes)  FC_OPT_FLAG="-O2" ;;
            full) FC_OPT_FLAG="-O3" ;;
            ultra) FC_OPT_FLAG="-O3" ;;
          esac
          FC_NOOPT_FLAG="-O0"
          FC_DEBUG_FLAG="-g"
          FC_ALWAY_FLAG="-fno-second-underscore"
	    FC_MACH_FLAG=""
         ;;
     *)
          FC_DBL_FLAG=""
          case $OPTIMIZED in
            yes)  FC_OPT_FLAG="-O" ;;
            full) FC_OPT_FLAG="-O" ;;
            ultra) FC_OPT_FLAG="-O" ;;
          esac
          FC_NOOPT_FLAG=""
          FC_DEBUG_FLAG="-g"
          FC_ALWAY_FLAG=""
	    FC_MACH_FLAG=""
         ;;
	esac
	;;

 *apple-darwin*)
  case $FC_BASE in
     *xlf90)
          FC_DBL_FLAG="-autodouble "
          case $OPTIMIZED in
            yes)   FC_OPT_FLAG="-O2" ;;
            full)  FC_OPT_FLAG="-O3 -qstrict" ;;
            ultra) FC_OPT_FLAG="-O3 -qstrict" ;;
          esac
          FC_NOOPT_FLAG="-O0"
          FC_DEBUG_FLAG="-g"
          FC_ALWAY_FLAG="-w -l32 -qextname -qalign=natural"
          FC_FIXED_FLAG="-qfixed" 
          FC_FREE_FLAG="-qfree -qsuffix=f=f90" 
          FC_PIC_FLAG="-qpic"
          FC_MACH_FLAG="-qarch=auto -qtune=auto -qcache=auto"
         ;;
     *Absoft*)
         dnl SEK: Not sure about some of these flags because the naming
         dnl      scheme is so screwed up
          FC_INCLUDE_FLAG="-p"
          FC_DBL_FLAG="-N11"
          case $OPTIMIZED in
            yes)   FC_OPT_FLAG="-O -B100 -cpu:g4" ;;
            full)  FC_OPT_FLAG="-O -B100 -cpu:g4" ;;
            ultra) FC_OPT_FLAG="-O -B100 -cpu:g4" ;;
          esac
          FC_NOOPT_FLAG="-O0"
          FC_DEBUG_FLAG="-g"
          FC_ALWAY_FLAG="-YEXT_NAMES=LCS -YEXT_SFX=_"
          FC_LD="$FC -lU77 -lfio -lf77math -lf90math"
	    FC_MACH_FLAG=""
         ;;
     *)
          FC_DBL_FLAG=""
          case $OPTIMIZED in
            yes)   FC_OPT_FLAG="-O" ;;
            full)  FC_OPT_FLAG="-O" ;;
            ultra) FC_OPT_FLAG="-O" ;;
          esac
          FC_NOOPT_FLAG=""
          FC_DEBUG_FLAG="-g"
          FC_ALWAY_FLAG=""
	    FC_MACH_FLAG=""
         ;;
	esac
	 ;;

 hppa*-hp-hpux*)
       FC_LD="$FC +z"
       case $OPTIMIZED in
         yes)  FC_OPT_FLAG="-O2" ;;
         full) FC_OPT_FLAG="-O3" ;;
         ultra) FC_OPT_FLAG="-O5" ;;
       esac
	;;
 alpha*-cray-unicos*)
       case $OPTIMIZED in
         yes)  FC_OPT_FLAG="-O2" ;;
         full) FC_OPT_FLAG="-O3" ;;
         ultra) FC_OPT_FLAG="-O5" ;;
       esac
      ;;

 *-*-aix*)
	dnl DEFAULT_FFLAGS="-g -O2 -qextname -qalign=natural"
      dnl  FC_OPT_FLAG="-O2 -tpp7 -xW"
      dnl SEK: Don't know how to get architecture yet
      dnl  FC_ALWAY_FLAG="-q32 -qxlf90=autodealloc"
       FC_DBL_FLAG="-qrealsize=8"
       case $OPTIMIZED in
         yes)   FC_OPT_FLAG="-O2 -qstrict" ;;
         full)  FC_OPT_FLAG="-O3 -qstrict" ;;
         ultra) FC_OPT_FLAG="-O5 -qstrict" ;;
       esac
       FC_MACH_FLAG="-qarch=auto -qtune=auto -qcache=auto -qmaxmem=-1"
       FC_NOOPT_FLAG="-qnoopt"
       FC_DEBUG_FLAG="-g -qcheck -qsigtrap -qflttrap"
      dnl autodealloc default on most compilers
       FC_ALWAY_FLAG="-qxlf90=autodealloc" 
       FC_FIXED_FLAG="-qfixed" 
       FC_FREE_FLAG="-qfree -qsuffix=f=f90:cpp=F90" 
       FC_PIC_FLAG="-qpic"
	;;
 *-sgi-irix6*)
       FC_DBL_FLAG="-r8 -d16"
       case $OPTIMIZED in
         yes)   FC_OPT_FLAG="-O" ;;
         full)  FC_OPT_FLAG="-O3" ;;
         ultra) FC_OPT_FLAG="-O5" ;;
       esac
       FC_NOOPT_FLAG=""
       FC_DEBUG_FLAG="-g"
       FC_ALWAY_FLAG=""
       FC_LDFLAGS=""
	;;


esac

dnl ----------------------------------------------------------------------
dnl Set up default flags by setting variables that haven't been defined
dnl ----------------------------------------------------------------------
if test -z "$FC_INCLUDE_FLAG"; then FC_INCLUDE_FLAG="-I"; fi

dnl Can't test on all platforms, so this could cause errors.
if test -z "$FC_PIC_FLAG"; then FC_PIC_FLAG="-fpic"; fi

dnl ----------------------------------------------------------------------
dnl 
dnl ----------------------------------------------------------------------
if test -n "$SAVE_FC_INCLUDE_FLAG"; then FC_INCLUDE_FLAG="$SAVE_FC_INCLUDE_FLAG"; fi
if test -n "$SAVE_FC_DBL_FLAG"; then FC_DBL_FLAG="$SAVE_FC_DBL_FLAG"; fi
if test -n "$SAVE_FC_OPT_FLAG"; then FC_OPT_FLAG="$SAVE_FC_OPT_FLAG"; fi
if test -n "$SAVE_FC_MACH_FLAG"; then FC_MACH_FLAG="$SAVE_FC_MACH_FLAG"; fi
if test -n "$SAVE_FC_FREE_FLAG"; then FC_FREE_FLAG="$SAVE_FC_FREE_FLAG"; fi
if test -n "$SAVE_FC_FIXED_FLAG"; then FC_FIXED_FLAG="$SAVE_FC_FIXED_FLAG"; fi
if test -n "$SAVE_FC_NOOPT_FLAG"; then FC_NOOPT_FLAG="$SAVE_FC_NOOPT_FLAG"; fi
if test -n "$SAVE_FC_DEBUG_FLAG"; then FC_DEBUG_FLAG="$SAVE_FC_DEBUG_FLAG"; fi
if test -n "$SAVE_FC_ALWAY_FLAG"; then FC_ALWAY_FLAG="$SAVE_FC_ALWAY_FLAG"; fi
if test -n "$SAVE_FC_PIC_FLAG"; then FC_PIC_FLAG="$SAVE_FC_PIC_FLAG"; fi
if test -n "$SAVE_FC_LDFLAGS"; then FC_LDFLAGS="$SAVE_FC_LDFLAGS"; fi

dnl ----------------------------------------------------------------------
dnl   Set flags based on input.  Debug flag overwrites optimization flag
dnl ----------------------------------------------------------------------

if test -n "$FC"; then
  if test -z "$FC_LD"; then
      FC_LD=$FC
  fi
  if test -z "$FC_LDFLAGS"; then
      FC_LDFLAGS=""
  fi
  if test -z "$SAVE_FCFLAGS"; then
     FCFLAGS="$FC_ALWAY_FLAG"
     
     dnl Note that NIMROD does this by hand in each Makefile.am if needed.
     if test "$DOUBLE" = yes; then
        FCFLAGS="$FC_DBL_FLAG $FCFLAGS"
     fi
     
     case $OPTIMIZED in
       yes)
         FCFLAGS="$FCFLAGS $FC_OPT_FLAG"
         ;;
       full | ultra)
         AC_MSG_WARN(May generate code for this processor only.)
         FCFLAGS="$FCFLAGS $FC_OPT_FLAG $FC_MACH_FLAG"
         ;;
       noopt)
         FCFLAGS="$FCFLAGS $FC_NOOPT_FLAG"
         ;;
     esac
     
     if test "$DEBUG" = yes; then
        FCFLAGS="$FC_DEBUG_FLAG $FCFLAGS"
     fi
  fi
fi

dnl ----------------------------------------------------------------------
dnl Allow overriding of FCFLAGS
dnl ----------------------------------------------------------------------
if test -n "$SAVE_FCFLAGS"; then FCFLAGS="$SAVE_FCFLAGS"; fi

dnl ----------------------------------------------------------------------
dnl  AC_SUBST everything to allow fine-grained control of compilation
dnl ----------------------------------------------------------------------
AC_SUBST(FCFLAGS)
AC_SUBST(FC_LD)
AC_SUBST(FC_LDFLAGS)
AC_SUBST(FC_INCLUDE_FLAG)
AC_SUBST(FC_DBL_FLAG)
AC_SUBST(FC_OPT_FLAG)
AC_SUBST(FC_MACH_FLAG)
AC_SUBST(FC_NOOPT_FLAG)
AC_SUBST(FC_DEBUG_FLAG)
AC_SUBST(FC_FIXED_FLAG)
AC_SUBST(FC_FREE_FLAG)
AC_SUBST(FC_ALWAY_FLAG)
AC_SUBST(FC_PIC_FLAG)
AC_SUBST(FC_WARN_FLAG)
dnl ----------------------------------------------------------------------
dnl  Print to config summary file if defined
dnl ----------------------------------------------------------------------
if test -n "$config_summary_file"; then
	echo " FC=$FC"                               >> $config_summary_file
	echo "   FCFLAGS=$FCFLAGS"                   >> $config_summary_file
	echo "   FC_INCLUDE_FLAG=$FC_INCLUDE_FLAG"   >> $config_summary_file
	echo "   FC_DBL_FLAG=$FC_DBL_FLAG"           >> $config_summary_file
	echo "   FC_OPT_FLAG=$FC_OPT_FLAG"           >> $config_summary_file
	echo "   FC_MACH_FLAG=$FC_MACH_FLAG"         >> $config_summary_file
	echo "   FC_FREE_FLAG=$FC_FREE_FLAG"         >> $config_summary_file
	echo "   FC_FIXED_FLAG=$FC_FIXED_FLAG"       >> $config_summary_file
	echo "   FC_NOOPT_FLAG=$FC_NOOPT_FLAG"       >> $config_summary_file
	echo "   FC_DEBUG_FLAG=$FC_DEBUG_FLAG"       >> $config_summary_file
	echo "   FC_ALWAY_FLAG=$FC_ALWAY_FLAG"       >> $config_summary_file
	echo "   FC_PIC_FLAG=$FC_PIC_FLAG"           >> $config_summary_file
	echo "   FC_LDFLAGS=$FC_LDFLAGS"             >> $config_summary_file
      echo                                         >> $config_summary_file
fi
