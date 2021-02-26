/*
====================================================================

portgauss.h

   Class representing a boundary on which a wave type boundary
   condition is applied.

====================================================================
*/

#ifndef	__PORTGAUSS_H
#define	__PORTGAUSS_H

#include "ovector.h"
#include "misc.h"
#include "particle.h"
#include "boundary.h"


class PortGauss : public Boundary
{
 protected:
    Grid* grid;
    Scalar rforEr, Er, intEr, A0, A1, a;
    Scalar EnergyOut, HonBoundary;
    int K, EFFlag, shift;
    Scalar time, tOld, dt;
    Scalar spotSize_p0, waveLeng_p0, amp_p0, focus_p0, raylLeng_p0; 
    Scalar spotSize_p1, waveLeng_p1, amp_p1, focus_p1, raylLeng_p1;
    Scalar wz_p0, Rz_p0, phig_p0, kg_p0, wz2_p0, zp2_p0, zp02_p0, zb_p0;
    Scalar wz_p1, Rz_p1, phig_p1, kg_p1, wz2_p1, zp2_p1, zp02_p1, zb_p1;
    Scalar tdelay_p0, tdelay_p1, pulLeng_p0, pulLeng_p1;
    int pulShp_p0, pulShp_p1;
    Scalar omeg_p0, omeg_p1, omgCh_p0, omgCh_p1, kCh_p0, kCh_p1;
    Scalar chirp_p0, chirp_p1;
    Scalar offset; /* offset from having peak at x2max/2 */


 public:
    PortGauss(oopicList <LineSegment> *segments, int _EFFlag,
    Scalar _spotSize_p0, Scalar _waveLeng_p0, Scalar _focus_p0,
    Scalar _amp_p0, Scalar _chirp_p0, int _pulShp_p0, Scalar _tdelay_p0, 
    Scalar _pulLeng_p0, Scalar _spotSize_p1, Scalar _waveLeng_p1, 
    Scalar _focus_p1, Scalar _amp_p1, Scalar _chirp_p1, int _pulShp_p1,
    Scalar _tdelay_p1, Scalar _pulLeng_p1, Scalar _offset=0.0);
    virtual void applyFields(Scalar t, Scalar dt);
    virtual ParticleList& emit(Scalar t, Scalar dt, Species *species);
    virtual void setPassives();
};

#endif	//	ifndef __PORTGAUSS_H
