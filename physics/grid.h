//====================================================================
/*

   GRID.H

   Purpose:        Member function definitions for the Grid class.  This 
   class provides the basis functions for the geometry of 
   the region and translations between physical and grid 
   coordinates.  Grid coordinates consist of pairs 
   (j+w1, k+w2), where j, k are grid indices and w1, w2 
   are fractions of the jth, kth cell.  Note that in non 
   uniform grids, the mapping is not linear between the 
   coordinates.

   Version:        $Id: grid.h 2081 2003-12-04 20:47:14Z bruhwile $

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
   0.970	(JohnV 05-18-94) Upgrade getMKS() to handle positions on right and
   upper boundaries.
   0.971	(JohnV 11-16-94) Add default for grid type in ::Grid().
   0.972	(JohnV 11-20-94) Aesthetics, move inlines to bottom.
   2.02  (JohnV 03-04-98) Added binomialFilter() to smooth arrays in place.

 */
//====================================================================

#ifndef	__GRID_H
#define	__GRID_H


#include  <ovector.h>
#include  <boundary.h>

//	Flags for determining which geometry to use
#ifndef ZRGEOM
#define ZRGEOM 0
#define ZXGEOM 1
#endif

#ifdef MPI_VERSION
#define 	getVecLocalIndex(j,k) (j + k*(J+1))
#endif /* MPI_VERSION*/

// HERE means j,k UP means lower index k, LEFT means lower index j, etc.
enum GRIDDIR { HERE, UP, RIGHT, DOWN, LEFT, UPWRAP, RIGHTWRAP, DOWNWRAP, LEFTWRAP};

class ostring;
class Periodic;

class Grid
{
protected:
	BOOL	axial;     //	TRUE if r=0 included
	int geometryFlag;
	BOOL PeriodicFlagX1, PeriodicFlagX2;
	int	J, K;	   //	nodes in x1 and x2 directions
	Vector2** X;	   //	node position in MKS units
	Boundary*** bc1; //	bc for cell side 1 (x2=const)
	Boundary*** bc2; //	bc for cell side 2 (x1=const)
	Boundary*** NodeBoundary;           // bc for node j,k
	Boundary*** CellMask;
	int** norm1;                      // keeps the normals for boundaries
	int** norm2;

	Scalar** halfCellVolumes;		// volume of a cell around a node
	void initHalfCellVolumes();
	Scalar** cellVolume_array;	// replacing inline cellVolume with an array
	void init_cellVolume();
	Vector3** dl_array;                   // replacing inline dl with an array
	void init_dl();
	Vector3** dS_array;                   // replacing inline dS with an array
	void init_dS();
	Vector3** dSPrime_array;               // replacing inline dSPrime with an array
	void init_dSPrime();
	Vector3** dlPrime_array;               // replacing inline dSPrime with an array
	void init_dlPrime();
	Scalar radial0; // correction for current and charge density on axis;
	Scalar radialK; // correction for current and charge density on outer radius

#ifdef COOPERD
	Scalar idl1, idl2, dl1c, dl2c;
#endif
#ifdef MPI_VERSION
public:
	int M,N;              //  MxN of the partitioning
	int MaxMPIJ,MaxMPIK;  //  MaxJ and MaxK of original Grid.  needed for boundaries.
	Scalar MaxMPIx1, MaxMPIx2;
	Scalar MinMPIx1, MinMPIx2;
	int neighbor[4];    //  MPI_RANK's of the 4 neighbors (right, left, up, down)
	Vector2 **MPInodeDim;  //  MPI dimensions of various regions.
	int **MPIgstarts;  //starting global index number for top-left corner of this region
	int getGlobalIndex(int j, int k,GRIDDIR dir);  //see enum GRIDDIR
#endif /* MPI_VERSION */
public:
	Grid(int J, int K, Vector2** X, int geometry=ZRGEOM, BOOL PeriodicFlagX1=FALSE,
			BOOL PeriodicFlagX2=FALSE);
	~Grid();

	void binomialFilter(Scalar **array, int n); // apply filter n times to array
	void	setBoundaryMask(BoundaryList& boundaryList);
	void SetNodeBoundary(Boundary *B,int j,int k);
	Boundary ***GetNodeBoundary() { return NodeBoundary; };
	Boundary ***GetBC1() {return bc1;};
	Boundary ***GetBC2() {return bc2;};
	Boundary ***GetCellMask() {return CellMask;};
	int	getJ() {return J;};
	int	getK() {return K;};
	int **getNorm1() {return norm1;};
	int **getNorm2() {return norm2;};
	int getPeriodicFlagX1() {return PeriodicFlagX1;};
	int getPeriodicFlagX2() {return PeriodicFlagX2;};
	inline Vector2	getMKS(const Vector2& x);			//	MKS from grid coords
	Vector2	getMKS(int j, int k) {return X[j][k];};	//	MKS from grid indices
	Vector2	getGridCoords(const Vector2& xMKS);	//	grid coords from MKS
	Vector2	getNearestGridCoords(const Vector2& xMKS);	//	int grid coords from MKS
	Scalar get_radial0() {return radial0;};
	Scalar get_radialK() {return radialK;};
	inline Vector3	interpolateBilinear(Vector3** A,const Vector2& x);
	inline Scalar   interpolateBilinear(Scalar** A, const Vector2& x);
	inline Vector2	dX(Vector2& x);
	Scalar	dl1(int j, int k) {return dl_array[j][k].e1();};
	Scalar	dl2(int j, int k) {return dl_array[j][k].e2();};
	Scalar  dl3(int j, int k) {return dl_array[j][k].e3();};
	Vector3  dl(int j, int k) {return dl_array[j][k];};
	Vector3** dl(void) {return dl_array;};
	Scalar	dS1(int j, int k) {return dS_array[j][k].e1();};
	Scalar	dS2(int j, int k) {return dS_array[j][k].e2();};
	Scalar	dS3(int j, int k) {return dS_array[j][k].e3();};
	Vector3 dS(int j, int k) {return dS_array[j][k];};
	Vector3** dS(void) {return dS_array;};
	Scalar	dS1Prime(int j, int k) {return dSPrime_array[j][k].e1();};
	Scalar	dS2Prime(int j, int k) {return dSPrime_array[j][k].e2();};
	Scalar	dS3Prime(int j, int k) {return dSPrime_array[j][k].e3();};
	Vector3  dSPrime(int j, int k) {return dSPrime_array[j][k];};
	Vector3** dSPrime(void) {return dSPrime_array;};
	Scalar cellVolume(int j, int k) {return cellVolume_array[j][k];};
	Scalar	dl1Prime(int j, int k) {return dlPrime_array[j][k].e1();};
	Scalar	dl2Prime(int j, int k) {return dlPrime_array[j][k].e2();};
	Scalar	dl3Prime(int j, int k) {return dlPrime_array[j][k].e3();};
	Vector3  dlPrime(int j, int k) {return dlPrime_array[j][k];};
	Vector3**  dlPrime() {return dlPrime_array;};
	Scalar	**get_halfCellVolumes(void) {return halfCellVolumes;}
	Scalar        halfcellVolume(int j, int k) { return halfCellVolumes[j][k]; }
	inline Boundary*	translate(Vector2& x, Vector3& dxMKS);
	//	Boundary*	translate(Vector2& x, Vector3& dxMKS);
	BOOL	axis() {return axial;};
	inline Vector3	differentialMove(const Vector2& x0,const Vector3& v, Vector3& u,
			Scalar dt);
	Vector2**	getX() {return X;}
	void	set_geometryFlag(int geometry) {geometryFlag = geometry;};
	int	query_geometry(void) {return geometryFlag;}
	void setBoundaryMask1(int j, int k, Boundary* boundary, int h_nor)
	{
		if(bc1[j][k]==NULL){
			bc1[j][k] = boundary;
			norm1[j][k] = h_nor;
		}
		else if(bc1[j][k]->getBCType() < boundary->getBCType()){
			bc1[j][k] = boundary;
			norm1[j][k] = h_nor;
		}
		SetNodeBoundary(boundary,j,k);
		SetNodeBoundary(boundary,MIN(J,j+1),k);
	}
	void setBoundaryMask2(int j, int k, Boundary* boundary, int v_nor)
	{
		if(bc2[j][k]==NULL){
			bc2[j][k] = boundary;
			norm2[j][k] = v_nor;
		}
		else if(bc2[j][k]->getBCType() < boundary->getBCType()){
			bc2[j][k] = boundary;
			norm2[j][k] = v_nor;
		}
		else if(bc2[j][k]==boundary)
			if (norm2[j][k]!=v_nor)
				norm2[j][k] = 0;

		SetNodeBoundary(boundary,j,k);
		SetNodeBoundary(boundary,j,MIN(K,k+1));
	}
	void setCellMask(int j, int k, Boundary* boundary){CellMask[j][k] = boundary;};
	inline BOOL PlasmaRegion(int j, int k) {
		return ((CellMask[(j==J) ? J-1 : j][(k==K) ? K-1 : k]==NULL)||
				(!((CellMask[(j==J) ? J-1 : j][(k==K) ? K-1 : k]!=NULL)&&
						(CellMask[(j==0) ? 0 : j-1][(k==K) ? K-1 : k]!=NULL)&&
						(CellMask[(j==0) ? 0 : j-1][(k==0) ? 0 : k-1]!=NULL)&&
						(CellMask[(j==J) ? J-1 : j][(k==0) ? 0 : k-1]!=NULL))));};
};

//--------------------------------------------------------------------
//	Calculate delta X of specified cell in MKS units.

inline Vector2 Grid::dX(Vector2& x)
{
	int	j =(int) x.e1();
	int	k =(int) x.e2();
	return X[j+1][k+1] - X[j][k];
}

//--------------------------------------------------------------------
//	Interpolate a vector grid quantity A[][] to the position x.

inline Vector3 Grid::interpolateBilinear(Vector3** A, const Vector2& x)
{
	int	j = (int) x.e1();
	int	k = (int) x.e2();
	Scalar	w1 = x.e1() - j;
	Scalar	w2 = x.e2() - k;
	return (1-w1)*(1-w2)*A[j][k] + (1-w1)*w2*A[j][k+1] + w1*(1-w2)*A[j+1][k]
																		  + w1*w2*A[j+1][k+1];
}

//--------------------------------------------------------------------
//	Interpolate a Scalar grid quantity A[][] to the position x.

inline Scalar Grid::interpolateBilinear(Scalar** A, const Vector2& x)
{
	int	j = (int) x.e1();
	int	k = (int) x.e2();
	Scalar	w1 = x.e1() - j;
	Scalar	w2 = x.e2() - k;
	return (1-w1)*(1-w2)*A[j][k] + (1-w1)*w2*A[j][k+1] + w1*(1-w2)*A[j+1][k]
																		  + w1*w2*A[j+1][k+1];
}

//--------------------------------------------------------------------
//	Given the initial position (x0), velocity v, normalized
//	momentum u = gamma*v, and timestep dt,
// compute the differential position and return it.  The present
//	routine is for z-r-phi coords only.  All units are MKS.  A side
//	effect of this routine is the rotation of v to the new local
//	coordinate system of the particle.

inline Vector3 Grid::differentialMove(const Vector2& x0,const Vector3& v, Vector3& u,
		Scalar dt)
{
	switch(geometryFlag)
	{
	case ZXGEOM:
		return	Vector3(v.e1()*dt, v.e2()*dt, v.e3()*dt);

	case ZRGEOM:
	default:
	{
		Vector3	xf(x0.e1() + v.e1()*dt, x0.e2() + v.e2()*dt, v.e3()*dt);
		//	get new radius:
		Scalar r2 = sqrt(xf.e2()*xf.e2() + xf.e3()*xf.e3());

		//	alpha is the angle of rotation, phi2 - phi1, for this timestep
		Scalar	sinAlpha;
		Scalar	cosAlpha;
		if (r2 > 0)
		{
			sinAlpha = xf.e3()/r2;
			cosAlpha = xf.e2()/r2;
		}
		else
		{
			sinAlpha = 0;
			cosAlpha = 1;
		}

		//	rotate u into new coordinate system:
		Scalar	u2r = cosAlpha*u.e2() + sinAlpha*u.e3();
		Scalar	u2phi = -sinAlpha*u.e2() + cosAlpha*u.e3();
		u.set_e2(u2r);
		u.set_e3(u2phi);

		//	compute and return dx:
		return Vector3(xf.e1() - x0.e1(), r2 - x0.e2(), xf.e3());
	}
	}
}

//--------------------------------------------------------------------
//	Return MKS coordinates from grid coordinates.

inline Vector2 Grid::getMKS(const Vector2& x)
{
	int j = (int)x.e1();
	int k = (int)x.e2();
	Scalar w1 = x.e1() - j;
	Scalar w2 = x.e2() - k;
	if (j == J)
	{
		j--;
		w1 += 1;
	}
	if (k == K)
	{
		k--;
		w2 += 1;
	}
	return (1-w1)*(1-w2)*X[j][k] + (1-w1)*w2*X[j][k+1] + w1*(1-w2)*X[j+1][k]
																		  + w1*w2*X[j+1][k+1];
}


//--------------------------------------------------------------------
//	Translate from the location x in grid coordinates to the nearest cell
//	side intersection along the trajectory given by dxMKS in MKS units.
//	Final position is stored in grid coordinates in x.  Assumes orthogonal
//	coordinate system.

#define EDGE_DELTA 1e-6

Boundary*	Grid::translate(Vector2& x, Vector3& dxMKS)
{
#ifdef DEBUG
	int BRANCH;
#endif
	int	j = (int)x.e1();
	int	k = (int)x.e2();
	//	if (x.e1()==(Scalar)j && dxMKS.e1() < 0) j--; // which cell when on cell edge
	//	if (x.e2()==(Scalar)k && dxMKS.e2() < 0) k--; // which cell when on cell edge
	if (x.e1()==(Scalar)j && dxMKS.e1()<0. && j!=0) j--; // which cell when on cell edge
	if (x.e2()==(Scalar)k && dxMKS.e2()<0. && k!=0) k--; // which cell when on cell edge
	int j1 = j+1, k1 = k+1;
	Scalar cell1 = dl1(j,k);
	Scalar dx1 = dxMKS.e1()/cell1;// /cellSize.e1();
	Scalar cell2 = dl2(j,k);
	Scalar dx2 = dxMKS.e2()/cell2;// /cellSize.e2();
	volatile Scalar tmp1 = x.e1() + dx1; // volatile ensures 32-bit truncation
	volatile Scalar tmp2 = x.e2() + dx2;
	Scalar x1New = tmp1;
	Scalar x2New = tmp2;
	double m=0.; // need double precision to ensure proper slope for small moves
	BOOL	mInfinite = FALSE;
	if (x1New != x.e1()){ // (fabs(dx1) > 0) {
		m = ((double)dx2)/dx1;	//	optimization: move m inside tests
		if (fabs(m) > 1e5) {
			mInfinite = TRUE; // slope too large -> loss of precision
			dx1 = 0;
			dxMKS.set_e1(0);
		}
		if (fabs(m) < 1e-5) { // slope too small -> loss of precision
			m = dx2 = 0;
			dxMKS.set_e2(0);
		}
	}
	else mInfinite = TRUE;
	Scalar x2Intersect;
	Scalar x1Intersect;

	if (x1New >= j1)
	{
		if (mInfinite) x2Intersect = x2New; // handles vertical move
		else x2Intersect = m*(j1 - x.e1()) + x.e2();
		if (x2Intersect >= k1)					//	top edge
		{
			if (mInfinite) x1Intersect = j1; // handles vertical edge move
			else if (m > 0) x1Intersect = MIN(j1,(k1 - x.e2())/m + x.e1());
			else x1Intersect = MIN(j1,x1New); // handles horizontal move
			if (fabs(x1New - j1) <= EDGE_DELTA && fabs(x2New - k1) <= EDGE_DELTA) {
				dxMKS.set_e1(0);
				dxMKS.set_e2(0);
			}
			else dxMKS -= Vector3((x1Intersect - x.e1())*cell1, (k1 - x.e2())*cell2, 0);
			x.set_e1(x1Intersect);
			x.set_e2(k1);
#ifdef DEBUG
			BRANCH = 1;
#endif
			if (bc2[j1][k] && x1Intersect >= j1) return bc2[j1][k];
			else return bc1[j][k1];
		}
		else if (x2Intersect <= k)			//	bottom edge
		{
			if (mInfinite) x1Intersect = j1; // handles vertical edge move
			else if (m < 0) x1Intersect = MIN(j1, (k - x.e2())/m + x.e1());
			else x1Intersect = MIN(j1,x1New); // handles horizontal move
			if (fabs(x1New - j1) <= EDGE_DELTA && fabs(x2New - k) <= EDGE_DELTA) {
				dxMKS.set_e1(0);
				dxMKS.set_e2(0);
			}
			else dxMKS -= Vector3((x1Intersect - x.e1())*cell1, (k - x.e2())*cell2, 0);
			x.set_e1(x1Intersect);
			x.set_e2(k);
#ifdef DEBUG
			BRANCH = 2;
#endif
			if (bc2[j1][k] && x1Intersect >= j1) return bc2[j1][k];
			else return bc1[j][k];
		}
		else										//	right edge
		{
			if (fabs(x1New - j1) <= EDGE_DELTA) {
				dxMKS.set_e1(0);
				dxMKS.set_e2(0);
			}
			else dxMKS -= Vector3((j1 - x.e1())*cell1, (x2Intersect - x.e2())*cell2, 0);
			x.set_e1(j1);
			x.set_e2(x2Intersect);
#ifdef DEBUG
BRANCH = 3;
#endif
return bc2[j1][k];
		}
	}
	else if (x1New <= j)
	{
		if (mInfinite) x2Intersect = x2New; // handles vertical move
		else x2Intersect = m*(j - x.e1()) + x.e2();
		if (x2Intersect >= k1)					//	top edge
		{
			if (mInfinite) x1Intersect = j; // handles vertical edge move
			else if (m < 0) x1Intersect = MAX(j, (k1 - x.e2())/m + x.e1());
			else x1Intersect = MAX(j, x1New); // handles horizontal move
			if (fabs(x1New - j) <= EDGE_DELTA && fabs(x2New - k1) <= EDGE_DELTA) {
				dxMKS.set_e1(0);
				dxMKS.set_e2(0);
			}
			else dxMKS -= Vector3((x1Intersect - x.e1())*cell1, (k1 - x.e2())*cell2, 0);
			x.set_e1(x1Intersect);
			x.set_e2(k1);
#ifdef DEBUG
			BRANCH = 4;
#endif
			if (bc2[j][k] && x1Intersect <= j) return bc2[j][k];
			else return bc1[j][k1];
		}
		else if (x2Intersect <= k)			//	bottom edge
		{
			if (mInfinite) x1Intersect = j; // handles vertical edge move
			if (m > 0) x1Intersect = MAX(j, (k - x.e2())/m + x.e1());
			else x1Intersect = MAX(j,x1New); // handles horizontal move
			if (fabs(x1New - j) <= EDGE_DELTA && fabs(x2New - k) <= EDGE_DELTA) {
				dxMKS.set_e1(0);
				dxMKS.set_e2(0);
			}
			else dxMKS -= Vector3((x1Intersect - x.e1())*cell1, (k - x.e2())*cell2, 0);
			x.set_e1(x1Intersect);
			x.set_e2(k);
#ifdef DEBUG
			BRANCH = 5;
#endif
			if (bc2[j][k] && x1Intersect <= j) return bc2[j][k];
			else return bc1[j][k];
		}
		else										//	left edge
		{
			if (fabs(x1New - j) <= EDGE_DELTA) {
				dxMKS.set_e1(0);
				dxMKS.set_e2(0);
			}
			else dxMKS -= Vector3((j - x.e1())*cell1, (x2Intersect - x.e2())*cell2, 0);
			x.set_e1(j);
			x.set_e2(x2Intersect);
#ifdef DEBUG
BRANCH = 6;
#endif
return bc2[j][k];
		}
	}
	else
	{
		if (x2New >= k1)						//	up
		{
			if (mInfinite) x1Intersect = x.e1();	//	vertical
			else if (m > 0) x1Intersect = MIN(j1, MAX(j, (k1 - x.e2())/m + x.e1()));
			else if (m < 0) x1Intersect = MAX(x1New, (k1 - x.e2())/m + x.e1());
			else x1Intersect = x1New; // handles horizontal move (j < x1New < j1)
			if (fabs(x2New - k1) <= EDGE_DELTA) {
				dxMKS.set_e1(0);
				dxMKS.set_e2(0);
			}
			else dxMKS -= Vector3((x1Intersect - x.e1())*cell1, (k1 - x.e2())*cell2, 0);
			x.set_e1(x1Intersect);
			x.set_e2(k1);
#ifdef DEBUG
			BRANCH = 7;
#endif
			return bc1[j][k1];
		}
		else if (x2New <= k)					//	down
		{
			if (mInfinite) x1Intersect = x.e1();	//	vertical
			else if (m < 0) x1Intersect = MIN(x1New, (k - x.e2())/m + x.e1()); // right
			else if (m < 0) x1Intersect = MIN(j1, MAX(j, (k - x.e2())/m + x.e1())); // left
			else x1Intersect = x1New; // handles horizontal move (j < x1New < j1)
			if (fabs(x2New - k) <= EDGE_DELTA) {
				dxMKS.set_e1(0);
				dxMKS.set_e2(0);
			}
			else dxMKS -= Vector3((x1Intersect - x.e1())*cell1, (k - x.e2())*cell2, 0);
			x.set_e1(x1Intersect);
			x.set_e2(k);
#ifdef DEBUG
			BRANCH = 8;
#endif
			return bc1[j][k];
		}
		else										//	same cell
		{
			dxMKS.set_e1(0);
			dxMKS.set_e2(0);
			x.set_e1(x1New);
			x.set_e2(x2New);
#ifdef DEBUG
			BRANCH = 9;
#endif
			return NULL;
		}
	}
}


#endif											//	ifndef __GRID_H
