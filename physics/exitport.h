/*
 * EXITPORT.H
 *
 *	Class representing a boundary on which a wave type boundary
 *	condition is applied.
 *
 * 0.99 	(NTG 12-29-93) Separated into individual module from pic.h.
 * 0.991	(JohnV 01-03-93) Aesthetics, compile.
 *
 */

#ifndef	__EXITPORT_H
#define	__EXITPORT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(_MSC_VER)
#include <iomanip>
#include <iostream>
using std::ends;
#else

using namespace std;
#endif

#include "ovector.h"
#include "misc.h"
#include "particle.h"
#include "boundary.h"
#include "port.h"


class ExitPort : public Port
{
protected:
	Scalar SourceTM(int j, int k, int index);
	Scalar SourceTE(int j, int k, int index);
	Grid* grid;
	Vector3** iL;
	Vector3** IntBdS;
	Vector3** IntEdl;
	Vector3** I;
	int normal;
	int shift;
	void    initialize(Scalar time,Scalar dt);  //  set up optimization variables
	Scalar tOld;
	Scalar time;
	Scalar R,L,C;
	Scalar Rin,Lin,Cin;
	Vector2* oldHonBoundary;
	Scalar *ATM, *iplusTM, *minusTM, *idtCTM;
	Scalar *ATE, *iplusTE, *minusTE, *idtCTE;
	Scalar *IntTE, *IntTM;
	Scalar Lprime, Rprime, dt;
	Scalar EnergyOut;
	Scalar iCdt2;
	BOOL    init;

public:
	ExitPort(oopicList <LineSegment> *segments, Scalar R, Scalar L, Scalar C, 
				Scalar Rin, Scalar Lin, Scalar Cin, int EFFlag);
	~ExitPort();
#ifdef HAVE_HDF5
	virtual int dumpH5(dumpHDF5 &dumpObj,int number);
	virtual int restoreH5(dumpHDF5 &restoreObj,int bType,string whichBoundary,int nQuads);
#endif
	virtual int Restore(FILE *DMPFile, int BoundaryType,
							  Scalar A1,Scalar A2, Scalar B1, Scalar B2,int nQuads);

	virtual int Restore_2_00(FILE *DMPFile,int j1,int k1,int j2,int k2);

	virtual int Dump(FILE* DMPFile);
	virtual void	applyFields(Scalar t,Scalar dt);
//	virtual ParticleList&	emit(Scalar t,Scalar dt);
	virtual void	setPassives();
};

#endif	//	ifndef __EXITPORT_H
