/*
====================================================================

cylindri.cpp

0.99	(NTG 12-29-93) Separated into individual module from pic.h.
0.991	(JohnV 01-03-94) Aesthetics, compile.
1.001	(JohnV 05-06-96) Added toNodes().

====================================================================
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <symmetry.h>
#include "cylindri.h"
#include "ptclgrp.h"
#include "fields.h"
#include "psolve.h"

using namespace std;

void CylindricalAxis::setPassives()
{	
	if(alongx2())
		for(int k = MIN(k1,k2); k<MAX(k1,k2); k++)
			// set up the coefficients for an electrostatic field solve, if any
			fields->getPoissonSolve()->set_coefficient(j1,k,CYLINDRICAL_AXIS,fields->get_grid());
	else
		for (int j=MIN(j1,j2); j<MAX(j1,j2); j++)
			fields->getPoissonSolve()->set_coefficient(j,k1,CYLINDRICAL_AXIS,fields->get_grid());
	return;
}

//--------------------------------------------------------------------
//	Collect particles

void CylindricalAxis::collect(Particle& p, Vector3& dxMKS)
{
//	p.get_u().set_e2(-p.get_u().e2());		//	reverse velocity
	Vector3 u = p.get_u();
	u.set_e2(-u.e2());							// reverse radial u
	p.set_u(u);										//	store it
//   p.get_x().set_e2(fabs(p.get_x().e2()));	//	abs(radius)
	dxMKS.set_e2(-dxMKS.e2());	         // currently unused...
	particleList.push(&p);
	cout << "CylindricalAxis::collect()" << endl;
}

//--------------------------------------------------------------------
//	Reinject collected particles

ParticleList& CylindricalAxis::emit(Scalar t,Scalar dt, Species * species)
{
	return particleList;
}


void CylindricalAxis::toNodes() 
  /* throw(Oops) */
{
	int j, k;
   Scalar w1m, w1p;
  
  if (normal != 1) {
    stringstream ss (stringstream::in | stringstream::out);   
    ss<<"CylindricalAxis::toNodes: Error: \n"<<
      "Normal must be 1"<<endl;

    std::string msg;
    ss >> msg;
    Oops oops(msg);
    throw oops;    // exit() nobody calls this

  }
  Grid* grid = fields->get_grid();
	Vector3** ENode = fields->getENode();
	Vector3** BNode = fields->getBNode();
	Vector3** intEdl = fields->getIntEdl();
	Vector3** intBdS = fields->getIntBdS();
										  
	for (j=j1, k=k1; j<=j2; j++)
	{
		if (j > 0 && j < grid->getJ())
		{
			w1m = grid->dl1(j, k)/(grid->dl1(j-1, k) + grid->dl1(j, k));
			w1p = 1 - w1m;
			ENode[j][k].set_e1(w1p*intEdl[j][k].e1()/grid->dl1(j, k) 
				+ w1m*intEdl[j-1][k].e1()/grid->dl1(j-1, k));
		}
		BNode[j][k].set_e1(2*intBdS[j][k1].e1()/grid->dS1(j, k1)
			- BNode[j][k1+1].e1());
	}
}

