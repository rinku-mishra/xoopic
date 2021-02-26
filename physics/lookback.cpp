/*
====================================================================

Started spring 94 for a 290q project.
Lookback Boundary Conditions

This is based on the paper by Ziolkowski, Madsen and Carpenter
"Three-Dimensional Computer modeling of Electromagnetic Fields: A
Global Lookback Lattice Truncation Scheme," Journal of Computational
Physics 50, 360-408 (1983).

The lookback BC is subject to catastrophic instability due to
numerical dispersion (the so-called late-time blow up). Another
defect is that the incident pulse must vanish; ie, a voltage
cannot be applied.

It would be nice to find
B, Goplen, "Boundary Conditions for MAGIC," Mission Research
Corporation Report, MRC/WDC-R-019, presented at the 23 Annual
Meeting APS Division of Plasma Physics, 12-16 October 1981.

The change from the paper is from XY to RZ.

====================================================================
*/

#include "port.h"
#include "fields.h"
#include "ptclgrp.h"
#include "exitport.h"
#include "lookback.h"

#ifdef _HAVE_CONFIG_H
#include <config.h>
#endif

#include <txc_streams.h>

Lookback::Lookback(oopicList <LineSegment> *segments) : Port(segments)
{
// getting memory for old intEdl and old intBdS to get dD/dt


//currentTime = new int;
//currentTime =0;

int currentTime=0;

// for now

int J= 20;
int K= 20;
Scalar x1f= .1;
Scalar x2f= .1;
Scalar dt=2e-12;
Scalar idt=1/dt;

Scalar R=sqrt(x1f*x1f+x2f*x2f);  // this needs to be changed for more
                                 // compicated boundaries

int MaxLookback=(int)(R*iSPEED_OF_LIGHT/dt+1);

/*oldB = new Vector3* [J+2*K][MaxLookback];
oldE = new Vector3* [J+2*K][MaxLookback];
dirvB = new Vector3* [J+2*K][MaxLookback];
dirvE = new Vector3* [J+2*K][MaxLookback];
edgePos = new Vector2[J+2*K]; */

oldB = new Vector3* [J+2*K];
oldE = new Vector3* [J+2*K];
dirvB = new Vector3* [J+2*K];
dirvE = new Vector3* [J+2*K];
edgePos = new Vector2[J+2*K]; 
N = new Vector3[J+2*K];

for (int i=0; i<(J+2*K); i++)
  {
    oldB[i] = new Vector3[MaxLookback];
    oldE[i] = new Vector3[MaxLookback];
    dirvB[i] = new Vector3[MaxLookback];
    dirvE[i] = new Vector3[MaxLookback]; 
  }   

Scalar i4PI=1/(4*M_PI);

}

//--------------------------------------------------------------------
//	Apply boundary conditions to fields locally.


void Lookback::applyFields(Scalar t,Scalar dt)
{
   if(dt==0) return;
  static int init_flag=1;
  Grid*   grid = fields->get_grid();
  Vector3 n;
  Scalar tr;
  Vector3 newE, Epast, newEdl;
  Vector2 R;
  Scalar magR;
  Scalar iR;
  Scalar iR3;
  int h;
  Scalar tau;

J=20;
K=20;
  if(init_flag)
    {
      x1f= .1;
      x2f= .1;
      dt=2e-12;
      idt=1/dt;
      newE=Vector3(0,0,0);
      Epast=Vector3(0,0,0);
      currentTime =0;
      TXSTRSTD::cout << "first time through" << TXSTRSTD::endl;

      Scalar maxr=sqrt(x1f*x1f+x2f*x2f);  // this needs to be changed for more 
                                       // compicated boundaries

      MaxLookback=(int)(maxr*iSPEED_OF_LIGHT/dt+1);

      i4PI=1/(4*M_PI);

      for (int ii=0; ii<(J+2*K); ii++)
			{
				for (int jj=0; jj<MaxLookback; jj++)
					{
						oldB[ii][jj] = Vector3(0,0,0);
						oldE[ii][jj] = Vector3(0,0,0);
						dirvB[ii][jj] = Vector3(0,0,0);
						dirvE[ii][jj] = Vector3(0,0,0);
					}
			}

      for (int i=0; i<(J+2*K); i++)
			{
				int k;
				for (k=0; k<K; k++)
					{
						edgePos[i]=grid->getMKS(0,k);
						N[i] = Vector3(-1,0,0);
					}
				for (int j=0; j<J; j++)
					{
						edgePos[K+j]=grid->getMKS(j,K);
						N[i] =Vector3(0,1,0);
					}
				for (k=0; k<K; k++)
					{
						edgePos[K+J+k]=grid->getMKS(J-1,k);
						N[i] = Vector3(1,0,0);
					}
			}
      init_flag=0;
	}
  
  if (alongx2()) //      vertical
	  {
		  for (int k=MIN(k1,k2); k<MAX(k1,k2); k++)
			  {
				  for (int i=0; i<(J+K+K); i++)
					  {
						  R=(grid->getMKS(j2-1,k))-edgePos[i];
						  magR=R.magnitude();
						  tr=t-magR*iSPEED_OF_LIGHT;
						  if ((tr>0.0) && (magR>0.0))
							  {
								  iR=1/magR;
								  iR3=1/(magR*magR*magR);
								  h=(int)(tr/dt);
								  tau=tr-h*dt;
		  h=currentTime-h;
		  if (h<0) h+=MaxLookback;
		  if (h<0) TXSTRSTD::cout << "you messed up on past values"<< TXSTRSTD::endl;
		  Epast=oldE[i][h] + tau*dirvE[i][h];
		  newE+=grid->dS1(j2-1,k)*i4PI*((N[i].cross(Epast)).cross(Vector3(R.e1(),R.e2(),0))*iR3 + 
			N[i].jvMult(Epast)*iR3*(Vector3(R.e1(),R.e2(),0))-N[i].cross(dirvB[i][h])*iR);
		}
	    }
	  newEdl=newE.jvMult(grid->dl(j2-1,k));
//	  fields->setIntEdl(j2-1,k,1,newEdl.e1());
	  fields->setIntEdl(j2,k,2,newEdl.e2());
	  fields->setIntEdl(j2,k,3,newEdl.e3());
	  newE=Vector3(0,0,0);
	}
    }
  else //      horizontal
    {
      for (int j=MIN(j1,j2); j<MAX(j1,j2); j++)
	{
	  fields->set_iC1(j, k1, 0);
	  fields->set_iC3(j, k1, 0);
	}
      fields->set_iC3(j2, k1, 0);
    }

// Saving old data
if (currentTime==0)
  {
    // special case on axis
    int k=0;
    oldB[k][currentTime]=fields->getIntBdS()[0][k].kcDivide(grid->dS(0,k));
    oldB[k][currentTime]=Vector3(oldB[k][currentTime].e1(),0,oldB[k][currentTime].e3());
    oldE[k][currentTime]=fields->getIntEdl()[0][k].kcDivide(grid->dl(0,k));
    oldE[k][currentTime]=Vector3(oldE[k][currentTime].e1(),oldE[k][currentTime].e2(),0);
    dirvB[k][currentTime]=idt*(oldB[k][currentTime]-oldB[k][MaxLookback-1]);
    dirvE[k][currentTime]=idt*(oldE[k][currentTime]-oldE[k][MaxLookback-1]);
    for (k=1; k<K; k++)
      {
	oldB[k][currentTime]=fields->getIntBdS()[0][k].kcDivide(grid->dS(0,k));
	oldE[k][currentTime]=fields->getIntEdl()[0][k].kcDivide(grid->dl(0,k));
	dirvB[k][currentTime]=idt*(oldB[k][currentTime]-oldB[k][MaxLookback-1]);
	dirvE[k][currentTime]=idt*(oldE[k][currentTime]-oldE[k][MaxLookback-1]);
      }
    for (int j=0; j<J; j++)
      {
	oldB[K+j][currentTime]=fields->getIntBdS()[j][K].kcDivide(grid->dS(j,K-1));
	oldE[K+j][currentTime]=fields->getIntEdl()[j][K].kcDivide(grid->dl(j,K-1));
	dirvB[K+j][currentTime]=idt*(oldB[K+j][currentTime] - oldB[K+j][MaxLookback-1]);
	dirvE[K+j][currentTime]=idt*(oldE[K+j][currentTime] - oldE[K+j][MaxLookback-1]);
      }
    // special case on axis
    k=0;
    oldB[K+J][currentTime]=fields->getIntBdS()[J-1][k].kcDivide(grid->dS(J-1,k));
    oldB[K+J][currentTime]=Vector3(oldB[K+J][currentTime].e1(),0,oldB[K+J][currentTime].e3());
    oldE[K+J][currentTime]=fields->getIntEdl()[J-1][k].kcDivide(grid->dl(J-1,k));
    oldE[K+J][currentTime]=Vector3(oldE[K+J][currentTime].e1(),oldE[K+J][currentTime].e2(),0);
    dirvB[K+J][currentTime]=idt*(oldB[K+J][currentTime]-oldB[K+J][MaxLookback-1]);
    dirvE[K+J][currentTime]=idt*(oldE[K+J][currentTime]-oldE[K+J][MaxLookback-1]);
    for (k=1; k<K; k++)
      {
	oldB[K+J+k][currentTime]=fields->getIntBdS()[J-1][k].kcDivide(grid->dS(J-1,k));
	oldE[K+J+k][currentTime]=fields->getIntEdl()[J-1][k].kcDivide(grid->dl(J-1,k));
	dirvB[K+J+k][currentTime]=idt*(oldB[K+J+k][currentTime]-oldB[K+J+k][MaxLookback-1]);
	dirvE[K+J+k][currentTime]=idt*(oldE[K+J+k][currentTime]-oldE[K+J+k][MaxLookback-1]);
      }
  }
else
  {
    // special case on axis
    int k=0;
    oldB[k][currentTime]=fields->getIntBdS()[0][k].kcDivide(grid->dS(0,k));
    oldB[k][currentTime]=Vector3(oldB[k][currentTime].e1(),0,oldB[k][currentTime].e3());
    oldE[k][currentTime]=fields->getIntEdl()[0][k].kcDivide(grid->dl(0,k));
    oldE[k][currentTime]=Vector3(oldE[k][currentTime].e1(),oldE[k][currentTime].e2(),0);
    dirvB[k][currentTime]=idt*(oldB[k][currentTime]-oldB[k][currentTime-1]);
    dirvE[k][currentTime]=idt*(oldE[k][currentTime]-oldE[k][currentTime-1]);
    for (k=1; k<K; k++)
      {
	oldB[k][currentTime]=fields->getIntBdS()[0][k].kcDivide(grid->dS(0,k));
	oldE[k][currentTime]=fields->getIntEdl()[0][k].kcDivide(grid->dl(0,k));
	dirvB[k][currentTime]=idt*(oldB[k][currentTime]-oldB[k][currentTime-1]);
	dirvE[k][currentTime]=idt*(oldE[k][currentTime]-oldE[k][currentTime-1]);
      }
    for (int j=0; j<J; j++)
      {
	oldB[K+j][currentTime]=fields->getIntBdS()[j][K].kcDivide(grid->dS(j,K-1));
	oldE[K+j][currentTime]=fields->getIntEdl()[j][K].kcDivide(grid->dl(j,K-1));
	dirvB[K+j][currentTime]=idt*(oldB[K+j][currentTime]-oldB[K+j][currentTime-1]);
	dirvE[K+j][currentTime]=idt*(oldE[K+j][currentTime]-oldE[K+j][currentTime-1]);
      }
    // special case on axis
    k=0;
    oldB[K+J+k][currentTime]=fields->getIntBdS()[J-1][k].kcDivide(grid->dS(J-1,k));
    oldB[K+J+k][currentTime]=Vector3(oldB[K+J+k][currentTime].e1(),0,oldB[K+J+k][currentTime].e3());
    oldE[K+J+k][currentTime]=fields->getIntEdl()[J-1][k].kcDivide(grid->dl(J-1,k));
    oldE[K+J+k][currentTime]=Vector3(oldE[K+J+k][currentTime].e1(),oldE[K+J+k][currentTime].e2(),0);
    dirvB[K+J+k][currentTime]=idt*(oldB[K+J+k][currentTime]-oldB[K+J+k][currentTime-1]);
    dirvE[K+J+k][currentTime]=idt*(oldE[K+J+k][currentTime]-oldE[K+J+k][currentTime-1]);
    for (k=1; k<K; k++)
      {
	oldB[K+J+k][currentTime]=fields->getIntBdS()[J-1][k].kcDivide(grid->dS(J-1,k));
	oldE[K+J+k][currentTime]=fields->getIntEdl()[J-1][k].kcDivide(grid->dl(J-1,k));
	dirvB[K+J+k][currentTime]=idt*(oldB[K+J+k][currentTime]-oldB[K+J+k][currentTime-1]);
	dirvE[K+J+k][currentTime]=idt*(oldE[K+J+k][currentTime]-oldE[K+J+k][currentTime-1]);
      }
  }
currentTime++;
if (currentTime>=MaxLookback) currentTime=0;
}

//--------------------------------------------------------------------
//	Set the passive bc for fields at the port.  Currently just a
//	copy of those for conductor.

void Lookback::setPassives()
{
  int	inc;
/*  if (alongx2()) //      vertical
    {
      for (int k=MIN(k1,k2); k<MAX(k1,k2); k++)
	{
	  fields->set_iC2(j1, k, 0);       
	  fields->set_iC3(j1, k, 0);
	}
      fields->set_iC3(j1, k2, 0);
    }
  else //      horizontal
    {
      for (int j=MIN(j1,j2); j<MAX(j1,j2); j++)
	{
	  fields->set_iC1(j, k1, 0);
	  fields->set_iC3(j, k1, 0);
	}
      fields->set_iC3(j2, k1, 0);
    }
*/
}

//--------------------------------------------------------------------
//	Lookback::emit() simply deletes Particles in its stack.  May add some
//	diagnostics for particles collected in the future.
#ifndef __linux__
#pragma argsused
#endif
ParticleList& Lookback::emit(Scalar t,Scalar dt)
{
  while(!particleList.isEmpty())
	 {
		Particle* p = particleList.pop();
		delete p;
	 }
  return particleList;
}

