/*
====================================================================

speciesg.cpp

Class definitions for the SpeciesParams class.  This class
instantiates a SpeciesParams object from input data.  The
list of these objects will instantiate a SpeciesList in the
physics kernel at runtime.

Revision/Author/Date
0.9	(JohnV 01-27-95)	Original code.
0.91	(JohnV 06-26-95)	Change speciesID -> speciesName
1.1   (JohnV 12-13-96)  Add supercycleIndex, rename speciesSub -> subcycleIndex.

====================================================================
*/

#include "speciesg.h"
#include "species.h"

SpeciesParams::SpeciesParams() : ParameterGroup()
{
	name = "Species";
	speciesName.setNameAndDescription("name", "unique name string");
	speciesName.setValue("NULL");
	parameterList.add(&speciesName);
	q.setNameAndDescription("q", "charge (C)");
	q.setValue("1.60206e-19");
	parameterList.add(&q);
	m.setNameAndDescription("m", "mass (kg)");
	m.setValue("9.1093897e-31");
	parameterList.add(&m);
	subcycleIndex.setNameAndDescription("subcycle", "subcycle index");
	subcycleIndex.setValue("1");
	parameterList.add(&subcycleIndex);
	supercycleIndex.setNameAndDescription("supercycle", "supercycle index");
	supercycleIndex.setValue("1");
	parameterList.add(&supercycleIndex);
	collisionModel.setNameAndDescription("collisionModel", 
      "model for collisions");
	collisionModel.setValue("none");
	parameterList.add(&collisionModel);
	threshold.setNameAndDescription("threshold", "threshold energy (eV)");
	threshold.setValue("0");
	parameterList.add(&threshold);

	particleLimit.setNameAndDescription("particleLimit", "This species particle limit");
	particleLimit.setValue("1e8");
	parameterList.add(&particleLimit);

	//setting species name of pair coulomb collision 
	TAModel.setNameAndDescription("TAModel", "pair species for inter species coulomb collisions");
	TAModel.setValue("none");
	parameterList.add(&TAModel);

	coulomb.setNameAndDescription("coulomb", "pair species nanbu coulomb collisions");
	coulomb.setValue("none");
	parameterList.add(&coulomb);

	//setting the default value of the intraspecies coulomb collision 
	nanbu.setNameAndDescription("nanbu",
	  "Flag = 1 for calculation of intraspecies coulomb collision;  Default value is 0.");
	nanbu.setValue("0");
	parameterList.add(&nanbu);

	//setting the default value of the intraspecies coulomb collision 
	IntraTAModel.setNameAndDescription("IntraTAModel",
	  "Flag = 1 for calculation of intraspecies coulomb collision;  Default value is 0.");
	IntraTAModel.setValue("0");
	parameterList.add(&IntraTAModel);

	// setting the default value of the rmsDiagnosticsFlag
	rmsDiagnosticsFlag.setNameAndDescription("rmsDiagnosticsFlag",
	  "Flag = 1 for calculation and plotting of RMS diagnostics;  Default value is 0.");
	rmsDiagnosticsFlag.setValue("0");
	parameterList.add(&rmsDiagnosticsFlag);
}

Species* SpeciesParams::createCounterPart()
{
   if (get_subcycleIndex() > 1) supercycleIndex.setValue("1");
	 
	 Species* species;
	 
         species =  new Species(get_speciesName(), get_q(), get_m(), get_subcycleIndex(),
				get_supercycleIndex(), get_collisionModel(), get_threshold(), 
				get_particleLimit(), get_nanbu(), get_coulomb(), get_TAModel(), get_IntraTAModel(), get_rmsDiagnosticsFlag() );

	 species->zeroKineticEnergy();

	 return species;

/*	return new Species(get_speciesName(), get_q(), get_m(), get_subcycleIndex(),
							 get_supercycleIndex(), get_collisionModel(), get_threshold(), 
										 get_particleLimit());
*/
}

CollisionModel SpeciesParams::get_collisionModel()
{
	CollisionModel model = none;
 
	if ((collisionModel.getValue()==(ostring)"electron")|(collisionModel.getValue()==(ostring)"1"))
		model = electron;
	else if ((collisionModel.getValue()==(ostring)"ion")|(collisionModel.getValue()==(ostring)"2"))
		model = ion;
	else if ((collisionModel.getValue()==(ostring)"test")|(collisionModel.getValue()==(ostring)"3"))
		model = test;
	else if ((collisionModel.getValue()==(ostring)"boltzmann")|(collisionModel.getValue()==(ostring)"4"))
		model = boltzmann;

	return (model);
}
