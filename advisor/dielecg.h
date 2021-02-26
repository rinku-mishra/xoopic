//dielecg.h

#ifndef   __DIELECG_H
#define   __DIELECG_H

#include "part1g.h"
#include "boundg.h"
#include "gridg.h"
#include "dielectr.h"

class DielectricParams;

//-----------------------------------------------
// SecondaryParams class: provides secondary emission capability
// this can occur multiple times per boundary, representing multiple 
// emission phenomena

class SecondaryGroup
{
 protected:
  Scalar secondary;
  Scalar threshold;
  Scalar Eemit;
  ostring secSpeciesName; // must retain names, Species may not be read in yet
  ostring iSpeciesName;
 public:
  SecondaryGroup(Scalar _secondary, Scalar _threshold, Scalar _Eemit, 
		 ostring _secSpeciesName, ostring _iSpeciesName);
  virtual ~SecondaryGroup() {}    //virtual destructor which does nothing
  void checkRules(oopicList<ostring> &ruleMessages, SpatialRegionGroup* srgPtr);
  virtual Secondary* CreateCounterPart(SpatialRegionGroup* srgPtr);
};

class SecondaryVaughanGroup : public SecondaryGroup
{
 protected:
  Scalar fReflected;
  Scalar fScattered;
  Scalar energy_max0;
  Scalar ks;
 public:
  SecondaryVaughanGroup(Scalar secondary, Scalar threshold, Scalar Eemit, ostring
		  secSpeciesName, ostring iSpeciesName, Scalar fReflected,
		  Scalar fScattered, Scalar energy_max0, Scalar ks);
  ~SecondaryVaughanGroup() {}
  virtual Secondary* CreateCounterPart(SpatialRegionGroup* srgPtr);
};


class SecondaryFurmanGroup : public SecondaryGroup 
{
 protected:
  // Emitted angular spectrum parameter (Furman section IIC1):
  Scalar alpha;

  // Backscattered electron parameters (Furman section IIIB):
  Scalar P_1e_inf;
  Scalar P_1e;
  Scalar E_e;
  Scalar W;  
  Scalar p;
  Scalar sigma_e;
  Scalar e_1;
  Scalar e_2;

  // Rediffused electron parameters (Furman section IIIC):
  Scalar P_1r_inf;
  Scalar E_r;
  Scalar r;
  Scalar q;
  Scalar r_1;
  Scalar r_2;

  // True secondary electron parameters (Furman section IIID):
  Scalar delta_ts;
  Scalar E_ts;
  Scalar s;
  Scalar t_1;
  Scalar t_2;
  Scalar t_3;
  Scalar t_4;

  // Total SEY parameters (Furman Table 1):
  Scalar E_t;
  Scalar delta_t;

  // Probability model parameters for true secondary electrons (Furman Table II):
  Scalar p_vec_1, p_vec_2, p_vec_3, p_vec_4, p_vec_5, p_vec_6, p_vec_7, p_vec_8, p_vec_9, p_vec_10;  // will become a vector of 10 elements
  Scalar eps_vec_1, eps_vec_2, eps_vec_3, eps_vec_4, eps_vec_5, eps_vec_6, eps_vec_7, eps_vec_8, eps_vec_9, eps_vec_10;  // will become a vector of 10 elements 

 public:

  SecondaryFurmanGroup(Scalar _alpha, Scalar _P_1e_inf, Scalar _P_1e, Scalar _E_e, Scalar _W,  Scalar _p,
                       Scalar _sigma_e, Scalar _e_1, Scalar _e_2, Scalar _P_1r_inf, Scalar _E_r,
                       Scalar _r, Scalar _q, Scalar _r_1, Scalar _r_2, Scalar _delta_ts, Scalar _E_ts,
                       Scalar _s, Scalar _t_1, Scalar _t_2, Scalar _t_3, Scalar _t_4, Scalar _E_t, Scalar _delta_t,
                       Scalar _p_vec_1, Scalar _p_vec_2, Scalar _p_vec_3, Scalar _p_vec_4, Scalar _p_vec_5, 
                       Scalar _p_vec_6, Scalar _p_vec_7, Scalar _p_vec_8, Scalar _p_vec_9, Scalar _p_vec_10,
                       Scalar _eps_vec_1, Scalar _eps_vec_2, Scalar _eps_vec_3, Scalar _eps_vec_4, Scalar _eps_vec_5, 
                       Scalar _eps_vec_6, Scalar _eps_vec_7, Scalar _eps_vec_8, Scalar _eps_vec_9, Scalar _eps_vec_10, 
                       Scalar secondary, Scalar threshold, Scalar Eemit, ostring secSpeciesName, ostring iSpeciesName);

  ~SecondaryFurmanGroup() {}

  virtual Secondary* CreateCounterPart(SpatialRegionGroup* srgPtr);
};


class SecondaryParams : public ParameterGroup
{
 protected:
  ScalarParameter secondary; //	secondary emission coeff.
  ScalarParameter threshold; // threshold for emission;
  ScalarParameter Eemit;     // max emission energy;
  StringParameter secSpeciesName; // secondary species to emit;
  StringParameter iSpeciesName;	// species which triggers sec;
  DielectricParams* boundary; // attach to boundary
 public:
  SecondaryParams();
  virtual ostring InitializeFromStream(std::ifstream &fin);
  void setBoundary(DielectricParams *_boundary) {boundary = _boundary;}
};

class SecondaryVaughanParams : public SecondaryParams  
{
 protected:
  ScalarParameter fReflected; // fraction of reflected primaries; - Furman's backscattered electrons
  ScalarParameter fScattered; // fraction of scattered primaries; - Furman's rediffused electrons
  ScalarParameter energy_max0; // energy at which sec. coeff = secondary;
  ScalarParameter ks; // smoothness, 0<=ks<=2, 2 is smoother;
 public:
  SecondaryVaughanParams();
  virtual ostring InitializeFromStream(std::ifstream &fin);
};


class SecondaryFurmanParams : public SecondaryParams    
{
 protected:
  // Emitted angular spectrum parameter (Furman section IIC1):
  ScalarParameter alpha;

  // Backscattered electron parameters (Furman section IIIB):
  ScalarParameter P_1e_inf;
  ScalarParameter P_1e;
  ScalarParameter E_e;
  ScalarParameter W;  
  ScalarParameter p;
  ScalarParameter sigma_e;
  ScalarParameter e_1;
  ScalarParameter e_2;

  // Rediffused electron parameters (Furman section IIIC):
  ScalarParameter P_1r_inf;
  ScalarParameter E_r;
  ScalarParameter r;
  ScalarParameter q;
  ScalarParameter r_1;
  ScalarParameter r_2;

  // True secondary electron parameters (Furman section IIID):
  ScalarParameter delta_ts;
  ScalarParameter E_ts;
  ScalarParameter s;
  ScalarParameter t_1;
  ScalarParameter t_2;
  ScalarParameter t_3;
  ScalarParameter t_4;

  // Total SEY parameters (Furman Table 1):
  ScalarParameter E_t;
  ScalarParameter delta_t;

  // Probability model parameters for true secondary electrons (Furman Table II):
  ScalarParameter p_vec_1, p_vec_2, p_vec_3, p_vec_4, p_vec_5, p_vec_6, p_vec_7, p_vec_8, p_vec_9, p_vec_10;  
  ScalarParameter eps_vec_1, eps_vec_2, eps_vec_3, eps_vec_4, eps_vec_5, eps_vec_6, eps_vec_7, eps_vec_8, eps_vec_9, eps_vec_10;  
  // The above lists of p_vec_n and eps_vec_n values will become vectors of 10 elements 

 public:
  SecondaryFurmanParams();
  virtual ostring InitializeFromStream(std::ifstream &fin);
};



//=================== DielectricParams Class

class DielectricParams : public BoundaryParams
{
protected:
  ScalarParameter er;
  IntParameter QuseFlag;
  ScalarParameter reflection; // reflected particle fraction
  ScalarParameter refMaxE; // reflect when E < refMaxE
  ScalarParameter transparency;
  SecondaryParams secondary_basic_params;
  SecondaryVaughanParams secondary_vaughan_params;
  SecondaryFurmanParams secondary_furman_params;
  oopicList<SecondaryGroup> secondaryGroupList;	
public:
  DielectricParams(GridParams *GP, SpatialRegionGroup* srg);
  virtual ~DielectricParams();
  void set_commonParams(Boundary* B);
  virtual void addSecondaries(Boundary* B);
  virtual void addSecondaryGroup(SecondaryGroup* sGroup);
  virtual void checkRules();
  virtual void checkSecondaryRules();
  virtual Boundary* CreateCounterPart();
};

//=================== DielectricRegionParams Class
//	Creates a region filled with a dielectric material
// (j1,k1), (j2,k2) specifies a rectangular region, not a line segment.

class DielectricRegionParams : public DielectricParams
{
public:
	DielectricRegionParams(GridParams *GP, SpatialRegionGroup *srg);
	virtual void checkRules();
	virtual Boundary* CreateCounterPart();
	virtual void makeSegmentFitGrid(LineSegment *S, Grid *G);
};

//=================== DielectricTriangle Class
//	Creates a triangular region filled with a dielectric material
// (j1,k1), (j2,k2) specifies a rectangular region, not a line segment.
// if the normal > 0, the region above the defined boundary is filled
// with dielectric, the opposite is true if normal < 0.

class DielectricTriangleParams : public DielectricParams
{
public:
	DielectricTriangleParams(GridParams *GP, SpatialRegionGroup *srg) 
: DielectricParams(GP,srg) {};
	virtual Boundary* CreateCounterPart();
};

#endif  //  __DIELECG_H
