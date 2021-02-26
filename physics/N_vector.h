#ifndef VECTOR_H
#define VECTOR_H

#ifdef _MSC_VER
#include <iomanip>
#include <iostream>
using std::ends;
#else

using namespace std;
#endif

#include <cassert>
#include <cmath>

#define true 1

template<class ScalarType> class Vector {

#ifdef UNIX
  friend ostream& operator<<(ostream& output,const Vector& v) {
	 output << "(";
	 for (int i=0; i<v.dimension; i++)
		output << "[" << i << "]=" << v.component[i] << " ";
	 output << ")";
	 return output;
  };
#endif

  friend Vector operator*(const ScalarType z, const Vector& v)  {
	 int dimension = v.dimension;
 	 Vector v1(dimension);
 	 for (int i=0; i<dimension; i++)
 		v1.component[i]=v.component[i]*z;
 	 return v1;
  };

public:

  // default constructor -- zero vector
  Vector(int dim=0)
  {
	 dimension = dim;
	 component = (dimension==0) ? (ScalarType*) NULL : new ScalarType[dimension];
	 
	 // terminate if memory not allocated
	 assert((component==(ScalarType*) NULL) ? true : (component!=0)); 

	 for (int i=0; i<dimension; i++)
		component[i] = 0; 
  };

  // redimensionalize zero vector 
  void redimensionalize(int dim)
  {
	 dimension = dim;
	 delete[] component;
	 component = (dimension==0) ? (ScalarType*) NULL : new ScalarType[dimension];

	 // terminate if memory not allocated
	 assert((component==(ScalarType*) NULL) ? true : (component!=0));

	 for (int i=0; i<dimension; i++)
		component[i] = 0; 
  }

  // explicit 2d constructor
  inline Vector(ScalarType x1, ScalarType x2)
  {
	 dimension = 2;
	 component = new ScalarType[dimension];
	 assert(component!=0); // terminate if memory not allocated
	 
 	 component[0] = x1;
 	 component[1] = x2;
  }

  // explicit 3d constructor
  inline Vector(ScalarType x1, ScalarType x2, ScalarType x3)
  {
	 dimension = 3;
	 component = new ScalarType[dimension];
	 assert(component!=0); // terminate if memory not allocated
	 
 	 component[0] = x1;
 	 component[1] = x2;
	 component[2] = x3;
  }

  Vector(const Vector& v)
  {
	 dimension = v.dimension;
	 component = new ScalarType[dimension];
	 assert(component!=0); // terminate if memory not allocated

	 for (int i=0; i<dimension; i++)
		component[i] = v.component[i];
  };

  inline ~Vector()
  { delete[] component; };

  inline int get_dimension() const
  { return dimension; };

  inline ScalarType& operator[](int index) const
  {
	 assert((index>=0)&&(index<dimension));    // ensure addressing is in bounds
	 return component[index];
  };

  const Vector& operator=(const Vector& v)
  {
	 assert(dimension==v.dimension);
	 if (this != &v) {                         // check for self-assignment
		delete[] component;                     // prevent memory leak
		dimension = v.dimension;
		component = new ScalarType[dimension];
		assert(component != 0);                 // ensure memory allocated
		for (int i=0; i<dimension; i++)
		  component[i] = v.component[i];
	 }
	 return *this;                             // enable concatenated assignment
  }

  const Vector& operator+=(const Vector& v)
  {
	 assert(dimension==v.dimension);
	 for (int i=0; i<dimension; i++)
		component[i] += v.component[i];
	 return *this;                             // enable concatenated assignment
  }
  
  Vector operator+(const Vector& v) const
  {
	 assert(dimension==v.dimension);
	 Vector v1(dimension);
	 for (int i=0; i<dimension; i++)
		v1.component[i]=component[i]+v.component[i];
	 return v1;
  }

  Vector operator-(const Vector& v) const
  {
	 assert(dimension==v.dimension);
	 Vector v1(dimension);
	 for (int i=0; i<dimension; i++)
		v1.component[i]=component[i]-v.component[i];
	 return v1;
  }

  // component-wise multiplication
  Vector operator*(const Vector& v) const
  {
	 assert(dimension==v.dimension);
	 Vector v1(dimension);
	 for (int i=0; i<dimension; i++)
		v1.component[i]=component[i]*v.component[i];
	 return v1;
  }

  // vector * ScalarType
  Vector operator*(const ScalarType z) const
  {
	 Vector v1(dimension);
	 for (int i=0; i<dimension; i++)
		v1.component[i]=component[i]*z;
	 return v1;
  }

  // standard inner product over reals
  // does not do Hermitian (complex) inner products; see commented line below
  ScalarType euclidean_inner_product(const Vector& v) const
  {
	 assert(dimension==v.dimension);
	 ScalarType sum=0;
	 for (int i=0; i<dimension; i++) {
		sum += component[i]*v.component[i];
		// sum += component[i]*v.component[i].conjugate();
	 }
	 return sum;
  }

  // l2 norm
  //  inline ScalarType l2_norm() 
  //  { return sqrt(fabs((*this).euclidean_inner_product(*this))); }

  // l^2 norm over reals and complexes
  double l2_norm() const
  {  
	 double sum=0;
	 for (int i=0; i<dimension; i++) {
		double abs_val = fabs(component[i]);
		sum += abs_val*abs_val;
	 }
	 return sqrt(sum);
  }

  // l^1 norm over reals and complexes
  double l1_norm() const
  {
	 double sum=0;
	 for (int i=0; i<dimension; i++) 
		sum += fabs(component[i]);
	 return sum;
  }

  // l^infinity norm over reals and complexes
  double max_norm() const
  {
	 double max=0;
	 for (int i=0; i<dimension; i++) {
		double abs_val = fabs(component[i]);
		if (max<abs_val)
		  max = abs_val;
	 }
	 return max;
  }

private:

  int dimension;
  ScalarType *component;

};

#endif

