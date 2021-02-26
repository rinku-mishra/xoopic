//====================================================================

#ifndef	__IREGION_H
#define	__IREGION_H

#include "ovector.h"
#include "misc.h"
#include "eval.h"
#include "port.h"
#include <oops.h>


//--------------------------------------------------------------------
//	

class CurrentRegion:public Port
{
protected:
	Grid* grid;
	Scalar **grided_current;
	Vector3 **If;
	Scalar I0;
	int direction;
	BOOL    init;
	ostring analyticF;
	ostring CurrentFile;
	int J,K;

	void init_evaluator();
	void init_default();
	void init_file_load() /* throw(Oops) */;

public:
	CurrentRegion(oopicList <LineSegment> *segments, 
					  Scalar I0, int _direction, 
					  const ostring &_analyticF,
					  const ostring &_CurrentFile) /* throw(Oops) */;
	virtual void putCharge_and_Current(Scalar t) /* throw(Oops) */;
	virtual void setPassives() {};
	virtual void applyFields(Scalar t,Scalar dt) {};
	virtual void setBoundaryMask(Grid &grid){};
//	virtual ParticleList&	emit(Scalar t,Scalar dt);
//	virtual ParticleList&	emit(Scalar t,Scalar dt) {return ;};
};
#endif  // ifndef __IREGION_H
