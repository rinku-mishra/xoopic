/*
====================================================================
PML.


====================================================================
*/


#include	"fields.h"
#include "pmlport.h"
#ifdef UNIX
#include "ptclgrp.h"
#endif

PMLPort::PMLPort(oopicList <LineSegment> *segments, Scalar _sigma, int _thickness): Port(segments)
{
	sigma_max = _sigma;
	thickness = _thickness;

	kmin = MIN(k1,k2);
	kmax = MAX(k1,k2);
	jmin = MIN(j1,j2);
	jmax = MAX(j1,j2);
	int L = abs(k2-k1);


	if (alongx2())		
	  {	
	    H_field_1 = new Vector3 *[thickness+1];
	    E_field_1 = new Vector3 *[thickness+1];
	    H_field_2 = new Vector3 *[thickness+1];
	    E_field_2 = new Vector3 *[thickness+1];

	    for(int k=0; k<=thickness; k++){
	      H_field_1[k] = new Vector3 [L+1];
	      E_field_1[k] = new Vector3 [L+1];
	      H_field_2[k] = new Vector3 [L+1];
	      E_field_2[k] = new Vector3 [L+1];
	    }	  }

	for(int j=0;j<=thickness;j++) 
	  for(int k=0;k<=L;k++) {
	    H_field_1[j][k] = Vector3(0.0, 0.0, 0.0);
	    E_field_1[j][k] = Vector3(0.0, 0.0, 0.0);
	    H_field_2[j][k] = Vector3(0.0, 0.0, 0.0);
	    E_field_2[j][k] = Vector3(0.0, 0.0, 0.0);
	  }
}
PMLPort::~PMLPort()
{
  for (int i=0; i<=thickness; i++){
    delete[] H_field_1[i];
    delete[] E_field_1[i];
    delete[] H_field_2[i];
    delete[] E_field_2[i];
  }
  
}

//--------------------------------------------------------------------
//      Apply boundary conditions to fields locally.

void PMLPort::applyFields(Scalar t,Scalar dt)
{
   if(dt==0) return;
   
   Grid* grid = fields->get_grid();		//	get pointer
   
   IntBdS = fields->getIntBdS();
   IntEdl = fields->getIntEdl();
   iL = fields->getiL();

   
   Scalar A_x, B_x, A_y, B_y, A_z, B_z;
   A_y = 1.0;
   B_y = 1.0;
   A_z = 1.0;
   B_z = 1.0;

   Scalar sigma_x = 0.0;



   if (alongx2())                      // vertical
     {
       if (get_normal() == 1)						// boundary on LHS
	 {
	   
	   //E_x calculation
	   
	   for(int index = thickness-1; index>=0;index--){
	     
	     int index_whole = index + jmin;
	     
	     for (int k=kmin+1; k<kmax; k++)
	       {
		 
		 E_field_1[index][k].set_e1(A_y*E_field_1[index][k].e1() + dt*iEPS0*B_y*(H_field_1[index][k].e3()+H_field_2[index][k].e3()-H_field_1[index][k-1].e3()-H_field_2[index][k-1].e3())/grid->dl2(index_whole,k));
		 
		 //set the electric field 
		 fields->setIntEdl(index_whole, k, 1,(E_field_1[index][k].e1() + E_field_2[index][k].e1())*grid->dl1(index_whole,k));	   

	       }
	     
	   }

	   for (int index=thickness; index>0; index--){
	     fields->setIntEdl(index+jmin, 0, 1,0.0);	   
	     fields->setIntEdl(index+jmin, kmax, 1,0.0);	   
	   }


	   //E_y calculation

	   for(int index = thickness; index>0;index--){
	     
	     int index_whole = index + jmin;
	   	    
	     for (int k=kmin; k<kmax; k++)
	       {
		
		 sigma_x = sigma_max*(Scalar(thickness-index)/Scalar(thickness));

		 A_x = (2*EPS0-sigma_x*dt)/(2*EPS0+sigma_x*dt);
		 B_x = 2*EPS0/(2*EPS0+sigma_x*dt);		

		 if(index == thickness){
		   //   E_field_1[index][k].set_e2(A_x*E_field_1[index][k].e2() - dt*iEPS0*B_x*(IntBdS[index_whole][k].e3()/grid->dS3(index_whole,k)*iL[index_whole][k].e3()-H_field_1[index-1][k].e3()-H_field_2[index-1][k].e3())/grid->dl1(index_whole-1,k));
		   E_field_1[index][k].set_e2(A_x*E_field_1[index][k].e2() - dt*iEPS0*B_x*(IntBdS[index_whole][k].e3()/grid->dS3(index_whole,k)*iMU0-H_field_1[index-1][k].e3()-H_field_2[index-1][k].e3())/grid->dl1(index_whole,k));
		 }
		 else{
		   E_field_1[index][k].set_e2(A_x*E_field_1[index][k].e2() - dt*iEPS0*B_x*(H_field_1[index][k].e3()+H_field_2[index][k].e3()-H_field_1[index-1][k].e3()-H_field_2[index-1][k].e3())/grid->dl1(index_whole-1,k));
		 }
		 //set the electric field 
		 fields->setIntEdl(index_whole, k, 2,(E_field_1[index][k].e2() + E_field_2[index][k].e2())*grid->dl2(index_whole,k));	   
	       }
	     
	   }

	   for (int k=0;k<kmax;k++){
	     E_field_1[0][k].set_e2(0.0);
	     E_field_2[0][k].set_e2(0.0);
	     fields->setIntEdl(jmin, k, 2,0.0);	   
	     }

	   //E_z calculation
	   //first component
	   
	   for(int index = thickness; index>0;index--){
	     
	     int index_whole = index + jmin;
	   	    
	     for (int k=0; k<kmax; k++)
	       {
		 sigma_x = sigma_max*(Scalar(thickness-index)/Scalar(thickness));
		 A_x = (2*EPS0-sigma_x*dt)/(2*EPS0+sigma_x*dt);
		 B_x = 2*EPS0/(2*EPS0+sigma_x*dt);
		 
		 if(index == thickness){
		   //		   E_field_1[index][k].set_e3(A_x*E_field_1[index][k].e3() - dt*iEPS0*B_x*(H_field_1[index-1][k].e2()+H_field_2[index-1][k].e2()-IntBdS[index_whole][k].e2()/grid->dS2(index_whole,k)*iL[index_whole][k].e2())/grid->dl1(index_whole-1,k));
		   E_field_1[index][k].set_e3(A_x*E_field_1[index][k].e3() - dt*iEPS0*B_x*(H_field_1[index-1][k].e2()+H_field_2[index-1][k].e2()-IntBdS[index_whole][k].e2()/grid->dS2(index_whole,k)*iMU0)/grid->dl1(index_whole-1,k));
		  
	 }
		 else{
		   E_field_1[index][k].set_e3(A_x*E_field_1[index][k].e3() - dt*iEPS0*B_x*(H_field_1[index-1][k].e2()+H_field_2[index-1][k].e2()-H_field_1[index][k].e2()-H_field_2[index][k].e2())/grid->dl1(index_whole-1,k));

		 }		
		 
	       }
	   }
	 
	   	   
	   //E_z second component
	   
	   for (int index=thickness; index>0; index--){
	     
	     int index_whole = index + jmin;
	     
	     for (int k=kmin+1; k<kmax; k++){
	       
	       E_field_2[index][k].set_e3(A_y*E_field_2[index][k].e3() - dt*iEPS0*B_y*(H_field_1[index][k].e1()+H_field_2[index][k].e1()-H_field_1[index][k-1].e1()-H_field_2[index][k-1].e1())/grid->dl2(index_whole,k-1));

	       fields->setIntEdl(index_whole, k, 3,(E_field_1[index][k].e3() + E_field_2[index][k].e3())*grid->dl3(index_whole,k));	    
	     }
	   }
	   
	   for (int k=0;k<=kmax;k++){
	     fields->setIntEdl(jmin, k, 3,0.0);	   
	   }
	   
	   for (int index=0; index<=thickness; index++){
	     fields->setIntEdl(index+jmin, 0, 3,0.0);	   
	     fields->setIntEdl(index+jmin, kmax, 3,0.0);	   
	   }
	   
	          
  
	   //H-field Calculation
	   
	   //H_x 
	   	  
	   for (int index=thickness; index>0; index--){

   	     int index_whole = index + jmin;

	     for (int k=kmin; k<kmax; k++){
	      
	       if(index==thickness){
		 H_field_1[index][k].set_e1(A_y*H_field_1[index][k].e1() - dt*iMU0*B_y*(IntEdl[index_whole][k+1].e3()/grid->dl3(index_whole,k+1) - IntEdl[index_whole][k].e3()/grid->dl3(index_whole,k))/grid->dl2(index_whole,k));
	       }
	       else{
		 H_field_1[index][k].set_e1(A_y*H_field_1[index][k].e1() - dt*iMU0*B_y*(E_field_1[index][k+1].e3()+E_field_2[index][k+1].e3() - E_field_1[index][k].e3() - E_field_2[index][k].e3())/grid->dl2(index_whole,k));
	       }
	       //	       fields->setIntBdS(index_whole, k, 1,(H_field_1[index][k].e1() + H_field_2[index][k].e1())*grid->dS1(index_whole,k)/iL[index_whole][k].e1());
	       fields->setIntBdS(index_whole, k, 1,(H_field_1[index][k].e1() + H_field_2[index][k].e1())*grid->dS1(index_whole,k)*MU0);
	     }
	     
	   }
	   
	   //H_y calculation
	   
	   for (int index = thickness-1; index >= 0 ; index--){

	     int index_whole = index + jmin;
	     
	     for (int k=kmin; k<kmax; k++){
	       
	       sigma_x = MU0/EPS0*sigma_max*((Scalar(thickness-(index+1))+0.5)/Scalar(thickness));

	       A_x = (2*MU0-sigma_x*dt)/(2*MU0+sigma_x*dt);
	       B_x = 2*MU0/(2*MU0+sigma_x*dt);
	     	      	       
	       if(index == thickness-1){
		 Scalar tmp_Ez=(IntEdl[index_whole+1][k].e3())/grid->dl3(index_whole+1,k);
		 H_field_1[index][k].set_e2(A_x*H_field_1[index][k].e2() + dt*iMU0*B_x*(tmp_Ez-E_field_1[index][k].e3()-E_field_2[index][k].e3())/grid->dl1(index_whole,k));
	       }
	       else{
		 H_field_1[index][k].set_e2(A_x*H_field_1[index][k].e2() + dt*iMU0*B_x*(E_field_1[index+1][k].e3()+E_field_2[index+1][k].e3() - E_field_1[index][k].e3() - E_field_2[index][k].e3())/grid->dl1(index_whole,k));
	       }
	     
	       //	       fields->setIntBdS(index_whole, k, 2,(H_field_1[index][k].e2() + H_field_2[index][k].e2())*grid->dS2(index_whole,k)/iL[index_whole][k].e2());	   
	       fields->setIntBdS(index_whole, k, 2,(H_field_1[index][k].e2() + H_field_2[index][k].e2())*grid->dS2(index_whole,k)*MU0);	   
	     }
	   
	     //Ez is zero along upper and lower boundary
	     fields->setIntBdS(index_whole, kmax, 2,0.0);	   
	          
	     
	   }
	   


	   //H_z calculation
	   //firstcomponent
	   for (int index=thickness-1; index>=0; index--){
	     
	     int index_whole = index + jmin;
	     
	     for (int k=kmin; k<kmax; k++){
	     	       
	       sigma_x = MU0/EPS0*sigma_max*((Scalar(thickness-(index+1))+0.5)/Scalar(thickness));
	       A_x = (2*MU0-sigma_x*dt)/(2*MU0+sigma_x*dt);
	       B_x = 2*MU0/(2*MU0+sigma_x*dt);
	       
	       if(index == thickness-1){

		 Scalar tmp_Ey=(IntEdl[index_whole+1][k].e2())/grid->dl2(index_whole,k);
		
		 H_field_1[index][k].set_e3(A_x*H_field_1[index][k].e3() - dt*iMU0*B_x*(tmp_Ey-E_field_1[index][k].e2()-E_field_2[index][k].e2())/grid->dl1(index_whole,k));
	       }
	       
	       else{
		 H_field_1[index][k].set_e3(A_x*H_field_1[index][k].e3() - dt*iMU0*B_x*(E_field_1[index+1][k].e2()+E_field_2[index+1][k].e2() - E_field_1[index][k].e2() - E_field_2[index][k].e2())/grid->dl1(index_whole,k));
	       }
	       
	     }
	   }
	   
	   //second component
	   for (int index=thickness-1; index >= 0; index--){
	     int index_whole = index + jmin;
	     for (int k=0; k<kmax;k++){
	   
	       H_field_2[index][k].set_e3(A_y*H_field_2[index][k].e3() + dt*iMU0*B_y*(E_field_1[index][k+1].e1()+E_field_2[index][k+1].e1() - E_field_1[index][k].e1() - E_field_2[index][k].e1())/grid->dl2(index_whole,k));
	       
	       //	       fields->setIntBdS(index_whole, k, 3,(H_field_1[index][k].e3()+H_field_2[index][k].e3())*grid->dS3(index_whole,k)/iL[index_whole][k].e3());	   
	       fields->setIntBdS(index_whole, k, 3,(H_field_1[index][k].e3()+H_field_2[index][k].e3())*grid->dS3(index_whole,k)*MU0);	   
	     }
	     
	   }
	   
	 }
       
       else  // Boundary on RHS;
	 {

	   //E_x calculation

	   for(int index = 0; index<thickness;index++){
	     
	     int index_whole = index + jmax - thickness;
	     
	     for (int k=kmin+1; k<kmax; k++)
	       {
		 E_field_1[index][k].set_e1(A_y*E_field_1[index][k].e1() + dt*iEPS0*B_y*(H_field_1[index][k].e3()+H_field_2[index][k].e3()-H_field_1[index][k-1].e3()-H_field_2[index][k-1].e3())/grid->dl2(index_whole,k));
		 //set the electric field 
		 fields->setIntEdl(index_whole, k, 1,(E_field_1[index][k].e1() + E_field_2[index][k].e1())*grid->dl1(index_whole,k));	   
	       }
	   }

	   for (int index=0; index<thickness; index++){
	     fields->setIntEdl(index+jmax-thickness, 0, 1,0.0);	   
	     fields->setIntEdl(index+jmax-thickness, kmax, 1,0.0);	   
	   }

	   
	   //E_y calculation

	   for(int index = 0; index<thickness;index++){
	     
	     int index_whole = index + jmax - thickness;
	     
	     for (int k=kmin; k<kmax; k++)
	       {	      
		 
		 sigma_x = sigma_max*(Scalar(index)/Scalar(thickness));
		 A_x = (2*EPS0-sigma_x*dt)/(2*EPS0+sigma_x*dt);
		 B_x = 2*EPS0/(2*EPS0+sigma_x*dt);		
		
		 if(index == 0){
		   E_field_1[index][k].set_e2(A_x*E_field_1[index][k].e2() - dt*iEPS0*B_x*(H_field_1[index][k].e3()+H_field_2[index][k].e3()-(IntBdS[index_whole-1][k].e3()/grid->dS3(index_whole-1,k)*iMU0))/grid->dl1(index_whole,k));
		   //		   E_field_1[index][k].set_e2(A_x*E_field_1[index][k].e2() - dt*iEPS0*B_x*(H_field_1[index][k].e3()+H_field_2[index][k].e3()-(IntBdS[index_whole-1][k].e3()/grid->dS3(index_whole-1,k)*iL[index_whole-1][k].e3()))/grid->dl1(index_whole,k));
		 }
		 else{
		   E_field_1[index][k].set_e2(A_x*E_field_1[index][k].e2() - dt*iEPS0*B_x*(H_field_1[index][k].e3()+H_field_2[index][k].e3()-H_field_1[index-1][k].e3()-H_field_2[index-1][k].e3())/grid->dl1(index_whole,k)); 
		 }
		 //set the electric field 
		 fields->setIntEdl(index_whole, k, 2,(E_field_1[index][k].e2() + E_field_2[index][k].e2())*grid->dl2(index_whole,k));
	       }
	   }
	     
	   
	   for (int k=0;k<kmax;k++){
	     E_field_1[thickness][k].set_e2(0.0);
	     fields->setIntEdl(jmax, k, 2,0.0);	   
	   }
	  
	   //E_z calculation
	   //first component
	   
	   for(int index = 0; index<thickness;index++){
	     
	     int index_whole = index + jmax - thickness;
	     
	     for (int k=0; k<kmax; k++)
	       {
		 
		 sigma_x = sigma_max*(Scalar(index)/Scalar(thickness));
		 
		 A_x = (2*EPS0-sigma_x*dt)/(2*EPS0+sigma_x*dt);
		 B_x = 2*EPS0/(2*EPS0+sigma_x*dt);
				 
		 if(index==0){
		   //		   E_field_1[index][k].set_e3(A_x*E_field_1[index][k].e3() + dt*iEPS0*B_x*(H_field_1[index][k].e2()+H_field_2[index][k].e2()-IntBdS[index_whole-1][k].e2()/grid->dS2(index_whole-1,k)*iL[index_whole-1][k].e2())/grid->dl1(index_whole,k));
		   E_field_1[index][k].set_e3(A_x*E_field_1[index][k].e3() + dt*iEPS0*B_x*(H_field_1[index][k].e2()+H_field_2[index][k].e2()-IntBdS[index_whole-1][k].e2()/grid->dS2(index_whole-1,k)*iMU0)/grid->dl1(index_whole,k));
		 }
		 else{
		   E_field_1[index][k].set_e3(A_x*E_field_1[index][k].e3() + dt*iEPS0*B_x*(H_field_1[index][k].e2()+H_field_2[index][k].e2()-H_field_1[index-1][k].e2()-H_field_2[index-1][k].e2())/grid->dl1(index_whole,k));
		 }		
		 
	       }
	   }
	   
	   	   
	   //E_z second component
	   
	   for (int index=0; index<thickness; index++){
	     
	     int index_whole = index + jmax - thickness;
	     
	     for (int k=kmin+1; k<kmax; k++){
	       
	       E_field_2[index][k].set_e3(A_y*E_field_2[index][k].e3() - dt*iEPS0*B_y*(H_field_1[index][k].e1()+H_field_2[index][k].e1()-H_field_1[index][k-1].e1()-H_field_2[index][k-1].e1())/grid->dl2(index_whole,k-1));
	       
	       fields->setIntEdl(index_whole, k, 3,(E_field_1[index][k].e3() + E_field_2[index][k].e3())*grid->dl3(index_whole,k));	    
	     }
	   }
	   
	   for (int k=0;k<=kmax;k++){
	     fields->setIntEdl(jmax, k, 3,0.0);	   
	   }
	   
	   for (int index=0; index<=thickness; index++){
	     fields->setIntEdl(index+jmax-thickness, 0, 3,0.0);	   
	     fields->setIntEdl(index+jmax-thickness, kmax, 3,0.0);	   
	   }
	   
	          
  
	   //H-field Calculation
	   
	   //H_x 
	   	  
	   for (int index=0; index<thickness; index++){

   	     int index_whole = index + jmax - thickness;
	     for (int k=kmin; k<kmax; k++){
	       
	       if(index==0){
		 H_field_1[0][k].set_e1(A_y*H_field_1[0][k].e1() - dt*iMU0*B_y*(IntEdl[index_whole][k+1].e3()/grid->dl3(index_whole,k+1) - IntEdl[index_whole][k].e3()/grid->dl3(index_whole,k))/grid->dl2(index_whole,k));
	       }
	       else{
		 H_field_1[index][k].set_e1(A_y*H_field_1[index][k].e1() - dt*iMU0*B_y*(E_field_1[index][k+1].e3()+E_field_2[index][k+1].e3() - E_field_1[index][k].e3() - E_field_2[index][k].e3())/grid->dl2(index_whole,k));
	       }
	      
	       //	       fields->setIntBdS(index_whole, k, 1,(H_field_1[index][k].e1() + H_field_2[index][k].e1())*grid->dS1(index_whole,k)/iL[index_whole][k].e1());
	       fields->setIntBdS(index_whole, k, 1,(H_field_1[index][k].e1() + H_field_2[index][k].e1())*grid->dS1(index_whole,k)*MU0);
	      
	     }
	     
	   }
	   
	   
	   //H_y calculation
	   
	   for (int index=0; index<thickness; index++){
	     int index_whole = index + jmax - thickness;
	     
	     for (int k=kmin; k<kmax; k++){
	       
	       sigma_x = MU0/EPS0*sigma_max*((Scalar(index)+0.5)/Scalar(thickness));
	       
	       A_x = (2*MU0-sigma_x*dt)/(2*MU0+sigma_x*dt);
	       B_x = 2*MU0/(2*MU0+sigma_x*dt);
	       
	       
	       if(index == 0){
		 Scalar tmp_Ez=(IntEdl[index_whole][k].e3())/grid->dl3(index_whole-1,k);
		 H_field_1[index][k].set_e2(A_x*H_field_1[index][k].e2() + dt*iMU0*B_x*(E_field_1[index+1][k].e3()+E_field_2[index+1][k].e3() - tmp_Ez)/grid->dl1(index_whole-1,k));
	       }
	       else{
		 H_field_1[index][k].set_e2(A_x*H_field_1[index][k].e2() + dt*iMU0*B_x*(E_field_1[index+1][k].e3()+E_field_2[index+1][k].e3() - E_field_1[index][k].e3() - E_field_2[index][k].e3())/grid->dl1(index_whole,k));
	       }
	       
	       //	       fields->setIntBdS(index_whole, k, 2,(H_field_1[index][k].e2() + H_field_2[index][k].e2())*grid->dS2(index_whole,k)/iL[index_whole][k].e2());	   
	       fields->setIntBdS(index_whole, k, 2,(H_field_1[index][k].e2() + H_field_2[index][k].e2())*grid->dS2(index_whole,k)*MU0);	   
	     }
	     //	     fields->setIntBdS(index_whole, kmax, 2,0.0);	   
	     
	     
	   }
	   

	   
	   //H_z calculation
	   //first component
	   for (int index=0; index<thickness; index++){
	     
	     int index_whole = index + jmax - thickness;
	     
	     for (int k=kmin; k<kmax; k++){
	       
	       sigma_x = MU0/EPS0*sigma_max*((Scalar(index)+0.5)/Scalar(thickness));
	       
	       A_x = (2*MU0-sigma_x*dt)/(2*MU0+sigma_x*dt);
	       B_x = 2*MU0/(2*MU0+sigma_x*dt);
	       
	       if(index == 0){
		 Scalar tmp_Ey=(IntEdl[index_whole][k].e2())/grid->dl2(index_whole-1,k);

		 H_field_1[index][k].set_e3(A_x*H_field_1[index][k].e3() - dt*iMU0*B_x*(E_field_1[index+1][k].e2()+E_field_2[index+1][k].e2() - tmp_Ey)/grid->dl1(index_whole-1,k));
	       }
	       else{
		 H_field_1[index][k].set_e3(A_x*H_field_1[index][k].e3() - dt*iMU0*B_x*(E_field_1[index+1][k].e2()+E_field_2[index+1][k].e2() - E_field_1[index][k].e2() - E_field_2[index][k].e2())/grid->dl1(index_whole-1,k));
	       }
	       
	     }
	   }
	   
	   
	   //second component
	   for (int index=0; index<thickness; index++){
	     int index_whole = index + jmax - thickness;
	     for (int k=kmin; k<kmax;k++){
	       
	       H_field_2[index][k].set_e3(A_y*H_field_2[index][k].e3() + dt*iMU0*B_y*(E_field_1[index][k+1].e1()+E_field_2[index][k+1].e1() - E_field_1[index][k].e1() - E_field_2[index][k].e1())/grid->dl2(index_whole,k));
	       
	       // fields->setIntBdS(index_whole, k, 3,(H_field_1[index][k].e3()+H_field_2[index][k].e3())*grid->dS3(index_whole,k)/iL[index_whole][k].e3());	   
	       fields->setIntBdS(index_whole, k, 3,(H_field_1[index][k].e3()+H_field_2[index][k].e3())*grid->dS3(index_whole,k)*MU0);	   
	       
	     }
	   }  
	   
	   
	 }
     }
   else											// horizontal
     {
       if (k1==0)
	 {
	   for (int j=jmin; j<jmax; j++)
	     {
	       
	     }
	 }
       else										// outer boundary
	 {
	   for (int j=jmin; j<jmax; j++)
	     {
	     }
	 }
     }
   //tOld = t;
}
