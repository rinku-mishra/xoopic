/*
====================================================================

LAUNCHWAVE.CPP

Standard open boundary port to launch a TEM wave of phase velocity
vphase in cylindrical geometry.

The Expert System MUST know that these objects can only be
instantiated at the simulation boundaries j=0 or j=J or k=0 or
k=K

Revision/Programmer/Date
0.9	(BillP 10-15-94) Initial code for launching wave from left boundary only
0.91	(JohnV 11-30-94) Compile and add to project.
0.92	(JohnV 12-05-94) Enhance and optimize.


====================================================================
*/

#include "fields.h"
#include "launchwa.h"
#ifdef UNIX
#include "ptclgrp.h"
#endif

LaunchWave::LaunchWave(oopicList <LineSegment> *segments, Scalar wavePhase,
	Scalar _amplitude, Scalar _riseTime, Scalar _pulseLength, Scalar _frequency) :
	Port(segments)
{

	tOld = 0.0;
	vPhase = wavePhase;
	riseTime = _riseTime;
	pulseLength = _pulseLength;
	amplitude = _amplitude;
	frequency = _frequency;
	kmin = MIN(k1, k2);
	kmax = MAX(k1, k2);
	jmin = MIN(j1, j2);
	jmax = MAX(j1, j2);

	int L1 = abs(k2 - k1) + 1;
	int L2 = abs(j2 - j1) + 1;

	if (alongx2())								// vertical
		oldE = new Vector2[L1];
	if (alongx1())								// horizontal
		oldE = new Vector2[L2];
}

LaunchWave::~LaunchWave()
{
	delete[] oldE;
}

//--------------------------------------------------------------------
//      Apply boundary conditions to fields locally.

void LaunchWave::applyFields(Scalar t,Scalar dt)
{
   if(dt==0) return;
	Grid* grid = fields->get_grid();   // get pointer
	Scalar A;
	Scalar Er, Etheta, Er2, Etheta2, Ez, Ez2;
//	Scalar oldEr, OldEtheta, OldEz;
	//	Scalar dt;
	Scalar dz, dr, vbar;
	Scalar MIN = 1e-30;

	A = amplitude * cos(2 * 3.141592654 * frequency * t);

	//		dt = t-tOld; //It is better to get dt by just passing it!!!

	if (alongx2())								// vertical
	{
		if (get_normal()==1)					//	right-facing
		{
			//normalization for line voltage
			Scalar rMin = grid->getMKS(0,kmin).e2();
			Scalar rMax = grid->getMKS(0,kmax).e2();
			Scalar vol = log(rMax/(rMin+MIN));

			for (int k = kmin; k < MAX(k1,k2); k++)
			{
			/* get values of int(E.dl) at places where they live
				one cell away from left-hand boundary.
				Note that Er and Etheta are positioned on the boundary itself.
			*/
				Er2 = fields->getIntEdl()[1][k].e2()/grid->dl2(1,k);
				if (k==0) Etheta2 = 0.0;
				else Etheta2 = fields->getIntEdl()[1][k].e3()/grid->dl3(1,k);

				dz = grid->dl1(0,k);
				vbar = vPhase*SPEED_OF_LIGHT*dt/dz;

				// calculate TEM amplitude factor
				
				Scalar Ebar = 4.0*A*dt/(vol*(1.0 + vbar));

				// now include trapezoidal current pulse for weighted dE/dt
				if (t < riseTime)	Ebar *= t/riseTime;
				else if (t <= riseTime + pulseLength);
				else if (t < 2*riseTime + pulseLength)	Ebar -= Ebar*t/riseTime;
				else Ebar = 0.0;

/* the following code gets Er and Etheta at the cell nodes one cell away from
	boundary; in general, we can just use their values at the places where they
	live on the Yee Mesh as above so we needn't implement this because
	then we would have to weight back the fields to places where they
	live when we do a setIntEdl() */
	/*	Er2 = fields->getENode()[1][k].e2();
		Etheta2 = fields->getENode()[1][k].e3(); */

// now construct current electric field values which must be passed to fields
// NOTE: oldE.e1() = Er and oldE.e2() = Etheta
				Er = (oldE[k - kmin].e1() - Er2*(1 - vbar))/(1 + vbar);
				Etheta = (oldE[k - kmin].e2() - Etheta2*(1 - vbar))/(1 + vbar);

// now include particular solution for launched wave
//				Er = Er + Ebar/grid->getMKS(0, k+1/2).e2();
				Er = Er + Ebar/(grid->getMKS(Vector2(0, k+.5))).e2();

// Store old values
//				oldEr = Er*(1-vbar) + Er2*(1+vbar);
// include launched wave
//				oldE[k - kmin].e1() = Er*(1-vbar) + Er2*(1+vbar) + Ebar*(1 - vbar)
//				/grid->getMKS(0, k + 1/2).e2();
//				oldE[k - kmin].e1() = Er*(1-vbar) + Er2*(1+vbar) + Ebar*(1 - vbar)
//					/(grid->getMKS(Vector2(0, k + .5))).e2();
//				oldE[k - kmin].e2() = Etheta *(1 - vbar) + Etheta2*(1 + vbar);
				oldE[k - kmin] = Vector2( Er*(1-vbar) + Er2*(1+vbar) + Ebar*(1 - vbar)
					/(grid->getMKS(Vector2(0, k + .5))).e2(),
												 Etheta *(1 - vbar) + Etheta2*(1 + vbar));		 

// Store new values of Electric field on Boundary
// Because Er and Etheta are calculated where they live we just multiply
// by dl().
//				Erdl = Er*grid->dl2(0,k);
//				Ethetadl = Etheta*grid->dl3(0,k);
				fields->setIntEdl(0, k, 2, Er*grid->dl2(0,k));
				fields->setIntEdl(0, k, 3, Etheta*grid->dl3(0,k));
			}
		}
		else										// Boundary on RHS;
		{
			for (int k=kmin; k<kmax; k++)
			{
			/* get values of int(E.dl) at places where they live
				one cell away from right-hand boundary.
				Note that Er and Etheta are positioned on the boundary itself.
			*/
				Er2 = fields->getIntEdl()[grid->getJ()-1][k].e2()/
					grid->dl2(grid->getJ()-1,k);
				if (k==0) Etheta2 = 0.0;
				else Etheta2 = fields->getIntEdl()[grid->getJ()-1][k].e3()/
					grid->dl3(grid->getJ()-1,k);

				dz = grid->dl1(grid->getJ()-1,k);
				vbar = vPhase*SPEED_OF_LIGHT*dt/dz;

/* the following code gets Er and Etheta at the cell nodes one cell away from
	boundary; in general, we can just use their values at the places where they
	live on the Yee Mesh as above so we needn't implement this because
	then we would have to weight back the fields to places where they
	live when we do a setIntEdl() */
		/*	Er2 = fields->getENode()[J-1][k].e2();
			Etheta2 = fields->getENode()[J-1][k].e3(); */

// now construct current electric field values which must be passed to fields
				Er = (oldE[k-kmin].e1() - Er2*(1-vbar))/(1+vbar);
				Etheta = (oldE[k-kmin].e2() - Etheta2*(1-vbar))/(1+vbar);

// Store old values
//				oldE[k-kmin].e1() = Er*(1-vbar) + Er2*(1+vbar);
//				oldE[k-kmin].e2() = Etheta *(1-vbar) + Etheta2*(1+vbar);
				oldE[k-kmin] = Vector2(Er*(1-vbar) + Er2*(1+vbar),
											   Etheta *(1-vbar) + Etheta2*(1+vbar));

// Because Er and Etheta are calculated where they live we just multiply
// by dl().
//				Erdl = Er*grid->dl2(grid->getJ(),k);
//				Ethetadl = Etheta*grid->dl3(grid->getJ(),k);

// Store new values of Electric field on Boundary
				fields->setIntEdl(grid->getJ(), k, 2, Er*grid->dl2(grid->getJ(),k));
				fields->setIntEdl(grid->getJ(), k, 3, Etheta*grid->dl3(grid->getJ(),k));
			}
		}
	}
	else											// horizontal
	{
		if (get_normal()==1)                       // going out (greater r)
		{
			for (int j=jmin; j<MAX(j1,j2); j++)
			{
			/* get values of int(E.dl) at places where they live
				one cell away from lower boundary.
				Note that Er and Etheta are positioned on the boundary itself.
			 */
				Ez2 = fields->getIntEdl()[j][1].e1()/grid->dl1(j,1);
				Etheta2 = fields->getIntEdl()[j][1].e3()/grid->dl3(j,1);

				dr = grid->dl2(j,0);
				vbar = vPhase*SPEED_OF_LIGHT*dt/dr;

/* the following code gets Er and Etheta at the cell nodes one cell away from
	boundary; in general, we can just use their values at the places where they
	live on the Yee Mesh as above so we needn't implement this because
	then we would have to weight back the fields to places where they
	live when we do a setIntEdl() */
	/*	Ez2 = fields->getENode()[j][1].e1();
		Etheta2 = fields->getENode()[j][1].e3();
		Vector2 EjustOffBoundary = Vector2(Ez2, Etheta2);
	*/

// now construct current electric field values which must be passed to fields
// NOTE:  oldE.e1() = Ez and oldE.e2() = Etheta
				Ez = (oldE[j-jmin].e1() - Ez2*(1-vbar))/(1+vbar);
				Etheta = (oldE[j-jmin].e2() - Etheta2*(1-vbar))/(1+vbar);

// Store old values
//				oldE[j-jmin].e1() = Ez*(1-vbar) + Ez2*(1+vbar);
//				oldE[j-jmin].e2() = Etheta *(1-vbar) + Etheta2*(1+vbar);
				oldE[j-jmin] = Vector2( Ez*(1-vbar) + Ez2*(1+vbar),
											  Etheta *(1-vbar) + Etheta2*(1+vbar));

// Because Er and Etheta are calculated where they live we just multiply
// by dl().
//				Ezdl = Ez*grid->dl1(j,0);
//				Ethetadl = Etheta*grid->dl3(j,0);
// Store new values of Electric field on Boundary
				fields->setIntEdl(j, 0, 1, Ez*grid->dl1(j,0));
				fields->setIntEdl(j, 0, 3, Etheta*grid->dl3(j,0));
			}
		}
		else										// going in
		{
			for (int j=jmin; j<jmax; j++)
			{
			/* get values of int(E.dl) at places where they live
				one cell away from lower boundary.
				Note that Er and Etheta are positioned on the boundary itself.
			*/
				Ez2 = fields->getIntEdl()[j][grid->getK()-1].e1()/
					grid->dl1(j,grid->getK()-1);
				Etheta2 = fields->getIntEdl()[j][grid->getK()-1].e3()/
					grid->dl3(j,grid->getK()-1);

				dr = grid->dl2(j, grid->getK()-1);        // this is dr
				vbar = vPhase*SPEED_OF_LIGHT*dt/dr;

/* the following code gets Er and Etheta at the cell nodes one cell away from
	boundary; in general, we can just use their values at the places where they
	live on the Yee Mesh as above so we needn't implement this because
	then we would have to weight back the fields to places where they
	live when we do a setIntEdl() */
	/*	Ez2 = fields->getENode()[j][1].e1();
		Etheta2 = fields->getENode()[j][1].e3(); */

// now construct current electric field values which must be passed to fields
// NOTE:  oldE.e1() = Ez and oldE.e2() = Etheta
				Ez = (oldE[j-jmin].e1() - Ez2*(1-vbar))/(1+vbar);
				Etheta = (oldE[j-jmin].e2() - Etheta2*(1-vbar))/(1+vbar);

// Store old values
//				oldE[j-jmin].e1() = Ez*(1-vbar) + Ez2*(1+vbar);
//				oldE[j-jmin].e2() = Etheta *(1-vbar) + Etheta2*(1+vbar);
				oldE[j-jmin] = Vector2(Ez*(1-vbar) + Ez2*(1+vbar),
											  Etheta *(1-vbar) + Etheta2*(1+vbar));

// Because Er and Etheta are calculated where they live we just multiply
// by dl().
//				Ezdl = Ez*grid->dl1(j,grid->getK());
//				Ethetadl = Etheta*grid->dl3(j,grid->getK());
// Store new values of Electric field on Boundary
				fields->setIntEdl(j, grid->getK(), 1, Ez*grid->dl1(j,grid->getK()));
				fields->setIntEdl(j, grid->getK(), 3, Etheta*grid->dl3(j,grid->getK()));
			}
		}
	}
	//	tOld = t;
}

