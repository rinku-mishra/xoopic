/*
 * boundary.cpp
 * 
 * 0.99  (NTG, JohnV, 12-28-93) Separated into individual module from pic.h.
 * 0.991  (JohnV 08-21-95) 
 * 1.001  (JohnV 05-06-96) Added toNodes() for all boundaries.
 * 1.002  (JohnV 05-09-96) In toNodes() use ngp for normal fields at edges.
 * 1.003  (PeterM 8-12-96) In the constructor, ordered the points left-to-right
 * 1.004  (JohnV 11-05-96) Constructor sets points = NULL, oblique() depends on this.
 * 1.1    (JohnV 01-31-00) Modifications to suppport space time functions for emitters.
 */

// System includes
#include <cassert>

#include "ptclgrp.h"
#include "boundary.h"
#include "fields.h"
#include "gpdist.h"
#include "mapNGDs.h"

#include "dump.h"

//--------------------------------------------------------------------
//	Set fields pointer upon attaching to a SpatialRegion.


Boundary::Boundary( oopicList<LineSegment> *_segments)
{
	segments = new oopicList<LineSegment>;
	*segments = *_segments;  //the advisor deletes *_segments.  We need a copy.
	j1 = (int)segments->headData()->A.e1();
	k1 = (int)segments->headData()->A.e2();
	j2 = (int)segments->headData()->B.e1();
	k2 = (int)segments->headData()->B.e2();
	points = segments->headData()->points;
	normal = segments->headData()->normal;
	fill = 0;
	fields = NULL;
	ptrMapNGDs = 0;
	xt_depend = NULL;
	EnergyFlux = NULL;
	Particle_diagnostic = NULL;
	BoundaryType = NOTHING;
}

void Boundary::setFields(Fields& f)
{
	fields = &f;
}

//
//--------------------------------------------------------------------
//	Collect particles.

void Boundary::collect(Particle& p, Vector3& dxMKS)
{
	//	consider switching to a ParticleGroup format to conserve memory

	if(Particle_diagnostic) Particle_diagnostic->record_particle(p,alongx2());

}

Boundary::~Boundary() 
{
	if (segments) {
		segments->deleteAll();
		segments=NULL;
	}
	if (Particle_diagnostic) {
		delete Particle_diagnostic;
		Particle_diagnostic = NULL;
	}
	if (xt_depend) {
		delete xt_depend;
		xt_depend = NULL;
	}

	// probably should delete particleoopicList contents here, if any
}

//---------------------------------------------------------------------
//	setBoundaryMask sets the boundary masks in Grid for determing boundaries.
//	Note that this version assume a line segment boundary, grid-aligned.
// Volumetric boundaries must overload this function.

void Boundary::setBoundaryMask(Grid &grid)
{	
	Grid* LocalGrid;

	minK=grid.getK(),maxK=0,minJ=grid.getJ(),maxJ=0;

	Vector2** X;
	int j,k;
	X = new Vector2*[minJ+1];
	for (j=0; j<=minJ; j++){
		X[j] = new Vector2[minK+1];
		for (k=0; k<=minK; k++)
			X[j][k] = grid.getMKS(j,k);
	}

	LocalGrid = new Grid(grid.getJ(),grid.getK(), X, grid.query_geometry(),
			grid.getPeriodicFlagX1(),
			grid.getPeriodicFlagX2());

	oopicListIter <LineSegment> lsI(*segments);
	for(lsI.restart();!lsI.Done();lsI++) {
		int j1,j2,k1,k2,normal,*points,h_nor,v_nor;  //local copies of above
		j1=(int)lsI.current()->A.e1();
		k1=(int)lsI.current()->A.e2();
		j2=(int)lsI.current()->B.e1();
		k2=(int)lsI.current()->B.e2();

		int temp;
		if (minK > (temp=MIN(k1,k2)))
			minK=temp;
		if (minJ > (temp=MIN(j1,j2)))
			minJ=temp;
		if (maxK < (temp=MAX(k1,k2)))
			maxK= temp;
		if (maxJ < (temp=MAX(j1,j2)))
			maxJ=temp;

		normal=lsI.current()->normal;
		h_nor = lsI.current()->h_nor;
		v_nor = lsI.current()->v_nor;
		points=lsI.current()->points;

		if (j1==j2)	for (k=k1; k<k2; k++) {
			grid.setBoundaryMask2(j1, k, this, v_nor);
			LocalGrid->setBoundaryMask2(j1, k, this, v_nor);
		}
		else if(k1==k2) for (j=j1; j<j2; j++) {
			grid.setBoundaryMask1(j, k1, this, h_nor);
			LocalGrid->setBoundaryMask1(j, k1, this, h_nor);
		}
		else // oblique boundary
		{
			int j,k, jl,kl,jh,kh;
			/* start with the second point, advance one point at a time */
			for(j=2;j<4*abs(j2-j1)+4;j+=2) {
				jl=points[j-2];
				kl=points[j-1];
				jh=points[j];
				kh=points[j+1];
				if(kh==kl && jl!=jh){ /*horizontal segment*/
					grid.setBoundaryMask1(jl,kl,this, h_nor);
					LocalGrid->setBoundaryMask1(jl,kl,this, h_nor);
				}
				else
					for(k=MIN(kl,kh);k<MAX(kl,kh);k++){
						grid.setBoundaryMask2(jl,k,this, v_nor);
						LocalGrid->setBoundaryMask2(jl,k,this, v_nor);
					}
			}
		}
	}

	int inside = 0;
	if (this->fill){
		for (j=minJ; j<maxJ; j++){
			inside = 0; // starting outside filled region
			for (k=minK; k<maxK; k++){
				if (LocalGrid->getNorm1()[j][k]<0)
					inside = 1;
				else if (LocalGrid->getNorm1()[j][k]>0)
					inside = 0;
				if (inside) {
					grid.SetNodeBoundary(this, j, k);
					grid.setCellMask(j,k,this);
				}
			}
		}
	}
	delete LocalGrid;
}

#ifdef HAVE_HDF5
int Boundary::dumpH5(dumpHDF5 &dumpObj, int number)
{

	//cerr << "Entered Boundary::Dump--HDF5.\n";
	hid_t fileId, groupId; //datasetId,dataspaceId;
	herr_t status;
	string outFile;
	hsize_t rank;
	//  hsize_t     dimsf[1];
	hsize_t dims;
	hid_t scalarType = dumpObj.getHDF5ScalarType();
	hid_t  attrdataspaceId,attributeId;

	//  strstream s;

	stringstream s;


	s <<  "boundary" << number <<ends;
	string bname = s.str();

	//  // Open the hdf5 file with write accessx
	fileId = H5Fopen(dumpObj.getDumpFileName().c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
	bname = "/Boundaries/" + bname;
	groupId = H5Gcreate(fileId,bname.c_str() ,0);


	string datasetName = "dims";

	rank = 1;
	dims = 4;
	attrdataspaceId = H5Screate_simple(rank, &dims, NULL);

	attributeId = H5Acreate(groupId, datasetName.c_str(), scalarType, attrdataspaceId,
			H5P_DEFAULT);
	// get the data
	Grid *grid = fields->get_grid();
	// Write the physical dimensions of the boundary

	Scalar xw[4];  // in this format:  xs, ys, xf, yf
	Vector2 **X=grid->getX();
	xw[0] = X[j1][k1].e1();
	xw[1] = X[j1][k1].e2();
	xw[2] = X[j2][k2].e1();
	xw[3] = X[j2][k2].e2();

	status = H5Awrite(attributeId, scalarType, xw);
	status = H5Aclose(attributeId);
	status = H5Sclose(attrdataspaceId);

	// dump boundary type as attribute
	dims = 1;
	attrdataspaceId = H5Screate_simple(1, &dims, NULL);
	attributeId = H5Acreate(groupId, "boundaryType",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
	status = H5Awrite(attributeId, H5T_NATIVE_INT, &BoundaryType);
	status = H5Aclose(attributeId);
	status = H5Sclose(attrdataspaceId);

	// dump n as attribute
	dims = 1;
	int n = 0;
	attrdataspaceId = H5Screate_simple(1, &dims, NULL);
	attributeId = H5Acreate(groupId, "nQuads",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
	status = H5Awrite(attributeId, H5T_NATIVE_INT,&n );
	status = H5Aclose(attributeId);
	status = H5Sclose(attrdataspaceId);

	H5Gclose(groupId);
	H5Fclose(fileId);

	//cerr << "exiting boundary::dumpH5.\n";
	return(TRUE);
}
#endif

int Boundary::Dump(FILE* DMPFile)
{
	//#ifdef UNIX
	Grid *grid = fields->get_grid();
	// Write the physical dimensions of the boundary
	{

		Scalar xw[4];  // in this format:  xs, ys, xf, yf
		Vector2 **X=grid->getX();
		xw[0] = X[j1][k1].e1();
		xw[1] = X[j1][k1].e2();
		xw[2] = X[j2][k2].e1();
		xw[3] = X[j2][k2].e2();
		XGWrite(xw,ScalarInt,4,DMPFile,ScalarChar);
	}
	// write the BoundaryType
	XGWrite(&BoundaryType, 4, 1, DMPFile, "int");

	// Write the min. dummy set of stuff
	int n=0; // number of floating point quads to restore
	XGWrite(&n,4,1,DMPFile,"int");

	// these two ints are presently unused
	// but they are expected by restore:
	XGWrite(&n,4,1,DMPFile,"int");
	XGWrite(&n,4,1,DMPFile,"int");


	//#endif
	return(TRUE);
}


// In practice, this should never be called
// but it'll read in data anwyay, and do nothing with it.
int Boundary::Restore(FILE *DMPFile, int _BoundaryType,
		Scalar _A1,Scalar _A2, Scalar _B1, Scalar _B2,int nQuads) {


	int i;
	if(BoundaryType !=  _BoundaryType) return FALSE;
	if(!onMe(_A1,_A2,_B1,_B2)) return FALSE;
	for(i=0;i<nQuads;i++) {
		Scalar x_in[4];
		XGRead(x_in,ScalarInt,4,DMPFile,ScalarChar);
	}

	return(TRUE);
}

int Boundary::Restore_2_00(FILE *DMPFile, int j1,int k1,int j2,int k2) {



	return(TRUE);
}

Scalar* Boundary::getEF()
{
	return(EnergyFlux);
}

Scalar Boundary::getBoltzmannFlux()
{
	return(-1);
}
//-------------------------------------------------------------------
// Interpolates fields to the nodal locations.  Could be more accurate
// but slower if it used the Q in the edge cells to obtain E normal.

void Boundary::toNodes()
{
	oopicListIter <LineSegment> lsI(*segments);
	for(lsI.restart();!lsI.Done();lsI++) {
		int j1,j2,k1,k2,normal,*points;  //local copies of above
		j1=(int)lsI()->A.e1();
		k1=(int)lsI()->A.e2();
		j2=(int)lsI()->B.e1();
		k2=(int)lsI()->B.e2();
		normal = lsI()->normal;
		points=lsI()->points;

		int j, k;
		Scalar w1m, w1p, w2m, w2p;
		Vector3** ENode = fields->getENode();
		Vector3** BNode = fields->getBNode();
		Vector3** intEdl = fields->getIntEdl();
		Vector3** intBdS = fields->getIntBdS();
		Grid* grid = fields->get_grid();
		if(j1==j2 && k1==k2) return;  // don't do toNodes on one-point boundaries.
		if (j1==j2)
		{
			int jin, jinHalf;
			if (normal == 1)
			{
				jin = j1 + 1;
				jinHalf = j1;
			}
			else jinHalf = jin = j1 - 1;

			for (j=j1, k=k1; k<=k2; k++)
			{
				ENode[j][k].set_e1(intEdl[jinHalf][k].e1()/grid->dl1(jinHalf, k));
				/*			ENode[j][k].set_e1(2*intEdl[jinHalf][k].e1()/grid->dl1(jinHalf, k)
					- ENode[jin][k].e1());
				 */
				if (k == grid->getK())			// fix B3 at outside edge
				{
					BNode[j][k].set_e3(intBdS[jinHalf][k-1].e3()/grid->dS3(jinHalf, k-1));
				}
				else if (k > 0)
				{
					w2m = grid->dl2(j, k)/(grid->dl2(j, k-1) + grid->dl2(j, k));
					w2p = 1 - w2m;
					ENode[j][k].set_e2(w2p*intEdl[j][k].e2()/grid->dl2(j, k)
							+ w2m*intEdl[j][k-1].e2()/grid->dl2(j, k-1));
					BNode[j][k].set_e1(intBdS[j][k].e1()*w2p/grid->dS1(j, k)
							+ intBdS[j][k-1].e1()*w2m/grid->dS1(j, k-1));
					//			 BNode[j][k].set_e3(2*(intBdS[jinHalf][k].e3()*w2p/grid->dS3(jinHalf, k)
					//			  + intBdS[jinHalf][k-1].e3()*w2m/grid->dS3(jinHalf, k-1)) - BNode[jin][k].e3());
					// BNode[jin][k] might be a boundary node
					BNode[j][k].set_e3(intBdS[jinHalf][k].e3()*w2p/grid->dS3(jinHalf, k)
							+ intBdS[jinHalf][k-1].e3()*w2m/grid->dS3(jinHalf, k-1));
				}
				if (k > 0 || !grid->axis())
				{
					ENode[j][k].set_e3(intEdl[j][k].e3()/grid->dl3(j,k));
					//	 BNode[j][k].set_e2(2*intBdS[jinHalf][k].e2()/grid->dS2(jinHalf, k)
					//						  - BNode[jin][k].e2());
					BNode[j][k].set_e2(intBdS[jinHalf][k].e2()/grid->dS2(jinHalf, k));
				}
			}
		}
		else if (k1==k2)
		{
			int kin, kinHalf;
			if (normal == 1)
			{
				kin = k1 + 1;
				kinHalf = k1;
			}
			else kin = kinHalf = k1 - 1;

			for (j=j1, k=k1; j<=j2; j++)
			{
				if (j > 0 && j < grid->getJ())
				{
					w1m = grid->dl1(j, k)/(grid->dl1(j-1, k) + grid->dl1(j, k));
					w1p = 1 - w1m;
					ENode[j][k].set_e1(w1p*intEdl[j][k].e1()/grid->dl1(j, k)
							+ w1m*intEdl[j-1][k].e1()/grid->dl1(j-1, k));
					if (k > 0 || !grid->axis())
						BNode[j][k].set_e2(intBdS[j][k].e2()*(w1p/grid->dS2(j, k))
								+ intBdS[j-1][k].e2()*w1m/grid->dS2(j-1, k));
					BNode[j][k].set_e3(2*(intBdS[j][kinHalf].e3()*w1p/grid->dS3(j, kinHalf)
							+ intBdS[j-1][kinHalf].e3()*w1m/grid->dS3(j-1, kinHalf)) - BNode[j][kin].e3());
				}
				ENode[j][k].set_e2(intEdl[j][kinHalf].e2()/grid->dl2(j, kinHalf));
				/*			ENode[j][k].set_e2(2*intEdl[j][kinHalf].e2()/grid->dl2(j, kinHalf)
					- ENode[j][kin].e2());
				 */
				if (k > 0 || !grid->axis()) ENode[j][k].set_e3(intEdl[j][k].e3()/grid->dl3(j, k));
				BNode[j][k].set_e1(2*intBdS[j][kinHalf].e1()/grid->dS1(j, kinHalf)
						- BNode[j][kin].e1());
			}
		}
	}

}


// return 1 if the two points are on the line determined by this boundary, 
//to within a reasonable
// tolerance.  return 0 otherwise.
// Also returns 0 if the neither point is actually on the segment
// determined by A1, A2, B1, B2
int Boundary::onMe(Scalar tA1,Scalar tA2,Scalar tB1,Scalar tB2) {
	Grid *grid = fields->get_grid();
	Scalar A1 = grid->getX()[j1][k1].e1();
	Scalar A2 = grid->getX()[j1][k1].e2();
	Scalar B1 = grid->getX()[j2][k2].e1();
	Scalar B2 = grid->getX()[j2][k2].e2();

	// First check to see if it can overlap the current boundary
	if(MIN(tA1,tB1) > MAX(A1,B1))  // there is no X1 overlap
		return 0;
	if(MAX(tA1,tB1) < MIN(A1,B1))  // there is no X1 overlap
		return 0;
	if(MIN(tA2,tB2) > MAX(A2,B2))  // there is no X2 overlap
		return 0;
	if(MAX(tA2,tB2) < MIN(A2,B2))  // there is no X2 overlap
		return 0;
	// pick a minimum delta:  if the boundary read in has distances less
	// than delta, assume it's the same boundary. 
	Scalar delta = 0.001*MIN(-grid->getX()[0][0].e2() + grid->getX()[0][1].e2(),
			-grid->getX()[0][0].e1() + grid->getX()[1][0].e1());

	Scalar Dist1 = LineDist(A1,A2,B1,B2,tA1,tA2);
	Scalar Dist2 = LineDist(A1,A2,B1,B2,tB1,tB2);

	// if both distances are less than Delta, it's in this boundary.
	if(Dist1 < delta && Dist2 < delta) { return 1;}
	else return 0;
}

void Boundary::recvStripe(int dir){

	Vector3 **intEdl = fields->getIntEdl();
	Vector3 **intBdS = fields->getIntBdS();
	Vector3 **I = fields->getI();

	switch(dir){

	// Fields coming from right (right moving window)
	case 1:{
		assert(j1 == j2);
		// On KC's authority, one always has k1 <= k2
		for(int k=k1; k<=k2; k++) {
			intEdl[j1-1][k].set_e1(0.);
			intEdl[j1][k].set_e2(0.);
			intEdl[j1][k].set_e3(0.);
			intBdS[j1][k].set_e1(0.);
			intBdS[j1-1][k].set_e2(0.);
			intBdS[j1-1][k].set_e3(0.);
			I[j1-1][k].set_e1(0.);
			I[j1][k].set_e2(0.);
			I[j1][k].set_e3(0.);
		}
	}
	break;

	// Fields coming from left (left moving window)
	case 2:{
		assert(j1 == j2);
		// On KC's authority, one always has k1 <= k2
		for(int k=k1; k<=k2; k++) {
			intEdl[j1][k] = 0.;
			intBdS[j1][k] = 0.;
			I[j1][k] = 0.;
		}
	}
	break;

	// Fields coming from bottom (from greater k, downward moving window)
	case 3:{
		assert(k1 == k2);
		// On KC's authority, one always has k1 <= k2
		for(int j=j1; j<=j2; j++) {
			intEdl[j][k1].set_e1(0.);
			intEdl[j][k1-1].set_e2(0.);
			intEdl[j][k1].set_e3(0.);
			intBdS[j][k1-1].set_e1(0.);
			intBdS[j][k1].set_e2(0.);
			intBdS[j][k1-1].set_e3(0.);
			I[j][k1].set_e1(0.);
			I[j][k1-1].set_e2(0.);
			I[j][k1].set_e3(0.);
		}
	}
	break;

	// Fields coming from top (from smaller k, upward moving window)
	case 4:{
		assert(k1 == k2);
		// On KC's authority, one always has k1 <= k2
		for(int j=j1; j<=j2; j++) {
			intEdl[j][k1] = 0.;
			intBdS[j][k1] = 0.;
			I[j][k1] = 0.;
		}
	}
	break;

	}

}

