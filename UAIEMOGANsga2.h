//---------------------------------------------------------------------------
#ifndef UAIEMOGANsga2H
#define UAIEMOGANsga2H
//---------------------------------------------------------------------------
#include "UAIEMOGABase.h"
#include "UAIERandomGenerator32.h"
//---------------------------------------------------------------------------
class TAIEMultiobjectiveGeneticAlgorithmNSGA2 : public TAIEMultiobjectiveGeneticAlgorithm
{
 private:
		TAIERandomGenerator32 FRG;

		virtual void __stdcall SelectSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* AQ,
											   TQuickList<TAIEMultiobjectiveGeneticSolution*>* AP);
		virtual void __stdcall CrowdingDistanceAssignment(TQuickList<TAIEMultiobjectiveGeneticSolution*>* AF);

 protected:
		virtual void __stdcall ExecuteNextGeneration(void);

 public:
		__stdcall TAIEMultiobjectiveGeneticAlgorithmNSGA2(void);
		virtual __stdcall ~TAIEMultiobjectiveGeneticAlgorithmNSGA2(void);
};
//---------------------------------------------------------------------------
#endif
