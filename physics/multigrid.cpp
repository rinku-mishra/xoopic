/**********************************************************************
  see write up on multigrid
  **********************************************************************/

#include <math.h>
#include "multigrid.h"
#include "grid.h"
#include "mg_utils.h"
#ifdef MGRID_DEBUG
extern "C" {
#include <xgrafix.h>
}
#endif

#ifdef _MSC_VER
using std::cout;
using std::cerr;
using std::endl;
#endif

extern "C++" void printf(char *);

void GSRB(Scalar**, Scalar**, const intVector2&, Scalar***, const int&, const int&);
Scalar Norm(Scalar**, const intVector2&,  Grid*);
void Residual(Scalar**, Scalar**, Scalar**, const intVector2&, 
							Scalar***, const int&, const int&);
void Average(Scalar**, Scalar**, const intVector2&, const intVector2&, const int&, const int&);
void CellAve(Scalar**, Scalar**, const intVector2&, const intVector2&);
void Sample(Scalar**, Scalar**, const intVector2&, const intVector2&);
void Interpolate(Scalar**, Scalar**, const intVector2&, const intVector2&, Scalar***);
void Insert(Scalar**, Scalar**, const intVector2&, const intVector2&, Scalar***);

Multigrid::Multigrid() {}

Multigrid::Multigrid(Scalar **_epsi,Grid *grid)		  
{  
	intVector2 temp_upper = intVector2(grid->getJ(),grid->getK());
	PeriodicFlagX1 = grid->getPeriodicFlagX1();
	PeriodicFlagX2 = grid->getPeriodicFlagX2();
	intVector2 temp_length = temp_upper;

	BoltzmannShift = 0;

	int tree_size1 = 1;
	int size1 = temp_length.e1();
	while ((size1%2)==0){
		size1/=2;
		tree_size1++;
	}
	int i;
	if (size1==1) tree_size1--;
	if (size1>=13)
		cout << "Multigrid may not be the Poisson solve for you.  Try to have 2^N = Jmax and 2^M = Kmax." << endl;
	int tree_size2 = 1;
	int size2 = temp_length.e2();
	while (size2%2==0){
		size2/=2;
		tree_size2++;
	}
	if (size2==1) tree_size2--;
	if (size2>=13)
		cout << "Multigrid may not be the Poisson solve for you.  Try to have 2^N = Jmax and 2^M = Kmax." << endl;
	tree_size = MAX(tree_size1,tree_size2);
//	tree_size = MIN(tree_size1,tree_size2);
	if (tree_size == 0) tree_size = 1;

	BadnessFactor = size1*size2;
	

	//		 Allocating the memory for all the levels.
	MultiGrid = new Grid*[tree_size];
	length = new intVector2[tree_size];


	//	setng up coeff for del(epsi(del())) operator
	//        N
	//     W  C  E
	//        S
	//
	size1 = temp_length.e1();
	size2 = temp_length.e2();

	oldphi = new Scalar*[size1+1];
	for (i=0;i<=size1;i++){
		oldphi[i] = new Scalar[size2+1];
		for (int j=0;j<=size2;j++)
			oldphi[i][j]=0.0;
	}
	
	epsi_local = new Scalar**[tree_size];

	resCoeff = new Scalar***[tree_size];
	GSRBCoeff = new Scalar***[tree_size];
	soln = new Scalar**[tree_size];
	rhs = new Scalar**[tree_size];
	res = new Scalar**[tree_size];
	for (level=0; level < tree_size; level++){
	  	soln[level] = new Scalar*[size1+1];
		rhs[level] = new Scalar*[size1+1];
		res[level] = new Scalar*[size1+1];
		resCoeff[level] = new Scalar**[size1+1];
		GSRBCoeff[level] = new Scalar**[size1+1];

		for (i=0;i<=size1;i++){
			soln[level][i] = new Scalar[size2+1];
			rhs[level][i] = new Scalar[size2+1]; 
			res[level][i] = new Scalar[size2+1]; 
			resCoeff[level][i] = new Scalar*[size2+1];
			GSRBCoeff[level][i] = new Scalar*[size2+1];
			for (int j=0;j<=size2;j++){
				resCoeff[level][i][j] = new Scalar[5];
				GSRBCoeff[level][i][j] = new Scalar[5];
			}
		}
		epsi_local[level] = new Scalar*[size1];
		for (i=0;i<size1;i++)
			epsi_local[level][i] = new Scalar[size2];
		
		if ((size1%2==0)&&(size1>2))
			size1/=2;
		if ((size2%2==0)&&(size2>2))
			size2/=2;
	}
	
	level = 0;
	MultiGrid[level] = grid;
	length[level] = temp_upper;

	for (i=0;i<(length[0].e1());i++)
		for (int j=0;j<(length[0].e2());j++)
			epsi_local[level][i][j] = _epsi[i][j];

	for (level=1; level < tree_size; level++){
		if (((length[level-1].e1()%2==0)&&(length[level-1].e1()>2))&&
				((length[level-1].e2()%2==0)&&(length[level-1].e2()>2))){
			coarsen(length[level-1],length[level],2,2);
			MultiGrid[level] = new Grid(length[level].e1(),length[level].e2(),
						    GridCoarsen(MultiGrid[level-1],2,2),
						    MultiGrid[level-1]->query_geometry(),
						    MultiGrid[level-1]->getPeriodicFlagX1(),
						    MultiGrid[level-1]->getPeriodicFlagX2());
			for(int j=0;j<=length[level-1].e1(); j++)
				for(int k=0;k<=length[level-1].e2(); k++){
					Boundary *B = MultiGrid[level-1]->GetNodeBoundary()[j][k];
					if (B!=NULL) 
						MultiGrid[level]->SetNodeBoundary(B,j/2,k/2);
					B = MultiGrid[level-1]->GetCellMask()[j][k];
					if (B!=NULL) 
						MultiGrid[level]->setCellMask(j/2,k/2,B);
				}	
		}
		else if ((length[level-1].e1()%2==0)&&(length[level-1].e1()>2)){
			coarsen(length[level-1],length[level],2,0);
			MultiGrid[level] = new Grid(length[level].e1(),length[level].e2(),
						    GridCoarsen(MultiGrid[level-1],2,1),
						    MultiGrid[level-1]->query_geometry(),
						    MultiGrid[level-1]->getPeriodicFlagX1(),
						    MultiGrid[level-1]->getPeriodicFlagX2());
			for(int j=0;j<=length[level-1].e1(); j++)
				for(int k=0;k<=length[level-1].e2(); k++){
					Boundary *B = MultiGrid[level-1]->GetNodeBoundary()[j][k];
					if (B!=NULL) 
						MultiGrid[level]->SetNodeBoundary(B,j/2,k);
					B = MultiGrid[level-1]->GetCellMask()[j][k];
					if (B!=NULL) 
						MultiGrid[level]->setCellMask(j/2,k,B);
				}	
		}
		else if ((length[level-1].e2()%2==0)&&(length[level-1].e2()>2)){
			coarsen(length[level-1],length[level],0,2);
			MultiGrid[level] = new Grid(length[level].e1(),length[level].e2(),
						    GridCoarsen(MultiGrid[level-1],1,2),
						    MultiGrid[level-1]->query_geometry(),
						    MultiGrid[level-1]->getPeriodicFlagX1(),
						    MultiGrid[level-1]->getPeriodicFlagX2());
			for(int j=0;j<=length[level-1].e1(); j++)
				for(int k=0;k<=length[level-1].e2(); k++){
					Boundary *B = MultiGrid[level-1]->GetNodeBoundary()[j][k];
//					BCTypes type;
					if (B!=NULL) 
						MultiGrid[level]->SetNodeBoundary(B,j,k/2);
					B = MultiGrid[level-1]->GetCellMask()[j][k];
					if (B!=NULL) 
						MultiGrid[level]->setCellMask(j,k/2,B);
			}	
		}
		
		CellAve(epsi_local[level-1],epsi_local[level],
						length[level-1],length[level]);
		
	}

#ifdef MGRID_DEBUG
  char buffer[50];
	x1_array = new Scalar*[tree_size];
	x2_array = new Scalar*[tree_size];
	Jmax = new int[tree_size];
	Kmax = new int[tree_size];
#endif
	for (level=0; level < tree_size; level++){
		int J,K;
		J = length[level].e1();
		K = length[level].e2();
#ifdef MGRID_DEBUG
		Jmax[level] = J+1;
		Kmax[level] = K+1;
		x1_array[level] = new Scalar[J+1];
		x2_array[level] = new Scalar[K+1];
		for(int j=0;j<=J;j++)
			x1_array[level][j]=MultiGrid[level]->getMKS(j,0).e1();

		for(int k=0;k<=K;k++)
			x2_array[level][k]=MultiGrid[level]->getMKS(0,k).e2();
#endif
		for(int j=0;j<=J; j++)
			for(int k=0;k<=K; k++){
				for(int s=0; s<5; s++){
					resCoeff[level][j][k][s] = 0;
					GSRBCoeff[level][j][k][s] = 0;
				}

				BCTypes type;
				Boundary *B = MultiGrid[level]->GetNodeBoundary()[j][k];
				if (B!=NULL) 
					type = B->getBCType();
				else if 
					(((0<k)&&(k<K))&&((0<j)&&(j<J))) type = FREESPACE;
				else type = CONDUCTING_BOUNDARY;
				MGSetCoeff(j,k,type,level);
			}
#ifdef MGRID_DEBUG
		sprintf(buffer, "Residual level %d", level);
		XGSet3D( "linlinlin","X1","X2",strdup(buffer),45.0,225.0,"closed",1,1,
						1.0,1.0,1.0,1,1,1,0,1.0,0,1.0,0.0,1.0);
		XGSurf( x1_array[level],x2_array[level],res[level], &Jmax[level], 
					 &Kmax[level], 1 );

		sprintf(buffer, "Soln level %d", level);
		XGSet3D( "linlinlin","X1","X2",strdup(buffer),45.0,225.0,"closed",1,1,
						1.0,1.0,1.0,1,1,1,0,1.0,0,1.0,0.0,1.0);
		XGSurf( x1_array[level],x2_array[level],soln[level], &Jmax[level], 
					 &Kmax[level], 1 );

		sprintf(buffer, "RHS level %d", level);
		XGSet3D( "linlinlin","X1","X2",strdup(buffer),45.0,225.0,"closed",1,1,
						1.0,1.0,1.0,1,1,1,0,1.0,0,1.0,0.0,1.0);
		XGSurf( x1_array[level],x2_array[level],rhs[level], &Jmax[level], 
					 &Kmax[level], 1 );
#endif
	}

	for (int l=0; l < tree_size; l++)
		for(i=0;i<=length[l].e1(); i++)
			for(int j=0;j<=length[l].e2(); j++){
				soln[l][i][j] = 0.0;
				rhs[l][i][j] = 0.0;
				res[l][i][j] = 0.0;
			}

// the lower level grid are not needed anymore all the needed info is saved locally
// Boltzmann model needs it

//	for (level=1; level < tree_size; level++) //MultiGrid[0] is grid from fields
//		delete MultiGrid[level];	
//	delete [] MultiGrid;
}

/******************************************************/


Multigrid::~Multigrid()
{
	for (level=0; level < tree_size; level++)
		for (int j=0; j<=length[level].e1();j++)
			for (int k=0; k<=length[level].e2();k++){
				delete [] resCoeff[level][j][k];
				delete [] GSRBCoeff[level][j][k];
			}
	for (level=0; level < tree_size; level++) {
		for (int j=0; j<=length[level].e1();j++){
			delete [] soln[level][j];
			delete [] res[level][j];
			delete [] rhs[level][j];
			delete [] resCoeff[level][j];
			delete [] GSRBCoeff[level][j];
		}
	}
	
	for (level=0; level < tree_size; level++){
		delete [] soln[level];
		delete [] res[level];
		delete [] rhs[level];
		delete [] resCoeff[level];
		delete [] GSRBCoeff[level];
	}
	delete [] soln;
	delete [] res;
	delete [] rhs;
	delete [] resCoeff;
	delete [] GSRBCoeff;

	for (level=1; level < tree_size; level++) //MultiGrid[0] is grid from fields
		delete MultiGrid[level];	
	delete [] MultiGrid;

	for (int j=0; j<=length[0].e1();j++)
			delete [] oldphi[j];
	delete [] oldphi;
	for (level=0; level < tree_size; level++)
		for (int j=0; j<length[level].e1();j++)
			delete [] epsi_local[level][j];
	for (level=0; level < tree_size; level++)
		delete [] epsi_local[level];
	delete [] epsi_local;
	delete [] length;
}

/******************************************************/

void Multigrid::mgrelax()   // Recursive relaxation step.
{
  int i;
	//     Call point relaxation step.
	for (i=0; i<(20*level+20); i++)
		GSRB(soln[level], rhs[level], length[level], GSRBCoeff[level], 
				 PeriodicFlagX1, PeriodicFlagX2);

	//		 Check to see whether the coarsest level has been reached.
	//		 If not, call mgrelax on the next level.

	if ((level+1)<tree_size){
//	if (level<MIN(1,tree_size-1)){

		//			  Initialize coarse grid correction to zero.
		for (i = 0;i <= length[level+1].e1(); i++) 
			for (int j = 0;j <= length[level+1].e2(); j++) 
				soln[level+1][i][j] = 0.;

		//       compute residual on fine grid.
		Residual(soln[level],rhs[level],res[level],length[level],resCoeff[level],
						 PeriodicFlagX1, PeriodicFlagX2);

		//		  Average residual onto coarse grid.

		Average(res[level],rhs[level+1], length[level], length[level+1], 
						PeriodicFlagX1, PeriodicFlagX2);
		//		  Call mgrelax recursively on coarse grid data.
		
		level++;
		mgrelax();
		level--;

		//		  Interpolate correction onto fine grid.

		Interpolate(soln[level],soln[level+1],length[level], length[level+1], resCoeff[level]);
	}

	//	 Call relaxation step again, and exit.
	for (i=0; i<(4*level*level+2); i++){
//	for (i=0; i<2; i++)
		GSRB(soln[level],rhs[level],length[level],GSRBCoeff[level],
				 PeriodicFlagX1, PeriodicFlagX2);
	}

}
Scalar Multigrid::errorest()
{
	Scalar norm;
	Residual(soln[0],rhs[0],res[0],length[0],resCoeff[0], PeriodicFlagX1, PeriodicFlagX2);
	norm = Norm(res[0],length[0],MultiGrid[0]);
	return norm;
}

int Multigrid::solve(Scalar **phi, Scalar **rho, int itermax, Scalar tol) 
{
	register int i,j;

	for (i = 0; i <= length[0].e1(); i++) 
		for (j = 0; j <= length[0].e2(); j++) {
			if (GSRBCoeff[0][i][j][SOURCE]==0.0)
				soln[0][i][j] = phi[i][j];
			else
				soln[0][i][j] = 0.0;
			rhs[0][i][j] = rho[i][j];
		}
	Scalar error0 = errorest();
	Scalar error = error0;

	if (error0!=0){
	
		for (i = 0; i <= length[0].e1(); i++) 
			for (j = 0; j <= length[0].e2(); j++) 
				if (!(GSRBCoeff[0][i][j][SOURCE]==0.0)){
//					soln[0][i][j] = phi[i][j];
					soln[0][i][j] = oldphi[i][j];
				}
		int iter2 = 10*BadnessFactor*itermax;

		level = 0;
		error = errorest();
		for (i = 0; (i < iter2) && (error > error0*tol); i++){
			mgrelax();
			for (int foo =0; foo<3; foo++)
	 			GSRB(soln[level],rhs[level],length[level],GSRBCoeff[level], PeriodicFlagX1, PeriodicFlagX2); 
			error = errorest();
#ifdef MGRID_DEBUG
			printf("%d "     "  %g\n",i,error/error0);
#endif
		}
#ifdef MGRID_DEBUG
//		printf("%d "     "  %g\n",i,error/error0);
#endif
		if (error > error0*tol)
			cout << "Multigrid did not reach tolerance.  Residual = "<< error/error0 << endl;
	}
	for (i = 0; i <= length[0].e1(); i++)   
		for (j = 0; j <= length[0].e2(); j++)
			phi[i][j] = oldphi[i][j] = soln[0][i][j];
	
	return 0;
}

/*void Multigrid::init_solve(Grid *grid);
	{

	}
	*/

void Multigrid::set_coefficient(int j, int k, BCTypes type, Grid *grid)
{
// not used
//	level = 0;	
//	MGSetCoeff(j,k,type,level);

}

void Multigrid::MGSetCoeff(int j, int k, BCTypes type, int level)
{
	Scalar iCoeff;
	
	Grid* grid = MultiGrid[level];
	int J,K;
	J=grid->getJ();
	K=grid->getK();

	Scalar* resCjk, *GSRBCjk;
	resCjk = resCoeff[level][j][k];
	GSRBCjk = GSRBCoeff[level][j][k];

	if((type==DIELECTRIC_BOUNDARY) && (((0<k)&&(k<K))&&((0<j)&&(j<J)))) type = FREESPACE;


	switch(type)
		{
		case FREESPACE:
			{
				resCjk[NORTH] = (epsi_local[level][j][k]+epsi_local[level][j-1][k])/2*
					grid->dS2Prime(j,k)/grid->dl2(j,k)/grid->get_halfCellVolumes()[j][k]; 
				resCjk[EAST] = (epsi_local[level][j][k]+epsi_local[level][j][k-1])/2*
					grid->dS1Prime(j,k)/grid->dl1(j,k)/grid->get_halfCellVolumes()[j][k];
				resCjk[SOUTH] = (epsi_local[level][j][k-1]+epsi_local[level][j-1][k-1])/2*
					grid->dS2Prime(j,k-1)/grid->dl2(j,k-1)/grid->get_halfCellVolumes()[j][k];
				resCjk[WEST] = (epsi_local[level][j-1][k]+epsi_local[level][j-1][k-1])/2*
					grid->dS1Prime(j-1,k)/grid->dl1(j-1,k)/grid->get_halfCellVolumes()[j][k];
				resCjk[SOURCE] = -resCjk[NORTH]- resCjk[EAST]
					-resCjk[SOUTH] - resCjk[WEST];
				
				iCoeff = -1/resCjk[SOURCE];
				GSRBCjk[NORTH] = resCjk[NORTH]*iCoeff;
				GSRBCjk[EAST] = resCjk[EAST]*iCoeff;
				GSRBCjk[SOUTH] = resCjk[SOUTH]*iCoeff;
				GSRBCjk[WEST] = resCjk[WEST]*iCoeff;
				GSRBCjk[SOURCE] = -iCoeff;
			}
			break;
		case CONDUCTING_BOUNDARY:
			{
				resCjk[NORTH] = 0.0;
				resCjk[EAST] = 0.0;
				resCjk[SOUTH] = 0.0;
				resCjk[WEST] = 0.0;
				resCjk[SOURCE] = 0.0;

				GSRBCjk[NORTH] = 0.0;
				GSRBCjk[EAST] = 0.0;
				GSRBCjk[SOUTH] = 0.0;
				GSRBCjk[WEST] = 0.0;
				GSRBCjk[SOURCE] = 0.0;
				break;
			}
		case PERIODIC_BOUNDARY:
			{
				int jm, km, jp;
				jm = j-1;
				jp = j;
				km = k-1;
				if (PeriodicFlagX1&&PeriodicFlagX2){
					if (j==0)
						jm = J-1;
					if (k==0) 
						km = K-1;
					if (j==J)
						j=0;
					if (k==K)
						k=0;
					
					resCjk[NORTH] = (epsi_local[level][j][k]+epsi_local[level][jm][k])/2*
						grid->dS2Prime(j,k)/grid->dl2(j,k)/grid->get_halfCellVolumes()[j][k]; 
					resCjk[EAST] = (epsi_local[level][j][k]+epsi_local[level][j][km])/2*
						grid->dS1Prime(j,k)/grid->dl1(j,k)/grid->get_halfCellVolumes()[j][k];
					resCjk[SOUTH] = (epsi_local[level][j][km]+epsi_local[level][jm][km])/2*
						grid->dS2Prime(j,km)/grid->dl2(j,km)/grid->get_halfCellVolumes()[j][k];
					resCjk[WEST] = (epsi_local[level][jm][k]+epsi_local[level][jm][km])/2*
						grid->dS1Prime(jm,k)/grid->dl1(jm,k)/grid->get_halfCellVolumes()[j][k];
				}
				else if (PeriodicFlagX1){
					if (j==0){
						jm = J-1;
						jp = 0;
					}
					if (j==J){
						jm = J-1;
						jp = 0;
					}
					if (k==0){
						resCjk[NORTH] = (epsi_local[level][jp][k]+epsi_local[level][jm][k])/2*
							grid->dS2Prime(jp,k)/grid->dl2(jp,k)/grid->get_halfCellVolumes()[jp][k];
						resCjk[SOUTH] = 0.0;
					}
					else if (k==K){
						resCjk[NORTH] = 0.0;
						resCjk[SOUTH] = (epsi_local[level][jp][km]+epsi_local[level][jm][km])/2*
							grid->dS2Prime(jp,km)/grid->dl2(jp,km)/grid->get_halfCellVolumes()[jp][k];
					}
					else{
						resCjk[NORTH] = (epsi_local[level][jp][k]+epsi_local[level][jm][k])/2*
							grid->dS2Prime(jp,k)/grid->dl2(jp,k)/grid->get_halfCellVolumes()[jp][k]; 
						resCjk[SOUTH] = (epsi_local[level][jp][km]+epsi_local[level][jm][km])/2*
							grid->dS2Prime(jp,km)/grid->dl2(jp,km)/grid->get_halfCellVolumes()[jp][k];
					}

					if (k==K){
						resCjk[WEST] = epsi_local[level][jm][km]*
							grid->dS1Prime(jm,k)/grid->dl1(jm,k)/grid->get_halfCellVolumes()[jp][k];
						resCjk[EAST] = epsi_local[level][jp][km]*
							grid->dS1Prime(jp,k)/grid->dl1(jp,k)/grid->get_halfCellVolumes()[jp][k]; 
					}
					else{ 
						resCjk[WEST] = (epsi_local[level][jm][k]+epsi_local[level][jm][km])/2*
							grid->dS1Prime(jm,k)/grid->dl1(jm,k)/grid->get_halfCellVolumes()[jp][k];
						resCjk[EAST] = (epsi_local[level][jp][k]+epsi_local[level][jp][km])/2*
							grid->dS1Prime(jp,k)/grid->dl1(jp,k)/grid->get_halfCellVolumes()[jp][k];
					}
				}
				else if (PeriodicFlagX2){
					if (k==0) 
						km = K-1;
					if (j==0){
						resCjk[EAST] = (epsi_local[level][j][k]+epsi_local[level][j][km])/2*
							grid->dS1Prime(j,k)/grid->dl1(j,k)/grid->get_halfCellVolumes()[j][k];
						resCjk[WEST] = 0.0;
					}
					else if (j==J){
						resCjk[EAST] = 0.0;
						resCjk[WEST] = (epsi_local[level][jm][k]+epsi_local[level][jm][km])/2*
							grid->dS1Prime(jm,k)/grid->dl1(jm,k)/grid->get_halfCellVolumes()[j][k];
					}
					else {
						resCjk[EAST] = (epsi_local[level][j][k]+epsi_local[level][j][km])/2*
							grid->dS1Prime(j,k)/grid->dl1(j,k)/grid->get_halfCellVolumes()[j][k];
						resCjk[WEST] = (epsi_local[level][jm][k]+epsi_local[level][jm][km])/2*
							grid->dS1Prime(jm,k)/grid->dl1(jm,k)/grid->get_halfCellVolumes()[j][k];
					}

					if (j==0){
						resCjk[NORTH] = epsi_local[level][j][k]*
							grid->dS2Prime(j,k)/grid->dl2(j,k)/grid->get_halfCellVolumes()[j][k]; 
						resCjk[SOUTH] = epsi_local[level][j][km]*
							grid->dS2Prime(j,km)/grid->dl2(j,km)/grid->get_halfCellVolumes()[j][k];
					}
					else if (j==J){
						resCjk[NORTH] = epsi_local[level][jm][k]*
							grid->dS2Prime(j,k)/grid->dl2(j,k)/grid->get_halfCellVolumes()[j][k]; 
						resCjk[SOUTH] = epsi_local[level][jm][km]*
							grid->dS2Prime(j,km)/grid->dl2(j,km)/grid->get_halfCellVolumes()[j][k];
					}
					else {
						resCjk[NORTH] = (epsi_local[level][j][k]+epsi_local[level][jm][k])/2*
							grid->dS2Prime(j,k)/grid->dl2(j,k)/grid->get_halfCellVolumes()[j][k]; 
						resCjk[SOUTH] = (epsi_local[level][j][km]+epsi_local[level][jm][km])/2*
							grid->dS2Prime(j,km)/grid->dl2(j,km)/grid->get_halfCellVolumes()[j][k];
					}
				}

				resCjk[SOURCE] = -resCjk[NORTH]- resCjk[EAST]
					-resCjk[SOUTH] - resCjk[WEST];	
				iCoeff = -1/resCjk[SOURCE];
				GSRBCjk[NORTH] = resCjk[NORTH]*iCoeff;
				GSRBCjk[EAST] = resCjk[EAST]*iCoeff;
				GSRBCjk[SOUTH] = resCjk[SOUTH]*iCoeff;
				GSRBCjk[WEST] = resCjk[WEST]*iCoeff;
				GSRBCjk[SOURCE] = -iCoeff;

				break;
			}
		case DIELECTRIC_BOUNDARY:
		case CYLINDRICAL_AXIS:	
			{
				if ((j==J)&&(k==K)){
					resCjk[NORTH] = 0.0;
					resCjk[EAST] = 0.0;
					resCjk[SOUTH] = epsi_local[level][j-1][k-1]*grid->dS2Prime(j,k-1)/
						grid->dl2(j,k-1)/grid->get_halfCellVolumes()[j][k];
					resCjk[WEST] = epsi_local[level][j-1][k-1]*grid->dS1Prime(j-1,k)/
						grid->dl1(j-1,k)/grid->get_halfCellVolumes()[j][k];
				}				
				else if ((j==0)&&(k==0)){
					resCjk[NORTH] = epsi_local[level][j][k]*grid->dS2Prime(j,k)/
						grid->dl2(j,k)/grid->get_halfCellVolumes()[j][k]; 
					resCjk[EAST] = epsi_local[level][j][k]*grid->dS1Prime(j,k)/
						grid->dl1(j,k)/grid->get_halfCellVolumes()[j][k];
					resCjk[SOUTH] = 0.0;
					resCjk[WEST] = 0.0;
				}				
				else if ((j==0)&&(k==K)){
					resCjk[NORTH] = 0.0;
					resCjk[EAST] = epsi_local[level][j][k-1]*grid->dS1Prime(j,k)/
						grid->dl1(j,k)/grid->get_halfCellVolumes()[j][k];
					resCjk[SOUTH] = epsi_local[level][j][k-1]*grid->dS2Prime(j,k-1)/
						grid->dl2(j,k-1)/grid->get_halfCellVolumes()[j][k];
					resCjk[WEST] = 0.0;
				}
				else if ((j==J)&&(k==0)){
					resCjk[NORTH] = epsi_local[level][j-1][k]*grid->dS2Prime(j,k)/
						grid->dl2(j,k)/grid->get_halfCellVolumes()[j][k]; 
					resCjk[EAST] = 0.0;
					resCjk[SOUTH] = 0.0;
					resCjk[WEST] = epsi_local[level][j-1][k]*grid->dS1Prime(j-1,k)/
						grid->dl1(j-1,k)/grid->get_halfCellVolumes()[j][k];
				}
				else if (k==K){
					resCjk[NORTH] = 0.0;
					resCjk[EAST] = epsi_local[level][j][k-1]*grid->dS1Prime(j,k)/
						grid->dl1(j,k)/grid->get_halfCellVolumes()[j][k];
					resCjk[SOUTH] = (epsi_local[level][j-1][k-1]+epsi_local[level][j][k-1])/2*
						grid->dS2Prime(j,k-1)/
							grid->dl2(j,k-1)/grid->get_halfCellVolumes()[j][k];
					resCjk[WEST] = epsi_local[level][j-1][k-1]*grid->dS1Prime(j-1,k)/
						grid->dl1(j-1,k)/grid->get_halfCellVolumes()[j][k];
				}
				else if (k==0){
					resCjk[NORTH] = (epsi_local[level][j][k]+epsi_local[level][j-1][k])/2*
						grid->dS2Prime(j,k)/
							grid->dl2(j,k)/grid->get_halfCellVolumes()[j][k]; 
					resCjk[EAST] = epsi_local[level][j][k]*grid->dS1Prime(j,k)/
						grid->dl1(j,k)/grid->get_halfCellVolumes()[j][k];
					resCjk[SOUTH] = 0.0;
					resCjk[WEST] = epsi_local[level][j-1][k]*grid->dS1Prime(j-1,k)/
						grid->dl1(j-1,k)/grid->get_halfCellVolumes()[j][k];
				}				
				else if (j==0){
					resCjk[NORTH] = epsi_local[level][j][k]*
						grid->dS2Prime(j,k)/
							grid->dl2(j,k)/grid->get_halfCellVolumes()[j][k]; 
					resCjk[EAST] = (epsi_local[level][j][k]+epsi_local[level][j][k-1])/2*
						grid->dS1Prime(j,k)/
							grid->dl1(j,k)/grid->get_halfCellVolumes()[j][k];
					resCjk[SOUTH] = epsi_local[level][j][k-1]*grid->dS2Prime(j,k-1)/
						grid->dl2(j,k-1)/grid->get_halfCellVolumes()[j][k];
					resCjk[WEST] = 0.0;
				}
				else if (j==J){
					resCjk[NORTH] = epsi_local[level][j-1][k]*
						grid->dS2Prime(j,k)/
							grid->dl2(j,k)/grid->get_halfCellVolumes()[j][k]; 
					resCjk[EAST] = 0.0;
					resCjk[SOUTH] = epsi_local[level][j-1][k-1]*grid->dS2Prime(j,k-1)/
						grid->dl2(j,k-1)/grid->get_halfCellVolumes()[j][k];
					resCjk[WEST] = (epsi_local[level][j-1][k]+epsi_local[level][j-1][k])/2*
						grid->dS1Prime(j-1,k)/
							grid->dl1(j-1,k)/grid->get_halfCellVolumes()[j][k];
				}
				else 
					cout << "MultiGrid edge error" << endl;
				

				resCjk[SOURCE] = -resCjk[NORTH]- resCjk[EAST]
					-resCjk[SOUTH] - resCjk[WEST];
				
				iCoeff = -1/resCjk[SOURCE];
				GSRBCjk[NORTH] = resCjk[NORTH]*iCoeff;
				GSRBCjk[EAST] = resCjk[EAST]*iCoeff;
				GSRBCjk[SOUTH] = resCjk[SOUTH]*iCoeff;
				GSRBCjk[WEST] = resCjk[WEST]*iCoeff;
				GSRBCjk[SOURCE] = -iCoeff;
		
				break;
			}
		default:
			{
//			  cout << "Multigrid doesn't know about this boundary condition type!\n;" << endl;
//			  cout << "(was it  SPATIAL_REGION_BOUNDARY?)\n" << endl;
//			  e_xit(1);
			}
			
		}
}

Vector2** Multigrid::GridCoarsen(Grid* grid, int ratio1, int ratio2)
{	
	Vector2** X;
	int j;
	X = new Vector2*[length[level].e1()+1];
	for (j=0; j<=length[level].e1(); j++){
		X[j] = new Vector2[length[level].e2()+1];
		for (int k=0; k<=length[level].e2(); k++)
			X[j][k] = grid->getMKS(ratio1*j,ratio2*k);
	}
	
	return X;
}

Scalar Multigrid::Resid(Scalar**rho, Scalar**phi)
{
	Scalar norm;
	for (int i = 0; i <= length[0].e1(); i++) 
		for (int j = 0; j <= length[0].e2(); j++) {
			if (GSRBCoeff[0][i][j][SOURCE]==0.0)
				soln[0][i][j] = phi[i][j];
			else
				soln[0][i][j] = 0.0;
			rhs[0][i][j] = -rho[i][j];
		}
	Scalar error0 = errorest();
	Residual(phi, rhs[0], res[0],length[0],resCoeff[0], PeriodicFlagX1, PeriodicFlagX2);
	norm = Norm(res[0],length[0],MultiGrid[0]);
	norm /= error0;
	return norm;
}

void Multigrid::PSolveBoltzCoeff(const Scalar& n0, const Scalar& qbyT, Scalar** brho, const Scalar& MinPot)
{

	Scalar temp;
	Scalar iCoeff;
	int J,K;
	Scalar* resCjk, *GSRBCjk;

	for (int i = 0; i <= length[0].e1(); i++) 
		for (int j = 0; j <= length[0].e2(); j++) 
			rhs[0][i][j] = brho[i][j];
		

	for (level=0; level < (tree_size-1); level++)
		Average(rhs[level], rhs[level+1], length[level], length[level+1], 
						PeriodicFlagX1, PeriodicFlagX2);

	for (level=0; level < tree_size; level++){
		J = length[level].e1();
		K = length[level].e2();
		for(int j=0;j<=J; j++)
			for(int k=0;k<=K; k++){
				GSRBCjk = GSRBCoeff[level][j][k];
				if (GSRBCjk[SOURCE])
					if(MultiGrid[level]->PlasmaRegion(j,k)){
						resCjk = resCoeff[level][j][k];
						temp = -rhs[level][j][k];
						if (n0)
							resCjk[SOURCE] = -resCjk[NORTH]- resCjk[EAST]
								-resCjk[SOUTH] - resCjk[WEST] + temp*qbyT;
						else
							resCjk[SOURCE] = -resCjk[NORTH]- resCjk[EAST]
								-resCjk[SOUTH] - resCjk[WEST];
						
						//this code chould be sped up a lot resCoeff_local is 
						//stored in the boltzmann object.

						iCoeff = -1/resCjk[SOURCE];
						GSRBCjk[NORTH] = resCjk[NORTH]*iCoeff;
						GSRBCjk[EAST] = resCjk[EAST]*iCoeff;
						GSRBCjk[SOUTH] = resCjk[SOUTH]*iCoeff;
						GSRBCjk[WEST] = resCjk[WEST]*iCoeff;
						GSRBCjk[SOURCE] = -iCoeff;
					}
			}
	}
	return;
}









