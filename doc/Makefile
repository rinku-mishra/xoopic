# JK, 2016-02-25
REFMAN=xoopic-source-reference.pdf

all: docs doxygen-docs

docs: 
	latex diagnostics_how_to.tex
	dvips diagnostics_how_to
	dvipdf diagnostics_how_to

doxygen-docs: 
	doxygen doxygen.conf
	make -C latex 
	cp latex/refman.pdf $(REFMAN)
	
clean:
	rm -fr diagnostics_how_to.dvi diagnostics_how_to.aux diagnostics_how_to.log
	rm -fr latex

distclean:
	make clean
	rm -fr diagnostics_how_to.pdf diagnostics_how_to.ps
	rm -fr xoopic-source-reference-html
	rm -f $(REFMAN)
	