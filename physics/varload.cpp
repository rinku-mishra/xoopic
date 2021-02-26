/*
====================================================================

varload.cpp

2.01 (Bruhwiler 10-28-99) Initial implementation.

====================================================================
 */

#include "varload.h"
#include "sptlrgn.h"
#include "maxwelln.h"
#include "misc.h"
#include "ostring.h"

#include <cstdio>

using namespace std;

VarWeightLoad::VarWeightLoad(
		SpatialRegion* _SR, Scalar _density, Maxwellian* max, Scalar _Min1MKS,
		Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS, int _LoadMethodFlag,
		Scalar _np2c, const ostring &_analyticF, const ostring &_DensityFile,
		const ostring &_name) :
		  Load(_SR, _density, max, _Min1MKS, _Max1MKS, _Min2MKS, _Max2MKS,
				  _LoadMethodFlag, _np2c, _analyticF, _DensityFile, _name)
{
	w0 = rMin/rMax;
	w1 = (rMax - rMin)/rMax;
}

VarWeightLoad::~VarWeightLoad()
{
}

int VarWeightLoad::load_it()
/* throw(Oops) */{
	Vector3 u;
	Vector2 x;
	Boundary*** CellMask = grid->GetCellMask();

	int seed = 0;

	if(numberParticles > 0) {
		Vector2 xc;
		Scalar Fxc;

		Scalar count = 0.;
		while (count < (Scalar)numberParticles)
		{
			switch (LoadMethodFlag){
			case RANDOM:
				xc.set_e1(p1.e1()+deltaP.e1()*frand());
				if(rz) {
					rFraction = getRandom_rFraction();
					xc.set_e2(rMax*rFraction);
				}
				else
					xc.set_e2(p1.e2()+deltaP.e2()*frand());
				break;
			case QUIET_START:
				xc.set_e1(p1.e1()+deltaP.e1()*revers2(seed));
				if(rz) {
					rFraction = getReverse_rFraction(seed,3);
					xc.set_e2(rMax*rFraction);
				}
				else
					xc.set_e2(p1.e2()+deltaP.e2()*revers(seed,3));
				break;
			default:
				stringstream ss (stringstream::in | stringstream::out);
				ss<<"VarWeightLoad::load_it: Error: \n"<<
						"LOAD: Bad value for loader flag"<<endl;

				std::string msg;
				ss >> msg;
				Oops oops(msg);
				throw oops;    // exit()VarWeightLoadParams::CreateCounterPart:

				break;
			}
			Fxc = F(xc);
			seed++;
			if ( (Fxc!=0) && (Fxc>=frand()) ){
				x = grid->getGridCoords(xc);
				if (CellMask[(int)x.e1()][(int)x.e2()]==0){  // FREESPACE
					if (rz) count += rFraction;
					else    count += 1.;
					u = maxwellian->get_U();
					Particle* particle = new Particle(x, u, species, np2cMax()*rFraction, TRUE);
					particleList.push(particle);
				}
			}
			//				if (count) if (count%50==0)	region->addParticleList(particleList);
		}
		region->addParticleList(particleList);
	}

	return(1);
}






