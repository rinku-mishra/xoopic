#! Use the common Unix template
#$ IncludeTemplate("recproj.t");

#######  Additional targets

##########
#
# Note the system of last build
#
##########

all: lastbuild

lastbuild:
	rm -f lastbuild*
	( machname=`uname -m | sed 's/\//-/'` ;\
	touch lastbuild=$(PROJ_ENV)-$$machname )

clean: cleanlastbuild

cleanlastbuild:
	rm -f lastbuild*

releasedocs:	apidocs
	(cd docs; makedist)
