/*
====================================================================

species.h

	The Species class describes the characteristics of a particle 
	species, including charge q and mass m.

Revision History
0.9	(JohnV 01-25-95)	Original version.
0.91	(JohnV 06-26-95)	Add name, make internal counter idCOunt for speciesID.
1.0	(JohnV 01-18-96)	Modified subcycling code originally by CooperD.
1.1   (JohnV 12-13-96)  Added supercycleIndex for supercycling (push faster than 
      field solve).
1.11  (JohnV 01-02-97)  Added collisionModel, eCreate, iCreate for more flexible
      mcc package.

====================================================================
*/

#ifndef	__SPECIES_H
#define	__SPECIES_H

#define NR_U2_LIMIT 1.E15
#define NR_V2_LIMIT 9.89E14

#include "oopiclist.h"
#include "ovector.h"
#include "ostring.h"
#include "misc.h"

enum CollisionModel {none, electron, ion, test, boltzmann};  //test = boltzmann test particles
                                                             //boltzmann = boltzmann electrons
class Species
{
	static int idCount;						//	internal counter for unique id
													//	This is initialized = 0 in misc.cpp
	int speciesID;								//	unique id for this species
	ostring name;								//	unique name for this species
	Scalar q_over_m;							//	charge (C) / mass (kg)
	Scalar m;                 // mass (kg) 
	Scalar q;									//	charge in C
	Scalar threshold;        // threshold for test collisionModel (eV)
	int particleLimit;

	ostring TAModel;
	ostring coulomb;
	int IntraTAModel;
	int nanbu;

	///*********************************************************///
	/// (Bruhwiler/Dimitrov, October 2000)
	//  Flag specifying that RMS beam sizes should be plotted.
  	//  Default value is 0 (no plotting); =1 for plotting.
	int rmsDiagnosticsFlag; 
	///*********************************************************///
	int subcycleIndex;						// formerly speciesSub
	int subcycleCount;						// formerly speciesSubIndex
	int supercycleIndex;                // push faster than timestep
	CollisionModel collisionModel;      // electron, ion, etc
	Scalar KineticEnergy;       // Kinetic energy of all the particles (MKS)
	Scalar temper;                //temperature
public:
	Species(ostring _name, Scalar _q, Scalar _m, int subIndex, int superIndex, 
		CollisionModel cm, Scalar _threshold, int _particleLimit, int _nanbu, ostring _coulomb, ostring _TAModel, int _IntraTAModel,
		int _rmsDiagnosticsFlag) 
	  {
	    name = _name,	q = _q, q_over_m = _q/_m, m=_m, subcycleIndex = subIndex; 
	    supercycleIndex = superIndex; collisionModel = cm; speciesID = idCount++;	
	    subcycleCount = MAX(subIndex/2,1); threshold = _threshold; 
	    particleLimit = _particleLimit; nanbu=_nanbu, coulomb=_coulomb, TAModel=_TAModel, IntraTAModel=_IntraTAModel, rmsDiagnosticsFlag = _rmsDiagnosticsFlag;
	  }

	~Species(){
	}

	BOOL cycleNow() {return ((collisionModel==boltzmann) ? FALSE : (subcycleCount >= subcycleIndex));}
	CollisionModel get_collisionModel() {return collisionModel;}
	Scalar get_threshold() {return threshold;}
	int get_particleLimit() {return particleLimit;}
	int get_IntraTAModel() {return IntraTAModel;}
	int get_rmsDiagnosticsFlag() {return rmsDiagnosticsFlag;}
	Scalar get_q() {return q;}
	Scalar get_m() {return m;}
	int get_subcycleIndex() {return subcycleIndex;}
	BOOL isSubcycled() {return (subcycleIndex > 1);}
	void incSubcycleCount() {subcycleCount++;}
	void resetSubcycleCount() {subcycleCount = 1;}
	int get_supercycleIndex() {return supercycleIndex;}
	Scalar get_q_over_m() {return q_over_m;}
	int getID() {return speciesID;}
	ostring get_name() {return name;}
	ostring get_coulomb() {return coulomb;}
	int get_nanbu() {return nanbu;}
	ostring get_TAModel() {return TAModel;}
	
	int is_coulomb() {
	  if(IntraTAModel) {
	    return 1;
	  } 
	  else if(nanbu){
	    return 1;
	  }
	  else if(TAModel!=(ostring) "none"){
	    return 1;
	  }
	  else if(coulomb!=(ostring) "none"){
	    return 1;
	  }
	  else {
	    return 0;
	  }
	}

	const char *get_name_ptr() { return name.c_str(); } 
	static void reset_idCount() {idCount=0;}
	void zeroKineticEnergy() {KineticEnergy = 0;}
	void addKineticEnergy(Scalar deltaE) {KineticEnergy+=deltaE;}
	Scalar getKineticEnergy() {return KineticEnergy;}
	Scalar getTemperature(int number){
	  temper = KineticEnergy / PROTON_CHARGE / number;
	  fprintf(stderr,"Kine=%g\n",KineticEnergy);
	  return temper; 
	}

};


//typedef oopicList<Species> SpeciesList;

#endif	//	ifndef __SPECIES_H









