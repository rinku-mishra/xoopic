#include "ovector.h"
#include "grid.h"

void boundary_Dirichlet(Scalar **, const intVector2&);
void boundary_Dirichlet_Zero(Scalar **, const intVector2&);
void boundary_Neumann_Zero(Scalar **, const intVector2&);
void GSRB_Boundary(Scalar**, Scalar**, const intVector2&, Scalar***, 
									 const int&, const int&, const int&);
void Residual_Boundary(Scalar**, Scalar**, Scalar**, const intVector2&, 
											 Scalar***, const int&, const int&);

#define NORTH 0
#define SOUTH 1
#define WEST 2
#define EAST 3
#define SOURCE 4

void GSRB(Scalar** p, Scalar** rhs, const intVector2& nhigh, Scalar ***Coeff, 
					const int& PeriodicFlagX1, const int& PeriodicFlagX2)
{
	int ioff, j, i, iinc;
	int nhigh1, nhigh2;
	nhigh1 = nhigh.e1();
	nhigh2 = nhigh.e2();
	Scalar* Cij;

	
	// Gauss - Seidel w/ Red - Black ordering, optimal parameter.
	for (ioff=0; ioff<2; ioff++){

		GSRB_Boundary(p, rhs, nhigh, Coeff, ioff, PeriodicFlagX1, PeriodicFlagX2);

		for (j=1; j<nhigh2; j++){
			iinc = (j+ioff)%2;
			for(i = 1 + iinc; i < nhigh1; i=i+2){
				Cij = Coeff[i][j];
				if (Cij[SOURCE]!=0.0)
					p[i][j] = p[i][j+1]*Cij[NORTH]+ Cij[SOUTH]*p[i][j-1] 
						+Cij[WEST]*p[i-1][j]+Cij[EAST]*p[i+1][j] + Cij[SOURCE]*rhs[i][j];
			}
		}
	}
}

void Interpolate(Scalar** fine,Scalar** coarse,const intVector2& nfhigh,
								 const intVector2& nchigh, Scalar ***FineCoeff)
{
	int nchigh1, nchigh2, i, j;
	
	nchigh1 = nchigh.e1();
	nchigh2 = nchigh.e2();
	if ((((Scalar)nfhigh.e1()/2)==nchigh.e1())&&(((Scalar)nfhigh.e2()/2)==nchigh.e2())){
		for(j = 1; j<=nchigh2; j++)
			for(i = 1; i <= nchigh1; i++){
				fine[2*i-1][2*j  ] +=  (FineCoeff[2*i-1][2*j  ][SOURCE]==0) ? 0 :  .5*(coarse[i-1][j]+coarse[i][j]); 
				fine[2*i  ][2*j-1] +=  (FineCoeff[2*i  ][2*j-1][SOURCE]==0) ? 0 :  .5*(coarse[i][j]+coarse[i][j-1]);
				fine[2*i-1][2*j-1] +=  (FineCoeff[2*i-1][2*j-1][SOURCE]==0) ? 0 : .25*(coarse[i][j]+coarse[i][j-1]+coarse[i-1][j-1]+coarse[i-1][j]); 
				fine[2*i  ][2*j  ] +=  (FineCoeff[2*i  ][2*j  ][SOURCE]==0) ? 0 :  coarse[i][j];
			}
		j=0;
		for(i = 1; i <= nchigh1; i++){
			fine[2*i-1][2*j  ] +=  (FineCoeff[2*i-1][2*j  ][SOURCE]==0) ? 0 : .5*(coarse[i-1][j]+coarse[i][j]); 
			fine[2*i  ][2*j  ] +=  (FineCoeff[2*i  ][2*j  ][SOURCE]==0) ? 0 :  coarse[i][j];
		}
		i=0;
		for(j = 1; j<=nchigh2; j++){
			fine[2*i  ][2*j-1] += (FineCoeff[2*i  ][2*j-1][SOURCE]==0) ? 0 : .5*(coarse[i][j]+coarse[i][j-1]);
			fine[2*i  ][2*j  ] += (FineCoeff[2*i  ][2*j  ][SOURCE]==0) ? 0 :  coarse[i][j];
		}
		j=0;
		fine[2*i  ][2*j  ] += (FineCoeff[2*i  ][2*j  ][SOURCE]==0) ? 0 : coarse[i][j];
	}
	else 	if (((nfhigh.e1()/2)==nchigh.e1())&&(nfhigh.e2()/2!=nchigh.e2()))	{
		for(j = 0; j<nchigh2; j++)
			for(i = 1; i < nchigh1; i++){
				fine[2*i-1][j] += (FineCoeff[2*i-1][j][SOURCE]==0) ? 0 : .5*(coarse[i][j]+coarse[i-1][j]);
				fine[2*i  ][j] += (FineCoeff[2*i  ][j][SOURCE]==0) ? 0 : coarse[i][j];
			}
		i=0;
		for(j = 0; j<nchigh2; j++)
			fine[2*i  ][j] += (FineCoeff[2*i  ][j][SOURCE]==0) ? 0 : coarse[i][j];
	}
	else if ((nfhigh.e1()/2!=nchigh.e1())&&(nfhigh.e2()/2==nchigh.e2())) {
		for(j = 1; j<nchigh2; j++)
			for(i = 0; i < nchigh1; i++)
				{
					fine[i][2*j-1] += (FineCoeff[i][2*j-1][SOURCE]==0) ? 0 :  .5*(coarse[i][j]+coarse[i][j-1]);
					fine[i][2*j  ] += (FineCoeff[i][2*j  ][SOURCE]==0) ? 0 :   coarse[i][j];
				}
		j=0;
		for(i = 0; i < nchigh1; i++)
			fine[i][2*j  ] += (FineCoeff[i  ][2*j  ][SOURCE]==0) ? 0 : coarse[i][j];
	}
}

void Average(Scalar** fine,Scalar** coarse,const intVector2& nfhigh, 
						 const intVector2& nchigh, const int& PeriodicFlagX1, const int& PeriodicFlagX2)
{
	int i,j;

	if ((((Scalar)nfhigh.e1()/2)==nchigh.e1())&&(((Scalar)nfhigh.e2()/2)==nchigh.e2())){
		for(j = 1; j < nchigh.e2(); j++)
			for(i = 1; i< nchigh.e1(); i++)
				coarse[i][j] = (fine[2*i-1][2*j-1] + fine[2*i-1][2*j+1] + 
												fine[2*i+1][2*j-1]+ fine[2*i+1][2*j+1] + 
												2*(fine[2*i-1][2*j] + 
													 fine[2*i][2*j-1] + fine[2*i][2*j+1]+ fine[2*i+1][2*j]) +
												4*fine[2*i][2*j])/16;
		if (PeriodicFlagX1){
			i=nchigh.e1();
			for(j = 1; j < nchigh.e2(); j++){
				coarse[i][j] = (fine[2*i-1][2*j-1] + fine[2*i-1][2*j+1] + 
												fine[1][2*j-1]+ fine[1][2*j+1] + 
												2*(fine[2*i-1][2*j] + 
													 fine[2*i][2*j-1] + fine[2*i][2*j+1]+ fine[1][2*j]) +
												4*fine[2*i][2*j])/16;
				coarse[0][j] = coarse[i][j];
			}
		}
		else {
			for(j = 1; j < nchigh.e2(); j++){
				i=0;
				coarse[i][j] = (fine[2*i+1][2*j-1]+ fine[2*i+1][2*j+1] + 
												2*(fine[2*i][2*j-1] + fine[2*i][2*j+1]+ fine[2*i+1][2*j]) +
												4*fine[2*i][2*j])/12;
				i=nchigh.e1();
				coarse[i][j] = (fine[2*i-1][2*j-1] + fine[2*i-1][2*j+1] + 
												2*(fine[2*i-1][2*j] + 
													 fine[2*i][2*j-1] + fine[2*i][2*j+1]) +
												4*fine[2*i][2*j])/12;
			}
		}
		if (PeriodicFlagX2){
			for(i = 1; i< nchigh.e1(); i++){
				j=nchigh.e2();
				coarse[i][j] = (fine[2*i-1][2*j-1] + fine[2*i-1][1] + 
												fine[2*i+1][2*j-1]+ fine[2*i+1][1] + 
												2*(fine[2*i-1][2*j] + 
													 fine[2*i][2*j-1] + fine[2*i][1]+ fine[2*i+1][2*j]) +
												4*fine[2*i][2*j])/16;
				coarse[i][0] = coarse[i][j];
			}
		}
		else{
			for(i = 1; i< nchigh.e1(); i++){
				j=0;
				coarse[i][j] = (fine[2*i-1][2*j+1] + 
												fine[2*i+1][2*j+1] + 
												2*(fine[2*i-1][2*j] + 
													 fine[2*i][2*j+1]+ fine[2*i+1][2*j]) +
												4*fine[2*i][2*j])/12;
				j=nchigh.e2();
				coarse[i][j] = (fine[2*i-1][2*j-1] + fine[2*i+1][2*j-1]+ 
												2*(fine[2*i-1][2*j] + 
													 fine[2*i][2*j-1] + fine[2*i+1][2*j]) +
												4*fine[2*i][2*j])/12;
			}
		}
		
		if (PeriodicFlagX1&&PeriodicFlagX2){
			i = nchigh.e1();
			j = nchigh.e2();
			coarse[i][j] = (fine[2*i-1][2*j-1] + fine[2*i-1][1] + 
												fine[1][2*j-1]+ fine[1][1] + 
												2*(fine[2*i-1][2*j] + 
													 fine[2*i][2*j-1] + fine[2*i][1]+ fine[1][2*j]) +
												4*fine[2*i][2*j])/16;
			coarse[0][j] = coarse[i][0] = coarse[0][0] = coarse[i][j];
		}  
		else if (PeriodicFlagX1){
			j = 0;
			i=nchigh.e1();
			coarse[i][j] = (fine[2*i-1][2*j+1] + 
											fine[1][2*j+1] + 
											2*(fine[2*i-1][2*j] + 
												 fine[2*i][2*j+1]+ fine[1][2*j]) +
											4*fine[2*i][2*j])/12;
			coarse[0][j] = coarse[i][j];
			j = nchigh.e2();
			coarse[i][j] = (fine[2*i-1][2*j-1] + 
											fine[1][2*j-1] + 
											2*(fine[2*i-1][2*j] + 
												 fine[2*i][2*j-1] + fine[1][2*j]) +
											4*fine[2*i][2*j])/12;
			coarse[0][j] = coarse[i][j];
		}
		else if (PeriodicFlagX2){
			i = nchigh.e1();
			j = nchigh.e2();
			coarse[i][j] = (fine[2*i-1][2*j-1] + fine[2*i-1][1] + 
											2*(fine[2*i-1][2*j] + 
												 fine[2*i][2*j-1] + fine[2*i][1]) +
											4*fine[2*i][2*j])/12;
			coarse[i][0] = coarse[i][j];
			i = 0;
			coarse[i][j] = (fine[2*i+1][2*j-1]+ fine[2*i+1][1] + 
												2*(fine[2*i][2*j-1] + fine[2*i][1]+ fine[2*i+1][2*j]) +
												4*fine[2*i][2*j])/12;
			coarse[i][0] = coarse[i][j];
		}
		else{
			i=0;j=0;
			coarse[i][j] = (fine[2*i+1][2*j+1] + 
											2*(fine[2*i][2*j+1]+ fine[2*i+1][2*j]) +
											4*fine[2*i][2*j])/9;
			i=nchigh.e1();
			coarse[i][j] = (fine[2*i-1][2*j+1] + 
											2*(fine[2*i-1][2*j] + fine[2*i][2*j+1])+
											4*fine[2*i][2*j])/9;	

			j=nchigh.e2();
			coarse[i][j] = (fine[2*i-1][2*j-1] + 
											2*(fine[2*i-1][2*j] +	fine[2*i][2*j-1]) + 
											4*fine[2*i][2*j])/9;

			i=0;
			coarse[i][j] = (fine[2*i+1][2*j-1]+
											2*(fine[2*i][2*j-1] + fine[2*i+1][2*j]) +
											4*fine[2*i][2*j])/9;
		}
	}
	
	else 	if (((nfhigh.e1()/2)==nchigh.e1())&&(nfhigh.e2()/2!=nchigh.e2())){
		for(j = 0; j <= nchigh.e2(); j++)
			for(i = 1; i< nchigh.e1(); i++)
				coarse[i][j] = .25*(fine[2*i+1][j]+2*fine[2*i][j] + fine[2*i-1][j]);

		if	(PeriodicFlagX1){
			for (j = 0; j <= nchigh.e2(); j++){
				i=nchigh.e1(); 
				coarse[i][j] = .25*(fine[1][j]+2*fine[2*i][j] + fine[2*i-1][j]);
				coarse[0][j] = coarse[i][j];
			}
		}
		else {
			for(j = 0; j <= nchigh.e2(); j++){
				i=0;
				coarse[i][j] = (fine[2*i+1][j]+2*fine[2*i][j])/3;
				i=nchigh.e1();
				coarse[i][j] = (2*fine[2*i][j] + fine[2*i-1][j])/3;
			}
		}
	}

	
	else if ((nfhigh.e1()/2!=nchigh.e1())&&(nfhigh.e2()/2==nchigh.e2())){
		for(j = 1; j < nchigh.e2(); j++)
			for(i = 0; i<= nchigh.e1(); i++)
				coarse[i][j] = .25*(fine[i][2*j+1] + 2*fine[i][2*j] + fine[i][2*j-1]);
		
	
		if (PeriodicFlagX2){
			for(i = 0; i<= nchigh.e1(); i++){
				j = nchigh.e2();
				coarse[i][j] = .25*(fine[i][1] + 2*fine[i][2*j] + fine[i][2*j-1]);
				coarse[i][0] = coarse[i][j];
			}
		}
		else{
			for(i = 0; i<= nchigh.e1(); i++){
				j=0;
				coarse[i][j] = (fine[i][2*j+1] + 2*fine[i][2*j])/3;
				j=nchigh.e2();
				coarse[i][j] = (2*fine[i][2*j] + fine[i][2*j-1])/3;
			}
		}
	}
	
}

void Sample(Scalar** fine,Scalar** coarse,const intVector2& nfhigh, 
						const intVector2& nchigh)
{
	int i,j;

	if ((((Scalar)nfhigh.e1()/2)==nchigh.e1())&&(((Scalar)nfhigh.e2()/2)==nchigh.e2())){
		for(j = 0; j <= nchigh.e2(); j++)
			for(i = 0; i <= nchigh.e1(); i++)
				coarse[i][j] = fine[2*i][2*j];
	}

	else 	if (((nfhigh.e1()/2)==nchigh.e1())&&(nfhigh.e2()/2!=nchigh.e2())){
		for(j = 0; j <= nchigh.e2(); j++)
			for(i = 1; i <= nchigh.e1(); i++)
				coarse[i][j] = fine[2*i][j];
	}
	
	else if ((nfhigh.e1()/2!=nchigh.e1())&&(nfhigh.e2()/2==nchigh.e2())){
		for(j = 0; j <= nchigh.e2(); j++)
			for(i = 0; i<= nchigh.e1(); i++)
				coarse[i][j] = fine[i][2*j];
	}
	
}
void Insert(Scalar** fine,Scalar** coarse,const intVector2& nfhigh,
						const intVector2& nchigh, Scalar ***FineCoeff)
{
	int nchigh1, nchigh2, i, j;
	
	nchigh1 = nchigh.e1();
	nchigh2 = nchigh.e2();
	if ((((Scalar)nfhigh.e1()/2)==nchigh.e1())&&(((Scalar)nfhigh.e2()/2)==nchigh.e2())){
		for(j = 0; j<=nchigh2; j++)
			for(i = 0; i <= nchigh1; i++)
				fine[2*i  ][2*j  ] +=  (FineCoeff[2*i  ][2*j  ][SOURCE]==0) ? 0 :  coarse[i][j];
	}
	
	else 	if (((nfhigh.e1()/2)==nchigh.e1())&&(nfhigh.e2()/2!=nchigh.e2()))	{
		for(j = 0; j<nchigh2; j++)
			for(i = 0; i < nchigh1; i++)
				fine[2*i  ][j] += (FineCoeff[2*i  ][j][SOURCE]==0) ? 0 : coarse[i][j];
	}

	else if ((nfhigh.e1()/2!=nchigh.e1())&&(nfhigh.e2()/2==nchigh.e2())) {
		for(j = 0; j<nchigh2; j++)
			for(i = 0; i < nchigh1; i++)
				fine[i][2*j  ] += (FineCoeff[i][2*j  ][SOURCE]==0) ? 0 : coarse[i][j];
	}
}


void CellAve(Scalar** fine,Scalar** coarse,const intVector2& nfhigh, 
						 const intVector2& nchigh)
{
	int i,j;

	if ((((Scalar)nfhigh.e1()/2)==nchigh.e1())&&(((Scalar)nfhigh.e2()/2)==nchigh.e2())){
		for(j = 0; j < nchigh.e2(); j++)
			for(i = 0; i < nchigh.e1(); i++)
				coarse[i][j] = (fine[2*i][2*j]+fine[2*i+1][2*j]+
												fine[2*i][2*j+1]+fine[2*i+1][2*j+1])/4;
	}

	else 	if (((nfhigh.e1()/2)==nchigh.e1())&&(nfhigh.e2()/2!=nchigh.e2())){
		for(j = 0; j < nchigh.e2(); j++)
			for(i = 0; i < nchigh.e1(); i++)
				coarse[i][j] = (fine[2*i][j]+fine[2*i+1][j])/2;
	}
	
	else if ((nfhigh.e1()/2!=nchigh.e1())&&(nfhigh.e2()/2==nchigh.e2())){
		for(j = 0; j < nchigh.e2(); j++)
			for(i = 0; i< nchigh.e1(); i++)
				coarse[i][j] = (fine[i][2*j]+fine[i][2*j+1])/2;
	}
	
}

Scalar Norm(Scalar** res, const intVector2& nhigh, Grid *grid)
{
	Scalar resnorm = 0.0;
	for (int j=0; j<= nhigh.e2(); j++)
		for (int i=0; i<= nhigh.e1(); i++)
			resnorm += sqr(res[i][j]);
//			resnorm += sqr(res[i][j]*grid->get_halfCellVolumes()[i][j]);

	resnorm = sqrt(resnorm);
	
	return resnorm;
}

void Residual(Scalar** p,Scalar** rhs,Scalar** res, 
							const intVector2 &nhigh, Scalar*** Coeff, const int& PeriodicFlagX1,
							const int& PeriodicFlagX2)
{
	int j,k;
	Scalar* Cjk;

	Residual_Boundary(p, rhs, res, nhigh, Coeff, PeriodicFlagX1, PeriodicFlagX2);	

	for(j = 1; j<nhigh.e1(); j++)
		for(k = 1; k<nhigh.e2(); k++){
			Cjk = Coeff[j][k];
			if (Cjk[SOURCE]!=0)
				res[j][k] = -p[j][k]*Cjk[SOURCE] - p[j-1][k]*Cjk[WEST] 
					- p[j+1][k]*Cjk[EAST] 
						- p[j][k-1]*Cjk[SOUTH] - p[j][k+1]*Cjk[NORTH] + rhs[j][k];

		}
}

void Residual_Boundary(Scalar** p,Scalar** rhs,Scalar** res, 
											 const intVector2 &nhigh,Scalar*** Coeff,
											 const int& PeriodicFlagX1, const int& PeriodicFlagX2)
{
	int i,j;
	Scalar* Cij;
	int nhigh1, nhigh2;
	
	nhigh1 = nhigh.e1();
	nhigh2 = nhigh.e2();

	if (PeriodicFlagX2){
		j=0;
		for(i = 1; i < nhigh1; i++){
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0){
				res[i][j] = -p[i][j+1]*Cij[NORTH] - Cij[WEST]*p[i-1][j] - 
					Cij[EAST]*p[i+1][j] - Cij[SOUTH]*p[i][nhigh2-1] 
						- Cij[SOURCE]*p[i][j] + rhs[i][j];
				res[i][nhigh2] = res[i][j];
			}
		}
	}
	else{
		j=0;
		for(i = 1; i < nhigh1; i++){
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0)
				res[i][j] = -p[i][j+1]*Cij[NORTH] - Cij[WEST]*p[i-1][j] - Cij[EAST]*p[i+1][j] 
					- Cij[SOURCE]*p[i][j] + rhs[i][j];
		}
		
		j=nhigh2;
		for(i = 1; i < nhigh1; i++){
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0)
				res[i][j] = -Cij[SOUTH]*p[i][j-1]-Cij[WEST]*p[i-1][j]-Cij[EAST]*p[i+1][j] 
					- Cij[SOURCE]*p[i][j] + rhs[i][j];
		}
	}
	
	if (PeriodicFlagX1){
		i = 0;
		for (j=1; j<nhigh2; j++){
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0){
				res[i][j] = -p[i][j+1]*Cij[NORTH]- Cij[SOUTH]*p[i][j-1] 
					- Cij[EAST]*p[i+1][j] - Cij[WEST]*p[nhigh1-1][j]  
						- Cij[SOURCE]*p[i][j] + rhs[i][j];
				res[nhigh1][j] = res[i][j];
			}
		}
	}
	else{
		i = 0;
		for (j=1; j<nhigh2; j++){
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0)
				res[i][j] = -p[i][j+1]*Cij[NORTH]- Cij[SOUTH]*p[i][j-1] 
					-Cij[EAST]*p[i+1][j] - Cij[SOURCE]*p[i][j] + rhs[i][j];
		}
		
		i = nhigh1;
		for (j=1; j<nhigh2; j++){
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0)
				res[i][j] = -p[i][j+1]*Cij[NORTH]- Cij[SOUTH]*p[i][j-1] 
					-Cij[WEST]*p[i-1][j] - Cij[SOURCE]*p[i][j] +rhs[i][j];
		}
	}

	if (!PeriodicFlagX1&&!PeriodicFlagX2){
		i=0;
		j=0;
		Cij = Coeff[i][j];
		if (Cij[SOURCE]!=0)
			res[i][j] = -p[i][j+1]*Cij[NORTH]-Cij[EAST]*p[i+1][j] 
				- Cij[SOURCE]*p[i][j]+rhs[i][j];
		
		i=nhigh1;
		Cij = Coeff[i][j];
		if (Cij[SOURCE]!=0)
			res[i][j] = -p[i][j+1]*Cij[NORTH]-Cij[WEST]*p[i-1][j] 
				- Cij[SOURCE]*p[i][j] + rhs[i][j];
		
		j=nhigh2;
		Cij = Coeff[i][j];
		if (Cij[SOURCE]!=0)
			res[i][j] = -Cij[SOUTH]*p[i][j-1] -Cij[WEST]*p[i-1][j]
				- Cij[SOURCE]*p[i][j] + rhs[i][j];
		
		i=0;
		Cij = Coeff[i][j];
		if (Cij[SOURCE]!=0)
			res[i][j] = -Cij[SOUTH]*p[i][j-1] -Cij[EAST]*p[i+1][j] 
				- Cij[SOURCE]*p[i][j] + rhs[i][j];
	}
	else if (PeriodicFlagX1&&PeriodicFlagX2){
		i=0;
		j=0;
		Cij = Coeff[i][j];
		if (Cij[SOURCE]!=0){
			res[i][j] = -p[i][j+1]*Cij[NORTH]-Cij[EAST]*p[i+1][j] 
				-Cij[WEST]*p[nhigh1-1][j] - Cij[SOUTH]*p[i][nhigh2-1] - 
					Cij[SOURCE]*p[i][j]+rhs[i][j];
			res[nhigh1][j] = res[i][nhigh2] = res[nhigh1][nhigh2] = res[j][j];
		}				
	}
	else if (PeriodicFlagX1){
		i=0;
		j=0;
		Cij = Coeff[i][j];
		if (Cij[SOURCE]!=0){
			res[i][j] = -p[i][j+1]*Cij[NORTH]-Cij[EAST]*p[i+1][j]
				- Cij[WEST]*p[nhigh1-1][j]
					- Cij[SOURCE]*p[i][j]+rhs[i][j];
			res[nhigh1][j] = res[i][j];
		}
		
		j=nhigh2;
		Cij = Coeff[i][j];
		if (Cij[SOURCE]!=0){
			res[i][j] = -Cij[SOUTH]*p[i][j-1] -Cij[WEST]*p[nhigh1-1][j]
				- Cij[EAST]*p[i+1][j] 
					- Cij[SOURCE]*p[i][j] + rhs[i][j];
			res[nhigh1][j] = res[i][j];
		}
	}
	else{
		i=0;
		j=0;
		Cij = Coeff[i][j];
		if (Cij[SOURCE]!=0){
			res[i][j] = -p[i][j+1]*Cij[NORTH]-Cij[EAST]*p[i+1][j]
				- Cij[SOUTH]*p[i][nhigh2-1]
					- Cij[SOURCE]*p[i][j]+rhs[i][j];
			res[i][nhigh2] = res[i][j];
		}
		i=nhigh1;
		Cij = Coeff[i][j];
		if (Cij[SOURCE]!=0){
			res[i][j] = -p[i][j+1]*Cij[NORTH]-Cij[WEST]*p[i-1][j] 
				- Cij[SOUTH]*p[i][nhigh2-1]
					- Cij[SOURCE]*p[i][j] + rhs[i][j];
			res[i][nhigh2] =res[i][j];
		}
	}
}
void GSRB_Boundary(Scalar** p, Scalar** rhs, const intVector2& nhigh, 
									 Scalar*** Coeff, const int& ioff, const int& PeriodicFlagX1,
									 const int& PeriodicFlagX2)
{
	int i,j,iinc;
	Scalar* Cij;

	int nhigh1, nhigh2;
	
	nhigh1 = nhigh.e1();
	nhigh2 = nhigh.e2();

	if (!PeriodicFlagX2){
		j=0;
		iinc = (j+ioff)%2;
		for(i = 1 + iinc; i < nhigh1; i=i+2){
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0)
				p[i][j] = p[i][j+1]*Cij[NORTH]+Cij[WEST]*p[i-1][j]+Cij[EAST]*p[i+1][j] 
					+ Cij[SOURCE]*rhs[i][j];
		}
		
		j=nhigh2;
		iinc = (j+ioff)%2;
		for(i = 1 + iinc; i < nhigh1; i=i+2){
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0)
				p[i][j] = Cij[SOUTH]*p[i][j-1]+Cij[WEST]*p[i-1][j]+Cij[EAST]*p[i+1][j] 
					+ Cij[SOURCE]*rhs[i][j];
		}
	}
	else{
		j=0;
		iinc = (j+ioff)%2;
		for(i = 1 + iinc; i < nhigh1; i=i+2){
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0){
				p[i][j] = p[i][j+1]*Cij[NORTH]+Cij[WEST]*p[i-1][j]+Cij[EAST]*p[i+1][j] 
					+Cij[SOUTH]*p[i][nhigh2-1]
						+ Cij[SOURCE]*rhs[i][j];
				p[i][nhigh2] = p[i][j];
			}
		}
	}
	
	if (!PeriodicFlagX1){
		i = 0;
		iinc = (i+ioff)%2;
		for (j=1+iinc; j<nhigh2; j=j+2){
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0)
				p[i][j] = p[i][j+1]*Cij[NORTH]+ Cij[SOUTH]*p[i][j-1] 
					+Cij[EAST]*p[i+1][j] + Cij[SOURCE]*rhs[i][j];
		}
		
		i = nhigh1;
		iinc = (i+ioff)%2;
		for (j=1+iinc; j<nhigh2; j=j+2){
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0)
				p[i][j] = p[i][j+1]*Cij[NORTH]+ Cij[SOUTH]*p[i][j-1] 
					+Cij[WEST]*p[i-1][j] + Cij[SOURCE]*rhs[i][j];
		}
	}
	else{
		i = 0;
		iinc = (i+ioff)%2;
		for (j=1+iinc; j<nhigh2; j=j+2){
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0){
				p[i][j] = p[i][j+1]*Cij[NORTH]+ Cij[SOUTH]*p[i][j-1] 
					+Cij[EAST]*p[i+1][j] + Cij[WEST]*p[nhigh1-1][j] + Cij[SOURCE]*rhs[i][j];
				p[nhigh1][j] = p[i][j];
			}
		}
	}		

	if (!PeriodicFlagX1&&!PeriodicFlagX2){
		if (ioff){
			i=0;
			j=0;
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0)
				p[i][j] = p[i][j+1]*Cij[NORTH]+Cij[EAST]*p[i+1][j] 
					+ Cij[SOURCE]*rhs[i][j];
		}
		
		if ((nhigh1%2)!=ioff){
			i=nhigh1;
			j=0;
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0)
				p[i][j] = p[i][j+1]*Cij[NORTH]+Cij[WEST]*p[i-1][j] 
					+ Cij[SOURCE]*rhs[i][j];
		}
		
		if (((nhigh1+nhigh2)%2)!=ioff){
			i=nhigh1;
			j=nhigh2;
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0)
				p[i][j] = Cij[SOUTH]*p[i][j-1] +Cij[WEST]*p[i-1][j]
					+ Cij[SOURCE]*rhs[i][j];
		}
		
		if ((nhigh2%2)!=ioff){
			i=0;
			j=nhigh2;
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0)
				p[i][j] = Cij[SOUTH]*p[i][j-1] +Cij[EAST]*p[i+1][j] 
					+ Cij[SOURCE]*rhs[i][j];
		}
	}
	else if (PeriodicFlagX1&&PeriodicFlagX2){
		if (ioff){
			i=0;
			j=0;
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0){
				p[i][j] = p[i][j+1]*Cij[NORTH]+Cij[EAST]*p[i+1][j]
					+ Cij[SOUTH]*p[i][nhigh2-1] + Cij[WEST]*p[nhigh1-1][j]
						+ Cij[SOURCE]*rhs[i][j];
				p[i][nhigh2]=p[nhigh1][j]=p[nhigh1][nhigh2]=p[i][j];
			}
		}
	}
	else if (PeriodicFlagX1){
		if (ioff){
			i=0;
			j=0;
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0){
				p[i][j] = p[i][j+1]*Cij[NORTH]+Cij[EAST]*p[i+1][j] 
					+ Cij[WEST]*p[nhigh1-1][j]
						+ Cij[SOURCE]*rhs[i][j];
				p[nhigh1][j] = p[i][j];
			}
		}
		
		
		if ((nhigh2%2)!=ioff){
			j=nhigh2;
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0){
				p[i][j] = Cij[SOUTH]*p[i][j-1] +Cij[EAST]*p[i+1][j] 
					+ Cij[WEST]*p[nhigh1-1][j]
						+ Cij[SOURCE]*rhs[i][j];
				p[nhigh1][j] = p[i][j];
			}	
		}
	}
	else{
		if (ioff){
			i=0;
			j=0;
			Cij = Coeff[i][j];
			if (Cij[SOURCE]!=0){
				p[i][j] = p[i][j+1]*Cij[NORTH]+Cij[EAST]*p[i+1][j] 
					+ Cij[SOUTH]*p[i][nhigh2]
						+ Cij[SOURCE]*rhs[i][j];
				p[i][nhigh2-1] = p[i][j];
			}
			
			if ((nhigh1%2)!=ioff){
				i=nhigh1;
				Cij = Coeff[i][j];
				if (Cij[SOURCE]!=0){
					p[i][j] = p[i][j+1]*Cij[NORTH]+Cij[WEST]*p[i-1][j]
						+ Cij[SOUTH]*p[i][nhigh2]
							+ Cij[SOURCE]*rhs[i][j];
					p[i][nhigh2-1] = p[i][j];
				}
			}
		}	
	}	
}
