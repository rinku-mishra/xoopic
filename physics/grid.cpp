//====================================================================
/*

GRID.CPP

Purpose:	Member function definitions for the Grid class.  This 
		class provides the basis functions for the geometry of 
		the region and translations between physical and grid 
		coordinates.  Grid coordinates consist of pairs 
		(j+w1, k+w2), where j, k are grid indices and w1, w2 
		are fractions of the jth, kth cell.  Note that in non 
		uniform grids, the mapping is not linear between the 
		coordinates.

Revision/Programmer/Date
0.1	(JohnV, 03-15-93)	Proto code.
0.9	(JohnV, 04-30-93)	Initial draft with particles.
0.91	(NTG, JohnV, 05-05-93) Add EmittingBoundary, ActiveBoundary, class
		library use for list management and iterators, KinematicalObject.
0.93	(JohnV, NTG 05-12-93) Fix Windows large model Gen. Prot. Fault due
		to reading outside arrays.
0.95	(JohnV, 06-29-93) Add Boundary::setPassives() to set up passive
		boundary conditions (Fields::iC and ::iL), add Fields::set_iCx()
		to support above.
0.96	(JohnV 08-23-93) Add new Boundary subtypes: BeamEmitter,
		CylindricalAxis, FieldEmitter, Port.  Modifications to
		Maxwellian: stores all parameters in MKS.
0.961	(PeterM, JohnV, 08-24-93) Replace Borland list containers with
		vanilla C++ template lists in oopiclist.h
0.963	(JohnV, 09-24-93)	Add Particle object for moving particle quantities
		around (emission, collection, etc.)
0.964	(JohnV, 11-22-93) Make agreed-upon changes in names: Vector->Vector3,
		Boundaries->BoundaryList, Particles->ParticleList, ParticleGroups
		->ParticleGroupList
0.965	(JohnV, 02-01-94) Move magnetic field advance to Fields::advanceB(),
		move boundary code, particle code to respective files.
0.966	(JohnV, 02-07-94)	Fixed striation in vr vs. z phase space seen
		for beam spreading in a simple cylinder.  Problem was weighting
		error in Grid::interpolateBilinear().
0.967	(JohnV 03-03-94) added Fields::setBz0() and ::epsilonR.
0.968	(JohnV 03-11-94) Fix divide by zero for non-translating particles
		with finite spin in Fields::translateAccumulate().  Also ensure all
		particles that touch boundaries get collected in Grid::translate()
		by changing </> to <=/>= for check conditions.
0.969	(JohnV 03-23-94) Restructure Grid, Fields, and SpatialRegion to
		an association rather than inheritance.
0.97	(JohnV 01-23-95) Clean up initialHalfCellVolumes().
0.98	(JohnV 06-30-95) Change setBoundaries() so that j<=J, k<=K, also
		streamline translate().
0.981	(JohnV 07-02-95) Undid above change to setBoundaries(); it was OK.
0.982 (JohnV 07-03-95) Loss of precision had caused some particles to
		"jump" from inside the system to the border of the system.
		w1new w2new changed to prevent this.
0.99  (KeithC 08-25-95) Lack of precision caused Bz0 to 'leak` into Etheta and Br.
      Added BNodeStatic to separate the Static part from the B fields that that Fields
      updates.  Changed Fields::setBz0() to Fields::setBNodeStatic.
1.00  (PeterM 03-11-97) Fix to GetGridCoords so that it won't return a value
      beyond the end of the mesh.
1.1   (JohnV 07-24-97) Repaired rare bug in translate which allowed particles to
      tunnel through boundaries without being collected. This is the result of
		the compiler (with optimization) comparing the position stored in an 80 bit
		register to the system edge, then later rounding the 80 bit value to fit in
		a 32 bit variable.
1.11  (JohnV 07-25-97) Added corner boundary checks.
2.0   (JohnV 12-08-97) Reverted to simpler translete(), using volatile to force 
      truncation to Scalar.
2.01  (JohnV 01-14-98) Repaired vertical and horizontal move problem in finding the
      x1Interept and x2Intercept - symptom was infinite loop in which particle did
		not move.
2.02  (JohnV 03-04-98) Added binomialFilter() to smooth arrays in place.
2.03  (JohnV 06-12-98) Repaired mInfinite=TRUE bug for particles exactly on a 
      grid edge moving vertically.
2.04  (JohnV 06-24-98) Added new mInfinite=TRUE case for edge vertical moves.

*/
//====================================================================

#include	"grid.h"
#include "ptclgrp.h"
#include "ostring.h"
#include "ngd.h"

//--------------------------------------------------------------------
//	Allocate the arrays for the grid coordinates.

Grid::Grid(int _J, int _K, Vector2** g,int geometry, BOOL _PeriodicFlagX1, BOOL _PeriodicFlagX2)
{
	J = _J;
	K = _K;
	PeriodicFlagX1 = _PeriodicFlagX1;
	PeriodicFlagX2 = _PeriodicFlagX2;
	X = g;
	bc1 = new Boundary** [J+1];
	bc2 = new Boundary** [J+1];
	NodeBoundary = new Boundary** [J+1];
	CellMask = new Boundary**[J+1];  //should be J

	halfCellVolumes = new Scalar* [J+1];
	cellVolume_array = new Scalar* [J+1]; //should be J
	dl_array = new Vector3* [J+1];
	dS_array = new Vector3* [J+1];
	dSPrime_array = new Vector3* [J+1];
	dlPrime_array = new Vector3* [J+1];
	norm1 = new int* [J+1];
	norm2 = new int* [J+1];
	int j;
	int k;

	for (j = 0; j<=J; j++)
	{
		bc1[j] = new Boundary* [K+1];
		bc2[j] = new Boundary* [K+1];
		NodeBoundary[j] = new Boundary* [K+1];
		CellMask[j] = new Boundary* [K+1]; //should be K
		halfCellVolumes[j] = new Scalar [K+1];  //Cell volumes centered on the Nodes
		cellVolume_array[j] = new Scalar [K+1]; //should be K //Cell volumes
		dl_array[j] = new Vector3 [K+1];
		dS_array[j] = new Vector3 [K+1];
		dSPrime_array[j] = new Vector3 [K+1];
		dlPrime_array[j] = new Vector3 [K+1];
		norm1[j] = new int [K+1];
		norm2[j] = new int [K+1];
		for (k=0; k<=K; k++) {
			NodeBoundary[j][k]=bc1[j][k] = bc2[j][k] = CellMask[j][k] = NULL;
			norm1[j][k]=norm2[j][k]=0;
		}
	}

	set_geometryFlag(geometry);
	if (X[0][0].e2() == 0.0 && geometryFlag == ZRGEOM) axial = TRUE;
	else axial = FALSE;
	radial0 = 0.75;
	radialK = (3*K - 0.75)/(3*K - 1);
	initHalfCellVolumes();
	init_cellVolume();
	init_dl();
	init_dS();
	init_dlPrime();
	init_dSPrime();
#ifdef MPI_VERSION
	M=1,N=1;
	MaxMPIJ=J;MaxMPIK=K;
	neighbor[0]=neighbor[1]=neighbor[2]=neighbor[3]=-1;
#endif
}

//--------------------------------------------------------------------
//	Free Grid arrays.

Grid::~Grid()
{	int j;
	for (j=0; j<=J; j++)
	{
		delete[] bc1[j];
		delete[] bc2[j];
		delete[] NodeBoundary[j];
		delete[] CellMask[j];
		delete[] halfCellVolumes[j];
		delete[] cellVolume_array[j];
		delete[] dl_array[j];
		delete[] dS_array[j];
		delete[] dSPrime_array[j];
		delete[] dlPrime_array[j];
		delete[] norm1[j];
		delete[] norm2[j];
	}
	delete[] bc1;
	delete[] bc2;
	delete[] NodeBoundary;
	delete[] CellMask;
	delete[] halfCellVolumes;
	delete[] cellVolume_array;
	delete[] dl_array;
	delete[] dS_array;
	delete[] dSPrime_array;
	delete[] dlPrime_array;
	delete[] norm1;
	delete[] norm2;
	for (j=0; j <= J; j++) delete[] X[j];
	delete[] X;
	X = NULL;
}

//--------------------------------------------------------------------
// Applies a binomial filter (4, 2, 1) in 2d to filter the short wavelengths
// in the arg array. n specifies the number of times to apply the filter.
// In order to do this in place (i.e. avoid a scratch array) the filter is performed 
// in four passes (B&L, Appendix C). The array is assumed to be on the grid,
// with dimensions JxK. The smoothing presently does not move charge off 
// boundary surfaces.

// need to fix for periodic bc

void Grid::binomialFilter(Scalar **array, int n)
{
  int i, j, k;
  for (i=0; i<n; i++){
	 for (j=0; j<=J; j++) // +x1 pass
		for (k=0; k<=K; k++)
		  if (j==J || (CellMask[j][k] && CellMask[j][MAX(0,k-1)]))
			 array[j][k] += array[j][k]; // so we can divide by 16 later
		  else array[j][k] += array[j+1][k];
	 for (j=J; j>=0; j--) // -x1 pass
		for (k=0; k<=K; k++)
		  if (j==0 || (CellMask[j-1][k] && CellMask[j-1][MAX(0,k-1)]))
			 array[j][k] += array[j][k]; // so we can divide by 16 later
		  else array[j][k] += array[j-1][k];
	 for (j=0; j<=J; j++) // +x2 pass
		for (k=0; k<=K; k++)
		  if (k==K || (CellMask[j][k] && CellMask[MAX(0,j-1)][k]))
			 array[j][k] += array[j][k]; // so we can divide by 16 later
		  else array[j][k] += array[j][k+1];
	 for (j=0; j<=J; j++) // -x2 pass
		for (k=K; k>=0; k--){
		  if (k==0 || (CellMask[j][k-1] && CellMask[MAX(0,j-1)][k-1]))
			 array[j][k] += array[j][k]; // so we can divide by 16 later
		  else array[j][k] += array[j][k-1];
		  array[j][k] *= 6.25e-2;
		}
	 if (PeriodicFlagX1){ // average along periodic boundaries
		for (k=0; k<=K; k++) {
		  array[0][k] += array[J][k];
		  array[0][k] *= 0.5;
		  array[J][k] = array[0][k]; // synchronize
		}
	 }
	 if (PeriodicFlagX2){
		for (j=0; j<=J; j++) {
		  array[j][0] += array[j][K];
		  array[j][0] *= 0.5;
		  array[j][K] = array[j][0];
		}
	 }
  }
}

//--------------------------------------------------------------------
//	Set boundary pointers as appropriate.

void Grid::setBoundaryMask(BoundaryList& boundaryList)
{
	oopicListIter<Boundary>	boundary(boundaryList);
	for (boundary.restart(); !boundary.Done(); boundary++)
	{
		boundary.current()->setBoundaryMask(*this);
/*    OLD VERSION:
		int lo, hi;
		if (boundary.current()->alongx2())//	vertical
		{
			lo = MIN(boundary.current()->get_k1(), boundary.current()->get_k2());
			hi = MAX(boundary.current()->get_k1(), boundary.current()->get_k2());
			for (int k=lo; k<hi; k++)
				bc2[boundary.current()->get_j1()][k] = boundary.current();
		}
		else										//	horizontal
		{
			lo = MIN(boundary.current()->get_j1(), boundary.current()->get_j2());
			hi = MAX(boundary.current()->get_j1(), boundary.current()->get_j2());
			for (int j=lo; j<hi; j++)
				bc1[j][boundary.current()->get_k1()] = boundary.current();
		}
*/
	}
}

void Grid::SetNodeBoundary(Boundary *B,int j,int k)
{
  if(NodeBoundary[j][k]==NULL) NodeBoundary[j][k]=B;
  // if the boundary is lower on the pecking order, replace it
  //  with the one just reporting
  else if(NodeBoundary[j][k]->getBCType() < B->getBCType()) NodeBoundary[j][k]=B; 

}

//--------------------------------------------------------------------
//	Return grid coordinates from MKS coordinates.
//	ntg5-4 Crude search method for finding grid coordinates (could be improved)
// Points outside the grid will be snapped back to the extremest points of the
// grid.

Vector2 Grid::getGridCoords(const Vector2& xMKS)
{
	Scalar x1 = xMKS.e1();
	// Scalar x1grid;
        Scalar xlo = X[0][0].e1();
	Scalar xhi = X[1][0].e1(); 
	int j;
	for (j = 0; j < J; j++)
	{
		xlo = X[j][0].e1(); xhi = X[j+1][0].e1();
		if((x1 >= xlo) && (x1 <= xhi)) break;
	}

	Scalar x1grid = j + (x1 - xlo)/(xhi - xlo);
	x1grid=MIN(x1grid,J);
	x1grid=MAX(x1grid,0);
	Scalar x2 = xMKS.e2(), x2grid;
	int k;
	for (k = 0; k < K; k++)
	{
		xlo = X[0][k].e2(); xhi = X[0][k+1].e2();
		if((xlo <= x2) && (x2 <= xhi)) break;
	}
	x2grid = k + (x2 - xlo)/(xhi - xlo);
	x2grid=MIN(x2grid,K);
	x2grid=MAX(x2grid,0);

	return Vector2(x1grid, x2grid);						//	replace with Westermann method.
}

// returns -1,-1 if the point is outside the grid.
Vector2 Grid::getNearestGridCoords(const Vector2& xMKS)
{
	Scalar x1 = xMKS.e1(), x1grid,dx;
        Scalar xlo = X[0][0].e1();
	Scalar xhi = X[1][0].e1(); 
	int j;
	for (j = 0; j < J; j++)
	{
		xlo = X[j][0].e1(); xhi = X[j+1][0].e1();
		if((x1 >= xlo) && (x1 <= xhi)) break;
	}
	dx=xhi-xlo;
	if(j==J) xlo = X[j][0].e1(); // catch things near the last edge.
	x1grid = j + (x1 - xlo)/dx;

	// round it off to the nearest grid
	if(x1grid >= 0) x1grid = x1grid + 0.5;
	else x1grid = x1grid - 0.5;

	if(x1grid > J+0.75  || x1grid < -0.75) { x1grid = -1; }  // outside this mesh.

	Scalar x2 = xMKS.e2(), x2grid;
	int k;
	for (k = 0; k < K; k++)
	{
		xlo = X[0][k].e2(); xhi = X[0][k+1].e2();
		if((xlo <= x2) && (x2 <= xhi)) break;
	}
	dx=xhi-xlo;
	if(k==K) xlo = X[0][k].e2(); // catch things near the last edge.
	x2grid = k + (x2 - xlo)/dx;


	// round it off to the nearest grid
	if(x2grid >= 0) x2grid = x2grid + 0.5;
	else x2grid = x2grid - 0.5;

	if(x2grid > K +0.75  || x2grid < -0.75) { x2grid = -1; }  // outside this mesh.

	if(x1grid == -1 || x2grid == -1) return Vector2(-1,-1);
	// truncate
	x1grid = (int) x1grid;
	x2grid = (int) x2grid;
	return Vector2(x1grid, x2grid);						//	replace with Westermann method.
}

//-----------------------------------------------
//  Initialize the cell volumes array.  This array is
//  used by the charge collection routine.

void Grid::initHalfCellVolumes(void)
{
	int	j, k;

	switch(geometryFlag)
	{
		case ZXGEOM:
			for (j=0; j<=J; j++)
			{
				Scalar	zp, zm, xp;
				Scalar	xm = X[j][0].e2();
				for (k=0; k<=K; k++)
				{
					zp = (j==J) ? X[j][k].e1() : X[j+1][k].e1();
					zm = (j==0) ? X[j][k].e1() : X[j-1][k].e1();
					xp = (k==K) ? X[j][k].e2() : X[j][k+1].e2();
					xm = (k==0) ? X[j][k].e2() : X[j][k-1].e2();
					halfCellVolumes[j][k] = 0.25*(zp - zm)*(xp - xm);
				}
			}
		break;

		case ZRGEOM:
		default:
			for (j=0; j<=J; j++)
			{
				Scalar	zp, zm, rp2;
				Scalar	rm2 = sqr(X[j][0].e2());
				for (k=0; k<=K; k++)
				{
					zp = (j==J) ? X[j][k].e1() : X[j+1][k].e1();
					zm = (j==0) ? X[j][k].e1() : X[j-1][k].e1();
					rp2 = (k==K) ? sqr(X[j][k].e2())
						: sqr(0.5*(X[j][k+1].e2() + X[j][k].e2()));
					halfCellVolumes[j][k] = M_PI*0.5*(zp - zm)*(rp2 - rm2);
					if (axis()) { // apply radial half cell vol corrections
					  if (k == 0) {
						  halfCellVolumes[j][k] /= radial0;
					  } else if (k==K) {
						  halfCellVolumes[j][k] /= radialK;
					  }
					}

					rm2 = rp2;
				}
			}
		break;
		}
	if (PeriodicFlagX1) {
		for (k=0;k<=K;k++){
			halfCellVolumes[0][k] += halfCellVolumes[J][k];
			halfCellVolumes[J][k] = halfCellVolumes[0][k];
		}
	}

	if (PeriodicFlagX2) {
		for (j=0; j<=J; j++){
			halfCellVolumes[j][0] += halfCellVolumes[j][K];
			halfCellVolumes[j][K] = halfCellVolumes[j][0];
		}
	}
}

void Grid::init_cellVolume(void)
{
	int j,k;
	switch(geometryFlag)
		{
		case ZXGEOM: 
			for (j=0; j<J; j++) {
				for (k=0; k<K; k++) {
					cellVolume_array[j][k] = (X[j][k+1].e2() - X[j][k].e2())* (X[j+1][k].e1() - X[j][k].e1());
				}
			}
			break;
		case ZRGEOM:
		default: 
			for (j=0; j<J; j++) {
				for (k=0; k<K; k++) {
					cellVolume_array[j][k] = M_PI*(X[j][k+1].e2()*X[j][k+1].e2() - X[j][k].e2()*X[j][k].e2())*(X[j+1][k].e1() - X[j][k].e1());
				}
			}
			break;
		}
}

void Grid::init_dl(void)
{
	int j,k;
	switch(geometryFlag)
		{
		case ZXGEOM: 
			for (j=0; j<=J; j++) {
				for (k=0; k<=K; k++) {
					dl_array[j][k] = Vector3(((j==J) ? 0 : X[j+1][k].e1() - X[j][k].e1()),
																	 ((k==K) ? 0 : X[j][k+1].e2() - X[j][k].e2()),
																	 1.0);
				}
			}
					
			break;
		case ZRGEOM:
		default: 
			for (j=0; j<=J; j++) {
				for (k=0; k<=K; k++) {
					dl_array[j][k] = Vector3(((j==J) ? 0 : X[j+1][k].e1() - X[j][k].e1()),
														 ((k==K) ? 0 : X[j][k+1].e2() - X[j][k].e2()),
														 2*M_PI*X[j][k].e2());
				}
			}
			break;
		}
	if (PeriodicFlagX1) {
		for (k=0;k<=K;k++) {
			dl_array[J][k] = dl_array[0][k];
		}
	}
		
	if (PeriodicFlagX2) {
		for (j=0; j<=J; j++) {
			dl_array[j][K] = dl_array[j][0];
		}
	}
}

void Grid::init_dS(void)
{
	int j,k;
	switch(geometryFlag)
		{
		case ZXGEOM: 
			for (j=0; j<=J; j++) {
				for (k=0; k<=K; k++) {
					dS_array[j][k] = Vector3(((k==K) ? 0 : X[j][k+1].e2() - X[j][k].e2()),
														 ((j==J) ? 0 : X[j+1][k].e1()-X[j][k].e1()),
														 (((j==J)||(k==K)) ? 0 : (X[j+1][k].e1() - X[j][k].e1())*(X[j][k+1].e2() - X[j][k].e2())));
				}
			}
			break;
		case ZRGEOM:
		default: 
			for (j=0; j<=J; j++) {
				for (k=0; k<=K; k++) {
					dS_array[j][k] = Vector3(((k==K) ? 0 : M_PI*(X[j][k+1].e2()*X[j][k+1].e2() - X[j][k].e2()*X[j][k].e2())),
														 ((j==J) ? 0 : 2*M_PI*X[j][k].e2()*(X[j+1][k].e1() - X[j][k].e1())),
														 (((j==J)||(k==K)) ? 0 : (X[j+1][k].e1() - X[j][k].e1())*(X[j][k+1].e2() - X[j][k].e2())));
				}
			}
			break;
		}

	if (PeriodicFlagX1) {
		for (k=0;k<=K;k++) {
			dS_array[J][k] = dS_array[0][k];
		}
	}
		
	if (PeriodicFlagX2) {
		for (j=0; j<=J; j++) {
			dS_array[j][K] = dS_array[j][0];
		}
	}
}

void Grid::init_dlPrime(void)
{
	int j,k;
//	Scalar dlPrime1, dlPrime2;
	switch(geometryFlag)
		{
		case ZXGEOM: 
			for (j=0; j<=J; j++) {
				for (k=0; k<=K; k++) {
					dlPrime_array[j][k] = Vector3(0.5*(((j==J) ? X[j][k].e1() : X[j+1][k].e1()) 
																						 - ((j==0) ? X[j][k].e1() : X[j-1][k].e1())),
																				0.5*(((k==K) ? X[j][k].e2() : X[j][k+1].e2()) 
																						 - ((k==0) ? X[j][k].e2() : X[j][k-1].e2())),
																				1.0);
				}
			}

			break;
		case ZRGEOM:
		default: 
			for (j=0; j<=J; j++) {
				for (k=0; k<=K; k++) {
					dlPrime_array[j][k] = Vector3(0.5*(((j==J) ? X[j][k].e1() : X[j+1][k].e1()) 
																						 - ((j==0) ? X[j][k].e1() : X[j-1][k].e1())),
																				0.5*(((k==K) ? X[j][k].e2() : X[j][k+1].e2()) 
																						 - ((k==0) ? X[j][k].e2() : X[j][k-1].e2())),
																				(k==K) ? 2*M_PI*X[j][k].e2() : M_PI*(X[j][k+1].e2() + X[j][k].e2()));
				}
			}
			break;
		}

	if (PeriodicFlagX1) {
		for (k=0;k<=K;k++){
			dlPrime_array[0][k].set_e1(dlPrime_array[0][k].e1()+dlPrime_array[J][k].e1());
			dlPrime_array[J][k].set_e1(dlPrime_array[0][k].e1());
		}
	}

	if (PeriodicFlagX2) {
		for (j=0; j<=J; j++){
			dlPrime_array[j][0].set_e2(dlPrime_array[j][0].e2()+dlPrime_array[j][K].e2());
			dlPrime_array[j][K].set_e2(dlPrime_array[j][0].e2());
		}
	}
}

void Grid::init_dSPrime(void)
{
	int j,k;
	// all geometry switches are handled by dlPrime
	switch(geometryFlag)
		{
		case ZXGEOM:
			for (j=0; j<=J; j++) {
				for (k=0; k<=K; k++) {
					dSPrime_array[j][k] = Vector3(dlPrime_array[j][k].e2()*dlPrime_array[j][k].e3(),
																				dlPrime_array[j][k].e1()*dlPrime_array[j][k].e3(),
																				dlPrime_array[j][k].e1()*dlPrime_array[j][k].e2());
				}
			}
			break;
		case ZRGEOM:
			for (j=0; j<=J; j++) {
				for (k=0; k<=K; k++) {
					dSPrime_array[j][k] = Vector3((k==K) ? M_PI*(sqr(X[j][k].e2())-sqr(.5*(X[j][k].e2() + X[j][k-1].e2()))) : 
																				(k==0) ? M_PI*(sqr(.5*(X[j][k+1].e2() + X[j][k].e2()))-sqr(.5*(X[j][k].e2()))) : 
																				M_PI*(sqr(.5*(X[j][k+1].e2() + X[j][k].e2()))-sqr(.5*(X[j][k].e2() + X[j][k-1].e2()))),
																				dlPrime_array[j][k].e1()*dlPrime_array[j][k].e3(),
																				dlPrime_array[j][k].e1()*dlPrime_array[j][k].e2());
				}
			}
			break;
		}
// Periodic handled by dlPrime_array
}


#ifdef MPI_VERSION

extern int MPI_RANK;
int Grid::getGlobalIndex(int j,int k, GRIDDIR dir) {
  // first compute the index of the j,k node
  int ROW = MPI_RANK/M, COLUMN=MPI_RANK%M;
  int rejstart = MPIgstarts[COLUMN][ROW];
  int result = rejstart + j + k*(J+1);
  int test;

  switch(dir) {
  case HERE: 
	 {
	 	return result;
	 }
  case UP:  // lesser k, i.e., above.
	 {
		test = result - (J+1);
		if(test >= rejstart) return test;
		// OK, the point above us is outside this region.
		// if we're in the top row, return an error condition
		if(ROW==0) return -1;
		return (int)( MPIgstarts[COLUMN][ROW -1] + 
		  (MPInodeDim[COLUMN][ROW-1].e1()+1)*(MPInodeDim[COLUMN][ROW-1].e2()-1) + j);
	 }
  case RIGHT:  //greater j, i.e., right.
	  {
		  if(j<J) return result+1;  // it's inside the system
		  if(M-COLUMN == 1) return -1;  // it's outside the simulation!!
		  return (int)(MPIgstarts[COLUMN+1][ROW] + k * (MPInodeDim[COLUMN+1][ROW].e1()+1) +1);
	  }
  case DOWN: // greater k, i.e., down.
	  {
		  if(k<K) return result + (J+1);
	  if(N-ROW == 1) return -1;  //it's outside the simulation!!
		  return (int)(MPIgstarts[COLUMN][ROW+1] + (MPInodeDim[COLUMN][ROW+1].e1()+1) +j);
	  }
  case LEFT: //lesser j, i.e., left.
	  {
		  if(j>0) return result -1;
		  if(COLUMN==0) return -1;
		  return (int)(MPIgstarts[COLUMN-1][ROW] + (k+1)*(MPInodeDim[COLUMN-1][ROW].e1()+1)-2);
	  }
  case UPWRAP:  // wrapping around.
	  {
		  return (int)(MPIgstarts[COLUMN][N-1] + 
			  (MPInodeDim[COLUMN][N-1].e1()+1)*(MPInodeDim[COLUMN][N-1].e2()-1) +j);
	  }
  case RIGHTWRAP: // wrapping around
	  {
		 return (int)(MPIgstarts[0][ROW] + k * (MPInodeDim[0][ROW].e1()+1) +1);
	 } 
  case DOWNWRAP: // wrapping around
	  {
		return (int)(MPIgstarts[COLUMN][0] + (MPInodeDim[COLUMN][0].e1()+1) +j);  
	}
  case LEFTWRAP: // wrapping around.
	  {
		  return (int)(MPIgstarts[M-1][ROW] + (k+1)*(MPInodeDim[M-1][ROW].e1()+1)-2);
	  }
  default:
  return result;
  }
}

#endif //MPI_VERSION
