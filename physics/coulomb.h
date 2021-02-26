/*
=====================================================================
coulomb.h

Describes the Coulomb collision class.  
This class provides collisions by operating on ParticleGroups.
=====================================================================
*/

#ifndef	__COULOMB_H
#define	__COULOMB_H

#include "species.h"
class ParticleGroup; 
class Grid;
class Fields;

//CellElement class is the class for 
//particlegroup pointer and index of the particle
class CellElement
{
  ParticleGroup *pg;
  int index;

 public:

  CellElement(void){pg=NULL; index=0;};
  CellElement(ParticleGroup *_pg, int _index) {pg=_pg; index=_index;};
  void set(ParticleGroup* _pg, int _index) {pg=_pg; index=_index;};
  int get_index(){return index;};
  ParticleGroup* get_pg(){return pg;};
};

typedef oopicList<CellElement> cellList;


class Coulomb 
{
 public:
  Coulomb(Species* _species, Grid* _grid, Fields* _fields, Scalar _dt, SpeciesList* _speciesList ) {
    elementarray = new CellElement [100000];
    species1=_species;
    grid=_grid;
    fields=_fields;
    dt=_dt;
    speciesList = _speciesList;
  };
  ~Coulomb() {
    delete[] elementarray;
  };

  void coulombcollide(ParticleGroupList** pgList);


 protected: 
  //randomize the cellList before pairing the collisions
  CellElement* randomize(cellList *onecell); 
  //update the velocity
  void update_velocity(CellElement cellelement1, CellElement cellelement2, Scalar squ_rel_vel);
  //calculate the scattering angle
  Scalar get_scatteringangle(Species* species1, Species* species2, Scalar reduced_mass, Scalar magnitude, Scalar chargedensity, Scalar squ_rel_vel);
  //get the cell volume
  Scalar get_cellVolume(int cellnumber);
  //get the number density inside the cell
  //We are using this in order to calculate the number of collision pairs for Nanbu model
  //scattering angle calculation uses the linear weighting scheme
  Scalar density_calculation(cellList *onecell, Scalar tmp_vol);
  //get the average velocity inside the cell
  Scalar velocity_calculation(cellList *onecell);
  //get the kinetic energy inside the cell
  Scalar kinetic_calculation(cellList *onecell);
  //calculate the tmeperature
  Scalar temperature_calculation(cellList *onecell);

 private:
  CellElement* elementarray;
  ParticleGroup* particlegroup1;
  ParticleGroup* particlegroup2;
  int sizeofcell;
  Species *species1;
  Species *species2;
  SpeciesList* speciesList;
  Grid* grid;
  Fields* fields;
  Scalar dt;
  Scalar T_1;
  Scalar T_2;
  int count;

  //get the new relative velocity after collision
  Vector3 newrelative(Vector3 oldv, Scalar _theta);
  //get the A value to calculate scattering angle for Nanbu
  Scalar get_A(Scalar tau);
  //get the x index from cell number
  int get_x_index(int cellnumber);
  //get the y index from cell number
  int get_y_index(int cellnumber);

  //remove cell list
  void remove_cell(cellList* cellArray, int J){
    if(cellArray){
      for (int i=0; i<J; i++){
	cellArray[i].deleteAll();
      }
      delete [] cellArray; 
    }
  }
  
};
#endif	//	ifndef __COULOMB_H

