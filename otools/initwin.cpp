//  This initializes the windows for diagnostics.

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "sptlrgn.h"

#include <stdlib.h>
#include "oopiclist.h"
#include "ovector.h"
#include "diagn.h"
#include "ptclgrp.h"
#include "globalvars.h"
#include "boundary.h"
#include "gpdist.h"
#include "history.h"

extern "C" {
#include <math.h>
#include <stdio.h>
//#include <malloc/malloc.h>

#ifdef NOX
#include <xgmini.h>
#else
#include <xgrafix.h>
void WinSet2D(const char *PlotType, const char *X_Label, const char *Y_Label, const char *W_Label,
		const char *State, int ulx, int uly, Scalar X_Scale, Scalar Y_Scale,
		int X_Auto_Rescale, int Y_Auto_Rescale, Scalar X_Min,
		Scalar X_Max, Scalar Y_Min, Scalar Y_Max);
#endif
}

#define True 1
#define False 0

//  These are xgrafix flags!
//  They turn on/off the copying of the diagnostics from the pic
//  structures to the arrays that xgrafix must have.


void Diagnostics::InitWin(void) {
	// initialize the U ostrings
	ostring U1, U2, U3;
	ostring U="u";
	//  We'll put the diagnostic calls to xgrafix here.

	jm = theSpace->getJ()+1;
	km = theSpace->getK()+1;
	km2 = km/2+1;

	jmNGD = theSpace->getJ();
	kmNGD = theSpace->getK();

	ostring prepend="";
	int i, isp;
	if(RegionName != (ostring)"Noname" && RegionName != (ostring)"") 
		prepend = RegionName + (ostring)": ";


	switch(theSpace->get_grid()->query_geometry())
	{
	case ZRGEOM:
	{
		X1="z";
		X2="r";
		X3="phi";
		K1="kz";
		K2="kr";
		U1=U+X1;
		U2=U+X2;
		U3=U+X3;
		E1= prepend + "Ez";
		E2= prepend + "Er";
		E3= prepend + "Ephi";
		B1= prepend + "Bz";
		B2= prepend + "Br";
		B3= prepend + "Bphi";
		I1= prepend + "Iz";
		I2= prepend + "Ir";
		I3= prepend + "Iphi";
		break;
	}
	case ZXGEOM:
	{
		X1="x";
		X2="y";
		X3="z";
		K1="kx";
		K2="ky";
		U1=U+X1;
		U2=U+X2;
		U3=U+X3;
		E1= prepend + "Ex";
		E2= prepend + "Ey";
		E3= prepend + "Ez";
		B1= prepend + "Bx";
		B2= prepend + "By";
		B3= prepend + "Bz";
		I1= prepend + "Ix";
		I2= prepend + "Iy";
		I3= prepend + "Iz";
		break;
	}
	};
	char buf1[100];
	sprintf(buf1,"u%s vs %s, all species",X1.c_str(),X1.c_str());
	U1vsX1 =   prepend + buf1;
	sprintf(buf1,"u%s vs %s, all species",X2.c_str(),X1.c_str());
	U2vsX1 =   prepend + buf1;
	sprintf(buf1,"u%s vs %s, all species",X3.c_str(),X1.c_str());
	U3vsX1 =   prepend + buf1;
	sprintf(buf1,"u%s vs %s, all species",X1.c_str(),X2.c_str());
	U1vsX2=   prepend + buf1;
	sprintf(buf1,"u%s vs %s, all species",X2.c_str(),X2.c_str());
	U2vsX2=   prepend + buf1;
	sprintf(buf1,"u%s vs %s, all species",X3.c_str(),X2.c_str());
	U3vsX2=   prepend + buf1;

	//  Set up the diagnostics
	//----------------------------------------------------------------------//
	// Set up the new diagnostics
	ostring winStatus = "closed";
	Diag* tmpDiagPt;

	DiagList *dlist = theSpace->getDiagList();
	oopicListIter<Diag> nextd(*dlist);

	for(nextd.restart();!nextd.Done(); nextd++) {
		// initialize all additional diagnostics, including DerivateiveDiags
		nextd.current()->initwin(prepend);
	}

	//----------------------------------------------------------------------//
	//set up electric fields

	winStatus = checkAddDiag2File(E1, X1, X2, x1_array, x2_array, E, &jm, &km, 0 );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,E1,45.0,225.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurfVector( x1_array,x2_array,(Scalar **)E, &jm, &km, 1 ,1,0,1,0,3,0);
#endif

	winStatus = checkAddDiag2File(E2, X1, X2, x1_array, x2_array, E, &jm, &km, 1 );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,E2,45.0,225.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurfVector( x1_array,x2_array,(Scalar **)E, &jm, &km, 1 ,1,0,1,0,3,1);
#endif

	winStatus = checkAddDiag2File(E3, X1, X2, x1_array, x2_array, E, &jm, &km, 2 );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,E3,45.0,225.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurfVector( x1_array,x2_array,(Scalar **)E, &jm, &km, 1 ,1,0,1,0,3,2);
#endif

#ifdef DEBUG_FIELDS
	winStatus = checkAddDiag2File((ostring)"intEdl1", X1, X2, x1_array, x2_array, intEdl, &jm, &km, 0 );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,"intEdl1",45.0,225.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurfVector( x1_array,x2_array,(Scalar **)intEdl, &jm, &km, 1 ,1,0,1,0,3,0);
#endif

	winStatus = checkAddDiag2File((ostring)"intEdl2", X1, X2, x1_array, x2_array, intEdl, &jm, &km, 1 );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,"intEdl2",45.0,225.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurfVector( x1_array,x2_array,(Scalar **)intEdl, &jm, &km, 1 ,1,0,1,0,3,1);
#endif

	winStatus = checkAddDiag2File((ostring)"intEdl3", X1, X2, x1_array, x2_array, intEdl, &jm, &km, 2 );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,"intEdl3",45.0,225.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurfVector( x1_array,x2_array,(Scalar **)intEdl, &jm, &km, 1 ,1,0,1,0,3,2);
#endif
#endif
	//set up magnetic fields

	winStatus = checkAddDiag2File(B1, X1, X2, x1_array, x2_array, B, &jm, &km, 0 );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,B1,45.0,225.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurfVector( x1_array,x2_array,(Scalar **)B, &jm, &km, 1,1,0,1,0,3,0 );
#endif

	winStatus = checkAddDiag2File(B2, X1, X2, x1_array, x2_array, B, &jm, &km, 1 );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,B2,45.0,225.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurfVector( x1_array,x2_array,(Scalar **)B, &jm, &km, 1,1,0,1,0,3,1 );
#endif

	winStatus = checkAddDiag2File(B3, X1, X2, x1_array, x2_array, B, &jm, &km, 2 );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,B3,45.0,225.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurfVector( x1_array,x2_array,(Scalar **)B, &jm, &km, 1,1,0,1,0,3,2 );
#endif

#ifdef DEBUG_FIELDS
	winStatus = checkAddDiag2File((ostring)"intBdS1", X1, X2, x1_array, x2_array, intBdS, &jm, &km, 0 );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,"intBdS1",45.0,225.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurfVector( x1_array,x2_array,(Scalar **)intBdS, &jm, &km, 1 ,1,0,1,0,3,0);
#endif

	winStatus = checkAddDiag2File((ostring)"intBdS2", X1, X2, x1_array, x2_array, intBdS, &jm, &km, 1 );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,"intBdS2",45.0,225.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurfVector( x1_array,x2_array,(Scalar **)intBdS, &jm, &km, 1 ,1,0,1,0,3,1);
#endif

	winStatus = checkAddDiag2File((ostring)"intBdS3", X1, X2, x1_array, x2_array, intBdS, &jm, &km, 2 );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,"intBdS3",45.0,225.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurfVector( x1_array,x2_array,(Scalar **)intBdS, &jm, &km, 1 ,1,0,1,0,3,2);
#endif
#endif

	//set up currents
	if(!electrostaticFlag) {
		winStatus = checkAddDiag2File(I1, X1, X2, x1_array, x2_array, I, &jm, &km, 0 );
#ifndef NOX
		XGSet3D( "linlinlin",X1,X2,I1,45.0,225.0,winStatus,1,1,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
		XGSurfVector( x1_array,x2_array,(Scalar **)I, &jm, &km, 1,1,0,1,0,3,0 );
#endif

		winStatus = checkAddDiag2File(I2, X1, X2, x1_array, x2_array, I, &jm, &km, 1 );
#ifndef NOX
		XGSet3D( "linlinlin",X1,X2,I2,45.0,225.0,winStatus,1,1,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
		XGSurfVector( x1_array,x2_array,(Scalar **)I, &jm, &km, 1,1,0,1,0,3,1 );
#endif

		winStatus = checkAddDiag2File(I3, X1, X2, x1_array, x2_array, I, &jm, &km, 2 );
#ifndef NOX
		XGSet3D( "linlinlin",X1,X2,I3,45.0,225.0,winStatus,1,1,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
		XGSurfVector( x1_array,x2_array,(Scalar **)I, &jm, &km, 1,1,0,1,0,3,2 );
#endif
	}
	//set up energies

	winStatus = checkAddDiag2File((prepend +"Ue"), X1, X2, x1_array, x2_array, Ue, &jm, &km );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,strdup((prepend +"Ue")),45.0,225.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurf( x1_array,x2_array,Ue, &jm, &km, 1 );
#endif

	winStatus = checkAddDiag2File((prepend +"Ub"), X1, X2, x1_array, x2_array, Ub, &jm, &km );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,strdup((prepend +"Ub")),45.0,225.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurf( x1_array,x2_array,Ub, &jm, &km, 2 );
#endif

	// Set up poynting vector
	if(!electrostaticFlag)
	{
		winStatus = checkAddDiag2File((prepend+"Poynting Vector"), X1, X2, x1_array, x2_array, S_array, &jm, &km, 1 );
#ifndef NOX
		XGSetVec("vecvec",X1,X2,strdup(prepend+"Poynting Vector"),winStatus,1,1,1.0,1.0,0,0,x1min,x1max,x2min,x2max);
		XGVectorVector(x1_array,x2_array,(Scalar **)S_array, (Scalar **)S_array,&jm,&km,2,1,0,1,0,3,0,3,1);
#endif

	}


	//  set up rho
	char buffer[100];
	if (boltzmannFlag)
		sprintf(buffer,"PIC ");
	else 
		sprintf(buffer,"%s","");

	winStatus = checkAddDiag2File((prepend + buffer + "rho"), X1, X2, x1_array, x2_array, rho, &jm, &km );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,strdup(prepend + buffer + "rho"),45.0,250.0,winStatus,1,400,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurf( x1_array,x2_array,rho, &jm, &km, 3 );
#endif

	/**
	 *  Plot the neutral gas densities, dad: 01/24/2001
	 */
	oopicListIter<NGD> NGDIter(*ptrNGDList);
	ostring tmpTitle = "";

	for ( NGDIter.restart(); !NGDIter.Done(); NGDIter++ ) {
		tmpTitle = prepend + "Neutral Gas Density for " + (NGDIter.current())->getGasType();
		winStatus = checkAddDiag2File(tmpTitle, X1, X2, x1_arrayNGD, x2_arrayNGD, (NGDIter.current())->getNGDdata(), &jmNGD, &kmNGD );
#ifndef NOX
		XGSet3D( "linlinlin",X1,X2,tmpTitle,45.0,250.0,winStatus,1,400,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
		XGSurf( x1_arrayNGD,x2_arrayNGD, (NGDIter.current())->getNGDdata(), &jmNGD, &kmNGD, 3 );
#endif
	}

	//  set up boltzmann rho
	if (boltzmannFlag) {
		winStatus = checkAddDiag2File((prepend +"Total Rho"), X1, X2, x1_array, x2_array, totalRho, &jm, &km );
#ifndef NOX
		XGSet3D( "linlinlin",X1,X2,strdup(prepend +"Total Rho"),45.0,250.0,winStatus,1,1,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
		XGSurf( x1_array,x2_array,totalRho, &jm, &km, 3 );
#endif
	}

	if(electrostaticFlag) {
		//  set up phi
		winStatus = checkAddDiag2File((prepend +"phi"), X1, X2, x1_array, x2_array, phi, &jm, &km );
#ifndef NOX
		XGSet3D( "linlinlin",X1,X2,strdup(prepend +"phi"),45.0,250.0,winStatus,1,1,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
		XGSurf( x1_array,x2_array,phi, &jm, &km, 3 );
#endif
	}

#ifdef DEBUG_PHI
	winStatus = checkAddDiag2File((prepend +"phi error"), X1, X2, x1_array, x2_array, phi_err, &jm, &km );
#ifndef NOX
	XGSet3D( "linlinlin",X1,X2,strdup(prepend +"phi error"),45.0,250.0,winStatus,1,1,
			1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	XGSurf( x1_array,x2_array,phi_err, &jm, &km, 3 );
#endif
#endif

	//  set up divderror
	if(!electrostaticFlag) {
		winStatus = checkAddDiag2File((prepend +"Divergence Error"), X1, X2, x1_array, x2_array, divderror, &jm, &km );
#ifndef NOX
		XGSet3D( "linlinlin",X1,X2,strdup(prepend +"Divergence Error"),45.0,250.0,winStatus,1,1,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
		XGSurf( x1_array,x2_array,divderror, &jm, &km, 3 );
#endif
	}

	//  The phase space diagnostics
	//	X2 vs X1
	for(isp=0;isp<number_of_species;isp++) {
		char * buf = new char[80];
		sprintf(buf,"%s-%s phase space for %s",X1.c_str(),X2.c_str(),theSpecies[isp].name);

		// check for diagnostics in input file; JK 2013-08-02
		tmpDiagPt = checkDiag2File(prepend +buf, X1, X2, 1);
		// add data if diagnostics is in input file; JK 2013-08-02
		if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(theSpecies[isp].x1positions,theSpecies[isp].x2positions,&(theSpecies[isp].nparticles_plot),0);
#ifndef NOX
		WinSet2D( "linlin", X1, X2, strdup(prepend +buf),"open",700,400,1,1,0,0,x1min,x1max,x2min,x2max);

		XGScat2D(theSpecies[isp].x1positions,theSpecies[isp].x2positions,
				&(theSpecies[isp].nparticles_plot),isp);
#endif
		delete [] buf;

#ifndef NOX
		//draw any boundaries or structures
		oopicListIter<Boundary> nextb(*blist);
		for(nextb.restart();!nextb.Done(); nextb++) {
			Scalar x1=0.;
			Scalar x2=0.;
			Scalar y1=0.;
			Scalar y2=0.;
			int j1,j2,k1,k2;

			if (nextb.current()->queryFill()){
				Boundary ***NB;
				j1=nextb.current()->getMinJ();
				j2=nextb.current()->getMaxJ();
				k1=nextb.current()->getMinK();
				k2=nextb.current()->getMaxK();
				NB=nextb.current()->getFields()->get_grid()->GetCellMask();

				int j,k;
				int on=0;
				for (j=j1; j<j2; j++){
					for (k=k1; k<k2; k++){
						if (NB[j][k]==nextb.current()){
							if (!on){
								on=1;
								x1 = x1_array[j];
								y1 = x2_array[k];
							}
						}
						else
							if (on) {
								on=0;
								int bcolor = nextb.current()->getBoundaryType();
								Scalar *fpoints = (Scalar *) calloc(32,sizeof(Scalar));
								x2 = x1_array[j+1];
								y2 = x2_array[k];
								fpoints[0]=x1;fpoints[1]=y1;fpoints[2]=x1;fpoints[3]=y2;
								fpoints[4]=x2;fpoints[5]=y2;fpoints[6]=x2;fpoints[7]=y1;
								fpoints[8]=x1;fpoints[9]=y1;
								XGStructureArray(5,FILLED,-bcolor,-bcolor,fpoints);
								free(fpoints);
							}
					}
					if (on){
						on = 0;
						Scalar *fpoints = (Scalar *) calloc(32,sizeof(Scalar));
						int bcolor = nextb.current()->getBoundaryType();
						fpoints[0]=x1;fpoints[1]=y1;fpoints[2]=x1;fpoints[3]=y2;
						fpoints[4]=x2;fpoints[5]=y2;fpoints[6]=x2;fpoints[7]=y1;
						fpoints[8]=x1;fpoints[9]=y1;
						x2 = x1_array[j+1];
						y2 = x2_array[k2];
						XGStructureArray(5,FILLED,-bcolor,-bcolor,fpoints);
						free(fpoints);
					}
				}
			}

			oopicListIter <LineSegment> lsI(*nextb.current()->getSegmentList());
			for(lsI.restart();!lsI.Done();lsI++) {
				int j1,j2,k1,k2,normal,*points;  //local copies of above
				j1=(int)lsI.current()->A.e1();
				k1=(int)lsI.current()->A.e2();
				j2=(int)lsI.current()->B.e1();
				k2=(int)lsI.current()->B.e2();
				points=lsI.current()->points;
				normal=lsI.current()->normal;

				x1 = x1_array[j1];
				x2 = x1_array[j2];
				y1 = x2_array[k1];
				y2 = x2_array[k2];
				int bcolor = nextb.current()->getBoundaryType();

				// Different sorts of boundaries need to be drawn differently
				switch(bcolor) {
				case DIELECTRIC_TRIANGLE_UP: // positive-normal DielectricTriangle
				{
					Scalar * fpoints = (Scalar *) calloc(4*abs(j2-j1+3),sizeof(Scalar));
					int j;
					for(j=0;j<4*(j2-j1)+4;j+=2)
					{ fpoints[j] = x1_array[points[j]];
					fpoints[j+1] = x2_array[points[j+1]];
					}
					fpoints[j] = (y1 < y2)? x1 :x2;
					fpoints[j+1] = MAX(y1,y2);
					fpoints[j+2]= fpoints[0];
					fpoints[j+3]= fpoints[1];
					XGStructureArray(2*(j2-j1)+4,FILLED,bcolor - 100, bcolor -100,
							fpoints);

					//				delete [] points;
					free(fpoints);

					break;
				}
				case DIELECTRIC_TRIANGLE_DOWN: //negative-normal DielectricTriangle
				{
					Scalar * fpoints = (Scalar *) calloc(4*abs(j2-j1+3),sizeof(Scalar));
					int *points = segmentate(j1,k1,j2,k2);
					int j;
					for(j=0;j<4*(j2-j1)+4;j+=2)
					{ fpoints[j] = x1_array[points[j]];
					fpoints[j+1] = x2_array[points[j+1]];
					}
					fpoints[j] = (y1 < y2)? x2 :x1;
					fpoints[j+1] = MIN(y1,y2);
					fpoints[j+2]= fpoints[0];
					fpoints[j+3]= fpoints[1];
					XGStructureArray(2*(j2-j1)+4,FILLED,-(bcolor + 100), -(bcolor +100),
							fpoints);

					delete [] points;
					free(fpoints);
					break;
				}
				case DIELECTRIC_REGION:  //filled square
				{
					Scalar *fpoints = (Scalar *) calloc(32,sizeof(Scalar));
					fpoints[0]=x1;fpoints[1]=y1;fpoints[2]=x1;fpoints[3]=y2;
					fpoints[4]=x2;fpoints[5]=y2;fpoints[6]=x2;fpoints[7]=y1;
					fpoints[8]=x1;fpoints[9]=y1;
					XGStructureArray(5,FILLED,-bcolor,-bcolor,fpoints);
					free(fpoints);
					break;
				}
				case CURRENT_REGION:  //filled square
				{
					Scalar *fpoints = (Scalar *) calloc(32,sizeof(Scalar));
					fpoints[0]=x1;fpoints[1]=y1;fpoints[2]=x1;fpoints[3]=y2;
					fpoints[4]=x2;fpoints[5]=y2;fpoints[6]=x2;fpoints[7]=y1;
					fpoints[8]=x1;fpoints[9]=y1;
					XGStructureArray(5,FILLED,-bcolor,-bcolor,fpoints);
					free(fpoints);
					break;
				}
				case ILOOP: // Hollow square
				{
					Scalar *fpoints = (Scalar *) calloc(32,sizeof(Scalar));
					fpoints[0]=x1;fpoints[1]=y1;fpoints[2]=x1;fpoints[3]=y2;
					fpoints[4]=x2;fpoints[5]=y2;fpoints[6]=x2;fpoints[7]=y1;
					fpoints[8]=x1;fpoints[9]=y1;
					XGStructureArray(5,FILLED,-bcolor,-bcolor,fpoints);
					free(fpoints);
					break;
				}
				case PERIODIC:
				{
					//do nothing
					break;
				}
				default: {   // simple or oblique line
					if(x1==x2 || y1==y2) {
						Scalar xa,ya;
						Scalar *fpoints = (Scalar *) calloc(32,sizeof(Scalar));
						xa = (x1+x2)/2.0;
						ya = (y1+y2)/2.0;
						fpoints[0]=x1;fpoints[1]=y1;
						fpoints[2]=x2;fpoints[3]=y2;
						XGStructureArray(2,HOLLOW,bcolor,bcolor,fpoints);
						fpoints[0]=xa;fpoints[1]=ya;
						fpoints[2]=((x1==x2)?xa+(normal*(x1_array[1]-x1_array[0]))/4.0 :xa);
						fpoints[3]=((y1==y2)?ya+(normal*(x2_array[1]-x2_array[0]))/4.0 :ya);
						XGStructureArray(2,HOLLOW,bcolor,bcolor,fpoints);
						free(fpoints);
					}
					else
					{
						int *points = segmentate(j1,k1,j2,k2);
						Scalar *fpoints = (Scalar *) calloc(32,sizeof(Scalar));
						int j, jl,kl,jh,kh;
						/* start with the second point, advance one point at a time */
						for(j=2;j<4*abs(j2-j1)+4;j+=2) {
							jl=points[j-2];
							kl=points[j-1];
							jh=points[j];
							kh=points[j+1];
							fpoints[0]=x1_array[jl];fpoints[1]=x2_array[kl];
							fpoints[2]=x1_array[jh];fpoints[3]=x2_array[kh];
							XGStructureArray(2,HOLLOW,bcolor,bcolor,fpoints);
						}
						delete[] points;
						free(fpoints);

					}
					break;
				}
				}
			}
		}
#endif
	}

	// 	U1 vs X1
	tmpDiagPt = checkDiag2File(U1vsX1, X1, U1, number_of_species);
#ifndef NOX
	winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
	WinSet2D( "linlin", X1,strdup(U1.c_str()), U1vsX1,winStatus,700,400,1,1,0,1,x1min,x1max,x2min,x2max);
#endif

	for(i=0;i<number_of_species;i++) {
#ifndef NOX
		XGScat2D(theSpecies[i].x1positions,theSpecies[i].x1velocities, &(theSpecies[i].nparticles_plot),i);
#endif
		if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(theSpecies[i].x1positions,theSpecies[i].x1velocities,&(theSpecies[i].nparticles_plot),i);
	}

	//	U2 vs X1
	tmpDiagPt = checkDiag2File(U2vsX1, X1, U2, number_of_species);
	winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
	WinSet2D( "linlin", X1,strdup(U2.c_str()),U2vsX1,winStatus,700,400,1,1,0,1,x1min,x1max,x2min,x2max);
#endif

	for(i=0;i<number_of_species;i++) {
#ifndef NOX
		XGScat2D(theSpecies[i].x1positions,theSpecies[i].x2velocities, &(theSpecies[i].nparticles_plot),i);
#endif
		if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(theSpecies[i].x1positions,theSpecies[i].x2velocities,&(theSpecies[i].nparticles_plot),i);
	}

	//	U3 vs X1
	tmpDiagPt = checkDiag2File(U3vsX1, X1, U3, number_of_species);
	winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
	WinSet2D( "linlin", X1,strdup(U3.c_str()),U3vsX1,winStatus,700,400,1,1,0,1,x1min,x1max,x2min,x2max);
#endif

	for(i=0;i<number_of_species;i++) {
#ifndef NOX
		XGScat2D(theSpecies[i].x1positions,theSpecies[i].x3velocities, &(theSpecies[i].nparticles_plot),i);
#endif
		if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(theSpecies[i].x1positions,theSpecies[i].x3velocities,&(theSpecies[i].nparticles_plot),i);
	}

	//	U1 vs X2
	tmpDiagPt = checkDiag2File(U1vsX2, X2, U1, number_of_species);
	winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
	WinSet2D( "linlin", X2,strdup(U1.c_str()),U1vsX2,winStatus,700,400,1,1,0,1,x2min,x2max,x2min,x2max);
#endif

	for(i=0;i<number_of_species;i++) {
#ifndef NOX
		XGScat2D(theSpecies[i].x2positions,theSpecies[i].x1velocities, &(theSpecies[i].nparticles_plot),i);
#endif
		if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(theSpecies[i].x2positions,theSpecies[i].x1velocities,&(theSpecies[i].nparticles_plot),i);
	}

	//	U2 vs X2
	tmpDiagPt = checkDiag2File(U2vsX2, X2, U2, number_of_species);
	winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
	WinSet2D( "linlin", X2,strdup(U2.c_str()),U2vsX2,winStatus,700,400,1,1,0,1,x2min,x2max,x2min,x2max);
#endif

	for(i=0;i<number_of_species;i++) {
#ifndef NOX
		XGScat2D(theSpecies[i].x2positions,theSpecies[i].x2velocities, &(theSpecies[i].nparticles_plot),i);
#endif
		if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(theSpecies[i].x2positions,theSpecies[i].x2velocities,&(theSpecies[i].nparticles_plot),i);
	}

	//	U3 vs X2
	tmpDiagPt = checkDiag2File(U3vsX2, X2, U3, number_of_species);
	winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
	WinSet2D( "linlin", X2,strdup(U3.c_str()),U3vsX2,winStatus,700,400,1,1,0,1,x2min,x2max,x2min,x2max);
#endif

	for(i=0;i<number_of_species;i++) {
#ifndef NOX
		XGScat2D(theSpecies[i].x2positions,theSpecies[i].x3velocities, &(theSpecies[i].nparticles_plot),i);
#endif
		if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(theSpecies[i].x2positions,theSpecies[i].x3velocities,&(theSpecies[i].nparticles_plot),i);
	}

	//  vector plot Er,Ez
	winStatus = checkAddDiag2File((prepend +"E"), X1, X2, x1_array, x2_array, E, &jm, &km, -1 );
#ifndef NOX
	XGSetVec("vecvec",X1,X2,strdup(prepend +"E"),winStatus,1,1,1.0,1.0,0,0,x1min,x1max,x2min,x2max);
	XGVectorVector(x1_array,x2_array,(Scalar **)E,(Scalar **)E,&jm,&km,2,1,0,1,0,3,0,3,1);
#endif

	//  vector plot Br,Bz
	winStatus = checkAddDiag2File((prepend +"B"), X1, X2, x1_array, x2_array, B, &jm, &km, -1 );
#ifndef NOX
	XGSetVec("vecvec",X1,X2,strdup(prepend +"B"),winStatus,1,1,1.0,1.0,0,0,x1min,x1max,x2min,x2max);
	XGVectorVector(x1_array,x2_array,(Scalar **)B,(Scalar **)B,&jm,&km,2,1,0,1,0,3,0,3,1);
#endif

	//  vector plot Ir,Iz
	if(!electrostaticFlag) {
		winStatus = checkAddDiag2File((prepend +"I"), X1, X2, x1_array, x2_array, I, &jm, &km, -1 );
#ifndef NOX
		XGSetVec("vecvec",X1,X2,strdup(prepend +"I"),winStatus,1,1,1.0,1.0,0,0,x1min,x1max,x2min,x2max);
		XGVectorVector(x1_array,x2_array,(Scalar **)I,(Scalar **)I,&jm,&km,2,1,0,1,0,3,0,3,1);
#endif
	}

	//  Time domain plots
	//  boundary current history diagnostics
	oopicListIter<Ihistdiag> nextdiag(*BoundaryIhist);

	for(nextdiag.restart();!nextdiag.Done();nextdiag++){
		char *buf = new char[80];
		sprintf(buf,"I history for boundary %s",nextdiag.current()->name);

		winStatus = checkAddDiag2File(prepend+ buf, "Time", "History", nextdiag.current()->Ihist->get_time_array(),nextdiag.current()->Ihist->get_data(),nextdiag.current()->Ihist->get_hi() );
#ifndef NOX
		WinSet2D("linlin","Time","History",strdup(prepend+ buf),winStatus,800,200,1.0,1.0,True,True,0,0,0.0,0.0);
		XGCurve(nextdiag.current()->Ihist->get_time_array(),nextdiag.current()->Ihist->get_data(),nextdiag.current()->Ihist->get_hi(),1);
#endif
		delete [] buf;

		// repeat for species
		char *buf2 = new char[80];
		sprintf(buf2,"I species history for boundary %s",nextdiag.current()->name);

		tmpDiagPt = checkDiag2File(prepend+ buf2, "Time", "History", number_of_species);
#ifndef NOX
		winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
		WinSet2D("linlin","Time","History",strdup(prepend+ buf2),winStatus,800,200,1.0,1.0,True,True,0,0,0.0,0.0);
#endif

		for (i=0; i<number_of_species; i++){
#ifndef NOX
			XGCurve(nextdiag.current()->Ihist_sp[i]->get_time_array(),nextdiag.current()->Ihist_sp[i]->get_data(),nextdiag.current()->Ihist_sp[i]->get_hi(),i);
#endif
			if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(nextdiag.current()->Ihist_sp[i]->get_time_array(),nextdiag.current()->Ihist_sp[i]->get_data(),nextdiag.current()->Ihist_sp[i]->get_hi(),i);
		}
		delete [] buf2;
	}

	oopicListIter<PFhistdiag> nextdiag2(*BoundaryPFhist);

	for(nextdiag2.restart();!nextdiag2.Done();nextdiag2++){
		char *buf = new char[80];
		sprintf(buf,"%sPoynting history for %s",prepend.c_str(),nextdiag2.current()->name);

		winStatus = checkAddDiag2File(buf, "Time", "Poynting History", nextdiag2.current()->PFhist->get_time_array(),nextdiag2.current()->PFhist->get_data(),nextdiag2.current()->PFhist->get_hi() );
#ifndef NOX
		WinSet2D("linlin","Time","Poynting History",strdup(buf),winStatus,800,200,1.0,1.0,True,True,0,0,0.0,0.0);
		XGCurve(nextdiag2.current()->PFhist->get_time_array(),nextdiag2.current()->PFhist->get_data(),nextdiag2.current()->PFhist->get_hi(),1);
#endif
		delete [] buf;

		char *buf2 = new char[80];
		sprintf(buf2,"%sPoynting Local history for %s",prepend.c_str(),nextdiag2.current()->name);

		winStatus = checkAddDiag2File(buf2, "Time", "Poynting Local History", nextdiag2.current()->PFlocal->get_time_array(),nextdiag2.current()->PFlocal->get_data(),nextdiag2.current()->PFlocal->get_hi() );
#ifndef NOX
		WinSet2D("linlin","Time","Poynting Local History",strdup(buf2),winStatus,800,200,1.0,1.0,True,True,0,0,0.0,0.0);
		XGCurve(nextdiag2.current()->PFlocal->get_time_array(),nextdiag2.current()->PFlocal->get_data(),nextdiag2.current()->PFlocal->get_hi(),1);
#endif
		delete [] buf2;
	}

	tmpDiagPt = checkDiag2File(prepend + "TE-beige KE-green Efield-yellow Bfield-Orange", "Time", "Poynting History", 4);
	winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
	WinSet2D("linlog", "Time","Poynting History",
			strdup(prepend + "TE-beige KE-green Efield-yellow Bfield-Orange"),
			winStatus, 800, 200, 1.0, 1.0, True, True,0,0,0.0,0.0);

	XGCurve(energy_all.get_time_array(), energy_all.get_data(), energy_all.get_hi(), 1);
	XGCurve(energy_k.get_time_array(), energy_k.get_data(), energy_k.get_hi(), 2);
	XGCurve(energy_e.get_time_array(), energy_e.get_data(), energy_e.get_hi(), 3);
	XGCurve(energy_b.get_time_array(), energy_b.get_data(), energy_b.get_hi(), 4);
#endif

	if( tmpDiagPt ) {
		tmpDiagPt->addSpeciesParameters(energy_all.get_time_array(), energy_all.get_data(), energy_all.get_hi(), 0);
		tmpDiagPt->addSpeciesParameters(energy_k.get_time_array(), energy_k.get_data(), energy_k.get_hi(), 1);
		tmpDiagPt->addSpeciesParameters(energy_e.get_time_array(), energy_e.get_data(), energy_e.get_hi(), 2);
		tmpDiagPt->addSpeciesParameters(energy_b.get_time_array(), energy_b.get_data(), energy_b.get_hi(), 3);
	}

	if(!electrostaticFlag) {
		winStatus = checkAddDiag2File(prepend + "Average magnitude of error in Div(D) at t", "Time", "Error",
										divderrorhis.get_time_array(), divderrorhis.get_data(), divderrorhis.get_hi() );
#ifndef NOX
		WinSet2D("linlin","Time","Error",strdup(prepend + "Average magnitude of error in Div(D) at t"),
				winStatus, 800, 200, 1.0, 1.0, True, True,0,0,0.0,0.0);
		XGCurve(divderrorhis.get_time_array(), divderrorhis.get_data(), divderrorhis.get_hi(),0);
#endif
	}

	tmpDiagPt = checkDiag2File(prepend + "number(t)", "Time", "number(t)", number_of_species);
	winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
	WinSet2D("linlin", "Time","number(t)",strdup(prepend + "number(t)"), winStatus, 700, 400,
			1.0, 1.0, True, True,0,0,0.0,0.0);
#endif

	for(i=0;i<number_of_species;i++) {
#ifndef NOX
		XGCurve(number[i]->get_time_array(), number[i]->get_data(),number[i]->get_hi(), i);
		//	  XGCurve(ngroups[i]->get_time_array(), ngroups[i]->get_data(),ngroups[i]->get_hi(), i);
#endif
		if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(number[i]->get_time_array(), number[i]->get_data(),number[i]->get_hi(), i);
	}

	tmpDiagPt = checkDiag2File(prepend + "Kinetic Energy(t)", "Time", "Kinetic Energy", number_of_species+1);
	winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
	WinSet2D("linlin", "Time","Kinetic Energy",strdup(prepend + "Kinetic Energy(t)"), winStatus, 700, 400,
			1.0, 1.0, True, True,0,0,0.0,0.0);
#endif

	for(i=0;i<number_of_species;i++) {
#ifndef NOX
		XGCurve(ke_species[i]->get_time_array(), ke_species[i]->get_data(),ke_species[i]->get_hi(), i);
#endif
		if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(ke_species[i]->get_time_array(), ke_species[i]->get_data(),ke_species[i]->get_hi(), i);
	}

#ifndef NOX
	XGCurve(energy_k.get_time_array(), energy_k.get_data(), energy_k.get_hi(), i+1);
#endif
	if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(energy_k.get_time_array(), energy_k.get_data(), energy_k.get_hi(), i+1);

	tmpDiagPt = checkDiag2File(prepend + "total density (t)", "Time", "total density", number_of_species);
	winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
	WinSet2D("linlin", "Time","total density",strdup(prepend + "total density (t)"), winStatus, 700, 400,
			1.0, 1.0, True, True,0,0,0.0,0.0);
#endif

	for(i=0;i<number_of_species;i++) {
#ifndef NOX
		XGCurve(total_density[i]->get_time_array(), total_density[i]->get_data(), total_density[i]->get_hi(), i);
#endif
		if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(total_density[i]->get_time_array(), total_density[i]->get_data(), total_density[i]->get_hi(), i);
	}

	/**********************************************************/
	// New time history plot for RMS beam parameters
	// Bruhwiler/Dimitrov 10/09/2000
	// search for the name/number for the species with rmsBeamSizeFlag set to 1 (default is 0)
	bool Species_Flag = false;
	oopicListIter<Species> siter(*theSpace->getSpeciesList());
	for(siter.restart();(!siter.Done()) && (!Species_Flag); siter++) {
		if( siter.current()->get_rmsDiagnosticsFlag() ) {
			char* rms_buf = new char[80];
			// Ave for Beam Size
			sprintf(rms_buf,"Averages for %s: %s-beige %s-green",
					static_cast<char*>(siter.current()->get_name()), X1.c_str(),X2.c_str());
			tmpDiagPt = checkDiag2File(prepend + rms_buf, "Time", "Average Beam Sizes", 2);
			winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
			WinSet2D("linlin", "Time", "Average Beam Sizes",strdup(prepend + rms_buf), winStatus, 700, 400,1.0, 1.0, True, True,0,0,0.0,0.0);
#endif

			for( i=0 ; i < 2 ; i++ ) {
#ifndef NOX
				XGCurve(aveBeamSize[i]->get_time_array(), aveBeamSize[i]->get_data(), aveBeamSize[i]->get_hi(), i+1);
#endif
				if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(aveBeamSize[i]->get_time_array(), aveBeamSize[i]->get_data(), aveBeamSize[i]->get_hi(), i);
			}

			//
			// RMS for Beam Size
			sprintf(rms_buf,"RMS' for %s: %s-beige %s-green",
					static_cast<char*>(siter.current()->get_name()), X1.c_str(),X2.c_str());
			tmpDiagPt = checkDiag2File(prepend + rms_buf, "Time", "RMS Beam Sizes", 2);
			winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
			WinSet2D("linlin", "Time", "RMS Beam Sizes",strdup(prepend + rms_buf), winStatus, 700, 400,1.0, 1.0, True, True,0,0,0.0,0.0);
#endif

			for( i=0 ; i < 2 ; i++ ) {
#ifndef NOX
				XGCurve(rmsBeamSize[i]->get_time_array(), rmsBeamSize[i]->get_data(), rmsBeamSize[i]->get_hi(), i+1);
#endif
				if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(rmsBeamSize[i]->get_time_array(), rmsBeamSize[i]->get_data(), rmsBeamSize[i]->get_hi(), i);
			}

			//
			// Ave for Velocities
			sprintf(rms_buf,"Ave Velocities for %s: U%s-beige U%s-green U%s-yellow",
					static_cast<char*>(siter.current()->get_name()), X1.c_str(),X2.c_str(), X3.c_str());
			tmpDiagPt = checkDiag2File(prepend + rms_buf, "Time", "Average Velocities", 3);
			winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
			WinSet2D("linlin", "Time",  "Average Velocities",strdup(prepend + rms_buf), winStatus, 700, 400, 1.0, 1.0, True, True,0,0,0.0,0.0);
#endif

			for( i=0 ; i < 3 ; i++ ) {
#ifndef NOX
				XGCurve(aveVelocity[i]->get_time_array(), aveVelocity[i]->get_data(), aveVelocity[i]->get_hi(), i+1);
#endif
				if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(aveVelocity[i]->get_time_array(), aveVelocity[i]->get_data(), aveVelocity[i]->get_hi(), i);
			}

			//
			// RMS for velocities
			sprintf(rms_buf,"Velocity RMS' for %s: U%s-beige U%s-green U%s-yellow",
					static_cast<char*>(siter.current()->get_name()), X1.c_str(),X2.c_str(), X3.c_str());
			tmpDiagPt = checkDiag2File(prepend + rms_buf, "Time", "RMS Velocities", 3);
			winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
			WinSet2D("linlin", "Time", "RMS Velocities",strdup(prepend + rms_buf), winStatus, 700, 400,1.0, 1.0, True, True,0,0,0.0,0.0);
#endif

			for( i=0 ; i < 3 ; i++ ) {
#ifndef NOX
				XGCurve(rmsVelocity[i]->get_time_array(), rmsVelocity[i]->get_data(), rmsVelocity[i]->get_hi(), i+1);
#endif
				if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(rmsVelocity[i]->get_time_array(), rmsVelocity[i]->get_data(), rmsVelocity[i]->get_hi(), i);
			}

			//
			// RMS for Emittance
			sprintf(rms_buf,"RMS Emittance for %s: E%s-beige E%s-green",
					static_cast<char*>(siter.current()->get_name()), X1.c_str(),X2.c_str());
			tmpDiagPt = checkDiag2File(prepend + rms_buf, "Time", "RMS Emittance", 2);
			winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
			WinSet2D("linlin", "Time", "RMS Emittance",strdup(prepend + rms_buf), winStatus, 700, 400,1.0, 1.0, True, True,0,0,0.0,0.0);
#endif

			for( i=0 ; i < 2 ; i++ ) {
#ifndef NOX
				XGCurve(rmsEmittance[i]->get_time_array(), rmsEmittance[i]->get_data(), rmsEmittance[i]->get_hi(), i+1);
#endif
				if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(rmsEmittance[i]->get_time_array(), rmsEmittance[i]->get_data(), rmsEmittance[i]->get_hi(), i);
			}

			// ave for Energy in eV
			/**************************************************
	sprintf(rms_buf,"Ave Energy for %s", static_cast<char*>(siter.current()->get_name()));
	WinSet2D("linlin", "Time", "Average Energy", strdup(prepend + rms_buf), "closed", 700, 400,
		1.0, 1.0, True, True,0,0,0.0,0.0);
	XGCurve(aveEnergy_eV->get_time_array(), aveEnergy_eV->get_data(), aveEnergy_eV->get_hi(), 1);
	// rms for Energy in eV
	sprintf(rms_buf,"RMS Energy for %s", static_cast<char*>(siter.current()->get_name()));
	WinSet2D("linlin", "Time", "RMS Energy", strdup(prepend + rms_buf), "closed", 700, 400,
		1.0, 1.0, True, True,0,0,0.0,0.0);
	XGCurve(rmsEnergy_eV->get_time_array(), rmsEnergy_eV->get_data(), rmsEnergy_eV->get_hi(), 1);
			 ***************************************************/
			Species_Flag = true;
			delete [] rms_buf;
		}
	}
	//end of Dimitrov/Bruhwiler changes
	/**********************************************************/

	tmpDiagPt = checkDiag2File(prepend + "Ave KE(t)", "Time", "Ave KE (eV)", number_of_species);
	winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
	WinSet2D("linlin", "Time","Ave KE (eV)", strdup(prepend + "Ave KE(t)"), winStatus, 700, 400, 1.0, 1.0, True, True,0,0,0.0,0.0);
#endif

	for(i=0;i<number_of_species;i++) {
#ifndef NOX
		XGCurve(Ave_KE[i]->get_time_array(), Ave_KE[i]->get_data(), Ave_KE[i]->get_hi(), i);
#endif
		if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(Ave_KE[i]->get_time_array(), Ave_KE[i]->get_data(), Ave_KE[i]->get_hi(), i);
	}

	//  set u the special button

	SetUpNewVar(&PhaseSpacePlots,(char *)"Phase Plots\0",(char *)ScalarChar);
	SetUpNewVar(&AllDiagnostics,(char *)"All Diags\0",(char *)ScalarChar);
	SetUpNewVar(&EnerPlots,(char *)"Energy Diags\0",(char *)ScalarChar);

	// Give us nice species diagnostics.
	for(i=0;i<number_of_species;i++) {
		char * buf = new char[80];
		sprintf(buf,"%sNumber density for %s ", (char *)(prepend),
				theSpecies[i].name);

		winStatus = checkAddDiag2File(buf, X1, X2, x1_array, x2_array, rho_species[i], &jm, &km );
#ifndef NOX
		XGSet3D("linlinlin", X1, X2, strdup(buf), 45.0, 225.0, winStatus, 1, 1,
				1.0, 1.0, 1.0/theSpecies[i].spec->get_q(), 0, 0, 1, x1min, x1max,
				x2min, x2max, 0.0, 1.0);
		XGSurf(x1_array, x2_array, rho_species[i], &jm, &km, 3 );
#endif
		delete [] buf;
	}


	for(i=0;i<number_of_species;i++) {
		if (Show_loaded_densityFlag)
		{
			char * buf = new char[80];
			sprintf(buf,"%sInitial number density for %s ",prepend.c_str(),theSpecies[i].name);

			winStatus = checkAddDiag2File(buf, X1, X2, x1_array, x2_array, loaded_density[i], &jm, &km );
#ifndef NOX
			XGSet3D( "linlinlin",X1,X2,strdup(buf),45.0,225.0,winStatus,1,1,
					1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
			XGSurf(x1_array,x2_array,loaded_density[i], &jm, &km, 3 );
#endif
			delete [] buf;
		}
	}

	/*  Boundary diagnostics */
	oopicListIter<Boundary> nextb(*blist);
	for(nextb.restart();!nextb.Done(); nextb++) {
		Boundary *B = nextb.current();
		PDistCol *PD = B->get_particle_diag();
		if (PD) if (PD->get_nxbins()>0) {
			ostring bid = B->getBoundaryName();

			char *buf1 = new char[80];
			sprintf(buf1, "nxpoints=%d for boundary %s\n", *PD->get_nxpoints(), bid.c_str());
			printf("%s", buf1);
			delete [] buf1;

			char *buf = new char[80];
			sprintf(buf,"%sf(x) on boundary %s", prepend.c_str(), bid.c_str());
			tmpDiagPt = checkDiag2File(buf, "x", "sf(x)", number_of_species);
			winStatus = (tmpDiagPt ? tmpDiagPt->getWinInitState() : "closed");
#ifndef NOX
			WinSet2D("linlin", "x", "sf(x)", strdup(buf), winStatus, 200, 200,
					1.0, 1.0, False, True, PD->get_x_MIN(), PD->get_x_MAX(), 0.0, 0.0);
#endif
			delete [] buf;

			for(i=0;i<number_of_species;i++) {
#ifndef NOX
				XGCurve(PD->get_x(), PD->get_fxdata(i), PD->get_nxpoints(), i);
#endif
				if( tmpDiagPt ) tmpDiagPt->addSpeciesParameters(PD->get_x(), PD->get_fxdata(i), PD->get_nxpoints(), i);
			}

			if (*PD->get_nepoints()) {
				for (i=0; i<number_of_species; i++){
					char *buf = new char[80];
					sprintf(buf, "%sf(x,energy) for %s on boundary %s", prepend.c_str(),
							theSpecies[i].name, B->getBoundaryName().c_str());
					winStatus = checkAddDiag2File(buf, "x", "energy", PD->get_x(), PD->get_energy(), PD->get_fedata(i), PD->get_nxpoints(), PD->get_nepoints() );
#ifndef NOX
					XGSet3D("linlinlin", "x", "energy", strdup(buf), 45, 225, winStatus, 1, 1,
							1, 1, 1, 0, 0, 1, PD->get_x_MIN(), PD->get_x_MAX(),
							PD->get_energy_MIN(), PD->get_energy_MAX(), 0, 1);
					XGSurf(PD->get_x(), PD->get_energy(), PD->get_fedata(i), PD->get_nxpoints(),
							PD->get_nepoints(), i);
#endif
					delete [] buf;
				}
			}

			if (*PD->get_ntpoints()){
				for (i=0; i<number_of_species; i++){
					char *buf = new char[80];
					sprintf(buf, "%sf(x,theta) for %s on boundary %s", prepend.c_str(),
							theSpecies[i].name, B->getBoundaryName().c_str());
					winStatus = checkAddDiag2File(buf, "x", "theta", PD->get_x(), PD->get_energy(), PD->get_ftdata(i), PD->get_nxpoints(), PD->get_nepoints() );
#ifndef NOX
					XGSet3D("linlinlin", "x", "theta", strdup(buf), 45, 225, winStatus, 1, 1,
							1, 1, 1, 0, 0, 1, PD->get_x_MIN(), PD->get_x_MAX(),
							PD->get_theta_MIN(), PD->get_theta_MAX(), 0, 1);
					XGSurf(PD->get_x(), PD->get_theta(), PD->get_ftdata(i), PD->get_nxpoints(),
							PD->get_ntpoints(), i);
#endif
					delete [] buf;
				}
			}
			// add theta and energy plots here
		}
	}

	//  Velocities and positions for all particles/species
	char buffer1[160], buffer2[160], buffer3[160];

	//	X1, X2, v1, v2, v3
	for(isp=0;isp<number_of_species;isp++) {
		// loop over all species
		sprintf(buffer1,"phase space for %s", theSpecies[isp].name);
		sprintf(buffer2,"%s %s", X1.c_str(), X2.c_str());
		sprintf(buffer3,"%s %s %s", U1.c_str(), U2.c_str(), U3.c_str());

		// check for diagnostics in input file and add data if diagnostics in input file; JK 2013-08-02
		checkAddDiag2File(prepend+buffer1, (ostring)buffer2, (ostring)buffer3, theSpecies[isp].x1positions, theSpecies[isp].x2positions,
							theSpecies[isp].x1velocities, theSpecies[isp].x2velocities, theSpecies[isp].x3velocities,
							&(theSpecies[isp].nparticles_plot), isp);
	}

}

