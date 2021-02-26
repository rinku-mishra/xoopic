/*
====================================================================

PML.H

====================================================================
*/

#ifndef __PMLPORT_H
#define __PMLPORT_H

#include "port.h"

class PMLPort : public Port
{
	int thickness;
	Scalar sigma_max;

	Vector2* oldE;

	//subcomponent1
	Vector3** H_field_1;
	Vector3** E_field_1;
	//subcomponent2
	Vector3** H_field_2;
	Vector3** E_field_2;

	Vector3** IntBdS;
	Vector3** IntEdl;
	Vector3** iL;

	int jmin, jmax;
	int kmin, kmax;
public:
	PMLPort(oopicList <LineSegment>*segments, Scalar _sigma, int _thickness);

	~PMLPort();
	virtual void applyFields(Scalar t,Scalar dt);
	virtual void setPassives() {};
};

#endif 
