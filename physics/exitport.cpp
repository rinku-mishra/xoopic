/*
	 ====================================================================

	 EXITPORT.CPP

	 Started spring 94 for a 290q project.
	 Using Surface Impedance Boundary Conditions (SIBC)
	 This work is following the paper  Finite-Difference Time-Domain
	 Implementation of Surface Impedance Boundary Conditions  by J.H. Beggs,
	 Luebbers, Yee, and Kunz in IEEE Transactions on Antennas
	 and Propagation, Vol. 40, No. 1, January 1992.

	 Changes from the paper:
	 In the paper a new H is found on the boundary.  This is changed
	 so that E is give on the boundary.

	 The boundary condition that is discussed on page 9 of 2D electronmagnetics 
	 and PIC on a quadrilateral mesh by Bruce Langdon is a special case of
	 the boundary condtion programed here.  If L=0 and R=mu0*c = 120PI ohms.

	 ====================================================================
	 */

#include "port.h"
#include "fields.h"
#include "ptclgrp.h"
#include "exitport.h"
#include "dump.h"

Scalar ExitPort::SourceTM(int j, int k, int index)
{
  Scalar iCeq=0;
  Scalar source=0;
  if (C)
    iCeq = 1/C;
  if (Cin)
    iCeq += 1/Cin*ATM[index];
  source = (R-Rin*ATM[index])*get_time_value(time)+
    (L-Lin*ATM[index])*get_time_value_deriv(time)+
    iCeq*get_time_value_int(time) + IntTM[index];
  return(source);
}

Scalar ExitPort::SourceTE(int j, int k, int index)
{
  Scalar source=0;
  Scalar iCeq=0;
  int on=0;
  if (on)
    {
      if (C)
        iCeq = 1/C;
      if (Cin)
        iCeq += 1/Cin*ATE[index];
      source = (R-Rin*ATE[index])*get_time_value(time)+
        (L-Lin*ATE[index])*get_time_value_deriv(time)+
        iCeq*get_time_value_int(time) + IntTE[index];
    }
  else
    source = 0;

  return(source);
}

ExitPort::ExitPort(oopicList <LineSegment> *segments, Scalar _RR, Scalar _LL, Scalar _CC, 
                   Scalar _Rin, Scalar _Lin, Scalar _Cin, 
                   int EFFlag)
  : Port(segments)
{
  if(EFFlag)
    {
      EnergyFlux = &EnergyOut;
      EnergyOut =0.0;
    }
  Rin = _Rin;
  Lin = _Lin;
  Cin = _Cin;
  R = _RR;
  L = _LL;
  C = _CC;
  BoundaryType = EXITPORT;
  // getting memory for old intEdl and old intBdS to get dD/dt
  tOld = 0.0;
  dt = 0.0;
  init = TRUE;

  if (alongx2())	// vertical
    {
      int kmax = abs(k2-k1) + 1;
      oldHonBoundary = new Vector2[kmax];
      iplusTE = new Scalar[kmax];
      minusTE = new Scalar[kmax];
      ATE = new Scalar[kmax];
      idtCTE = new Scalar[kmax];
      IntTE =  new Scalar[kmax];
      iplusTM = new Scalar[kmax];
      minusTM = new Scalar[kmax];
      ATM = new Scalar[kmax];
      idtCTM = new Scalar[kmax];
      IntTM =  new Scalar[kmax];
      for (int k = 0; k<kmax; k++)
        {
          oldHonBoundary[k] = Vector2(0,0);
          iplusTE[k] = 0;
          minusTE[k] =0;
          ATE[k] =0;
          idtCTE[k] =0;
          IntTE[k] =0;
          iplusTM[k] = 0;
          minusTM[k] =0;
          ATM[k] =0;
          idtCTM[k] =0;
          IntTM[k] = 0;
        }
    }
  else // horizontal
    {
      int jmax = abs(j2-j1) + 1;
      oldHonBoundary = new Vector2[jmax];
      iplusTE = new Scalar[jmax];
      minusTE = new Scalar[jmax];
      ATE = new Scalar[jmax];
      idtCTE = new Scalar[jmax];
      IntTE = new Scalar[jmax];
      iplusTM = new Scalar[jmax];
      minusTM = new Scalar[jmax];
      ATM = new Scalar[jmax];
      idtCTM = new Scalar[jmax];
      IntTM = new Scalar[jmax];
      for (int j = 0; j<jmax; j++)
        {
          oldHonBoundary[j] = Vector2(0,0);
          iplusTE[j] = 0;
          minusTE[j] =0;
          ATE[j] =0;
          idtCTE[j] =0;
          IntTE[j] =0;
          iplusTM[j] = 0;
          minusTM[j] =0;
          ATM[j] =0;
          idtCTM[j] =0;
          IntTM[j] = 0;
        }
    }
}

ExitPort::~ExitPort()
{
  delete[] oldHonBoundary;
  delete[] ATE;
  delete[] iplusTE;
  delete[] minusTE;
  delete[] idtCTE;
  delete[] ATM;
  delete[] iplusTM;
  delete[] minusTM;
  delete[] idtCTM;
}

//--------------------------------------------------------------------
//	Apply boundary conditions to fields locally.


void ExitPort::applyFields(Scalar t,Scalar dt)
{

  if(!t) return;  // don't need to do this at t=0, crashes in fact.
  if (init) initialize(t,dt);
  time = t - dt/2;
  //	cout << "time " << time << endl;
  Scalar H3, dE2dl, E2dl, HonBoundary, Stuff, H2, dE3dl;
  Scalar EonBoundary, E3dl, H1, dE1dl, E1dl;
  Scalar E2origdl, E3origdl, E1origdl;
  int Kdex, Jdex;

  if (fields->getSub_Cycle_Iter() ==1) EnergyOut = 0.0;
  else if (fields->getSub_Cycle_Iter() ==0) EnergyOut = 0.0;

  //	EnergyOut = 0.0;

  if (alongx2())			//	vertical
    {
      // wave going to the right normal = -1, shift = -1
      // wave going to the left normal = 1, shift = 0
      for (int k = k1; k < k2; k++)
        {
          Kdex = k-k1;
          // TM mode
          E2origdl = IntEdl[j2][k].e2();
          H3 =  (iL[j2+shift][k].e3())*(IntBdS[j2+shift][k].e3());
          Stuff = 2*H3 + normal*I[j2][k].e2();
          dE2dl = normal*(Rprime*Stuff - 2*Lprime*oldHonBoundary[Kdex].e1()) 
            - 2*SourceTM(j2,k,Kdex);
          E2dl = (minusTM[Kdex]*E2origdl - dE2dl)*iplusTM[Kdex];
          dE2dl = E2dl - E2origdl;
          EonBoundary = .5*(E2dl + E2origdl)/grid->dl2(j2,k);
          HonBoundary = .5*(Stuff + normal*idtCTM[Kdex]*dE2dl);
          fields->setIntEdl(j2, k, 2, E2dl);
          IntTM[Kdex] += iCdt2*(HonBoundary + oldHonBoundary[Kdex].e1());
          oldHonBoundary[Kdex].set_e1(HonBoundary);
          Scalar g = grid->dl3(j2+shift,k);
          if (g!=0)
            EnergyOut += normal*EonBoundary*HonBoundary*grid->dS(j2+shift,k).e1()/g;

          // TE mode
          E3origdl = IntEdl[j2][k].e3();
          H2 =  (iL[j2+shift][k].e2())*(IntBdS[j2+shift][k].e2());
          if (k==0)
            {
              Stuff = normal*I[j2][k].e3() + 2*H2;
            }
          else
            {
              Stuff = normal*(-iL[j2][k].e1()*IntBdS[j2][k].e1() +
                              iL[j2][k-1].e1()*IntBdS[j2][k-1].e1() -
                              I[j2][k].e3()) + 2*H2;
            }
          dE3dl = normal*(Rprime*Stuff + 2*Lprime*oldHonBoundary[Kdex].e2())
            - 2*SourceTE(j2,k,Kdex);
          E3dl = (minusTE[Kdex]*E3origdl + dE3dl)*iplusTE[Kdex];
          dE3dl = (E3dl - E3origdl);
          HonBoundary =  .5*(Stuff - normal*idtCTE[Kdex]*dE3dl);
          g = grid->dl3(j2,k);
          if (g!=0)
            EonBoundary = .5*(E3dl + E3origdl)/g;
          else
            EonBoundary = 0;
          fields->setIntEdl(j2, k, 3, E3dl);
          IntTE[Kdex] += iCdt2*(HonBoundary + oldHonBoundary[Kdex].e2());
          oldHonBoundary[Kdex].set_e2(HonBoundary);
          g = grid->dl2(j2+shift,k);
          if (g!=0)
            EnergyOut -= normal*EonBoundary*HonBoundary*grid->dS(j2+shift,k).e1()/g;
        }
    }
  else						// 	horizontal
    {
      // wave going in (smaller y) normal = +1, shift = 0
      // wave going out (bigger y) normal = -1, shift = -1
      for (int j=MIN(j1,j2); j<MAX(j1,j2); j++)
        {
          Jdex = j-j1;
          // TM mode
          E1origdl = IntEdl[j][k2].e1();
          H3 =  iL[j][k2+shift].e3()*IntBdS[j][k2+shift].e3();
          Stuff = 2*H3-normal*I[j][k2].e1();
          dE1dl = normal*(Rprime*Stuff - 2*Lprime*oldHonBoundary[Jdex].e1()) - 2*SourceTM(j,k2,Jdex);
          E1dl = (minusTM[Jdex]*E1origdl + dE1dl)*iplusTM[Jdex];
          dE1dl = E1dl - E1origdl;
          EonBoundary = .5*(E1origdl+E1dl)/grid->dl1(j,k2);
          HonBoundary = .5*(Stuff - normal*idtCTM[Jdex]*dE1dl);
          fields->setIntEdl(j, k2, 1, E1dl);
          oldHonBoundary[Jdex].set_e1(HonBoundary);
          EnergyOut -= normal*EonBoundary*HonBoundary*grid->dS(j,k2+shift).e2()/
            (grid->dl(j,k2+shift).e3());

          // TE mode
          if (j==0) 	fields->setIntEdl(0, k2, 3, 0);
          else
            {
              E3origdl = IntEdl[j][k2].e3();
              H1 = (iL[j][k2+shift].e1())*(IntBdS[j][k2+shift].e1());
              Stuff = 2*H1 
                -normal*(iL[j][k2].e2()*IntBdS[j][k2].e2() - 
                         iL[j-1][k2].e2()*IntBdS[j-1][k2].e2() + 
                         I[j][k2].e3());
              dE3dl = -normal*(Rprime*Stuff - 2*Lprime*oldHonBoundary[Jdex].e2()) - 2*SourceTE(j,k2,Jdex);
              E3dl = (minusTE[Jdex]*E3origdl + dE3dl)*iplusTE[Jdex];
              dE3dl = E3dl - E3origdl;
              EonBoundary = .5*(E3origdl + E3dl)/grid->dl3(j,k2);
              HonBoundary = .5*(Stuff + normal*idtCTE[Jdex]*dE3dl);
              fields->setIntEdl(j, k2, 3, E3dl);
              oldHonBoundary[Jdex].set_e2(HonBoundary);
              EnergyOut += normal*EonBoundary*HonBoundary*grid->dS(j,k2+shift).e2()/
                (grid->dl(j,k2+shift).e1());
            }
        }
    }
  if (fields->getFieldSub() == fields->getSub_Cycle_Iter()) EnergyOut /= (fields->getFieldSub());
}

void ExitPort::initialize(Scalar t,Scalar dt)
{
  grid = fields->get_grid();
  iL = fields->getiL();
  IntBdS = fields->getIntBdS();
  IntEdl = fields->getIntEdl();
  I = fields->getI();
  Lprime = L/dt;
  normal = get_normal();
  if (normal == 1) shift = 0;
  else shift = -1;
  if (C)
    {
      Rprime = R+Lprime+dt/(4*C);
      iCdt2 = dt/(2*C);
      Lprime -= iCdt2;
    }
  else 
    {
      Rprime = R+Lprime;
      iCdt2 = 0;
    }
	
  Scalar RidtC, dl3local;
  int Jdex, Kdex;

  if (alongx2())			//	vertical
    {
      // wave going to the right normal = -1 
      // wave going to the left normal = 1
      for (int k = k1; k < k2; k++)
        {
          Kdex = k-k1;
          // TM mode
          if (grid->query_geometry())
            dl3local = 1;
          else
            dl3local = 2*M_PI*(grid->getMKS(Vector2(j1+shift, k + .5))).e2();
          ATM[Kdex] = dl3local/(grid->dl2(j1,k));
          if(fields->getiC()[j2+normal][k].e2())
            {
              idtCTM[Kdex] = 1/(dt*fields->getiC()[j2+normal][k].e2()); // iC j2 is equal to zero because of the passive B.C.
              RidtC = Rprime*idtCTM[Kdex];
              iplusTM[Kdex] = 1/(ATM[Kdex]+RidtC);
              minusTM[Kdex] = RidtC-ATM[Kdex];
            }
          else
            idtCTM[Kdex] = iplusTM[Kdex] = minusTM[Kdex] =  FALSE;
					
          // TE mode
          if (grid->query_geometry())
            dl3local = 1;
          else
            dl3local = 2*M_PI*(grid->getMKS(Vector2(j1, k + .5))).e2();
          ATE[Kdex] = (grid->dl2(j1,k))/dl3local;
          if(fields->getiC()[j2+normal][k].e3())
            {
              idtCTE[Kdex] = 1/(dt*fields->getiC()[j2+normal][k].e3());//iC on the edge is set to zero
              RidtC = Rprime*idtCTE[Kdex];
              iplusTE[Kdex] = 1/(ATE[Kdex]+RidtC);
              minusTE[Kdex] = RidtC-ATE[Kdex];
            }
          else
            idtCTE[Kdex] = iplusTE[Kdex] = minusTE[Kdex] = FALSE;
        }
    }
  else						// 	horizontal
    {
      // wave going out (greater r) normal = -1
      // wave going in              normal = 1 
      for (int j=j1; j<j2; j++)
        {
          Jdex= j-j1;
          // TM mode
          if (grid->query_geometry())
            dl3local = 1;
          else
            dl3local = 2*M_PI*(grid->getMKS(Vector2(j, k2+normal/2))).e2();
          ATM[Jdex] = dl3local/(grid->dl1(j,k2));
          if (fields->getiC()[j][k2+normal].e1())
            {
              idtCTM[Jdex] = 1/(dt*(fields->getiC()[j][k2+normal].e1())); // iC k2 is equal to zero because of the passive B.C.
              RidtC = Rprime*idtCTM[Jdex];
              iplusTM[Jdex] = 1/(RidtC+ATM[Jdex]);
              minusTM[Jdex] = RidtC-ATM[Jdex]; 
            }
          else
            idtCTM[Jdex] = iplusTM[Jdex] = minusTM[Jdex] = FALSE;
					
          // TE mode
          if (grid->query_geometry())
            dl3local = 1;
          else
            dl3local = 2*M_PI*(grid->getMKS(Vector2(j, k2 + .5))).e2();
          ATE[Jdex] = (grid->dl1(j,k2))/dl3local;
          if (fields->getiC()[j][k2+normal].e3())
            {
              idtCTE[Jdex] = 1/(dt*(fields->getiC()[j][k2+normal].e3()));
              RidtC = Rprime*idtCTE[Jdex];
              iplusTE[Jdex] = 1/(RidtC+ATE[Jdex]);
              minusTE[Jdex] = RidtC-ATE[Jdex];
            }
          else
            idtCTE[Jdex] = iplusTE[Jdex] = minusTE[Jdex] = FALSE;
        }
    }
  init = FALSE;
}

//--------------------------------------------------------------------
//	Set the passive bc for fields at the port.  Currently just a
//	copy of those for conductor.

void ExitPort::setPassives()
{
  int k,j;
  if (alongx2())			//	vertical
    {
      for (k=MIN(k1,k2); k<MAX(k1,k2); k++)
        {
          fields->set_iC2(j1, k, 0);
          if (get_normal()!=1)
            {
              if (j1<(fields->getJ())) fields->set_iC1(j1+1, k, 0);
              if (j1<(fields->getJ())) fields->set_iC2(j1+1, k, 0);
              if (j1<(fields->getJ())) fields->set_iC3(j1+1, k, 0);
            }
          else
            {
              if (j1>0) fields->set_iC1(j1-1, k, 0);
              if (j1>0) fields->set_iC2(j1-1, k, 0);
              if (j1>0) fields->set_iC3(j1-1, k, 0);
            }
          fields->set_iC3(j1, k, 0);
        }
      fields->set_iC3(j1, k, 0);
    }
  else						  //	horizontal
    {
      for (j=MIN(j1,j2); j<MAX(j1,j2); j++)
        {
          fields->set_iC1(j, k1, 0);
          if (get_normal()!=1)
            {
              if (k1<(fields->getK())) fields->set_iC1(j, k1+1, 0);
              if (k1<(fields->getK())) fields->set_iC2(j, k1+1, 0);
              if (k1<(fields->getK())) fields->set_iC3(j, k1+1, 0);
            }
          else
            {
              if (k1>0) fields->set_iC1(j, k1-1, 0);
              if (k1>0) fields->set_iC3(j, k1-1, 0);
              if (k1>0) fields->set_iC2(j, k1-1, 0);
            }
          fields->set_iC3(j, k1, 0);
        }
      fields->set_iC3(j, k1, 0);
    }
}

//--------------------------------------------------------------------
//	ExitPort::emit() simply deletes Particles in its stack.  May add some
//	diagnostics for particles collected in the future.
#if !defined __linux__ && !defined _WIN32
#pragma argsused
#endif
/*
ParticleList& ExitPort::emit(Scalar t,Scalar dt)
{
	while(!particleList.isEmpty())
		{
			Particle* p = particleList.pop();
			delete p;
		}
	return particleList;
}
*/
#ifdef HAVE_HDF5
int ExitPort::dumpH5(dumpHDF5 &dumpObj,int number)
{

  //cerr << "entering ExitPort::dumpH5() . \n";
//  Scalar temp;
//  Scalar x,y;
  int length;
  
  hid_t fileId, groupId, datasetId,dataspaceId,HspaceId,HsetId  ;
  herr_t status;
  hsize_t rank; 
//  hsize_t     dimsf[1];   
  hsize_t dims;
  hid_t scalarType = dumpObj.getHDF5ScalarType();
//  int         attrData;
  hid_t  attrdataspaceId,attributeId;
#ifdef NEW_H5S_SELECT_HYPERSLAB_IFC
  hsize_t start[2]; /* Start of hyperslab */
#else
  hssize_t start[2]; /* Start of hyperslab */
#endif
  hsize_t count[2];  /* Block count */

  //  strstream s;

  stringstream s;

  s << "boundary" << number <<ends; 
  string portname = s.str();
   portname = "/Boundaries/" + portname;
// Open the hdf5 file with write access
  fileId = H5Fopen(dumpObj.getDumpFileName().c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
  groupId = H5Gcreate(fileId,portname.c_str(),0);
// get the data   
 

  Grid *grid = fields->get_grid();
// Write the physical dimensions of the boundary
  
  Scalar xw[4];  // in this format:  xs, ys, xf, yf
  Vector2 **X=grid->getX();
  xw[0] = X[j1][k1].e1();
  xw[1] = X[j1][k1].e2();
  xw[2] = X[j2][k2].e1();
  xw[3] = X[j2][k2].e2();
  
/* Create the data space for the attribute. */
  dims = 4;
  attrdataspaceId = H5Screate_simple(1, &dims, NULL);
/* Create a dataset attribute. */
  attributeId = H5Acreate(groupId, "dims",scalarType, attrdataspaceId, H5P_DEFAULT);
/* Write the attribute data. */
  status = H5Awrite(attributeId, scalarType, xw);
/* Close the attribute. */
  status = H5Aclose(attributeId);
/* Close the dataspace. */
  status = H5Sclose(attrdataspaceId);

// write Boundary Type as attribute
  dims = 1;
  attrdataspaceId = H5Screate_simple(1, &dims, NULL);
  attributeId = H5Acreate(groupId, "boundaryType", H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
  status = H5Awrite(attributeId, H5T_NATIVE_INT, &BoundaryType);
  status = H5Aclose(attributeId);
  status = H5Sclose(attrdataspaceId);

// write length as attribute --------
  length = MAX( (abs(k2-k1)),(abs(j2-j1))) +1;
  dims = 1;
  attrdataspaceId = H5Screate_simple(1, &dims, NULL);
  attributeId = H5Acreate(groupId, "nQuads", H5T_NATIVE_INT , attrdataspaceId, H5P_DEFAULT);
  status = H5Awrite(attributeId, H5T_NATIVE_INT, &length);
  status = H5Aclose(attributeId);
  status = H5Sclose(attrdataspaceId);


  length = MAX( (abs(k2-k1)),(abs(j2-j1))) +1;
  rank = 2;
  hsize_t     dimsf2[2];
  dimsf2[0] = length;
  dimsf2[1] = 2;

  dataspaceId = H5Screate_simple(rank, dimsf2, NULL); 
  string datasetName = "Points";  
  datasetId = H5Dcreate(groupId, datasetName.c_str(), scalarType, dataspaceId,
			H5P_DEFAULT);
  
  HspaceId = H5Screate_simple(rank, dimsf2, NULL); 
  HsetId = H5Dcreate(groupId, "oldH", scalarType, HspaceId,
			H5P_DEFAULT);
  hsize_t dim2 = 2;
  hid_t slabId = H5Screate_simple(1, &dim2, NULL);

  if (alongx2())	// vertical
    {
      int kmax = abs(k2-k1) + 1;
      for (int k = 0; k<kmax; k++)
	{
	  Scalar data[2] = {X[j1][k].e1(),X[j1][k].e2()};  
	  start[0]  = 0;  count[0]  = 2;
	  status = H5Sselect_hyperslab(slabId, H5S_SELECT_SET, start, NULL, count, NULL);
	  start[0]  = k; start[1]  = 0;
	  count[0]  = 1; count[1]  = 2;     
	  status = H5Sselect_hyperslab(dataspaceId, H5S_SELECT_SET, start, NULL, count, NULL);
	  status = H5Dwrite(datasetId, scalarType, slabId, dataspaceId, H5P_DEFAULT, data);
     
	  data[0] = oldHonBoundary[k].e1();  
	  data[1] = oldHonBoundary[k].e2();   
	  start[0]  = 0;  count[0]  = 2;
	  status = H5Sselect_hyperslab(slabId, H5S_SELECT_SET, start, NULL, count, NULL);
	  start[0]  = k; start[1]  = 0;
	  count[0]  = 1; count[1]  = 2;     
	  status = H5Sselect_hyperslab(HspaceId, H5S_SELECT_SET, start, NULL, count, NULL);
	  status = H5Dwrite(HsetId, scalarType, slabId, HspaceId, H5P_DEFAULT, data);
	}
    }
  else   // horizontal
    {
      int jmax = abs(j2-j1) + 1;
      for (int j = 0; j<jmax; j++)
	{
	  Scalar data[2] = {X[j][k1].e1(),X[j][k1].e2()};  
	  start[0]  = 0;  count[0]  = 2;
	  status = H5Sselect_hyperslab(slabId, H5S_SELECT_SET, start, NULL, count, NULL);
	  start[0]  = j; start[1]  = 0;
	  count[0]  = 1; count[1]  = 2;     
	  status = H5Sselect_hyperslab(dataspaceId, H5S_SELECT_SET, start, NULL, count, NULL);
	  status = H5Dwrite(datasetId, scalarType, slabId, dataspaceId, H5P_DEFAULT, data);

	  data[0] = oldHonBoundary[j].e1();  
	  data[1] = oldHonBoundary[j].e2();   
	  start[0]  = 0;  count[0]  = 2;
	  status = H5Sselect_hyperslab(slabId, H5S_SELECT_SET, start, NULL, count, NULL);
	  start[0]  = j; start[1]  = 0;
	  count[0]  = 1; count[1]  = 2;     
	  status = H5Sselect_hyperslab(HspaceId, H5S_SELECT_SET, start, NULL, count, NULL);
	  status = H5Dwrite(HsetId, scalarType, slabId, HspaceId, H5P_DEFAULT, data);
	}
    }	
   
  H5Sclose(dataspaceId);
  H5Dclose(datasetId);   
  H5Sclose(HspaceId);
  H5Dclose(HsetId);   

  // write fields
   
  H5Gclose(groupId);
  H5Fclose(fileId);
  return(1);
}
#endif

int ExitPort::Dump(FILE * DMPFile)
{
  Scalar temp;
  Scalar x,y;
  int length;

	
  //#ifdef UNIX
  {
    Scalar xw[4];  // in this format:  xs, ys, xf, yf
    Vector2 **X=grid->getX();
    xw[0] = X[j1][k1].e1();
    xw[1] = X[j1][k1].e2();
    xw[2] = X[j2][k2].e1();
    xw[3] = X[j2][k2].e2();
    XGWrite(xw,ScalarInt,4,DMPFile,ScalarChar);
  }
  // write the BoundaryType
  XGWrite(&BoundaryType, 4, 1, DMPFile, "int");


  Vector2** X=fields->get_grid()->getX();
	
  length = MAX( (abs(k2-k1)),(abs(j2-j1))) +1;
  XGWrite(&length,4,1,DMPFile,"int");
  XGWrite(&length,4,1,DMPFile,"int");  // redundant, but necessary
  XGWrite(&length,4,1,DMPFile,"int");  // redundant, but necessary
  if (alongx2())	// vertical
    {
      int kmax = abs(k2-k1) + 1;
      for (int k = 0; k<kmax; k++)
        {
          x = X[j1][k].e1();
          y = X[j1][k].e2();
          XGWrite(&x,ScalarInt,1,DMPFile,ScalarChar);
          XGWrite(&y,ScalarInt,1,DMPFile,ScalarChar);
          temp = oldHonBoundary[k].e1();
          XGWrite(&temp, ScalarInt, 1, DMPFile, ScalarChar);
          temp = oldHonBoundary[k].e2();
          XGWrite(&temp, ScalarInt, 1, DMPFile, ScalarChar);
        }
    }
  else   // horizontal
    {
      int jmax = abs(j2-j1) + 1;
      for (int j = 0; j<jmax; j++)
        {
          x = X[j][k1].e1();
          y = X[j][k1].e2();
          XGWrite(&x,ScalarInt,1,DMPFile,ScalarChar);
          XGWrite(&y,ScalarInt,1,DMPFile,ScalarChar);
          temp = oldHonBoundary[j].e1();
          XGWrite(&temp, ScalarInt, 1, DMPFile, ScalarChar);
          temp = oldHonBoundary[j].e2();
          XGWrite(&temp, ScalarInt, 1, DMPFile, ScalarChar);
        }
    }	
  //#endif
  return(TRUE);
}



#ifdef HAVE_HDF5
int ExitPort::restoreH5(dumpHDF5 &restoreObj,int bType,string whichBoundary,int nQuads)
{
  //cerr << "entering ExitPort::restoreH5().\n";
//  int dummy;
  int i;
  int jl,kl;
  Scalar bDims[4];
  hid_t fileId, groupId, datasetId,dataspaceId;
  herr_t status;
  string outFile;
//  hsize_t rank; 
//  hsize_t     dimsf[1];   
//  hsize_t dims;
  hid_t scalarType = restoreObj.getHDF5ScalarType();
//  hid_t  attrspaceId,
  hid_t attrId;
#ifdef NEW_H5S_SELECT_HYPERSLAB_IFC
  hsize_t offset[2]; /* Offset of hyperslab */
#else
  hssize_t offset[2]; /* Offset of hyperslab */
#endif
  hsize_t count[2];  /* Block count */


  fileId = H5Fopen(restoreObj.getDumpFileName().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  groupId = H5Gopen(fileId, whichBoundary.c_str());

  //read dimensions of exitport
  attrId = H5Aopen_name(groupId, "dims" );
  status = H5Aread(attrId, scalarType, bDims);
  H5Aclose(attrId);

 
  

  if(BoundaryType != bType) return FALSE;
  if(!onMe(bDims[0],bDims[1],bDims[2],bDims[3])) return FALSE;
    
  // OK, this dielectric must be us.

  // Open the dataset for reading.  Determine shape and set field size.
  datasetId = H5Dopen(groupId,"Points" );
  dataspaceId = H5Dget_space(datasetId );
  int dataRank = H5Sget_simple_extent_ndims(dataspaceId);
  hsize_t *sizes = new hsize_t(dataRank);
  int res = H5Sget_simple_extent_dims(dataspaceId, sizes, NULL);
  hsize_t dim2 = sizes[1]; //should be 2
  hid_t posSlabId =  H5Screate_simple(1, &dim2, NULL);

  hid_t HsetId = H5Dopen(groupId,"oldH" );
  hid_t  HspaceId = H5Dget_space(HsetId );

  for(i=0;i<nQuads;i++) {
    Scalar x,y,H1,H2;
    Vector2 GridLoc;
 
    Scalar pos[2];   
    offset[0]  = 0;  count[0]  = 2;
    status = H5Sselect_hyperslab(posSlabId, H5S_SELECT_SET, offset, NULL, count, NULL);
    offset[0]  = i; offset[1]  = 0;;
    count[0]  = 1; count[1]  = 2;    
    status = H5Sselect_hyperslab(dataspaceId, H5S_SELECT_SET, offset, NULL, count, NULL);

    status = H5Dread(datasetId, scalarType, posSlabId, dataspaceId, H5P_DEFAULT, pos);
	      
    x = pos[0];
    y = pos[1];

    Scalar H[2];   
    //    status = H5Sselect_hyperslab(posSlabId, H5S_SELECT_SET, offset, NULL, count, NULL);
    status = H5Sselect_hyperslab(HspaceId, H5S_SELECT_SET, offset, NULL, count, NULL);
    status = H5Dread(HsetId, scalarType, posSlabId, HspaceId, H5P_DEFAULT, H);

    H1 = H[0];
    H2 = H[1];
    // Find the j/k value of the nearest
    // point on this boundary
    GridLoc = fields->get_grid()->getNearestGridCoords(Vector2(x,y));
    jl = (int) (GridLoc.e1() + 0.5);
    kl = (int) (GridLoc.e2() + 0.5);

    // find the point on this boundary this GridLoc corresponds to
    if (alongx2())	// vertical
      {
        int kmax = abs(k2-k1) +1;
        int ki = kl - MIN(k1,k2);
        if(ki <= kmax && kl >= 0)
          oldHonBoundary[ki]= Vector2(H1,H2);

      }
    else   // horizontal
      {
        int jmax = abs(j2-j1) + 1;
        int ji = jl = MIN(j1,j2);
        if(ji <= jmax && ji >=0)
          oldHonBoundary[ji]= Vector2(H1,H2);
      }	
    // #endif
  }


  H5Sclose(dataspaceId);
  H5Dclose(datasetId);
  H5Sclose(HspaceId);
  H5Dclose(HsetId);
  H5Sclose(posSlabId);
  H5Gclose(groupId);
  H5Fclose(fileId);

  delete sizes; 
  return(1);
}
#endif



int ExitPort::Restore(FILE *DMPFile,int _BoundaryType, Scalar A1, 
                      Scalar A2, Scalar B1, Scalar B2, int nQuads)
{
	
  int dummy;
  int i;
  int jl,kl;
  // #ifdef UNIX
  // Two checks to see if this data is loadable by this boundary.
  if(BoundaryType != _BoundaryType) return FALSE;
  if(!onMe(A1,A2,B1,B2)) return FALSE;
	

  // next two don't matter
  XGRead(&dummy,4,1,DMPFile,"int");
  XGRead(&dummy,4,1,DMPFile,"int");
	
//  Scalar temp1, temp2;

  for(i=0;i<nQuads;i++) {
    Scalar x,y,H1,H2;
    Vector2 GridLoc;
    XGRead(&x,ScalarInt,1,DMPFile,ScalarChar);
    XGRead(&y,ScalarInt,1,DMPFile,ScalarChar);
    XGRead(&H1,ScalarInt,1,DMPFile,ScalarChar);
    XGRead(&H2,ScalarInt,1,DMPFile,ScalarChar);
    // Find the j/k value of the nearest
    // point on this boundary
    GridLoc = fields->get_grid()->getNearestGridCoords(Vector2(x,y));
    jl = (int) (GridLoc.e1() + 0.5);
    kl = (int) (GridLoc.e2() + 0.5);

    // find the point on this boundary this GridLoc corresponds to
    if (alongx2())	// vertical
      {
        int kmax = abs(k2-k1) +1;
        int ki = kl - MIN(k1,k2);
        if(ki <= kmax && kl >= 0)
          oldHonBoundary[ki]= Vector2(H1,H2);

      }
    else   // horizontal
      {
        int jmax = abs(j2-j1) + 1;
        int ji = jl = MIN(j1,j2);
        if(ji <= jmax && ji >=0)
          oldHonBoundary[ji]= Vector2(H1,H2);
      }	
    // #endif
  }
  return(TRUE);

}

int ExitPort::Restore_2_00(FILE * DMPFile, int j1test, 
                           int k1test, int j2test, int k2test)
{
  if ((j1 == j1test)&&(k1 == k1test)&&(j2 == j2test)&&(k2 == k2test))
    {
      //#ifdef UNIX
      Scalar temp1, temp2;

      if (alongx2())	// vertical
        {
          int kmax = abs(k2-k1) + 1;
          for (int k = 0; k<kmax; k++)
            {
              XGRead(&temp1, ScalarInt, 1, DMPFile, ScalarChar);
              XGRead(&temp2, ScalarInt, 1, DMPFile, ScalarChar);
              oldHonBoundary[k] = Vector2(temp1,temp2);
            }
        }
      else   // horizontal
        {
          int jmax = abs(j2-j1) + 1;
          for (int j = 0; j<jmax; j++)
            {
              XGRead(&temp1, ScalarInt, 1, DMPFile, ScalarChar);
              XGRead(&temp2, ScalarInt, 1, DMPFile, ScalarChar);
              oldHonBoundary[j] = Vector2(temp1,temp2);
            }
        }	
      //#endif
      return(TRUE);
    }
  else
    return(FALSE);
}
