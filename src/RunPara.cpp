/**\file
\brief constructor of struct SRunPara and Initialization of static Variables
*/
//---------------------------------------------------------------------------
//#pragma hdrstop

#include "RunPara.h"
#include "CEnvir.h"
#include "SPftTraits.h"

//#include <iostream>
#include <cstdlib>
#include <sstream>
//---------------------------------------------------------------------------
//#pragma package(smart_init)
//Input Files
std::string SRunPara::NamePftFile; // trait file for experiment species
std::string SRunPara::NameSimFile = "data/in/SimFile.txt"; //file with simulation scenarios

SRunPara SRunPara::RunPara=SRunPara();
//-------------------------------------------------------------------
SRunPara::SRunPara() :
		Version(version1), ITV(off), ITVsd(0),
		AboveCompMode(sym), BelowCompMode(sym), PFT(1), SPAT(1), SPATyear(0), COMP(1),
		mort_base(0.007), LitterDecomp(0.5), DiebackWinter(0.5), EstabRamet(1),
		GridSize(173), CellNum(173), Tmax(10), NPft(81), GrazProb(0), PropRemove(0.5),
		BitSize(0.5), MassUngraz(15300), BelGrazProb(0), BelPropRemove(0), BelGrazMode(0),
		BGThres(1), HetBG(false), CutMass(5000), NCut(0), torus(true), DistAreaYear(0),
		AreaEvent(0.1), mort_seeds(0.5), meanARes(100), meanBRes(100), Rootherb(false),
		Aampl(0), Bampl(0), SeedInput(0), SeedRainType(0)
{
	;
}

/**
 *
 * PFT, SPATyear, ITVsd, SPAT, COMP
\author MSC
\date  19-01-2015
*/
std::string SRunPara::toString(){
	std::stringstream mystream;
	mystream << Version << "\t"
			<< ITVsd << "\t"
			<< Tmax << "\t"
			<< meanARes << "\t"
			<< meanBRes << "\t"
			<< GrazProb << "\t"
			<< PropRemove << "\t"
			<< NCut << "\t"
			<< CutMass << "\t"
			<< DistAreaYear << "\t"
			<< AreaEvent << "\t"
			<< SeedRainType << "\t"
			<< SeedInput << "\t"
			;

	return mystream.str();
}

/*
 * MSC
 * 19-01-2015
 */
std::string SRunPara::headerToString() {
	std::stringstream mystream;
	mystream
			<< "IC_vers" << "\t"
			<< "ITVsd" << "\t"
			<< "Tmax" << "\t"
			<< "ARes" << "\t"
			<< "BRes" << "\t"
			<< "GrazProb" << "\t"
			<< "PropRemove" << "\t"
			<< "NCut" << "\t"
			<< "CutMass" << "\t"
			<< "DistAreaYear" << "\t"
			<< "AreaEvent" << "\t"
			<< "SeedRainType" << "\t"
			<< "SeedInput" << "\t"
			;
	return mystream.str();
}

void SRunPara::setRunPara(std::string def){
	using namespace std;
	stringstream dummi; dummi<<def;
	string d;

  dummi>>d;
  switch(atoi(d.c_str())){
  case 0: Version=version1;break;
  case 1:Version=version2;break;
  case 2:Version=version3;break;
  default:break;
  }
  dummi>>d;//AboveCompMode=atoi(d.c_str());
  switch(atoi(d.c_str())){
  case 0: AboveCompMode=sym;break;
  case 1: AboveCompMode=asympart;break;
  case 2: AboveCompMode=asymtot;break;
  default:break;
  }
  dummi>>d;//BelowCompMode=atoi(d.c_str());
  switch(atoi(d.c_str())){
  case 0: BelowCompMode=sym;break;
  case 1: BelowCompMode=asympart;break;
  case 2: BelowCompMode=asymtot;break;
  default:break;
  }
  dummi>> GridSize; CellNum=GridSize;
  dummi>> Tmax>> torus;
  dummi>> mort_seeds >> EstabRamet >> mort_base >> LitterDecomp >> DiebackWinter;
  dummi>> GrazProb>> PropRemove>>BitSize >> BelGrazProb >>BelPropRemove>> BelGrazMode
       >> BGThres>> HetBG >> NCut>> CutMass >> meanARes>> meanBRes>>DistAreaYear
       >> AreaEvent;
}

std::string SRunPara::getFileID() {

	string t = to_string(CEnvir::SimNr) + "_" + to_string(CEnvir::ComNr) + "_" + to_string(CEnvir::RunNr);
	return t;

}
//eof  ---------------------------------------------------------------------
