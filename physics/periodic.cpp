/*
==================================================================

periodic.cpp
2.1 (keith) Creation.  get poisson solve to work
2.11 (KC)   displaced the particles off the periodic boundary.  Had a
             particle land right on the boundary 
2.12 (KC 11-11-97) downward moving particle positions were being set to zero.


====================================================================
*/

#include "periodic.h"
#include "fields.h"
#include "ptclgrp.h"
#include "misc.h"
#include "psolve.h"

Periodic::Periodic(oopicList <LineSegment> *segments,int _PeriodicFlagX1, 
int _PeriodicFlagX2) : Boundary(segments)
{
	PeriodicFlagX1 = _PeriodicFlagX1;
	PeriodicFlagX2 = _PeriodicFlagX2;

	J = MAX(j1,j2);
	K = MAX(k1,k2);

	if (PeriodicFlagX2)
		iCX1 = new Vector3[J + 1];
	if (PeriodicFlagX1)
		iCX2 = new Vector3[K + 1];
	
//	deltaVertical = Vector2(j1*1e-6 + 1e-20, 0);
//	deltaHorizontal = Vector2(0, k1*1e-6 + 1e-20);
	
	BoundaryType = PERIODIC;
	BCType = PERIODIC_BOUNDARY;
}

Periodic::~Periodic()
{
	if (PeriodicFlagX2)
		delete[] iCX1;
	if (PeriodicFlagX1)
		delete[] iCX2;
}

void	Periodic::collect(Particle& p, Vector3& dxMKS)
{
  Grid *grid = fields->get_grid();
  Boundary *bPtr;
  // vertical boundary
  if(p.get_x().e1()==0 || p.get_x().e1()==J) {
	 
	 if(p.get_u().e1()>0) //rightward moving particle
		p.set_x(Vector2(0+1e-20,p.get_x().e2()));
	 else //leftward moving particle
		p.set_x( Vector2((grid->getJ())*(1-1e-6) -1e-20,p.get_x().e2()));
  }
  else
	 {
	 if(p.get_u().e2()>0) //upward moving particle
		p.set_x(Vector2(p.get_x().e1(),0+1e-20));
	 else //downward moving particle
		p.set_x( Vector2(p.get_x().e1(),(grid->getK())*(1-1e-6) -1e-20));
  }
  // finish the push
  Vector2 x = p.get_x();
  if ((bPtr = fields->translateAccumulate(x, dxMKS, p.get_q()/fields->get_dt()))) {
	 p.set_x(x);
	 //	send this particle to boundary
	 bPtr->collect(p, dxMKS);
  }
  else {
	 p.set_x(x);
	 particleList.add(&p);
  }
}

void Periodic::putCharge_and_Current(Scalar t)
{
  int nSpecies = fields->getnSpecies();
  Vector3 **I=fields->getI();
  int J = fields->getJ();
  int K = fields->getK();

  if (fields->getElectrostatic()==0){
	 if (PeriodicFlagX2)
		for (int j=0; j<=J; j++){
		  I[j][0].set_e1(I[j][0].e1()+I[j][K].e1());  
		  I[j][K].set_e1(I[j][0].e1());
		}
	 if (PeriodicFlagX1)
		for (int k=0; k<=K; k++){
		  I[0][k].set_e2(I[0][k].e2() + I[J][k].e2());
		  I[J][k].set_e2(I[0][k].e2());
		}
  }
}

void Periodic::setBoundaryMask(Grid &grid)
{
	if (PeriodicFlagX2)
		for (int j=0; j<J; j++){
			grid.setBoundaryMask1(j,0,this,1);
			grid.setBoundaryMask1(j,K,this,-1);
		}
	if (PeriodicFlagX1)
		for (int k=0; k<K; k++){
			grid.setBoundaryMask2(0,k,this,1);
			grid.setBoundaryMask2(J,k,this,-1);
		}
}

void Periodic::toNodes()
{
	Vector3** ENode = fields->getENode();
	Scalar** Phi = fields->getphi();
	Grid* grid = fields->get_grid();
	int j;
	int k;
	if (fields->getElectrostatic()){
		if (PeriodicFlagX2){
			j=0;
			ENode[j][0].set_e2((Phi[j][K-1]-Phi[j][1])/(grid->dl2(j,0)+grid->dl2(j,K-1)));
			ENode[j][K].set_e2(ENode[j][K].e2());
			for (j=1; j<J; j++){
				ENode[j][0].set_e2((Phi[j][K-1]-Phi[j][1])/(grid->dl2(j,0)+grid->dl2(j,K-1)));
				ENode[j][K].set_e2(ENode[j][K].e2());
				ENode[j][0].set_e1((Phi[j-1][0]-Phi[j+1][0])/(grid->dl1(j-1,0)+grid->dl1(j,0)));
				ENode[j][K].set_e1(ENode[j][0].e1());
			}
			ENode[j][0].set_e2((Phi[j][K-1]-Phi[j][1])/(grid->dl2(j,0)+grid->dl2(j,K-1)));
			ENode[j][K].set_e2(ENode[j][K].e2());
		}
		if (PeriodicFlagX1){
			k=0;
			ENode[0][k].set_e1((Phi[J-1][k]-Phi[1][k])/(grid->dl1(0,k)+grid->dl1(J-1,k)));
			ENode[J][k].set_e1(ENode[0][k].e1());
			for (k=1; k<K; k++){
				ENode[0][k].set_e1((Phi[J-1][k]-Phi[1][k])/(grid->dl1(0,k)+grid->dl1(J-1,k)));
				ENode[J][k].set_e1(ENode[0][k].e1());
				ENode[0][k].set_e2((Phi[0][k-1]-Phi[0][k+1])/(grid->dl2(0,k-1)+grid->dl2(0,k)));
				ENode[J][k].set_e2(ENode[0][k].e2());
			}
			ENode[0][k].set_e1((Phi[J-1][k]-Phi[1][k])/(grid->dl1(0,k)+grid->dl1(J-1,k)));
			ENode[J][k].set_e1(ENode[0][k].e1());
		}
	}
	else{
		Vector3** BNode = fields->getBNode();
		Vector3** intEdl = fields->getIntEdl();
		Vector3** intBdS = fields->getIntBdS();
		if (PeriodicFlagX2){
			Scalar w1p, w1m, w2m,w2p;
			k=0;
			j=0;
			w2m = grid->dl2(j, k)/(grid->dl2(j, K-1) + grid->dl2(j, k));
			w2p = 1 - w2m;
			w1p = 1;
			w1m = 0;
			ENode[j][k].set_e1(w1p*intEdl[j][k].e1()/grid->dl1(j, k));
			ENode[j][k].set_e2(w2p*intEdl[j][k].e2()/grid->dl2(j, k) 
												 + w2m*intEdl[j][K-1].e2()/grid->dl2(j, K-1));
			ENode[j][k].set_e3(intEdl[j][k].e3()/grid->dl3(j,k));
			BNode[j][k].set_e1(intBdS[j][k].e1()*(w2p/grid->dS1(j, k)) 
												 + intBdS[j][K-1].e1()*w2m/grid->dS1(j, K-1));
			BNode[j][k].set_e2(intBdS[j][k].e2()*(w1p/grid->dS2(j, k)));
			BNode[j][k].set_e3(intBdS[j][k].e3()*w2p/grid->dS3(j, k)
												 + intBdS[j][K-1].e3()*w2m/grid->dS3(j, K-1));
			ENode[j][K] = ENode[j][k];
			BNode[j][K] = BNode[j][k];

			j=1;
			w2m = grid->dl2(j, k)/(grid->dl2(j, K-1) + grid->dl2(j, k));
			w2p = 1 - w2m;
			for (; j<J; j++)					// all interior points
				{
					w1m = grid->dl1(j, k)/(grid->dl1(j-1, k) + grid->dl1(j, k));
					w1p = 1 - w1m;		
					ENode[j][k].set_e1(w1p*intEdl[j][k].e1()/grid->dl1(j, k) 
														 + w1m*intEdl[j-1][k].e1()/grid->dl1(j-1, k));
					ENode[j][k].set_e2(w2p*intEdl[j][k].e2()/grid->dl2(j, k) 
														 + w2m*intEdl[j][K-1].e2()/grid->dl2(j, K-1));
					ENode[j][k].set_e3(intEdl[j][k].e3()/grid->dl3(j,k));
					BNode[j][k].set_e1(intBdS[j][k].e1()*(w2p/grid->dS1(j, k)) 
														 + intBdS[j][K-1].e1()*w2m/grid->dS1(j, K-1));
					BNode[j][k].set_e2(intBdS[j][k].e2()*(w1p/grid->dS2(j, k)) 
														 + intBdS[j-1][k].e2()*w1m/grid->dS2(j-1, k));
					BNode[j][k].set_e3(intBdS[j][k].e3()*w1p*w2p/grid->dS3(j, k)
														 + intBdS[j-1][k].e3()*w1m*w2p/grid->dS3(j-1, k)
														 + intBdS[j][K-1].e3()*w1p*w2m/grid->dS3(j, K-1)
														 + intBdS[j-1][K-1].e3()*w1m*w2m/grid->dS3(j-1, K-1));
					
					ENode[j][K] = ENode[j][k];
					BNode[j][K] = BNode[j][k];
				}
			w1m = grid->dl1(j, k)/(grid->dl1(j-1, k) + grid->dl1(j, k));
			w1p = 1 - w1m;		
			ENode[j][k].set_e1(intEdl[j-1][k].e1()/grid->dl1(j-1, k));
			ENode[j][k].set_e2(w2p*intEdl[j][k].e2()/grid->dl2(j, k) 
												 + w2m*intEdl[j][K-1].e2()/grid->dl2(j, K-1));
			ENode[j][k].set_e3(intEdl[j][k].e3()/grid->dl3(j,k));
			BNode[j][k].set_e1(intBdS[j][k].e1()*(w2p/grid->dS1(j, k)) 
												 + intBdS[j][K-1].e1()*w2m/grid->dS1(j, K-1));
			BNode[j][k].set_e2(intBdS[j-1][k].e2()/grid->dS2(j-1, k));
			BNode[j][k].set_e3(intBdS[j-1][k].e3()*w2p/grid->dS3(j-1, k)
												 + intBdS[j-1][K-1].e3()*w2m/grid->dS3(j-1, K-1));
			
			ENode[j][K] = ENode[j][k];
			BNode[j][K] = BNode[j][k];
		}
		if (PeriodicFlagX1){
			for (k=0; k<=K; k++){
				ENode[0][k].set_e1((Phi[J-1][k]-Phi[1][k])/(grid->dl1(0,k)+grid->dl1(J-1,k)));
				ENode[J][k].set_e1(ENode[0][k].e1());
			}
			for (k=1; k<K; k++){
				ENode[0][k].set_e2((Phi[0][k-1]-Phi[0][k+1])/(grid->dl2(0,k-1)+grid->dl2(0,k)));
				ENode[J][k].set_e2(ENode[0][k].e2());
			}
		}
	}
}

void Periodic::applyFields(Scalar t, Scalar dt)
{
   if(dt==0) return;

	if (fields->getElectrostatic()==0){

	Vector3** intEdl = fields->getIntEdl();
	Vector3** intBdS = fields->getIntBdS();
	Vector3** I = fields->getI();
	Vector3** iL = fields->getiL();
	Vector3 *iL0, *iLk1, *iLj1, *Bk1, *Bj1, *B0;

	int j,k;
#ifdef OLD_CODE
	if (PeriodicFlagX2){
//		for (j=0; j<=J; j++){
		j=0;
		intEdl[j][K] += intEdl[j][0]+I[j][0];
//		intEdl[j][K] /= 2.0;
		intEdl[j][0] = intEdl[j][K];
		for (j=1; j<=J; j++){
			intEdl[j][K] += intEdl[j][0]+I[j][0];
//			intEdl[j][K] /= 2.0;
			intEdl[j][0] = intEdl[j][K];
		}
	}
	if (PeriodicFlagX1){
		for (k=0; k<=K; k++){
			intEdl[J][k] += intEdl[0][k];
			intEdl[J][k] /= 2.0;
			intEdl[0][k] = intEdl[J][k];
		}
	}
#endif


	if (PeriodicFlagX2){
		j=0;
		k=0;
		iL0 = &iL[j][0];
		iLk1 = &iL[j][K-1];
		intEdl[0][k] += dt*iCX1[0].jvMult(Vector3(
												 intBdS[0][k].e3()*iL0->e3() - intBdS[0][K-1].e3()*iLk1->e3(),
												 -intBdS[0][k].e3()*iL0->e3(),
												 intBdS[0][K-1].e1()*iLk1->e1() - intBdS[0][k].e1()*iL0->e1()
												 + intBdS[0][k].e2()*iL0->e2()) 
																				 - I[0][k]);
		intEdl[0][K] = intEdl[0][k];
		for (j=1; j<=J; j++){
			iLk1 = &iL[j][K-1];
			iLj1 = &iL[j-1][k];
			iL0 = &iL[j][k];
			Bk1 = &intBdS[j][K-1];
			Bj1 = &intBdS[j-1][k];
			B0 = &intBdS[j][k];
			intEdl[j][k] += dt*iCX1[j].jvMult(Vector3(
														 B0->e3()*iL0->e3() - Bk1->e3()*iLk1->e3(),
														 -B0->e3()*iL0->e3() + Bj1->e3()*iLj1->e3(),
														 Bk1->e1()*iLk1->e1() - B0->e1()*iL0->e1()
														 + B0->e2()*iL0->e2() - Bj1->e2()*iLj1->e2()) 
																						 - I[j][k]);	
			intEdl[j][K] = intEdl[j][k];
		}
	}
}
}
Scalar Periodic::getBoltzmannFlux()
{
	return(0);
}

void Periodic::setPassives()
{

 	if (PeriodicFlagX2){
		for (int j=0; j<=J; j++){
			iCX1[j] = fields->getiC()[j][0];
			fields->set_iC1(j,0,0);
			fields->set_iC2(j,0,0);	
			fields->set_iC3(j,0,0);
			fields->set_iC1(j,K,0);
			fields->set_iC2(j,K,0);	
			fields->set_iC3(j,K,0);
		}
	}
}
