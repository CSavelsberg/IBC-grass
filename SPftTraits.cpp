/*
 * SPftTraits.cpp
 *
 *  Created on: 21.04.2014
 *      Author: KatrinK
 */

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <cassert>

#include "SPftTraits.h"
#include "CEnvir.h"

SPftTraits::SPftTraits() :
		TypeID(999), name("default"), MaxAge(100), AllocSeed(0.05), LMR(0), m0(
				0), MaxMass(0), SeedMass(0), Dist(0), pEstab(0.5), Gmax(0), memory(
				0), SLA(0), palat(0), RAR(1), growth(0.25), mThres(0.2), Dorm(
				1), FlowerWeek(16), DispWeek(20), PropSex(0.1), meanSpacerlength(
				17.5), sdSpacerlength(12.5), Resshare(true), mSpacer(70), AllocSpacer(
				0.05), clonal(true), myTraitType(SPftTraits::species)
{
	myTraitType = SPftTraits::species;
} //end constructor

SPftTraits::SPftTraits(const SPftTraits& s) :
		TypeID(s.TypeID), name(s.name), MaxAge(s.MaxAge), AllocSeed(s.AllocSeed), LMR(s.LMR),
		m0(s.m0), MaxMass(s.MaxMass), SeedMass(s.SeedMass), Dist(s.Dist), pEstab(s.pEstab),
		Gmax(s.Gmax), memory(s.memory), SLA(s.SLA), palat(s.palat), RAR(s.RAR), growth(s.growth),
		mThres(s.mThres), Dorm(s.Dorm), FlowerWeek(s.FlowerWeek), DispWeek(s.DispWeek),
		PropSex(s.PropSex), meanSpacerlength(s.meanSpacerlength),sdSpacerlength(s.sdSpacerlength),
		Resshare(s.Resshare), mSpacer(s.mSpacer), AllocSpacer(s.AllocSpacer), clonal(s.clonal),
		myTraitType(SPftTraits::species)
{
	myTraitType = SPftTraits::species;
}

SPftTraits::~SPftTraits() {
	// TODO Auto-generated destructor stub
}

//std::vector<SPftTraits*> SPftTraits::PftList;//(CRunPara::RunPara.NPft,new SPftTraits);
//std::vector<SclonalTraits*> SclonalTraits::clonalTraits;//(8,new SclonalTraits());
map<string, SPftTraits*> SPftTraits::PftLinkList = map<string, SPftTraits*>();

// MSC : This vector is for remembering the order of the PFTs if this is an InvasionCriterion experiment.
vector<string> SPftTraits::pftInsertionOrder = vector<string>();

//------------------------------------------------------------------------------
/**
 * Get - link for specific PFT
 * @param type PFT asked for
 * @return Object pointer to PFT definition
 */
SPftTraits* SPftTraits::getPftLink(string type) {
	SPftTraits* traits = NULL;
	map<string, SPftTraits*>::iterator pos = PftLinkList.find(type);
	if (pos == (PftLinkList.end()))
		cerr << "type not found:" << type << endl;
	else
		traits = pos->second;
	if (traits == NULL)
		cerr << "NULL-pointer error\n";
	return traits;
}

/**
 * Get - the instance (pass by value) of a specific PFT (as defined by its name)
 * @param type PFT asked for
 * @return Object instance defining a PFT.
 */
SPftTraits* SPftTraits::createPftInstanceFromPftType(string type) {
	SPftTraits* traits = NULL;
	map<string, SPftTraits*>::iterator pos = PftLinkList.find(type);
	if (pos == (PftLinkList.end()))
		cerr << "type not found:" << type << endl;
	else
		traits = new SPftTraits(*pos->second);
	return traits;
}

SPftTraits* SPftTraits::createPftInstanceFromPftLink(SPftTraits* traits) {
	assert(traits != NULL);
	traits = new SPftTraits(*traits);
	return traits;
}

//-----------------------------------------------------------------------------
/**
 * Read definition of PFTs used in the simulation
 * @param file file containing PFT definitions
 * @param n default=-1; in case of monoculture runs, nb of PFT to test
 */
void SPftTraits::ReadPFTDef(const string& file, int n) {
	//delete old definitions
	for (map<string, SPftTraits*>::iterator i = SPftTraits::PftLinkList.begin();
			i != SPftTraits::PftLinkList.end();
			++i)
	{
		delete i->second;
	}
	//delete static pointer vectors
	SPftTraits::PftLinkList.clear();
	SPftTraits::pftInsertionOrder.clear(); // MSC

	//Open InitFile
	ifstream InitFile(file.c_str());
	if (!InitFile.good()) {
		cerr << ("Fehler beim �ffnen InitFile");
		cerr << file.c_str();
		exit(3);
	}
	cout << "InitFile: " << file << endl;

	string line;
	getline(InitFile, line); //skip header line
	//skip first lines if only one Types should be initiated
	if (n > -1) // MSC: This is a broken window.
		for (int x = 0; x < n; x++)
			getline(InitFile, line);

	int dummi1;
	string dummi2; // int PFTtype; string Cltype;

	do {
		// erstelle neue traits
		SPftTraits* traits = new SPftTraits();
//		SclonalTraits* cltraits=new SclonalTraits();
		// get type definitions from file
		InitFile >> dummi1;
		InitFile >> dummi2;
		InitFile >> traits->MaxAge >> traits->AllocSeed >> traits->LMR
				>> traits->m0 >> traits->MaxMass >> traits->SeedMass
				>> traits->Dist >> traits->pEstab >> traits->Gmax >> traits->SLA
				>> traits->palat >> traits->memory >> traits->RAR
				>> traits->growth >> traits->mThres >> traits->clonal
				>> traits->PropSex >> traits->meanSpacerlength
				>> traits->sdSpacerlength >> traits->Resshare >>
				traits->AllocSpacer >> traits->mSpacer;
		traits->name = dummi2; //=cltraits->name
		traits->TypeID = dummi1;
		//in Listen einf�gen..
		SPftTraits::addPftLink(dummi2, traits);
		pftInsertionOrder.push_back(dummi2); // MSC
//	    addClLink(dummi2,cltraits);
//		SPftTraits::PftList.push_back(traits);

		if (!InitFile.good() || n > -1) {
			return;
		}
	} while (!InitFile.eof());

//	return InitFile;
} //read PFT defs

/* MSC
 * Unfortunately, because the traits (i.e. LMR, m0, ...) are stored as variables
 * rather than inside a map, or some such, there is no simple way to parameterize
 * which values get varied. The reason is that one can't get a variable (i.e. LMR)
 * from a string (which would be the input value).
 *
 * Don't forget that you'll have to deliver the new traits to the seeds (and they'll in turn
 * create a plant which will then fecund new seeds).
 */
void SPftTraits::varyTraits() {
	// TODO: ensure that all these variables are named correctly.
	assert(myTraitType == SPftTraits::species);

	double variancePerLinkedTrait;
	myTraitType = SPftTraits::individual;

	variancePerLinkedTrait = CEnvir::normrand(1.0, SRunPara::RunPara.indivVariationSD);
	LMR = LMR * variancePerLinkedTrait;

	variancePerLinkedTrait = CEnvir::normrand(1.0, SRunPara::RunPara.indivVariationSD);
	m0 = m0 * variancePerLinkedTrait;
	MaxMass = MaxMass * variancePerLinkedTrait;
	SeedMass = SeedMass * variancePerLinkedTrait;
	Dist = Dist * variancePerLinkedTrait;

	variancePerLinkedTrait = CEnvir::normrand(1.0, SRunPara::RunPara.indivVariationSD);
	Gmax = Gmax * variancePerLinkedTrait;
	memory = memory * variancePerLinkedTrait;

	variancePerLinkedTrait = CEnvir::normrand(1.0, SRunPara::RunPara.indivVariationSD);
	palat = palat * variancePerLinkedTrait;
	SLA = SLA * variancePerLinkedTrait;
}

/**
 initialize clonal traits with default values
 * /
 SclonalTraits::SclonalTraits()  //clonal plant with specifc traits
 :name("default")//,PropSex(0.1),meanSpacerlength(17.5),sdSpacerlength(12.5),
 //Resshare(true),mSpacer(70),AllocSpacer(0.05),clonal(true)
 {
 }
 /* /---------------------------------------------------------------------------
 void SclonalTraits::ReadclonalTraits(char* file)    // not needed anymore -> new initialisation
 {
 std::string sfile=file;
 std::ifstream clonalFile;
 //open parameter file
 if (sfile=="") sfile=(CClonalGridEnvir::NameClonalPftFile);
 clonalFile.open(sfile.c_str());
 if (!clonalFile.good()) {std::cerr<<("Fehler beim �ffnen clonalFile");exit(3); }
 string line1;
 getline(clonalFile,line1);
 //*******************

 //loop for all clonal types
 for (int type=0; type<8; type++)
 {   int num; string name;SclonalTraits* temp=new SclonalTraits;
 //read plant parameter from inputfile
 clonalFile>>num
 //             >>(char*)clonalTraits[type].name
 >>name
 >>temp->PropSex
 >>temp->meanSpacerlength
 >>temp->sdSpacerlength
 >>temp->Resshare
 ;
 temp->name=name;
 clonalTraits.push_back(temp);
 }
 clonalFile.close();
 }
 */
//eof
