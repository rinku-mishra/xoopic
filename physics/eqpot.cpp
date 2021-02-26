//====================================================================
#ifdef	COMMENTS

EQPOT.CPP

0.90	(PeterM 1-9-95) Creation.
0.901 (JohnV 04-19-95) bug fix for set_iC out of bounds
1.001 (PeterM 8-12-96) Oblique equipotentials and better overlap behavior.

#endif	//COMMENTS
//====================================================================

#include "eqpot.h"
#include	"fields.h"
#include "ptclgrp.h"
#include "psolve.h"

Equipotential::Equipotential( oopicList <LineSegment> *segments, ostring _VanalyticF)
: Conductor(segments)
{

   VanalyticF = _VanalyticF + "\n";
   
   BoundaryType = EQUIPOTENTIAL;
   omit_left = 0;
   omit_right = 0;
}

Equipotential::~Equipotential() 
{
   int j;
   if(LaplaceSolution)
      for(j=0;j<=fields->getJ();j++)
         delete[] LaplaceSolution[j];
   delete[] LaplaceSolution;
   LaplaceSolution = 0;
}



//  This function sets the voltage for the use of the Poisson solve


#ifndef MAX
#define MAX(x,y) (((x)<(y))? (y):(x))
#endif
void Equipotential::applyFields(Scalar t,Scalar dt)
{
   int j,k,J=fields->getJ(),K=fields->getK();
   Scalar **Phi=fields->getphi();
   V=get_time_value(t);
   //  This applies the laplace solution at time t, scaled by the current voltage
   for(j=0;j<=J;j++) for(k=0;k<=K;k++)
      Phi[j][k]+=LaplaceSolution[j][k]*V;
//   applyBC(Phi);
   //	setQ(0.0,0);
}

void Equipotential::collect(Particle &p, Vector3& dxMKS) 
{
   //	setQ(*getQ()+p.get_q(),0);  // add the charge of the collected particle.
   //	delete &p;
   Conductor::collect(p, dxMKS);			// pass it up chain, will handle secondaries
}

void Equipotential::InitializeLaplaceSolution()
{
   int j,k,J,K;
   Scalar **rho;  //we need a zero array!  Wasteful, but we only need it once.
   Scalar Vt;

   J=fields->getJ();
   K=fields->getK();
   LaplaceSolution= new Scalar * [J+1];
   for(j=0;j<=J;j++) LaplaceSolution[j]= new Scalar [K+1];

   rho= new Scalar * [J+1];
   for(j=0;j<=J;j++)  { 
      rho[j]= new Scalar [K+1];
      for(k=0;k<=K;k++)
         rho[j][k]=0;
   }

   for(j=0;j<=J;j++) 
      for(k=0;k<=K;k++)
         LaplaceSolution[j][k]=0;
   Vt=V;
   V=1.0;
   applyBC(LaplaceSolution);
   V=Vt;

   if (!(fields->getBoltzmannFlag()))
      fields->getPoissonSolve()->laplace_solve(LaplaceSolution,rho,100,(fields->getpresidue()/**0.01*/));

   //now to delete those things we so painstakingly created
   for(j=0;j<=J;j++)
      delete [] rho[j];
   delete [] rho;
}

void Equipotential::applyBC(Scalar **phi) {

   int j,k;

   Scalar x1; // These can now be Scalar -- RT, 2003/12/09
   Scalar x2;
   adv_eval->add_indirect_variable("x1",&x1);
   adv_eval->add_indirect_variable("x2",&x2);
   Vector2 XMKS;
   Scalar VL;

   Vector2 **X = fields->get_grid()->getX();
   if (this->fill){
      Boundary ***NB;
      NB=fields->get_grid()->GetNodeBoundary();
      for (j=minJ; j<=maxJ; j++)
         for (k=minK; k<=maxK; k++)
            if (NB[j][k]==this) {
               XMKS = X[j][k];
               x1 = XMKS.e1();
               x2 = XMKS.e2();
               VL = V*adv_eval->Evaluate(VanalyticF.c_str());
               phi[j][k]=VL;
            }
   }
   else{
      // Set up the unit value on this equipotential
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
            for (k=MIN(k1,k2); k<=MAX(k1,k2); k++) {
               XMKS = X[j1][k];
               x1 = XMKS.e1();
               x2 = XMKS.e2();
               VL = V*adv_eval->Evaluate(VanalyticF.c_str());
               phi[j1][k]=VL;
            } 
         else if (k1==k2)											//	horizontal
            for (j=MIN(j1,j2); j<=MAX(j1,j2); j++) {
               XMKS = X[j][k1];
               x1 = XMKS.e1();
               x2 = XMKS.e2();
               VL = V*adv_eval->Evaluate(VanalyticF.c_str());
               phi[j][k1]=VL;
            }
         else {
            int jl,kl,jh,kh;
            /* start with the second point, advance one point at a time */
            for(j=2;j<4*abs(j2-j1)+4;j+=2) {
               jl=points[j-2];
               kl=points[j-1];
               jh=points[j];
               kh=points[j+1];
               if (jl==jh) for (k=MIN(kl,kh); k<=MAX(kh,kl); k++) {
                  XMKS = X[jl][k];
                  x1 = XMKS.e1();
                  x2 = XMKS.e2();
                  VL = V*adv_eval->Evaluate(VanalyticF.c_str());
                  phi[jl][k]=VL;
               }
               else {
                  XMKS = X[jl][kl];
                  x1 = XMKS.e1();
                  x2 = XMKS.e2();
                  VL = V*adv_eval->Evaluate(VanalyticF.c_str());
                  phi[jl][kl]=VL;

                  XMKS = X[jh][kh];
                  x1 = XMKS.e1();
                  x2 = XMKS.e2();
                  VL = V*adv_eval->Evaluate(VanalyticF.c_str());
                  phi[jh][kh]=VL;
               }
            }
         }
      }
   }
}





