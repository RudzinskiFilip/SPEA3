//---------------------------------------------------------------------------
#ifndef UAIEMOGAVegaH
#define UAIEMOGAVegaH
//---------------------------------------------------------------------------
#include "UAIEMOGABase.h"
//---------------------------------------------------------------------------
class TAIEMultiobjectiveGeneticAlgorithmVEGA : public TAIEMultiobjectiveGeneticAlgorithm
{
 private:
		void __stdcall ExecuteNextGeneration(void);

		void __stdcall SelectSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);

 public:
		__stdcall TAIEMultiobjectiveGeneticAlgorithmVEGA(void);
};
//---------------------------------------------------------------------------
#endif
