
#ifndef CSeedH
#define CSeedH

#include <memory>

class CCell;
class CPlant;
class SPftTraits;

class CSeed
{
	protected:
	   CCell* cell;

	public:
	   std::shared_ptr<SPftTraits> Traits;

	   double mass;
	   double estab;
	   int Age;
	   bool remove;

	   CSeed(std::shared_ptr<CPlant> plant, CCell* cell);
	   CSeed(double estab, std::shared_ptr<SPftTraits> traits, CCell* cell);
	   ~CSeed();

	   CCell* getCell(){ return cell; };

	   static bool GetSeedRemove(const std::unique_ptr<CSeed> & s) {
		   return s->remove;
	   };
};

#endif
