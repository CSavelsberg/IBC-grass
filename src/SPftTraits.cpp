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
#include <sstream>

#include "SPftTraits.h"
#include "CEnvir.h"

/*
 * Default constructor
 */
SPftTraits::SPftTraits() :
		TypeID(999), name("default"), MaxAge(100), AllocSeed(0.05), LMR(0), m0(
				0), MaxMass(0), SeedMass(0), Dist(0), pEstab(0.5), Gmax(0), memory(
				0), SLA(0), palat(0), RAR(1), growth(0.25), mThres(0.2), Dorm(
				1), FlowerWeek(16), DispWeek(20), PropSex(0.1), meanSpacerlength(
				17.5), sdSpacerlength(12.5), Resshare(true), mSpacer(70), AllocSpacer(
				0.05), clonal(true), myTraitType(SPftTraits::species)
{

}

/*
 * Constructor copying a given set of traits. This is necessary for IBC-grass.ITV.
 */
SPftTraits::SPftTraits(const SPftTraits& s) :
		TypeID(s.TypeID), name(s.name), MaxAge(s.MaxAge), AllocSeed(s.AllocSeed), LMR(s.LMR),
		m0(s.m0), MaxMass(s.MaxMass), SeedMass(s.SeedMass), Dist(s.Dist), pEstab(s.pEstab),
		Gmax(s.Gmax), memory(s.memory), SLA(s.SLA), palat(s.palat), RAR(s.RAR), growth(s.growth),
		mThres(s.mThres), Dorm(s.Dorm), FlowerWeek(s.FlowerWeek), DispWeek(s.DispWeek),
		PropSex(s.PropSex), meanSpacerlength(s.meanSpacerlength),sdSpacerlength(s.sdSpacerlength),
		Resshare(s.Resshare), mSpacer(s.mSpacer), AllocSpacer(s.AllocSpacer), clonal(s.clonal),
		myTraitType(s.myTraitType)
{
	if (SRunPara::RunPara.ITV == off) {
		assert(s.myTraitType == SPftTraits::species);
	}

	if (s.myTraitType == SPftTraits::individualized) {
		assert(SRunPara::RunPara.ITV == on);
	}
}

SPftTraits::~SPftTraits() {
	// TODO Auto-generated destructor stub
}

//std::vector<SPftTraits*> SPftTraits::PftList;//(CRunPara::RunPara.NPft,new SPftTraits);
//std::vector<SclonalTraits*> SclonalTraits::clonalTraits;//(8,new SclonalTraits());
map<string, SPftTraits*> SPftTraits::PftLinkList = map<string, SPftTraits*>();

/** MSC
 * Records every "individualized" trait syndrome (or rather, the pointer to it).
 * This is used to remove them after each run. By the end of a 100 year run with 128cm grid cells,
 * there will be about 2GB of memory used. To improve this algorithm (though complicating the code),
 * one could delete the trait syndromes after their final use (i.e. a seed does not germinate, or a
 * plant dies). One must be cognizant to ensure that:
 *				0) all plants use individualized traits (though they do not necessarily have to vary) so that the
 *					destructor doesn't accidentally delete a species level trait. Or, if individualization is off (that is,
 *					no traits are actually being varied), all copies can just be deleted with their parent's (plant or seed) destructor.
 *				1) all seeds pass their traits to the plants they become (enabled by the use of SPftTrait's copy constructor) inside
 *					the plant constructor.
 *				2) all seeds and plants, when they die, remove their own traits.
 *				3) they got rid of the individualPftList (as all individualPfts have a very short life).
 */

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

	//Open InitFile
	if(SRunPara::RunPara.verbose) cout << file << endl;
	ifstream InitFile(file.c_str());
	if (!InitFile.good()) {
		cerr << ("Fehler beim �ffnen InitFile");
		cerr << file.c_str();
		exit(3);
	}
	if(SRunPara::RunPara.verbose) cout << "InitFile: " << file << endl;

	string line;
	getline(InitFile, line); //skip header line
	//skip first lines if only one Types should be initiated
	if (n > -1) // MSC: This is a broken window.
		for (int x = 0; x < n; x++)
			getline(InitFile, line);

	int dummi1;
	string dummi2; // int PFTtype; string Cltype;

	do {
		SPftTraits* traits = new SPftTraits();
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
		traits->name = dummi2;
		traits->TypeID = dummi1;

		SPftTraits::addPftLink(dummi2, traits);

		if (!InitFile.good() || n > -1) {
			return;
		}
	} while (!InitFile.eof());
}

/* MSC
 * Vary the current individual's traits, based on a Gaussian distribution with a
 * standard deviation of "ITVsd". Sub-traits that are tied will vary accordingly.
 * Bounds on 1 and -1 ensure that no trait garners a negative value and keep the resulting
 * distribution balanced. Other, trait-specific, requirements are checked as well. (e.g.,
 * LMR cannot be greater than 1, memory cannot be less than 1).
 */
void SPftTraits::varyTraits() {
	assert(myTraitType == SPftTraits::species);
	assert(SRunPara::RunPara.ITV == on);

	myTraitType = SPftTraits::individualized;
	double dev;

//	cout << "Varying traits! LMR..." << endl;
	double LMR_;
	do {
		dev = CEnvir::normrand(0, SRunPara::RunPara.ITVsd);
		LMR_ = LMR + (LMR * dev);
	} while (dev < -1.0 || dev > 1.0 || LMR_ < 0 || LMR_ > 1);
	LMR = LMR_;
//	cout << "LMR: " << LMR << endl;

//	cout << "Varying traits! Mass..." << endl;
	double m0_, MaxMass_, SeedMass_, Dist_;
	do {
		dev = CEnvir::normrand(0, SRunPara::RunPara.ITVsd);
		m0_ = m0 + (m0 * dev);
		MaxMass_ = MaxMass + (MaxMass * dev);
		SeedMass_ = SeedMass + (SeedMass * dev);
		Dist_ = Dist - (Dist * dev);
	} while (dev < -1.0 || dev > 1.0 || m0_ < 0 || MaxMass_ < 0 || SeedMass_ < 0 || Dist_ < 0);
	m0 = m0_;
	MaxMass = MaxMass_;
	SeedMass = SeedMass_;
	Dist = Dist_;
//	cout << "m0: " << m0 << endl;
//	cout << "MaxMass: " << MaxMass << endl;
//	cout << "SeedMass: " << SeedMass << endl;
//	cout << "Dist: " << Dist << endl;

	//	cout << "Varying traits! Gmax..." << endl;
	double Gmax_;
	int memory_;
	do {
//		cout << "Drawing variate..." << endl;
		dev = CEnvir::normrand(0, SRunPara::RunPara.ITVsd);
//		cout << "Setting Gmax ..." << endl;
		Gmax_ = Gmax + (Gmax * dev);
//		cout << "Setting memory..." << endl;
		memory_ = (int) round((double) memory - ((double) memory * dev));
//		cout << "Testing values: dev: " << dev << " Gmax: " << Gmax_ << " memory: " << memory_  << endl;
	} while (dev < -1.0 || dev > 1.0 || Gmax_ < 0 || memory_ == 0);
	Gmax = Gmax_;
	memory = memory_;
//	cout << "Gmax: " << Gmax << endl;
//	cout << "Memory: " << memory << endl;

//	cout << "Varying traits! Palat..." << endl;
	double palat_, SLA_;
	do {
		dev = CEnvir::normrand(0, SRunPara::RunPara.ITVsd);
		palat_ = palat + (palat * dev);
		SLA_ = SLA + (SLA * dev);
	} while (dev < -1.0 || dev > 1.0 || palat_ < 0 || palat_ > 1 || SLA_ < 0);
	palat = palat_;
	SLA = SLA_;
//	cout << "palat: " << palat << endl;
//	cout << "SLA: " << SLA << endl;

	//	cout << "Varying traits! Spacers..." << endl;
	double meanSpacerlength_, sdSpacerlength_;
	do {
		dev = CEnvir::normrand(0, SRunPara::RunPara.ITVsd);
		meanSpacerlength_ = meanSpacerlength + (meanSpacerlength * dev);
		sdSpacerlength_ = sdSpacerlength + (sdSpacerlength * dev);
	} while (dev < -1.0 || dev > 1.0 || meanSpacerlength_ < 0 || sdSpacerlength_ < 0);
	meanSpacerlength = meanSpacerlength_;
	sdSpacerlength = sdSpacerlength_;
//	cout << "meanSpacerlength: " << meanSpacerlength << endl;
//	cout << "sdSpacerlength: " << sdSpacerlength << endl;

//	cout << "Done varying traits!" << endl;

}

string SPftTraits::toString()
{
	std::stringstream mystream;
	mystream << name << '\t'
			<< AllocSeed << "\t"
			<< LMR << "\t"
			<< m0 << "\t"
			<< MaxMass << "\t"
			<< SeedMass << "\t"
			<< Dist << "\t"
			<< pEstab << "\t"
			<< Gmax << "\t"
			<< SLA << "\t"
			<< palat << "\t"
			<< memory << "\t"
			<< RAR << "\t"
			<< growth << "\t"
			<< mThres << "\t"
			<< clonal << "\t"
			<< PropSex << "\t"
			<< meanSpacerlength << "\t"
			<< sdSpacerlength << "\t"
			<< Resshare << "\t"
			<< AllocSpacer << "\t"
			<< mSpacer << "\t"
			;

	return mystream.str();
}

string SPftTraits::headerToString()
{
	std::stringstream mystream;
	mystream << "PFT" << "\t"
			<< "AllocSeed" << "\t"
			<< "LMR" << "\t"
			<< "m0" << "\t"
			<< "MaxMass" << "\t"
			<< "SeedMass" << "\t"
			<< "Dist" << "\t"
			<< "pEstab" << "\t"
			<< "Gmax" << "\t"
			<< "SLA" << "\t"
			<< "palat" << "\t"
			<< "memory" << "\t"
			<< "RAR" << "\t"
			<< "growth" << "\t"
			<< "mThres" << "\t"
			<< "clonal" << "\t"
			<< "PropSex" << "\t"
			<< "meanSpacerlength" << "\t"
			<< "sdSpacerlength" << "\t"
			<< "Resshare" << "\t"
			<< "AllocSpacer" << "\t"
			<< "mSpacer" << "\t"
		;

	return mystream.str();
}
