#include "diags.h"
#include "sptlrgn.h"
#include "ptclgrp.h"
#include <precision.h>
///\dad{begin}
#include "spbound.h"
#include "diags.h"
///\dad{end}
Scalar erf(Scalar x);
Scalar erfc(Scalar x);

DiagnosticParams::DiagnosticParams(GridParams* _GP)
	  : ParameterGroup()
{
	j1.setNameAndDescription(ostring("j1"),
		ostring("x1 Index for a boundary endpoint"));
	k1.setNameAndDescription(ostring("k1"),
		ostring("x2 Index for a boundary endpoint"));
	parameterList.add(&k1);
	parameterList.add(&j1);
}

void DiagnosticParams::checkRules()
{
	ParameterGroup::checkRules();
}
#ifndef UNIX
/**
void DiagnosticParams::setClickPoint(Point clickPoint)
{
//	j1.setNumericalValue(clickPoint.x / 2);
//	k1.setNumericalValue(clickPoint.y / 2);
	char	buffer[18]; // itoa returns up to 17 characters
	j1.setValue(itoa(clickPoint.x / 2, buffer, 10));
	k1.setValue(itoa(clickPoint.y / 2, buffer, 10));
} 

 // query function for GUI

Segment DiagnosticParams::getSegment()
{
	return Segment(Point(j1.getValue(), k1.getValue()),
	Point(j1.getValue(), k1.getValue()));
}
void DiagnosticParams::setSegment(Segment& s)
{
	char buff[200];
	itoa(s.getj1(), buff, 10);
	j1.setValue(buff);
	itoa(s.getk1(), buff, 10);
	k1.setValue(buff);
}

EFieldProbe::EFieldProbe(GridParams* _GP) : DiagnosticParams(_GP)
{
	component.setNameAndDescription(ostring("component"),
		  ostring("direction index of E field"));
//	component.setNumericalValue(1);
	component.setValue("1");
	parameterList.add(&component);
}

IntParameter& EFieldProbe::getcomponent()
	{ return component; }

void EFieldProbe::createProbeHistory(GUIConfig* theGUI,
	SpatialRegion* sr)
{
	char buff[200];
	char buf2[200];
	Vector3** ENode = sr->getENode();
	int j = j1.getValue();
	int k = k1.getValue();
	int comp = component.getValue() - 1;
	Scalar* probeRef = ((Scalar*)&(ENode[j][k])) + comp;

//	sprintf(buff, "E%d[%d,%d] [V/m]", comp + 1, j, k);
	sprintf(buff, "E%d[%d,%d]", comp + 1, j, k);
	sprintf(buf2, "%s vs time", buff);
	theGUI->createProbeHistory(*probeRef, "History", "efield", buff);
}

void BFieldProbe::createProbeHistory(GUIConfig* theGUI,
	SpatialRegion* sr)
{
	char buff[200];
	char buf2[200];
	Vector3** BNode = sr->getBNode();
	int j = j1.getValue();
	int k = k1.getValue();
	int comp = component.getValue() - 1;
	Scalar* probeRef = ((Scalar*)&(BNode[j][k])) + comp;

//	sprintf(buff, "B%d[%d,%d] [Tesla]", comp + 1, j, k);
	sprintf(buff, "B%d[%d,%d]", comp + 1, j, k);
	sprintf(buf2, "%s vs time", buff);
	theGUI->createProbeHistory(*probeRef, "History", "bfield", buff);
}

void CurrentProbe::createProbeHistory(GUIConfig* theGUI,
	SpatialRegion* sr)
{
	char buff[200];
	char buf2[200];
	Vector3** I = sr->getI();
	int j = j1.getValue();
	int k = k1.getValue();
	int comp = component.getValue() - 1;
	Scalar* probeRef = ((Scalar*)&(I[j][k])) + comp;

//	sprintf(buff, "I%d[%d,%d] [amps]", comp + 1, j, k);
	sprintf(buff, "I%d[%d,%d]", comp + 1, j, k);
	sprintf(buf2, "%s vs time", buff);
	theGUI->createProbeHistory(*probeRef, "History", "current", buff);
}


void RhoProbe::createProbeHistory(GUIConfig* theGUI,
	SpatialRegion* sr)
{
	char buff[200];
	char buf2[200];
	Vector3** rho = sr->getrho();
	int j = j1.getValue();
	int k = k1.getValue();
	int comp = component.getValue() - 1;
	Scalar* probeRef = ((Scalar*)&(rho[j][k])) + comp;

	sprintf(buff, "rho%d[%d,%d]", comp + 1, j, k);
	sprintf(buf2, "%s vs time", buff);
	theGUI->createProbeHistory(*probeRef, "History", buf2, buff);
}


void DivDerrorProbe::createProbeHistory(GUIConfig* theGUI,
	SpatialRegion* sr)
{
	char buff[200];
	char buf2[200];
	Scalar** div = sr->getDivDerror();
	int j = j1.getValue();
	int k = k1.getValue();
	Scalar* probeRef = &(div[j][k]);

	sprintf(buff, "DivDerror[%d,%d]", j, k);
	sprintf(buff, "DivDerror[%d,%d]", j, k);
	sprintf(buf2, "%s vs time", buff);
	theGUI->createProbeHistory(*probeRef, "History", buf2, buff);
}

*/
#endif

