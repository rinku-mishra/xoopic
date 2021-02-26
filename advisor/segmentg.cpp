#include "segmentg.h"
using namespace std;

  ostring SegmentParams::InitializeFromStream(ifstream &fin) {
	 //Use the old parameter group to read the parameters
	 LineSegment *S;
	 ostring msg;
	 msg = ParameterGroup::InitializeFromStream(fin);
	 //We need to add A1,A2,B1,B2 to the list
	 if(j1.getValue()!=-1) { // segment endpoints given in integer grid locations
		A1.setValue(j1.getValue());
		A2.setValue(k1.getValue());
		B1.setValue(j2.getValue());
		B2.setValue(k2.getValue());
		S = new LineSegment(Vector2(A1.getValue(),A2.getValue()),
								Vector2(B1.getValue(),B2.getValue()),
								normal.getValue());
		S->points=(int *)1;  //flag telling code that j's and k's were given
	 }
	 else
		 S = new LineSegment(Vector2(A1.getValue(),A2.getValue()),
												 Vector2(B1.getValue(),B2.getValue()),
												 normal.getValue());
	 
	 S->setSegmentName(SegName.getValue());
	 S->setNormals();
	 ReadLineSegmentList.add(S);
	 return msg;
  };

SegmentParams::SegmentParams():ParameterGroup() {
	 name = "Segment";
	 A1.setNameAndDescription("A1","x1 location for first segment endpoint.");
	 A2.setNameAndDescription("A2","x2 location for first segment endpoint.");
	 B1.setNameAndDescription("B1","x1 location for second segment endpoint.");
	 B2.setNameAndDescription("B2","x2 location for second segment endpoint.");
	 normal.setNameAndDescription("normal","direction of normal.");
	 A1.setValue("-1");
	 A2.setValue("-1");
	 B1.setValue("-1");
	 B2.setValue("-1");
	 normal.setValue("0");
	 parameterList.add(&A1);
	 parameterList.add(&B1);
	 parameterList.add(&A2);
	 parameterList.add(&B2);
	 parameterList.add(&normal);

	 SegName.setNameAndDescription("name","name of this segment");
	 SegName.setValue("Noname");
	 parameterList.add(&SegName);

	 j1.setNameAndDescription("j1","x1 location for first segment endpoint.");
	 k1.setNameAndDescription("k1","x2 location for first segment endpoint.");
	 j2.setNameAndDescription("j2","x1 location for second segment endpoint.");
	 k2.setNameAndDescription("k2","x2 location for second segment endpoint.");

	 j1.setValue("-1");
	 k1.setValue("-1");
	 j2.setValue("-1");
	 k2.setValue("-1");

	 parameterList.add(&j1);
	 parameterList.add(&j2);
	 parameterList.add(&k1);
	 parameterList.add(&k2);

  };



