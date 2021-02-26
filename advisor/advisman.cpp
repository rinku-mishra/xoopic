//advisman.cpp

#include "speciesg.h"
#include "spatialg.h"
#include "species.h"
#include "advisman.h"
#include "varg.h"
#include "part1g.h"
#include "spbound.h"
#ifdef UNIX
#include "ptclgrp.h"
#include "diags.h"
#endif

extern "C++" void printf(char *);

#ifdef MPI_VERSION
#include "mpi.h"
extern MPI_Comm XOOPIC_COMM;
extern int MPI_RANK;
#define SRB_ANNOUNCE_TAG 100
#define SRB_LINK_TAG 200
typedef struct {
	char *name;
	int index;
	int linkedP;
} SRBdat;
#endif /*MPI_VERSION*/

Evaluator *adv_eval;
using namespace std;

// (10-29-94 Hadon) added AdvisorManager::getSpatialRegionGroupList()
// (JohnV 01-29-95) Support SpeciesParamsList and SpeciesList
//	(JohnV 08-21-95) Support for DielectricRegion


// these could be placed in a service class rather than C-style funcs
ostring getname(ostring line, int pos);
ostring getvalue(ostring line, int pos);
ostring Standardize(ostring in);
ostring parseFirst(ostring str);
ostring parseSecond(ostring str);
int member(ostring s, oopicList<ostring>* l);

AdvisorManager::AdvisorManager(const char* fileName)
{
	adv_eval = new Evaluator();
	// list for GUI
	ListOfAllBoundaryTypes.add(new ostring("Conductor"));
	ListOfAllBoundaryTypes.add(new ostring("Polarizer"));
	ListOfAllBoundaryTypes.add(new ostring("SpatialRegionBoundary"));
	ListOfAllBoundaryTypes.add(new ostring("CylindricalAxis"));
	ListOfAllBoundaryTypes.add(new ostring("FieldEmitter"));
	ListOfAllBoundaryTypes.add(new ostring("FieldEmitter2"));
	ListOfAllBoundaryTypes.add(new ostring("BeamEmitter"));
	ListOfAllBoundaryTypes.add(new ostring("OpenBoundaryBeamEmitter"));
	ListOfAllBoundaryTypes.add(new ostring("FowlerNordheimEmitter"));
	ListOfAllBoundaryTypes.add(new ostring("PlasmaSource"));
	ListOfAllBoundaryTypes.add(new ostring("LaserPlasmaSource"));
	ListOfAllBoundaryTypes.add(new ostring("EmitPort"));
	ListOfAllBoundaryTypes.add(new ostring("VarWeightBeamEmitter"));
	ListOfAllBoundaryTypes.add(new ostring("Iloop"));
	ListOfAllBoundaryTypes.add(new ostring("CurrentRegion"));
	ListOfAllBoundaryTypes.add(new ostring("PortTM"));
	ListOfAllBoundaryTypes.add(new ostring("PortTE"));
	ListOfAllBoundaryTypes.add(new ostring("PortGauss"));
	ListOfAllBoundaryTypes.add(new ostring("PMLPort"));
	ListOfAllBoundaryTypes.add(new ostring("ExitPort"));
	ListOfAllBoundaryTypes.add(new ostring("Gap"));
	ListOfAllBoundaryTypes.add(new ostring("AbsorbWave"));
	ListOfAllBoundaryTypes.add(new ostring("LaunchWave"));
	ListOfAllBoundaryTypes.add(new ostring("Equipotential"));
	ListOfAllBoundaryTypes.add(new ostring("Dielectric"));
	ListOfAllBoundaryTypes.add(new ostring("DielectricRegion"));
	ListOfAllBoundaryTypes.add(new ostring("DielectricTriangle"));
	ListOfAllBoundaryTypes.add(new ostring("Foil"));
	ListOfAllBoundaryTypes.add(new ostring("SingleParticleEmitter"));
	ListOfAllBoundaryTypes.add(new ostring("PortTEM_MW"));
	ListOfAllBoundaryTypes.add(new ostring("CurrentSource"));

	ListOfAllParameterGroups.add(new ostring("Control"));
	ListOfAllParameterGroups.add(new ostring("Variables"));
	ListOfAllParameterGroups.add(new ostring("Grid"));
	ListOfAllParameterGroups.add(new ostring("Species"));
	ListOfAllParameterGroups.add(new ostring("MCC"));
	ListOfAllParameterGroups.add(new ostring("Load"));
	ListOfAllParameterGroups.add(new ostring("VarWeightLoad"));
	ListOfAllParameterGroups.add(new ostring("Load_PW"));

	ListOfAllParameterGroups.add(new ostring("Diagnostic"));
	ListOfAllParameterGroups.add(new ostring("H5Diagnostic"));
	oopicListIter<ostring>	sI(ListOfAllBoundaryTypes);
	for (sI.restart(); !sI.Done(); sI++)
	{
		ListOfAllParameterGroups.add(sI.current());
	}

	ListOfAllProbeTypes.add(new ostring("E Field"));
	ListOfAllProbeTypes.add(new ostring("B Field"));
	ListOfAllProbeTypes.add(new ostring("Current"));
	ListOfAllProbeTypes.add(new ostring("DivDerror"));

	speciesParamsList = new SpeciesParamsList;
	speciesList = NULL;						// JohnV 03-15-95
	//	speciesList = new SpeciesList;

	setFileName(fileName);
}

AdvisorManager::~AdvisorManager()
{
	//	speciesParamsList->deleteAll();
	oopicListIter<SpeciesParams> spIter(*speciesParamsList);
	for (spIter.restart(); !spIter.Done(); spIter++)
		spIter.deleteCurrent();
	speciesParamsList->removeAll();
	srgList.deleteAll();
	delete speciesParamsList;
	// do NOT delete speciesList here; it gets passed to PlasmaDevice!
}


void AdvisorManager::getLine(ifstream& input, char* buffer, int buf_size){
	char c;
	int i = 0;
	while (1){
		if (i >= (buf_size - 1) ){
			buffer[i] = '\0';
			break;
		}
		if (!input.eof()){
			input.get(c);
		}
		else {
			buffer[i-1] = '\0';
			return;
		}

		if (c != '\r' && c!='\n'){
			buffer[i] = c;
		}
		else if (c == '\r'){
			buffer[i] = '\0'; // terminate this.
			if (input.peek() == '\n')
				input.ignore();
			break;
		}
		else if (c== '\n'){  // \n
			buffer[i] = '\0'; // terminate this
			break;
		}

		i++;
	}
}



// return message ...
void AdvisorManager::setFileName(const char* fileName)
{
	InputErrorMessage = ostring("");
	inputFile.open(fileName);

	char buffer[512];
	VarGroup::getLine(inputFile, buffer, 511); // first line should be caseTitle
	caseTitle = Standardize(ostring(buffer));

	// this reads the case description
	while(!inputFile.eof())
	{
		VarGroup::getLine(inputFile,buffer, 511); // get a line
		ostring line = stripWS(ostring(buffer));
		printf(line);
		if (line == (ostring)"{") { // skip this line and read another
			VarGroup::getLine(inputFile,buffer, 511);
			line = stripWS(ostring(buffer));
		}
		if (line == (ostring)"}")  // terminates the input for this group
			break;
		else {
			// push this line of text into caseDescription
			ostring* oline = new ostring(line);
			caseDescription.add(oline);
		}
	}

	// Read the regions and the variables group, if any

	while(!inputFile.eof()) {
		ostring line;
		buffer[0]=0;  // fixes a "getline" bug on cray T3E
		VarGroup::getLine(inputFile,buffer,511);
		line=stripWS(Standardize(buffer));
		if(line == (ostring)"Region") {
			SpatialRegionGroup*	srg = new SpatialRegionGroup(this);
			ostring message = readSpatialRegionGroup(*srg);
			if (message.length() > 1) {
				/*
			cout << "ERROR READING INPUT FILE!!!" << std::endl
			  << "FILE NAME = " << fileName << std::endl
				 << "BAD LINE = " << message << std::endl;
				 */
				printf("ERROR READING INPUT FILE!!!");
				char buffer[512];
				sprintf(buffer,"FILE NAME = %s" ,fileName);
				printf(buffer);
				printf(strdup("BAD LINE = " + message));
				sprintf(buffer, "ERROR READING INPUT FILE!!!\nFILE NAME = %s\nBAD LINE = %s\n",
						fileName, message.c_str());
				InputErrorMessage = ostring(buffer);
				inputFile.close();
				return;
			}
			srgList.add(srg);
		}
		else if(line == (ostring)"Variables") {
			VG.InitializeFromStream(inputFile);
		}
		else {
			if(line != (ostring)"") {
				// cout << "Warning: Unrecognized line: check your input file for:\n" << line << "\n";
				// cout << "Expected 'Region' or 'Variables' or description.\n";
				printf("Warning: Unrecognized line: check your input file for:");
				printf(line);
				printf("Expected 'Region' or 'Variables' or description.");
			}
		}
	}
	inputFile.close();
}

void AdvisorManager::writeOutputFile(const char* fileName)
{
	outputFile.open(fileName);
	// writes the caseName and caseDescription
	outputFile << caseTitle << std::endl;
	outputFile << "{" << std::endl;
	oopicListIter<ostring>	sI(caseDescription);
	for (sI.restart(); !sI.Done(); sI++)
		outputFile << *sI.current() << std::endl;
	outputFile << "}" << std::endl;

	oopicListIter<SpeciesParams> sIter(*speciesParamsList); // write out all species
	for (sIter.restart(); !sIter.Done(); sIter++)	//	JohnV 06-26-95
		sIter.current()->writeOutputFile(outputFile);
	oopicListIter<SpatialRegionGroup>	srgIter(srgList);
	for (srgIter.restart(); !srgIter.Done(); srgIter++)
		srgIter.current()->writeOutputFile(outputFile);
	outputFile.close();
} 

ostring AdvisorManager::readSpatialRegionGroup(SpatialRegionGroup& srg) {
	char buffer[512];
	oopicList<ostring> alist;
	ostring name,value,message;
	while (inputFile) {
		VarGroup::getLine(inputFile,buffer, 511); // first line should be groupname
		ostring line = stripWS(Standardize(ostring(buffer)));
		int pos = line.find("=", 0); // is this a parameter = value line?

		if (pos > 0) {
			name = getname(line, pos);
			if(srg.LegalParamName(name)) {
				value = getvalue(line, pos);
				alist.add(new ostring(name + ostring(" ") + value));
			}
			else {
				return(line);
			}
		}
		else if(line == (ostring)"{") { //skip this line and get another
			continue;
		}
		else if(line== ostring("")) { continue; }
		else if (member(line, &ListOfAllParameterGroups) == 1) {
			// cout << "creating " << line << std::endl;
			// JRC: memory leak.  printf is not a handoff.
			// printf(strdup("creating " +line));
			printf(strdup("creating " +line));
			message = srg.CreateParamGroup(inputFile, line);
			if (message.length() > 1) {return(message);}
		}
		else if (line == (ostring)"}") {
			srg.setValues(alist);
			break;
		}
		else {

			return(line);
		}
	}
	return (ostring(""));
}

//-------------------------------------------------------------------
//	describe each SpatialRegionGroup

void AdvisorManager::describe()
{
	oopicListIter<SpatialRegionGroup>	srgIter(srgList);
	for (srgIter.restart(); !srgIter.Done(); srgIter++)
		srgIter.current()->describe();
}

//-------------------------------------------------------------------
//	check rules for each SpatialRegionGroup

void AdvisorManager::checkRules()
{
	oopicListIter<SpatialRegionGroup>	srgIter(srgList);
	for (srgIter.restart(); !srgIter.Done(); srgIter++)
	{
		SpatialRegionGroup* srg = srgIter.current();
		srg->checkRules();
		srg->showRuleMessages();
	}
	oopicListIter<SpeciesParams> sIter(*speciesParamsList);
	for (sIter.restart(); !sIter.Done(); sIter++)
	{
		SpeciesParams *s = sIter.current();
		s->checkRules();
		s->showRuleMessages();
	}
}

//-------------------------------------------------------------------
//	create the SpatialRegionList for PlasmaDevice

SpatialRegionList* AdvisorManager::createDevice()
/* throw(Oops) */{
	// Build a SpeciesList:
	speciesList = new SpeciesList;
	Species::reset_idCount();				//	start speciesID from 0
	oopicListIter<SpeciesParams> spIter(*speciesParamsList);

#ifdef TXQ_GUI
	setProgress(1, "Creating AdvisorManger::SpeciesParam");
#endif
	for (spIter.restart(); !spIter.Done(); spIter++)
		speciesList->add(spIter.current()->createCounterPart());

	SpatialRegionList*	srList = new SpatialRegionList;
	oopicListIter<SpatialRegionGroup>	srgIter(srgList);

#ifdef TXQ_GUI
	setProgress(1, "Creating AdvisorManger::SpatialRegionGroup");
#endif

	// the MPI version only has 1 region per process
#ifndef MPI_VERSION
	for (srgIter.restart(); !srgIter.Done(); srgIter++) {
		try{
			srList->add(srgIter.current()->CreateCounterPart());
		}
		catch(Oops& oops2){
			oops2.prepend("AdvisorManager::createDevice: Error: \n");//done
			throw oops2;
		}
	}

#else  /* MPI_VERSION */
	int i;
	for(i=0,srgIter.restart();!srgIter.Done();srgIter++,i++){
		/*  if(i==MPI_RANK || srList->nItems()==1)  // MPI_RANK is this processes ID
		 // or if we're doing autopartitioning.*/
		try{
			srList->add(srgIter.current()->CreateCounterPart());
		}
		catch(Oops& oops2){
			oops2.prepend("AdvisorManager::createDevice: Error: \n");//OK
			throw oops2;
		}
	}

#ifdef TXQ_GUI
	setProgress(75, "Finished AdvisorManager::SpatialRegionGroup");
#endif

#endif /* MPI_VERSION */

	// perform the tasks involved in linking boundaries
	//linkSRB();
	return srList;
}

// could add writeOutPutFile() like above


oopicList<ostring>* AdvisorManager::getListOfAllBoundaryTypes()
{
	return &ListOfAllBoundaryTypes;
}

oopicList<ostring>* AdvisorManager::getListOfAllProbeTypes()
{
	return &ListOfAllProbeTypes;
}

SpatialRegionGroupList* AdvisorManager::getSpatialRegionGroupList()
{
	return &srgList;
}

ostring AdvisorManager::GetErrorMessage()
{return InputErrorMessage;}

BOOL AdvisorManager::KnownBoundary(ostring &s)
{
	oopicListIter<ostring> nS(ListOfAllBoundaryTypes);
	for (nS.restart(); !nS.Done(); nS++)
		if (*nS.current() == s) return TRUE;
	return FALSE;
}

BOOL AdvisorManager::KnownSpecies(ostring &speciesName)
{
	oopicListIter<SpeciesParams> spIter(*speciesParamsList);
	for (spIter.restart(); !spIter.Done(); spIter++)
		if (spIter.current()->get_speciesName() == speciesName) return TRUE;
	return FALSE;
};

Species* AdvisorManager::get_speciesPtr(const ostring &speciesName)
{
	oopicListIter<Species> speciesIter(*speciesList);
	for (speciesIter.restart(); !speciesIter.Done(); speciesIter++)
		if (speciesIter.current()->get_name() == speciesName)
			return speciesIter.current();
	return NULL;	//	could not find a match
}

// Read a Species from input file, add to speciesList.
ostring AdvisorManager::readSpecies(ifstream& fin)
{
	char buffer[512];
	SpeciesParams* speciesParams = new SpeciesParams();
	speciesParams->InitializeFromStream(fin);
	ostring speciesName = speciesParams->get_speciesName();

	if (speciesName == (ostring)"NULL")
	{
		sprintf(buffer, "Species: Name is a required parameter!");
		delete speciesParams;
	}
	else if (KnownSpecies(speciesName))
	{
		sprintf(buffer, "Species Name %s is not unique", speciesName.c_str());
		delete speciesParams;
	}
	else
	{
		speciesParamsList->add(speciesParams);
		sprintf(buffer, "%s","");
	}
	return ostring(buffer);
}

void AdvisorManager::linkSRB() /* throw(Oops) */{
	stringstream ss (stringstream::in | stringstream::out);
	ss << "AdvisorManager::linkSRB: Error: \n" <<
			"This function is not currently compatible with\n"<<
			"SPBoundLink class and needs to be modified..." << std::endl;

	std::string msg;
	ss >> msg;
	Oops oops(msg);
	throw oops;    // exit() MPI_EXIT  only called by MPI
	/**
	 * comment this function's body completely since it is not used.
	 * Note that if it were to be used again the number of arguments
	 * of the SPBoundLink constructor has changed and this should be
	 * taken care of before instantiating SPBoundLink objects in
	 * the body of this function. One of the SPBoundLink::makeLink(...)
	 * functions, the one that is used below,
	 * will also need modifications. dad 05/23/01
	 *
	 * Here are Peter Mardal's comments on the cases of communication
	 * designed in SPBoundLink:
	 *
	 * "The reason for the other cases of communication other
	 * than MPI_LINK were to deal with other methods of interprocess
	 * communication, such as shared memory, and even mixed methods
	 * of communication.
	 *
	 * For example, on the new IBM at Nersc, it is made up of clusters
	 * of SMP machines.  You use MPI to communicate between the machines,
	 * and shared memory to communicate between processors on the same
	 * machine.
	 *
	 * No one has implemented shared memory communication for XOOPIC
	 * yet, but that is where it would go, and is the reason
	 * for the switches and the abstraction layers used.
	 *
	 * As for AdvisorManager::linkSRB, that is code which was once
	 * used and which has been moved mostly to SpatialRegion::linkSRB now.
	 * I kept it around in case there was a need to back out some changes.
	 *
	 * At some point, we can safely get rid of it.
	 *
	 * PeterM"
	 ****************************************************************************
	oopicListIter<SpatialRegionGroup>	srgIter(srgList);

	//  It is necessary here to link pairs of SpatialRegionBoundaries

	//  First we check for regions on the same CPU
#ifndef MPI_VERSION
	for(srgIter.restart();!srgIter.Done();srgIter++) {

	  oopicListIter <SpatialRegionBoundary> SRBIter(srgIter.current()->getSpatialRegionBoundaryList());

	  for(SRBIter.restart();!SRBIter.Done();SRBIter++) {

		 if(SRBIter.current()->isLinked()) continue;  // if this boundary is linked already, skip it.
		 else {
			// we must search all the boundaries available for a counterpart.  
			oopicListIter <SpatialRegionGroup> srgIter2(srgList);
			for(srgIter2.restart();!srgIter2.Done();srgIter2++) {

			  oopicListIter <SpatialRegionBoundary> SRBIter2(srgIter2.current()->getSpatialRegionBoundaryList());
			  for(SRBIter2.restart();!SRBIter2.Done();SRBIter2++) {
				 // if the two boundaries match and they're not the same boundary, link them.
				 if((SRBIter.current()->PartnerName == SRBIter2.current()->PartnerName) 
					 &&SRBIter.current()!=SRBIter2.current() ) {
					SpatialRegionBoundary *SRB1 = SRBIter.current();
					SpatialRegionBoundary *SRB2 = SRBIter2.current();
					SPBoundLink *link1 = new SPBoundLink(SRB1->lEsend,SRB1->lBsend,
							SRB1->lJsend,SRB1->getLength(),SRB2->getLength());           //the link from 1 to 2
					SPBoundLink *link2 = new SPBoundLink(SRB2->lEsend,SRB2->lBsend,
							SRB2->lJsend,SRB2->getLength(),SRB1->getLength());           //the link from 2 to 1
					SRB1->LinkBoundary(link1);
					SRB2->LinkBoundary(link2);
					link1->makeLink(SRB2->lE,SRB2->lB,SRB2->lJ,&(SRB1->lE),&(SRB1->lB),&(SRB1->lJ));
					link2->makeLink(SRB1->lE,SRB1->lB,SRB1->lJ,&(SRB2->lE),&(SRB2->lB),&(SRB2->lJ));
					//  now that the links are established, have them send across liL.
					SRB1->send_liL();
					SRB2->send_liL();
					SRB1->get_liL();
					SRB2->get_liL();
				 }
			  }

			}
		 }
	  }
	}
#else // MPI_VERSION 
	//  Now we look for boundaries on other processes/CPU's
	for(srgIter.restart();!srgIter.Done();srgIter++) {  // there should really only be 1 region
	  oopicListIter <SpatialRegionBoundary> SRBIter(srgIter.current()->getSpatialRegionBoundaryList());
	  for(SRBIter.restart();!SRBIter.Done();SRBIter++) {
		 if(SRBIter.current()->isLinked()) continue;  // if this boundary is linked already, skip it.
		 char buf[256];  //  the send buffer
		 strncpy(buf,SRBIter.current()->PartnerName.c_str(),255);
		 // send our ID to the root process
		 MPI_Send(buf,255,MPI_CHAR,0,SRB_ANNOUNCE_TAG,XOOPIC_COMM);
	  }
	}


	//  We need to block execution until everyone has sent ID's for
	//  all their boundaries to everyone else.
	MPI_Barrier(XOOPIC_COMM);

	// Now the root process has to receive all the ID's and then distribute
	// them to everyone.
  int tag_modifier=0;
	if(MPI_RANK==0 ) {
	  oopicList <SRBdat> SRBdatList;
	  int incoming_messagesP;
	  MPI_Status message_status;
	  MPI_Iprobe(MPI_ANY_SOURCE,SRB_ANNOUNCE_TAG,XOOPIC_COMM,&incoming_messagesP,&message_status);

	  //  load all the incoming SRB information into SRBdatList
	  while(incoming_messagesP) {
		 SRBdat *SRBtemp = new SRBdat;
		 SRBtemp->name = new char[256];
		 MPI_Recv(SRBtemp->name,255,MPI_CHAR,MPI_ANY_SOURCE,SRB_ANNOUNCE_TAG,XOOPIC_COMM,&message_status);
		 SRBtemp->index = message_status.MPI_SOURCE;
		 SRBtemp->linkedP = 0;
		 SRBdatList.add(SRBtemp);
		 MPI_Iprobe(MPI_ANY_SOURCE,SRB_ANNOUNCE_TAG,XOOPIC_COMM,&incoming_messagesP,&message_status);
	  }


	  // Now go through the list we've created, linking any pairs we find.
	  oopicListIter<SRBdat> SRBdatIter(SRBdatList);
	  for(SRBdatIter.restart();!SRBdatIter.Done();SRBdatIter++)
		 {
			SRBdat *SRBtmp = SRBdatIter.current();
			// check and see if this boundary is linked already
			if(!SRBtmp->linkedP) {  // search through the rest of the list for a match
			  oopicListIter<SRBdat> SRBdatIter2(SRBdatList);
			  for(SRBdatIter2.restart();!SRBdatIter2.Done();SRBdatIter2++)
				 {
					SRBdat *SRBtmp2 = SRBdatIter2.current();
					// if it's already linked, or if it is the same, it cannot be a match
					// also check match on name
					if(! (SRBtmp2->linkedP || SRBtmp2 == SRBtmp) && !strcmp(SRBtmp->name,SRBtmp2->name) ) {

					  tag_modifier++;
					  // message both machines, telling them their counterparts.
					  // message has the format: (concatenated null-terminated strings)
					  // name\0index" , "tag_modifier\0
					  char buf[256];
					  //  tell SRBtmp2 about SRBtmp
					  sprintf(buf,"%s",SRBtmp->name);
					  sprintf(buf+strlen(SRBtmp->name) +1, "%d , %d",SRBtmp->index,tag_modifier);
					  MPI_Send(buf,255,MPI_CHAR,SRBtmp2->index,SRB_LINK_TAG,XOOPIC_COMM);

					  // tell SRBtmp about SRBtmp2
					  sprintf(buf,"%s",SRBtmp2->name);
					  sprintf(buf+strlen(SRBtmp2->name) +1, "%d , %d",SRBtmp2->index,tag_modifier);
					  MPI_Send(buf,255,MPI_CHAR,SRBtmp->index,SRB_LINK_TAG,XOOPIC_COMM);


					}

				 }
			}
		 }
	}

	if(tag_modifier > 3900) {
	  printf("XOOPIC is unable to handle so much job distriubtion.  Use fewer nodes.\n");
	  exit(0);//MPI_EXIT
	}


	// synchronize everone after all the link messages have been sent
	MPI_Barrier(XOOPIC_COMM);


	// begin processing the link messages
	MPI_Status SRBincoming_status;
	int incoming_SRB_P;
	MPI_Iprobe(0,SRB_LINK_TAG,XOOPIC_COMM,&incoming_SRB_P,&SRBincoming_status);
	while(incoming_SRB_P) {
	  char buf[256];
	  MPI_Recv(buf,255,MPI_CHAR,0,SRB_LINK_TAG,XOOPIC_COMM,&SRBincoming_status);

	  //  look for the local matching boundary
	  for(srgIter.restart();!srgIter.Done();srgIter++) {
		 oopicListIter <SpatialRegionBoundary> SRBIter(srgIter.current()->getSpatialRegionBoundaryList());
		 for(SRBIter.restart();!SRBIter.Done();SRBIter++) {
			if(SRBIter.current()->isLinked()) continue;  //  if it's linked already, this isn't it.

			// Does the linkname match the current boundary?  If so, link it.
			if(!strcmp(SRBIter.current()->PartnerName.c_str(),buf)) {
			  SpatialRegionBoundary *SRB = SRBIter.current();
			  SPBoundLink *link = new SPBoundLink(SRB->lE,SRB->lB,SRB->lJ,SRB->getLength(),SRB->getLength());
			  link->makeLink(atoi(buf + strlen(buf) +1),atoi(strstr(buf+strlen(buf)+1,",")));
			  SRB->LinkBoundary(link);
			  SRB->initialize_passives();
			}

		 }
	  }
#ifdef MPI_DEBUG
	  printf("\nAdvisman: MPI init, receiving incoming link info, #%d",MPI_RANK);
#endif
	  MPI_Iprobe(0,SRB_LINK_TAG,XOOPIC_COMM,&incoming_SRB_P,&SRBincoming_status);
	}
#endif // MPI_VERSION 
	 **********************************************************************
	 * end of commenting the body of the function...
	 **********************************************************************/
}

#ifdef TXQ_GUI
void AdvisorManager::setProgress(int p, const char* c) {
	// cout << "AdvisorManager::" << c << " " << std::endl;
}
#endif
