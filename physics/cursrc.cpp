//====================================================================
#ifdef	COMMENTS

CURSRC.CPP 

Provides a floating conductor boundary with a time-dependent external
current source attached.  This will only work properly with a single
segment at this time.

CAUTION: this initial implementation is for uniform meshes ONLY, and 
currently is only configured for vertical left electrode.

0.90  (JohnV 06 Jan 2001) Creation.
0.91  (JohnV 02 Apr 2001) Debugged.
0.92  (JohnV 06 Apr 2001) Add Dump and Restore to preserve total charge

#endif	//COMMENTS
//====================================================================

#include "cursrc.h"
#include "fields.h"
#include "ptclgrp.h"
#include "psolve.h"

CurrentSource::CurrentSource(oopicList <LineSegment> *segments)
	: Equipotential(segments)
{
  BoundaryType = CURRENTSOURCE;
  init = TRUE;
  restore = FALSE;
}

CurrentSource::~CurrentSource() 
{
}

// perform initializations that require a pointer to fields
void CurrentSource::initialize(Scalar t, Scalar dt)
  /* throw(Oops) */{
area = 0;
 int flag = 0;
int J=fields->getJ(),K=fields->getK();
// (xMKS.e1(),xMKS.e2()) = MKS values at grid location (0,0), (axial,radial);
Vector2 xMKS = fields->getMKS(0,0);
Scalar x1min=xMKS.e1(); // minimum axial coordinate
Scalar x2min=xMKS.e2(); // minimum radial coordinate
  
printf("j1=%d k1= %d j2= %d k2= %d\n",j1,k1,j2,k2);
if (alongx2()) 	// vertical boundary 
	for (int k=k1; k<=k2; k++) 
	{
	  flag = fields->get_grid()->getPeriodicFlagX2();
	   
		Scalar areaX =  fields->get_grid()->dS1Prime(j1,k);
		if (k==k1) // bottom of current source 
			{
			if (k!=0) areaX *=0.5; // too much area
			if ((k==0) && (x2min!=0)) areaX*=0.5;
			}
		if(k==k2 && (k==K && flag)) { break; }
		if (k==k2 && k!=K) areaX *= 0.5; // O.K. at top
		area += areaX;
	}

else	// horizontal boundary 
	for (int j=j1; j<=j2; j++) 
	{
	  flag = fields->get_grid()->getPeriodicFlagX1();
		Scalar areaX =  fields->get_grid()->dS2Prime(j,k1);
		if (j==j1) // left side of current source
			{
			if (j!=0) areaX *=0.5; // too much area
			if ((j==0) && (x1min!=0)) areaX*=0.5;
			}
		if(j==j2 && (j==J && flag)) { break; }
		if (j==j2 && j!=J) areaX *= 0.5; // O.K. at right
		area += areaX;
	}

 printf("\nCurrent source area = %g", area);

if(restore == FALSE)
	{
    try{
	    initializevoltage();
    }
    catch(Oops& oops){
      oops.prepend("CurrentSource::initialize: Error: \n"); //CurrentSource::applyFields
      throw oops;
    }
	}
Qtotal -= get_time_value((float)t)*dt;
 
init = FALSE;
}

void CurrentSource::initializevoltage(void) 
  /* throw(Oops) */{
  
printf("\ninitV=%g", initV);

int J=fields->getJ(),K=fields->getK();
// (xMKS.e1(),xMKS.e2()) = MKS values at grid location (0,0), (axial,radial);
Vector2 xMKS = fields->getMKS(0,0);
Scalar x1min=xMKS.e1(); // minimum axial coordinate
Scalar x2min=xMKS.e2(); // minimum radial coordinate
  
Scalar sumrho, sumPhiP, sumPhiL,areaV,eps;
int Ja,Jb,Ka,Kb;
 int flag = 0;
sumrho=sumPhiP=sumPhiL=0;
// Problems on top or right because :
// fields->get_grid()->dl1(JMax,k) = 0 &fields->get_grid()->dl2(j,KMax) = 0
Scalar dx = fields->get_grid()->dl1(0,0);
Scalar dy = fields->get_grid()->dl2(0,0);
if (alongx2()) // vertical
	{
	if (j1==J) //vertical, on right
		{
		Ja=j2;
		Jb=j2-1;
		eps=fields->get_epsi(j1-1,k1);
		} 
	else // vertical, on left
		{
		Ja=j1;
		Jb=j1+1;
		eps=fields->get_epsi(j1,k1);
		}

	flag = fields->get_grid()->getPeriodicFlagX2();

	if(k1!=0) //  collect charge on bottom
		{
		sumPhiP += dx*fields->get_grid()->dS2Prime(Ja,k1)*fields->getPhiP(Ja,k1-1)/dy;
		sumPhiL += dx*fields->get_grid()->dS2Prime(Ja,k1)*(1.0 - LaplaceSolution[Ja][k1-1])/dy;
		}
	if(k2!=K) //  collect charge on top
		{
		sumPhiP += dx*fields->get_grid()->dS2Prime(Ja,k2)*fields->getPhiP(Ja,k2+1)/dy;
		sumPhiL += dx*fields->get_grid()->dS2Prime(Ja,k2)*(1.0 - LaplaceSolution[Ja][k2+1])/dy;
		}
	for (int k=k1; k<=k2; k++) 
      		{
		areaV = fields->get_grid()->dS1Prime(Ja,k);
		// 1/2 area on current source ends UNLESS already at grid limits where dS1Prime returns 1/2 area already
		if (k==k1) // bottom of current source 
			{
			if (k!=0) areaV *=0.5; // too much area
			if ((k==0) && (x2min!=0)) areaV*=0.5;
			}
		if(k==k2 && (k==K && flag)) { break; }
		if (k==k2 && k!=K) areaV *= 0.5; // O.K. at top
		sumrho += fields->getrho(Ja,k)*areaV*dx*0.5;
		sumPhiP += fields->getPhiP(Jb,k)*areaV;
		sumPhiL += areaV*(1.0 - LaplaceSolution[Jb][k]);
      		}
	}
else if (alongx1()) // horizontal
	{
	if (k1==K) //horizontal, on top
		{
		Ka=k2;
		Kb=k2-1;
		eps=fields->get_epsi(j1,k1-1);
		} 
	else	// horizontal, on bottom
		{
		Ka=k1;
		Kb=k1+1;
		eps=fields->get_epsi(j1,k1);
		}

	flag = fields->get_grid()->getPeriodicFlagX1();

	if(j1!=0) // collect charge on left
		{
		sumPhiP += dy*fields->get_grid()->dS1Prime(j1,Ka)*fields->getPhiP(j1-1,Ka)/dx;
		sumPhiL += dy*fields->get_grid()->dS1Prime(j1,Ka)*(1.0 - LaplaceSolution[j1-1][Ka])/dx;
		}
	if(j2!=J) // collect charge on right
		{
		sumPhiP += dy*fields->get_grid()->dS1Prime(j2,Ka)*fields->getPhiP(j2+1,Ka)/dx;
		sumPhiL += dy*fields->get_grid()->dS1Prime(j2,Ka)*(1.0 - LaplaceSolution[j2+1][Ka])/dx;
       		}
	for(int j=j1; j<=j2-flag; j++) 
		{
		areaV = fields->get_grid()->dS2Prime(j,Ka);
		if (j==j1) // left side of current source
			{
			if (j!=0) areaV *=0.5; // too much area
			if ((j==0) && (x1min!=0)) areaV*=0.5;
			}
		if(j==j2 && (j==J && flag)) { break; } // don't double count periodic boundaries
		if (j==j2 && j!=J) areaV *= 0.5; // O.K. at right
		sumrho += fields->getrho(j, Ka)*fields->get_grid()->halfcellVolume(j,Kb);
		sumPhiP += fields->getPhiP(j,Kb)*areaV;
		sumPhiL += areaV*(1.0 - LaplaceSolution[j][Kb]);
		}
	}
else
	{
  stringstream ss (stringstream::in | stringstream::out);
  ss << "CurrentSource::initializevoltag: Error: \n" << 
	  "current source must be along left,right,bottom, or top !\n";

	std::string msg;
  ss >> msg;
  Oops oops(msg);
  throw oops;    // exit() CurrentSource::initialize

	}
printf("dx = %g dy = %g eps =%g\n",dx,dy,eps);
Qtotal = initV*sumPhiL - sumPhiP;
Qtotal *= (eps/dx);
//Qtotal *= Scalar(normal);
Qtotal -= sumrho;
}
//  This function sets the voltage for the use of the Poisson solve; see Vahedi
// et al for the meaning for the variables. These must be modified to run on a 
// nonuniform mesh. Also assumes uniform epsilon.

void CurrentSource::applyFields(Scalar t,Scalar dt)
  /* throw(Oops) */{
static Scalar QtotalOld = 0;
if (init) {
  try{
    initialize(t, dt);
  }
  catch(Oops& oops2){
    oops2.prepend("CurrentSource::applyFields: Error: \n");
    throw oops2;
  }
}
 int flag=0;
int J=fields->getJ(),K=fields->getK();
// (xMKS.e1(),xMKS.e2()) = MKS values at grid location (0,0), (axial,radial);
Vector2 xMKS = fields->getMKS(0,0);
Scalar x1min=xMKS.e1(); // minimum axial coordinate
Scalar x2min=xMKS.e2(); // minimum radial coordinate

Qtotal += get_time_value((float)t)*dt; // add charge from external source into Qtotal;
QtotalOld = Qtotal;
Scalar sumrho, sumPhiP, sumPhiL,areaV,V,eps;
Scalar dx = fields->get_grid()->dl1(0,0);
Scalar dy = fields->get_grid()->dl2(0,0);
int Ja,Jb,Ka,Kb; // number of nodes in boundary
sumrho=sumPhiP=sumPhiL=0;
if (alongx2()) // vertical
	{
	  flag = fields->get_grid()->getPeriodicFlagX2();
	Ja=j1;Jb=j1+1; 
	if (j1==J) // vertical, on right 
		{
		Ja=j2;
		Jb=j2-1;
		eps = fields->get_epsi(j1-1,k1);
		} 
	else // vertical, on left
		{
		Ja=j1;
		Jb=j1+1;
		eps = fields->get_epsi(j1,k1);
		}
	if(k1!=0) //  collect charge on bottom
		{
		sumPhiP += dx*fields->get_grid()->dS2Prime(Ja,k1)*fields->getPhiP(Ja,k1-1)/dy;
		sumPhiL += dx*fields->get_grid()->dS2Prime(Ja,k1)*(1.0 - LaplaceSolution[Ja][k1-1])/dy;
		}
	if(k2!=K) //  collect charge on top
	  	{
		sumPhiP += dx*fields->get_grid()->dS2Prime(Ja,k2)*fields->getPhiP(Ja,k2+1)/dy;
		sumPhiL += dx*fields->get_grid()->dS2Prime(Ja,k2)*(1.0 - LaplaceSolution[Ja][k2+1])/dy;
		}
	for (int k=k1; k<=k2; k++) 
		{
		areaV = fields->get_grid()->dS1Prime(Ja,k);
		if (k==k1) // bottom of current source 
			{
			if (k!=0) areaV *=0.5; // too much area
			if ((k==0) && (x2min!=0)) areaV*=0.5;
			}
		if(k==k2 && (k==K && flag)) { break; }
		if (k==k2 && k!=K) areaV *= 0.5; // O.K. at top
		sumrho += fields->getrho(Ja,k)*areaV*dx*0.5;
		sumPhiP += fields->getPhiP(Jb,k)*areaV;
		sumPhiL += areaV*(1.0 - LaplaceSolution[Jb][k]);
	    	}
	}
else // horizontal
	{
	  flag = fields->get_grid()->getPeriodicFlagX1();
	Ka=k1;Kb=k1+1; 
	if (k1==K) // horizontal, on top 
		{
		Ka=k2;
		Kb=k2-1;
		eps = fields->get_epsi(j1,k1-1);
		}
	else  // horizontal, on bottom
		{
		Ka=k1;
		Kb=k1+1;
		eps = fields->get_epsi(j1,k1);
		}
	if(j1!=0) // collect charge on left
		{
		sumPhiP += dy*fields->get_grid()->dS1Prime(j1,Ka)*fields->getPhiP(j1-1,Ka)/dx;
		sumPhiL += dy*fields->get_grid()->dS1Prime(j1,Ka)*(1.0 - LaplaceSolution[j1-1][Ka])/dx;
		}
	if(j2!=J) //  collect charge on right
		{
		sumPhiP += dy*fields->get_grid()->dS1Prime(j2,Ka)*fields->getPhiP(j2+1,Ka)/dx;
		sumPhiL += dy*fields->get_grid()->dS1Prime(j2,Ka)*(1.0 - LaplaceSolution[j2+1][Ka])/dx;
	        }
	for(int j=j1; j<=j2; j++) 
		{
		areaV = fields->get_grid()->dS2Prime(j,Ka);
		if (j==j1) // left side of current source
			{
			if (j!=0) areaV *=0.5; // too much area
			if ((j==0) && (x1min!=0)) areaV*=0.5;
			}
		if(j==j2 && (j==J && flag)) { break; }
		if (j==j2 && j!=J) areaV *= 0.5; // O.K. at right
		sumrho += fields->getrho(j, Ka)*fields->get_grid()->halfcellVolume(j,Kb);
		sumPhiP += fields->getPhiP(j,Kb)*areaV;
		sumPhiL += areaV*(1.0 - LaplaceSolution[j][Kb]);
		}
	}
	
V = Qtotal + sumrho;
//V *= Scalar(normal);
V *= (dx/eps);
V += sumPhiP;
V /= sumPhiL;
fflush(stdout);
int j,k;
Scalar **Phi=fields->getphi();
//  This applies the laplace solution at time t, scaled by the current voltage
for(j=0;j<=J;j++) 
	{
	for(k=0;k<=K;k++)
		{
		Phi[j][k]+=LaplaceSolution[j][k]*V;
		}
	}
}

// collect presently does not consider secondaries, but should
void CurrentSource::collect(Particle &p, Vector3& dxMKS) 
{
Qtotal += p.get_q(); // add particle charge into Qtotal 
Conductor::collect(p, dxMKS);			// pass it up chain, will handle secondaries
}

int CurrentSource::Dump(FILE *DMPFile)
{
Scalar xw[4];  // in this format:  xs, ys, xf, yf
Vector2 **X=fields->get_grid()->getX();
xw[0] = X[j1][k1].e1();
xw[1] = X[j1][k1].e2();
xw[2] = X[j2][k2].e1();
xw[3] = X[j2][k2].e2();
XGWrite(xw,ScalarInt,4,DMPFile,ScalarChar);

// write the BoundaryType
XGWrite(&BoundaryType, 4, 1, DMPFile, "int");

// write nQuad data (placeholder in this case)
int nQuadData = 1;
XGWrite(&nQuadData, 4, 1, DMPFile, "int");

// write the total charge
XGWrite(&Qtotal, ScalarInt, 1, DMPFile, ScalarChar);

printf("writing Qtotal = %g\n", Qtotal);

return(TRUE);
}

int CurrentSource::Restore(FILE *DMPFile, int _BoundaryType, Scalar _A1,
                           Scalar _A2, Scalar _B1, Scalar _B2,int nQuads)
{

// Two checks to see if this data is loadable by this boundary.
if(BoundaryType != _BoundaryType) return FALSE;
if(!onMe(_A1,_A2,_B1,_B2)) return FALSE;
	
// OK, this is the correct boundary
	
// next two don't matter
// XGRead(&dummy,4,1,DMPFile,"int");
// XGRead(&dummy,4,1,DMPFile,"int");

// read the total charge on the boundary:
XGRead(&Qtotal, ScalarInt, 1, DMPFile, ScalarChar);

printf("read Qtotal = %g\n", Qtotal);
restore = TRUE;
return(TRUE);
}

