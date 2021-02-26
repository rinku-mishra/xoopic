/*
	 ====================================================================

	 probergn.cpp

	 1.01	(KC, 9-21-95) After release. First draft.
	 1.02	(JohnV 03-05-96) Changed SpatialRegion* -> PlasmaDevice* for PC
	 version to build plots.
     1.03	(JK, 2013-08-27) Added writing diagnostics to file

	 ====================================================================
 */

#include "newdiag.h"
#include "diagn.h"
#include "probergn.h"
#include "boundary.h"
#include "grid.h"
#include "globalvars.h"
#include "diag2file.h"

//#ifndef UNIX
//#include "scaliter.h"
//#include "guiconfg.h"
//#endif

extern "C++" void printf(char *);


extern "C" {
#ifdef NOX
#include <xgmini.h>
#else
#include <xgdefs.h>
#endif	//NOX
}


//--------------------------------------------------------------------

ProbeRegion::ProbeRegion(SpatialRegion* _region, int j1, int k1, int j2, int k2,
		int nfft, int HistMax, int Ave, int Comb, ostring VarName, ostring x1Label,
		ostring x2Label, ostring x3Label, ostring title, int save, int open_init, struct write_params write_data)
: Diag(_region, j1, k1, j2, k2, nfft, HistMax, Ave, Comb, VarName, x1Label,
		x2Label, x3Label, title, save, open_init, write_data)
{
	sizeK = k2-k1+1;
	sizeJ = j2-j1+1;
	int j,k;

	Grid* grid = region->get_grid();

	if ((sizeK == 1)||(sizeJ == 1))
		std::cout << " You should use GridProbe " << endl;

	x_array = new Scalar[sizeJ];
	y_array = new Scalar[sizeK];

	for (k=0; k<sizeK; k++)
		y_array[k] = region->getMKS(j1,k1+k).e2();

	for (j=0; j<sizeJ; j++)
		x_array[j] = region->getMKS(j1+j,k1).e1();

	EVarPtr = NULL;
	VarPtr = NULL;

	if (VarName == (ostring)"JdotE"){
		printf("You Chose Varname = JdotE ");

		EVarPtr = new Vector3**[sizeJ];
		IVarPtr = new Vector3**[sizeJ];
		AVarPtr = new Vector3*[sizeJ];

		for (j=0; j<sizeJ; j++){
			EVarPtr[j] = new Vector3*[sizeK];
			IVarPtr[j] = new Vector3*[sizeK];
			AVarPtr[j] = new Vector3[sizeK];
		}

		for (j=0; j<sizeJ; j++)
			for ( k=0; k<sizeK; k++)
			{
				EVarPtr[j][k] = &region->getENode()[j1+j][k1+k];
				IVarPtr[j][k] = &region->getI()[j1+j][k1+k];
				AVarPtr[j][k] = grid->dSPrime(j1+j,k1+k);
			}
		history = new JE_Region_History(sizeJ,sizeK, Ave);
	}
	else
	{
		VarPtr = new Scalar**[sizeJ];
		for (j=0; j<sizeJ; j++)
			VarPtr[j] = new Scalar*[sizeK];

		if (VarName ==(ostring) "E1")
		{
			printf("You Chose Varname = E1 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(region->getENode()[j1+j][k1+k])))+0);
		}
		else if(VarName == (ostring)"E2")
		{
			printf("You Chose Varname = E2 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(region->getENode()[j1+j][k1+k])))+1);
		}
		else if(VarName == (ostring)"E3")
		{
			printf("You Chose Varname = E3 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(region->getENode()[j1+j][k1+k])))+2);
		}
		else if (VarName == (ostring)"intEdl1")
		{
			printf("You Chose Varname = intEdl1 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(region->getIntEdl()[j1+j][k1+k])))+0);
		}
		else if(VarName == (ostring)"intEdl2")
		{
			printf("You Chose Varname = intEdl2 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(region->getIntEdl()[j1+j][k1+k])))+1);
		}
		else if(VarName ==(ostring) "intEdl3")
		{
			printf("You Chose Varname = intEdl3 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(region->getIntEdl()[j1+j][k1+k])))+2);
		}
		else if(VarName ==(ostring) "B1")
		{
			printf("You Chose Varname = B1 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(region->getBNode()[j1+j][k1+k])))+0);
		}
		else if(VarName == (ostring)"B2")
		{
			printf("You Chose Varname = B2 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(region->getBNode()[j1+j][k1+k])))+1);
		}
		else if(VarName ==(ostring) "B3")
		{
			printf("You Chose Varname = B3 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(region->getBNode()[j1+j][k1+k])))+2);
		}
		else if(VarName == (ostring)"I1")
		{
			printf("You Chose Varname = I1 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(region->getI()[j1+j][k1+k])))+0);
		}
		else if(VarName == (ostring)"I2")
		{
			printf("You Chose Varname = I2 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(region->getI()[j1+j][k1+k])))+1);
		}
		else if(VarName == (ostring)"I3")
		{
			printf("You Chose Varname = I3 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(region->getI()[j1+j][k1+k])))+2);
		}
		else if(VarName == (ostring)"rho")
		{
			printf("You Chose Varname = rho ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(region->getRho()[j1+j][k1+k]))));
		}
		else if(VarName ==(ostring) "phi")
		{
			printf("You Chose Varname = phi ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(region->getPhi()[j1+j][k1+k]))));
		}
		else if(VarName == (ostring)"poynting1")
		{
			printf("You Chose Varname = poynting 1 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(theDiagnostics->S_array[j1+j][k1+k])))+0);
		}
		else if(VarName == (ostring)"poynting2")
		{
			printf("You Chose Varname = poynting 2 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(theDiagnostics->S_array[j1+j][k1+k])))+1);
		}
		else if(VarName == (ostring)"poynting3")
		{
			printf("You Chose Varname = poynting 3 ");
			for (j=0; j<sizeJ; j++)
				for (k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(theDiagnostics->S_array[j1+j][k1+k])))+2);
		}

		else
			VarPtr[0][0] = NULL;

		history = new Region_History(sizeJ,sizeK,Ave);
	}
}

ProbeRegion::~ProbeRegion()
{
	for (int i = 0; i<sizeJ;i++)
	{
		delete[] EVarPtr[i];
		delete[] IVarPtr[i];
		delete[] AVarPtr[i];
	}
	delete[] EVarPtr;
	delete[] IVarPtr;
	delete[] AVarPtr;
}

// added "n", JK 2013-07-29
void ProbeRegion::MaintainDiag(Scalar t, int n)
{
	if (EVarPtr != NULL)
		history->add(EVarPtr,IVarPtr,AVarPtr,t);
	else if (VarPtr[0][0]==NULL)
		Chose_VarPtr();
	else if (VarPtr != NULL)
		history->add(VarPtr,t);

}

void ProbeRegion::initwin(ostring prepend)
{
	// JK, 2013-08-27
	defineParameters(x1Label, x2Label, x_array,y_array,history->get_region_data(), &sizeJ, &sizeK);
#ifndef NOX
	XGSet3D( "linlinlin",x1Label.c_str(),x2Label.c_str(),strdup((prepend + title).c_str()),
			45.0,225.0,win_init_stat,1,1,1.0,1.0,1.0,1,1,1,0,1,0,1,0,1);
	XGSurf(x_array,y_array,history->get_region_data(), &sizeJ, &sizeK, 1 );
#endif
}

void ProbeRegion::initwin(GUIConfig* theGUI, int itemNumber)
{
	/*
//#ifndef UNIX
	int J = region->getJ();
	int K = region->getK();
	char* plotTag = "probeRegion";
	//	theGUI->DefinePlot("3d Plots", "3d", x1Label.c_str(), x2Label.c_str(), title.c_str(),
	//		linear, linear, linear, TRUE, TRUE, TRUE,	0, 1, 0, 1, 0, 1);

	ScalarIter* x1Iter =  new FieldScalarIter(region->getX(), J+1, K+1, 1);
	ScalarIter* x2Iter =  new FieldScalarIter(region->getX(), J+1, K+1, 2);

	ScalarIter* dataIter =  new FieldScalarIter(history->get_region_data(), J+1, K+1);

	theGUI->SetPlotData("Field Plot", "Field", plotTag, x1Label.c_str(),
											x2Label.c_str(), title.c_str(), x1Iter, x2Iter, dataIter, mesh, ltBlue);
	//	theGUI->setQSItem("Field Plot", itemNumber,  "JdotE", "Field",
	//		plotTag, title.c_str(), plotTag, "z", plotTag, "r");
//#endif
	 */
}
/*
int ProbeRegion::Dump(FILE* DMPFile)
{
#ifndef NOX
#ifdef UNIX
	XGWrite(&j1, 4, 1, DMPFile, "int");
	XGWrite(&k1, 4, 1, DMPFile, "int");
	XGWrite(&j2, 4, 1, DMPFile, "int");
	XGWrite(&k2, 4, 1, DMPFile, "int");
	if (save)
		history->dump(DMPFile);
#endif
#endif
	return(TRUE);
}

int ProbeRegion::Restore(FILE* DMPFile, int j1test, int k1test, int j2test, int k2test)
{
	if ((j1 == j1test)&&(k1 == k1test)&&(j2 == j2test)&&(k2 == k2test))
		{
			//Diags is the same get your information from fill
			if (save)
				history->restore(DMPFile);
			return(TRUE);
		}
	else
		return(FALSE);
}
 */

void ProbeRegion::Chose_VarPtr()
{
	for(int isp=0;isp<number_of_species;isp++)
		if (VarName == (ostring)theDiagnostics->theSpecies[isp].name)
		{
			std::cout << "You Chose Varname = " << theDiagnostics->theSpecies[isp].name << endl;
			for (int j=0; j<sizeJ; j++)
				for (int k=0; k<sizeK; k++)
					VarPtr[j][k] = (((Scalar*)(&(theDiagnostics->rho_species[isp][j1+j][k1+k]))));
		}
}
