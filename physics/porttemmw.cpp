/*
====================================================================

PORT_TEM_MW.CPP

Moving window TEM wave emitting port.


====================================================================
*/

#include "port.h"
#include "fields.h"
#include "ptclgrp.h"
#include "misc.h"
#include "porttemmw.h"
#include "sptlrgn.h"
#include "eval.h"
extern Evaluator *adv_eval;
//--------------------------------------------------------------------
//	Apply boundary conditions to fields locally.
//      We have to over-ride the fields both on the edge and
//      in the cell next to the edge.
void PortTEM_MW::updateEdgeFields(Scalar tE,Scalar tB,Scalar dt)
{
   if(dt==0) return;
   if(tE<pwDelay) return;
   int k;
   Grid*	grid = fields->get_grid();
   int J = grid->getJ();
   int K = grid->getK();
   

   Scalar dx = grid->dl1(0,0);  //UNIFORM mesh
   Scalar dy = grid->dl2(J-1,2);  //UNIFORM mesh
   Scalar dS = grid->dS3(J-1,K-1);
   Scalar A = pwAmplitude;
   Scalar w = pwFrequency;

   Vector3 ** iEdl = fields->getIntEdl();
   Vector3 ** iBdS = fields->getIntBdS();
   // px is the position in the plane wave
   // it's dx * (number of shifts + the fractional part of x)
   Scalar px = dx * (sRegion->getShiftNum() );

   // the E field at the edge of the leading edge cell
   // (J-1)  (should be unnecessary)
   Scalar Eydl;
//   Scalar Eydl = dy*A * cos(pwWavenumber * px + w * tE + pwPhase);
//   for(k=0;k<K;k++) iEdl[J-1][k]=Vector3(0,Eydl,0);

   // the E field at the edge of the system
   //  (J)
   px = dx * (1+sRegion->getShiftNum() );
   Eydl = dy * A * sin(pwWavenumber * px + w * tE + pwPhase);
   for(k=0;k<K;k++) iEdl[J][k]=Vector3(0,Eydl,0);
   if(EF) {  /* scale transversely by the shape function */
     Scalar x1,x2,local_time; // These can now be scalar -- RT, 2003/12/09
     adv_eval->add_indirect_variable("t",&local_time);
     adv_eval->add_indirect_variable("x1",&x1);
     adv_eval->add_indirect_variable("x2",&x2);
     local_time = tE;
     for(k=0;k<K;k++) {
       x1 = px;
       x2 = grid->getX()[J][k].e2();
       iEdl[J][k] *= adv_eval->Evaluate(AnalyticEy);
     }
   } 
     

   // the B field in the center of the last cell
   Scalar e = ELECTRON_CHARGE;
   Scalar m = ELECTRON_MASS;
   px = dx * (0.5+sRegion->getShiftNum() );
   if(! EF && ! NF) {  //do the default thing.
     Scalar BzdS = -dS*A*(pwWavenumber / w)* sin( pwWavenumber *px + w *tB +pwPhase) ;
     for(k=0;k<K;k++) iBdS[J-1][k] = Vector3(0,0,BzdS);
   }
   else {  // more complex calculation of B is needed.
     // assumption is made that the E,B relationship satisfies the
     // plane wave dispersion relation and that they're actually in phase.
     Scalar Wp2,nK,N; 
     Scalar x1,x2,local_time; // These can now be scalar -- RT, 2003/12/09
     adv_eval->add_indirect_variable("t",&local_time);
     adv_eval->add_indirect_variable("x1",&x1);
     adv_eval->add_indirect_variable("x2",&x2);
     local_time = tB;
     for(k=0;k<K;k++) {
       x1 = px;
       x2 = grid->getX()[J][k].e2();
       Scalar Scale = adv_eval->Evaluate(AnalyticEy);
       if(NF) {
         Scalar w2 = w*w;
         N = adv_eval->Evaluate(AnalyticN);
         Wp2 = N * e * e / (m * EPS0);
         if(Wp2 >= w2) 
           nK=0;
         else
           nK = sqrt(w2 - Wp2) / SPEED_OF_LIGHT;
       }
       else nK = pwWavenumber;
       Scalar BzdS = -dS*A*Scale*(nK / w)* sin( pwWavenumber *px + w *tB +pwPhase) ;
       iBdS[J-1][k] = Vector3(0,0,BzdS);
     }
   }

}

void PortTEM_MW::toNodes() 
{
   Scalar t = sRegion->getTime();
   Scalar dt = sRegion->get_dt();
   Vector3** BNode = fields->getBNode();

   Boundary::toNodes();  // this gets most of the fields right.

   // We have to adjust B3 only.  (Bz actually)
   

   int k;
   Grid*	grid = fields->get_grid();
   int J = grid->getJ();
   int K = grid->getK();
   

   Scalar dx = grid->dl1(0,0);  //UNIFORM mesh
   Scalar dy = grid->dl2(J-1,2);  //UNIFORM mesh
   Scalar dS = grid->dS3(J-1,K-1);
   Scalar A = pwAmplitude;
   Scalar w = pwFrequency;
   Scalar e = ELECTRON_CHARGE;
   Scalar m = ELECTRON_MASS;
   Scalar px;  // position at which we're calculating the field.

   px = dx * (0.5+sRegion->getShiftNum() );
   Scalar Bleft = -A*(pwWavenumber/w)* sin( pwWavenumber *px + w *t +pwPhase) ;

   px = dx * (1.5+sRegion->getShiftNum() );
   Scalar Bright = -A*(pwWavenumber/w) * sin( pwWavenumber *px + w *t +pwPhase) ;

   Scalar B3 = (Bright +Bleft) / 2.0;  // UNIFORM mesh assumption.
   for(k=0;k<K;k++)  BNode[J][k].set_e3(B3);
}
