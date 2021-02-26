/*
 * VECTOR.H
 *
 * Class definitions for vectors, Vector3 and Vector2.  The Vector3 class is
 * a 3-component Vector3 representation with all the usual operators.  Vector2
 * is an implementation for 2-component vectors.  Note that all operations
 * are not valid on the latter.
 *
 * The Vector3 class is a 3-component vector representation
 * encapsulating many of the standard mathematical vector operations.
 * Here 'vector' refers to the mathematical concept of a vector,
 * an ordered triplet representing a point in three-space.  
 * 
 * The Vector2 class is a 2-component vector representation
 * encapsulating many of the standard mathematical vector operations.
 * Here 'vector' refers to the mathematical concept of a vector,
 * an ordered doublet representing a point in two-space.  
 *
 * Revision/Programmer/Date
 * 0.1	(JohnV, 12-07-92)	Proto code.
 * 0.2	(JohnV, 01-17-93) Fix case.
 * 0.3	(JohnV, 02-20-93) Separate header file.
 * 0.4	(JohnV, 02-27-93) Add Vector:: operator / (), -=() and +=().
 * 0.5	(JohnV, 03-18-93)	Add Vector2 class.
 * 0.6	(JohnV, 08-02-93) Inline constructors, eliminate VECTOR.CPP, add
 * 		Vector::operator *=(Vector&).
 * 0.61	(JohnV, 11-22-93) Rename Vector->Vector3, add Vector3::norm(),
 * 		Vector3::scaleBy().
 * 0.62	(JohnV NTG, 12-02-93) Removed friend function for c*Vector3,
 * 		absorbed Sqrt() global as Vector3::ComponentSqrt(), renamed
 * 		Vector3::norm() to magnitude().  Same changes for Vector2.
 * 0.63	(JohnV 02-09-95) Added Vector3::operator /=.
 * 0.64	(JohnV 05-28-95) Added isNonZero().
 * 1.01	(PeterM 03-25-97) Segment class added.
 * 1.02  (KeithC 06-08-97) added int vectors.
 * 1.03 (PVS 07-05) added dotprod
 * 
 */

#ifndef	__OVECTOR_H
#define	__OVECTOR_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <cmath>
#include  "misc.h" 
#include  "precision.h" 

#include <ostring.h>

#define sqrt(x) sqrt((Scalar)(x))
class Vector3
{
	Scalar	x1, x2, x3;

public:
	Vector3(Scalar _x1=0.0, Scalar _x2=0.0, Scalar _x3=0.0) {x1=_x1; x2=_x2;
		x3=_x3;};
	Scalar	e1() const {return x1;};
	Scalar	e2() const {return x2;};
	Scalar	e3() const {return x3;};
	Scalar  e(int component){
		switch (component)
			{
			case 1:
				return x1;
			case 2:
				return x2;
			case 3:
				return x3;
			}
		return (0.0);
	}
	void	set_e1(Scalar _x1) {x1 = _x1;};
	void	set_e2(Scalar _x2) {x2 = _x2;};
	void	set_e3(Scalar _x3) {x3 = _x3;};
	void set(int component, Scalar value){
		switch (component)
			{
			case 1:
				x1 = value;
				break;
			case 2:
				x2 = value;
				break;
			case 3:
				x3 = value;
				break;
			}
	}
	Vector3 operator + (const Vector3& v) {return Vector3(x1+v.x1, x2+v.x2, x3+v.x3);};
	Vector3	operator - (const Vector3& v) {return Vector3(x1-v.x1, x2-v.x2, x3-v.x3);};
	Vector3	operator * (double c) {return Vector3(c*x1, c*x2, c*x3);};
	Vector3	operator / (double c) {Scalar ic = 1/c; return Vector3(ic*x1, ic*x2,
		ic*x3);};
	double	operator * (const Vector3& v) {return x1*v.x1 + x2*v.x2 + x3*v.x3;};
	Vector3&	operator += (double c) {x1 += c; x2 += c; x3 += c; return *this;};
	Vector3&	operator -= (double c) {x1 -= c; x2 -= c; x3 -= c; return *this;};
	Vector3&	operator *= (double c) {x1 *= c; x2 *= c; x3 *= c; return *this;};
	Vector3&	operator *= (const Vector3& v) {x1 *= v.x1; x2 *= v.x2; x3 *= v.x3;
		return *this;};
	Vector3&	operator += (const Vector3& v) {x1 += v.x1; x2 += v.x2; x3 += v.x3;
		return *this;};
	Vector3&	operator -= (const Vector3& v) {x1 -= v.x1; x2 -= v.x2; x3 -= v.x3;
		return *this;};
	Vector3& operator /= (double c) {Scalar ic = 1/c; return *this *= ic;}
	Vector3	cross(const Vector3& v){return Vector3(x2*v.x3 - x3*v.x2, x3*v.x1
		- x1*v.x3, x1*v.x2 - x2*v.x1);};
	Vector3	jvMult(const Vector3& v) {return Vector3(x1*v.x1, x2*v.x2, x3*v.x3);};
	Vector3	scaleBy(const Vector3 v) {return jvMult(v);};
	Vector3	jvDivide(const Vector3& v) {return Vector3(x1/v.x1, x2/v.x2, x3/v.x3);}; //added by kc 8-30-94
	Scalar	magnitude() {return sqrt(x1*x1 + x2*x2 + x3*x3);};
	Scalar  dotprod(const Vector3& v) {return x1*v.x1+x2*v.x2+x3*v.x3;}; //pvs 7.15.05
	Vector3	ComponentSqrt(){return Vector3(sqrt(x1), sqrt(x2),	sqrt(x3));}
	Vector3 ComponentSqr(){return Vector3(sqr(x1), sqr(x2), sqr(x3));}
	int	isNonZero() {if (x1 == 0.0 && x2 == 0.0 && x3 == 0) return 0;
		else return 1;}
};

inline Vector3	operator *(double c,const Vector3& v)
{
	return Vector3(c*v.e1(), c*v.e2(), c*v.e3());
}


class Vector2
{
	Scalar	x1, x2;

public:
	Vector2(Scalar _x1=0.0, Scalar _x2=0.0) {x1=_x1; x2=_x2;};
	Vector2(const Vector2& v) {x1 = v.x1; x2 = v.x2;};
	Scalar e1() const {return x1;};
	Scalar e2() const  {return x2;};
	Scalar  e(int component){
		switch (component)
			{
			case 1:
				return x1;
			case 2:
				return x2;
			}
		return (0.0);
	}
	void	set_e1(Scalar _x1) {x1 = _x1;};
	void	set_e2(Scalar _x2) {x2 = _x2;};
	void set(int component, Scalar value){
		switch (component)
			{
			case 1:
				x1 = value;
				break;
			case 2:
				x2 = value;
				break;
			}
	}
	Vector2 operator + (const Vector2& v) {return Vector2(x1+v.x1, x2+v.x2);};
	Vector2	operator - (const Vector2& v) {return Vector2(x1-v.x1, x2-v.x2);};
	Vector2	operator * (double c) {return Vector2(c*x1, c*x2);};
	Vector2	operator / (double c) {Scalar ic = 1/c; return Vector2(ic*x1,
		ic*x2);};
	double	operator * (const Vector2& v) {return x1*v.x1 + x2*v.x2;};
	Vector2&	operator += (double c) {x1 += c; x2 += c; return *this;};
	Vector2&	operator -= (double c) {x1 -= c; x2 -= c; return *this;};
	Vector2&	operator *= (double c) {x1 *= c; x2 *= c; return *this;};
	Vector2&	operator *= (const Vector2& v) {x1 *= v.x1; x2 *= v.x2;
		return *this;};
	Vector2&	operator += (const Vector2& v) {x1 += v.x1; x2 += v.x2; return *this;};
	Vector2&	operator -= (const Vector2& v) {x1 -= v.x1; x2 -= v.x2; return *this;};
	Vector2	jvMult(const Vector2& v) {return Vector2(x1*v.x1, x2*v.x2);};
	Vector2	jvDivide(const Vector2& v) {return Vector2(x1/v.x1, x2/v.x2);}; // added by kc 8-30-94
	Scalar	magnitude() {return sqrt(x1*x1 + x2*x2);};
	Vector2	ComponentSqrt(){return Vector2(sqrt(x1), sqrt(x2));}
	Vector2 ComponentSqr(){return Vector2(sqr(x1), sqr(x2));}
	int	isNonZero() {if (x1 == 0.0 && x2 == 0) return 0;
		else return 1;}
};

inline Vector2	operator *(double c, const Vector2& v)
{
	return Vector2(c*v.e1(), c*v.e2());
}

class LineSegment
{

 public:
  Vector2 A;
  Vector2 B;
  int normal;
  int v_nor;
  int h_nor;
  int *points;
  ostring SegName;
  
  LineSegment(const Vector2& _vectorA, const Vector2& _vectorB, int _normal) {
    A = _vectorA;
    B = _vectorB;
    normal=_normal;
    points=0;
    h_nor=0;
    v_nor=0;
  }
  void setSegmentName(ostring SN) {SegName=SN;}
  void setNormals(void) {
    h_nor=0;
    v_nor=0;
    Vector2 delta = A - B;
    if (delta.e1()==0){ //along x2
      v_nor=normal;
    }
    else if (delta.e2()==0){ //along x1
      h_nor=normal;
    }
    else if (delta.e1()*delta.e2()>0){
      h_nor=-normal;
      v_nor=normal;
    }
    else{
      h_nor=normal;
      v_nor=normal;
    }
  }
  ~LineSegment() {
    delete[] points;
  }
};  

/**************************************************/

typedef	int	Int;

class intVector3
{
	Int	x1, x2, x3;

public:
	intVector3(Int _x1=0, Int _x2=0, Int _x3=0) {x1=_x1; x2=_x2;
		x3=_x3;};
	Int	e1() const {return x1;};
	Int	e2() const {return x2;};
	Int	e3() const {return x3;};
	Int  e(int component){
		switch (component)
			{
			case 1:
				return x1;
			case 2:
				return x2;
			case 3:
				return x3;
			}
		return (0);
	}
	void	set_e1(Int _x1) {x1 = _x1;};
	void	set_e2(Int _x2) {x2 = _x2;};
	void	set_e3(Int _x3) {x3 = _x3;};
	void set(int component, Int value){
		switch (component)
			{
			case 1:
				x1 = value;
				break;
			case 2:
				x2 = value;
				break;
			case 3:
				x3 = value;
				break;
			}
	}
	intVector3 operator + (const intVector3& v) {return intVector3(x1+v.x1, x2+v.x2, x3+v.x3);};
	intVector3	operator - (const intVector3& v) {return intVector3(x1-v.x1, x2-v.x2, x3-v.x3);};
	intVector3	operator * (Int c) {return intVector3(c*x1, c*x2, c*x3);};
	Vector3	operator / (double c) {Scalar ic = 1/c; return Vector3(ic*x1, ic*x2,
		ic*x3);};
	Int	operator * (const intVector3& v) {return x1*v.x1 + x2*v.x2 + x3*v.x3;};
	intVector3&	operator += (Int c) {x1 += c; x2 += c; x3 += c; return *this;};
	intVector3&	operator -= (Int c) {x1 -= c; x2 -= c; x3 -= c; return *this;};
	intVector3&	operator *= (Int c) {x1 *= c; x2 *= c; x3 *= c; return *this;};
	intVector3&	operator *= (const intVector3& v) {x1 *= v.x1; x2 *= v.x2; x3 *= v.x3;
		return *this;};
	intVector3&	operator += (const intVector3& v) {x1 += v.x1; x2 += v.x2; x3 += v.x3;
		return *this;};
	intVector3&	operator -= (const intVector3& v) {x1 -= v.x1; x2 -= v.x2; x3 -= v.x3;
		return *this;};
	Vector3 operator /= (double c) {Scalar ic = 1/c; return Vector3(ic*x1,ic*x2,ic*x3);}
	intVector3	cross(const intVector3& v){return intVector3(x2*v.x3 - x3*v.x2, x3*v.x1
		- x1*v.x3, x1*v.x2 - x2*v.x1);};
	intVector3	jvMult(const intVector3& v) {return intVector3(x1*v.x1, x2*v.x2, x3*v.x3);};
	intVector3	scaleBy(const intVector3 v) {return jvMult(v);};
	Vector3	jvDivide(const intVector3& v) {return Vector3(x1/v.x1, x2/v.x2, x3/v.x3);};
	Scalar	magnitude() {return sqrt(x1*x1 + x2*x2 + x3*x3);};
	Vector3	ComponentSqrt(){return Vector3(sqrt(x1), sqrt(x2),	sqrt(x3));}
	intVector3 ComponentSqr(){return intVector3(sqr(x1), sqr(x2), sqr(x3));}
	int	isNonZero() {if (x1 == 0.0 && x2 == 0.0 && x3 == 0) return 0;
		else return 1;}
};

inline intVector3	operator *(Int c,const intVector3& v)
{
	return intVector3(c*v.e1(), c*v.e2(), c*v.e3());
}


class intVector2
{
	Int	x1, x2;

public:
	intVector2(Int _x1=0, Int _x2=0) {x1=_x1; x2=_x2;};
	intVector2(const intVector2& v) {x1 = v.x1; x2 = v.x2;};
	Int e1() const {return x1;};
	Int e2() const  {return x2;};
	Int  e(int component){
		switch (component)
			{
			case 1:
				return x1;
			case 2:
				return x2;
			}
		return (0);
	}
	void	set_e1(Int _x1) {x1 = _x1;};
	void	set_e2(Int _x2) {x2 = _x2;};
	void set(int component, Int value){
		switch (component)
			{
			case 1:
				x1 = value;
				break;
			case 2:
				x2 = value;
				break;
			}
	}
	intVector2 operator + (const intVector2& v) {return intVector2(x1+v.x1, x2+v.x2);};
	intVector2	operator - (const intVector2& v) {return intVector2(x1-v.x1, x2-v.x2);};
	intVector2	operator * (Int c) {return intVector2(c*x1, c*x2);};
	Vector2	operator / (double c) {Scalar ic = 1/c; return Vector2(ic*x1,
		ic*x2);};
	double	operator * (const intVector2& v) {return x1*v.x1 + x2*v.x2;};
	intVector2&	operator += (Int c) {x1 += c; x2 += c; return *this;};
	intVector2&	operator -= (Int c) {x1 -= c; x2 -= c; return *this;};
	intVector2&	operator *= (Int c) {x1 *= c; x2 *= c; return *this;};
	intVector2&	operator *= (const intVector2& v) {x1 *= v.x1; x2 *= v.x2;
		return *this;};
	intVector2&	operator += (const intVector2& v) {x1 += v.x1; x2 += v.x2; return *this;};
	intVector2&	operator -= (const intVector2& v) {x1 -= v.x1; x2 -= v.x2; return *this;};
	intVector2	jvMult(const intVector2& v) {return intVector2(x1*v.x1, x2*v.x2);};
	Vector2	jvDivide(const intVector2& v) {return Vector2(x1/v.x1, x2/v.x2);}; // added by kc 8-30-94
	Scalar	magnitude() {return sqrt(x1*x1 + x2*x2);};
	Vector2	ComponentSqrt(){return Vector2(sqrt(x1), sqrt(x2));}
	intVector2 ComponentSqr(){return intVector2(sqr(x1), sqr(x2));}
	int	isNonZero() {if (x1 == 0.0 && x2 == 0) return 0;
		else return 1;}
};

inline intVector2	operator *(Int c, const intVector2& v)
{
	return intVector2(c*v.e1(), c*v.e2());
}

#endif	// __OVECTOR_H
