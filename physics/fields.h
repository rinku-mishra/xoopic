#ifndef	__FIELDS_H
#define	__FIELDS_H

/*
====================================================================

fields.h

Purpose:	Class definitions for Fields class.  This class provides 
		the full set of electromagnetic fields, as well as 
		methods for advancing the PDEs in time.

Version:	$Id: fields.h 2311 2007-09-17 05:42:18Z johnv $

Revision/Programmer/Date
0.1	(JohnV, 03-15-93)	Proto code.
0.9	(JohnV, 04-30-93)	Initial draft with particles.
0.91	(NTG, JohnV, 05-05-93) Add EmittingBoundary, ActiveBoundary, class
		library use for list management and iterators, KinematicalObject.
0.93	(JohnV, NTG 05-12-93) Fix Windows large model Gen. Prot. Fault due
		to reading outside arrays.
0.95	(JohnV, 06-29-93) Add Boundary::setPassives() to set up passive
		boundary conditions (Fields::iC and ::iL), add Fields::set_iCx()
		to support above.
0.96	(JohnV 08-23-93) Add new Boundary subtypes: BeamEmitter,
		CylindricalAxis, FieldEmitter, Port.  Modifications to
		Maxwellian: stores all parameters in MKS.
0.961	(PeterM, JohnV, 08-24-93) Replace Borland list containers with
		vanilla C++ template lists in oopiclist.h
0.963	(JohnV, 09-24-93)	Add Particle object for moving particle quantities
		around (emission, collection, etc.)
0.964	(JohnV, 11-22-93) Make agreed-upon changes in names: Vector->Vector3,
		Boundaries->BoundaryList, Particles->ParticleList, ParticleGroups
		->ParticleGroupList
0.965	(JohnV, 02-01-94) Move magnetic field advance to Fields::advanceB(),
		move boundary code, particle code to respective files.
0.966	(JohnV, 02-07-94)	Fixed striation in vr vs. z phase space seen
		for beam spreading in a simple cylinder.  Problem was weighting
		error in Grid::interpolateBilinear().
0.967	(JohnV 03-03-94) added Fields::setBz0() and ::epsilonR.
0.968	(JohnV 03-11-94) Fix divide by zero for non-translating particles
		with finite spin in Fields::translateAccumulate().  Also ensure all
		particles that touch boundaries get collected in Grid::translate()
		by changing </> to <=/>= for check conditions.
0.969	(JohnV 03-23-94) Restructure Grid, Fields, and SpatialRegion to
0.970	(JohnV 05-18-94) Upgrade accumulate() to handle right and top
		boundaries properly.
0.975 (PeterM 08-29-94) Added Divergence Clean, Electrostatic solve,
		a Poisson solver.
0.976 (PeterM 04-26-95) Changed so that boundaryList is local data.
		Many local functions need it now.
0.980	(JohnV 08-17-95) Add compute_iC_iL(), generalize for arbitrary meshed epsilon.
0.99  (KeithC 08-25-95) Lack of precision caused Bz0 to 'leak` 
		into Etheta and Br.  Added BNodeStatic to separate the 
		static part from the B fields that that Fields
		updates. Changed Fields::setBz0() to Fields::setBNodeStatic.
0.991	(JohnV 09-10-95) Add setBeamDump() to create a magnetic beam dump;
		required making BNodeStatic a Vector3.
1.0005 (KC 5-15-96) Added EMdamping.  Naoki will tells us what we are doing to the 
      physics.
1.001	(JohnV 05-15-96) Add speciesList, Ispecies to improve subcycling.
1.002 (JohnV 09-30-97) Add freezeFields, to allow fixed-field simulation. 
2.01  (JohnV 03-19-98) Add nSmoothing to apply binomial filter to rho.
2.011 (PeterM 11/4/98) ShiftFields added, for moving window.
2.02  (JohnV 02-04-99) MarderFlag->MarderIter
2.03  (Bruhwiler 10-08-99) added SynchRadiationFlag and get/set methods
2.0?  (Cary 22 Jan 00) Broke up shift fields into two calls, shiftFieldsAndSend
	and recvShiftedFields, in anticipation of MPI installation.
CVS1.42.2.2 (Cary 23 Jan 00) added the boundary pointer to the calls of
	shiftFieldsAndSend and recvShiftedFields.  Also added documentation.
CVS1.44 (JohnV 05 Jan 2001) Added check for magnetic field file in setBNodeStatic()

Some considerations:

o	Performs interpolation separately for E(x) and B(x) in ParticleGroup::
	advance().  Could modify by interpolating together.

====================================================================
 */

#include <grid.h>
#include <boltzman.h>
#include "ngd.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//morgan
class Domain;
class Electrostatic_Operator;

//class PoissonSolve;
class Boltzmann;

#define ELECTROMAGNETIC_SOLVE 0
#define DADI_SOLVE 1
#define CG_SOLVE 2
#define GMRES_SOLVE 3
#define MGRID_SOLVE 4
#define PERIODIC_X1_DADI 5
#define PETSC_SOLVE 6

class Fields
{
	// morgan
	Domain *dom;
	Electrostatic_Operator *poisson;

	Grid*	grid;			     //	pointer to Grid
	PoissonSolve *psolve;  //  the poisson solve object for RZ or ZX
	Scalar presidue;      // residue for poisson solver
	int	J, K;			     //	= grid->J,K
	Vector3**   ENode;     	//	E at nodes at integer time
	Vector3**   BNode;		//	B at nodes at integer time
	Vector3**   BNodeStatic;  // Externally given static B at nodes
	Vector3**   BNodeDynamic; // B at nodes calculated by the field solve
	Vector3**   I;			  //	current = J.dS
	Vector3***  Ispecies;					//	current by species
	Vector3**   accumulator;				// ptr to I array for current accumulation
	Vector3**   Inode;     // current at nodes
	Vector3**   intEdl;	  //	int(E.dl)
	Vector3**   intBdS;	  //	int(B.dS)
	Vector3**   iC;		  //	inverse 'capacitance' matrix
	Vector3**   iL;        //	inverse 'inductance' matrix
	Scalar      epsilonR;  //	relative permittivity
	Scalar      Bz0;
	Scalar      Bx0;
	Scalar      zoff;    //  zoffset offset for wiggler field
	Scalar      betwig;  //  kz for wiggler field
	Scalar **   epsi;      //  permittivity in space
	Scalar **   rho;       //      the charge
	Scalar **   Charge;    // charge accumulated on dielectric boundaries (for diagn)
	Scalar **   backGroundRho;  // time independant rho
	Scalar ***  loaddensity;
	int         ShowInitialDensityFlag;
	int         BGRhoFlag;
	Scalar ***  rho_species;  //  the charge by species
	Scalar **   DivDerror; //      -Divergence of D + rho
	Scalar **   Phi;       //      Potential in the system
	Scalar **   PhiP;      // The potential due to the charge, boundaries grounded
	Scalar **   delPhi;    // used by divergence clean, initialization very
	// important, no divergence clean, no memory use
	Scalar      local_dt;  // a local copy of dt, updated in advance()

	Scalar **   minusrho;  // used by the Poisson solve

	Scalar **d;  // Used by the marder correction

	inline void	accumulate(Vector2& xOld, Vector2& x, Scalar frac_v3dt, Scalar q_dt);
	void	advanceB(Scalar dt2);		     //	advance B 1/2 timestep
	void  advanceE(Scalar dt);          // Advance E

	int MarderIter;
	int DivergenceCleanFlag;
	int CurrentWeightingFlag;
	int ElectrostaticFlag;
	int SynchRadiationFlag;
	int freezeFields;
	int InfiniteBFlag;
	int NonRelativisticFlag;
	int FieldSubFlag;
	int BoltzmannFlag;
	int nSmoothing; // iterations of binomial (1-2-1 in 2d) filter to apply, 0=off.
	int sub_cycle_iter;
	Scalar EMdamping;  //stuff for EM damping
	Vector3 ** intEdlBar; //stuff for EM damping
	Vector3 ** intEdlPrime; //stuff for EM damping
	Vector3 ** intEdlBasePtr; //stuff for EM damping
	Scalar MarderParameter;
	Boltzmann *theBoltzmann;
	BoundaryList *boundaryList;  // needed for the poisson solver and div. clean
	SpeciesList* speciesList;
	Species* bSpecies;
	int nSpecies;
	void radialCurrentCorrection();

public:
	Fields(Grid* grid, BoundaryList* boundaryList, SpeciesList* speciesList, Scalar epsR,
			int ESflag,Scalar presidue, int BoltzmannFlag, int SRflag);
	~Fields();
	void setpresidue(Scalar res) { presidue=res;};
	Scalar getpresidue() { return presidue;};
	SpeciesList* get_speciesList() {return speciesList;};
	int getnSpecies() {return nSpecies;};
	void	advance(Scalar t, Scalar dt) /* throw(Oops) */;  // solve Maxwell equations
	void  ElectrostaticSolve(Scalar t, Scalar dt)/* throw(Oops) */; //
	void	toNodes(Scalar t);				//intEdl->ENode, intBdS->BNode
	Vector3 E(const Vector2& x) {return grid->interpolateBilinear(ENode, x);};
	Vector3 B(const Vector2& x) {return grid->interpolateBilinear(BNode, x);};
	Boundary* translateAccumulate(Vector2& x, Vector3& dxMKS, Scalar
			qOverDt);
	void 	setIntEdl(int j, int k, int component, Scalar value){
		intEdl[j][k].set(component,value);};
	void  setIntBdS(int j, int k, int component, Scalar value){
		intBdS[j][k].set(component,value);};
	void 	setIntEdlBar(int j, int k, int component, Scalar value){
		intEdlBar[j][k].set(component,value);};
	void	setI(int j, int k, int component, Scalar value){
		I[j][k].set(component,value);};
	Vector3** getENode() {return ENode;}
	Vector3** getBNode() {return BNode;}
	Vector3** getBNodeStatic() {return BNodeStatic;}
	Vector3** getBNodeDynamic() {return BNodeDynamic;}
	Vector3** getI() {return I;}
	Vector3** getIntBdS() {return intBdS;}         // Added 6-2-95 by KC
	Vector3** getIntEdl() {return intEdl;}         // Added 6-2-95 by KC
	Vector3** getIntEdlBar() {return intEdlBar;}         // Added 5-16-96 by KC
	Vector3** getiC() {return iC;}                 // Added 6-2-95 by KC
	Vector3** getiL() {return iL;}                 // Added 6-2-95 by KC
	Vector3 getIMesh(int j, int k) {return I[j][k];};
	void	clearI(int i = -1);			//	set Ispecies[i] to 0, -1 means use I
	void	setAccumulator(int i);			// i = species index to accumulate
	void	addtoI(int i);						// for sub_cycling particles
	void	set_iC1(int j, int k, Scalar value) {iC[j][k].set_e1(value);};
	void	set_iC2(int j, int k, Scalar value) {iC[j][k].set_e2(value);};
	void	set_iC3(int j, int k, Scalar value) {iC[j][k].set_e3(value);};
	void	set_iL1(int j, int k, Scalar value) {iL[j][k].set_e1(value);};
	void	set_iL2(int j, int k, Scalar value) {iL[j][k].set_e2(value);};
	void	set_iL3(int j, int k, Scalar value) {iL[j][k].set_e3(value);};
	void	Excite(); //ntg8-16 excites a field component at t = 0
	void	SeedFieldEmitter();
	void  setdt(Scalar dt) {local_dt = dt;};
	BOOL  setBNodeStatic(Vector3 B0,Scalar betwig,Scalar zoff, char* Bfname,const ostring &B01anal,const ostring &B02anal,const ostring &B03anal);   //	static uniform initial magnetic field
	void	setBeamDump(int j1, int j2);	// set up a magnetic beam dump between j1 and j2

	void setEinit(const ostring &E1init, const ostring &E2init, const ostring &E3init);
	void setBinit(const ostring &B1init, const ostring &B2init, const ostring &B3init);
	void  setrho(int j,int k, Scalar value) {rho[j][k]=value;};
	void  setbgrho(int j, int k, Scalar value) {backGroundRho[j][k]+=value;};
	void  setloaddensity(int isp, int j, int k, Scalar &value) {loaddensity[isp][j][k]+=value;};
	void  setShowInitialDensityFlag(int Flag) {ShowInitialDensityFlag=Flag;};
	void  setBGRhoFlag(int Flag) {BGRhoFlag = Flag;};
	Scalar **getrho() {return rho;};
	Scalar getrho(int j, int k) { return rho[j][k]; }; // added for current source, 8/7/02, JeffH
	Scalar **getQ() {return Charge;};
	void  collect_charge_to_grid(ParticleGroup *group,Scalar **_rho);  // collects the charges from particle group 'group' onto the grid rho
	void  compute_rho(ParticleGroupList **list, int nSpecies);  //computes rho, the charge density
	void ZeroCharge(void);
	void  charge_to_charge_density(int nSpecies);  //converts rho from charge on the grid to charge density on the grid
	Scalar get_rhospecies(int id, int j, int k);
	void addBGRho(void);    // addes DC background rho
	Scalar **getDivDerror() {return DivDerror;}
	void  updateDivDerror(void);   //      Calculate DivD
	void  updatePhi(Scalar **source,Scalar **dest,Scalar t,Scalar dt)/* throw(Oops) */; //      Update the potential
	void  initPhi(void) /* throw(Oops) */;  //      Initialize our poisson solver
	void  setEfromPhi(void);  //  Make our electric fields consistent with our poisson solution
	Scalar **getphi() { return Phi; };
	void setPhi(Scalar value,int j,int k) { Phi[j][k]=value; }  //set phi at j,k
	void  DivergenceClean(void);  // Do a divergence clean.  call compute_rho and updateDivDerror (in that order) before calling this.

	Vector2 getMKS(int j, int k){return grid->getMKS(j, k);}
	Vector2 getMKS(Vector2 x){return grid->getMKS(x);}
	Vector2 getGridCoords(Vector2 xMKS){return grid->getGridCoords(xMKS);}
	Vector3 differentialMove(const Vector2& x0, const Vector3& v, Vector3& u,
			Scalar dt) {return grid->differentialMove(x0, v, u, dt);}
	Grid*	get_grid(){return grid;}
	Scalar getBz0(){return Bz0;}
	Scalar getBx0(){return Bx0;}
	int getJ() {return grid->getJ();}
	int getK() {return grid->getK();}
	int getSub_Cycle_Iter() {return sub_cycle_iter;}
	Scalar get_dt() { return local_dt; }
	void InodeToI(); //  average the inode currents to the I currents

	void MarderCorrection(Scalar dt);  // Do a marder correction pass

	void setMarderIter(int flag) { MarderIter=flag;}
	void setDivergenceCleanFlag(int flag) { DivergenceCleanFlag=flag;}
	void setCurrentWeightingFlag(int flag) { CurrentWeightingFlag=flag;}
	void setElectrostaticFlag(int flag) { ElectrostaticFlag=flag;}
	void setSynchRadiationFlag(int flag) { SynchRadiationFlag=flag;}
	void set_freezeFields(int flag) {freezeFields=flag;}
	void setNonRelativisticFlag(int flag) { NonRelativisticFlag=flag;}
	void set_nSmoothing(int n) {nSmoothing = n;}
	void setInfiniteBFlag(int flag) { InfiniteBFlag=flag;}
	void setFieldSubFlag(int flag) { FieldSubFlag=flag;}
	void setEMdamping(Scalar value) {EMdamping = value;}
	void setBoltzmannFlag(int flag) { BoltzmannFlag=flag;}
	int getBoltzmannFlag() {return BoltzmannFlag;}
	void setMarderParameter(Scalar value) { MarderParameter = value;}
	int getElectrostatic() { return ElectrostaticFlag; }
	int getSynchRadiation() { return SynchRadiationFlag; }
	int get_freezeFields() {return freezeFields;}
	Scalar getEMdamping() {return EMdamping;}
	int getNonRelativistic() { return NonRelativisticFlag; }
	int getInfiniteBFlag() { return InfiniteBFlag; }
	int getFieldSub() { return FieldSubFlag; }
	PoissonSolve *getPoissonSolve() { return psolve; }
	void set_epsi(Scalar value,int j,int k) {epsi[j][k]=value;};
	Scalar get_epsi(int j, int k) {return epsi[j][k];};
	void setBoltzmann(Boltzmann *B) { theBoltzmann=B; };
	Scalar *** getRhoSpecies() { return rho_species; };
	Scalar ** getBoltzmannRho() {return theBoltzmann->getBoltzmannRho();};
	Species* get_bSpecies() {return bSpecies;};
	void set_bSpecies(Species *BS) {bSpecies=BS;};
	void CopyBoltzmannRho() {theBoltzmann->CopyBoltzmannRho();}
	Scalar ** getTotalRho() {return theBoltzmann->getTotalRho();};
	Scalar ** getdelPhi() { return delPhi; };
	Scalar getPhiP(int j, int k) { return PhiP[j][k]; }; // added for current source, 8/7/02, JeffH
	Scalar *** getloaddensity() { return loaddensity;};
	int getShowInitialDensityFlag() {return ShowInitialDensityFlag;};
	void compute_iC_iL();
	//  Scalar getMinPot(int sign);
	//	void setBoltzQE(Scalar charge, Scalar energy) {theBoltzmann->setQE(charge,energy);};
	ParticleList& testParticleList(ParticleGroupList& pgList) {return theBoltzmann->testParticleList(pgList);};
	void IncBoltzParticles(ParticleGroupList& pgList) {theBoltzmann->IncBoltzParticles(pgList);};


#ifdef HAVE_HDF5

	int dumpH5(dumpHDF5 &dumpObj);
	int restoreH5(dumpHDF5 &restoreObj);
#endif

	int Dump(FILE *DMPFile);
	int Restore(FILE *DMPFile);
	int Restore_2_00(FILE *DMPFile);

	// morgan
	void set_up_inverter(Grid*) /* throw(Oops) */;

	// support for moving window
	// direction:  1 = j+1 ==> j, 2 = j-1 ==> j, 3 = k+1 --> k, 4 = k-1 --> k

	/**
	 * Send fields in a given direction, which corresponds to a certain
	 * boundary
	 *
	 * @param direction The direction of the shift
	 * @param bPtr the boundary that fields are going out
	 */
	void sendFieldsAndShift(int direction, Boundary* bPtr);

	/**
	 * Receive fields due to a shift of a given direction.
	 *
	 * @param direction The direction of the shift
	 * @param bPtr the boundary that fields are going in from
	 */
	void recvShiftedFields(int direction, Boundary* bPtr,Scalar t, Scalar dt);

	/**
	 * Load in any new fields for moving boundary
	 *
	 */
	void loadShiftedFields(Scalar t, Scalar dt);

};

//--------------------------------------------------------------------
//	Accumulate current based on x(t) and x(t+dt*f).  Here frac_v3dt
//	is the fraction of the timestep for this piece of the move times
//	the azimuthal displacement v3*dt.  Cell volume is more accurately
//	chosen separately for each current element.
// q_dt == q/dt

inline void Fields::accumulate(Vector2& xOld, Vector2&x, Scalar frac_v3dt, Scalar q_dt)
{
	if (get_freezeFields()) return; // emit always does EM push
	int	j = (int) MIN(xOld.e1(),x.e1());
	int	k = (int) MIN(xOld.e2(),x.e2());
	if (j == grid->getJ()) j--;
	if (k == grid->getK()) k--;
	Vector2	dw = x - xOld;
	Vector2	avg_w = 0.5*(xOld + x) - Vector2(j, k);

	frac_v3dt /= grid->cellVolume(j, k);
	if(CurrentWeightingFlag==0)
	{
		if(accumulator) {
			accumulator[j][k] += q_dt*Vector3(dw.e1()*(1 - avg_w.e2()),
					dw.e2()*(1 - avg_w.e1()),
					grid->dS3Prime(j, k)*(1 - avg_w.e1())*
					(1 - avg_w.e2())*frac_v3dt);

			accumulator[j][k+1] += q_dt*Vector3(dw.e1()*avg_w.e2(),
					0,
					grid->dS3Prime(j, k+1)*
					(1 - avg_w.e1())*avg_w.e2()*frac_v3dt);
			accumulator[j+1][k] += q_dt*Vector3(0,
					dw.e2()*avg_w.e1(),
					grid->dS3Prime(j+1, k)*
					avg_w.e1()*(1 - avg_w.e2())*frac_v3dt);

			accumulator[j+1][k+1] += q_dt*Vector3(0,
					0,
					grid->dS3Prime(j+1, k+1)*avg_w.e1()*avg_w.e2()
					*frac_v3dt);
		}
	}
	else  /*  the bilinear weighting of current */
	{
		if(accumulator) {
			accumulator[j][k] += q_dt*Vector3(dw.e1()*(1 - avg_w.e2())*(1-avg_w.e1()),
					dw.e2()*(1 - avg_w.e1())*(1-avg_w.e2()),
					grid->dS3Prime(j, k)*(1 - avg_w.e1())*
					(1 - avg_w.e2())*frac_v3dt);

			accumulator[j][k+1] += q_dt*Vector3(dw.e1()*avg_w.e2()*(1-avg_w.e1()),
					dw.e2()*avg_w.e2()*(1-avg_w.e1()),
					grid->dS3Prime(j, k+1)*
					(1 - avg_w.e1())*avg_w.e2()*frac_v3dt);

			accumulator[j+1][k] += q_dt*Vector3(dw.e1()*avg_w.e1()*(1-avg_w.e2()),
					dw.e2()*avg_w.e1()*(1-avg_w.e2()),
					grid->dS3Prime(j+1, k)*
					avg_w.e1()*(1 - avg_w.e2())*frac_v3dt);
			accumulator[j+1][k+1] += q_dt*Vector3(dw.e1()*avg_w.e1()*avg_w.e2(),
					dw.e2()*avg_w.e1()*avg_w.e2(),
					grid->dS3Prime(j+1, k+1)*avg_w.e1()*avg_w.e2()
					*frac_v3dt);
		}
	}
}

#endif // #ifndef __FIELDS_H
