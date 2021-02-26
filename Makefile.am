## ##########################################################################
##
## File:        OOPIC Makefile.am
##
## Purpose:     Run automake at the top level to make Makefile.in in all
##              subdirectories.
##
## ##########################################################################
 
PIC_DIRS = otools physics advisor xg 
SUBDIRS  = $(PIC_DIRS) config
 
all-local:
	-rm -f xoopic
	-/bin/ln -s xg/xoopic xoopic

depend:
	for i in $(PIC_DIRS) ; do \
	  (cd $$i; \
           echo "Making depend in $$i"; \
           make depend) \
	done

crufix: linuxfix

linuxfix:
	for i in $(PIC_DIRS) ; do \
	( cd $$i; \
	  echo "fixing the Makefile in $$i"; \
	  mv Makefile Makefile.old ;\
	  cat Makefile.old | sed 's/ cru / /' > Makefile);\
	  mv Makefile.in Makefile.in.old ;\
	  cat Makefile.in.old | sed 's/ cru / /' > Makefile.in) \
	done

automake:
	./config/automake.sh
