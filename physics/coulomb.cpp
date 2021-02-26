/*
  =====================================================================
  coulomb.cpp
  
  =====================================================================
*/
#include "coulomb.h"
#include "species.h"
#include "misc.h"
#include "ptclgrp.h"
#include "grid.h"
#include "fields.h"

void Coulomb::coulombcollide(ParticleGroupList** pgList){

  Scalar ave_velocity1 = 0.0;
  Scalar ave_velocity2 = 0.0; 
  Scalar KineticEnergy1 = 0.0;
  Scalar KineticEnergy2 = 0.0;
  
  Scalar inside_density1 = 0.0;
  Scalar inside_density2 = 0.0;
  
  Scalar tmp_cellVolume = 0.0;

  if(species1->get_coulomb()!=(ostring) "none"){
    
    int NumberofCell=grid->getJ()*grid->getK();
    
    cellList* cellArray1 = new oopicList<CellElement>[NumberofCell];
        
    oopicListIter<ParticleGroup> pgIter1(*pgList[species1->getID()]);
    for (pgIter1.restart(); !pgIter1.Done(); pgIter1++) {
      for(int k=0;k<pgIter1()->get_n();k++){
	CellElement* celement = new CellElement(pgIter1(),k);
	int cell_index = int(pgIter1()->get_x(k).e1())+int(pgIter1()->get_x(k).e2())*grid->getJ();
	cellArray1[cell_index].add(celement);
      }
    }

    //checking the target species    oopicListIter<Species> sIter(*speciesList);
    oopicListIter<Species> sIter(*speciesList);
    for (sIter.restart(); !sIter.Done(); sIter++){
      if(species1->get_coulomb()==sIter()->get_name()) species2 = sIter();
    }
    
    cellList *cellArray2 = new oopicList<CellElement>[NumberofCell];
   
    oopicListIter<ParticleGroup> pgIter2(*pgList[species2->getID()]);
    for (pgIter2.restart(); !pgIter2.Done(); pgIter2++) {
      for(int k=0;k<pgIter2()->get_n();k++){
	CellElement* celement = new CellElement(pgIter2(),k);
	int cell_index = int(pgIter2()->get_x(k).e1())+int(pgIter2()->get_x(k).e2())*grid->getJ();
	cellArray2[cell_index].add(celement);
      }
    }


    for (int i=0; i<NumberofCell; i++){
      
      Scalar squ_rel_vel;
      
      tmp_cellVolume = get_cellVolume(i); 
      
      inside_density1 = density_calculation(&cellArray1[i],tmp_cellVolume); 
      inside_density2 = density_calculation(&cellArray2[i],tmp_cellVolume); 

      ave_velocity1 = velocity_calculation(&cellArray1[i]);     
      ave_velocity2 = velocity_calculation(&cellArray2[i]);          
      
      KineticEnergy1 = kinetic_calculation(&cellArray1[i]);
      KineticEnergy2 = kinetic_calculation(&cellArray2[i]);
      
      T_1 = temperature_calculation(&cellArray1[i]);
      T_2 = temperature_calculation(&cellArray2[i]);
      
      squ_rel_vel = 2*KineticEnergy1*PROTON_CHARGE/species1->get_m()+2*KineticEnergy2*PROTON_CHARGE/species2->get_m()+(ave_velocity1-ave_velocity2)*(ave_velocity1-ave_velocity2);
      
      //randomize the species 1
      //calculate the number of collisions species1 to species1
      CellElement* elementarray1 = randomize(&cellArray1[i]);
      int numberofspecies1 = sizeofcell;            //number of particles inside cell
      //number of collisions between species 1 and species 2
      int collision_12 = numberofspecies1*int(inside_density2/(inside_density1+inside_density2)); 
      
      
      //randomize the species 2
      //calculate the number of collisions species 2 to species 2
      CellElement* elementarray2 = randomize(&cellArray2[i]);
      int numberofspecies2 = sizeofcell;             
      int collision_22 = numberofspecies2*int(inside_density2/(inside_density1+inside_density2));      

      //species 1 and species 2 collision 
      if( collision_12 > 0){
	for (int j=0; j<collision_12;j++){
	  update_velocity(elementarray1[j],elementarray2[j],squ_rel_vel);
	}
      }
      
      
      //species 1 and species 1 collision
      
      int collision_11 = numberofspecies1 - collision_12;
      
      if( collision_11 > 1){
	
	//number of particles is odd
	if(collision_11%2==1){
	  for (int j=collision_12; j<numberofspecies1-2;){
	    update_velocity(elementarray1[j],elementarray1[j+1], squ_rel_vel);
	    j = j+2;
	  }
	  update_velocity(elementarray1[collision_12],elementarray1[numberofspecies1-1],squ_rel_vel);
	}	//number of particles is even
	else{
	  for (int j=collision_12; j<numberofspecies1;){
	    update_velocity(elementarray1[j],elementarray1[j+1],squ_rel_vel);
	    j=j+2;
	  }
	}
      }
      
      
      //species 2 and species 2 collision
      
      if( collision_22 > 1){
	
	//number of particles is odd
	if(collision_22%2==1){

	  for (int j=collision_12; j<numberofspecies2-2;){
	    update_velocity(elementarray2[j],elementarray2[j+1],squ_rel_vel);
	    j = j+2;
	  }
	  update_velocity(elementarray2[collision_12],elementarray2[numberofspecies2-1],squ_rel_vel);
	}
	//number of particles is even
	else{
	  for (int j=collision_12; j<numberofspecies2;){
	    update_velocity(elementarray2[j],elementarray2[j+1],squ_rel_vel);
	    j=j+2;
	  }
	}
      }
    }
    remove_cell(cellArray1,NumberofCell);
    remove_cell(cellArray2,NumberofCell);
  }

  if(species1->get_IntraTAModel()){
   
    int NumberofCell=grid->getJ()*grid->getK();

   cellList* cellArray1 = new oopicList<CellElement>[NumberofCell];
    
    oopicListIter<ParticleGroup> pgIter1(*pgList[species1->getID()]);
    for (pgIter1.restart(); !pgIter1.Done(); pgIter1++) {
      for(int k=0;k<pgIter1()->get_n();k++){
	CellElement* celement = new CellElement(pgIter1(),k);
	int cell_index = int(pgIter1()->get_x(k).e1())+int(pgIter1()->get_x(k).e2())*grid->getJ();
	cellArray1[cell_index].add(celement);
      }
    }

    for (int i=0; i<NumberofCell; i++){
      
      Scalar squ_rel_vel;
     
      ave_velocity1 = velocity_calculation(&cellArray1[i]);     
      KineticEnergy1 = kinetic_calculation(&cellArray1[i]);
      T_1 = temperature_calculation(&cellArray1[i]);
      
      squ_rel_vel = 2*KineticEnergy1*PROTON_CHARGE/species1->get_m()+(ave_velocity1-ave_velocity2)*(ave_velocity1-ave_velocity2);
    
      //randomize the species 1
      //calculate the number of collisions species1 to species1
      CellElement* elementarray1 = randomize(&cellArray1[i]);
      int numberofspecies1 = sizeofcell;            //number of particles inside cell
      //number of collisions between species 1 and species 2
       
      if(numberofspecies1 > 1){
	
	if(numberofspecies1%2==1){

	  //When the number of particles is odd
	  for (int j=0; j<numberofspecies1-2;){
	    update_velocity(elementarray1[j],elementarray1[j+1],squ_rel_vel);
	     j=j+2;
	  }
	  update_velocity(elementarray1[0],elementarray1[numberofspecies1-1],squ_rel_vel);
	}
	//When the number of particles is even
	else{
	  for (int j=0; j<numberofspecies1-1;){
	    update_velocity(elementarray1[j],elementarray1[j+1],squ_rel_vel);
	    j=j+2;
	  }
	}
      }
    }
    
    if(species1->get_TAModel()!=(ostring) "none"){

      oopicListIter<Species> sIter(*speciesList);

      for (sIter.restart(); !sIter.Done(); sIter++){
	if(species1->get_TAModel()==sIter()->get_name()) species2 = sIter();
      }
      
      int NumberofCell=grid->getJ()*grid->getK();
    
      cellList *cellArray2 = new oopicList<CellElement>[NumberofCell];
      
      oopicListIter<ParticleGroup> pgIter2(*pgList[species2->getID()]);
      for (pgIter2.restart(); !pgIter2.Done(); pgIter2++) {
	for(int k=0;k<pgIter2()->get_n();k++){
	  CellElement* celement = new CellElement(pgIter2(),k);
	  int cell_index = int(pgIter2()->get_x(k).e1())+int(pgIter2()->get_x(k).e2())*grid->getJ();
	  cellArray2[cell_index].add(celement);
	}
      }
      
      for (int i=0; i<NumberofCell; i++){
	
	Scalar squ_rel_vel;

	ave_velocity1 = velocity_calculation(&cellArray1[i]);     
	ave_velocity2 = velocity_calculation(&cellArray2[i]);          
      
	KineticEnergy1 = kinetic_calculation(&cellArray1[i]);
	KineticEnergy2 = kinetic_calculation(&cellArray2[i]);
      
	T_1 = temperature_calculation(&cellArray1[i]);
	T_2 = temperature_calculation(&cellArray2[i]);

	squ_rel_vel = 2*KineticEnergy1*PROTON_CHARGE/species1->get_m()+2*KineticEnergy2*PROTON_CHARGE/species2->get_m()+(ave_velocity1-ave_velocity2)*(ave_velocity1-ave_velocity2);
	
	CellElement* elementarray1 = randomize(&cellArray1[i]);
	int numberofspecies1 = sizeofcell;     
	
	CellElement* elementarray2 = randomize(&cellArray2[i]);
	int numberofspecies2 = sizeofcell;
     
	if(numberofspecies2>0&&numberofspecies1 > 0){
	  
	  if (numberofspecies2 > numberofspecies1) {
	    for (int j=0; j<numberofspecies1;j++){
	      update_velocity(elementarray1[j],elementarray2[j],squ_rel_vel);
	    }
	    for (int j=numberofspecies1; j<numberofspecies2; j++){
	      int off_value = j - numberofspecies1;
	      while (off_value > numberofspecies1-1) off_value -= numberofspecies1;
	      update_velocity(elementarray1[off_value],elementarray2[j],squ_rel_vel);
	    }
	  }
	  if (numberofspecies1 >= numberofspecies2) {

	    for (int j=0; j<numberofspecies2;j++){
	      update_velocity(elementarray1[j],elementarray2[j],squ_rel_vel);
	    }
	    for (int j=numberofspecies2; j<numberofspecies1; j++){
	      int off_value = j - numberofspecies2;
	      while (off_value > numberofspecies2-1) off_value -= numberofspecies2;
	      update_velocity(elementarray1[j],elementarray2[off_value],squ_rel_vel);
	    }
	  }
	}
      }
      remove_cell(cellArray2,NumberofCell); 
    }
    remove_cell(cellArray1,NumberofCell);
  }
}



CellElement* Coulomb::randomize(cellList* onecell){
  sizeofcell = onecell->nItems();
  
  
  oopicListIter<CellElement> cellIter(*onecell);
  int cellnum=0;
  
  
  //copy the onecell into elementarry 
  for(cellIter.restart();!cellIter.Done();cellIter++){
    elementarray[cellnum].set(cellIter()->get_pg(),cellIter()->get_index());
    cellnum++;
  }

  if (sizeofcell < 2) return elementarray;

  //randomize the elementarray
  for(int i=0; i<sizeofcell; i++){

    int j=int((sizeofcell-i)*frand()+i);
    
    if (j == sizeofcell) j--;

    CellElement temp;
    temp.set(elementarray[i].get_pg(),elementarray[i].get_index());
    elementarray[i].set(elementarray[j].get_pg(),elementarray[j].get_index());
    elementarray[j].set(temp.get_pg(),temp.get_index());
  }


  return elementarray;
}

void Coulomb::update_velocity(CellElement cellelement1, CellElement cellelement2, Scalar squ_rel_vel){

  particlegroup1 = cellelement1.get_pg();
  particlegroup2 = cellelement2.get_pg();

  int index1 = cellelement1.get_index();
  int index2 = cellelement2.get_index();

  Vector3 velocity1 = particlegroup1->get_u(index1)/particlegroup1->gamma(index1);
  Vector3 velocity2 = particlegroup2->get_u(index2)/particlegroup2->gamma(index2);

  Vector2 position1 = particlegroup1->get_x(index1);
  Vector2 position2 = particlegroup2->get_x(index2);

  Species* spe1 = particlegroup1->get_species();
  Species* spe2 = particlegroup2->get_species();

  Scalar *** rho_spe = fields -> getRhoSpecies();

  Scalar **rho_spe1 = rho_spe[spe1->getID()];
  Scalar **rho_spe2 = rho_spe[spe2->getID()];
  
  Scalar density1 = grid -> interpolateBilinear(rho_spe1, position1)/particlegroup1->get_q(index1)*particlegroup1->get_np2c(index1);
  Scalar density2 = grid -> interpolateBilinear(rho_spe2, position2)/particlegroup2->get_q(index2)*particlegroup2->get_np2c(index2);

  Scalar chargedensity = MIN(density1,density2);

  Scalar phi = TWOPI*frand();
 
  Scalar reduced_mass = spe1->get_m()*spe2->get_m();
  reduced_mass/=(spe1->get_m()+spe2->get_m());

  Vector3 rel_v;
  rel_v.set_e1(velocity1.e1()-velocity2.e1());
  rel_v.set_e2(velocity1.e2()-velocity2.e2());
  rel_v.set_e3(velocity1.e3()-velocity2.e3());
  
  Scalar magnitude = rel_v.magnitude();

  if(magnitude<1e-30) return;

  Scalar theta = get_scatteringangle(spe1, spe2, reduced_mass, magnitude, chargedensity, squ_rel_vel);

  Vector3 new_particle1;
  Vector3 new_particle2;
  
  Vector3 new_relative = newrelative(rel_v,theta);
  Vector3 del_relative = (new_relative - rel_v);
  
  new_particle1 = velocity1+reduced_mass/spe1->get_m()*del_relative;
  new_particle2 = velocity2-reduced_mass/spe2->get_m()*del_relative;

  particlegroup1->set_v(new_particle1,index1);
  particlegroup2->set_v(new_particle2,index2);
  
}

Scalar Coulomb::get_scatteringangle(Species* spe1, Species* spe2, Scalar reduced_mass, Scalar magnitude, Scalar chargedensity, Scalar squ_rel_vel){
 
  Scalar scattering=0.0;
  Scalar lamdaD = 0.0;

  //tamodel
  if(spe1->get_IntraTAModel()){

    if(T_1==0) T_1=T_2;
    T_1 = 0.001;
    lamdaD = sqrt(T_1*PROTON_CHARGE/(chargedensity*PROTON_CHARGE*PROTON_CHARGE)*EPS0);
    
    Scalar coulomb_loga = log(4*M_PI*EPS0*reduced_mass*magnitude*magnitude/fabs(spe1->get_q())/fabs(spe2->get_q())*lamdaD);
    coulomb_loga = 24-log(sqrt(chargedensity)/T_1);
    Scalar nu = chargedensity*coulomb_loga*spe1->get_q()*spe1->get_q()*spe2->get_q()*spe2->get_q()*iEPS0*iEPS0/reduced_mass/reduced_mass/magnitude/magnitude/magnitude;
    
    Scalar R=frand();
    
    scattering = sqrt(fabs(-2*nu*dt*log(1-frand())));
      
    if(fabs(scattering) > M_PI){ 
      fprintf(stderr,"Need smaller time step for TA Coulomb collision model\n");
      scattering = TWOPI*frand();
    }
  }

  
  //nanbu

  if(spe1->get_coulomb()!=(ostring)"none"||spe1->get_nanbu()){

    if(T_1==0) T_1=T_2;
    
    lamdaD = sqrt(T_1*EPS0/(chargedensity*PROTON_CHARGE));
    
    Scalar coulomb_loga = log(4*M_PI*EPS0*reduced_mass*squ_rel_vel*lamdaD/fabs(spe1->get_q()*spe2->get_q()));
    
    Scalar A_ab = chargedensity/4/M_PI*spe1->get_q()*spe1->get_q()*spe2->get_q()*spe2->get_q()*iEPS0*iEPS0/reduced_mass/reduced_mass*coulomb_loga;
    
    Scalar tau = A_ab*dt/magnitude/magnitude/magnitude;
    
    Scalar R=frand();
    Scalar A=0.0;

    if(tau < 0.01) {
      A = 1/tau;
      //      scattering  = log(exp(-A)+2*frand()*sinh(A))/A;
      scattering = 1+tau*log(frand());
    }
    else if(0.01<= tau && tau <3){
      A = get_A(tau) ;
      //      scattering  = log(exp(-A)+2*frand()*sinh(A))/A;
      scattering = 1+tau*log(frand());
    }
    else if(3<= tau && tau<6.0) {
      A = 3*exp(-tau);
      //      scattering  = log(exp(-A)+2*frand()*sinh(A))/A;
      scattering = 1+tau*log(frand());
    }
    else{
      scattering = 2*frand() - 1;
    }

    if (scattering > 1) scattering = 1;
    if (scattering < -1) scattering = 1;
    
    Scalar daum = scattering;
    scattering = acos(scattering);
   
  }

  return scattering;
}
 
Vector3 Coulomb::newrelative(Vector3 oldv, Scalar theta)
{
  Scalar	up1, up2, up3;
  Scalar	coschi;
  Scalar	sinchi;
  
  coschi = cos(theta);
  sinchi = sqrt(1 - coschi*coschi);
  Scalar	phi = TWOPI*frand();
  Scalar	cosphi = cos(phi);
  Scalar	sinphi = sin(phi);
  Vector3 newVel;
  
  if(oldv.magnitude()<1e-30){
    newVel.set_e1(oldv.e1());
    newVel.set_e2(oldv.e2()); 
    newVel.set_e3(oldv.e3());
    return newVel;
  }
  
  Scalar	r13 = oldv.e1()/oldv.magnitude();
  Scalar	r23 = oldv.e2()/oldv.magnitude();
  Scalar	r33 = oldv.e3()/oldv.magnitude();
  
  if (r33 == 1 || r33 == -1)	{up1 = 0; up2 = 1; up3 = 0;}
  else				{up1 = 0; up2 = 0; up3 = 1;}
  
  Scalar	r12 = r23*up3 - r33*up2;
  Scalar	r22 = r33*up1 - r13*up3;
  Scalar	r32 = r13*up2 - r23*up1;
  Scalar	mag = sqrt(r12*r12 + r22*r22 + r32*r32);
  
  r12 /= mag;
  r22 /= mag;
  r32 /= mag;  
  
  Scalar	r11 = r22*r33 - r32*r23;
  Scalar	r21 = r32*r13 - r12*r33;
  Scalar	r31 = r12*r23 - r22*r13;
  
  
  newVel.set_e1(oldv.magnitude()*(r11*sinchi*cosphi + r12*sinchi*sinphi + r13*coschi));
  newVel.set_e2(oldv.magnitude()*(r21*sinchi*cosphi + r22*sinchi*sinphi + r23*coschi)); 
  newVel.set_e3(oldv.magnitude()*(r31*sinchi*cosphi + r32*sinchi*sinphi + r33*coschi));

  return newVel;
}

Scalar Coulomb::get_cellVolume(int cellnumber){
   
  int max_x = grid -> getJ();
  int y_index = 0;
  
  while (cellnumber > max_x-1){
    cellnumber = cellnumber - max_x;
    y_index++;
  }
  
  return grid->cellVolume(cellnumber, y_index);
}

int Coulomb::get_x_index(int cellnumber){
 
  int max_x = grid -> getJ();
 
  while (cellnumber > max_x-1){
    cellnumber = cellnumber - max_x;
  }
  return cellnumber;
}

int Coulomb::get_y_index(int cellnumber){
 
  int max_x = grid -> getJ();
  int y_index = 0;
  
  while (cellnumber > max_x-1){
     cellnumber = cellnumber - max_x;
     y_index++;
  }
  return y_index;
}

Scalar Coulomb::density_calculation(cellList* onecell, Scalar tmp_vol){
  int number_inside = onecell->nItems();
  int counter=0;
  
  Scalar density = 0.0;
 
  if (number_inside == 0) return 0.0;

  oopicListIter<CellElement> cellIter(*onecell);

  for(cellIter.restart();!cellIter.Done();cellIter++){
    density += (cellIter()->get_pg())->get_np2c(cellIter()->get_index());
    counter++;
  }

  return density/tmp_vol;
}

Scalar Coulomb::kinetic_calculation(cellList* onecell){

  Scalar kinetic=0.0;

  oopicListIter<CellElement> cellIter(*onecell);

  for(cellIter.restart();!cellIter.Done();cellIter++){
    kinetic += (cellIter()->get_pg())->energy_eV(cellIter()->get_index());
  }
  return kinetic;
}

Scalar Coulomb::velocity_calculation(cellList* onecell){

  //number of particles inside cell
  int number_inside = onecell->nItems();

  Vector3 velocity=0.0;

  if(number_inside ==0) return 0.0;
  oopicListIter<CellElement> cellIter(*onecell);

  for(cellIter.restart();!cellIter.Done();cellIter++){
    velocity += (cellIter()->get_pg())->get_u(cellIter()->get_index());
  }

  return velocity.magnitude()/number_inside;
}

Scalar Coulomb::temperature_calculation(cellList* onecell){

  Scalar temperature = 0.0;
  int number_inside = onecell->nItems();

  if(number_inside ==0) return 0.0;

  oopicListIter<CellElement> cellIter(*onecell);

  for(cellIter.restart();!cellIter.Done();cellIter++){
    temperature += (cellIter()->get_pg())->energy_eV(cellIter()->get_index());
  }
  return temperature*2/3/Scalar(number_inside);
}

Scalar Coulomb::get_A(Scalar tau){

  Scalar tmp_A=0;

  if ( tau >= 0.01 && tau < 0.02 ) tmp_A=-50/0.01*(tau - 0.01)+100.5;
  else if ( tau >= 0.02 && tau < 0.03) tmp_A=(33.84-50.50)/0.01*(tau-0.02)+50.50;
  else if ( tau >= 0.03 && tau < 0.04) tmp_A= (25.50-33.84)/0.01*(tau-0.03)+33.84;
  else if (tau >= 0.04 && tau < 0.05) tmp_A=-5/0.01*(tau-0.04)+25.50;
  else if (tau >=0.05 && tau < 0.06)  tmp_A=(17.17-20.50)/0.01*(tau-0.05)+20.50;
  else if (tau>=0.06 && tau<0.07)   tmp_A=(14.79-17.17)/0.01*(tau-0.06)+17.17;
  else if(tau >= 0.07 && tau < 0.08)  tmp_A=(13.01-14.79)/0.01*(tau-0.07)+14.79;
  else if(tau>=0.08 && tau < 0.09)   tmp_A=(11.62-13.01)/0.01*(tau-0.08)+13.01;
  else if(tau>=0.09 && tau< 0.1)   tmp_A=(10.52-11.62)/0.01*(tau-0.09)+11.62;
  else if(tau>=0.1 && tau< 0.2)   tmp_A=(5.516-10.52)/0.1*(tau-0.1)+10.52;
  else if(tau>=0.2 && tau < 0.3)  tmp_A=(3.845-5.516)/0.1*(tau-0.2)+5.516;
  else if(tau>=0.3 && tau < 0.4)  tmp_A=(2.987-3.845)/0.1*(tau-0.3)+3.845;
  else if(tau>=0.4 && tau<0.5)  tmp_A= (2.448-2.987)/0.1*(tau-0.4)+2.987;
  else if(tau>=0.5 && tau<0.6)   tmp_A=(2.067-2.448)/0.1*(tau-0.5)+2.448;
  else if(tau>=0.6 && tau<0.7)  tmp_A= (1.779-2.067)/0.1*(tau-0.6)+2.067;
  else if(tau>=0.7 && tau<0.8) tmp_A= (1.551-1.779)/0.1*(tau-0.7)+1.779;
  else if(tau>=0.8 && tau < 0.9)  tmp_A=(1.363-1.551)/0.1*(tau-0.8)+1.551;
  else if(tau>=0.9 && tau <1)   tmp_A=(1.207-1.363)/0.1*(tau-0.9)+1.363;
  else if(tau>=1 && tau < 2)   tmp_A=(0.4105-1.207)*(tau-1)+1.207;
  else if(tau >=2 && tau<3)   tmp_A=(0.1496-0.4105)*(tau-2)+0.4105;
  else if(tau == 3 )  tmp_A=0.1496;

  return tmp_A;
}
