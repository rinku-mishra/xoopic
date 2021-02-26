//dielecg.cpp

#include "dielecg.h"
#include "secondary.h"
#include "spatialg.h"
#include "spbound.h"
#include "diags.h"
using namespace std;


///// Begin SecondaryGroup /////////////////////////

SecondaryGroup::SecondaryGroup(Scalar _secondary, Scalar _threshold, Scalar 
  _Eemit, ostring _secSpeciesName, ostring _iSpeciesName)
{
  secondary = _secondary; 
  threshold = _threshold; 
  Eemit = _Eemit; 
  secSpeciesName = _secSpeciesName; 
  iSpeciesName = _iSpeciesName;
}


void SecondaryGroup::checkRules(oopicList<ostring>& ruleMessages, 
				SpatialRegionGroup *spatialRegionGroup)
{


  if (secondary > 0) {
    if (secSpeciesName == (ostring)"NULL")
      ruleMessages.add(new ostring("secSpeciesName is required"));
    else if (!spatialRegionGroup->KnownSpecies(secSpeciesName)){
      char buffer[512];
      sprintf(buffer, "Unrecognized secSpeciesName: %s", secSpeciesName.c_str());
      ruleMessages.add(new ostring(buffer));
    }
    if (iSpeciesName == (ostring)"NULL")
      ruleMessages.add(new ostring("iSpeciesName is required when secondary > 0"));
    else if (!spatialRegionGroup->KnownSpecies(iSpeciesName)){
      char buffer[512];
      sprintf(buffer, "Unrecognized iSpeciesName: %s", iSpeciesName.c_str());
      ruleMessages.add(new ostring(buffer));
    }
  }

}


Secondary* SecondaryGroup::CreateCounterPart(SpatialRegionGroup* srgPtr)
{
  Species* secSpeciesPtr = srgPtr->get_speciesPtr(secSpeciesName);
  Species* iSpeciesPtr = srgPtr->get_speciesPtr(iSpeciesName);
  return new Secondary(secondary, threshold, Eemit, secSpeciesPtr, iSpeciesPtr);
}



///// Begin SecondaryVaughanGroup /////////////////////////

SecondaryVaughanGroup::SecondaryVaughanGroup(Scalar secondary, Scalar threshold, Scalar Eemit, ostring secSpeciesName, ostring iSpeciesName, 
                                             Scalar _fReflected, Scalar _fScattered, Scalar _energy_max0, Scalar _ks)
:  SecondaryGroup(secondary, threshold, Eemit, secSpeciesName, iSpeciesName)
{
  fReflected = _fReflected; 
  fScattered = _fScattered;
  energy_max0 = _energy_max0;
  ks = _ks;
}


Secondary* SecondaryVaughanGroup::CreateCounterPart(SpatialRegionGroup* srgPtr)
{
  Species* secSpeciesPtr = srgPtr->get_speciesPtr(secSpeciesName);
  Species* iSpeciesPtr = srgPtr->get_speciesPtr(iSpeciesName);
  return new Secondary_Vaughan(secondary, threshold, Eemit, secSpeciesPtr, iSpeciesPtr,
			fReflected, fScattered, energy_max0, ks);
}


SecondaryFurmanGroup::SecondaryFurmanGroup( Scalar _alpha, Scalar _P_1e_inf, Scalar _P_1e, Scalar _E_e, Scalar _W,  Scalar _p,
                                            Scalar _sigma_e, Scalar _e_1, Scalar _e_2, Scalar _P_1r_inf, Scalar _E_r,
                                            Scalar _r, Scalar _q, Scalar _r_1, Scalar _r_2, Scalar _delta_ts, Scalar _E_ts,
                                            Scalar _s, Scalar _t_1, Scalar _t_2, Scalar _t_3, Scalar _t_4, Scalar _E_t, Scalar _delta_t,
                                            Scalar _p_vec_1, Scalar _p_vec_2, Scalar _p_vec_3, Scalar _p_vec_4, Scalar _p_vec_5, 
                                            Scalar _p_vec_6, Scalar _p_vec_7, Scalar _p_vec_8, Scalar _p_vec_9, Scalar _p_vec_10,
                                            Scalar _eps_vec_1, Scalar _eps_vec_2, Scalar _eps_vec_3, Scalar _eps_vec_4, Scalar _eps_vec_5, 
                                            Scalar _eps_vec_6, Scalar _eps_vec_7, Scalar _eps_vec_8, Scalar _eps_vec_9, Scalar _eps_vec_10, 
                                            Scalar secondary, Scalar threshold, Scalar Eemit, ostring secSpeciesName, ostring iSpeciesName)
:  SecondaryGroup(secondary, threshold, Eemit, secSpeciesName, iSpeciesName)
{

  alpha = _alpha;
  P_1e_inf = _P_1e_inf;
  P_1e = _P_1e;
  E_e = _E_e;
  W = _W;
  p = _p;
  sigma_e = _sigma_e;
  e_1 = _e_1;
  e_2 = _e_2;
  P_1r_inf = _P_1r_inf;
  E_r = _E_r;
  r = _r;
  q = _q;
  r_1 = _r_1;
  r_2 = _r_2;
  delta_ts = _delta_ts;
  E_ts = _E_ts;
  s = _s;
  t_1 = _t_1;
  t_2 = _t_2;
  t_3 = _t_3;
  t_4 = _t_4;
  E_t = _E_t;


  delta_t = _delta_t;
  p_vec_1 = _p_vec_1;
  p_vec_2 = _p_vec_2;
  p_vec_3 = _p_vec_3;
  p_vec_4 = _p_vec_4;
  p_vec_5 = _p_vec_5;
  p_vec_6 = _p_vec_6;
  p_vec_7 = _p_vec_7;
  p_vec_8 = _p_vec_8;
  p_vec_9 = _p_vec_9;
  p_vec_10 = _p_vec_10;
  eps_vec_1 = _eps_vec_1;
  eps_vec_2 = _eps_vec_2;
  eps_vec_3 = _eps_vec_3;
  eps_vec_4 = _eps_vec_4;
  eps_vec_5 = _eps_vec_5;
  eps_vec_6 = _eps_vec_6;
  eps_vec_7 = _eps_vec_7;
  eps_vec_8 = _eps_vec_8;
  eps_vec_9 = _eps_vec_9;
  eps_vec_10 = _eps_vec_10;

}



///// Begin SecondaryFurmanGroup /////////////////////////

Secondary* SecondaryFurmanGroup::CreateCounterPart(SpatialRegionGroup* srgPtr)
{
  Species* secSpeciesPtr = srgPtr->get_speciesPtr(secSpeciesName);
  Species* iSpeciesPtr = srgPtr->get_speciesPtr(iSpeciesName);

/*
  int Furman_max_secondaries = 10;   // length of the vectors p_vec and eps_vec that were read-in
  Scalar* p_vec = new Scalar[Furman_max_secondaries];
  Scalar* eps_vec = new Scalar[Furman_max_secondaries];
  p_vec[0] = p_vec_1;
  p_vec[1] = p_vec_2;
  p_vec[2] = p_vec_3;
  p_vec[3] = p_vec_4;
  p_vec[4] = p_vec_5;
  p_vec[5] = p_vec_6;
  p_vec[6] = p_vec_7;
  p_vec[7] = p_vec_8;
  p_vec[8] = p_vec_9;
  p_vec[9] = p_vec_10;
  eps_vec[0] = eps_vec_1;
  eps_vec[1] = eps_vec_1;
  eps_vec[2] = eps_vec_1;
  eps_vec[3] = eps_vec_1;
  eps_vec[4] = eps_vec_1;
  eps_vec[5] = eps_vec_1;
  eps_vec[6] = eps_vec_1;
  eps_vec[7] = eps_vec_1;
  eps_vec[8] = eps_vec_1;
  eps_vec[9] = eps_vec_1;
*/

  Scalar p_vec[] = {p_vec_1, p_vec_2, p_vec_3, p_vec_4, p_vec_5, p_vec_6, p_vec_7, p_vec_8, p_vec_9, p_vec_10};
  Scalar eps_vec[] = {eps_vec_1, eps_vec_2, eps_vec_3, eps_vec_4, eps_vec_5, eps_vec_6, eps_vec_7, eps_vec_8, eps_vec_9, eps_vec_10}; 

  return new Secondary_Furman(alpha, P_1e_inf,  P_1e,  E_e,  W,  p,  sigma_e,  e_1,  e_2, P_1r_inf,  E_r,  r,  q,  r_1,  r_2, delta_ts,  E_ts,
                              s, t_1,  t_2,  t_3,  t_4,  E_t,  delta_t, p_vec, eps_vec, secondary, threshold, Eemit, secSpeciesPtr, iSpeciesPtr);
}



///// Begin SecondaryParams /////////////////////////

SecondaryParams::SecondaryParams()
{
  name = "Secondary";

  secondary.setNameAndDescription("secondary", "");  // reference param.h line 86
  secondary.setValue("0");
  parameterList.add(&secondary);  // list of all parameters that can be read from input file 
 // each data object such as secondary, threshold, etc. must be added to the list to be parsed 
 // value will be stored in secondary.value, which is presumed private
 // value can be retrieved by (Scalar datatype) = secondary.getValue() 
 // parameterList is a member of class ParameterGroup, reference part1g.h line 14  
 // SecondaryParams is a derived class of ParameterGroup, reference dielecg.h line 75 

  threshold.setNameAndDescription("threshold","");
  threshold.setValue("0.5");
  parameterList.add(&threshold);

  Eemit.setNameAndDescription("Eemit","");
  Eemit.setValue("2");
  parameterList.add(&Eemit);
  
  secSpeciesName.setNameAndDescription("secSpecies","");
  secSpeciesName.setValue("NULL");
  parameterList.add(&secSpeciesName);

  iSpeciesName.setNameAndDescription("iSpecies","");
  iSpeciesName.setValue("NULL");
  parameterList.add(&iSpeciesName);
}


ostring SecondaryParams::InitializeFromStream(std::ifstream &fin)
{
  ostring msg = ParameterGroup::InitializeFromStream(fin);  // reference part1g.h, line 87 and part1g.cpp, line 159 
  boundary->addSecondaryGroup(new SecondaryGroup(secondary.getValue(), 
    threshold.getValue(), Eemit.getValue(), secSpeciesName.getValue(), 
    iSpeciesName.getValue()));
  return msg;
}



///// Begin SecondaryVaughanParams /////////////////////////

SecondaryVaughanParams::SecondaryVaughanParams() : SecondaryParams()
{
  name = "Secondary_Vaughan";

  fReflected.setNameAndDescription("fReflected","");
  fReflected.setValue("0");
  parameterList.add(&fReflected);

  fScattered.setNameAndDescription("fScattered","");
  fScattered.setValue("0");
  parameterList.add(&fScattered);

  energy_max0.setNameAndDescription("energy_max0","");
  energy_max0.setValue("0");
  parameterList.add(&energy_max0);

  ks.setNameAndDescription("ks","");
  ks.setValue("1");
  parameterList.add(&ks);
}


ostring SecondaryVaughanParams::InitializeFromStream(ifstream &fin)  // I will need to add a function like this for my secondary models
{
  ostring msg = ParameterGroup::InitializeFromStream(fin);  // this reads in the input file section for the secondary model parameters 
  // the output message is in case of error 

  boundary->addSecondaryGroup(new SecondaryVaughanGroup(secondary.getValue(), 
    threshold.getValue(), Eemit.getValue(), secSpeciesName.getValue(), 
    iSpeciesName.getValue(), fReflected.getValue(), fScattered.getValue(), /*energy_w.getValue(),*/
    energy_max0.getValue(), ks.getValue()));
  // boundary is a pointer, we don't know what kind of boundary at this point (dielectric, conductor, equipotential, etc.)
  // boundary* object contains an addSecondaryGroup member function 

  // STACK = temporary, heap = permanent memory; the above "new" statement puts the Secondary2Group object in the heap, not the stack  

  return msg;
}



///// Begin SecondaryFurmanParams /////////////////////////

SecondaryFurmanParams::SecondaryFurmanParams() : SecondaryParams()  // This section will need to be updated for Furman's model
{
  name = "Secondary_Furman";

  // The following parameters will need to be read in:

  //  alpha = 1;
  alpha.setNameAndDescription("alpha","");
  alpha.setValue("1");
  parameterList.add(&alpha);

  //  P_1e_inf = 0.02;
  P_1e_inf.setNameAndDescription("P_1e_inf","");
  P_1e_inf.setValue("0");
  parameterList.add(&P_1e_inf);

  //  P_1e = 0.496;
  P_1e.setNameAndDescription("P_1e","");
  P_1e.setValue("0");
  parameterList.add(&P_1e);

  //  E_e = 0;
  E_e.setNameAndDescription("E_e","");
  E_e.setValue("0");
  parameterList.add(&E_e);

  //  W = 60.86;  
  W.setNameAndDescription("W","");
  W.setValue("0");
  parameterList.add(&W);

  //  p = 1;
  p.setNameAndDescription("p","");
  p.setValue("0");
  parameterList.add(&p);

  //  sigma_e = 2;
  sigma_e.setNameAndDescription("sigma_e","");
  sigma_e.setValue("0");
  parameterList.add(&sigma_e);

  //  e_1 = 0.26;
  e_1.setNameAndDescription("e_1","");
  e_1.setValue("0");
  parameterList.add(&e_1);

  //  e_2 = 2;
  e_2.setNameAndDescription("e_2","");
  e_2.setValue("0");
  parameterList.add(&e_2);

  //  P_1r_inf = 0.2;
  P_1r_inf.setNameAndDescription("P_1r_inf","");
  P_1r_inf.setValue("0");
  parameterList.add(&P_1r_inf);

  //  E_r = 0.041;
  E_r.setNameAndDescription("E_r","");
  E_r.setValue("0");
  parameterList.add(&E_r);

  //  r = 0.104;
  r.setNameAndDescription("r","");
  r.setValue("0");
  parameterList.add(&r);

  //  q = 0.5;
  q.setNameAndDescription("q","");
  q.setValue("0");
  parameterList.add(&q);

  //  r_1 = 0.26;
  r_1.setNameAndDescription("r_1","");
  r_1.setValue("0");
  parameterList.add(&r_1);

  //  r_2 = 2;
  r_2.setNameAndDescription("r_2","");
  r_2.setValue("0");
  parameterList.add(&r_2);

  //  delta_ts = 1.8848;
  delta_ts.setNameAndDescription("delta_ts","");
  delta_ts.setValue("0");
  parameterList.add(&delta_ts);

  //  E_ts = 276.8;
  E_ts.setNameAndDescription("E_ts","");
  E_ts.setValue("0");
  parameterList.add(&E_ts);

  //  s = 1.54;
  s.setNameAndDescription("s","");
  s.setValue("0");
  parameterList.add(&s);

  //  t_1 = 0.66;
  t_1.setNameAndDescription("t_1","");
  t_1.setValue("0");
  parameterList.add(&t_1);

  //  t_2 = 0.8;
  t_2.setNameAndDescription("t_2","");
  t_2.setValue("0");
  parameterList.add(&t_2);

  //  t_3 = 0.7;
  t_3.setNameAndDescription("t_3","");
  t_3.setValue("0");
  parameterList.add(&t_3);

  //  t_4 = 1;
  t_4.setNameAndDescription("t_4","");
  t_4.setValue("0");
  parameterList.add(&t_4);

  //  E_t = 271;
  E_t.setNameAndDescription("E_t","");
  E_t.setValue("0");
  parameterList.add(&E_t);

  //  delta_t = 2.1;
  delta_t.setNameAndDescription("delta_t","");
  delta_t.setValue("0");
  parameterList.add(&delta_t);

  //  p_vec(1) = 2.5;
  p_vec_1.setNameAndDescription("p_vec_1","");
  p_vec_1.setValue("0");
  parameterList.add(&p_vec_1);

  //  p_vec(2) = 3.3;
  p_vec_2.setNameAndDescription("p_vec_2","");
  p_vec_2.setValue("0");
  parameterList.add(&p_vec_2);

  //  p_vec(3) = 2.5;
  p_vec_3.setNameAndDescription("p_vec_3","");
  p_vec_3.setValue("0");
  parameterList.add(&p_vec_3);

  //  p_vec(4) = 2.5;
  p_vec_4.setNameAndDescription("p_vec_4","");
  p_vec_4.setValue("0");
  parameterList.add(&p_vec_4);

  //  p_vec(5) = 2.8;
  p_vec_5.setNameAndDescription("p_vec_5","");
  p_vec_5.setValue("0");
  parameterList.add(&p_vec_5);

  //  p_vec(6) = 1.3;
  p_vec_6.setNameAndDescription("p_vec_6","");
  p_vec_6.setValue("0");
  parameterList.add(&p_vec_6);

  //  p_vec(7) = 1.5;
  p_vec_7.setNameAndDescription("p_vec_7","");
  p_vec_7.setValue("0");
  parameterList.add(&p_vec_7);

  //  p_vec(8) = 1.5;
  p_vec_8.setNameAndDescription("p_vec_8","");
  p_vec_8.setValue("0");
  parameterList.add(&p_vec_8);

  //  p_vec(9) = 1.5;
  p_vec_9.setNameAndDescription("p_vec_9","");
  p_vec_9.setValue("0");
  parameterList.add(&p_vec_9);

  //  p_vec(10) = 1.5;
  p_vec_10.setNameAndDescription("p_vec_10","");
  p_vec_10.setValue("0");
  parameterList.add(&p_vec_10);

  //  eps_vec(1) = 1.5;
  eps_vec_1.setNameAndDescription("eps_vec_1","");
  eps_vec_1.setValue("0");
  parameterList.add(&eps_vec_1);

  //  eps_vec(2) = 1.75;
  eps_vec_2.setNameAndDescription("eps_vec_2","");
  eps_vec_2.setValue("0");
  parameterList.add(&eps_vec_2);

  //  eps_vec(3) = 1;
  eps_vec_3.setNameAndDescription("eps_vec_3","");
  eps_vec_3.setValue("0");
  parameterList.add(&eps_vec_3);

  //  eps_vec(4) = 1.375;
  eps_vec_4.setNameAndDescription("eps_vec_4","");
  eps_vec_4.setValue("0");
  parameterList.add(&eps_vec_4);

  //  eps_vec(5) = 8.5;
  eps_vec_5.setNameAndDescription("eps_vec_5","");
  eps_vec_5.setValue("0");
  parameterList.add(&eps_vec_5);

  //  eps_vec(6) = 11.5;
  eps_vec_6.setNameAndDescription("eps_vec_6","");
  eps_vec_6.setValue("0");
  parameterList.add(&eps_vec_6);

  //  eps_vec(7) = 2.5;
  eps_vec_7.setNameAndDescription("eps_vec_7","");
  eps_vec_7.setValue("0");
  parameterList.add(&eps_vec_7);

  //  eps_vec(8) = 3;
  eps_vec_8.setNameAndDescription("eps_vec_8","");
  eps_vec_8.setValue("0");
  parameterList.add(&eps_vec_8);

  //  eps_vec(9) = 2.5;
  eps_vec_9.setNameAndDescription("eps_vec_9","");
  eps_vec_9.setValue("0");
  parameterList.add(&eps_vec_9);

  //  eps_vec(10) = 3;
  eps_vec_10.setNameAndDescription("eps_vec_10","");
  eps_vec_10.setValue("0");
  parameterList.add(&eps_vec_10);
}


ostring SecondaryFurmanParams::InitializeFromStream(ifstream &fin)  // This section will need to be updated for Furman's model
{
  ostring msg = ParameterGroup::InitializeFromStream(fin);  // this reads in the input file section for the secondary model parameters 
  // the output message is in case of error 

  boundary->addSecondaryGroup(
              new SecondaryFurmanGroup(
					alpha.getValue(),
					P_1e_inf.getValue(),
					P_1e.getValue(),
					E_e.getValue(),
					W.getValue(),  
					p.getValue(),
					sigma_e.getValue(), 
					e_1.getValue(),
					e_2.getValue(), 
					P_1r_inf.getValue(),
					E_r.getValue(),
					r.getValue(),
					q.getValue(), 
					r_1.getValue(), 
					r_2.getValue(), 
					delta_ts.getValue(), 
					E_ts.getValue(),
					s.getValue(), 
					t_1.getValue(), 
					t_2.getValue(), 
					t_3.getValue(), 
					t_4.getValue(),
					E_t.getValue(), 
					delta_t.getValue(),
					p_vec_1.getValue(), 
					p_vec_2.getValue(),
					p_vec_3.getValue(), 
					p_vec_4.getValue(), 
					p_vec_5.getValue(), 
					p_vec_6.getValue(), 
					p_vec_7.getValue(),
					p_vec_8.getValue(), 
					p_vec_9.getValue(), 
					p_vec_10.getValue(),
					eps_vec_1.getValue(), 
					eps_vec_2.getValue(), 
					eps_vec_3.getValue(), 
					eps_vec_4.getValue(), 
					eps_vec_5.getValue(), 
					eps_vec_6.getValue(),
					eps_vec_7.getValue(), 
					eps_vec_8.getValue(),
					eps_vec_9.getValue(),
					eps_vec_10.getValue(), 
					secondary.getValue(), 
					threshold.getValue(), 
					Eemit.getValue(), 
					secSpeciesName.getValue(), 
					iSpeciesName.getValue()
                  )  // closing parenthesis for SecondaryFurmanGroup() call
            );       // closing parenthesis for addSecondaryGroup() call

  // STACK = temporary, heap = permanent memory; the above "new" statement puts the SecondaryFurmanGroup object in the heap, not the stack  

  return msg;
}




//=================== DielectricParams Class
DielectricParams::DielectricParams(GridParams* _GP,SpatialRegionGroup *srg) 
  : BoundaryParams(_GP,srg)
{
  name = "Dielectric";

  parameterGroupList.add(&secondary_basic_params); // make this an eligible subgroup
  secondary_basic_params.setBoundary(this);

  parameterGroupList.add(&secondary_vaughan_params);
  secondary_vaughan_params.setBoundary(this);

  parameterGroupList.add(&secondary_furman_params);
  secondary_furman_params.setBoundary(this);

/////////////////////  NOTHING BELOW THIS LINE SHOULD NEED TO BE MODIFIED //////////////////////////////////////////////////



  er.setNameAndDescription("er","Relative dielectric constant");
  er.setValue(ostring("1.0"));
  parameterList.add(&er);

  QuseFlag.setNameAndDescription("QuseFlag","Relative dielectric constant");
  QuseFlag.setValue("1");
  parameterList.add(&QuseFlag);

  reflection.setNameAndDescription("reflection", "reflected particle fraction");
  reflection.setValue("0");
  parameterList.add(&reflection);
  addLimitRule("reflection", "<", 0.0, "Fatal  -- 0 <= reflection <= 1", 1);
  addLimitRule("reflection", ">", 1.0, "Fatal  -- 0 <= reflection <= 1", 1);

  refMaxE.setNameAndDescription("refMaxE", "reflect for energy < refMaxE");
  refMaxE.setValue("1e10");
  parameterList.add(&refMaxE);

  transparency.setNameAndDescription("transparency","");
  transparency.setValue("0");
  parameterList.add(&transparency);
  addLimitRule("transparency", "<", 0.0, "Fatal  -- 0 <= transparency <= 1", 1);
  addLimitRule("transparency", ">", 1.0, "Fatal  -- 0 <= transparency <= 1", 1);
}

DielectricParams::~DielectricParams()
{
  secondaryGroupList.deleteAll();
}

void DielectricParams::addSecondaryGroup(SecondaryGroup* sGroup)
{
  secondaryGroupList.add(sGroup);
}

Boundary* DielectricParams::CreateCounterPart()
{
   Boundary* B = new Dielectric(SP.GetLineSegments(), er.getValue(),
                                QuseFlag.getValue());
   set_commonParams(B);
   return B;
}

void DielectricParams::set_commonParams(Boundary *B)
{
   Dielectric* D = (Dielectric*)B;
   D->set_reflection(reflection.getValue());
   D->set_refMaxE(refMaxE.getValue());
   D->set_transparency(transparency.getValue());
   addSecondaries(D);
}

// addSecondaries is provided as a function for descendants to call

void DielectricParams::addSecondaries(Boundary* B)
{
	oopicListIter<SecondaryGroup> sIter(secondaryGroupList);
	for (sIter.restart(); !sIter.Done(); sIter++)
	  B->addSecondary(sIter()->CreateCounterPart(spatialRegionGroup));
}

void DielectricParams::checkRules()
{
  BoundaryParams::checkRules();
  checkSecondaryRules();
}

void DielectricParams::checkSecondaryRules()
{
  oopicListIter<SecondaryGroup> sIter(secondaryGroupList);
  for (sIter.restart(); !sIter.Done(); sIter++)
	 sIter()->checkRules(ruleMessages, spatialRegionGroup);
}

//------------------------------------------------------------
//	DielectricRegionParams

DielectricRegionParams::DielectricRegionParams(GridParams *GP,
	SpatialRegionGroup *srg) : DielectricParams(GP, srg)
{
	name = "DielectricRegion";
}

Boundary* DielectricRegionParams::CreateCounterPart()
{
  Boundary* B = new DielectricRegion(SP.GetLineSegments(),
    er.getValue(), QuseFlag.getValue(), reflection.getValue());
  set_commonParams(B);
  return B;
}

void DielectricRegionParams::checkRules()
{
	// Rules from BoundaryParams
	ostring result = OnGridConstraint();
	if (strlen(result.c_str()) > 0) ruleMessages.add(new ostring(result));
	// Loop thru rules
	oopicListIter<RuleBase> nR(RuleList);
	oopicListIter<BaseParameter> nP(parameterList);
	for (nR.restart(); !nR.Done(); nR++)
	// Loop thru parameters and set parameter values required by rule
	{
		for (nP.restart(); !nP.Done(); nP++)
			nR.current()->setRuleVariable(nP.current()->getName(),
				nP.current()->getValueAsDouble());
		// Check the rule
		ostring result = nR.current()->checkRule();
		if (strlen(result.c_str()) > 0) ruleMessages.add(new ostring(result));
	}
	checkSecondaryRules();
}


Boundary* DielectricTriangleParams::CreateCounterPart()
{
	Boundary* B = new DielectricTriangle(SP.GetLineSegments(),
													 er.getValue(),QuseFlag.getValue());
	set_commonParams(B);
	return B;
}


// This overrides the boundg version, which doesn't do the proper windowing
// needed for a dielectric region split into SRB's.
void DielectricRegionParams::makeSegmentFitGrid(LineSegment *S, Grid *G) {
#ifndef MPI_VERSION
  // later on we expect coordinates in MKS units.  Convert them
  // here
  if(S->points==(int *)1) {
	 S->points=0;
	 S->A.set_e1(G->getX()[(int)S->A.e1()][(int)S->A.e2()].e1());
	 S->A.set_e2(G->getX()[(int)S->A.e1()][(int)S->A.e2()].e2());
	 S->B.set_e1(G->getX()[(int)S->B.e1()][(int)S->B.e2()].e1());
	 S->B.set_e2(G->getX()[(int)S->B.e1()][(int)S->B.e2()].e2());
  }


#endif // ndef MPI_VERSION  
#ifdef MPI_VERSION
//  Here's where we fix up the j's and k's or A's and B's to fit
//  into the grid window we're trying to create.  An annoying thing
//  but necessary.
//  I basically have to clip the "lines" defined by the coordinates
//  given.

  if(S->points==(int *)1) { // we're working in real-world coordinates 
	 // easier to work in MKS units here
	 // assume a UNIFORM mesh for autopartitioning.
	 // transform j1, j2, k1, k2 into MKS units.
	 S->A = Vector2((G->MaxMPIx1-G->MinMPIx1)*S->A.e1()/(Scalar)G->MaxMPIJ,
						 (G->MaxMPIx2-G->MinMPIx2)*S->A.e2()/(Scalar)G->MaxMPIK);
	 S->B = Vector2((G->MaxMPIx1-G->MinMPIx1)*S->B.e1()/(Scalar)G->MaxMPIJ,
						 (G->MaxMPIx2-G->MinMPIx2)*S->B.e2()/(Scalar)G->MaxMPIK);

/*		A1.setValue((G->MaxMPIx1-G->MinMPIx1)*j1.getValue()/(Scalar)G->MaxMPIJ);
	 B1.setValue((G->MaxMPIx1-G->MinMPIx1)*j2.getValue()/(Scalar)G->MaxMPIJ);
	 A2.setValue((G->MaxMPIx2-G->MinMPIx2)*k1.getValue()/(Scalar)G->MaxMPIK);
	 B2.setValue((G->MaxMPIx2-G->MinMPIx2)*k2.getValue()/(Scalar)G->MaxMPIK);
*/
  }
  Scalar y,m,x,b;  // linear parameters
  Scalar dx2,dy2;  // need these because floating point isn't exact.
  Scalar x1min = G->getX()[0][0].e1();
  Scalar x2min = G->getX()[0][0].e2();
  Scalar x1max = G->getX()[G->getJ()][0].e1();
  Scalar x2max = G->getX()[0][G->getK()].e2();
  dx2 = G->getX()[1][0].e1()-G->getX()[0][0].e1();
  dx2 /=2.01; //  so that rounding will occur properly later.
  dy2 = G->getX()[0][1].e2()-G->getX()[0][0].e2();
  dy2 /=2.01; //  so that rounding will occur properly later.
  Scalar a1,a2,b1,b2,t1,t2;
  a1 = S->A.e1(); a2 = S->A.e2(); b1 = S->B.e1();b2 = S->B.e2();
  if(a1 > b1 ) { // swap the points
	 t1 = a1; t2 = a2;
	 a1 = b1; a2 = b2;
	 b1 = t1; b2 = t2;
  }
  //  discard any boundaries completely out of bounds.
  if( (a1 < x1min - dx2 && b1 < x1min - dx2 ) || ( b1 > x1max + dx2 && a1 > x1max + dx2)) 
	 {
		S->A.set_e1(-1);
		return;
	 }
  
  if(a1 < x1min )  // out of bounds, need to squish it....
	 {
		a1 = x1min;
	 }
  if(b1 > x1max ) // out of bounds...
	 {
		b1 = x1max;
	 }
  //  discard any boundaries completely out of bounds.
  if( (a2 < x2min - dy2&& b2 < x2min - dy2) || ( b2 > x2max + dx2 && a2 > x2max + dx2)) 
	 {
		S->A.set_e1(-1);
		return;
	 }

  if(a2 > b2 ) { // swap the points
	 t1 = a1; t2 = a2;
	 a1 = b1; a2 = b2;
	 b1 = t1; b2 = t2;
  }
  
  if(a2 < x2min) // out of bounds, need to squish it.....
	 {
		a2 = x2min;
	 }

  if(b2 > x2max) // out of bounds, need to squish it.....
	 {
		b2 = x2max;
	 }
  if( (a2 < x2min - dy2&& b2 < x2min - dy2) || ( b2 > x2max + dx2 && a2 > x2max + dx2)) 
	 {
		S->A.set_e1(-1);
		return;
	 }

  // Hopefully, we now have a normalized, windowed line, write it back.
  S->A.set_e1(a1); S->A.set_e2(a2); S->B.set_e1(b1); S->B.set_e2(b2);   

#endif  /* MPI_VERSION */



}
