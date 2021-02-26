//gridg.cpp

#include "grid.h"
#include "gridg.h"
extern Evaluator *adv_eval;
extern "C++" void printf(char *);

#ifdef MPI_VERSION
#include "mpi.h"
extern MPI_Comm XOOPIC_COMM;
extern int MPI_RANK;

#if defined(MPI_DEBUG) && defined(MPI_VERSION)
extern ofstream dfout;
#endif /* defined(MPI_DEBUG) && defined(MPI_VERSION) */

#define SRB_ANNOUNCE_TAG 100
#define SRB_LINK_TAG 200
typedef struct {
  char *name;
  int index;
  int linkedP;
} SRBdat;
#endif /*MPI_VERSION*/


//=================== GridParams Class

GridParams::GridParams()
	  : ParameterGroup()
{
	name = "Grid";
	J.setNameAndDescription(ostring("J"),
			ostring("Integer number of grid cells in x1 direction"));
	K.setNameAndDescription(ostring("K"),
			ostring("Integer number of grid cells in x2 direction"));
	x1s.setNameAndDescription(ostring("x1s"),
			ostring("Scalar starting value for grid x1 (left)"));
	x1f.setNameAndDescription(ostring("x1f"),
			ostring("Scalar finishing value for grid x1 (right)"));
	n1.setNameAndDescription(ostring("n1"),
			ostring("Non-uniform grid exponent in x1"));
	x2s.setNameAndDescription(ostring("x2s"),
			ostring("Scalar starting value for grid x2 (bottom)"));
	x2f.setNameAndDescription(ostring("x2f"),
			ostring("Scalar finishing value for grid x2 (top)"));
	n2.setNameAndDescription(ostring("n2"),
			ostring("Non-uniform grid exponent in x2"));
	Geometry.setNameAndDescription(ostring("Geometry"),
			ostring("Geometry of mesh RZ (flag = 0; it is default value if parameter not given) or XY (flag = 1)"));
	Geometry.setValue("0");					//	default to ZRGEOM
	PeriodicFlagX1.setNameAndDescription("PeriodicFlagX1", "Flag: 0 not periodic, 1 periodic");
	PeriodicFlagX1.setValue("0");
	PeriodicFlagX2.setNameAndDescription("PeriodicFlagX2", "Flag: 0 not periodic, 1 periodic");
	PeriodicFlagX2.setValue("0");

	dx1.setNameAndDescription("dx1","Function scaling mesh spacing in X1");
	dx2.setNameAndDescription("dx2","Function scaling mesh spacing in X2");
	dx1.setValue("1");
	dx2.setValue("1");

	parameterList.add(&J);
	parameterList.add(&x1s);
	parameterList.add(&x1f);
	parameterList.add(&n1);
	parameterList.add(&K);
	parameterList.add(&x2s);
	parameterList.add(&x2f);
	parameterList.add(&n2);
	parameterList.add(&Geometry);
	parameterList.add(&PeriodicFlagX1);
	parameterList.add(&PeriodicFlagX2);
	parameterList.add(&dx1);
	parameterList.add(&dx2);

	// rules can be added during construction or by external messages
	addLimitRule(ostring("J"), ostring("<="), 3.0,
			ostring("Fatal -- J <= 3 field solve algorithm will fail"), 1);
	addRelationRule(ostring("x1f"), ostring("<"), ostring("x1s"),
			ostring("Fatal -- Starting grid value x1s must be less than end grid value x1f"), 1);
	addLimitRule("K", "<=", 3.0,
			"Fatal -- K <= 3 field solve algorithm will fail", 1);
	addRelationRule("x2f", "<", "x2s",
			"Fatal -- Starting grid value x2s must be less than end grid value x2f", 1);
	addLimitRule("K", "<=", 3.0,
			"Fatal -- K <= 3 field solve algorithm will fail", 1);
	addLimitRule("Geometry", "<", 0.0,
			"Fatal --  0 and 1 are valid geometries, RZ and XY respectively", 1);
	addLimitRule("Geometry", ">", 1.0,
			"Fatal --  0 and 1 are valid geometries, RZ and XY respectively", 1);

	X = NULL;            // jbha 9/95
}


GridParams::~GridParams()
{
//	deleteCellVertices(); NO - this is done in grid.cpp
}


Grid* GridParams::CreateCounterPart()
{
	if ((Geometry.getValue()==0)&&(PeriodicFlagX2.getValue()==1))
		printf("Can't be Periodic in the R direction");
	
	int JJ = J.getValue(), KK = K.getValue();
        // minimums and maximums of physical dimension for entire device along x1:
        Scalar x1min = getX1s();
        Scalar x1max = getX1f();  
        // difference between two successive points along x1 on the grid:
        Scalar deltaX1=(x1max - x1min)/((Scalar)JJ); 
	Grid* g = new Grid(JJ, KK, createCellVertices(x1min, deltaX1), Geometry.getValue(), 
										 PeriodicFlagX1.getValue(), PeriodicFlagX2.getValue());
	return g;
}


#ifdef MPI_VERSION

Grid* GridParams::CreateCounterPart(const ostring &MPIpartition) 
{
	int i,j,k;
	if ((Geometry.getValue()==0)&&(PeriodicFlagX2.getValue()==1))
		printf("Can't be Periodic in the R direction");
	
	if(PeriodicFlagX2.getValue() || PeriodicFlagX1.getValue()) {
	  printf("\nError:  Periodic models not yet supported in MPI version.\n");
	  exit(1); // MPI_EXIT
	}
	
	int JJ = J.getValue(), KK = K.getValue();
	int Jmax, Kmax;

	Scalar x1lmin,x2lmin,x1lmax,x2lmax;  // minimums and maximums of 
	                                  // physical dimensions for local region

	Scalar x1min,x2min,x1max,x2max;  // minimums and maximums of 
	                              // physical dimension for entire device.

	x1min = getX1s(); x2min = getX2s(); x1max = getX1f(); x2max = getX2f();
        Scalar deltaX1; // for the difference between two successive points along x1 on the grid
 
	int MPI_MAX_RANK;
	MPI_Comm_size(XOOPIC_COMM,&MPI_MAX_RANK);

	int M, N;  // partitions in x1 and y1, respectively.
	int Column, Row;

	/*  Now the fun part.  We've got to partition ourselves a smaller
		 segment of the total physical model for our local computational
		 domain.  */
	
	if(MPIpartition == "Nx1") { // one-D partitioning special case
	  Jmax = JJ / MPI_MAX_RANK;
	  N = 1; M = MPI_MAX_RANK;
	  Column = MPI_RANK % M;
	  Row = MPI_RANK/M;
          deltaX1 = (x1max - x1min)/((Scalar)JJ);
	  Kmax = KK;  //  no partitioning

          x1lmin = x1min;
          Scalar scalarJmax = (Scalar)Jmax;
          for ( i = 1; i <= MPI_RANK; i++ )
            x1lmin += deltaX1*scalarJmax;
          x1lmax = x1lmin + deltaX1*scalarJmax; 

	  x2lmin = x2min; x2lmax = x2max;

	  /*  need to add some more to this partition if we're the last partition
			and things don't quite fit.  */
	  if(MPI_RANK == MPI_MAX_RANK-1) {  // we're the last partition
		 Jmax += JJ % MPI_MAX_RANK;
		 x1lmax = x1max;
	  }
	}
	else
	  {  
            cerr << endl << "Only the case MPIpartition = Nx1 is implemented in " << endl
                 << "Grid* GridParams::CreateCounterPart(const ostring &MPIpartition)"
                 << endl << "Exiting to the system..." << endl;
            exit(1); // MPI_EXIT
            /*******************************************************
             * The code commented out in this block is broken and 
             * should not be used before fixing and testing it extensively:
            //  the more general case of an MxN partition 
		 Scalar x1lsize, x2lsize;
		 sscanf(MPIpartition.c_str(),"%dx%d",&M,&N);

		 Column = MPI_RANK % M;
		 Row = MPI_RANK / M;

		 if(M*N > MPI_MAX_RANK ) {
			printf("\nERROR:  you haven't got enough CPU's for the number of\n");
			printf("\nERROR:  partitions you specified in the input file.\n");
			exit(1);MPI_EXIT
		 }
		 Jmax = JJ/M;
		 Kmax = KK/N;
		 x1lsize = (Scalar)Jmax / (Scalar)JJ * (x1max - x1min);
		 x2lsize = (Scalar)Kmax / (Scalar)KK * (x2max - x2min);
		 x1lmin = x1lsize * Column + x1min;
		 //  the tertiary operators below add any left-over cells/area
		 //  to the last partition.
		 x1lmax = (M - Column == 1) ? x1max : x1lmin + x1lsize;
		 x2lmin = x2lsize * Row + x2min;
		 x2lmax = (N - Row == 1) ? x2max : x2lmin + x2lsize;
		 if(M-Column == 1) Jmax += JJ % M;
		 if(N-Row == 1) Kmax += KK % N;
            *******************************************************/	 
          } 
	//  Now I have to reset the values of x1s, x1f, x2s, x2f
        // NB: the x1s, x2s, x1f, x2f are of type ScalarParameter which
        //     keeps its data members as floats! If x1lmin, etc. are of
        //     type double, truncation will occur and precision will be lost.
        //     x1s and x1f are no longer used after this point but I've
        //     left the set calls for them as the code initially was in case
        //     developers decide to use them again. 
        //     However, without changing the data members of
        //     ScalarParameter to Scalar, this may cause problems again.
        //     dad Fri May  3, 2002
	//
	// I fixed the evaluator, so the type of ScalarParameter is now
	// Scalar rather than float. So truncation is no longer an issue.
	// RT, 2003/12/09
	x1s.setValue(x1lmin);
	x2s.setValue(x2lmin);
	x1f.setValue(x1lmax);
	x2f.setValue(x2lmax);

#if defined(MPI_DEBUG) && defined(MPI_VERSION)
        dfout << "x1lmin = " << x1lmin << "; x1s.getValue() = " << x1s.getValue() << endl
              << "x1lmax = " << x1lmax << "; x1f.getValue() = " << x1f.getValue() << endl
              << "x1lmax - x1lmin = " << x1lmax - x1lmin << "; x1f - x1s = " 
              << x1f.getValue() - x1s.getValue() << endl;
#endif /* defined(MPI_DEBUG) && defined(MPI_VERSION) */         

	J.setValue(Jmax); K.setValue(Kmax);
	Grid* g = new Grid(Jmax, Kmax, createCellVertices(x1lmin, deltaX1), Geometry.getValue(), 
                           PeriodicFlagX1.getValue(), PeriodicFlagX2.getValue());
	g->M = M; g->N = N; g->MaxMPIJ = JJ; g->MaxMPIK = KK;
	g->MaxMPIx1 = x1max; g->MaxMPIx2= x2max; 	g->MinMPIx1 = x1min; g->MinMPIx2= x2min;
	g->neighbor[0] = g->neighbor[1] = g->neighbor[2] = g->neighbor[3] = -1;

	// This segment sets up the global node numbering for a poisson solve.
	g->MPInodeDim = new Vector2 *[M];
	g->MPIgstarts = new int *[M];

	for(i=0;i<M;i++) {
	  g->MPInodeDim[i] = new Vector2[N];
	  g->MPIgstarts[i] = new int[N];
	}
	// find the dimensions of each region
	for(j=0;j<M;j++)
	  for(k=0;k<N;k++) 
		 g->MPInodeDim[j][k] = Vector2(JJ/M + (((M-j ==1)&&(M>1)) ? (JJ%M):0) ,
												 KK/N + (((N-k == 1) && (N>1) ) ? (KK %N):0));
	g->MPIgstarts[0][0]=0;

	//  find the start numbers for each ranked region:  each node must
	//  have a unique ID number, and we have to recover the ID numbers for
	//  setting up the matrix.  This is supposed to help.
	for(i=1;i<MPI_MAX_RANK;i++)
	  g->MPIgstarts[i%M][i/M]=g->MPIgstarts[(i-1)%M][(i-1)/M] + 
		 (int)((1+g->MPInodeDim[(i-1)%M][(i-1)/M].e1())*(1+g->MPInodeDim[(i-1)%M][(i-1)/M].e2()));
	  
		 
	// set the right-hand neighbor, if any
	g->neighbor[0] = (Column < M-1) ? MPI_RANK+1 : -1;
	g->neighbor[1] = (Column > 0 )  ? MPI_RANK-1 : -1;
	g->neighbor[2] = (Row < N-1 ) ? MPI_RANK + M : -1;
	g->neighbor[3] = (Row > 0 ) ? MPI_RANK - M : -1;
#ifdef MPI_DEBUG
	printf("\n%d:Grid: x1lmin: %f x1lmax %f x2lmin %f x2lmax %f",MPI_RANK,x1lmin,x1lmax,x2lmin,x2lmax);
	printf("\n%d: left %d right %d up %d down %d",MPI_RANK,g->neighbor[0],g->neighbor[1],g->neighbor[2],g->neighbor[3]);
	printf("\n%d: M %d N %d Row %d Column %d",MPI_RANK,M,N,Row,Column);
	printf("\n%d: Jmax %d Kmax %d",MPI_RANK,Jmax,Kmax); 
#endif
	
	return g;
}  

#endif /* MPI_VERSION */

Scalar GridParams::mapping_function(Scalar x, Scalar x1,
												Scalar x2, Scalar n)
{Scalar term = (x - x1)/(x2 - x1);
 return x1 + (x2 - x1)*pow(term, n);}

Vector2** GridParams::createCellVertices(Scalar x1min, Scalar deltaX1)
{
 int JJ = J.getValue(), KK = K.getValue();
 Scalar X2s = x2s.getValue();
 Scalar X2f = x2f.getValue();
 Scalar N2 = n2.getValue();
 ostring DX1 = dx1.getValue(), DX2=dx2.getValue();
 Scalar *x1_array= new Scalar[JJ+1]; 
 Scalar *x2_array= new Scalar[KK+1]; 

 Vector2** XX = new Vector2* [JJ+1];
 int j,k; //used as indices
 for (j = 0; j <= JJ; j++) XX[j] = new Vector2[KK+1];

  //first calculate x1_array, x2_array un-normalized
  x2_array[0]=x1_array[0]=0;
  DX1+='\n'; DX2+='\n';
  char buf[32];

  for(j=1; j<=JJ; j++) {
	 sprintf(buf,"j=%f\n",(Scalar)(j-0.5));
	 adv_eval->Evaluate(buf);
	 x1_array[j]=x1_array[j-1]+adv_eval->Evaluate(DX1.c_str());
  }
  for(k=1; k<=KK; k++) {
	 sprintf(buf,"k=%f\n",(Scalar)(k-0.5));
	 adv_eval->Evaluate(buf);
	 x2_array[k]=x2_array[k-1]+adv_eval->Evaluate(DX2.c_str());
  }

  // normalizations for the x2 arrays above:
  Scalar norm2=(X2f-X2s)/x2_array[KK];  
	 
  //scale by the normalizations.
  for(k=1; k<=KK; k++) x2_array[k]*=norm2;
  for(j=1; j<=JJ; j++) { 
    x1_array[j]*=deltaX1;
  }
  //Now we're ready to compute the actual location of the vertices
  for(j=0; j<=JJ; j++)
    for(k=0; k<=KK; k++) 
      XX[j][k]=Vector2(x1min+x1_array[j],X2s+x2_array[k]);
  X=XX;

#if defined(MPI_DEBUG) && defined(MPI_VERSION)
  dfout << endl << "GRID COORDINATES" << endl;
  for(j=0; j<=JJ; j++)
    for(k=0; k<=KK; k++) 
      dfout << "X[" << j << "][" << k << "] = ( "
            << X[j][k].e1() << ", " 
            << X[j][k].e2() << " )" << endl;
  dfout.close();
#endif /* defined(MPI_DEBUG) && defined(MPI_VERSION) */
 
 delete[] x1_array;
 delete[] x2_array;

	return XX;
}

Vector2** GridParams::getCellVertices()
{
	return X;
}

void GridParams::deleteCellVertices()
{
	if (X)
	{
		int JJ = J.getValue();
		for (int j = 0; j <= JJ; j++)
			if (X[j]) {delete[] X[j]; X[j] = NULL; }
		delete [] X;
		X = NULL;
	}
}

