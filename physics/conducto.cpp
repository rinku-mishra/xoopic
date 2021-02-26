/*
====================================================================

CONDUCTO.CPP

0.99	(NTG 12-29-93) Separated into individual module from pic.h.
0.991	(JohnV 05-10-94) Fix setPassives() for TE mode at corners.
0.992	(JohnV 02-09-95) Added secondary electron emission coefficient.
0.993 (JohnV 04-19-95) bug fix for set_iC out of bounds
0.994 (JohnV 09-02-95) Move secondary electron params to Dielectric.

====================================================================
*/

#include "conducto.h"
#include	"fields.h"
#include "ptclgrp.h"
#include "psolve.h"

Conductor::Conductor(oopicList <LineSegment> *segments)
	: Dielectric(segments, (Scalar)1.0)
{
	BoundaryType = CONDUCTOR;
	BCType = CONDUCTING_BOUNDARY;
	// Oblique boundaries are stairstepped by diagnostics.
}

//--------------------------------------------------------------------
//	Configure Fields::iC and :: iL for passive BC.  Currently assumes
//	conductor, sets iC = 0 along its surface.  Also requires vertical
//	or horizontal boundaries.  Note that we must step thru in the
//	correct order; this once expended a large amount of effort to
//	determine that iC along a conductor with k2 < k1 was not being
//	set, causing a leaky boundary.

void Conductor::setPassives()
{
	PoissonSolve *psolve=fields->getPoissonSolve();
	Grid *grid = fields->get_grid();
	oopicListIter <LineSegment> lsI(*segments);
	for(lsI.restart();!lsI.Done();lsI++) {
	  int j1,j2,k1,k2,normal,*points;  //local copies of above
	  j1=(int)lsI.current()->A.e1();
	  k1=(int)lsI.current()->A.e2();
	  j2=(int)lsI.current()->B.e1();
	  k2=(int)lsI.current()->B.e2();
	  points=lsI.current()->points;
	  normal=lsI.current()->normal;
	  
	  if (j1==j2)								//	vertical
		 {
			for (int k=k1; k<=k2; k++)
			  {
				 fields->set_iC2(j1, k, 0);       	
				 fields->set_iC3(j1, k, 0);
		}	
	}
	else if(k1==k2)											//	horizontal
	{
		for (int j=j1; j<=j2; j++)
		{
			fields->set_iC1(j, k1, 0);
			fields->set_iC3(j, k1, 0);
		}
	}
	else	{ // oblique boundaries

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
				
				else {
					for(k=MIN(kl,kh);k<=MAX(kl,kh);k++){   /* <= to get the last point */
						fields->set_iC2(jh,k,0);
						fields->set_iC3(jh,k,0);
					}
				}
			}
		 }
	}
}
//--------------------------------------------------------------------

void Conductor::collect(Particle& p, Vector3& dxMKS)
{
	Dielectric::collect(p,dxMKS);
//	delete &p;  deletion already done in Dielectric::collect
}


