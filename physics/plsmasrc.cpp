/*
	 ====================================================================

	 plasmaSource.cpp

	 This class describes a volumetric plasma source, and can create pairs
	 with the specified distribution.

	 1.0	(JohnV 05-18-96) Original code.
	 1.1  (JohnV 06-11-00) Add temporal dependence.
    1.2  (JohnV 12-Mar-2002) Fixed sin(x1) dependence to uniform

	 ====================================================================
	 */

#include "plsmasrc.h"
#include "maxwelln.h"
#include "fields.h"
#include "eval.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


using namespace std;

Scalar PlasmaSource::F(Vector2 x, Scalar t = 0) 
{
	if(analyticF==(ostring)"0.0\n") {
		//return(fabs(sin((x.e1()-p1.e1())/(p2.e1()-p1.e1())* M_PI/2))); 
      // (JohnV) this should return a uniform dist by default, not half sine
      return 1;
		// if the return value is negitive bad things will happen
  }
	else {
		adv_eval->add_variable("x1",x.e1());
		adv_eval->add_variable("x2",x.e2());
		adv_eval->add_variable("t", t);
		return adv_eval->Evaluate(analyticF.c_str());
	}
	
}

PlasmaSource::PlasmaSource(Maxwellian* max1, Maxwellian* max2, Scalar _sourceRate, 
													 oopicList <LineSegment> *segments, Scalar np2c,const ostring &_analyticF)
: Emitter(segments, max1->get_speciesPtr(), np2c)
{
	analyticF=_analyticF;
	analyticF+='\n';
	maxwellian1 = max1;
	maxwellian2 = max2;
	species2 = max2->get_speciesPtr();
	sourceRate = _sourceRate;
	init = TRUE;
	extra = 0.5;
}

PlasmaSource::~PlasmaSource()
{
	delete maxwellian1;
	delete maxwellian2;
}

void PlasmaSource::initialize()
{
	init = FALSE;
	Grid *grid = fields->get_grid();
	if (grid->query_geometry() == ZXGEOM) is_xy = TRUE;
	else is_xy = FALSE;
	p1 = fields->getMKS(j1, k1);
	p2 = fields->getMKS(j2, k2);
	p1 += 1E-6*p2;// move off the edge a small amount
	p2 -= 1E-6*p2;
	deltaP = p2-p1;
	Scalar volume = deltaP.e1();
	if (is_xy) volume *= deltaP.e2();
	else 
		{
			rMinSqr = p1.e2()*p1.e2();
			drSqr = p2.e2()*p2.e2()-rMinSqr;
			volume *= M_PI*drSqr;
		}
	sourceRate *= volume/np2c;					// convert to num/time

  // Markov Chain Monte Carlo Load
	// doing a one time burn in
  int BurnNumber = 10000;
  Vector2 xc;
	// initial guess of x0
	x0.set_e1(p1.e1()+deltaP.e1()*frand());
	if(is_xy)
		x0.set_e2(p1.e2()+deltaP.e2()*frand());
	else
		x0.set_e2(sqrt(drSqr*frand()+p1.e2()));
  int seed = 0;
  
  // burn in
  Fx0 = F(x0,0);
  while (seed < BurnNumber)
    {
			xc.set_e1(p1.e1()+deltaP.e1()*frand());
			if(is_xy)
				xc.set_e2(p1.e2()+deltaP.e2()*frand());
			else
				xc.set_e2(sqrt(drSqr*frand()+rMinSqr));
      Fxc = F(xc,0);
      if ((Fxc!=0)&&((Fx0==0)||(Fxc>=Fx0*(Scalar)frand()))) 
				{
					seed++;
					x0=xc; 
					Fx0=Fxc;
				}
    }

	return;
}

ParticleList& PlasmaSource::emit(Scalar t, Scalar dt, Species* species_to_push)
{
	if (init) initialize();
	if (species==species_to_push){
		extra += sourceRate*dt*get_time_value(t)*species->get_subcycleIndex()/species->get_supercycleIndex();
		int N = (int) extra;
		Vector3 u = 0;							// pick velocity
		Vector2 xMKS,xc;

		int count = 0;
		while (count < N) {
         xc.set_e1(p1.e1()+deltaP.e1()*frand());
         if(is_xy)
            xc.set_e2(p1.e2()+deltaP.e2()*frand());
         else
            xc.set_e2(sqrt(drSqr*frand()+rMinSqr));
         Fxc = F(xc,t);
         if ((Fxc!=0) && (Fxc>=Fx0*(Scalar)frand())) {
            x0=xc;
            Fx0=Fxc;
            u=maxwellian1->get_U();
            Vector2	x = fields->getGridCoords(x0);
            particleList.add(new Particle(x, u, species, np2c));
            u=maxwellian2->get_U();
            particleList.add(new Particle(x, u, species2, np2c));
         }
         count++; // the calculation of N neglects the integral of F(x1,x2,t)
      }
		extra -= N;
	}
	return particleList;
}
