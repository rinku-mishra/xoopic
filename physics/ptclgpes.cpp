/*
====================================================================

PARTICLEGROUPES.CPP

Electrostatic version of ParticleGroup.

Revision History
0.99	(JohnV 01-23-95)	Original version.
0.991	(JohnV 05-28-95)	Complete and integrate.

Should optimize for B=0.

====================================================================
*/

#include "ptclgpes.h"
#include "fields.h"
#include	"particle.h"

void ParticleGroupES::advance(Fields& fields, Scalar dt)
{
	Scalar	f = 0.5*dt*q_over_m;
	Scalar	q_dt = q/dt;
	Vector3	uPrime;
	Vector3 uOld;
	Vector3	a;
	Vector3	t;
	Vector3	s;
	Vector2* x = ParticleGroup::x;
	Vector3* u = ParticleGroup::u;
	Vector3	dxMKS;
	Boundary*	bPtr;
	Grid&	grid = *fields.get_grid();
	Vector3	B;
//	register Scalar temp;
	double localEnergy = 0; // overflows if float

	for (int i=0; i<n; i++, x++, u++)
	{
		uOld = *u;
		bPtr = 0;
		a = f*fields.E(*x);
		*u += a;									//	half acceleration
		B = fields.B(*x);   // speed-ups make an array for f*E and f*B
		                    // have a and t returned with one call
		                    // if test B.isNonZero outside this loop
		                    // then f*B doesn't have to be returned and
								  // an if test and += are saved per particle
		if (B.isNonZero())
		{
			t = f*B;								// reuse B!
			uPrime = *u + u->cross(t);
			s = 2*t/(1 + t*t);
			*u += uPrime.cross(s);			//	rotation
		}
		*u += a;									//	half acceleration
		dxMKS = grid.differentialMove(fields.getMKS(*x), *u, *u, dt);
		while (fabs(dxMKS.e1()) + fabs(dxMKS.e2()) > 1E-25 && !bPtr)
			bPtr = grid.translate(*x, dxMKS);
		if (bPtr)
		{
		  //	send this particle to boundary
		  bPtr->collect(*(new Particle(*x, *u, species, get_np2c(i),
					       (BOOL)(qArray!=0))), dxMKS);
		  n--; // decrement number of particles
		  //	Move last particle into this slot and advance it.
		  if (i == n) break; //	last particle in array?
		  *x = ParticleGroup::x[n]; //	move last particle into open slot
		  *u = ParticleGroup::u[n];
		  if (qArray) qArray[i] = qArray[n];
		  i--; // Set index to do i again.
		  x--; // Setup to redo the array element
		  u--; // it gets inc in for loop
		}
		else{
		  if (qArray)
		    localEnergy += qArray[i]**u*uOld;
		  else
		    localEnergy += *u*uOld;
		}
#ifdef DEBUG
		if(strncmp(HOSTTYPE, "alpha", 5) != 0)
		  {
		    if(((*x).e1()>=grid.getJ() || (*x).e1()<=0 || (*x).e2()>=grid.getK()
			|| (*x).e2()<=0)&&!bPtr) {
		      printf("Particle escaped.\n");
		      int *crash = 0;	
		      *crash = 10;
		    }
		  }
		
#endif //DEBUG
		
	}
	if (qArray)
	  species->addKineticEnergy(0.5*localEnergy/q_over_m);
	else
	  species->addKineticEnergy(0.5*localEnergy*get_m());
}
