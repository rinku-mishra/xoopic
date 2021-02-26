XX=g++
CXXFLAGS=-pthread -g -fPIC -I$(UAL)/include -I$(UAL)/lowlevel -I$(UAL)/cppinterface/ -I/afs/ipp-garching.mpg.de/itm/switm/blitz/0.10/include/
LDFLAGS=-L$(UAL)/lib -lUALCPPInterface -lUALLowLevel -L/afs/ipp-garching.mpg.de/itm/switm/blitz/0.10/lib/ -lblitz

all: libxoopic_nocpo.a
#all: xoopic_nocpo.exe

xoopic_nocpo.exe: xoopic_nocpo.o
	$(CXX) $(LDFLAGS) -o $@ $^
 
libxoopic_nocpo.a: xoopic_nocpo.o
	ar -rvs $@ $^

xoopic_nocpo.o: xoopic_nocpo.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

clean:
	rm *.a *.o
