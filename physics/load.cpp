/*
====================================================================

load.cpp

2.01 (Bruhwiler 10-28-99) Modified for new derived class VarWeightLoad
2.02 (JohnV 20-Jun-2002) Fixed bug which caused Load to fail with analyticF

====================================================================
 */

#include "sptlrgn.h"
#include "ptclgrp.h"
#include "maxwelln.h"
#include "load.h"
#include "oopiclist.h"
#include "misc.h"
#include "ostring.h"

#include <cstdio>

using namespace std;

Scalar Load::F(Vector2& x) 
{
	if(analyticF==(ostring)"0.0\n") 
		return(1);
	else if (analyticF==(ostring)"fajans\n")
		return(fajans_load(x));

	else if (analyticF==(ostring)"file\n")
		return(gridded_load(x));

	else
		return (gridded_load(x)); //evaluator load

}

Load::Load(SpatialRegion* _SR,Scalar _density, Maxwellian* max, Scalar _Min1MKS, 
		Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS, int _LoadMethodFlag,
		Scalar _np2c, const ostring &_analyticF, const ostring &_DensityFile,const ostring &_name)
/* throw(Oops) */{

	int j,k;
	/// \dad{begin}
	rmax_fn_z = 0;
	/// \dad{end}
#ifndef UNIX
	analyticF=_analyticF;
	ostring endLine = "\n";
	analyticF=_analyticF + endLine;
#else
	analyticF=_analyticF + '\n';
#endif

	region = _SR;
	DensityFile = _DensityFile;
	name = _name;
	grid = region->get_grid();
	fields = region->get_fields();
	maxwellian = max;
	LoadMethodFlag = _LoadMethodFlag;
	species = maxwellian->get_speciesPtr();
	Min1MKS = MIN(_Min1MKS,_Max1MKS);
	Max1MKS = MAX(_Max1MKS,_Min1MKS);
	Min2MKS = MIN(_Min2MKS,_Max2MKS);
	Max2MKS = MAX(_Max2MKS,_Min2MKS);

	// determine the maxima for this grid.  Clip the load if it's too large.
	Scalar x1min = grid->getX()[0][0].e1();
	Scalar x2min = grid->getX()[0][0].e2();
	Scalar x1max = grid->getX()[grid->getJ()][0].e1();
	Scalar x2max = grid->getX()[0][grid->getK()].e2();

	Min1MKS = MAX(x1min,Min1MKS);
	Max1MKS = MIN(x1max,Max1MKS);
	Min2MKS = MAX(x2min,Min2MKS);
	Max2MKS = MIN(x2max,Max2MKS);


	q = species->get_q();
	density = _density;
	np2c = _np2c;

	// This is a small change so that particles are not right on the boundary
	Scalar deltaR = (x2max-x2min)*1e-6;
	Scalar deltaZ = (x1max-x1min)*1e-6;
	Min1MKS+=deltaZ;
	Max1MKS-=deltaZ;
	Min2MKS+=deltaR;
	Max2MKS-=deltaR;

	p1 = Vector2(Min1MKS,Min2MKS);
	p2 = Vector2(Max1MKS,Max2MKS);
	deltaP = p2-p1;
	if (grid->query_geometry() == ZXGEOM) rz = FALSE;
	else if (grid->query_geometry() == ZRGEOM) rz = TRUE;
	else
	{
		stringstream ss (stringstream::in | stringstream::out);
		ss << "Load::Load: Error: \n"<<
				"load doesn't recognize the geometery flag" << endl;

		std::string msg;
		ss >> msg;
		Oops oops(msg);
		throw oops;    // exit()LoadParams::CreateCounterPart

	}

	if (np2c==0)
		fields->setBGRhoFlag(TRUE);

	fields->setShowInitialDensityFlag(TRUE);

	J = grid->getJ();
	K = grid->getK();
	gridded_density = new Scalar *[J+1];
	for (j=0; j<=J; j++){
		gridded_density[j] = new Scalar[K + 1];
		for(k=0;k<=K;k++){ 
			gridded_density[j][k]=0;
		}
	}

	numberParticles =0;  //  Assume no particles are to be loaded until shown otherwise.

	set_coefficients();
	if(analyticF==(ostring)"0.0\n") 
		init_default();
	if (analyticF==(ostring)"file\n")
	{
		try{
			init_file_load();
		}
		catch(Oops& oops){
			oops.prepend("Load::Load: Error: \n"); //OK
			throw oops;
		}
	}


	else if (analyticF==(ostring)"fajans\n")
	{
		try{
			init_fajans_load();
		}
		catch(Oops& oops){
			oops.prepend("Load::Load: Error: \n"); //OK
			throw oops;
		}
	}
	else // general analyticF case
	{
		init_evaluator();
	}
	IntegrateGriddedLoad();
}

Load::~Load()
{
	for (int j=0; j<=J; j++)
		delete[] gridded_density[j];
	delete[] gridded_density;
	delete maxwellian; /*dad: maxwellian was assigned a value from the input parameters 
			     but whatever it points to is deleted here */

	if (rmax_fn_z) {
		delete [] rmax_fn_z;
		rmax_fn_z = 0;
	}
}

int Load::load_it()
/* throw(Oops) */{
	Vector3 u;
	Vector2 x;
	Boundary*** CellMask = grid->GetCellMask();

	int seed = 0;

	if(numberParticles > 0) {
		Vector2 xc;
		Scalar Fxc;

		int count = 0;
#ifdef FAJANS
		while (seed < numberParticles)
		{
			seed++;
			xc=Vector2(Min1MKS+dz*revers(seed,7),
					sqrt(drSqr*revers(seed,3)+rMinSqr));
			Fxc = F(xc);
			if ((Fxc!=0) && (Fxc>=(Scalar)frand()))
			{
				count++;
				x = grid->getGridCoords(xc);
				/*						v=maxwellian->get_newV();
						Vector3	beta = iSPEED_OF_LIGHT*v;
						Scalar gamma0 = 1/sqrt(1 - beta*beta);
						v *= gamma0; */
				u=maxwellian->get_U();
				Particle*       particle = new Particle(x, u, species, np2c);
				particleList.push(particle);
			}
			if (seed)
				if (seed%50==0)
					region->addParticleList(particleList);
		}
#else
		while (count < numberParticles)
		{
			switch (LoadMethodFlag){
			case RANDOM:
				xc.set_e1(p1.e1()+deltaP.e1()*frand());
				if(rz)
					xc.set_e2(sqrt(drSqr*frand()+rMinSqr));
				else
					xc.set_e2(p1.e2()+deltaP.e2()*frand());
				break;
			case QUIET_START:
				xc.set_e1(p1.e1()+deltaP.e1()*revers2(seed));
				if(rz)
					xc.set_e2(sqrt(drSqr*revers(seed,3)+rMinSqr));
				else
					xc.set_e2(p1.e2()+deltaP.e2()*revers(seed,3));
				break;
			default:
				stringstream ss (stringstream::in | stringstream::out);
				ss<<"Load::load_it: Error: \n"<<
						"Bad value for loader flag"<<endl;

				std::string msg;
				ss >> msg;
				Oops oops(msg);
				throw oops;    // exit()   SpatialRegion::shiftRegion   LoadParams::CreateCounterPart

				break;
			}
			Fxc = F(xc);
			seed++;
			if ((Fxc!=0) && (Fxc>=(Scalar)frand())){
				x = grid->getGridCoords(xc);
				if (CellMask[(int)x.e1()][(int)x.e2()]==0){  //FREESPACE
					count++;
					/*						v=maxwellian->get_V();
						Vector3	beta = iSPEED_OF_LIGHT*v;
						Scalar gamma0 = 1/sqrt(1 - beta*beta);
						v *= gamma0;
					 */
					u = maxwellian->get_U();
					Particle*       particle = new Particle(x, u, species, np2c);
					particleList.push(particle);
				}
			}
			if (count)
				if (count%50==0)
					region->addParticleList(particleList);
		}
#endif
		region->addParticleList(particleList);
	}

	return(1);
}


// This is a special loader for Prof. Fajans and his group.  
// It returns a 1 if a particle should be laid down and a zero
// if not.

void Load::init_fajans_load()
/* throw(Oops) */{
#define MAX_ITER  50
	int j, k, i;
	/* new code for smart loading - djc */

	rmax_fn_z = new Scalar[J+1];
	Scalar interpolation;
	Scalar constant;
	Scalar dr, f, df;
	int j1 =0;
	/* I assume J=jmax and j= 0 (cells) are boundaries where the plasma density
		 may be assumed nil, and the potential is given very nearly by
		 the vaccuum solution - djc */
	/* I assume getphi()[z][r] */
	constant = density*(q)*iEPS0/4.0;  /* positron charge assumed */

	// run a zero souce though dadi to get the vaccuum solution
	region->ComputePhi();

	Scalar equi_pot = fields->getphi()[j1][0];

	/*iterate and generate rmax_fn_z here using small r approx. for first guess */
	/* numerical recipes Newton-Raphson method*/

	rmax_fn_z[j1] = 0.0;
	for (k= (j1 + 1); k<J; k++) {
		if ( k == (j1 + 1)) {
			rmax_fn_z[k] = sqrt((equi_pot - fields->getphi()[k][0])/2.0/constant);
		}
		else {
			rmax_fn_z[k] = rmax_fn_z[k-1];
		}
		for(i=1; i<= MAX_ITER; i++) 
		{
			f = constant*rmax_fn_z[k]*rmax_fn_z[k]*(1 + 2.0*log(Max2MKS/rmax_fn_z[k])) - (equi_pot - fields->getphi()[k][0]);
			df = 4.0*constant*rmax_fn_z[k]*log(Max2MKS/rmax_fn_z[k]);
			dr = f/df;
			rmax_fn_z[k] -= dr;

			if ((Max2MKS - rmax_fn_z[k])*(rmax_fn_z[k]) < 0.0) {
				stringstream ss (stringstream::in | stringstream::out);
				ss << "Load::init_fajans_load: Error: \n"<<
						"jumped out of bounds in load " << endl;

				std::string msg;
				ss >> msg;
				Oops oops(msg);
				throw oops;    // exit() //Load::Load

			}
			if (fabs(dr/rmax_fn_z[k]) <= 0.01) break;        /* 1 percent tollerance imposed */
		}
		if (fabs(dr/rmax_fn_z[k]) > 0.01) 
		{
			cout << "max number of iterations reached in load " << endl;
		}
	}
	rmax_fn_z[J] = 0.0;
	Scalar temp_density = 0;
	for (j=0; j<=J; j++)
	{
		for (k=0; k<=K; k++)
		{
			/* interpolation */
			interpolation = rmax_fn_z[j];
			if ((grid->getMKS(j,k).e2()) > interpolation)
			{
				temp_density=0;
			}
			else
			{
				temp_density = 1;
			}
			gridded_density[j][k] = temp_density;
			fields->setloaddensity(species->getID(),j,k,temp_density);
		}
	}
}

int Load::fajans_load(Vector2& x)
{

	Scalar interpolation;
	Scalar r;
	int k;


	/* interpolation */
	r = grid->getGridCoords(x).e2();
	k = (int)r;
	interpolation = rmax_fn_z[k] + (rmax_fn_z[k+1] - rmax_fn_z[k])*(r-k);
	if (x.e2() > interpolation) return(0);
	return(1);

}

void Load::init_file_load()
/* throw(Oops) */{
	FILE *openfile;
	int status;
	int j,k;
	char* den_file;
	Scalar total_density=0;
	Scalar max_density=0;
	Vector2 p1Grid = grid->getGridCoords(p1);
	Vector2 p2Grid = grid->getGridCoords(p2)+Vector2(1,1);


	den_file = DensityFile.c_str();

	if ((openfile = fopen (den_file, "r"))  != 0)
	{
		// read in
		Scalar dum1, dum2; // dummies, remove later
		cout << "  " << endl;
		cout << "started reading density file " << den_file << endl;
		for (j=0; j<=J; j++)
			for(k=0; k<=K; k++){
#ifdef SCALAR_DOUBLE
				status = fscanf(openfile, "%lg %lg %lg", &dum1, &dum2, &gridded_density[j][k]);
#else
				status = fscanf(openfile, "%g %g %g", &dum1, &dum2, &gridded_density[j][k]);
#endif
				if (!((j>p1Grid.e1())&&(j<p2Grid.e1())&&(k>p1Grid.e2())&&(k<p2Grid.e2())))
					gridded_density[j][k] =0;
			}
		cout << "finished reading density file " << den_file << endl;
		fclose (openfile);

	}
	else
	{
		stringstream ss (stringstream::in | stringstream::out);
		ss << "Load::init_file_load: Error: \n"<<
				"Density file not valid." << endl;
		std::string msg;
		ss >> msg;
		Oops oops(msg);
		throw oops;    // exit()  Load::Load

	}
}

void Load::IntegrateGriddedLoad(void)  //and set back ground rho and show initial load
{

	int j,k;
	Scalar total_density=0;
	Scalar max_density=0;
	Scalar ** int_density;
	int_density = new Scalar *[J+1];
	for (j=0; j<=J; j++){
		int_density[j] = new Scalar[K + 1];
		for(k=0;k<=K;k++){ 
			int_density[j][k]=0;
		}
	}

	Vector2 p1Grid = grid->getGridCoords(p1);
	Vector2 p2Grid = grid->getGridCoords(p2)+Vector2(1,1);

	/***doing a second order integration of the gridded_density***/

	Boundary*** CellMask = grid->GetCellMask();
	Scalar dV=0;
	Scalar fx, fy;
	Scalar fx13,fx31,fx12;
	Scalar fy13,fy31,fy12;

	for (j=(int)p1Grid.e1(); j<MIN((int)p2Grid.e1(),J); j++)
		for (k=(int)p1Grid.e2(); k<MIN((int)p2Grid.e2(),K); k++){ 	
			if (CellMask[j][k]==NULL){ //FREESPACE
				// first if is getting all the half filled cells
				if (((j==(int)p1Grid.e1())&&(p1Grid.e1()!=0))||
						((j==((int)p2Grid.e1()-1))&&((p1Grid.e1()-1)!=J))||
						((k==((int)p1Grid.e2())&&(p1Grid.e2()!=0)))||
						((k==((int)p2Grid.e2()-1))&&((p2Grid.e2()-1)!=K))){  //this is over checking, trying to increase speed
					if ((j==(int)p1Grid.e1())&&(k==(int)p1Grid.e2())){  //lower left corner
						dV = grid->cellVolume(j,k);
						fx = p1Grid.e1()-(int)p1Grid.e1();
						fy = p1Grid.e2()-(int)p1Grid.e2();
						fx13 = cube(1-fx);
						fx31 = 1-cube(fx);
						fx12 = (1+2*fx)*sqr(1-fx);

						fy13 = cube(1-fy);
						fy31 = 1-cube(fy);
						fy12 = (1+2*fy)*sqr(1-fy);

						int_density[j][k] += (4*fx13*fy13*gridded_density[j][k]+
								2*fx12*fy13*gridded_density[j+1][k]+
								2*fx13*fy12*gridded_density[j][k+1]+
								fx12*fy12*gridded_density[j+1][k+1])/36*dV;
						int_density[j+1][k] += (4*fx31*fy13*gridded_density[j+1][k]+
								2*fx12*fy13*gridded_density[j][k]+
								2*fx31*fy12*gridded_density[j+1][k+1]+
								fx12*fy12*gridded_density[j][k+1])/36*dV;
						int_density[j+1][k+1] += (4*fx31*fy31*gridded_density[j+1][k+1]+
								2*fx31*fy12*gridded_density[j+1][k]+
								2*fx12*fy31*gridded_density[j][k+1]+
								fx12*fy12*gridded_density[j][k])/36*dV;
						int_density[j][k+1] += (4*fx13*fy31*gridded_density[j][k+1]+
								2*fx12*fy31*gridded_density[j+1][k+1]+
								2*fx13*fy12*gridded_density[j][k]+
								fx12*fy12*gridded_density[j+1][k])/36*dV;

					}
					else if (j==((int)p1Grid.e1())&&(k==((int)p2Grid.e2()-1))){  //upper left corner	
						dV = grid->cellVolume(j,k);
						fx = p1Grid.e1()-(int)p1Grid.e1();
						fy = (int)p2Grid.e2()-p2Grid.e2()+1;
						fx13 = cube(1-fx);
						fx31 = 1-cube(fx);
						fx12 = (1+2*fx)*sqr(1-fx);

						fy13 = cube(1-fy);
						fy31 = 1-cube(fy);
						fy12 = (1+2*fy)*sqr(1-fy);

						int_density[j][k] += (4*fx13*fy31*gridded_density[j][k]+
								2*fx12*fy31*gridded_density[j+1][k]+
								2*fx13*fy12*gridded_density[j][k+1]+
								fx12*fy12*gridded_density[j+1][k+1])/36*dV;
						int_density[j+1][k] += (4*fx31*fy31*gridded_density[j+1][k]+
								2*fx12*fy31*gridded_density[j][k]+
								2*fx31*fy12*gridded_density[j+1][k+1]+
								fx12*fy12*gridded_density[j][k+1])/36*dV;
						int_density[j+1][k+1] += (4*fx31*fy13*gridded_density[j+1][k+1]+
								2*fx31*fy12*gridded_density[j+1][k]+
								2*fx12*fy13*gridded_density[j][k+1]+
								fx12*fy12*gridded_density[j][k])/36*dV;
						int_density[j][k+1] += (4*fx13*fy13*gridded_density[j][k+1]+
								2*fx12*fy13*gridded_density[j+1][k+1]+
								2*fx13*fy12*gridded_density[j][k]+
								fx12*fy12*gridded_density[j+1][k])/36*dV;


					}
					else if ((j==((int)p2Grid.e1()-1))&&(k==((int)p1Grid.e2()))){  //lower right corner
						dV = grid->cellVolume(j,k);
						fx = (int)p2Grid.e1()-p2Grid.e1()+1;
						fy = p1Grid.e2()-(int)p1Grid.e2();

						fx13 = cube(1-fx);
						fx31 = 1-cube(fx);
						fx12 = (1+2*fx)*sqr(1-fx);

						fy13 = cube(1-fy);
						fy31 = 1-cube(fy);
						fy12 = (1+2*fy)*sqr(1-fy);

						int_density[j][k] += (4*fx31*fy13*gridded_density[j][k]+
								2*fx12*fy13*gridded_density[j+1][k]+
								2*fx31*fy12*gridded_density[j][k+1]+
								fx12*fy12*gridded_density[j+1][k+1])/36*dV;
						int_density[j+1][k] += (4*fx13*fy13*gridded_density[j+1][k]+
								2*fx12*fy13*gridded_density[j][k]+
								2*fx13*fy12*gridded_density[j+1][k+1]+
								fx12*fy12*gridded_density[j][k+1])/36*dV;
						int_density[j+1][k+1] += (4*fx13*fy31*gridded_density[j+1][k+1]+
								2*fx13*fy12*gridded_density[j+1][k]+
								2*fx12*fy31*gridded_density[j][k+1]+
								fx12*fy12*gridded_density[j][k])/36*dV;
						int_density[j][k+1] += (4*fx31*fy31*gridded_density[j][k+1]+
								2*fx12*fy31*gridded_density[j+1][k+1]+
								2*fx31*fy12*gridded_density[j][k]+
								fx12*fy12*gridded_density[j+1][k])/36*dV;

					}
					else if ((j==((int)p2Grid.e1()-1))&&(k==((int)p2Grid.e2()-1))){  //upper right corner
						dV = grid->cellVolume(j,k);
						fx = (int)p2Grid.e1()-p2Grid.e1()+1;
						fy = (int)p2Grid.e2()-p2Grid.e2()+1;

						fx13 = cube(1-fx);
						fx31 = 1-cube(fx);
						fx12 = (1+2*fx)*sqr(1-fx);

						fy13 = cube(1-fy);
						fy31 = 1-cube(fy);
						fy12 = (1+2*fy)*sqr(1-fy);

						int_density[j][k] += (4*fx31*fy31*gridded_density[j][k]+
								2*fx12*fy31*gridded_density[j+1][k]+
								2*fx31*fy12*gridded_density[j][k+1]+
								fx12*fy12*gridded_density[j+1][k+1])/36*dV;
						int_density[j+1][k] += (4*fx13*fy31*gridded_density[j+1][k]+
								2*fx12*fy31*gridded_density[j][k]+
								2*fx13*fy12*gridded_density[j+1][k+1]+
								fx12*fy12*gridded_density[j][k+1])/36*dV;
						int_density[j+1][k+1] += (4*fx13*fy13*gridded_density[j+1][k+1]+
								2*fx13*fy12*gridded_density[j+1][k]+
								2*fx12*fy13*gridded_density[j][k+1]+
								fx12*fy12*gridded_density[j][k])/36*dV;
						int_density[j][k+1] += (4*fx31*fy13*gridded_density[j][k+1]+
								2*fx12*fy13*gridded_density[j+1][k+1]+
								2*fx31*fy12*gridded_density[j][k]+
								fx12*fy12*gridded_density[j+1][k])/36*dV;

					}
					else if (k==(int)p1Grid.e2()){ //lower edge, no corners
						dV = grid->cellVolume(j,k);
						fy = p1Grid.e2()-(int)p1Grid.e2();

						fy13 = cube(1-fy);
						fy31 = 1-cube(fy);
						fy12 = (1+2*fy)*sqr(1-fy);

						int_density[j][k] += (4*fy13*gridded_density[j][k]+
								2*fy13*gridded_density[j+1][k]+
								2*fy12*gridded_density[j][k+1]+
								fy12*gridded_density[j+1][k+1])/36*dV;
						int_density[j+1][k] += (4*fy13*gridded_density[j+1][k]+
								2*fy13*gridded_density[j][k]+
								2*fy12*gridded_density[j+1][k+1]+
								fy12*gridded_density[j][k+1])/36*dV;
						int_density[j+1][k+1] += (4*fy31*gridded_density[j+1][k+1]+
								2*fy12*gridded_density[j+1][k]+
								2*fy31*gridded_density[j][k+1]+
								fy12*gridded_density[j][k])/36*dV;
						int_density[j][k+1] += (4*fy31*gridded_density[j][k+1]+
								2*fy31*gridded_density[j+1][k+1]+
								2*fy12*gridded_density[j][k]+
								fy12*gridded_density[j+1][k])/36*dV;

					}
					else if (j==(int)p1Grid.e1()){  //left edge, no corner
						dV = grid->cellVolume(j,k);
						fx = p1Grid.e1()-(int)p1Grid.e1();

						fx13 = cube(1-fx);
						fx31 = 1-cube(fx);
						fx12 = (1+2*fx)*sqr(1-fx);

						int_density[j][k] += (4*fx13*gridded_density[j][k]+
								2*fx12*gridded_density[j+1][k]+
								2*fx13*gridded_density[j][k+1]+
								fx12*gridded_density[j+1][k+1])/36*dV;
						int_density[j+1][k] += (4*fx31*gridded_density[j+1][k]+
								2*fx12*gridded_density[j][k]+
								2*fx31*gridded_density[j+1][k+1]+
								fx12*gridded_density[j][k+1])/36*dV;
						int_density[j+1][k+1] += (4*fx31*gridded_density[j+1][k+1]+
								2*fx31*gridded_density[j+1][k]+
								2*fx12*gridded_density[j][k+1]+
								fx12*gridded_density[j][k])/36*dV;
						int_density[j][k+1] += (4*fx13*gridded_density[j][k+1]+
								2*fx12*gridded_density[j+1][k+1]+
								2*fx13*gridded_density[j][k]+
								fx12*gridded_density[j+1][k])/36*dV;

					}
					else if (k==((int)p2Grid.e2()-1)){  //upper edge, no corner
						dV = grid->cellVolume(j,k);
						fy = (int)p2Grid.e2()-p2Grid.e2()+1;

						fy13 = cube(1-fy);
						fy31 = 1-cube(fy);
						fy12 = (1+2*fy)*sqr(1-fy);

						int_density[j][k] += (4*fy31*gridded_density[j][k]+
								2*fy31*gridded_density[j+1][k]+
								2*fy12*gridded_density[j][k+1]+
								fy12*gridded_density[j+1][k+1])/36*dV;
						int_density[j+1][k] += (4*fy31*gridded_density[j+1][k]+
								2*fy31*gridded_density[j][k]+
								2*fy12*gridded_density[j+1][k+1]+
								fy12*gridded_density[j][k+1])/36*dV;
						int_density[j+1][k+1] += (4*fy13*gridded_density[j+1][k+1]+
								2*fy12*gridded_density[j+1][k]+
								2*fy13*gridded_density[j][k+1]+
								fy12*gridded_density[j][k])/36*dV;
						int_density[j][k+1] += (4*fy13*gridded_density[j][k+1]+
								2*fy13*gridded_density[j+1][k+1]+
								2*fy12*gridded_density[j][k]+
								fy12*gridded_density[j+1][k])/36*dV;

					}
					else if (j==((int)p2Grid.e1()-1)){  //right edge, no corner
						dV = grid->cellVolume(j,k);
						fx = (int)p2Grid.e1()-p2Grid.e1()+1;

						fx13 = cube(1-fx);
						fx31 = 1-cube(fx);
						fx12 = (1+2*fx)*sqr(1-fx);

						int_density[j][k] += (4*fx31*gridded_density[j][k]+
								2*fx12*gridded_density[j+1][k]+
								2*fx31*gridded_density[j][k+1]+
								fx12*gridded_density[j+1][k+1])/36*dV;
						int_density[j+1][k] += (4*fx13*gridded_density[j+1][k]+
								2*fx12*gridded_density[j][k]+
								2*fx13*gridded_density[j+1][k+1]+
								fx12*gridded_density[j][k+1])/36*dV;
						int_density[j+1][k+1] += (4*fx13*gridded_density[j+1][k+1]+
								2*fx13*gridded_density[j+1][k]+
								2*fx12*gridded_density[j][k+1]+
								fx12*gridded_density[j][k])/36*dV;
						int_density[j][k+1] += (4*fx31*gridded_density[j][k+1]+
								2*fx12*gridded_density[j+1][k+1]+
								2*fx31*gridded_density[j][k]+
								fx12*gridded_density[j+1][k])/36*dV;
					}
				}
				else{
					dV = grid->cellVolume(j,k);
					int_density[j][k] += (4*gridded_density[j][k]+2*gridded_density[j+1][k]+
							2*gridded_density[j][k+1]+gridded_density[j+1][k+1])/36*dV;
					int_density[j+1][k] += (4*gridded_density[j+1][k]+2*gridded_density[j][k]+
							2*gridded_density[j+1][k+1]+gridded_density[j][k+1])/36*dV;
					int_density[j+1][k+1] += (4*gridded_density[j+1][k+1]+2*gridded_density[j+1][k]+
							2*gridded_density[j][k+1]+gridded_density[j][k])/36*dV;
					int_density[j][k+1] += (4*gridded_density[j][k+1]+2*gridded_density[j+1][k+1]+
							2*gridded_density[j][k]+gridded_density[j+1][k])/36*dV;
				}										
			}
		}

	for (j=(int)p1Grid.e1(); j<=MIN((int)p2Grid.e1(),J); j++)
		for (k=(int)p1Grid.e2(); k<=MIN((int)p2Grid.e2(),K); k++){
			total_density += int_density[j][k]; // this is really total charge
			int_density[j][k] /= grid->get_halfCellVolumes()[j][k];
			if (grid->axis()) {
				if (k == 0) int_density[j][k] *= 0.5/grid->get_radial0();
				else if (k == K) int_density[j][k] *= grid->get_radialK();
			}
			fields->setloaddensity(species->getID(),j,k,int_density[j][k]);
			if (np2c==0)
				fields->setbgrho(j,k,q*int_density[j][k]);
			if (gridded_density[j][k]>max_density) 
			{
				max_density=gridded_density[j][k];
			}
		}

	if (np2c&&(numberParticles ==0))
		numberParticles = (int)(total_density/np2c+0.5);
	if (numberParticles){
		if(max_density < 1E-16) { max_density = 1; }
		for (j=(int)p1Grid.e1(); j<=MIN((int)p2Grid.e1(),J); j++)
		{
			for (k=(int)p1Grid.e2(); k<=MIN((int)p2Grid.e2(),K); k++)
			{
				gridded_density[j][k] /= max_density;
			}
		}
	}

	for (j=0; j<=J; j++)
		delete [] int_density[j];
	delete [] int_density;
	int_density = 0;

}


Scalar Load::gridded_load(Vector2& x)
{
	Vector2 gridx = grid->getGridCoords(x);
	return grid->interpolateBilinear(gridded_density,gridx);
}

void Load::init_evaluator()
{
	int j,k;
	Vector2 x;
	Vector2 p1Grid = grid->getGridCoords(p1);
	Vector2 p2Grid = grid->getGridCoords(p2)+Vector2(1,1);
	for (j=(int)p1Grid.e1(); j<=MIN((int)p2Grid.e1(),J); j++)
	{
		for (k=(int)p1Grid.e2(); k<=MIN((int)p2Grid.e2(),K); k++)
		{
			x = grid->getMKS(j,k);
			adv_eval->add_variable("x1",x.e1());
			adv_eval->add_variable("x2",x.e2());
			gridded_density[j][k] = adv_eval->Evaluate(analyticF.c_str());
		}
	}
}

// Compute the coefficients used later in r-z geometry as needed

void Load::set_coefficients()
{
	if (rz) {
		rMin = p1.e2();
		rMax = p2.e2();
		rMinSqr = rMin * rMin;
		drSqr   = rMax * rMax - rMinSqr;
	}
}

void Load::init_default()
{
	volume = deltaP.e1();
	if(rz) {
		volume *= M_PI*drSqr;
	}
	else
		volume *=deltaP.e2();

	if (np2c)
		numberParticles = (int)(volume*density/np2c+0.5);

	int j,k;
	Vector2 gridp1, gridp2;
	gridp1 = grid->getGridCoords(p1);
	gridp2 = grid->getGridCoords(p2);
	Vector2 upper,lower;
	for (j=((int)(gridp1.e1()+.5)); j<=((int)(gridp2.e1()+.5)); j++)
		for (k=((int)(gridp1.e2()+.5)); k<=((int)(gridp2.e2()+.5)); k++)
			gridded_density[j][k] = density;

}





