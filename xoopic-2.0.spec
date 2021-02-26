Summary:  EM and ES 2d 3v Particle-In-Cell Plasma Simulation in rz and xy
Name:  xoopic
Version: 2.0
Release: 1
Copyright: Berkeley, distributable, UC Regents
Group: Applications/Engineering
Source: xoopic-2.0.tar.gz
%description

  XOOPIC is a two-dimensional 3-velocity particle in cell simulator.
It can handle electrostatic and electromagnetic models, has
a large variety of boundary conditions, supports multiple gasses
and gas chemisty, and is easily reconfigurable via an input file.

%prep

%build
make 

%install
install -s -m 755 -o 0 -g 0 xg/xoopic /usr/local/bin/xoopic
install -m 755 -o 0 -g 0 -d /usr/local/lib/xoopic
install -m 755 -o 0 -g 0 -d /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/RKO.base.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/RKO.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/ambipolar.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/cavity.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/cmaser.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/dc-NeXe.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/dcdis.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/default.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/dring.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/ebeam1a.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/ebeam1b.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/ebeam2.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/ebeam3.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/ebeam4.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/gas.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/hollow.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/klygap.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/klysimple.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/klystron.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/plsource.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/rzgrating.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/square_v.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/waveguid.inp /usr/local/lib/xoopic/input
install  -m 444 -o 0 -g 0 input/xybeam.inp /usr/local/lib/xoopic/input

%files 
/usr/local/bin/xoopic
/usr/local/lib/xoopic/input/RKO.base.inp
/usr/local/lib/xoopic/input/RKO.inp
/usr/local/lib/xoopic/input/ambipolar.inp
/usr/local/lib/xoopic/input/cavity.inp
/usr/local/lib/xoopic/input/cmaser.inp
/usr/local/lib/xoopic/input/dc-NeXe.inp
/usr/local/lib/xoopic/input/dcdis.inp
/usr/local/lib/xoopic/input/default.inp
/usr/local/lib/xoopic/input/dring.inp
/usr/local/lib/xoopic/input/ebeam1a.inp
/usr/local/lib/xoopic/input/ebeam1b.inp
/usr/local/lib/xoopic/input/ebeam2.inp
/usr/local/lib/xoopic/input/ebeam3.inp
/usr/local/lib/xoopic/input/ebeam4.inp
/usr/local/lib/xoopic/input/gas.inp
/usr/local/lib/xoopic/input/hollow.inp
/usr/local/lib/xoopic/input/klygap.inp
/usr/local/lib/xoopic/input/klysimple.inp
/usr/local/lib/xoopic/input/klystron.inp
/usr/local/lib/xoopic/input/plsource.inp
/usr/local/lib/xoopic/input/rzgrating.inp
/usr/local/lib/xoopic/input/square_v.inp
/usr/local/lib/xoopic/input/waveguid.inp
/usr/local/lib/xoopic/input/xybeam.inp


%doc README COPYRIGHT CHANGES BUGS INPUT_CHANGES doc input



