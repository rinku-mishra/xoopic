#ifndef	__DIELECTR_H
#define	__DIELECTR_H

/*
====================================================================

dielectric.h

This class represents a general dielectric surface.

0.99	(NTG, 12-29-93) Separated into individual module from
		pic.h.
0.991	(JohnV 01-11-94) Add collect().
0.992 (JohnV 08-09-95) Add DielectricRegion
0.993 (JohnV 09-02-95) Move secondary params into setSecondary().
1.001	(JohnV 05-07-96) Add toNodes() for DielectricRegion.
1.1   (JohnV 10-09-96) Add reflection = fraction of particles reflected.
1.11  (JohnV 12-16-96) Add refMaxE = Energy ceiling for reflection.
2.0   (JohnV 08-19-97) Extend for multiple secondary packages.
2.1   (JohnV 06-15-98) Added Secondary::threshold and Eemit
2.2   (JohnV 09-01-98) Moved Secondary to secondary.h.
2.3   (JohnV 04-Mar-2002) Added transparency.

====================================================================
*/

#include "boundary.h"
class Secondary;

class Dielectric : public Boundary
{
protected:
   oopicList<Secondary> secondaryList;
   Vector2 _delta; // offset for secondary emission
   Scalar *Q;  // Charge accumulated on the dielectric
   int QuseFlag;  //boolean
   Scalar reflection; // fraction of particles to reflect
   Scalar refMaxE; // energy ceiling for reflection
   Scalar transparency; // fraction of particles which pass through
   int nPoints;	// number of elements in Q array
   Scalar epsilon; // the dielectric constant
public:
   Dielectric(oopicList <LineSegment> *segments, Scalar er, 
	      int QuseFlag=0, Scalar reflection=0, Scalar refMaxE=1e10);
   virtual ~Dielectric();
   virtual void addSecondary(Secondary* secondary);
   virtual Vector2 delta(Particle& p) {return _delta;};
   virtual ParticleList& emit(Scalar t,Scalar dt, Species* species) {return particleList;};
   virtual void setPassives();
   virtual void applyFields(Scalar t,Scalar dt) {};
   virtual void setQ(Scalar _QQ, int j) { Q[j]=_QQ; };
   virtual Scalar* getQ() { return Q; };
   virtual void putCharge_and_Current(Scalar t);
   virtual void putCharge(); 
   virtual void collect(Particle& p, Vector3& dxMKS);
   void setQuseFlag(int flag) { QuseFlag = flag; };
   void set_reflection(Scalar r) {reflection = r;}
   void set_refMaxE(Scalar energy) {refMaxE = energy;}
   void set_transparency(Scalar t) {transparency = t;}
   virtual void setFields(Fields &f);
   virtual int Restore_2_00(FILE* DMPFile, int j1, int k1, int j2, int k2);
#ifdef HAVE_HDF5
   virtual int dumpH5(dumpHDF5 &dumpObj,int number);
   int restoreH5(dumpHDF5 &restoreObj,int bType,string whichBoundary,int nQuads);
#endif

   int Restore(FILE *DMPFile, int BoundaryType,
	       Scalar A1,Scalar A2, Scalar B1, Scalar B2,int nQuads);
 
   virtual int Dump(FILE* DMPFile);
   virtual void toNodes() {Boundary::toNodes();}
};

//================================================================
//	DielectricRegion represents an ideal volume dielectric material

class DielectricRegion : public Dielectric
{
   enum EDGE {LEFT, RIGHT, BOTTOM, TOP};
   virtual void setPassives();
   void setEpsilon();
   void addQ(EDGE edge, int j, Scalar w, Scalar q);
   int getIndex(int j, int k);			// convert from j, k to index into Q
public:
   DielectricRegion(oopicList <LineSegment> *segments, Scalar er, 
		    int QuseFlag=1, Scalar reflection=0, Scalar refMaxE=1e10);
   virtual ~DielectricRegion();
   virtual void collect(Particle& p, Vector3& dxMKS);
#ifdef HAVE_HDF5
   virtual int dumpH5(dumpHDF5 &dumpObj,int number);
   int restoreH5(dumpHDF5 &restoreObj,int bType,string whichBoundary,int nQuads);
#endif
   virtual int Dump(FILE* DMPFile);
   virtual void putCharge_and_Current(Scalar t);
   virtual void putCharge();

 
   int Restore(FILE *DMPFile, int BoundaryType,
	       Scalar A1,Scalar A2, Scalar B1, Scalar B2,int nQuads);
   virtual int Restore_2_00(FILE* DMPFile, int j1, int k1, int j2, int k2);
   virtual void setBoundaryMask(Grid &grid);
   virtual void setFields(Fields &f) {Boundary::setFields(f); setEpsilon();};
   virtual void toNodes();
};

class DielectricTriangle : public Dielectric
{
  virtual void setPassives();
  void setEpsilon();
 public:
  DielectricTriangle(oopicList <LineSegment> *segments, Scalar _er,
		     int _QuseFlag=0);
  virtual void setFields(Fields &f) {Boundary::setFields(f); setEpsilon();};
  virtual void collect(Particle &p, Vector3& dxMKS);
  virtual void putCharge_and_Current(Scalar t) {};
  virtual void setBoundaryMask(Grid &grid);
  virtual int Restore_2_00(FILE* DMPFile, int j1, int k1, int j2, int k2){return 0;};
  virtual int Restore(FILE *DMPFile, int BoundaryType,
                 Scalar A1,Scalar A2, Scalar B1, Scalar B2,int nQuads) { 
#ifndef _MSC_VER
     cout << "Dump/Restore for dielectric triangles not implemented."; 
#endif
     return 0;
  };
 
  virtual int Dump(FILE* DMPFile);
};

#endif	//	ifndef __DIELECTR_H
