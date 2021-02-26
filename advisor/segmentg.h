//  A group for segment parameters.
//  3-25-97, PeterM:  Creation
#ifndef __SEGMENT_H
#define __SEGMENT_H
#include "part1g.h"
#include "ovector.h"

class SegmentParams: public ParameterGroup
{
 protected:
  oopicList<LineSegment> LineSegmentList;  //  the points that make up this set of segments.
  oopicList<LineSegment> ReadLineSegmentList;  //  Line segments from the input file
	 // previous is what is actually used to make boundaries.
	 // these can be different.
  public:
	 
  ScalarParameter A1,A2,B1,B2;
  IntParameter j1,j2,k1,k2;
  IntParameter normal;
  StringParameter SegName;

  virtual ostring InitializeFromStream(std::ifstream &fin);
  oopicList<LineSegment> *GetLineSegments() {
	 return &LineSegmentList;
  }
  oopicList<LineSegment> *GetReadLineSegments() {
	 return &ReadLineSegmentList;
  }
  SegmentParams();

};

#endif


