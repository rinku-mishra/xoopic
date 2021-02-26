/*
	 ====================================================================

	 LASERSPOTDIAG.CPP

	 1.01	(KC, 9-21-95) After release. First draft.
	 1.02	(JohnV, 10-27-95) def'ed out xgrafix include for PC version
     1.03	(JK, 2013-08-27) Added writing diagnostics to file

	 ====================================================================
	 */

#include "newdiag.h"
#include "diagn.h"
#include "laserspotdiag.h"
#include "boundary.h"
#include "grid.h"
#include "globalvars.h"
#include "diag2file.h"
#define True 1

extern "C++" void printf(char *);
extern "C" {
#ifdef NOX
#include <xgmini.h>
#else
#include <xgdefs.h>
#endif //NOX
}

//--------------------------------------------------------------------

LaserSpotDiagnostic::LaserSpotDiagnostic(SpatialRegion* SR, 
		      int j1, int k1, int j2, int k2, int nfft, int HistMax,
		      int Ave, int Comb, ostring VarName, ostring x1Label,
		      ostring x2Label, ostring x3Label, ostring title, int save, int open_init, struct write_params write_data)
:Diag(SR, j1, k1, j2, k2, nfft, HistMax, 
			Ave, Comb, VarName, x1Label, 
			x2Label, x3Label, title, 0, open_init, write_data)
{
  // Assume always vertical diagnostic.
  size = region->getK();

  past_measurements = new Scalar_Local_History(HistMax,1);
  past_measurements_y4Ey2 = new Scalar_Local_History(HistMax,1);
  past_measurements_y2Ey2 = new Scalar_Local_History(HistMax,1);
  past_measurements_yEy2 = new Scalar_Local_History(HistMax,1);
  past_measurements_Ey2 = new Scalar_Local_History(HistMax,1);
  history = new Scalar_Local_History(HistMax,1);

}

LaserSpotDiagnostic::~LaserSpotDiagnostic()
{
  delete past_measurements;
  delete past_measurements_y2Ey2;
  delete past_measurements_y4Ey2;
  delete past_measurements_yEy2;
  delete past_measurements_Ey2;

  delete history;
}

// added "n", JK 2013-07-29
void LaserSpotDiagnostic::MaintainDiag(Scalar t, int n)
{
  int k;
  int hist_num;
  Scalar WeightSum=0;
  Scalar Weight1Sum=0;
  Scalar Weight2Sum=0;
  
  Scalar NormSum=0;
  Scalar R=0;
  Scalar yC=region->getMKS(j1,size/2).e2();
  Vector3 **Edl = region->getIntEdl();
  Vector2 **X = region->getX();

  // Do the integral:  I could do this more accurately... 
  // This is a first-order integral algorithm.
  // Integrate Y*Y * Ey(Y) * Ey(Y) 
  // divided by Ey(Y)*Ey(Y)
  for(k=0;k<size;k++) {
    Scalar y = 0.5 * (X[j1][k].e2() + X[j1][k+1].e2()); 
    Scalar Eydl = Edl[j1][k].e2();
//    Weight1Sum += (y-yC) * sqr(Eydl);
    WeightSum += sqr(y - yC) * sqr(Eydl);
//    Weight2Sum += sqr(sqr(y-yC)) * sqr(Eydl);
    NormSum += sqr(Eydl);
  }
  
  past_measurements_y2Ey2->add(WeightSum,t);
//  past_measurements_yEy2->add(Weight1Sum,t);
//  past_measurements_y4Ey2->add(Weight2Sum,t);
  past_measurements_Ey2->add(NormSum,t);
  
  // Average the past measurements and put it into history
  NormSum = 0;
  WeightSum=0;
  if( (hist_num =  *(past_measurements_Ey2->get_hi())) > 0) {
    for(k=0;k<hist_num;k++) {
      NormSum += past_measurements_Ey2->get_data()[k];
      WeightSum += past_measurements_y2Ey2->get_data()[k];
    }
  }

  if(NormSum > 0) { 
    R = sqrt(WeightSum / NormSum);
  }

  history->add(R,t);
  
}

void LaserSpotDiagnostic::initwin(ostring prepend)
{
	// JK, 2013-08-27
	defineParameters(x1Label, prepend + " ", history->get_time_array(),history->get_data(), history->get_hi());

#ifndef NOX
  WinSet2D("linlin",x1Label.c_str()," ",strdup((prepend + title).c_str()),win_init_stat,800,200,
	  1.0,1.0,True,True,0,0,0.0,0.0);
  XGCurve(history->get_time_array(),history->get_data(), history->get_hi(),1);
#endif
}





