/*
	 ====================================================================

	 FIELDEMI.CPP

	 0.99	(NTG 12-30-93) Separated into individual module from pic.h.
	 0.991	(JohnV, BillP 03-11-94)	Refine algorithm for emission in
	 x1-direction only.  Fix random seed for proper particle
	 distribution.
	 0.992	(JohnV 03-23-94) Streamline includes, remove pic.h.
	 0.993	(BillP 05-30-94) Fix sign of emission and other bugs.  Complete
	 algorithm for emission in all directions.
	 0.994 (JohnV 09-05-94) Integrate Child's law field emitter into oopic.
	 0.995 (JohnV 01-29-95) Add Species capability.
	 1.001 (JohnV, BillP 04-09-96) Streamline emit().
	 1.1   (JohnV 03-12-97) Add FieldEmitter2, based on Gauss's Law.
	 1.2   (JohnV 18Jul2003) Update FieldEmitter2

	 ====================================================================
	 */

#include "fieldemi.h"
#include	"fields.h"
#include "ptclgrp.h"


#ifdef _MSC_VER
using std::cout;
using std::cerr;
using std::endl;
#endif

//--------------------------------------------------------------------
//	Construct FieldEmitter object

FieldEmitter::FieldEmitter(oopicList <LineSegment> *segments,
													 Species* s, Scalar np2c, Scalar t) : Emitter(segments, s,
																																				np2c)
{
  if(segments->nItems() > 1) {
	 cout << "Warning, FieldEmitters boundaries can only have 1 segment.\n";
	 cout << "Check your input file.\n";
  }
  
  threshold = fabs(t);								//	t = 1E7 for most materials
  emitFlag = FALSE;
  const1 = get_q()/(3*iEPS0*iEPS0*get_m());
}

//--------------------------------------------------------------------
//	Emit uniformly along the surface of the emitter based on the
//	local field value.
#if !defined __linux__ && !defined _WIN32
#pragma	argsused
#endif
ParticleList& FieldEmitter::emit(Scalar t,Scalar dt,Species *species_to_push)
  /* throw(Oops) */{
  if (species==species_to_push){
	 if (!emitFlag){								//	test emission: exit if below threshold
		Scalar Ecenter;
		Vector2 center(0.5*(j1+j2), 0.5*(k1+k2));
		if (alongx2()) Ecenter = fields->E(center).e1();
		else Ecenter = fields->E(center).e2();
		if (fabs(Ecenter) < threshold) return particleList;
		emitFlag = TRUE;
		grid = fields->get_grid();
	 }
	 Scalar temp = 0.5*normal;
	 if (alongx2()){					  			// vertical boundary
		int jcell = j1 - (normal==1) ? 0 : 1; // subtract 1 when normal neg.
		for (int kcell = k1; kcell < k2; kcell++){
		  Scalar Ecell = fields->E(Vector2(j1+temp,kcell+0.5)).e1();
		  if (normal*Ecell*get_q() <= 0) continue;
		  Scalar dxHalf = 0.5*(grid->dl1(jcell, kcell) + grid->dl1(jcell, kcell+1));
		  // Itotal = I required at cell to satisfy Child-Langmuir
		  Scalar Itotal = normal*Ecell*sqrt(fabs(const1*Ecell)/dxHalf)
			 *0.5*(grid->dS1(jcell, kcell) + grid->dS1(jcell, kcell+1));
		  // Icell = existing I at cell center
		  Scalar Icell = 0.5*(fields->getIMesh(jcell, kcell).e1() + fields->getIMesh(jcell, kcell+1).e1());
		  int numberEmitted = (int)(0.5 + (Itotal - Icell)*dt/get_q());
		  
		  Scalar phiRatio = fabs(get_q()*Ecell*dxHalf/get_m());
		  // u = gamma*v for particles to get to cell center
		  Vector3 u(normal*sqrt(phiRatio*(2 + phiRatio*iSPEED_OF_LIGHT_SQ)), 0, 0);

		  for (int i=0; i < numberEmitted; i++){
			 //	Pick sqrt(frand()) in cylindrical coords to get uniform
			 //	density in cell due to volume ratio.
			 Scalar w2=0.;
			 if (grid->query_geometry() == ZRGEOM){
				Scalar x1, x1s, x2, x2s;
				x1 = grid->getMKS(Vector2(j1 + temp, kcell)).e2();
				x1s = x1*x1;
				x2 = grid->getMKS(Vector2(j1 + temp, kcell + 1)).e2();
				x2s = x2*x2;
				w2 = (sqrt(x1s + (x2s - x1s)*frand()) - x1)/(x2 - x1);
			 }
			 else if (grid->query_geometry() == ZXGEOM) w2 = frand();
			 Vector2 position(j1, kcell + w2);
			 // dxMKS = MKS distance particle should move into system
			 Vector3 dxMKS(fields->getMKS(Vector2(j1 + temp, kcell + 0.5)).e1() 
								- fields->getMKS(Vector2(j1, kcell + 0.5)).e1(), 0, 0);
			 // below should check boundary intersection
			 fields->translateAccumulate(position, dxMKS, get_q()/dt);
			 particleList.push(new Particle(position, u, species, np2c));
		  }                                                 
		}
	 }
	 else	{										//	horizontal boundary
		int kcell = k1 - (normal == 1) ? 0 : 1; // subtract 1 for neg. normal
		for (int jcell = j1; jcell < j2; jcell++){
		  Scalar Ecell = fields->E(Vector2(jcell+0.5, k1+temp)).e2();
		  if (normal*Ecell*get_q() <= 0) continue;
		  Scalar dxHalf = 0.5*(grid->dl2(jcell, kcell) + grid->dl2(jcell+1, k1));
		  // Itotal = I required at cell to satisfy Child-Langmuir
		  Scalar Itotal = normal*Ecell*sqrt(fabs(const1*Ecell)/dxHalf)
			 *0.5*(grid->dS2(jcell, kcell) + grid->dS2(jcell+1, kcell));
		  // Icell = existing I at cell center
		  Scalar Icell = 0.5*(fields->getIMesh(jcell, kcell).e2() + fields->getIMesh(jcell+1, kcell).e2());
		  int numberEmitted = (int)(0.5 + (Itotal - Icell)*dt/get_q());
		  
		  Scalar phiRatio = fabs(get_q()*Ecell*dxHalf/get_m());
		  // u = gamma*v for particles to get to cell center
		  Vector3 u(normal*sqrt(phiRatio*(2 + phiRatio*iSPEED_OF_LIGHT_SQ)), 0, 0);
		  
		  for (int i=0; i < numberEmitted; i++){
			 // x1 is always a linear dimension, z or x, so use frand()
			 Scalar w1;
			 w1 = frand();
			 Vector2 position(jcell + w1, k1);
			 // dxMKS = MKS distance particle should move into system
			 Vector3 dxMKS(0, fields->getMKS(Vector2(jcell + 0.5, k1 + temp)).e2()
								- fields->getMKS(Vector2(jcell + 0.5, k1)).e2(), 0);
			 // below should check boundary intersection
			 fields->translateAccumulate(position, dxMKS, get_q()/dt);
			 particleList.push(new Particle(position, u, species, np2c));
		  }                                                 
		}
	 }
  }
  return particleList;

#ifdef OLD_CODE
	Grid*	grid = fields->get_grid();
	Scalar	Etest;
	//	check sign of charge; emit if electric field is right direction
	int	sign = (int) (get_q()/fabs(get_q()));

	//		cout << " in fieldemit.cpp and going into Alongx2()" << endl;
	
	if (alongx2())
		{
			int jemit = j1;
			int ktest = (k2 + k1)/2;  //sample electric field at center
			// determine direction
			if (k2 < k1) {jemit--;}

			Scalar 	EmitWhichWay = (k2-k1)/(fabs(k2-k1));
			//EmitWhichWay = +1 for emission from left to right
			//EmitWhichWay = -1 for emission from right to left

			Vector2	Eposition1(jemit + 0.5, ktest);
			Etest = fields->E(Eposition1).e1();

			if (emitFlag == FALSE)
				{
					if (  sign*Etest*EmitWhichWay < threshold)
						return particleList;	   //ONLY emit when E-field is larger than threshold
					emitFlag = TRUE;
				}
			
			Scalar ratio;

			Scalar	uBeam;

			Vector3	u;
			Scalar	r1;
			Scalar	r1s;
			Scalar	r2;
			Scalar	r2s;
			Vector2	deltaX;
			Scalar 	Ecell;
			Scalar	cellsize;

			Scalar  CurrentDen;
			Scalar  DeltaI;
			int 	numberEmitted = 0;
			Scalar  current;
			Vector2	position;
			Vector3	dxMKS;
			//		Scalar  J;
			//		Scalar  newCurrent;

			//for debugging...
			Scalar klow=k1;
			Scalar khigh=k2;
			if (k2 < k1)
				{
					klow = k2;
					khigh = k1;
				}

			for (int kcell = (int) klow; kcell < khigh; kcell++)
				{
					r1 = grid->getMKS((int)(jemit+0.5), kcell).e2();
					r1s = r1*r1;
					r2 = grid->getMKS((int)(jemit+0.5), kcell+1).e2();
					r2s = r2*r2;
					cellsize = 0.5*(grid->dl1(jemit, kcell)
													+ grid->dl1(jemit, kcell+1));

					//find normal electric field at cell center
					Ecell = fields->E(Vector2(jemit+0.5,kcell+0.5)).e1();

					// calculate current density (A/m^2) needed at cell center:
					CurrentDen = EmitWhichWay*
						sign*sqrt(fabs(ELECTRON_CHARGE*Ecell*Ecell*Ecell)
											/(6*0.5*cellsize*iEPS0*iEPS0*ELECTRON_MASS));

					//	vBeam NOT relativistic
					ratio = sqrt(fabs(ELECTRON_CHARGE*Ecell*cellsize*iSPEED_OF_LIGHT_SQ/
														ELECTRON_MASS));
					uBeam = ratio * sqrt(1 + ratio*ratio/4) * SPEED_OF_LIGHT;
					/*  MARK MARK!!  A FIX NEEDS TO BE DONE OTHER PLACES LIKE ABOVE 2 LINES */

					uBeam *= EmitWhichWay;       // ensure proper sign
					u = Vector3(uBeam, 0, 0);

					// get existing currents and average to cell center;

					current = 0.5*(fields->getIMesh(jemit,kcell).e1()
												 + fields->getIMesh(jemit,kcell+1).e1());

					//			J = current/(0.5*(grid->dS1(jemit,kcell)
					//				+ grid->dS1(jemit+1,kcell)));

					//calculate additional total current and current density
					// at cell center which is needed
					DeltaI = CurrentDen*0.5*(grid->dS1(jemit,kcell)
																	 + grid->dS1(jemit+1,kcell)) - current;

					// find number of particles to be emitted and add nearly one to round-up
					// the value...
					numberEmitted = 1 + (int)(fabs(DeltaI*dt/get_q()));

					//	For electrons, add current when DeltaI < 0; for ions add
					//	current when DeltaI > 0, where DeltaI is the change in current
					//	required to make E_normal = 0 at the emitter.
					if (sign*EmitWhichWay*DeltaI > 0)
						{
							for (int ipart=0; ipart < numberEmitted; ipart++)
								{
									//	Pick sqrt(frand()) in cylindrical coords to get uniform
									//	density in cell due to volume ratio.
									Scalar w2;
									if(grid->query_geometry()==ZRGEOM)
										w2 = (sqrt(r1s + (r2s - r1s)*frand()) - r1)/(r2 - r1);
									else if (grid->query_geometry()==ZXGEOM)
										w2 = frand();
									else
										{
                      stringstream ss (stringstream::in | stringstream::out);
                      ss<< "FieldEmitter::emit: Error: \n"<< 
                        "load doesn't recognize the geometery flag" << endl;

											std::string msg;
                      ss >> msg;
                      Oops oops(msg);
                      throw oops;    // exit()

										}
									// below is position ON BOUNDARY where particle should be emitted
									position = Vector2(jemit + 0.5*(1.-EmitWhichWay),
																		 kcell + w2);

									Vector2 HalfCellOffBoundary = Vector2(jemit + 0.5, position.e2());

									// Now subtract to find emission direction

									Vector2 temp = fields->getMKS(HalfCellOffBoundary -position);
									dxMKS = Vector3(temp.e1(), 0, 0);

									// below should check boundary intersection
									fields->translateAccumulate(position, dxMKS, get_q()/dt);
									particleList.push(new Particle(position, u, species, np2c));
								}
						}
				}
		}
	else											// Alongx1()
		{
			int kemit = k1;
			int jtest = (j1 + j2)/2;  //sample electric field at center of emitter.

			Scalar 	EmitWhichWay = -(j2-j1)/(fabs(j2-j1));
			//EmitWhichWay = -1 for emission downward
			//EmitWhichWay = +1 for emission upward

			if (j1 < j2) {kemit--;} // faces downward if j1 < j2

			Vector2	Eposition1(jtest, kemit + 0.5);
			Etest = fields->E(Eposition1).e2();

			if (emitFlag == FALSE)  //ONLY emit when E-field is larger than threshold
				{
					if (  sign*Etest*EmitWhichWay  < threshold)
						return particleList;
					emitFlag = TRUE;
				}


			Scalar	vBeam;
			Scalar	beta;
			Scalar	uBeam;

			Vector3	u;




			Vector2	deltaY;
			Scalar 	Ecell;
			Scalar	cellsize;

			Scalar  CurrentDen;
			Scalar  DeltaI;
			int 	numberEmitted = 0;
			Scalar  current;
			Vector2	position;
			Vector3	dyMKS;

			Scalar jlow=j1;
			Scalar jhigh=j2;
			if (j2<j1)
				{
					jlow = j2;
					jhigh = j1;
				}

			for (int jcell = (int)jlow; jcell < jhigh; jcell++)
				{
					/*      metric elements for r only
									r1 = grid->getMKS(jemit+0.5, kcell).e2();
									r1s = r1*r1;
									r2 = grid->getMKS(jemit+0.5, kcell+1).e2();
									r2s = r2*r2;
									*/

					cellsize = 0.5*(grid->dl2(jcell, kemit)
													+ grid->dl2(jcell+1, kemit));

					//find normal electric field at cell center
					Ecell = fields->E(Vector2(jcell+0.5,kemit+0.5)).e2();
					//	cout << "Ecell is " << Ecell << endl;


					// calculate current density (A/m^2) needed at cell center:
					CurrentDen = EmitWhichWay*sign*
						sqrt(fabs(ELECTRON_CHARGE*Ecell*Ecell*Ecell)
								 /(6*0.5*cellsize*iEPS0*iEPS0*ELECTRON_MASS));

					//	vBeam NOT relativistic
					vBeam = sqrt(fabs(ELECTRON_CHARGE*Ecell*cellsize/ELECTRON_MASS));
					beta = vBeam*iSPEED_OF_LIGHT;
					uBeam = vBeam/sqrt(1 - beta*beta);
					uBeam *= EmitWhichWay;			// ensure proper sign
					u = Vector3(0, uBeam, 0);

					// get existing currents and average to cell center;
					current = 0.5*(fields->getIMesh(jcell,kemit).e2()
												 + fields->getIMesh(jcell+1,kemit).e2());

					//calculate additional total current and current density
					// at cell center which is needed
					// check this!
					DeltaI = CurrentDen*0.5*(grid->dS2(jcell,kemit)
																	 + grid->dS2(jcell,kemit+1)) - current;

					// find number of particles to be emitted and add nearly one to round-up
					// the value...
					numberEmitted = 1 + (int)fabs(DeltaI*dt/get_q());

					//	For electrons, add current when DeltaI < 0; for ions add
					//	current when DeltaI > 0, where DeltaI is the change in current
					//	required to make E_normal = 0 at the emitter.
					if (sign*DeltaI*EmitWhichWay > 0)
						{
							for (int ipart=0; ipart < numberEmitted; ipart++)
								{
									// below is position ON BOUNDARY where particle should be emitted
									position = Vector2(jcell+frand(), kemit + 0.5*(1.-EmitWhichWay) );
									Vector2 HalfCellOffBoundary = Vector2(position.e1(), kemit+0.5);

									// Now subtract to find emission direction vector
									Vector2 temp = fields->getMKS(HalfCellOffBoundary -position);
									dyMKS = Vector3(0, temp.e2(), 0);
									// below should check boundary intersection
									fields->translateAccumulate(position, dyMKS, get_q()/dt);
									particleList.push(new Particle(position, u, species, np2c));
								}
						}
				}
		}
	return particleList;
#endif
}

//--------------------------------------------------------------
// FieldEmitter2
//
// This bc injects charge sufficient to achieve E =0 normal to surface
// by Gauss's Law

FieldEmitter2::FieldEmitter2(oopicList <LineSegment> *segments,
			     Species* s, Scalar np2c, Scalar t, 
			     MaxwellianFlux *max) 
  : Emitter(segments, s, np2c)
{
  maxwellian = max;
  threshold = fabs(t);
//	t = 1E7 for most materials
  emitFlag = FALSE;
  emitDirection = normal*((species->get_q() > 0) ? 1 : -1);
  delta = normal*1e-4;
  grid = NULL;
  init = TRUE;
}

//---------------------------------------------------------------
// assume orthogonal boundary

ParticleList& FieldEmitter2::emit(Scalar t, Scalar dt, Species* species_to_push)
{
  Scalar N1, N2, R;

  if (species==species_to_push){
    if (init) initialize();
 
//    Scalar Dp, Dm;
    Boundary* bPtr;
    Scalar del_t;

    if (alongx1()){   // horizontal emitter
      Vector2 x(0, k1 + delta);
		Vector3 dxMKS(0,0,0);
      int kindex = (normal==1)?k1:k1-1;
      for (int j=j1; j<j2; j++){
		  N1 = normal*fields->get_epsi(j,kindex)*fields->getIntEdl()[j][kindex].e2()/grid->dl2(j, kindex)*grid->dSPrime()[j][kindex].e2()-rho[j][k1]*grid->get_halfCellVolumes()[j][k1];
		  N2 = normal*fields->get_epsi(j+1,kindex)*fields->getIntEdl()[j+1][kindex].e2()/grid->dl2(j+1, kindex)*grid->dSPrime()[j+1][kindex].e2()-rho[j+1][k1]*grid->get_halfCellVolumes()[j+1][k1];
		  N1 /= species->get_q()*np2c;
		  N2 /= species->get_q()*np2c;
		  Scalar N=0.5*(N1+N2);
		  // Scalar Nthres = fabs(fields->get_epsi(j,kindex)*threshold*
		  // grid->dS1(j,kindex)
		  // /(species->get_q()*np2c)); 
	
		  // N -= Nthres;
		  for (int i=0; i < (int)N; i++){
			 del_t = dt*(i+0.5)/((int) N);
			 R=frand();
			 // x.set_e1(j - (N1-sqrt(-(R-1)*sqr(N1)+R*sqr(N2)))/(N2-N1));
			 if (fabs(N2-N1)>1E-6)
				x.set_e1(j - (N1-sqrt(N1*N1*(1-R) + R*N2*N2))/(N2 - N1));
			 else 
				x.set_e1(j + R);
			 if (x.e1() < j || x.e1() > j+1) printf("FieldEmitter2: x = %G\n", x.e1());
			 Vector3 u=maxwellian->get_U(normal);
			 Particle* p = new Particle(x, u, species, np2c);
			 bPtr = initialPush(del_t, dt, *p);
			 if (!bPtr) particleList.add(p);
			 else bPtr->collect(*p, dxMKS);
		  }
      }
    }
    else { // vertical
      Vector2 x(j1 + delta, 0);
      int jindex = (normal==1)?j1:j1-1;
      for (int k=k1; k<k2; k++){
		  N1 = normal*fields->get_epsi(jindex,k)*fields->getIntEdl()[jindex][k].e1()/grid->dl1(jindex,k)* grid->dSPrime()[jindex][k].e1()-rho[j1][k]*grid->get_halfCellVolumes()[j1][k];
		  N2 =normal*fields->get_epsi(jindex,k+1)*fields->getIntEdl()[jindex][k+1].e1()/grid->dl1(jindex, k+1)*grid->dSPrime()[jindex][k+1].e1()-rho[j1][k+1]*grid->get_halfCellVolumes()[j1][k+1];
		  N1 /= species->get_q()*np2c;
		  N2 /= species->get_q()*np2c;
		  Scalar N=0.5*(N1+N2);
		  //Scalar Nthres = fabs(fields->get_epsi(jindex,k)*threshold*
		  // grid->dS1(jindex,k)
		  // /(species->get_q()*np2c)); 
	
		  // N-=Nthres;
		  //	cout << " number of particles " << N << " at j,k "<< jindex << " "<< k << endl;  
		  //	cout << " N1 " << N1 << " N2 " << N2 << endl;
		  for (int i=0; i<(int)N; i++){
			 del_t = dt*(i+0.5)/((int) N);
			 R=frand();
			 if (rweight) R=sqrt(k*k + R*(2*k + 1))-k;
			 x.set_e2(k - (N1-sqrt(-(R-1)*sqr(N1)+R*sqr(N2)))/(N2-N1));
			 Vector3 u=maxwellian->get_U(normal);
			 Particle* p = new Particle(x, u, species, np2c);
			 bPtr = initialPush(del_t, dt, *p);
			 if (!bPtr) particleList.add(p);
		  }
      }
    }
  }
  return particleList;
}

void FieldEmitter2::initialize()
{
  Emitter::initialize();
  grid = fields->get_grid();
  rho = fields->getrho();
}

#ifdef old_code
  if (species==species_to_push){
	 if (!grid) grid = fields->get_grid();
	 Vector3** ENode = fields->getENode();
	 if (!emitFlag){ // First, determine if it should begin emitting
		if (alongx1()){  // horizontal
		  for (int j=j1; j<j2; j++) 
			 if (ENode[j][k1].e2() > emitDirection*threshold)
				emitFlag = TRUE;
		}
		else {  // vertical
		  for (int k=k1; k<k2; k++)
			 if (ENode[j1][k].e1() > emitDirection*threshold)
				emitFlag = TRUE;
		}
	 }
	 
	 if (emitFlag){
		Scalar Dp, Dm;
		Boundary* bPtr;
		Scalar del_t;
		if (alongx1()){   // horizontal emitter
		  Vector2 x(0, k1 + delta);
		  for (int j=j1; j<j2; j++){
			 Scalar N = 0.5*fields->get_epsi(j,k1)*(ENode[j][k1].e2() + ENode[j+1][k1].e2())
				*grid->dS2(j,k1);
			 N /= species->get_q()*np2c;
			 for (int i=0; i < (int)N; i++){
				del_t = dt*(i+0.5)/((int) N);
				x.set_e1(j+frand());
				Vector3 u; // zero by default
				Particle* p = new Particle(x, u, species, np2c);
				bPtr = initialPush(del_t, dt, *p);
				if (!bPtr) particleList.add(p);
			 }
		  }
		}
		else { // vertical
		  Vector2 x(j1 + delta, 0);
		  for (int k=k1; k<k2; k++){
			 Scalar N = 0.5*fields->get_epsi(j1,k)*(ENode[j1][k].e1() + ENode[j1][k+1].e1())
				*grid->dS1(j1,k);
			 N /= species->get_q()*np2c;
			 for (int i=0; i<(int)N; i++){
				del_t = dt*(i+0.5)/((int) N);
				if (rweight) x.set_e2(sqrt(k*k + frand()*(2*k + 1)));
				else x.set_e2(k + frand());
				Vector3 u;
				Particle* p = new Particle(x, u, species, np2c);
				bPtr = initialPush(del_t, dt, *p);
				if (!bPtr) particleList.add(p);
			 }
		  }
		}
	 }
  }
  return particleList;
}
#endif
