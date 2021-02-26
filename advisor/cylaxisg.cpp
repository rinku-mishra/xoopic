//cylaxisg.cpp

#include "cylaxisg.h"
#ifdef _MSC_VER
#include <symmetry.h>
#endif
#include "cylindri.h"


//=================== CylindricalAxisParams Class

ostring CylindricalAxisParams::OnAxisConstraint()
{char buffer[200];
 if ((k1.getValue() == 0) && (k2.getValue() == 0))
  sprintf(buffer, "%s","");
 else
  sprintf(buffer, "cyl. axis boundary (%d, %d)- (%d, %d) is off axis",
		j1.getValue(), k1.getValue(), j2.getValue(), k2.getValue());
 return(ostring(buffer));}

void CylindricalAxisParams::checkRules()
{
 BoundaryParams::checkRules();
 ostring result = OnAxisConstraint();
 if (strlen(result.c_str()) > 0)
	 ruleMessages.add(new ostring(result));
 // now check underlying rules
}

Boundary* CylindricalAxisParams::CreateCounterPart()
{Boundary* B =
  new CylindricalAxis(SP.GetLineSegments());
 return B;}

 
