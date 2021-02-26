/*
====================================================================

	 ILOOP.CPP

	 0.90	(PeterM 4-16-96) Creation.

====================================================================
*/

#include "iloop.h"
#include "misc.h"
#include "fields.h"

#ifdef ICPCRAP
void Iloop::putCharge_and_Current(Scalar t) {
	int j,k;
	// first calculate the current right at this instant.
	Scalar Inow = I*get_time_value(t);
	Vector3 **If = fields->getI();

	// Loop over the loop setting the current
	for(k=k1;k<k2;k++)
		for(j=j1;j<j2;j++)
			If[j][k]+=Vector3(0,0,Inow);

}
#else
void Iloop::putCharge_and_Current(Scalar t) {
	int j,k;
	// first calculate the current right at this instant.
	Scalar Inow =I * get_time_value(t);
	Vector3 **If = fields->getI();

	// Loop over the loop setting the current
	for(k=k1;k<k2;k++)
		{ 
			If[j1][k]+=Vector3(0,Inow,0);
			If[j2][k]+=Vector3(0,-Inow,0);
		}
	for(j=j1;j<j2;j++)
		{
			If[j][k1]+=Vector3(-Inow,0,0);
			If[j][k2]+=Vector3(Inow,0,0);
		}
}
#endif
