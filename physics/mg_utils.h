#ifndef _mg_utils_h_
#define _mg_utils_h_
void coarsen(intVector2& vect, intVector2& vectc,int ratio0,int ratio1)
{
	if ((ratio0)&&(ratio1))
		vectc = intVector2(((vect.e1()<0) ? -abs(vect.e1()+1)/ratio0-1 : vect.e1()/ratio0 ),
							  ((vect.e2()<0) ? -abs(vect.e2()+1)/ratio1-1 : vect.e2()/ratio1 ));
	else if (ratio0)
		vectc = intVector2(((vect.e1()<0) ? -abs(vect.e1()+1)/ratio0-1 : vect.e1()/ratio0 ),
							  (vect.e2()));
	else if (ratio1)
		vectc = intVector2((vect.e1()),
							  ((vect.e2()<0) ? -abs(vect.e2()+1)/ratio1-1 : vect.e2()/ratio1 ));
}
#endif
