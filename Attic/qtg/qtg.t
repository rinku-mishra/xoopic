#! Use the common Unix template
#$ IncludeTemplate("recproj.t");


LFLAG_STATIC =

#######  Additional targets


SRCMOC    = moc_ButtonList.cpp \
	moc_DiagnosticList.cpp \
	moc_Information.cpp \
	moc_TxqPlot.cpp \
	moc_TxqPlotBorder.cpp \
	moc_TxqPlotWindow.cpp \
	moc_TxqPlot3DWindow.cpp \
	moc_GLPlot3D.cpp \
	moc_ButtonCmds.cpp \
	moc_MainButtons.cpp

OBJMOC	  = moc_ButtonList.o \
	moc_DiagnosticList.o \
	moc_Information.o \
	moc_TxqPlot.o \
	moc_TxqPlotBorder.o \
	moc_TxqPlotWindow.o \
	moc_TxqPlot3DWindow.o \
	moc_GLPlot3D.o \
	moc_MainButtons.o \
	moc_ButtonCmds.o


#
## Dependencies
#

evaluator.tab.c:	evaluator.y
	bison evaluator.y

moc_ButtonList.cpp: ButtonList.h
	$(MOC) ButtonList.h -o moc_ButtonList.cpp

moc_MainButtons.cpp: MainButtons.h
	$(MOC) MainButtons.h -o moc_MainButtons.cpp

moc_DiagnosticList.cpp: DiagnosticList.h
	$(MOC) DiagnosticList.h -o moc_DiagnosticList.cpp

moc_Information.cpp: Information.h
	$(MOC) Information.h -o moc_Information.cpp


moc_CreateWindows.cpp: CreateWindows.h
	$(MOC) CreateWindows.h -o moc_CreateWindows.cpp

moc_TxqPlot.o: moc_TxqPlot.cpp \
		TxqPlot.h

moc_TxqPlotBorder.o: moc_TxqPlotBorder.cpp \
		TxqPlotBorder.h TxqPlot.h

moc_TxqPlotWindow.o: moc_TxqPlotWindow.cpp \
		TxqPlotWindow.h TxqPlot.h

moc_TxqPlot3DWindow.o: moc_TxqPlot3DWindow.cpp \
		TxqPlot3DWindow.h 

moc_GLPlot3D.o: moc_GLPlot3D.cpp \
		GLPlot3D.h 

moc_TxqPlot.cpp: TxqPlot.h
	$(MOC) TxqPlot.h -o moc_TxqPlot.cpp
 
moc_TxqPlotBorder.cpp: TxqPlotBorder.h
	$(MOC) TxqPlotBorder.h -o moc_TxqPlotBorder.cpp

moc_TxqPlotWindow.cpp: TxqPlotWindow.h TxqPlotBorder.h
	$(MOC) TxqPlotWindow.h -o moc_TxqPlotWindow.cpp

moc_TxqPlot3DWindow.cpp: TxqPlot3DWindow.h 
	$(MOC) TxqPlot3DWindow.h -o moc_TxqPlot3DWindow.cpp

moc_GLPlot3D.cpp: GLPlot3D.h 
	$(MOC) GLPlot3D.h -o moc_GLPlot3D.cpp

moc_ButtonCmds.cpp: ButtonCmds.h
	$(MOC) ButtonCmds.h -o moc_ButtonCmds.cpp

all: xoopic

xoopic:
	rm -f qtoopic
	ln -s xgmain qtoopic

clean:
	rm -f *.o $(SRCMOC)


