//eportg.cpp

#include "eportg.h"
#include "exitport.h"
#include "dielectr.h"

//=================== ExitPortParams Class
ExitPortParams::ExitPortParams(ControlParams* _CP, GridParams* _GP,SpatialRegionGroup *srg) 
	: PortParams(_GP,srg)
{
	CP = _CP;
	name = "ExitPort";

	R.setNameAndDescription("R","resistance looking out (ohms)");
	R.setValue(ostring("376.99"));
	parameterList.add(&R);

	L.setNameAndDescription("L","inductance looking out (H)");
	L.setValue("0");
	parameterList.add(&L);

	Cap.setNameAndDescription("Cap","capacitance looking out (C)");
	Cap.setValue("0");
	parameterList.add(&Cap);

	Rin.setNameAndDescription("Rin","resistance looking in (ohms)");
	Rin.setValue(ostring("376.99"));
	parameterList.add(&Rin);

	Lin.setNameAndDescription("Lin","inductance looking in (H)");
	Lin.setValue("0");
	parameterList.add(&Lin);

	Capin.setNameAndDescription("Capin","capacitance looking in (C)");
	Capin.setValue("0");
	parameterList.add(&Capin);	
}
// Exit port boundary
Boundary* ExitPortParams::CreateCounterPart()
{
	Boundary* B;
	if(!((*CP).getElectrostaticFlag()))
		B = new ExitPort(SP.GetLineSegments(),
							  R.getValue(), L.getValue(), Cap.getValue(), 
							  Rin.getValue(), Lin.getValue(), Capin.getValue(), 
							  EFFlag.getValue());
	else
		B = new Dielectric(SP.GetLineSegments(), 
								1,0);
 return B;}
