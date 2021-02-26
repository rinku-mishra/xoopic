#include "sptlrgn.h"
#include "iregion.h"
#include "oopiclist.h"
#include "misc.h"
#include "fields.h"
#include "ostring.h"

#include <cstdio>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


using namespace std;

CurrentRegion::CurrentRegion(oopicList <LineSegment> *segments, 
			     Scalar _I0, int _direction, 
			     const ostring &_analyticF,
           const ostring &_CurrentFile) /* throw(Oops) */: Port(segments){ // maybe this should be Dielectric

#ifndef UNIX
  analyticF=_analyticF ;
  ostring endLine="\n";
  analyticF=_analyticF + endLine;
#else
  analyticF=_analyticF + '\n';
#endif
 	CurrentFile = _CurrentFile;
	I0 = _I0;
	direction =_direction;
	init = TRUE;
	BoundaryType = CURRENT_REGION;
}

void CurrentRegion::putCharge_and_Current(Scalar t)
  /* throw(Oops) */{
  int j,k;
	if (init){
	  
		grid = fields->get_grid();
		If = fields->getI();
		J = grid->getJ();
		K = grid->getK();
		grided_current = new Scalar *[J+1]; //current = J.dS
		for (j=0; j<=J; j++)
			grided_current[j] = new Scalar[K + 1];
		for(j=0;j<=J;j++)
			for(int k=0;k<=K;k++) 
				grided_current[j][k]=0;
		init = FALSE;
		if(analyticF==(ostring)"0.0\n") 
			init_default();
    else if (analyticF==(ostring)"file\n"){
      try{
			  init_file_load();
      }
      catch(Oops& oops){
        oops.prepend("CurrentRegion::putCharge_and_Current: Error: \n");//SpatialRegion::particleAdvance
        throw oops;
      }
    }
		else 
			init_evaluator();
	}
	Scalar temp=0;
	// Loop over the loop setting the current
	for (j=MIN(j1,j2); j<=MAX(j1,j2);j++)
	  for(k=MIN(k1,k2); k<=MAX(k1,k2);k++)
	  {
	    temp = get_time_value(t)*grided_current[j][k];
	    if (direction==1)
	      If[j][k]+=Vector3(temp,0,0);
	    else if (direction==2)
	      If[j][k]+=Vector3(0,temp,0);
	    else
	      If[j][k]+=Vector3(0,0,temp);
	  }
}

void CurrentRegion::init_file_load() 
  /* throw(Oops) */{
	FILE *openfile;
	int status;
	int j,k;
	char* test;
	Scalar scale=0; 
	Scalar total_current=0;
	
	test = CurrentFile.c_str();
	
	if ((openfile = fopen (test, "r"))  != NULL)
		{
			// read in
			Scalar dum1, dum2; // dummies, remove later
			cout << "  " << endl;
			cout << "started reading current file " << test << endl;
			for (j=0; j<=J; j++)
				for(k=0; k<=K;k++)
					{
#ifdef SCALAR_DOUBLE
						status = fscanf (openfile, "%lg %lg %lg", &dum1, &dum2, &grided_current[j][k]);
#else
						status = fscanf (openfile, "%g %g %g", &dum1, &dum2, &grided_current[j][k]);
#endif
						total_current += grided_current[j][k];
					}
			cout << "finished reading current file " << test << endl;
			fclose (openfile);

			scale = I0/total_current;
			for (j=0; j<=J; j++)
				for(k=0; k<=K;k++)
					grided_current[j][k] *= scale;
		}

	else
		{
      stringstream ss (stringstream::in | stringstream::out);
      ss<< "CurrentRegion::init_file_load: Error: \n"<<
			"Current file not valid." << endl;
			
      std::string msg;
      ss >> msg;
      Oops oops(msg);
      throw oops;    // exit() CurrentRegion::putCharge_and_Current
		}
}

void CurrentRegion::init_evaluator()
{
	int j,k;
	Scalar total_current=0;
	Scalar scale;
	Vector2 x;
	for (j=MIN(j1,j2); j<=MAX(j2,j1); j++)
	  for (k=MIN(k1,k2); k<=MAX(k2,k1); k++)
	  { 
	    x = grid->getMKS(j,k);
	    adv_eval->add_variable("x1",x.e1());
	    adv_eval->add_variable("x2",x.e2());
	    grided_current[j][k] = adv_eval->Evaluate(analyticF.c_str());
	    total_current +=grided_current[j][k];
	  }

	scale = I0/total_current;
	for (j=MIN(j1,j2); j<=MAX(j1,j2);j++)
		for(k=MIN(k1,k2); k<=MAX(k1,k2);k++)
			grided_current[j][k] *= scale;	
}

void CurrentRegion::init_default()
{

	int j,k;
	Scalar total_current=0;
	Scalar scale;
	total_current = (j2-j1)*(k2-k1);

	scale = I0/total_current;
	for (j=MIN(j1,j2); j<=MAX(j1,j2);j++)
		for(k=MIN(k1,k2); k<=MAX(k1,k2);k++)
			grided_current[j][k] = scale;	
}
