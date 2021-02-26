/*
====================================================================

PARTICLEGROUPIB.CPP

Electrostatic version of ParticleGroup. Assumes infinite B along x1.

Revision History
1.01  kc and dc added to advance particles non-relativistically with and infinte B-field

====================================================================
*/

#include "ptclgpib.h"
#include "fields.h"
#include	"particle.h"

void ParticleGroupIBES::advance(Fields& fields, Scalar dt)
{
  Scalar f = dt*q_over_m;
  Scalar q_dt = q/dt;
  Scalar a;
  Vector3 s;
  Vector2* x = ParticleGroup::x;
  Vector3* u = ParticleGroup::u;
  Vector3 dxMKS;
  Boundary* bPtr;
  Grid&	grid = *fields.get_grid();

  // cout << get_speciesSub() << endl;
  for (int i=0; i<n; i++, x++, u++){
    bPtr = 0;
    if (qArray) q_dt = qArray[i]/dt; //	variable weighted particles
    a = f*fields.E(*x).e1();
    *u += Vector3(a,0,0); // full acceleration
    dxMKS = grid.differentialMove(fields.getMKS(*x), *u, *u, dt);
    if ((bPtr = fields.translateAccumulate(*x, dxMKS, q_dt))){// send this particle to boundary
      bPtr->collect(*(new Particle(*x, *u, species, get_np2c(i),
				   (BOOL)(qArray!=0))), dxMKS);// Move last particle into this slot and advance it.
      n--;
      if (i == n) break;				//	last particle in array?
      *x = ParticleGroup::x[n];		//	move last particle into open slot
      *u = ParticleGroup::u[n];
      if (qArray) qArray[i] = qArray[n];
      i--;									//	Set index to do i again.
      x--;									//	Setup to redo the array element
      u--;                          // it gets inc in for loop
    }
  }
}

void ParticleGroupIBEM::advance(Fields& fields, Scalar dt){
  Scalar	f = dt*q_over_m;
  Scalar	q_dt = q/dt;
  Scalar	a;
  Vector2*	x = ParticleGroup::x;
  Vector3*	u = ParticleGroup::u;
  Vector3	dxMKS;
  Boundary*	bPtr;
  Grid&	grid = *fields.get_grid();
  Scalar igamma;

  for (int i=0; i<n; i++, x++, u++){
    if (qArray) q_dt = qArray[i]/dt; //	variable weighted particles
    a = f*fields.E(*x).e1();
    *u += Vector3(a,0,0); // full acceleration
    igamma = 1/ParticleGroup::gamma(*u);// compute gamma once to save sqrt()
    dxMKS = grid.differentialMove(fields.getMKS(*x), *u*igamma, *u, dt);
    if ((bPtr = fields.translateAccumulate(*x, dxMKS, q_dt))){
      // send this particle to boundary
      bPtr->collect(*(new Particle(*x, *u, species, get_np2c(i),
				   (BOOL)(qArray!=0))), dxMKS);
      // Move last particle into this slot and advance it.
      n--;
      if (i == n) break; // last particle in array?
      *x = ParticleGroup::x[n];	// move last particle into open slot
      *u = ParticleGroup::u[n];
      if (qArray) qArray[i] = qArray[n];
      i--; // Set index to do i again.
      x--; // Setup to redo the array element
      u--; // it gets inc in for loop
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
