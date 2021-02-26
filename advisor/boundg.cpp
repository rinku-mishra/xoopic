//boundg.cpp

#include "boundg.h"
#include "boundary.h"
#include "gpdist.h"
#include "spatialg.h"
#include "grid.h"
#include "segmentg.h"
///\dad{begin}
#include "spbound.h"
#include "diags.h"
///\dad{end}

#ifdef MPI_VERSION
#include "mpi.h"
extern MPI_Comm XOOPIC_COMM;
extern int MPI_RANK;
#define SRB_ANNOUNCE_TAG 100
#define SRB_LINK_TAG 200
typedef struct {
	char *name;
	int index;
	int linkedP;
} SRBdat;
#endif


int Between(Scalar i, Scalar lo, Scalar hi)
{return ((lo <= i) && (i <= hi));}


//=================== BoundaryParams Class

BoundaryParams::BoundaryParams(GridParams* _GP, SpatialRegionGroup* srg)
: ParameterGroup()
{
	name = "Boundary";
	parameterGroupList.add(&SP);
	GP = _GP;
	spatialRegionGroup = srg;
	j1.setNameAndDescription(ostring("j1"),
			ostring("x1 Index for a boundary endpoint"));
	j1.setValue("-1");
	k1.setNameAndDescription(ostring("k1"),
			ostring("x1 Index for a boundary endpoint"));
	k1.setValue("-1");
	j2.setNameAndDescription(ostring("j2"),
			ostring("x2 Index for a boundary endpoint"));
	j2.setValue("-1");
	k2.setNameAndDescription(ostring("k2"),
			ostring("x2 Index for a boundary endpoint"));
	k2.setValue("-1");
	A1.setNameAndDescription("A1","x1 Location (MKS) of boundary endpoint");
	A1.setValue("-1");
	parameterList.add(&A1);
	A2.setNameAndDescription("A2","x2 Location (MKS) of boundary endpoint");
	A2.setValue("-1");
	parameterList.add(&A2);
	B1.setNameAndDescription("B1","x1 Location (MKS) of boundary endpoint");
	B1.setValue("-1");
	parameterList.add(&B1);
	B2.setNameAndDescription("B2","x2 Location (MKS) of boundary endpoint");
	B2.setValue("-1");
	parameterList.add(&B2);

	fill.setNameAndDescription("fill",  "if fill = 1 the boundary is filled in");
	fill.setValue("0");
	parameterList.add(&fill);

	normal.setNameAndDescription(ostring("normal"),
			ostring("Sign of normal to surface (1 or -1)"));
	normal.setValue("1");

	EFFlag.setNameAndDescription("EFFlag",  "A flag to record EM energy");
	EFFlag.setValue("0");

	IdiagFlag.setNameAndDescription("IdiagFlag","Flag turning on and off current collection for this boundary");
	IdiagFlag.setValue("0");
	parameterList.add(&IdiagFlag);

	Ihist_len.setNameAndDescription("Ihist_len","Length of the current history arrays. Default is 1024.");
	Ihist_len.setValue("1024");
	parameterList.add(&Ihist_len);

	Ihist_avg.setNameAndDescription("Ihist_avg","Number of timesteps for averaging current plots. Default is 1.");
	Ihist_avg.setValue("1");
	parameterList.add(&Ihist_avg);

	spotSize_p0.setNameAndDescription("spotSize_p0"," ");
	spotSize_p1.setNameAndDescription("spotSize_p1"," ");
	waveLeng_p0.setNameAndDescription("waveLeng_p0"," ");
	waveLeng_p1.setNameAndDescription("waveLeng_p1"," ");
	focus_p0.setNameAndDescription("focus_p0"," ");
	focus_p1.setNameAndDescription("focus_p1"," ");
	amp_p0.setNameAndDescription("amp_p0"," ");
	amp_p1.setNameAndDescription("amp_p1"," ");
	tdelay_p0.setNameAndDescription("tdelay_p0"," ");
	tdelay_p1.setNameAndDescription("tdelay_p1"," ");
	pulLeng_p0.setNameAndDescription("pulLeng_p0"," ");
	pulLeng_p1.setNameAndDescription("pulLeng_p1"," ");
	pulShp_p0.setNameAndDescription("pulShp_p0"," ");
	pulShp_p1.setNameAndDescription("pulShp_p1"," ");
	chirp_p0.setNameAndDescription("chirp_p0"," ");
	chirp_p1.setNameAndDescription("chirp_p1"," ");

	parameterList.add(&j1);
	parameterList.add(&k1);
	parameterList.add(&j2);
	parameterList.add(&k2);
	parameterList.add(&normal);
	parameterList.add(&EFFlag);
	parameterList.add(&spotSize_p0);
	parameterList.add(&spotSize_p1);
	parameterList.add(&waveLeng_p0);
	parameterList.add(&waveLeng_p1);
	parameterList.add(&focus_p0);
	parameterList.add(&focus_p1);
	parameterList.add(&amp_p0);
	parameterList.add(&amp_p1);
	parameterList.add(&tdelay_p0);
	parameterList.add(&tdelay_p1);
	parameterList.add(&pulLeng_p0);
	parameterList.add(&pulLeng_p1);
	parameterList.add(&chirp_p0);
	parameterList.add(&chirp_p1);
	parameterList.add(&pulShp_p0);
	parameterList.add(&pulShp_p1);


	nxbins.setNameAndDescription("nxbins","");
	nxbins.setValue("0");
	parameterList.add(&nxbins);

	nenergybins.setNameAndDescription("nenergybins","");
	nenergybins.setValue("0");
	parameterList.add(&nenergybins);

	nthetabins.setNameAndDescription("nthetabins", "");
	nthetabins.setValue("0");
	parameterList.add(&nthetabins);

	theta_min.setNameAndDescription("theta_min", "");
	theta_min.setValue("0");
	parameterList.add(&theta_min);

	theta_max.setNameAndDescription("theta_max", "");
	theta_max.setValue("90");
	parameterList.add(&theta_max);

	energy_min.setNameAndDescription("energy_min", "");
	energy_min.setValue("0");
	parameterList.add(&energy_min);

	energy_max.setNameAndDescription("energy_max", "");
	energy_max.setValue("100");
	parameterList.add(&energy_max);

	Name.setNameAndDescription("name","A string label for this boundary");
	Name.setValue("Noname");
	parameterList.add(&Name);

	//Time parameter stuff

	C.setNameAndDescription("C","DC value of time dependent function on the boundary");
	A.setNameAndDescription("A","AC sinusoidal value of the boundary");
	frequency.setNameAndDescription("frequency","frequency of the AC signal in Hz");
	phase.setNameAndDescription("phase","phase of the AC signal");

	trise.setNameAndDescription("trise","rise time of the envelope");
	tpulse.setNameAndDescription("tpulse","pulse time of the envelope");
	tfall.setNameAndDescription("tfall","fall time of the envelope");
	tdelay.setNameAndDescription("tdelay","delay time before the envelope");
	a0.setNameAndDescription("a0","lower amplitude of envelope");
	a1.setNameAndDescription("a1","upper amplitude of envelope");
	parameterList.add(&C);
	C.setValue("1.0");
	parameterList.add(&A);
	A.setValue("0.0");
	parameterList.add(&frequency);
	frequency.setValue("0.0");
	parameterList.add(&phase);
	phase.setValue("0.0");

	parameterList.add(&trise);
	trise.setValue("0.0");
	parameterList.add(&tpulse);
	tpulse.setValue("10000.0");
	parameterList.add(&tfall);
	tfall.setValue("0.0");
	parameterList.add(&tdelay);
	tdelay.setValue("0.0");
	parameterList.add(&a0);
	a0.setValue("1.0");
	parameterList.add(&a1);
	a1.setValue("1.0");

	xtFlag.setNameAndDescription("xtFlag","");
	xtFlag.setValue("0");
	parameterList.add(&xtFlag);

	F.setNameAndDescription("F","");
	F.setValue("100");
	parameterList.add(&F);


	bPtr = NULL;								// gets set by CreateCounterPart()
};

BoundaryParams::~BoundaryParams() {
	int j = 1;
	j+=1;
}

ostring BoundaryParams::AlignmentConstraint()
{char buffer[200];
sprintf(buffer, "%s","");
return(ostring(buffer));}

ostring BoundaryParams::OnGridConstraint() {
	char buffer[200];
	oopicList<LineSegment> *LS = SP.GetLineSegments();
	oopicListIter<LineSegment> lsiter(*LS);

	//Iterate over all the segments to make sure they're on the boundary
	for(lsiter.restart(); !lsiter.Done(); lsiter++) {
		LineSegment *S = lsiter.current();
		int j1,j2,k1,k2,J,K,norm;
		j1 = (int)S->A.e1();
		k1 = (int)S->A.e2();
		j2 = (int)S->B.e1();
		k2 = (int)S->B.e2();
		J=GP->getJ();
		K=GP->getK();
		norm=S->normal;
		ostring name;
		name=S->SegName;


		if  ((Between(j1, 0,J) && Between(k1, 0,K) &&
				Between(j2, 0,J) && Between(k2, 0,K)))
			sprintf(buffer, "%s","");
		else
		{
			sprintf(buffer, "segment in boundary %s at (%d, %d)- (%d, %d) is not contained on grid\n", Name.getValue().c_str(),
					j1,k1,j2,k2);
			break;
		}
		//  Check normal to warn about boundaries on the edge of the grid which have outward
		// normals
		if( ((j1==j2)&&j1==0 && norm ==-1) ||  ((j1==j2)&&j1==J && norm ==1)
				||((k1==k2)&&k1==0 && norm ==-1) ||  ((k1==k2)&&k1==K && norm ==1) ){
			sprintf(buffer,
					"warning: segment %s in boundary %s at (%d, %d)- (%d, %d) has normal directed outside the region\n",
					S->SegName.c_str(),
					Name.getValue().c_str(),
					j1,k1,j2,k2);
			std::cout << buffer;
		}
	}

	return(ostring(buffer));

}


BOOL BoundaryParams::HasLength()
{if ((j1.getValue() == j2.getValue()) &&
		(k1.getValue() == k2.getValue()))
	return 0;
else
	return 1;}


void BoundaryParams::checkRules()
{
	//ruleMessages.removeAll();
	ostring result = AlignmentConstraint();
	if (strlen(result.c_str()) > 0)
		ruleMessages.add(new ostring(result));
	result = OnGridConstraint();
	if (strlen(result.c_str()) > 0)
		ruleMessages.add(new ostring(result));
	// Loop thru rules
	oopicListIter<RuleBase> nR(RuleList);
	oopicListIter<BaseParameter> nP(parameterList);
	for (nR.restart(); !nR.Done(); nR++)
		// Loop thru parameters and set parameter values required by rule
	{for (nP.restart(); !nP.Done(); nP++)
		nR.current()->setRuleVariable(nP.current()->getName(),
				nP.current()->getValueAsDouble());
	// Check the rule
	ostring result = nR.current()->checkRule();
	if (strlen(result.c_str()) > 0)
		ruleMessages.add(new ostring(result));}}

//--------------------------------------------------------------------
// setBoundaryParams sets up the following:
//	1.	Boundary ID
//	2.	ParticleDiagnostic
//	3.	TimeFunction

void BoundaryParams::setBoundaryParams(Boundary* _bPtr)
{
	bPtr = _bPtr;
	bPtr->setBoundaryName(Name.getValue());
	bPtr->setFill(fill.getValue());
	bPtr->set_particle_diag(createDistributionCollector());
	bPtr->set_Ihist_len(Ihist_len.getValue());
	bPtr->set_Ihist_avg(Ihist_avg.getValue());
	SpaceTimeFunction* xtf = new SpaceTimeFunction(A.getValue(), C.getValue(),
			frequency.getValue(), phase.getValue(),
			trise.getValue(), tfall.getValue(), tpulse.getValue(),
			tdelay.getValue(), a0.getValue(),
			a1.getValue(), F.getValue(), xtFlag.getValue());
	bPtr->set_xt_function(xtf);
}

PDistCol* BoundaryParams::createDistributionCollector()
{
	// if we're not collecting either current or distribution, do not create.
	if(nxbins.getValue()==0 && IdiagFlag.getValue()==0) return NULL;
	//vstart(v1start.getValue(), v2start.getValue(), v3start.getValue());
	//vend(v1end.getValue(), v2end.getValue(), v3end.getValue());

	int i1, i2;
	if(bPtr->alongx2())
	{
		i1 = k1.getValue();
		i2 = k2.getValue();
	}
	else 
	{
		i1 = j1.getValue();
		i2 = j2.getValue();
	}
	int x_min = MIN(i1,i2);
	int x_max = MAX(i1,i2);
	int nSpecies = spatialRegionGroup->get_speciesList()->nItems();

	return new PDistCol(nxbins.getValue(), nenergybins.getValue(), nthetabins.getValue(),
			nSpecies, energy_min.getValue(), energy_max.getValue(),
			x_min, x_max,
			theta_min.getValue(), theta_max.getValue() /*,
							  spatialRegionGroup->get_speciesPtr(sname)*/);
}

// Changes boundary locations in terms of MKS into boundary locations
// in terms of grid units
void BoundaryParams::toGrid(Grid *G) /* throw(Oops) */ {

	oopicList<LineSegment> *RLS = SP.GetReadLineSegments();
	oopicList<LineSegment> *LS = SP.GetLineSegments();

	if(RLS->nItems()!=0 && (j1.getValue()!=-1 || A1.getValue()!=-1)) {
		stringstream ss (stringstream::in | stringstream::out);
		ss<<"BoundaryParams::toGrid: Error: \n"<<
				"Please don't specify both Segments and j's/k's A's/B's for the same boundary.\n"<<
				"It is ambiguous.  Boundary " << Name.getValue().c_str()<<endl;

		std::string msg;
		ss >> msg;
		Oops oops(msg);
		throw oops;    // exit() SpatialRegionGroup::CreateCounterPart:
	}


	// if we don't have a segment already, take the parameters given and
	// make them into a segment.
	if(RLS->nItems()==0) {  //handles the case where segment is not given
		LineSegment *S;
		Vector2 temp;
		if(j1.getValue()!=-1) {
			RLS->add(S=new LineSegment(Vector2(j1.getValue(),k1.getValue()),
					Vector2(j2.getValue(),k2.getValue()),
					normal.getValue()));
			S->points=(int *) 1; // flag telling code that j's and k's were given
		}
		else
			RLS->add(S = new LineSegment(Vector2(A1.getValue(),A2.getValue()),
					Vector2(B1.getValue(),B2.getValue()),
					normal.getValue()));
		S->setNormals();
	}

	// go through the segments and normalize them
	// this might include cutting them to fit the grid.
	// invalid line segments won't get added to the list, LS
	oopicListIter<LineSegment> lsiter(*RLS);

	for(lsiter.restart();!lsiter.Done();lsiter++) {
		LineSegment *S=lsiter.current();
		makeSegmentFitGrid(S,G);

		//  if makeSegmentFitGrid discarded this segment
		if(S->A.e1()==-1) continue;

		//  Convert coordinates to grid locations.
		S->A = Vector2((int) (0.5 +G->getGridCoords(S->A).e1()),
				(int) (0.5 +G->getGridCoords(S->A).e2()));
		S->B = Vector2((int) (0.5 +G->getGridCoords(S->B).e1()),
				(int) (0.5 +G->getGridCoords(S->B).e2()));


		if(S->A.e1()>S->B.e1()) {  //need to reorder
			Vector2 temp;
			temp=S->A;S->A=S->B;S->B=temp;
		}
		if(S->A.e1()!=S->B.e1()&&S->A.e2()!=S->B.e2()) //if the boundary is oblique
		{if(S->points==(int *)0 || S->points==(int *)1)
			S->points = segmentate((int)S->A.e1(),(int)S->A.e2(),(int)S->B.e1(),(int)S->B.e2());
		}
		else {
			if(S->A.e1()==S->B.e1())  //vertical line
				if(S->B.e2()<S->A.e2()) { //reorder so that A is left/down from B
					int tmp=(int)S->B.e2();
					S->B.set_e2(S->A.e2());
					S->A.set_e2(tmp);
				}
			if(S->A.e2()==S->B.e2())  //Horizontal
				if(S->B.e1()<S->A.e1()) { //reorder so that A is left/down from B
					int tmp=(int)S->B.e1();
					S->B.set_e1(S->A.e1());
					S->A.set_e1(tmp);
				}
		}
		LS->add(S);  // add this guy to the list
	}

	// set j's and k's to be the first segment's values.
	oopicListIter<LineSegment> lsiter2(*LS);
	lsiter2.restart();
	LineSegment *firstS = lsiter2.current();
	if(LS->nItems() == 0 ) {
		j1.setValue(-1); A1.setValue(-1);
		return;
	}

	j1.setValue((int)firstS->A.e1());
	k1.setValue((int)firstS->A.e2());
	j2.setValue((int)firstS->B.e1());
	k2.setValue((int)firstS->B.e2());



}




void BoundaryParams::makeSegmentFitGrid(LineSegment *S, Grid *G) {

#ifndef MPI_VERSION
	// later on we expect coordinates in MKS units.  Convert them
	// here
	if(S->points==(int *)1) {
		S->points=0;
		S->A.set_e1(G->getX()[(int)S->A.e1()][(int)S->A.e2()].e1());
		S->A.set_e2(G->getX()[(int)S->A.e1()][(int)S->A.e2()].e2());
		S->B.set_e1(G->getX()[(int)S->B.e1()][(int)S->B.e2()].e1());
		S->B.set_e2(G->getX()[(int)S->B.e1()][(int)S->B.e2()].e2());
	}


#endif // ndef MPI_VERSION  
#ifdef MPI_VERSION
	//  Here's where we fix up the j's and k's or A's and B's to fit
	//  into the grid window we're trying to create.  An annoying thing
	//  but necessary.
	//  I basically have to clip the "lines" defined by the coordinates
	//  given.
	int seglen;  // length of the segment in int.
	if(S->points==(int *)1) { // we're working in real-world coordinates
		// easier to work in MKS units here
		// assume a UNIFORM mesh for autopartitioning.
		// transform j1, j2, k1, k2 into MKS units.
		S->points=(int *)0;
		// if the boundary is oblique, we must segmentate it now.
		if(S->A.e1()!=S->B.e1()  &&  S->A.e2()!=S->B.e2()) {
			S->points = segmentate((int)S->A.e1(),(int)S->A.e2(),(int)S->B.e1(),(int)S->B.e2());
			seglen = abs((int)(S->B.e1()-S->A.e1()));
		}
		S->A = Vector2((G->MaxMPIx1-G->MinMPIx1)*S->A.e1()/(Scalar)G->MaxMPIJ,
				(G->MaxMPIx2-G->MinMPIx2)*S->A.e2()/(Scalar)G->MaxMPIK);
		S->B = Vector2((G->MaxMPIx1-G->MinMPIx1)*S->B.e1()/(Scalar)G->MaxMPIJ,
				(G->MaxMPIx2-G->MinMPIx2)*S->B.e2()/(Scalar)G->MaxMPIK);

		/*		A1.setValue((G->MaxMPIx1-G->MinMPIx1)*j1.getValue()/(Scalar)G->MaxMPIJ);
	 B1.setValue((G->MaxMPIx1-G->MinMPIx1)*j2.getValue()/(Scalar)G->MaxMPIJ);
	 A2.setValue((G->MaxMPIx2-G->MinMPIx2)*k1.getValue()/(Scalar)G->MaxMPIK);
	 B2.setValue((G->MaxMPIx2-G->MinMPIx2)*k2.getValue()/(Scalar)G->MaxMPIK);
		 */
	}

	Scalar y,m,x,b;  // linear parameters
	Scalar dx2,dy2;  // need these because floating point isn't exact.
	Scalar x1min = G->getX()[0][0].e1();
	Scalar x2min = G->getX()[0][0].e2();
	Scalar x1max = G->getX()[G->getJ()][0].e1();
	Scalar x2max = G->getX()[0][G->getK()].e2();
	dx2 = G->getX()[1][0].e1()-G->getX()[0][0].e1();
	dx2 /=2.01; //  so that rounding will occur properly later.
	dy2 = G->getX()[0][1].e2()-G->getX()[0][0].e2();
	dy2 /=2.01; //  so that rounding will occur properly later.

	Scalar a1,a2,b1,b2,t1,t2;

	//  a1 = A1.getValue(); a2 = A2.getValue(); b1 = B1.getValue();b2 = B2.getValue();
	a1 = S->A.e1(); a2 = S->A.e2(); b1 = S->B.e1();b2 = S->B.e2();

	// segmentate, if necessary
	if(S->points==(int *)0 && a1!=b1 && a2!=b2) {
		int d1,d2,e1,e2;
		d1 = (int) (0.5+(S->A.e1() - x1min)/dx2*2.01);
		e1 = (int) (0.5+(S->B.e1() - x1min)/dx2*2.01);
		d2 = (int) (0.5+(S->A.e2() - x2min)/dy2*2.01);
		e2 = (int) (0.5+(S->B.e2() - x2min)/dy2*2.01);
		seglen = abs(e1 - d1);
		S->points=segmentate(d1,d2,e1,e2);
	}

	//  at this point, S->points, if it exists, is segmentated
	// based on global j's and k's.  We need to go through the
	// array, put it in terms of local j's and k's, and rebuild S->points
	// appropriately.
	if(S->points!=(int *)0 && S->points!=(int *)1)
	{ int d1l,d2l,J,K;
	int *points=S->points;
	int *newpoints;
	int j,jn;
	J = G->getJ();
	K = G->getK();
	d1l = (int) (0.5 + (x1min-G->MinMPIx1)/(dx2*2.01));
	d2l = (int) (0.5 + (x2min-G->MinMPIx2)/(dy2*2.01));
	for(j=0;j<4*seglen+4;j+=2) {
		points[j]-=d1l;
		points[j+1]-=d2l;
	}
	newpoints=new int[4 *( 3+ (seglen))];
	for(j=0,jn=0;j<4*seglen+4;j+=2) {
		// if this is a legal point
		if(points[j] >=0 && points[j] <=J && points[j+1]>=0 && points[j+1]<=K)
		{
			newpoints[jn]=points[j];
			newpoints[jn+1]=points[j+1];
			jn+=2;
		}
	}
	delete[] points;
	S->points=newpoints;
	}

	if(a1 > b1 ) { // swap the points
		t1 = a1; t2 = a2;
		a1 = b1; a2 = b2;
		b1 = t1; b2 = t2;
	}
	if(a1 != b1 )
		m = (a2 - b2) / (a1 - b1 ) ;
	else m=0;
	b = - m * a1 + a2;

	//  discard any boundaries completely out of bounds.
	if( (a1 < x1min - dx2 && b1 < x1min - dx2 ) || ( b1 > x1max + dx2 && a1 > x1max + dx2))
	{
		//		j1.setValue(-1);
		//		A1.setValue(-1);
		S->A.set_e1(-1);
		return;
	}

	if(a1 < x1min )  // out of bounds, need to squish it....
	{
		a1 = x1min;
		if(m!=0)
			a2 = m * a1 + b;
	}
	if(b1 > x1max ) // out of bounds...
	{
		b1 = x1max;
		if(m!=0) 
			b2 = m * b1 + b;
	}
	//  discard any boundaries completely out of bounds.
	if( (a2 < x2min - dy2&& b2 < x2min - dy2) || ( b2 > x2max + dx2 && a2 > x2max + dx2))
	{
		//		j1.setValue(-1);
		//		A1.setValue(-1);
		S->A.set_e1(-1);
		return;
	}

	if(a2 > b2 ) { // swap the points
		t1 = a1; t2 = a2;
		a1 = b1; a2 = b2;
		b1 = t1; b2 = t2;
	}

	if(a2 < x2min) // out of bounds, need to squish it.....
	{
		a2 = x2min;
		if(m!=0)
			a1 = 1.0 / m * ( a2 - b2 ) + b1;
	}

	if(b2 > x2max) // out of bounds, need to squish it.....
	{
		b2 = x2max;
		if(m!=0)
			b1 = 1.0/m * (b2 - a2) + a1;
	}

	if( (a2 < x2min - dy2&& b2 < x2min - dy2) || ( b2 > x2max + dx2 && a2 > x2max + dx2))
	{
		//		j1.setValue(-1);
		//		A1.setValue(-1);
		S->A.set_e1(-1);
		return;
	}

	// Hopefully, we now have a normalized, windowed line, write it back.
	S->A.set_e1(a1); S->A.set_e2(a2); S->B.set_e1(b1); S->B.set_e2(b2);

#endif  /* MPI_VERSION */


}
