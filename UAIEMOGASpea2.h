//---------------------------------------------------------------------------
#ifndef UAIEMOGASpea2H
#define UAIEMOGASpea2H
//---------------------------------------------------------------------------
#include "UAIEMOGABase.h"
//---------------------------------------------------------------------------
class TAIEMultiobjectiveGeneticAlgorithmSPEA2 : public TAIEMultiobjectiveGeneticAlgorithm
{
 protected:
		int    FFrontMaxSize;
		TQuickList<TAIEMultiobjectiveGeneticSolution*> FArchive;

		virtual void __stdcall SelectSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* AArchive,
											   TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);
		virtual void __stdcall EvaluateFitness(TQuickList<TAIEMultiobjectiveGeneticSolution*>* AArchive,
											   TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);
		virtual void __stdcall CopyBestSolutionsToArchive(TQuickList<TAIEMultiobjectiveGeneticSolution*>* AArchive,
														  TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);

		virtual double __stdcall DistanceToKNearestNeighbour(TAIEMultiobjectiveGeneticSolution* ASolution,
													 TQuickList<TAIEMultiobjectiveGeneticSolution*>* AArchive,
													 TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);

 protected:
		virtual void __stdcall ExecuteFirstGeneration(void);
		virtual void __stdcall ExecuteNextGeneration(void);

 public:
		__stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA2(void);
		virtual __stdcall ~TAIEMultiobjectiveGeneticAlgorithmSPEA2(void);

		virtual int __stdcall GetFinalSolutionCount(void);
		virtual TAIEMultiobjectiveGeneticSolution* __stdcall GetFinalSolution(int ASolutionIndex);

		int __stdcall GetFrontMaxSize(void);
		void __stdcall SetFrontMaxSize(int AFrontMaxSize);
};

//---------------------------------------------------------------------------
int __stdcall COMPARE_DOUBLE(const double& A, const double& B);
int __stdcall COMPARE_FITNESS(TAIEMultiobjectiveGeneticSolution* const& A, TAIEMultiobjectiveGeneticSolution* const& B);
//---------------------------------------------------------------------------
#endif
