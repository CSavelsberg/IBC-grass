#ifndef environmentH
#define environmentH

#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

#include "CGrid.h"
#include "Output.h"
#include "RandomGenerator.h"


//---------------------------------------------------------------------------
/// virtual Basic Results Class with general static simulation parameters
/** The class contains
 - simulation-wide (static) information on
 - Names of in- and output-files,
 - an Random Number Generator (plus some service functions), and
 - a template for above-and belowground resources as well as
 - current simulation status (current year, week etc.)
 - variables storing result information on grid and single pfts
 - functions
 - collecting and writing results to output-files
 - reading-in Resource data
 - core function OneWeek(), running a week of the simulation
 \par time scales of the simulations:
 - 1 step = 1 week
 - 1 year = 30 weeks
 */
class CEnvir
{

public:
	static RandomGenerator rng;
	static Output output;

	static std::map<std::string, long> PftInitList; // list of Pfts used
	static std::map<std::string, int> PftSurvTime;	// array for survival times of PFTs [years];

	static std::vector<double> AResMuster; // mean above-ground resource availability [resource units per cm^2]
	static std::vector<double> BResMuster; // mean below-ground resource availability [resource units per cm^2]

	static int week;       		// current week (0-30)
	static int year;        	// current year
	static int WeeksPerYear;  	// number of weeks per year (constantly at value 30)
	static int NRep;        	// number of replications -> read from SimFile;
	static int SimNr;       	// simulation-ID
	static int ComNr;			// Community identifier for multiple parameter settings of the same community.
	static int RunNr;       	// repetition number

	CEnvir();
	virtual ~CEnvir();

	static void ReadLandscape(); 	// Populated grid space with resources
	void GetSim(std::string data); 	// Simulation read in

	static void ResetT() {
		year = 1;
		week = 0;
	};

	static void NewWeek() {
		week++;
		if (week > WeeksPerYear) {
			week = 1;
			year++;
		}
	};

	/*
	 * Helper function for comparing floating point numbers for equality
	 */
	static bool AreSame(double a, double b) {
	    return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
	}

	/**
	 * \name core simulation functions (virtual)
	 * Functions needed for simulation runs.
	 * To be defined in inheriting classes.
	 */
	virtual void InitRun();   // init a new run
	virtual void OneWeek()=0; // calls all weekly processes
	virtual void OneYear()=0; // runs one year in default mode
	virtual void OneRun()=0;  // runs one simulation run in default mode

};
//---------------------------------------------------------------------------
#endif
