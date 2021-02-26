/*
====================================================================

PARTICLEGROUPSR.CPP

Revision History
0.99 (Bruhwiler 09-29-99) Original version.
1.01 (JohnV 10Jul2003) fixed recomputation of gamma before x update.

====================================================================
*/

#include	"ptclgpsr.h"
#include	"particle.h"
#include	"fields.h"
#include "dump.h"


//--------------------------------------------------------------------
//	Advance particle equations of motion one timestep.  This includes
//	weighting fields to the particle, solving the equation of motion,
//	and accumulating the particle current on the grid.  Boris mover
//	with u = gamma*v, v in MKS units; see B&L for variables.

// Differs from ParticleGroup::advance() only in that synchrotron
// radiation (SR) damping is added to electric field kick.

void ParticleGroupSR::advance(Fields& fields, Scalar dt)
{
  Scalar	f = 0.5*dt*q_over_m;
  Scalar	q_dt = q/dt;
  Vector3	uPrime;
  Vector3	a;
  Vector3	t;
  Vector3	s;
  Vector2*	x = ParticleGroup::x;  // x(t)
  Vector3*	u = ParticleGroup::u;  // u(t-dt/2), where u = gamma * v
  Vector3	dxMKS;
  Vector3 uPrevious, du, duSR;
  Scalar  u1, u2, u3, uScalar, duScalar, srFactor;
  Boundary*	bPtr;
  Grid&	grid = *fields.get_grid();
  Scalar igamma, localGamma, localBetaSq;
  Scalar localEnergy = 0;

  for (int i=0; i<n; i++, x++, u++){
    // store u(t-dt/2) to calculate time-centered momentum derivatives
    uPrevious = *u;
    u1 = u->e1();
    u2 = u->e2();
    u3 = u->e3();
    uScalar = sqrt( u1*u1 + u2*u2 + u3*u3 );

    // variable weighted particles are treated differently
    if (qArray) q_dt = qArray[i] / dt;

    // ***********************************************************
    // Begin cut-and-paste of base class electromagnetic algorithm
    a = f*fields.E(*x);
    *u += a;                         // the 1st half acceleration  
    
    // compute gamma once to save sqrt(), using pre-rotation u(t)
    localGamma = ParticleGroup::gamma(*u);
    igamma = 1. / localGamma;
    
    t = (f*igamma)*fields.B(*x);
    uPrime = *u + u->cross(t);
    s = 2*t/(1 + t*t);
    *u += uPrime.cross(s);				//	rotation
    
    *u += a;									//	the 2nd half acceleration
    
    // End cut-and-paste of base class electromagnetic algorithm
    // ***********************************************************
    // Now, apply the synchrotron radiation force:

    // du is the change in u due to external electromagnetic forces
    du = *u - uPrevious;

    // new scalar value of u
    u1 = u->e1();
    u2 = u->e2();
    u3 = u->e3();
    duScalar = sqrt( u1*u1 + u2*u2 + u3*u3 );

    // change in the scalar value of u
    duScalar -= uScalar;

    // duSR is the change in (vector) u due to synch radiation
    u1 = du.e1();
    u2 = du.e2();
    u3 = du.e3();
    localBetaSq = (localGamma+1.)*(localGamma-1.)/pow(localGamma,2);
    srFactor = -q_dt * q_over_m * pow(localGamma/SPEED_OF_LIGHT,2)
      * ( u1*u1 + u2*u2 + u3*u3 - localBetaSq*duScalar*duScalar )
      / ( 1.5e+07 * uScalar * np2c0);

    duSR.set_e1( srFactor * u->e1() );
    duSR.set_e2( srFactor * u->e2() );
    duSR.set_e3( srFactor * u->e3() );

		/*
    cout << endl;
    cout << "Acceleration due to SR is: " << endl;
    cout << "  duSR_1 = " << duSR.e1() << endl;
    cout << "  duSR_2 = " << duSR.e2() << endl;
    cout << "  duSR_3 = " << duSR.e3() << endl;
    cout << endl;
    cout << "  u_1 = " << u->e1() << endl;
    cout << "  u_2 = " << u->e2() << endl;
    cout << "  u_3 = " << u->e3() << endl;
    cout << "  u   = " << uScalar << endl;
		cout << endl;
    cout << "  du_1 = " << du.e1()  << endl;
    cout << "  du_2 = " << du.e2()  << endl;
    cout << "  du_3 = " << du.e3()  << endl;
    cout << "  du   = " << duScalar << endl;
		cout << endl;
    cout << "q_dt     = " << q_dt        << endl;
    cout << "gamma    = " << localGamma  << endl;
    cout << "beta^2   = " << localBetaSq << endl;
		cout << endl;
    cout << "q_over_m = " << q_over_m       << endl;
    cout << "c        = " << SPEED_OF_LIGHT << endl;
		cout << endl;
		*/

		// now apply the acceleration
    *u += duSR;
      

// ***********************************************************
// Resume cut-and-paste of base class electromagnetic algorithm

    igamma = 1/ParticleGroup::gamma(*u); // must recompute gamma
    // advance particle positions from x(t) to x(t+dt), using u(t+dt/2)
    dxMKS = grid.differentialMove(fields.getMKS(*x), *u*igamma, *u, dt);

    // accumulate particle current onto the grid
    // if nonzero value is returned, then handle particle BC's
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
    else{
      if (qArray)
	localEnergy += qArray[i]*(1/igamma-1);
      else
	localEnergy += 1/igamma-1;
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
  if (qArray)
    species->addKineticEnergy(SPEED_OF_LIGHT_SQ*localEnergy/q_over_m);
  else
    species->addKineticEnergy(SPEED_OF_LIGHT_SQ*localEnergy*get_m());
}
