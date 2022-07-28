//---------------------------------------------------------------------------
#ifndef UAIEMOGAeNsga2H
#define UAIEMOGAeNsga2H
//---------------------------------------------------------------------------
#include "UAIEMOGABase.h"
#include "UAIEMOGANSGA2.h"
//---------------------------------------------------------------------------
class TAIEMultiobjectiveGeneticAlgorithmENSGA2 : public TAIEMultiobjectiveGeneticAlgorithmNSGA2
{
 private:
		int    FGridSize;

		TQuickList< TAIEMultiobjectiveGeneticSolution* > FArchive;

 protected:
		virtual void __stdcall ExecuteFirstGeneration(void);
		virtual void __stdcall ExecuteNextGeneration(void);

 public:
		__stdcall TAIEMultiobjectiveGeneticAlgorithmENSGA2(void);
		virtual __stdcall ~TAIEMultiobjectiveGeneticAlgorithmENSGA2(void);

		virtual int __stdcall GetFinalSolutionCount(void);
		virtual TAIEMultiobjectiveGeneticSolution* __stdcall GetFinalSolution(int ASolutionIndex);

		int __stdcall GetGridSize(void);
		void __stdcall SetGridSize(int AGridSize);

		virtual double __stdcall GetObjectiveEpsilon(int AObjectiveIndex, double AValue);
};
//---------------------------------------------------------------------------
#endif
