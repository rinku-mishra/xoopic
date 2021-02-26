#ifdef PETSC
#define MPICH_SKIP_MPICXX
extern "C" {
#include "mpi.h"
}
#include "parpoi.h"
#include "grid.h"
extern MPI_Comm XOOPIC_COMM;
extern int *GlobalArgc;
extern char ***GlobalArgv;

#define DEBUG

ParallelPoisson::ParallelPoisson(Scalar **_epsi, Grid *_grid) {
  epsi = _epsi;
  grid=_grid;
  Viewer viewer;
  Viewer aviewer;
  Viewer maviewer;
  int nPoints = (grid->getJ() +1) * (grid->getK() +1);
  int debug=0;
  int ierr;
#ifdef DEBUG
  int size;
  double foo=5;
  debug = 1;
#endif

  // create the mpiPhi vector.
  //  all the petsc stuff returns an error.  
  if(debug) printf("\n Calling petsc initialize\n");
//  CHKERRA( PetscInitialize(GlobalArgc,GlobalArgv,(char *)0,"RTFM") );
 // ierr=PetscInitialize(GlobalArgc,GlobalArgv,(char *)0,"RTFM");
  CHKERRA(ierr);
/*  if(debug) printf("\n MPI_COMM_WORLD: %d\n",MPI_COMM_WORLD);  
  if(debug) printf("\n PETSC_COMM_WORLD: %d\n",PETSC_COMM_WORLD);  
  if(debug) MPI_Comm_size(PETSC_COMM_WORLD,&size); 
  if(debug) printf("\n size: %d\n",size);  
  if(debug) printf("\n VecCreateMPI\n"); */

//  ierr = ViewerDrawOpen(PETSC_COMM_WORLD,PETSC_NULL,PETSC_NULL,0,0,1500,900,
//                         &viewer); CHKERRA(ierr);
  ierr= ViewerASCIIOpen(PETSC_COMM_WORLD,"asciiviewer",&aviewer);
  CHKERRA(ierr);

  ierr= ViewerASCIIOpen(PETSC_COMM_WORLD,"mpiCoeff.txt",&maviewer);
  CHKERRA(ierr);

  ierr= VecCreateMPI(PETSC_COMM_WORLD, nPoints,PETSC_DECIDE, &mpiPhi) ;
  CHKERRA(ierr);
  ierr= VecCreateMPI(PETSC_COMM_WORLD, nPoints,PETSC_DECIDE, &mpiRho) ;
  CHKERRA(ierr);
   ierr=VecCreateMPI(PETSC_COMM_WORLD, nPoints,PETSC_DECIDE, &mpiepsi);
  CHKERRA(ierr);
  ierr=VecCreateMPI(PETSC_COMM_WORLD, nPoints,PETSC_DECIDE, &mpiNumbering);
  CHKERRA(ierr);
  if(debug) printf("\n MatrCreateMPI\n");
  ierr= MatCreateMPIAIJ(PETSC_COMM_WORLD, nPoints,nPoints,PETSC_DECIDE,
									PETSC_DECIDE,5,PETSC_NULL,2,PETSC_NULL,&mpiCoef);
  CHKERRA(ierr);

  int j,k;  // set up the default coefficients.
  for(k=0;k<=grid->getK();k++) 
	  for(j=0;j<=grid->getJ();j++) {
		double tmp = epsi[MIN(j,grid->getJ()-1)][MIN(k,grid->getK()-1)];
		BCTypes type;
		Boundary *B =  grid->GetNodeBoundary()[j][k];
		if(B!=NULL) type = B->getBCType();
		else type = FREESPACE;
    try{
		  set_coefficient(j,k,type,grid);
    }
    catch(Oops& oops2){
      oops2.prepend("ParallelPoisson::ParallelPoisson: Error: \n");//done
      throw oops2;
    }
		PetscVecSetValue(mpiepsi,grid->getGlobalIndex(j,k,HERE),tmp,INSERT_VALUES);
		PetscVecSetValue(mpiNumbering,grid->getGlobalIndex(j,k,HERE),
							  (double) grid->getGlobalIndex(j,k,HERE), INSERT_VALUES);
	}


  if(debug) printf("\n VecAssemble\n");
  ierr=VecAssemblyBegin(mpiepsi);
  CHKERRA(ierr);
  ierr=VecAssemblyBegin(mpiPhi);
  CHKERRA(ierr);
  ierr=VecAssemblyBegin(mpiRho);
  CHKERRA(ierr);
  ierr=VecAssemblyEnd(mpiepsi);
  CHKERRA(ierr);
  ierr=VecAssemblyEnd(mpiNumbering);
  CHKERRA(ierr);
  ierr=VecAssemblyEnd(mpiPhi);
  CHKERRA(ierr);
  ierr=(VecAssemblyEnd(mpiRho));
  CHKERRA(ierr);
  ierr = ViewerPushFormat(aviewer,VIEWER_FORMAT_ASCII_INDEX,"Tabulation");CHKERRA(ierr);
  ierr = VecView(mpiepsi,aviewer); CHKERRA(ierr);
  ierr = VecView(mpiNumbering, aviewer); CHKERRA(ierr);

  ierr=MatAssemblyBegin(mpiCoef,MAT_FINAL_ASSEMBLY);
  CHKERRA(ierr);
  ierr=MatAssemblyEnd(mpiCoef,MAT_FINAL_ASSEMBLY);
  CHKERRA(ierr);
  ierr = ViewerPushFormat(maviewer,VIEWER_FORMAT_ASCII_MATLAB, "Matrix");
  CHKERRA(ierr); 
  ierr = MatView(mpiCoef,maviewer);
  CHKERRA(ierr);
  ierr=(SLESCreate(PETSC_COMM_WORLD, &petsc_solver));
  CHKERRA(ierr);
  ierr=(SLESSetOperators(petsc_solver,mpiCoef,mpiCoef,SAME_PRECONDITIONER));
  CHKERRA(ierr);
  SLESSetFromOptions(petsc_solver);
  //SLESPrintHelp(petsc_solver);
}


// switches to the appropraite set_coefficient subfunction based on
// the grid geometry.
void ParallelPoisson::set_coefficient(int j,int k, BCTypes type, Grid *grid) 
  /* throw(Oops) */{

	switch (grid->query_geometry()) {
	 case ZRGEOM:
		 set_coefficient_rz( j,  k, type, grid);
		 break;
	  case ZXGEOM:
      try{
		    set_coefficient_xy( j,  k, type, grid);
      }
      catch(Oops& oops){
        oops.prepend("ParallelPoisson::set_coefficient: Error:\n"); //ParallelPoisson::ParallelPoisson
        throw oops;
      }
		  break;
	 };
}



// Set the matrix coefficients to reflect the local epsilon and
// boundary conditions, for XY geometry.
void ParallelPoisson::set_coefficient_xy(int j, int k, BCTypes type, Grid *grid) 
  /* throw(Oops) */{
  Vector2 **X=grid->getX();
  int J=grid->getJ();
  int K=grid->getK();

  int Iup,Idown,Ileft,Iright,Icenter;  //  global indices...
  Iup = grid->getGlobalIndex(j,k,UP);
  Idown = grid->getGlobalIndex(j,k,DOWN);
  Ileft = grid->getGlobalIndex(j,k,LEFT);
  Iright = grid->getGlobalIndex(j,k,RIGHT);
  Icenter = grid->getGlobalIndex(j,k,HERE);

  if(type == PERIODIC_BOUNDARY)  { //need to diddle the indices a bit
	  Iup = grid->getGlobalIndex(j,k,UPWRAP);
	  Idown = grid->getGlobalIndex(j,k,DOWNWRAP);
	  Ileft = grid->getGlobalIndex(j,k,LEFTWRAP);
	  Iright = grid->getGlobalIndex(j,k,RIGHTWRAP);
	  //Icenter handled already.
  }
  
  Scalar dxa = X[j][k].e1()  -X[MAX(0,j-1)][k].e1(); // dx-
  Scalar dxb = X[MIN(J,j+1)][k].e1()-X[j][k].e1();  // dx+
  Scalar dxave= 0.5*(dxa + dxb);  //dx
  Scalar dya = X[j][k].e2()-X[j][MAX(0,k-1)].e2();  //dy-
  Scalar dyb = X[j][MIN(K,k+1)].e2() - X[j][k].e2();  //dy+
  Scalar dyave = 0.5 * (dya + dyb); //dy


	// epsilon coefficients, weighted appropriately for each cell side
	Scalar e1a,e1b,e2a,e2b;

//	e1a = (epsi[i][j]*dya + epsi[i][MIN(J,j+1)]*dyb)/(dya + dyb);
//	e1b = (epsi[MIN(i+1,I)][j]*dya + epsi[MIN(i+1,I)][MIN(J,j+1)]*dyb)/(dya + dyb);
//	e2a = (epsi[i][j]*dxa + epsi[MIN(I,i+1)][j]*dxa)/(dxa + dxb);
//	e2b = (epsi[i][MIN(j+1,J)]*dxa + epsi[MIN(i+1,I)][MIN(J,j+1)]*dxb)/(dxa + dxb);

  int jm = MAX(j-1,0);
  int km = MAX(k-1,0);

  e1a = (epsi[jm][km]*dya + epsi[jm][MIN(k,K-1)]*dyb)/(dya + dyb);
  e1b = (epsi[MIN(j,J-1)][km]*dya + epsi[MIN(j,J-1)][MIN(k,K-1)]*dyb)/(dya + dyb);
  e2a = (epsi[jm][km]*dxa + epsi[MIN(j,J-1)][km]*dxb)/(dxa + dxb);
  e2b = (epsi[jm][MIN(k,K-1)]*dxa + epsi[MIN(j,J-1)][MIN(k,K-1)]*dxb)/(dxa + dxb);

  Scalar upc,downc,rightc,leftc,centerc;  //  the coefficients for the stencil.

  

  // The next block of code sets stuff up appropriately if 

  // we're at the edge of the system and it is a SPATIAL_REGION_BOUNDARY
  // Need to do some special things at spatial region boundaries!
  // we just correct some things which weren't right for the 
  // normal FREESPACE or DIELECTRIC
  if(j==0 && Ileft != -1) { // we're on the left hand side
	  dxave=dxa=dxb;  // uniform mesh assumption.
  }
  if(j==J && Iright != -1) { // we're on the righthand side
	  dxave=dxb=dxa;  // uniform mesh assumption.
  }
  if(k==0 && Iup != -1) { // we're on the top side
	  dyave=dya=dyb;  // uniform mesh assumption.
  }
  if(k==K && Idown != -1) { // we're on the top side
	  dyave=dyb=dya;  // uniform mesh assumption.
  }
  
  switch(type) 
	  {
		 
		case SPATIAL_REGION_BOUNDARY:
		case PERIODIC_BOUNDARY:  
		case FREESPACE:
			{
				centerc = upc = downc = leftc = rightc = 0;
				if(Iup >=0) { /* valid index */
					upc = e2a/(dyave*dya);
				}
				if(Idown >=0) { /* valid index */
					downc = e2b/(dyave*dyb);
				}
				if(Ileft >=0) { /* valid index */
					leftc = e1a/(dxa * dxave);
				}
				if(Iright >=0) { /* valid index */
					rightc = e1b/(dxb * dxave);
				}
				centerc = -( upc + downc + leftc + rightc );
				break;
			}   //case FREESPACE
		case CONDUCTING_BOUNDARY:
			{
				upc=downc=leftc=rightc=0;
				centerc = 1;
				break;
			}
		 case DIELECTRIC_BOUNDARY:
			 {
				 centerc = upc = downc = leftc = rightc = 0;

				 if(j==0 && Ileft == -1) { // left hand wall neumann
					 leftc = 0;
					 rightc = 2 * e1b / (dxb * dxb);
					 centerc -= rightc;	
				 }	 else	
					 if(j==J && Iright == -1) { // right hand wall neumann
						 rightc = 0;
						 leftc = 2 * e1a / (dxa * dxa);
						 centerc -= leftc;
					 } else {  // dielectric in the center of the system:  normal coeff.
						 leftc = e1a / (dxa * dxave);
						 rightc = e1b/(dxb * dxave);
						 centerc -= leftc + rightc;
					 }
				 if(k==0 && Iup == -1) { //top wall neumann
					 upc = 0;
					 downc = 2 * e2b / (dyb * dyb);
					 centerc -= downc;
				 } else
					 if(k==K && Idown == -1) { //bottom wall neumann
						 downc = 0;
						 upc = 2 * e2a / (dya * dya);
						 centerc -= upc;
					 } else {// dielectric in the center of the system:  normal coeff.
						 upc = e2a/(dyave*dya);
						 downc = e2b/(dyave*dyb);
						 centerc -= upc + downc;
					 }
				 break;
			 }  // case DIELECTRIC_BOUNDARY
		 case CYLINDRICAL_AXIS:
			 {
         stringstream ss (stringstream::in | stringstream::out);
         ss <<"ParallelPoisson::set_coefficient_xy: Error: \n"<<
           "Can't have a cylindrical axis in xy-geometry!\n" << endl;

				 std::string msg;
         ss >> msg;
         Oops oops(msg);
         throw oops;    // exit()  ParallelPoisson::set_coefficient:

				 break;
			 }

		 default: 
			 {
				 cout << "Error, unhandled boundary type in ParallelPoisson-xy\n" << endl;
				 break;
			 }
	 };

  // All cases have to set up their coefficients.
  PetscMatSetValue(mpiCoef,Icenter,Icenter,centerc,INSERT_VALUES);
  if(Iup >=0) { /* valid index */
	  PetscMatSetValue(mpiCoef,Icenter,Iup,upc,INSERT_VALUES);
  }
  if(Idown >=0) { /* valid index */
	  PetscMatSetValue(mpiCoef,Icenter,Idown,downc,INSERT_VALUES);
  }
  if(Ileft >=0) { /* valid index */
	  PetscMatSetValue(mpiCoef,Icenter,Ileft,leftc,INSERT_VALUES);
  }
  if(Iright >=0) { /* valid index */
	  PetscMatSetValue(mpiCoef,Icenter,Iright,rightc,INSERT_VALUES);
  }	
}

// Set the matrix coefficients to reflect the local epsilon and
// boundary conditions, for XY geometry.

void ParallelPoisson::set_coefficient_rz(int j,int k, BCTypes type, Grid *grid) {
	Vector2 **X = grid->getX();
	int J = grid->getJ();
	int K = grid->getK();

	int Iup,Idown,Ileft,Iright,Icenter;  //  global indices...
	Iup = grid->getGlobalIndex(j,k,UP);
	Idown = grid->getGlobalIndex(j,k,DOWN);
	Ileft = grid->getGlobalIndex(j,k,LEFT);
	Iright = grid->getGlobalIndex(j,k,RIGHT);
	Icenter = grid->getGlobalIndex(j,k,HERE);

	if(type == PERIODIC_BOUNDARY)  { //need to diddle the indices a bit
		Iup = grid->getGlobalIndex(j,k,UPWRAP);
		Idown = grid->getGlobalIndex(j,k,DOWNWRAP);
		Ileft = grid->getGlobalIndex(j,k,LEFTWRAP);
		Iright = grid->getGlobalIndex(j,k,RIGHTWRAP);
		//Icenter handled already.
	}
  
	// These calculations in the z direction are the same
	// as for the x-y.
	Scalar dxa =  X[j][k].e1()  -X[MAX(0,j-1)][k].e1();
	Scalar dxb =  X[MIN(J,j+1)][k].e1()-X[j][k].e1();
	Scalar dxave= 0.5*(dxa + dxb);

	// Calculations peculiar to r-z geometry.  radial, in other words.
	
	Scalar ra = X[j][MAX(0,k-1)].e2();
	Scalar r  = X[j][k].e2();
	Scalar rb = X[j][MIN(K,k+1)].e2();
	

	// Need to do some special things at spatial region boundaries!
	// we just correct some things which weren't right for the 
	// normal FREESPACE or DIELECTRIC
	if(j==0 && Ileft != -1) { // we're on the left hand side
		dxave=dxa=dxb;  // uniform mesh assumption.
	}
	if(j==J && Iright != -1) { // we're on the righthand side
		dxave=dxb=dxa;  // uniform mesh assumption.
	}
	if(k==0 && Iup != -1) { // we're on the top side
		// uniform mesh assumption.
		rb = r + (r - ra);
	}
	if(k==K && Idown != -1) { // we're on the top side
		// uniform mesh assumption.
		ra = r - (rb - r);
	}

	Scalar ra2= 0.5*(ra + r);
	Scalar rb2= 0.5*(r +rb);
	Scalar dr2 = rb2*rb2-ra2*ra2;
	Scalar drb = rb -  r;
	Scalar dra = r  - ra;


	Scalar da1a,da1b,da2aa,da2ab,da2ba,da2bb;  // Areas of surfaces, for epsilons
	
	Scalar e1a,e1b,e2a,e2b;  /* epsilon (i-1/2, j), (i+1/2,j),(i,j-1/2),(i,j+1/2) */
	
	// the area weightings to use for the epsilons
	da1a = sqr(r) - sqr(ra2);
	da1b = sqr(rb2)-sqr(r);
	da2aa= dxa;
	da2ab= dxb;
	da2ba= dxa;
	da2bb= dxb;

	int jm = MAX(j-1,0);
	int km = MAX(k-1,0);
	e1a = (epsi[jm][km]*da1a   + epsi[jm][MIN(k,K-1)]*da1b ) / ( da1a  + da1b);
	e1b = (epsi[MIN(j,J-1)][km]*da1a + epsi[MIN(j,J-1)][MIN(k,K-1)]*da1b  ) / ( da1a  + da1b);
	e2a = (epsi[jm][km]*da2aa  + epsi[MIN(j,J-1)][km] * da2ab  ) / ( da2aa + da2ab);
	e2b = (epsi[jm][MIN(k,K-1)]*da2ba  + epsi[MIN(j,J-1)][MIN(k,K-1)] * da2bb  ) / ( da2aa + da2ab);


	Scalar upc,downc,rightc,leftc,centerc;  //  the coefficients for the stencil.

	// recast as freespace any dielectric found in the interior
	if(type==DIELECTRIC_BOUNDARY && (j>0 && j<J && k>0 && k<K))
		type = FREESPACE;
	// similarly recast as freespace any dielectric found on an edge where
	// there is a SRB
	if(type==DIELECTRIC_BOUNDARY && (
			(j==0 && Ileft >=0) ||
			(j==J && Iright>=0) ||
         (k==0 && Iup >=0)   ||
         (k==K && Idown >=0))) 
		type=FREESPACE;
	
	switch(type) {
	 case CONDUCTING_BOUNDARY:// easy case first
		 {
			 upc=downc=leftc=rightc=0;
			 centerc = 1;
			 break;
		 }
	  case SPATIAL_REGION_BOUNDARY:
	  case PERIODIC_BOUNDARY:
	  case FREESPACE: //  no boundaries, center of system.
		  {
			  centerc = upc = downc = leftc = rightc = 0;
			  // cartesian part of the coefficients.
			  if(Ileft >=0) leftc= e1a/(dxa * dxave);
			  if(Iright >=0) rightc = e1b/(dxb * dxave);
			  centerc -= leftc + rightc;

			  if(Iup >=0) upc = e2a*2*ra2/(dr2 * dra);
			  if(Idown >=0) downc = e2b*2*rb2/(dr2 * drb);
			  centerc -= upc + downc;
			  break;
		  }
	  case DIELECTRIC_BOUNDARY:
		  {
			  upc=downc=leftc=rightc=0;
			  if(j==0 && Ileft==-1) { // left wall Neumann
				  // left right coefficients
				  leftc = 0;
				  rightc = 2 * e1b / (dxb * dxb);
				  centerc -= rightc;
				  // r coefficients
				  if(k==0 && Iup == -1) { // neuman/neumann corner bottom left
					  Scalar dria = X[j][1].e2() -X[j][0].e2();
					  upc = 0;
					  downc =  e2b / (dria * dria);
					  centerc -= upc + downc;
				  } else
				  if(k==K && Idown == -1) {  //neumann/naumann corner top left
					  downc = 0;
					  upc = e2a* 2*ra2/(dr2*dra);
					  centerc -= upc + downc;
				  } else { // on left face
					  upc =  e2a*2*ra2/(dr2 * dra);
					  downc = e2b*2*rb2/(dr2 * drb);
					  centerc -= upc + downc;
				  }
			  }
			  if(j==J&&Iright == -1) { // right wall Neumann
				  rightc = 0;
				  leftc = 2 * e1a / (dxa * dxa);
				  centerc -= rightc + leftc;
				  if(k==0&&Iup == -1) {  //neumann/neumann corner
					  Scalar dria =  X[j][k+1].e2() - X[j][k].e2();
					  upc = 0;
					  downc = e2b / (dria * dria);
					  centerc -= upc + downc;
				  } else
				  if(k==K & Idown == -1) { //neumann/neumann corner
					  downc = 0;
					  upc = e2a*2*ra2/(dr2*dra);
					  centerc -= upc + downc;
				  } else { //on right face
					  upc =  e2a*2*ra2/(dr2 * dra);
					  downc = e2b*2*rb2/(dr2 * drb);
					  centerc -= upc + downc;
				  }

			  }

			  if(k==K && Idown == -1) { // top face, not a corner.
				  Scalar dria = X[j][k].e2()-X[j][k-1].e2();
				  upc = 2 * e2a * ra2/(dria * (r*r-ra2 * ra2));
				  downc = 0;
				  leftc = e1a / (dxa * dxave);
				  rightc = e1b / (dxb * dxave);
				  centerc -= upc +downc +leftc +rightc;
			  }
			  break;
		  }
	  case CYLINDRICAL_AXIS:
		  {
			  centerc = upc = downc = rightc = leftc = 0;
			  if(j==0&&Ileft == -1 || j==J && Iright == -1) break;  // case handled above.
			  Scalar dria = X[j][1].e2() - X[j][0].e2();
			  upc = 0;
			  downc = e2b/(dria*dria);
			  leftc = e1a/(dxa*dxave);
			  rightc = e1b/(dxb*dxave);
			  centerc -= upc + downc + leftc + rightc;
			  
			  break;
		  }
	  default: {
		  cout << "Error, unhandled boundary type in ParallelPoisson-xy\n" << endl;
		  break;
	  }
		 
	 }
	// All cases have to set up their coefficients.
	PetscMatSetValue(mpiCoef,Icenter,Icenter,centerc,INSERT_VALUES);
	if(Iup >=0) { /* valid index */
		PetscMatSetValue(mpiCoef,Icenter,Iup,upc,INSERT_VALUES);
	}
	if(Idown >=0) { /* valid index */
		PetscMatSetValue(mpiCoef,Icenter,Idown,downc,INSERT_VALUES);
	}
	if(Ileft >=0) { /* valid index */
		PetscMatSetValue(mpiCoef,Icenter,Ileft,leftc,INSERT_VALUES);
	}
	if(Iright >=0) { /* valid index */
		PetscMatSetValue(mpiCoef,Icenter,Iright,rightc,INSERT_VALUES);
	}	

}


// because this solver uses a different matrix solver
int ParallelPoisson::laplace_solve(Scalar **u_in, Scalar **s, int itermax, Scalar tol_test) {
	int j,k;

	int J = grid->getJ();
	int K = grid->getK();
	for(j=0;j<=J;j++) {
		for(k=0;k<=K;k++) {
			s[j][k] = u_in[j][k];
		}
	}
	return solve(u_in,s,itermax,tol_test);
}

// perform the actual parallel solve.
	
int ParallelPoisson::solve(Scalar **u_in, Scalar **s, int itermax, Scalar tol_test) {

  double *s_petsc;
  double *u_petsc;
  int ierr;
  int J = grid->getJ();
  int K = grid->getK();
  int j,k;
  int iternum;
  Viewer RhoViewer;

  ierr= ViewerASCIIOpen(PETSC_COMM_WORLD,"RhoViewer",&RhoViewer);

  CHKERRA(ierr);
  ierr = ViewerPushFormat(RhoViewer,VIEWER_FORMAT_ASCII_INDEX,"Rho");CHKERRA(ierr);
  // get the array from PetSC-- it's almost certainly quicker
  //to set the values than have PetSC do it:  it likes to send messages.

  ierr=(VecGetArray(mpiRho,&s_petsc));
  CHKERRA(ierr);
  // write the s_data into the local PETSC array
  for(j=0;j<=J;j++) {
	 for(k=0;k<=K;k++) {
		 s_petsc[getVecLocalIndex(j,k)]=s[j][k];
//		 s_petsc[getVecLocalIndex(j,k)]=getVecLocalIndex(j,k);
	 }
  }
  ierr=(VecRestoreArray(mpiRho,&s_petsc));
  VecView(mpiRho,RhoViewer);

  CHKERRA(ierr);
  // Actually call the solve
  ierr=(SLESSolve(petsc_solver,mpiRho,mpiPhi,&iternum));
  CHKERRA(ierr);

  // Copy the data back into our own datastructures.
  ierr=(VecGetArray(mpiPhi,&u_petsc));
  CHKERRA(ierr);
  for(j=0;j<=J;j++) {
	 for(k=0;k<=K;k++) {
		u_in[j][k]=u_petsc[getVecLocalIndex(j,k)];
	 }
  }
  ierr=(VecRestoreArray(mpiPhi,&u_petsc));
  CHKERRA(ierr); 
  return 0;
}

ParallelPoisson::~ParallelPoisson() {
  
  CHKERRA(SLESDestroy(petsc_solver));

}

void ParallelPoisson::init_solve(Grid *grid,Scalar **epsi)
{
	// All the initialization work is done in parpoi.cpp

}

#endif  /* PETSC */
