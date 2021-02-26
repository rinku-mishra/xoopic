/*
====================================================================

PARTICLEGROUPNR.CPP

Electrostatic version of ParticleGroup.

Revision History
1.01  kc and dc added to advance particles non-relativistically in an EM field solve

====================================================================
*/

#include "ptclgpnr.h"
#include "fields.h"
#include	"particle.h"

void ParticleGroupNR::advance(Fields& fields, Scalar dt)
{
	dt = species->get_subcycleIndex()*dt;
	Scalar	f = 0.5*dt*q_over_m;
	Scalar	q_dt = q/dt;
	Vector3	uPrime;
	Vector3	a;
	Vector3	t;
	Vector3	s;
	Vector2*	x = ParticleGroup::x;
	Vector3*	u = ParticleGroup::u;
	Vector3	dxMKS;
	Boundary*	bPtr;
	Grid&	grid = *fields.get_grid();
	Vector3	B;

	// cout << get_speciesSub() << endl;
	for (int i=0; i<n; i++, x++, u++)
	{
		bPtr = 0;
		if ((qArray)) q_dt = qArray[i]/dt;	//	variable weighted particles
		a = f*fields.E(*x);
		*u += a;									//	half acceleration
		// B = fields.B(*x);
			t = f*fields.B(*x);
			uPrime = *u + u->cross(t);
			s = 2*t/(1 + t*t);
			*u += uPrime.cross(s);				//	rotation
		*u += a;									//	half acceleration
		dxMKS = grid.differentialMove(fields.getMKS(*x), *u, *u, dt);
		// while (fabs(dxMKS.e1()) + fabs(dxMKS.e2()) > 1E-25 && !bPtr)
		//	bPtr = grid.translate(*x, dxMKS);
		// if (bPtr)
		if ((bPtr = fields.translateAccumulate(*x, dxMKS, q_dt)))
		{
			//	send this particle to boundary
			bPtr->collect(*(new Particle(*x, *u, species, get_np2c(i),
				(BOOL)(qArray!=0))), dxMKS);
			//	Move last particle into this slot and advance it.
			n--;
			if (i == n) break;				//	last particle in array?
			*x = ParticleGroup::x[n];		//	move last particle into open slot
			*u = ParticleGroup::u[n];
			if (qArray) qArray[i] = qArray[n];
			i--;									//	Set index to do i again.
			x--;									//	Setup to redo the array element
			u--;                          // it gets inc in for loop
		}
#ifdef DEBUG
	if(((*x).e1()==grid.getJ() || (*x).e1()==0 || (*x).e2()==grid.getK()
		|| (*x).e2()==0)&&!bPtr) {
		printf("Particle escaped.\n");
		int *crash = 0;	
		*crash = 10;
		}
#endif //DEBUG

	}
}
