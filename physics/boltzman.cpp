#include "boltzman.h"
#include "psolve.h"
#include "ovector.h"
#include "misc.h"
#include "ptclgrp.h"
#include "particle.h"
#include "fields.h"
#include <math.h>

#ifdef _MSC_VER
using std::cout;
using std::cerr;
using std::endl;
#endif

/* 
   Will only work with Multigrid and maybe with DADI - 
   Keith will be happy to fix if you have a problem -
   send complaints to kc@langmuir.eecs.berkeley.edu
 */

Boltzmann::Boltzmann(Fields *F, Scalar _temperature, Species* _BoltzSpecies, bool _EB_flag)
{
	dump = false;
	fields = F;
	BoltzSpecies = _BoltzSpecies;
	grid = fields->get_grid();
	psolve = fields->getPoissonSolve();
	ElectrostaticFlag = fields->getElectrostatic();
	bcNode = grid->GetNodeBoundary();

	itermax=100;  //maximum # of iterations for psolve

	//tolerance for the boltzmann solve is set from input file
	tolerance = fields->getpresidue();

	qMKS = BoltzSpecies->get_q();
	q = qMKS/fabs(ELECTRON_CHARGE);
	J = fields->getJ();
	K = fields->getK();

	temperature = _temperature;
	qbytemp = q/temperature;

	EB_flag = _EB_flag;  // if EB_flag the energy balance is used
	// otherwise the temperature is constant

	// allocate field arrays
	foo = new Scalar *[J+1];
	phi = new Scalar *[J+1];
	rho = new Scalar *[J+1];
	BoltzmannRho = new Scalar *[J+1];
	totalRho = new Scalar *[J+1];
	int j, k;
	for(j=0;j<=J;j++)
	{
		foo[j] = new Scalar[K+1];
		phi[j] = new Scalar[K+1];
		rho[j] = new Scalar[K+1];
		BoltzmannRho[j] = new Scalar[K+1];
		totalRho[j] = new Scalar[K+1];
	}

	// zero field arrays
	for(j=0;j<=J;j++) for(k=0;k<=K;k++)
	{
		phi[j][k] = 0.0;
		foo[j][k] = 0.0;
		BoltzmannRho[j][k] = 0.0;
		totalRho[j][k] = 0.0;
	}

	switch(ElectrostaticFlag)
	{
	default:
		fprintf(stderr,
				"WARNING, poisson solve defaulting in Boltzmann Solve\n");

	case ELECTROMAGNETIC_SOLVE:
	case DADI_SOLVE:
	case PERIODIC_X1_DADI:
		b1_orig = new Scalar *[J+1];
		b2_orig = new Scalar *[J+1];

		for(j=0;j<=J;j++)
		{
			b1_orig[j] = new Scalar[K+1];
			b2_orig[j] = new Scalar[K+1];
		}

		// *1_local, *2_local is shared by psolve and boltzmann
		a1_local = psolve->get_a1_coefficients();
		a2_local = psolve->get_a2_coefficients();
		b1_local = psolve->get_b1_coefficients();
		b2_local = psolve->get_b2_coefficients();
		c1_local = psolve->get_c1_coefficients();
		c2_local = psolve->get_c2_coefficients();

		for(j=0;j<=J;j++) for(k=0;k<=K;k++)
		{
			b1_orig[j][k] = b1_local[j][k];
			b2_orig[j][k] = b2_local[j][k];
		}
		break;

	case CG_SOLVE:
		fprintf(stderr,
				"WARNING, CG solve has not been written for Boltzmann Solve\n");
		exit(1);
		break;

	case GMRES_SOLVE:
		fprintf(stderr,
				"WARNING, GMRES has not been written for Boltzmann Solve\n");
		exit(1);
		break;

	case MGRID_SOLVE:

		resCoeff = new Scalar**[J+1];
		for (j=0;j<=J;j++)
		{
			resCoeff[j] = new Scalar*[K+1];
			for (k=0;k<=K;k++)
				resCoeff[j][k] = new Scalar[5];
		}
		resCoeff_local = psolve->get_resCoeff();

		for(j=0;j<=J;j++) for(k=0;k<=K;k++)
			for (int s=0; s<5; s++)
				resCoeff[j][k][s] = resCoeff_local[j][k][s];
		break;
	}
}

Boltzmann::~Boltzmann()
{

	// deallocate grid arrays
	for(int j=0; j<=J; j++)
	{
		switch(ElectrostaticFlag)
		{
		default:
			fprintf(stderr,
					"WARNING, poisson solve defaulting in Boltzmann Solve\n");

		case ELECTROMAGNETIC_SOLVE:
		case DADI_SOLVE:
		case PERIODIC_X1_DADI:

			delete[] b1_orig[j];
			delete[] b1_local[j];
			break;

		case MGRID_SOLVE:
			for(int k=0; k < K; k++)
			{
				delete[] resCoeff[j][k];
			}
			delete[] resCoeff[j];
			break;
		}
		delete[] phi[j];
		delete[] BoltzmannRho[j];
		delete[] rho[j];
		delete[] totalRho[j];
		delete[] foo[j];
	}

	switch(ElectrostaticFlag)
	{
	default:
		fprintf(stderr,
				"WARNING, poisson solve defaulting in Boltzmann Solve\n");

	case ELECTROMAGNETIC_SOLVE:  
	case DADI_SOLVE:
	case PERIODIC_X1_DADI:

		delete[] b1_orig;
		delete[] b1_local;
		break;

	case MGRID_SOLVE:
		delete[] resCoeff;
		break;
	}
	delete[] phi;
	delete[] BoltzmannRho;
	delete[] rho;
	delete[] totalRho;
	delete[] foo;
}

// called from Fields::ElectrostaticSolve()
void Boltzmann::updatePhi(Scalar **source, Scalar **dest, 
		const Scalar t, Scalar dt)
{
	int j,k;
	static int init_flag=1;

	if(EB_flag)
	{
		fprintf(stderr, "Energy Balance flag is on (buggy!)[press enter]\n");
		getchar();
	}

	if (init_flag)
	{
		init(source, dest);
		init_flag = 0;
		return;
	}

	// copy Fields phi (dest) to local phi
	for(j=0;j<=J;j++) for(k=0;k<=K;k++)
		phi[j][k] = dest[j][k];

	if(totalCharge)
	{
		if (totalCharge*BoltzSpecies->get_q()<0)
		{
			totalCharge = 0;
			energy = 0;
		}
	}

	if(totalCharge)
	{
		// why is this done?, JH, Aug. 2005
		if (temperature<.05)
		{
			totalCharge = 0.0;
			temperature = 6;
			energy = 0;
		}
	}

	if(totalCharge)
	{
		if(NonlinearSolve(source))
		{
			BoltzSpecies->zeroKineticEnergy();
			BoltzSpecies->addKineticEnergy(3.0/2.0*temperature*totalCharge/q);

			// deltaC: change in total charge (.e1())
			//  and energy (.e2())
			Vector2 deltaC = Vector2(0,0);

			// currently, charge and energy xfer due to collisions
			// not accounted for (but should be?), JH, Sept. 7, 2005
			//			deltaC = Collisions(dt);

			// particle and energy flux to wall
			deltaC += dt*ParticleFlux(dt);

			// update total charge and energy quantities
			totalCharge += deltaC.e1();
			energy += 5.0/2.0*temperature*deltaC.e1()/qMKS;
			energy += deltaC.e2();
		}
		else
		{
			LinearSolve(source);
			BoltzSpecies->zeroKineticEnergy();
			BoltzSpecies->addKineticEnergy((3.0/2.0*totalCharge*temperature)/q);
		}
	}
	else
	{
		LinearSolve(source);
		BoltzSpecies->zeroKineticEnergy();
	}

	// copy local phi back to Fields:: phi (dest)
	for(j=0;j<=J;j++) for(k=0;k<=K;k++)
		dest[j][k]=phi[j][k];
}

// initial Boltzmann solver first time updatePhi is called
void Boltzmann::init(Scalar **source, Scalar **dest)
{
	int j,k;
	Scalar PotEnergy = 0;
	Scalar totalVolume = 0;
	bool EBFlag_orig;

	// copy Fields:: phi (dest) to local phi
	for(j=0;j<=J;j++) for(k=0;k<=K;k++)
		if (!phi[j][k])
			phi[j][k] = dest[j][k];

	if (!dump)
	{
		// set the init totalCharge equal - particle density
		totalCharge=0.0;
		for(j=0;j<=J;j++)
			for(k=0;k<=K;k++)
				totalCharge+=source[j][k]*grid->get_halfCellVolumes()[j][k];
		totalCharge *= q;

		LinearSolve(source);

		// calculate initial energy
		energy = temperature*3.0/2.0*totalCharge;

		// swap energy balance flag and zero
		EBFlag_orig = EB_flag;
		EB_flag = false;

		// Start off with this temperture and find the PotEnergy
		bRho = fields->getRhoSpecies()[BoltzSpecies->getID()];

		if (totalCharge)
			NonlinearSolve(source);
		else
			LinearSolve(source);

		EB_flag = EBFlag_orig; // swap energy balance flag back
	}
	else // if dumped
	{
		bRho = fields->getRhoSpecies()[BoltzSpecies->getID()];

		if (totalCharge)
			NonlinearSolve(source);
		else
			LinearSolve(source);
	}

	// copy local phi back to Fields:: phi (dest)
	for(j=0;j<=J;j++) for(k=0;k<=K;k++)
		dest[j][k]=phi[j][k];
}

void Boltzmann::PSolveCoeff(Scalar **source)
{

	Scalar temp;
	int j,k;

	switch(ElectrostaticFlag)
	{
	default:
		fprintf(stderr,
				"WARNING, poisson solve defaulting in Boltzmann Solve\n");

	case ELECTROMAGNETIC_SOLVE:
	case DADI_SOLVE:
	case PERIODIC_X1_DADI:
		for(j=0;j<=J;j++) for(k=0;k<=K;k++)
		{
			temp = -BoltzmannRho[j][k];
			//		temp=-n0*exp(-phi[j][k]*qbytemp);
			rho[j][k] = -source[j][k]+(1+qbytemp*phi[j][k])*temp;
			if (b1_orig[j][k])
				b1_local[j][k] = b1_orig[j][k]+0.5*qbytemp*temp;
			if (b2_orig[j][k])
				b2_local[j][k] = b2_orig[j][k]+0.5*qbytemp*temp;
		}
		break;

	case CG_SOLVE:
	case GMRES_SOLVE:
		fprintf(stderr,
				"WARNING, CG and GMRES solves has not been written for Boltzmann Solve\n");
		break;

	case MGRID_SOLVE:
		psolve->PSolveBoltzCoeff(n0, qbytemp, BoltzmannRho, MinPot);
		for(j=0;j<=J;j++) for(k=0;k<=K;k++){
			temp = -BoltzmannRho[j][k];
			rho[j][k] = -source[j][k]+(1+qbytemp*phi[j][k])*temp;
		}
		break;
	}
}

Scalar Boltzmann::Residual()
{
	Scalar res = 0.0;
	Scalar temp = 0.0;

	switch(ElectrostaticFlag)
	{
	default:
		fprintf(stderr,
				"WARNING, poisson solve defaulting in Boltzmann Solve\n");

	case ELECTROMAGNETIC_SOLVE:
	case DADI_SOLVE:
		int j,k;

		j=0;k=0;
		if (b1_orig[j][k] !=0)
		{
			temp += totalRho[j][k];
			res += sqr(b1_orig[j][k]*phi[j][k]+c1_local[j][k]*phi[j+1][k]+
					b2_orig[j][k]*phi[j][k]+c2_local[j][k]*phi[j][k+1]+
					totalRho[j][k]);

			foo[j][k] = sqr(b1_orig[j][k]*phi[j][k]+c1_local[j][k]*phi[j+1][k]+
					b2_orig[j][k]*phi[j][k]+c2_local[j][k]*phi[j][k+1]+
					totalRho[j][k]);
		}

		k=K;
		if (b1_orig[j][k] !=0)
		{
			temp += totalRho[j][k];
			res += sqr(b1_orig[j][k]*phi[j][k]+c1_local[j][k]*phi[j+1][k]+
					a2_local[j][k]*phi[j][k-1]+b2_orig[j][k]*phi[j][k]+
					totalRho[j][k]);

			foo[j][k] = sqr(b1_orig[j][k]*phi[j][k]+c1_local[j][k]*phi[j+1][k]+
					a2_local[j][k]*phi[j][k-1]+b2_orig[j][k]*phi[j][k]+
					totalRho[j][k]);
		}

		j=J;
		if (b1_orig[j][k] !=0)
		{
			temp += totalRho[j][k];
			res += sqr(a1_local[j][k]*phi[j-1][k]+b1_orig[j][k]*phi[j][k]+
					a2_local[j][k]*phi[j][k-1]+b2_orig[j][k]*phi[j][k]+
					totalRho[j][k]);

			foo[j][k] = sqr(a1_local[j][k]*phi[j-1][k]+b1_orig[j][k]*phi[j][k]+
					a2_local[j][k]*phi[j][k-1]+b2_orig[j][k]*phi[j][k]+
					totalRho[j][k]);
		}

		k=0;

		if (b1_orig[j][k] !=0)
		{
			temp += totalRho[j][k];
			res += sqr(a1_local[j][k]*phi[j-1][k]+b1_orig[j][k]*phi[j][k]+
					b2_orig[j][k]*phi[j][k]+c2_local[j][k]*phi[j][k+1]+
					totalRho[j][k]);

			foo[j][k] = sqr(a1_local[j][k]*phi[j-1][k]+b1_orig[j][k]*phi[j][k]+
					b2_orig[j][k]*phi[j][k]+c2_local[j][k]*phi[j][k+1]+
					totalRho[j][k]);
		}

		for (j=1;j<J;j++)
		{
			k = 0;
			if (b1_orig[j][k] !=0){
				temp += totalRho[j][k];
				res += sqr(a1_local[j][k]*phi[j-1][k]+
						b1_orig[j][k]*phi[j][k]+c1_local[j][k]*phi[j+1][k]+
						b2_orig[j][k]*phi[j][k]+c2_local[j][k]*phi[j][k+1]+
						totalRho[j][k]);

				foo[j][k] = sqr(a1_local[j][k]*phi[j-1][k]
														+b1_orig[j][k]*phi[j][k]
																			  +c1_local[j][k]*phi[j+1][k]+
																			  b2_orig[j][k]*phi[j][k]
																								   +c2_local[j][k]*phi[j][k+1]+
																								   totalRho[j][k]);
			}

			k = K;
			if (b1_orig[j][k] !=0)
			{
				temp += totalRho[j][k];
				res += sqr(a1_local[j][k]*phi[j-1][k]
												   +b1_orig[j][k]*phi[j][k]
																		 +c1_local[j][k]*phi[j+1][k]+
																		 a2_local[j][k]*phi[j][k-1]
																							   +b2_orig[j][k]*phi[j][k]+
																							   totalRho[j][k]);

				foo[j][k] = sqr(a1_local[j][k]*phi[j-1][k]
														+b1_orig[j][k]*phi[j][k]
																			  +c1_local[j][k]*phi[j+1][k]+
																			  a2_local[j][k]*phi[j][k-1]
																									+b2_orig[j][k]*phi[j][k]+
																									totalRho[j][k]);
			}
		}

		for(k=1;k<K;k++)
		{
			j = 0;
			if (b1_orig[j][k] !=0)
			{
				temp += totalRho[j][k];
				res += sqr(b1_orig[j][k]*phi[j][k]+c1_local[j][k]*phi[j+1][k]+
						a2_local[j][k]*phi[j][k-1]+b2_orig[j][k]*phi[j][k]+
						c2_local[j][k]*phi[j][k+1]+
						totalRho[j][k]);

				foo[j][k] = sqr(b1_orig[j][k]*phi[j][k]
													 +c1_local[j][k]*phi[j+1][k]+
													 a2_local[j][k]*phi[j][k-1]
																		   +b2_orig[j][k]*phi[j][k]
																								 +c2_local[j][k]*phi[j][k+1]+
																								 totalRho[j][k]);
			}

			j = J;
			if (b1_orig[j][k] !=0)
			{
				temp += totalRho[j][k];
				res += sqr(a1_local[j][k]*phi[j-1][k]+b1_orig[j][k]*phi[j][k]+
						a2_local[j][k]*phi[j][k-1]+b2_orig[j][k]*phi[j][k]+
						c2_local[j][k]*phi[j][k+1]+
						totalRho[j][k]);

				foo[j][k] = sqr(a1_local[j][k]*phi[j-1][k]
														+b1_orig[j][k]*phi[j][k]+
														a2_local[j][k]*phi[j][k-1]
																			  +b2_orig[j][k]*phi[j][k]
																									+c2_local[j][k]*phi[j][k+1]+
																									totalRho[j][k]);
			}
		}

		// interior
		for (j=1;j<J;j++) for(k=1;k<K;k++)
		{
			res += sqr(a1_local[j][k]*phi[j-1][k]+b1_orig[j][k]*phi[j][k]
																	   +c1_local[j][k]*phi[j+1][k]+
																	   a2_local[j][k]*phi[j][k-1]+b2_orig[j][k]*phi[j][k]
																													   +c2_local[j][k]*phi[j][k+1]+
																													   totalRho[j][k]);

			foo[j][k] = sqr(a1_local[j][k]*phi[j-1][k]+b1_orig[j][k]*phi[j][k]
																			+c1_local[j][k]*phi[j+1][k]+
																			a2_local[j][k]*phi[j][k-1]+b2_orig[j][k]*phi[j][k]
																															+c2_local[j][k]*phi[j][k+1]+
																															totalRho[j][k]);


			if (b1_orig[j][k]!=0)
				temp += sqr(totalRho[j][k]);
		}
		if (temp)
		{
			res = sqrt(res/temp);
			for (j=0;j<=J;j++) for(k=0;k<=K;k++)
				foo[j][k] = sqrt(foo[j][k]/temp);

		}

		break;

	case CG_SOLVE:
	case GMRES_SOLVE:
		fprintf(stderr,
				"WARNING, CG and GMRES solves has not been written for Boltzmann Solve\n");
		break;

	case MGRID_SOLVE:
		for (j=0;j<=J;j++)
			for (k=0;k<=K;k++)
				resCoeff_local[j][k][4] = resCoeff[j][k][4];

		res = psolve->Resid(totalRho, phi);
	}
	return (res);
}

Vector2 Boltzmann::ParticleFlux(Scalar dt)
{
	Scalar PartCurrent;
	Scalar EnergyCurrent;
	Scalar current = 0.;
	int j,k;

	Boundary* bPtr;
	Boundary*** bc1 = grid->GetBC1();
	Boundary*** bc2 = grid->GetBC2();
	Vector3** dS = grid->dS();

	const Scalar scale = dt/BoltzSpecies->get_q()*.5*sqrt(2*temperature*fabs(ELECTRON_CHARGE)/M_PI/BoltzSpecies->get_m());

	PartCurrent = 0;
	EnergyCurrent = 0;

	for (j=1;j<=J;j++)
		for(k=1;k<=K;k++)
		{
			if((bPtr=bcNode[j][k]))
			{
				if((bPtr=bc1[j-1][k]))
					current = Collect(bPtr, .5*dS[j-1][k].e2(),
							Vector2(j,k),  BoltzmannRho[j][k], scale);

				if((bPtr=bc2[j][k-1]))
					current += Collect(bPtr, .5*dS[j][k-1].e1(),
							Vector2(j,k), BoltzmannRho[j][k], scale);

				PartCurrent += current;
				EnergyCurrent += phi[j][k]*current;
			}
		}

	for (j=1;j<J;j++)
		for(k=1;k<K;k++)
		{
			if((bPtr=bcNode[j][k]))
			{
				if((bPtr=bc1[j][k]))
					current = Collect(bPtr,.5*dS[j][k].e2(),
							Vector2(j,k), BoltzmannRho[j][k],scale);

				if((bPtr=bc2[j][k]))
					current += Collect(bPtr, .5*dS[j][k].e1(),
							Vector2(j,k), BoltzmannRho[j][k], scale);

				PartCurrent += current;
				EnergyCurrent += phi[j][k]*current;
			}
		}

	j=0; k=0;
	if((bPtr=bcNode[j][k]))
	{
		if((bPtr=bc1[j][k]))
			current = Collect(bPtr, .5*dS[j][k].e2(),
					Vector2(j,k), BoltzmannRho[j][k], scale);

		if((bPtr=bc2[j][k]))
			current += Collect(bPtr, .5*dS[j][k].e1(),
					Vector2(j,k), BoltzmannRho[j][k], scale);

		PartCurrent += current;
		EnergyCurrent += phi[j][k]*current;
	}

	j=0;
	for(k=1;k<K;k++)
	{
		if((bPtr=bcNode[j][k]))
		{
			if((bPtr=bc1[j][k]))
				current = Collect(bPtr, .5*dS[j][k].e2(),
						Vector2(j,k), BoltzmannRho[j][k], scale);

			if((bPtr=bc2[j][k]))
				current += Collect(bPtr, .5*dS[j][k].e1(),
						Vector2(j,k), BoltzmannRho[j][k], scale);

			if((bPtr=bc2[j][k-1]))
				current += Collect(bPtr, .5*dS[j][k-1].e1(),
						Vector2(j,k), BoltzmannRho[j][k], scale);

			PartCurrent += current;
			EnergyCurrent += phi[j][k]*current;
		}
	}

	if((bPtr=bcNode[j][k]))
		if((bPtr=bc2[j][k-1]))
		{
			current = Collect(bPtr, .5*dS[j][k-1].e1(),
					Vector2(j,k), BoltzmannRho[j][k], scale);

			PartCurrent += current;
			EnergyCurrent += phi[j][k]*current;
		}

	k=0;
	for (j=1;j<J;j++)
	{
		if((bPtr=bcNode[j][k]))
		{
			if((bPtr=bc1[j][k]))
				current = Collect(bPtr, .5*dS[j][k].e2(),
						Vector2(j,k), BoltzmannRho[j][k], scale);

			if((bPtr=bc1[j-1][k]))
				current = Collect(bPtr, .5*dS[j-1][k].e2(),
						Vector2(j,k), BoltzmannRho[j][k], scale);

			if((bPtr=bc2[j][k]))
				current = Collect(bPtr, .5*dS[j][k].e1(),
						Vector2(j,k), BoltzmannRho[j][k], scale);

			PartCurrent += current;
			EnergyCurrent += phi[j][k]*current;
		}
	}

	if((bPtr=bcNode[j][k]))
		if((bPtr=bc1[j-1][k]))
		{
			current = Collect(bPtr, .5*dS[j-1][k].e2(),
					Vector2(j,k), BoltzmannRho[j][k], scale);

			PartCurrent += current;
			EnergyCurrent += phi[j][k]*current;
		}

	// do correct normalization for particle and energy current

	PartCurrent *= .5*sqrt(2*temperature*fabs(ELECTRON_CHARGE)/M_PI/BoltzSpecies->get_m());  // don't use q_over_m because fabs(ELECTRON_CHARGE) is the converstion from eV to J

	EnergyCurrent *= .5*sqrt(2*temperature/fabs(ELECTRON_CHARGE)/M_PI/BoltzSpecies->get_m());  // don't use q_over_m because fabs(ELECTRON_CHARGE) is the converstion from eV to J

	return (Vector2(PartCurrent, EnergyCurrent));
}

Scalar Boltzmann::Collect(Boundary* bPtr, Scalar area, 
		const Vector2& x,const Scalar& edge_rho,
		const Scalar& scale)
{
	Scalar Flux=0;

	Flux = area*edge_rho*bPtr->getBoltzmannFlux();
	if (Flux)
	{
		Scalar periodicFactor=1;
		if (grid->getPeriodicFlagX1()&&(x.e1()==0||x.e1()==J))
			periodicFactor = .5;
		if (grid->getPeriodicFlagX2()&&(x.e2()==0||x.e2()==K))
			periodicFactor = .5;


		bPtr->collect(*new Particle(x, Vector3(0,0,0),
				BoltzSpecies,
				fabs(periodicFactor*Flux*scale),1),
				*new Vector3(0,0,0));  // varweight particle
	}

	return (Flux);
}

ParticleList& Boltzmann::testParticleList(ParticleGroupList& pgList)
{

	if (pgList.isEmpty()) return pList;
	Species* species = pgList.headData()->get_species();
	ParticleGroup* pg;
	Scalar phi0;

	getMinPot();

	oopicListIter<ParticleGroup> pgIter(pgList);
	for (pgIter.restart(); !pgIter.Done(); pgIter++)
	{
		pg = pgIter();
		for (int index=0; index<pgIter()->get_n(); index++)
		{
			//bilinear interpolation
			phi0 = grid->interpolateBilinear(phi, pg->get_x(index));

			if ((MinPot-phi0+pg->energy_eV(index))<species->get_threshold())
			{
				// make the Boltzmann pg varweight.
				pList.add(new Particle(pg->get_x(index), pg->get_u(index),
						fields->get_bSpecies(),
						pg->get_np2c(index), 1));

				//	Move last particle into this slot
				pg->fill(index);
				index--;
			}
		}
	}
	return pList;
}

void Boltzmann::IncBoltzParticles(ParticleGroupList& pgList)
{

	if (pgList.isEmpty()) return;

	Scalar charge = 0;
	Scalar KEint = 0;
	Scalar phi0=0;
	Scalar Denergy = 0;
	Scalar KE = 0;

	ParticleGroup* pg;

	int i = BoltzSpecies->getID();

	getMinPot();

	oopicListIter<ParticleGroup> pgIter(pgList);
	for (pgIter.restart(); !pgIter.Done(); pgIter++)
	{
		pg = pgIter();
		for (int index=(pgIter()->get_n()-1); index>=0; index--)
		{
			phi0 = grid->interpolateBilinear(phi, pg->get_x(index));
			charge = pg->get_q(index);

			KE = pg->energy_eV(index);
			Denergy = charge*(-phi0+pg->energy_eV(index))/qMKS;
			Scalar FE = charge*(MinPot-phi0+pg->energy_eV(index))/qMKS;
			energy += Denergy;
			totalCharge += charge;

		}
		pg->set_n(0);
	}
}

void Boltzmann::getMinPot(void)
{
	// if sign == 1 return a min
	// if sign == -1 return a max
	// need to add something here to see if phi has changed
	//	Scalar MinPot=sign*10000;
	MinPot = q*10000;
	int sign = (int)(q/fabs(q));

	for(int j=0;j<=J;j++) for(int k=0;k<=K;k++)
		if (grid->PlasmaRegion(j,k))
			if ((sign*MinPot) > (sign*phi[j][k]))
				MinPot = phi[j][k];
}


#ifdef HAVE_HDF5

int Boltzmann::dumpH5(dumpHDF5 &dumpObj)
{

	cerr << "entered boltzman::dumpH5(dumpObj)\n";

	hid_t fileId, groupId, datasetId,dataspaceId;
	herr_t status;
	hsize_t rank;
	hsize_t     phiDim[2];
	hsize_t     dims;

	hid_t  attrdataspaceId,attributeId;
	hid_t scalarType = dumpObj.getHDF5ScalarType();
	string datasetName;

	// Open the hdf5 file with write access
	fileId = H5Fopen(dumpObj.getDumpFileName().c_str(),
			H5F_ACC_RDWR, H5P_DEFAULT);
	groupId = H5Gcreate(fileId,"Boltzmann",0);

	// dump total charge as attribute
	dims = 1;
	attrdataspaceId = H5Screate_simple(1, &dims, NULL);

	attributeId = H5Acreate(groupId, "totalCharge",scalarType,
			attrdataspaceId, H5P_DEFAULT);

	status = H5Awrite(attributeId, scalarType, &totalCharge);
	status = H5Aclose(attributeId);
	status = H5Sclose(attrdataspaceId);

	// dump energy as attribute
	dims = 1;
	attrdataspaceId = H5Screate_simple(1, &dims, NULL);

	attributeId = H5Acreate(groupId, "energy",scalarType,
			attrdataspaceId, H5P_DEFAULT);

	status = H5Awrite(attributeId, scalarType, &energy);
	status = H5Aclose(attributeId);
	status = H5Sclose(attrdataspaceId);

	datasetName = "phi";
	rank = 2;
	phiDim[0] = J+1;
	phiDim[1] = K+1;
	dataspaceId = H5Screate_simple(rank, phiDim, NULL);

	datasetId = H5Dcreate(groupId, datasetName.c_str(), scalarType, dataspaceId,
			H5P_DEFAULT);

	status = H5Dwrite(datasetId, scalarType, H5S_ALL, H5S_ALL,
			H5P_DEFAULT, phi);

	H5Sclose(dataspaceId);
	H5Dclose(datasetId);
	H5Gclose(groupId);
	H5Fclose(fileId);

	return (1);
}

#endif

int Boltzmann::Dump(FILE* DMPFile)
{

	XGWrite(&totalCharge, ScalarInt, 1, DMPFile, ScalarChar);
	XGWrite(&energy, ScalarInt, 1, DMPFile, ScalarChar);
	for (int j=0; j<=J;j++)
		for (int k=0; k<=K;k++)
			XGWrite(&phi[j][k], ScalarInt, 1, DMPFile, ScalarChar);

	return (1);
}


#ifdef HAVE_HDF5

int Boltzmann::restoreH5(dumpHDF5 &restoreObj)
{
	hid_t fileId, groupId, datasetId=0, dataspaceId;
	herr_t status;
	string outFile;
	hid_t scalarType = restoreObj.getHDF5ScalarType();
	hid_t attrId; // attrspaceId,

	dump = true;

	fileId = H5Fopen(restoreObj.getDumpFileName().c_str(),
			H5F_ACC_RDONLY, H5P_DEFAULT);

	groupId = H5Gopen(fileId, "/Boltzmann");

	// Read the totalCharge attribute
	attrId = H5Aopen_name(datasetId, "totalCharge");
	status = H5Aread(attrId,scalarType , &totalCharge);
	status = H5Aclose(attrId);

	// Read the energy attribute
	attrId = H5Aopen_name(datasetId, "energy");
	status = H5Aread(attrId,scalarType , &energy);
	status = H5Aclose(attrId);

	// Read phi
	datasetId = H5Dopen(groupId,"phi" );
	dataspaceId = H5Dget_space(datasetId );

	status = H5Dread(datasetId, scalarType, H5S_ALL, dataspaceId,
			H5P_DEFAULT, phi);

	H5Sclose(dataspaceId);
	H5Dclose(datasetId);
	H5Gclose(groupId);
	H5Fclose(fileId);

	return (1);
}
#endif

int Boltzmann::Restore(FILE* DMPFile)
{
	dump = true;

	XGRead(&totalCharge, ScalarInt, 1, DMPFile, ScalarChar);
	XGRead(&energy, ScalarInt, 1, DMPFile, ScalarChar);
	for (int j=0; j<=J;j++)
		for (int k=0; k<=K;k++)
			XGRead(&phi[j][k], ScalarInt, 1, DMPFile, ScalarChar);

	return (1);
}

int Boltzmann::NonlinearSolve(Scalar** source)
{
	int j,k;
	Scalar res=10;
	int it=0;
	int it_out = 0;
	int itmax=20;

	int EB;
	Scalar pstol = .1f;  //tolerance for the poisson solver => tolerance*pstol

	getMinPot();  // Finds the min pot within the plasma
	UpdateBRho();

	for(j=0;j<=J;j++) for(k=0;k<=K;k++)
		totalRho[j][k] = source[j][k] + BoltzmannRho[j][k];

	EBCFlag = 0;
	PSCFlag = 0;

	while ((EBCFlag==0)&&(PSCFlag==0)&&(it_out < (itmax/5)))
	{
		while ((EBCFlag==0)&&(PSCFlag==0)&&(it < itmax))
		{
			PSolveCoeff(source); // change psolve coefficients
			psolve->solve(phi,rho,itermax,pstol*tolerance);

			getMinPot();
			UpdateBRho();

			for(j=0;j<=J;j++) for(k=0;k<=K;k++)
				totalRho[j][k] = source[j][k] + BoltzmannRho[j][k];

			res = Residual();

			if (res < tolerance)
				PSCFlag = 1;

			EB = EnergyBalance();
			if (EB==1)
			{
				for(j=0;j<=J;j++) for(k=0;k<=K;k++)
					totalRho[j][k] = source[j][k] + BoltzmannRho[j][k];

				res = Residual();
				it++;

			}

			else if (EB==2)
				return (0);

		}
		if (res > tolerance)
		{
			cout << "Nonlinear Boltzmann Solve FAILURE: Residual = "
					<< res << endl;

			pstol *=.5;
			it_out++;
			cout << "Decreasing psolve tolerance to " << pstol*tolerance << endl;
			it = 0;
		}
	}

	if (res > tolerance)
	{
		cout << "Nonlinear Boltzmann Solve TOTAL FAILURE: Residual = "
				<< res << endl;
	}

	return (1);
}

void Boltzmann::LinearSolve(Scalar** source)
{
	int j,k;

	for(j=0;j<=J;j++) for(k=0;k<=K;k++) 
		rho[j][k] = -source[j][k];

	switch(ElectrostaticFlag) {
	default:
		fprintf(stderr,"WARNING, poisson solve defaulting to Multigrid in Boltzmann Solve\n");

	case ELECTROMAGNETIC_SOLVE:  
	case DADI_SOLVE:
	case PERIODIC_X1_DADI:
		for(j=0;j<=J;j++) for(k=0;k<=K;k++){
			if (b1_orig[j][k])
				b1_local[j][k] = b1_orig[j][k];
			if (b2_orig[j][k])
				b2_local[j][k] = b2_orig[j][k];
		}
		break;
	case CG_SOLVE:
	case GMRES_SOLVE:
		fprintf(stderr,"WARNING, CG and GMRES solves has not been written for Boltzmann Solve\n");
		break;
	case MGRID_SOLVE:
		psolve->PSolveBoltzCoeff(0, qbytemp, phi, 0);
	}

	psolve->solve(phi,rho,itermax,tolerance);

	for(j=0;j<=J;j++) for(k=0;k<=K;k++){
		BoltzmannRho[j][k] = 0.0;
		totalRho[j][k] = source[j][k] + BoltzmannRho[j][k];
	}

}

void Boltzmann::UpdateBRho()
{
	int j,k;
	Scalar sum_of_exp = 0.0;

	for(j=0;j<=J;j++) 
		for(k=0;k<=K;k++){
			if (grid->PlasmaRegion(j,k)){
				if ((-(phi[j][k]-MinPot)*qbytemp)<-50.0)
					BoltzmannRho[j][k] = 0;
				else
					BoltzmannRho[j][k] = exp(-(phi[j][k]-MinPot)*qbytemp);
				sum_of_exp += (BoltzmannRho[j][k]*grid->get_halfCellVolumes()[j][k]);
			}
			else 
				BoltzmannRho[j][k] = 0.0;
		}

	n0 = totalCharge/sum_of_exp;

	for(j=0;j<=J;j++) for(k=0;k<=K;k++)
		BoltzmannRho[j][k] *= n0;

	return;
}

// int Boltzmann::EnergyBalance(void) -- returns one if balanced
int Boltzmann::EnergyBalance()  
{
	int j,k;

	Scalar PotEnergy = 0.0;
	Scalar PotEnergyDiv = 0.0;
	Scalar PEgrid = 0.0;
	Scalar deltaT = 0.0;

	Scalar OldTemp = temperature;

	if (EB_flag)
	{
		for(j=0;j<=J;j++) for(k=0;k<=K;k++)
		{
			PEgrid =
					phi[j][k]*BoltzmannRho[j][k]*grid->get_halfCellVolumes()[j][k];

			PotEnergy += PEgrid;
			PotEnergyDiv += PEgrid*(phi[j][k]-MinPot);
		}

		PotEnergy /= fabs(ELECTRON_CHARGE); // J->eV
		PotEnergyDiv *= qbytemp/temperature;
		PotEnergyDiv /= fabs(ELECTRON_CHARGE); // J->eV

		if (PSCFlag&&(fabs((totalCharge*3.0*temperature/qMKS/2.0
				+ PotEnergy - energy)/energy)<tolerance))
		{
			EBCFlag = 1;
		}
		else
		{
			PSCFlag = 0;
			Scalar NewTemp = 2.0*(energy - PotEnergy)*qMKS/totalCharge/3.0;
			temperature = (NewTemp+temperature)/2;

			if (temperature<=0)
			{
				// if the temperature goes negative there is not
				// enought thermal energy for this method to converge

				cout << "temperature went negative.  " << temperature << endl;

				qbytemp = -1;
				totalCharge =0;
				energy = 0;
				temperature = 0;
				return 2;

			}
			else
			{
				qbytemp = q/temperature;
				EBCFlag = 0;
				return 0;
			}
		}
	}
	else
	{
		EBCFlag = 1;
	}

	return 1;
}

Vector2 Collisions(Scalar dt)
{	  
	// currently, charge & energy xfer due to collisions
	// not accounted for (but should be?), JH, Sept. 7, 2005
	return Vector2(0,0);
}
