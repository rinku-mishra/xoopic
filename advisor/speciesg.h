/*
====================================================================

speciesg.h

Class definitions for the SpeciesParams class.  This class
instantiates a SpeciesParams object from input data.  The
list of these objects will instantiate a SpeciesList in the
physics kernel at runtime.

Revision/Author/Date
0.9	(JohnV 01-26-95)	Original code.
0.91	(JohnV 06-26-95)	SpeciesID -> StringParameter.
1.1   (JohnV 12-13-96)  Add supercycleIndex, rename speciesSub -> subcycleIndex.
1.2   (JohnV 01-08-97)  Added collisionModel to specify collision type by species.

====================================================================
*/

#ifndef	__SPECIESG_H
#define	__SPECIESG_H

#include "species.h"
#include "part1g.h"

class SpeciesParams : public ParameterGroup
{
	ScalarParameter q;	// charge in coulombs of physical particle
	ScalarParameter m;	//	mass in kg of physical particle
	ScalarParameter threshold;  // threshold for removal if the collisionModel is test, in eV.
	IntParameter particleLimit;  // max number of paricles in this species
	StringParameter speciesName;	// unique species name
	IntParameter subcycleIndex;   //sub cycle value
	IntParameter supercycleIndex;
	StringParameter collisionModel; // determines model for collisions
	StringParameter TAModel; // determines pair species for coulomb collisions
	IntParameter IntraTAModel; //determines whether intraspecies coulomb collisions occurs;
	IntParameter nanbu; //determines whether intraspecies coulomb collisions occurs;
	StringParameter coulomb; // determines pair species for coulomb collisions

	// Parameter for specifying that RMS diagnostics should be calculated and plotted
	// Bruhwiler and Dimitrov, 10/10/2000
	IntParameter rmsDiagnosticsFlag;
  
public:
	SpeciesParams();
	virtual ~SpeciesParams() {}
	Scalar get_q() {return q.getValue();}
	Scalar get_m() {return m.getValue();}
	Scalar get_threshold() {return threshold.getValue();}
	int get_particleLimit() {return particleLimit.getValue();}
	int get_rmsDiagnosticsFlag() {return rmsDiagnosticsFlag.getValue();}
	int get_subcycleIndex() {return subcycleIndex.getValue();}
	int get_supercycleIndex() {return supercycleIndex.getValue();}
	int get_IntraTAModel() {return IntraTAModel.getValue();}
	ostring get_speciesName() {return speciesName.getValue();}
	ostring get_TAModel() {return TAModel.getValue();}
	ostring get_coulomb() {return coulomb.getValue();}
	int get_nanbu() {return nanbu.getValue();}
//	CollisionModel get_collisionModel() {return (CollisionModel)collisionModel.getValue();}
	CollisionModel get_collisionModel();
	Species* createCounterPart();
};

typedef oopicList<SpeciesParams> SpeciesParamsList;

#endif	//	__SPECIESG_H



