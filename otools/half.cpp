/*  a program which will read an input file and
then output a file with 1/2 as many particles */

#include <stdio.h>
#include "xgdefs.h"
#ifndef UNIX
extern "C" void exit(int status);
#endif

void main(int argc,char **argv) {
	FILE *In,*Out;
	Scalar ftemp;
	int number_of_species,pgnumparticlesin,pgnumparticlesout,speciesID;
	char Revision[40];
	int isp,j;

    if(argc <3) exit(1); // OK_EXIT
	if((In = fopen(argv[1],"rb"))==NULL) exit(1);// OK_EXIT
	if((Out = fopen(argv[2],"w"))==NULL) exit(1);// OK_EXIT
	//Read and write the revision
	XGRead(Revision,1,4,In,"char");
	XGWrite(Revision,1,4,Out,"char");
	//read and write the simulation time
	XGRead(&ftemp,ScalarInt,1,In,ScalarChar);
	XGWrite(&ftemp,ScalarInt,1,Out,ScalarChar);
	//read and write nspecies
	XGRead(&number_of_species,4,1,In,"int");
	XGWrite(&number_of_species,4,1,Out,"int");
	for(isp=0;isp<number_of_species;isp++)
		{
			do {
				XGRead(&speciesID,4,1,In,"int");
				XGRead(&pgnumparticlesin,4,1,In,"int");
				pgnumparticlesout=pgnumparticlesin/2 + pgnumparticlesin%2;
				XGWrite(&speciesID,4,1,Out,"int");
				XGWrite(&pgnumparticlesout,4,1,Out,"int");
				for(j=0;j<pgnumparticlesin;j++)
					{
						XGRead(&ftemp, ScalarInt, 1, In, ScalarChar);//x.e1
						if(!(j%2)) XGWrite(&ftemp, ScalarInt, 1, Out, ScalarChar);
						XGRead(&ftemp, ScalarInt, 1, In, ScalarChar); //x.e2
						if(!(j%2)) XGWrite(&ftemp, ScalarInt, 1, Out, ScalarChar);
						XGRead(&ftemp, ScalarInt, 1, In, ScalarChar); //v.e1
						if(!(j%2)) XGWrite(&ftemp, ScalarInt, 1, Out, ScalarChar);
						XGRead(&ftemp, ScalarInt, 1, In, ScalarChar); //v.e2
						if(!(j%2)) XGWrite(&ftemp, ScalarInt, 1, Out, ScalarChar);
						XGRead(&ftemp, ScalarInt, 1, In, ScalarChar); //v.e3
						if(!(j%2)) XGWrite(&ftemp, ScalarInt, 1, Out, ScalarChar);
						XGRead(&ftemp, ScalarInt, 1, In, ScalarChar); //np2c
						ftemp*=2;
						if(!(j%2)) XGWrite(&ftemp, ScalarInt, 1, Out, ScalarChar);	
					}  //for
			} while(pgnumparticlesin);
		}//for
	int zero=0;
	XGWrite(&zero, 4, 1, Out, "int");
	XGWrite(&zero, 4, 1, Out, "int");
	XGWrite(&zero, 4, 1, Out, "int");
	XGWrite(&zero, 4, 1, Out, "int");

	fclose(In);
	fclose(Out);

}
