//----------------------------------------------------------------------------
//
// File:        NGD.h
//
// Purpose:     A container of a Neutral Gas Density for use in 
//              tunneling ionziation simulations. 
//
// Version:    $Id: ngd.h 2295 2006-09-18 18:05:10Z yew $ 
//
// Copyright 2001 by Tech-X Corporation
//
//----------------------------------------------------------------------------

#ifndef NGD_H
#define NGD_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "ostring.h"
#include "precision.h" // needed for the Scalar type
#include "ovector.h"
#include "evaluator.h" 

#include <oops.h>


#if defined(_MSC_VER)
#include <iomanip>
#include <sstream>
#include <iostream>
using std::ends;
#else

using namespace std;
#endif


extern char Revision[];
extern Scalar dumpFileRevisionNumber;

#ifdef HAVE_HDF5
#include "dumpHDF5.h"
#endif

class Grid;

class NGD {
 public:
  NGD(Grid* pgrid, const ostring& gType, 
      const ostring& _analyticF, const Scalar& _gasDensity,  
      const Vector2& _p1Grid, const Vector2& _p2Grid,
      const int& _discardDumpFileNGDDataFlag) /* throw(Oops) */;
  ~NGD();

// Accessors
  Scalar getNGD(const int& j, const int& k) const /* throw(Oops) */;

  Scalar** getNGDdata() const {
    return NeutralGasDensity;
  }

  ostring getGasType() {
    return gasType;
  }

  Scalar getMaxNGD() {
    return maxNGD;
  }

  /** 
   * get the number of physical particles per computational 
   * particle for a cell on the 2D grid. The cell is specified
   * by the j, k pair of indeces.
   */
  Scalar getNGD_TI_np2c(const int& j, const int& k) const /* throw(Oops) */;

  int getJ() const { return J;}
  int getK() const { return K;}

  /** 
   * set the number of physical particles per computational 
   * particle for a cell on the 2D grid. The cell is specified
   * by the j, k pair of indices.
   */
  void setNGD_TI_np2c(const int j, const int k, Scalar value) /* throw(Oops) */;

  /**
   * get the excess number of ions for cell (j, k)
   */
  Scalar getNGD_excessNumIons( const int& j, const int& k ) const /* throw(Oops) */;

// Manipulators
  /**
   * set the uniform gas density 
   */
  void setNeutralGasDensity(const Scalar&, 
                            const Vector2&, const Vector2&) /* throw(Oops) */; 
  void setNeutralGasDensity(const ostring &, const Vector2&, const Vector2&) /* throw(Oops) */;
  void set(const int& j, const int& k, const Scalar value) {
    NeutralGasDensity[j][k] = value;
  }

  /** 
   * allocate memory for TI_np2c and set its values
   */ 
  void initNGD_TI_np2c(const Scalar& TI_numMacroParticlesPerCell, Grid* grid) /* throw(Oops) */; 

  /**
   * allocate memory for excessNumIons and set its elements to the
   * default values of 0
   */
  void initNGD_excessNumIons();

  /**
   * set the excess number of ions for cell (j, k)
   */
  void setNGD_excessNumIons( const int& j, const int& k, const Scalar& value) /* throw(Oops) */;

  /**
   * add to the excess number of ions for cell (j, k)
   */
  void add_to_NGD_excessNumIons( const int& j, const int& k, 
                                 const Scalar& value) /* throw(Oops) */;
  /**
   * Dump the data structures for the neutral gas density
   */
#ifdef HAVE_HDF5
  int dumpH5(dumpHDF5 &dumpObj,int number);
  int restoreH5(dumpHDF5 &restoreObj,string whichNGD) /* throw(Oops) */;
#endif
  int Dump(FILE *DMPFile);

  /**
   * Restore the data structures for the neutral gas density
   */
  int Restore(FILE *DMPFile);

  int getisTIOn() const {
    return isTIOn;
  }
  
  void setisTIOn() {
    if ( TI_np2c != 0 && excessNumIons != 0 ) 
      isTIOn = 1;
  }

 private: 
  // we do not want to allow copy and assignment of this class
  NGD(const NGD&);
  NGD& operator=(const NGD&);

  // data members
  Grid* ptrLocalGrid;
  ostring gasType;
  int J; 
  int K;
  Scalar maxNGD; // max value of the NeutralGasDensity
  Scalar** NeutralGasDensity;
  /**
   * The discardDumpFileNGDDataFlag is used to decide if 
   * the NGD data in a dump file is to be used or not. 
   * Its default falue is false in which case the 
   * NGD data in the dump file is read into the corresponding
   * data members of the NGD object instances. If the value
   * of the flag is true, the NGD data in a dump file is
   * read but discarded. The simulation uses the input file
   * information to initialize the NGD data structures in this case.
   */
  bool discardDumpFileNGDDataFlag;

  /**
   * We need the number of physical particles per computational 
   * particle for each cell. This will be stored in the 
   * matrix pointed to by "np2c". This pointer is needed
   * only for the tunneling ionization simulations. It will be
   * set by default to 0. (dad, 04/06/01).
   */
  Scalar** TI_np2c;

  /**
   * macro particles with ions, due to ionization of neutral atoms,
   * will be created when the number of
   * ions in a cell exceeds the number of ions in a macro particle
   * for the particular cell. 
   * "excessNumIons[j][k]" is needed to monitor current number of 
   * ions in the cell (j, k)
   * requested due to tunneling ionization. When this number 
   * exceeds TI_np2c(j, k) (the number of ions in a macro particle
   * for the cell (j, k)): 
   * excessNumIons[j][k] = numMP * TI_np2c(j, k) + Remainder, 
   * then numMP macro particles are created and excessNumIons[j][k] 
   * is reset: excessNumIons[j][k] = Remainder. dad 04/06/01.
   */
  Scalar** excessNumIons;
  /**
   * The TI_np2c and excessNumIons data structures are used only
   * in the case of tunneling ionization simulations for now.
   * The isTIOn integer will be set to non zero only if 
   * memory is allocated for both the TI_np2c and the excessNumIons.
   * It is initially set to zero in the constructor of NGD. 
   */
  int isTIOn; 

  /** 
   * helper function to check for out of bounds condition in both
   * the j and k indeces
   */
  bool check_jk_bounds(const int& j, const int& k, char* place) const /* throw(Oops) */;

  /**
   * The first revision number for which the NGD class appeared.
   */
  static const Scalar firstRevisionNumberWithNGD;
};

#endif // NGD_H
