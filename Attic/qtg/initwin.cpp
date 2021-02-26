
#include "initwin.h"

void Diagnostics::InitWin(void) {


  //  We'll put the diagnostic calls to xgrafix here.

  jm = theSpace->getJ()+1;
  km = theSpace->getK()+1;
  km2 = km/2+1;

	ostring prepend="";
  int i,j, isp;
	if(RegionName != (ostring)"Noname" && RegionName != (ostring)"") 
		prepend = RegionName + (ostring)": ";


  switch(theSpace->get_grid()->query_geometry())
	 {
	 case ZRGEOM:
		{
		  X1="z";
		  X2="r";
		  X3="phi";
		  E1= prepend + "Ez";
		  E2= prepend + "Er";
		  E3= prepend + "Ephi";
		  B1= prepend + "Bz";
		  B2= prepend + "Br";
		  B3= prepend + "Bphi";
		  I1= prepend + "Iz";
		  I2= prepend + "Ir";
		  I3= prepend + "Iphi";
		  break;
		}
	 case ZXGEOM:
		{
		  X1="x";
		  X2="y";
		  X3="z";
		  E1= prepend + "Ex";
		  E2= prepend + "Ey";
		  E3= prepend + "Ez";
		  B1= prepend + "Bx";
		  B2= prepend + "By";
		  B3= prepend + "Bz";
		  I1= prepend + "Ix";
		  I2= prepend + "Iy";
		  I3= prepend + "Iz";
		  break;
		}
	 };			
  char buf1[100];
  sprintf(buf1,"u%s vs %s, all species",X1.c_str(),X1.c_str());
  U1vsX1 =   prepend + buf1;
  sprintf(buf1,"u%s vs %s, all species",X2.c_str(),X1.c_str());
  U2vsX1 =   prepend + buf1;
  sprintf(buf1,"u%s vs %s, all species",X3.c_str(),X1.c_str());
  U3vsX1 =   prepend + buf1;
  sprintf(buf1,"u%s vs %s, all species",X1.c_str(),X2.c_str());
  U1vsX2=   prepend + buf1;
  sprintf(buf1,"u%s vs %s, all species",X2.c_str(),X2.c_str());
  U2vsX2=   prepend + buf1;
  sprintf(buf1,"u%s vs %s, all species",X3.c_str(),X2.c_str());
  U3vsX2=   prepend + buf1;

#ifndef NOX
  //  Set up the diagnostics 
  //----------------------------------------------------------------------//
  // Set up the new diagnostics

  DiagList *dlist = theSpace->getDiagList();
	oopicListIter<Diag> nextd(*dlist);
  for(nextd.restart();!nextd.Done(); nextd++) 
		nextd.current()->initwin();

  //----------------------------------------------------------------------//
  //set up electric fields
  XGSet3D( "linlinlin",X1,X2,E1,45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)E, &jm, &km, 1 ,1,0,1,0,3,0);

  XGSet3D( "linlinlin",X1,X2,E2,45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)E, &jm, &km, 1 ,1,0,1,0,3,1);

  XGSet3D( "linlinlin",X1,X2,E3,45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)E, &jm, &km, 1 ,1,0,1,0,3,2);

  //set up magnetic fields

  XGSet3D( "linlinlin",X1,X2,B1,45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)B, &jm, &km, 1,1,0,1,0,3,0 );

  XGSet3D( "linlinlin",X1,X2,B2,45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)B, &jm, &km, 1,1,0,1,0,3,1 );

  XGSet3D( "linlinlin",X1,X2,B3,45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurfVector( x1_array,x2_array,(Scalar **)B, &jm, &km, 1,1,0,1,0,3,2 );

  //set up currents
  if(!electrostaticFlag) {
	 XGSet3D( "linlinlin",X1,X2,I1,45.0,225.0,"closed",1,1,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	 XGSurfVector( x1_array,x2_array,(Scalar **)I, &jm, &km, 1,1,0,1,0,3,0 );

	 XGSet3D( "linlinlin",X1,X2,I2,45.0,225.0,"closed",1,1,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	 XGSurfVector( x1_array,x2_array,(Scalar **)I, &jm, &km, 1,1,0,1,0,3,1 );

	 XGSet3D( "linlinlin",X1,X2,I3,45.0,225.0,"closed",1,1,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	 XGSurfVector( x1_array,x2_array,(Scalar **)I, &jm, &km, 1,1,0,1,0,3,2 );
  }
  //set up energies

  XGSet3D( "linlinlin",X1,X2,strdup((prepend +"Ue")),45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurf( x1_array,x2_array,Ue, &jm, &km, 1 );

  XGSet3D( "linlinlin",X1,X2,strdup((prepend +"Ub")),45.0,225.0,"closed",1,1,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurf( x1_array,x2_array,Ub, &jm, &km, 2 );

  // Set up poynting vector
  if(!electrostaticFlag)
	 {
		XGSetVec("vecvec",X1,X2,strdup(prepend+"Poynting Vector"),"closed",1,1,1.0
					,1.0,0,0,x1min,x1max,x2min,x2max);

		XGVectorVector(x1_array,x2_array,(Scalar **)S_array,
							(Scalar **)S_array,&jm,&km,2,1,0,1,0,3,0,3,1);

	 }


  //  set up rho
	char buffer[100];
	if (boltzmannFlag)
		sprintf(buffer,"PIC ");
	else 
		sprintf(buffer,"%s","");

  XGSet3D( "linlinlin",X1,X2,strdup(prepend + buffer + "rho"),45.0,250.0,"closed",1,400,
			 1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
  XGSurf( x1_array,x2_array,rho, &jm, &km, 3 );

  //  set up boltzmann rho
  if (boltzmannFlag)
	 {
/*		XGSet3D( "linlinlin",X1,X2,strdup(prepend +"Boltzmann Rho"),45.0,250.0,"closed",1,1,
				  1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
		XGSurf( x1_array,x2_array,boltzmannRho, &jm, &km, 3 );
*/
		XGSet3D( "linlinlin",X1,X2,strdup(prepend +"Total Rho"),45.0,250.0,"closed",1,1,
				  1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
		XGSurf( x1_array,x2_array,totalRho, &jm, &km, 3 );
	 }
  if(electrostaticFlag) {
	 //  set up phi
	 XGSet3D( "linlinlin",X1,X2,strdup(prepend +"phi"),45.0,250.0,"closed",1,1,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	 XGSurf( x1_array,x2_array,phi, &jm, &km, 3 );
  }
  //  set up divderror
  if(!electrostaticFlag) {
	 XGSet3D( "linlinlin",X1,X2,strdup(prepend +"Divergence Error"),45.0,250.0,"closed",1,1,
				1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
	 XGSurf( x1_array,x2_array,divderror, &jm, &km, 3 );
  }

  //  The phase space diagnostics
  //	X2 vs X1

  for(isp=0;isp<number_of_species;isp++) {
	 char * buf = new char[80];
	 sprintf(buf,"%s-%s phase space for %s ",X1.c_str(),X2.c_str(),theSpecies[isp].name);

	 XGSet2D( "linlin", X1,strdup(prepend +buf),"open"
				,700,400,1,1,0,0,x1min,x1max,x2min,x2max);

	 XGScat2D(theSpecies[isp].x1positions,theSpecies[isp].x2positions,
				 &(theSpecies[isp].nparticles_plot),isp);


	 //draw any boundaries or structures
	oopicListIter<Boundary> nextb(*blist);
	 for(nextb.restart();!nextb.Done(); nextb++) {
		Scalar x1,x2,y1,y2;
		int j1,j2,k1,k2;
		oopicListIter <LineSegment> lsI(*nextb.current()->getSegmentList());
		for(lsI.restart();!lsI.Done();lsI++) {
		  int j1,j2,k1,k2,normal,*points;  //local copies of above
		  j1=(int)lsI.current()->A.e1();
		  k1=(int)lsI.current()->A.e2();
		  j2=(int)lsI.current()->B.e1();
		  k2=(int)lsI.current()->B.e2();
		  points=lsI.current()->points;
		  normal=lsI.current()->normal;

		  x1 = x1_array[j1]; 
		  x2 = x1_array[j2];
		  y1 = x2_array[k1];
		  y2 = x2_array[k2];
		  int bcolor = nextb.current()->getBoundaryType();
		  
		  // Different sorts of boundaries need to be drawn differently
		  switch(bcolor) {
		  case DIELECTRIC_TRIANGLE_UP: // positive-normal DielectricTriangle
			 {				 
				Scalar * fpoints = (Scalar *) calloc(4*abs(j2-j1+3),sizeof(Scalar));
				int j;
				for(j=0;j<4*(j2-j1)+4;j+=2)
				  { fpoints[j] = x1_array[points[j]];
					 fpoints[j+1] = x2_array[points[j+1]];
				  }
				fpoints[j] = (y1 < y2)? x1 :x2;
				fpoints[j+1] = max(y1,y2);
				fpoints[j+2]= fpoints[0];
				fpoints[j+3]= fpoints[1];
				XGStructureArray(2*(j2-j1)+4,FILLED,bcolor - 100, bcolor -100,
									  fpoints);
				
				delete [] points;
				free(fpoints);
				
				break;
			 }
		  case DIELECTRIC_TRIANGLE_DOWN: //negative-normal DielectricTriangle
			 {
				Scalar * fpoints = (Scalar *) calloc(4*abs(j2-j1+3),sizeof(Scalar));
				int *points = segmentate(j1,k1,j2,k2);
				int j;
				for(j=0;j<4*(j2-j1)+4;j+=2)
				  { fpoints[j] = x1_array[points[j]];
					 fpoints[j+1] = x2_array[points[j+1]];
				  }
				fpoints[j] = (y1 < y2)? x2 :x1;
				fpoints[j+1] = min(y1,y2);
				fpoints[j+2]= fpoints[0];
				fpoints[j+3]= fpoints[1];
				XGStructureArray(2*(j2-j1)+4,FILLED,-(bcolor + 100), -(bcolor +100),
									  fpoints);

				delete [] points;
				free(fpoints);
				break;
			 }
		  case DIELECTRIC_REGION:  //filled square
			 {
				XGStructure(5,FILLED,-bcolor,
								-bcolor,
								x1,y1,
								x1,y2,
								x2,y2,
								x2,y1,
								x1,y1);
				break;
			 }
		  case CURRENT_REGION:  //filled square
			 {
				XGStructure(5,FILLED,-bcolor,
								-bcolor,
								x1,y1,
								x1,y2,
								x2,y2,
								x2,y1,
								x1,y1);
				break;
			 }
		  case ILOOP: // Hollow square
			 {
				XGStructure(5,HOLLOW,-bcolor,
								-bcolor,
								x1,y1,
								x1,y2,
								x2,y2,
								x2,y1,
								x1,y1);
				break;
			 }
		 case PERIODIC:
			 {
				 //do nothing
				 break;
			 }
		  default: {   // simple or oblique line
			 if(x1==x2 || y1==y2) {
				Scalar xa,ya;
				xa = (x1+x2)/2.0;
				ya = (y1+y2)/2.0;
				XGStructure(2,HOLLOW,bcolor,
								bcolor,
								x1,y1,
								x2,y2);
				XGStructure(2,HOLLOW,bcolor,bcolor,xa,ya,
								((x1==x2)?xa+(normal*(x1_array[1]-x1_array[0]))/4.0 :xa),
								((y1==y2)?ya+(normal*(x2_array[1]-x2_array[0]))/4.0 :ya));
			 }
			 else
				{  
				  int *points = segmentate(j1,k1,j2,k2);
				  int j,k, jl,kl,jh,kh;
				  /* start with the second point, advance one point at a time */
				  for(j=2;j<4*abs(j2-j1)+4;j+=2) {
					 jl=points[j-2];
					 kl=points[j-1];
					 jh=points[j];
					 kh=points[j+1];
					 XGStructure(2,HOLLOW,bcolor,bcolor,x1_array[jl],x2_array[kl],
									 x1_array[jh],x2_array[kh]);
				  }
				  delete[] points;
				  
				}
			 break;
		  }
		  }
		}
	 }

  }

  // 	U1 vs X1

  XGSet2D( "linlin", X1,U1vsX1,"closed",700,400,1,1,0,1,x1min,x1max,x2min,x2max);
  // XGSet2D( "linlin", X1,U1vsX1,"open",700,400,1,1,0,1,x1min,x1max,x2min,x2max);

  for(i=0;i<number_of_species;i++)
	 XGScat2D(theSpecies[i].x1positions,theSpecies[i].x1velocities,
				 &(theSpecies[i].nparticles_plot),i);

  //	U2 vs X1
  XGSet2D( "linlin", X1,U2vsX1,"closed",700,400,1,1,0,1,x1min,x1max,x2min,x2max);

  for(i=0;i<number_of_species;i++)
	 XGScat2D(theSpecies[i].x1positions,theSpecies[i].x2velocities,
				 &(theSpecies[i].nparticles_plot),i);

  //	U3 vs X1
  XGSet2D( "linlin", X1,U3vsX1,"closed",700,400,1,1,0,1,x1min,x1max,x2min,x2max);

  for(i=0;i<number_of_species;i++)
	 XGScat2D(theSpecies[i].x1positions,theSpecies[i].x3velocities,
				 &(theSpecies[i].nparticles_plot),i);

  //	U1 vs X2
   XGSet2D( "linlin", X2,U1vsX2,"closed",700,400,1,1,0,1,x2min,x2max,x2min,x2max);

  for(i=0;i<number_of_species;i++)
	 XGScat2D(theSpecies[i].x2positions,theSpecies[i].x1velocities,
				 &(theSpecies[i].nparticles_plot),i);

  //	U2 vs X2
   XGSet2D( "linlin", X2,U2vsX2,"closed",700,400,1,1,0,1,x2min,x2max,x2min,x2max);

  for(i=0;i<number_of_species;i++)
	 XGScat2D(theSpecies[i].x2positions,theSpecies[i].x2velocities,
				 &(theSpecies[i].nparticles_plot),i);

  //	U3 vs X2
   XGSet2D( "linlin", X2,U3vsX2,"closed",700,400,1,1,0,1,x2min,x2max,x2min,x2max);

  for(i=0;i<number_of_species;i++)
	 XGScat2D(theSpecies[i].x2positions,theSpecies[i].x3velocities,
				 &(theSpecies[i].nparticles_plot),i);


  //  vector plot Er,Ez

  XGSetVec("vecvec",X1,X2,strdup(prepend +"E"),"closed",1,1,1.0,1.0,0,0,x1min,x1max,x2min,x2max);

  XGVectorVector(x1_array,x2_array,(Scalar **)E,(Scalar **)E,&jm,&km,2,1,0,1,0,3,0,3,1);

  //  vector plot Br,Bz

  XGSetVec("vecvec",X1,X2,strdup(prepend +"B"),"closed",1,1,1.0,1.0,0,0,x1min,x1max,x2min,x2max);

  XGVectorVector(x1_array,x2_array,(Scalar **)B,(Scalar **)B,&jm,&km,2,1,0,1,0,3,0,3,1);

  //  vector plot Ir,Iz
  if(!electrostaticFlag) {
	 XGSetVec("vecvec",X1,X2,strdup(prepend +"I"),"closed",1,1,1.0,1.0,0,0,x1min,x1max,x2min,x2max);

	 XGVectorVector(x1_array,x2_array,(Scalar **)I,(Scalar **)I,&jm,&km,2,1,0,1,0,3,0,3,1);
  }


  //  Time domain plots

  //  boundary current history diagnostics
  oopicListIter<Ihistdiag> nextdiag(*BoundaryIhist);
  
  for(nextdiag.restart();!nextdiag.Done();nextdiag++)
		{
			char *buf = new char[80];
			sprintf(buf,"I history for boundary %s",nextdiag.current()->name);
			 XGSet2D("linlin","Time",strdup(prepend+ buf),"closed",800,200,1.0,1.0,True,True,0,0,0.0,0.0);
			XGCurve(nextdiag.current()->Ihist->get_time_array(),nextdiag.current()->Ihist->get_data(),nextdiag.current()->Ihist->get_hi(),1);
		}

  oopicListIter<PFhistdiag> nextdiag2(*BoundaryPFhist);
  
  for(nextdiag2.restart();!nextdiag2.Done();nextdiag2++)
		{
			char *buf = new char[80];
			sprintf(buf,"%sPoynting history for %s",prepend.c_str(),nextdiag2.current()->name);
			 XGSet2D("linlin","Time",buf,"closed",800,200,1.0,1.0,True,True,0,0,0.0,0.0);
			XGCurve(nextdiag2.current()->PFhist->get_time_array(),nextdiag2.current()->PFhist->get_data(),nextdiag2.current()->PFhist->get_hi(),1);
			sprintf(buf,"%sPoynting Local history for %s",prepend.c_str(),nextdiag2.current()->name);
			 XGSet2D("linlin","Time",buf,"closed",800,200,1.0,1.0,True,True,0,0,0.0,0.0);
			XGCurve(nextdiag2.current()->PFlocal->get_time_array(),nextdiag2.current()->PFlocal->get_data(),nextdiag2.current()->PFlocal->get_hi(),1);
		}

   XGSet2D("linlog", "Time",strdup(prepend + "TE-beige KE-green Efield-yellow Bfield-Orange"),
			 "closed", 800, 200, 1.0, 1.0, True, True,0,0,0.0,0.0); 
  XGCurve(energy_all.get_time_array(), energy_all.get_data(), energy_all.get_hi(), 1);
  XGCurve(energy_k.get_time_array(), energy_k.get_data(), energy_k.get_hi(), 2);
  XGCurve(energy_e.get_time_array(), energy_e.get_data(), energy_e.get_hi(), 3);
  XGCurve(energy_b.get_time_array(), energy_b.get_data(), energy_b.get_hi(), 4);

  if(!electrostaticFlag) {
	  XGSet2D("linlin","Time",strdup(prepend + "Average magnitude of error in Div(D) at t"),
				"closed", 800, 200, 1.0, 1.0, True, True,0,0,0.0,0.0);
	 XGCurve(divderrorhis.get_time_array(), divderrorhis.get_data(), divderrorhis.get_hi(),0);
  }

   XGSet2D("linlin", "Time",strdup(prepend + "number(t)"), "closed", 700, 400,
			 1.0, 1.0, True, True,0,0,0.0,0.0);
  for(i=0;i<number_of_species;i++) {
	 XGCurve(number[i]->get_time_array(), number[i]->get_data(),number[i]->get_hi(), i);
	 //	  XGCurve(ngroups[i]->get_time_array(), ngroups[i]->get_data(),ngroups[i]->get_hi(), i);

  }

   // XGSet2D("linlin", "Time",strdup(prepend + "Kinetic Energy(t)"), "open", 700, 400,
   XGSet2D("linlin", "Time",strdup(prepend + "Kinetic Energy(t)"), "closed", 700, 400,
			 1.0, 1.0, True, True,0,0,0.0,0.0);
  for(i=0;i<number_of_species;i++) 
		XGCurve(ke_species[i]->get_time_array(), ke_species[i]->get_data(),ke_species[i]->get_hi(), i);
	
  XGCurve(energy_k.get_time_array(), energy_k.get_data(), energy_k.get_hi(), i+1);

   // XGSet2D("linlin", "Time",strdup(prepend + "total density (t)"), "open", 700, 400,
   XGSet2D("linlin", "Time",strdup(prepend + "total density (t)"), "closed", 700, 400,
			 1.0, 1.0, True, True,0,0,0.0,0.0);

  for(i=0;i<number_of_species;i++) 
	 XGCurve(total_density[i]->get_time_array(), total_density[i]->get_data(), total_density[i]->get_hi(), i);

   XGSet2D("linlin", "Time",strdup(prepend + "Ave KE(t)"), "closed", 700, 400,
			 1.0, 1.0, True, True,0,0,0.0,0.0);
  for(i=0;i<number_of_species;i++) 
	 XGCurve(Ave_KE[i]->get_time_array(), Ave_KE[i]->get_data(), Ave_KE[i]->get_hi(), i);

  //  set u the special button

  SetUpNewVar(&PhaseSpacePlots,"Phase Plots\0","float");
  SetUpNewVar(&AllDiagnostics,"All Diags\0","float");
  SetUpNewVar(&EnerPlots,"Energy Diags\0","float");

  //give us nice species diagnostics.
  for(i=0;i<number_of_species;i++) 
	 {
		char * buf = new char[80];
		sprintf(buf,"%sNumber density for %s ",(char *)(prepend), theSpecies[i].name);

		XGSet3D( "linlinlin",X1,X2,buf,45.0,225.0,"closed",1,1,
				  1.0,1.0,1.0/theSpecies[i].spec->get_q(),0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
		XGSurf(x1_array, x2_array, rho_species[i], &jm, &km, 3 );
	 }


  for(i=0;i<number_of_species;i++) 
	 {
		if (Show_loaded_densityFlag)
		  {
			 char * buf = new char[80];
			 sprintf(buf,"%sInitial number density for %s ",prepend.c_str(),theSpecies[i].name);
			 
			 XGSet3D( "linlinlin",X1,X2,buf,45.0,225.0,"closed",1,1,
						1.0,1.0,1.0,0,0,1,x1min,x1max,x2min,x2max,0.0,1.0);
			 XGSurf(x1_array,x2_array,loaded_density[i], &jm, &km, 3 );
		  }
	 }

  /*  Boundary diagnostics */
  oopicListIter<Boundary> nextb(*blist);
  for(nextb.restart();!nextb.Done(); nextb++) {
	 Boundary *B = nextb.current();
	 PDistCol *PD = B->get_particle_diag();
	 if (PD) {
		char *buf = new char[80];
		ostring bid = B->getBoundaryName();
		sprintf(buf,"%sf(x) on boundary %s", prepend.c_str(), bid.c_str());
		 XGSet2D("linlin", "x", buf, "closed", 200, 200,
				  1.0, 1.0, False, True, PD->get_x_MIN(), PD->get_x_MAX(), 0.0, 0.0);
		for(i=0;i<number_of_species;i++)
		  XGCurve(PD->get_x(), PD->get_fxdata(i), PD->get_nxpoints(), i);
		if (*PD->get_nepoints()) {
		  for (i=0; i<number_of_species; i++){
			 sprintf(buf, "%sf(x,energy) for %s on boundary %s", prepend.c_str(), 
						theSpecies[i].name, B->getBoundaryName().c_str());
			 XGSet3D("linlinlin", "x", "energy", buf, 45, 225, "closed", 1, 1,
						1, 1, 1, 0, 0, 1, PD->get_x_MIN(), PD->get_x_MAX(), 
						PD->get_energy_MIN(), PD->get_energy_MAX(), 0, 1);
			 XGSurf(PD->get_x(), PD->get_energy(), PD->get_fedata(i), PD->get_nxpoints(),
					  PD->get_nepoints(), i);
		  }
		}
		if (*PD->get_ntpoints()){
		  for (i=0; i<number_of_species; i++){
			 sprintf(buf, "%sf(x,theta) for %s on boundary %s", prepend.c_str(), 
						theSpecies[i].name, B->getBoundaryName().c_str());
			 XGSet3D("linlinlin", "x", "theta", buf, 45, 225, "closed", 1, 1,
						1, 1, 1, 0, 0, 1, PD->get_x_MIN(), PD->get_x_MAX(), 
						PD->get_theta_MIN(), PD->get_theta_MAX(), 0, 1);
			 XGSurf(PD->get_x(), PD->get_theta(), PD->get_ftdata(i), PD->get_nxpoints(),
					  PD->get_ntpoints(), i);
		  }
		}
// add theta and energy plots here
	 }
  }
#endif  // NOX
}

// #endif // UNIX
