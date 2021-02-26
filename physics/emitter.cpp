/*
====================================================================

EMITTER.CPP

	A pure virtual class representing all conducting surfaces that can
	emit particles.

0.99	(NTG 12-29-93) Separated into individual module from pic.h.
0.991	(JohnV 01-11-94) Add setPassives().
0.992	(JohnV 01-27-95) Add Species.
0.993	(JohnV 02-10-95) Add secondary emission.
1.1   (JohnV 03-12-97) Add generic initialPush() for advancing emitted particles
1.11  (JohnV 06-26-97) Upgraded accuracy of initialPush().

====================================================================
 */

#include "emitter.h"
#include	"fields.h"
#include "ptclgrp.h"
#include "psolve.h"


#ifdef _MSC_VER
using std::cout;
using std::cerr;
using std::endl;
#endif

Emitter::Emitter(oopicList <LineSegment> *segments, Species* emitSpecies,
		Scalar _np2c) : Dielectric(segments,(Scalar)1.0)
{
	if(segments->nItems() > 1) {
		cout << "Warning, Emitter-derived boundaries can only have 1 segment.\n";
		cout << "Check your input file.\n";
	}
	np2c = _np2c;
	species = emitSpecies;
	BCType = CONDUCTING_BOUNDARY;
}

void Emitter::initialize()
{
	rweight=FALSE;
	switch(fields->get_grid()->query_geometry()) {
	case ZRGEOM:
		rweight=TRUE;
		break;
	case ZXGEOM:
		rweight=FALSE;
		break;
	}
}

//--------------------------------------------------------------------
//	Configure Fields::iC and :: iL for passive BC.  Currently assumes
//	conductor, sets iC = 0 along its surface.  Also requires vertical
//	or horizontal boundaries.  Note that we must step thru in the
//	correct order; this once expended a large amount of effort to
//	determine that iC along a conductor with k2 < k1 was not being
//	set, causing a leaky boundary.  This default emitter behaves
//	like a conductor.

void Emitter::setPassives()
{
	PoissonSolve *psolve=fields->getPoissonSolve();
	Grid *grid = fields->get_grid();
	if (alongx2())								//	vertical
	{
		for (int k=k1; k<=k2; k++)
		{
			fields->set_iC2(j1, k, 0);       	
			fields->set_iC3(j1, k, 0);
		}
	}
	else if (k1==k2)							//	horizontal
	{
		for (int j=j1; j<=j2; j++)
		{
			fields->set_iC1(j, k1, 0);
			fields->set_iC3(j, k1, 0);
		}
	}
	else {                              // oblique
		int j,k, jl,kl,jh,kh;
		/* start with the second point, advance one point at a time */
		for(j=2;j<4*abs(j2-j1)+4;j+=2) {
			jl=points[j-2];
			kl=points[j-1];
			jh=points[j];
			kh=points[j+1];
			if(kh==kl) /*horizontal segment*/
			{
				fields->set_iC1(jl,kl,0);
				fields->set_iC3(jl,kl,0);
			}
			else { // vertical segment
				for(k=MIN(kl,kh);k<=MAX(kl,kh);k++){   /* <= to get the last point */
					fields->set_iC2(jh,k,0);
					fields->set_iC3(jh,k,0);
				}
			}
		}
	}
}

//--------------------------------------------------------------------
// collect() throws out incident particle.

void Emitter::collect(Particle& p, Vector3& dxMKS)
{
	Dielectric::collect(p,dxMKS);
}

//--------------------------------------------------------------------
// initialPush() pushes emitted particles through a smaller timestep,
// del_t.  dt specifies the full timestep for this species, x is the initial
// position, u is the initial momentum.  Note that u = gamma*v.  The
// position is advanced using the initial velocity, 1st order accurate.
// The u0 and other 0 variable series correspond to a 1st order half push for
// the velocity, which will be used to obtain x. Refer to the better than
// 1st order method in Cartwright et. al. Injection ...

Boundary* Emitter::initialPush(Scalar del_t, Scalar dt, Particle &p)
{
	Vector3 u = p.get_u();
	Vector2 x = p.get_x();
	Vector3 u0 = u;
	Scalar igamma = 1/p.gamma();
	Scalar f = 0.5*(del_t - 0.5*dt)*p.get_q_over_m();
	Scalar f0 = 0.25*del_t*p.get_q_over_m();
	Vector3 a = f*fields->E(x);
	Vector3 a0 = f0*fields->E(x);
	u += a;
	u0 += a0;
	Vector3 tt = fields->B(x);
	if (tt.e1()!=0 || tt.e2()!=0 || tt.e3()!=0){ // magnetized push
		Vector3 tt0 = tt*f0*igamma;
		tt *= f*igamma;
		Vector3 u1 = u + u.cross(tt);
		Vector3 u2 = u0 + u0.cross(tt0);
		Vector3 ss = 2*tt/(1 + tt*tt);
		Vector3 ss0 = 2*tt0/(1 + tt0*tt0);
		u += u1.cross(ss);
		u0 += u2.cross(ss0);
	}
	u += a;
	u0 += a0;
	Vector3 dxMKS = fields->differentialMove(fields->getMKS(x), u0*igamma, u0, del_t);
	Boundary* bPtr = fields->translateAccumulate(x, dxMKS, p.get_q()/dt);
	p.set_u(u);
	p.set_x(x);
	if (bPtr) bPtr->collect(p, dxMKS);
	else{
		if (igamma>.86)
			species->addKineticEnergy(.5*u0*igamma*u*p.get_m());
		else
			species->addKineticEnergy(p.get_m()*(p.gamma()-1)*SPEED_OF_LIGHT_SQ);
	}

	// ScottRice; 2017
	Vector3 Emission_v = p.get_u()/p.gamma();        // Incident_v is the velocity of the incident particle
	Scalar Emission_KE = pow(Emission_v.magnitude(), 2) * ELECTRON_MASS / (2*PROTON_CHARGE);  // Incident electron energy in electron volts
	cout << "\nIn emitter.cpp";
	printf (", Emission_KE =  %.10g, igamma = %.10g, v = [%.10g %.10g %.10g]\n", Emission_KE, igamma, Emission_v.e1(), Emission_v.e2(), Emission_v.e3());
	// (Emission_KE - 300) << ", igamma = " << igamma << ", v = [" << Emission_v.e1() << ", " << Emission_v.e2() << ", " << Emission_v.e3() << "]";

	return bPtr;
}




