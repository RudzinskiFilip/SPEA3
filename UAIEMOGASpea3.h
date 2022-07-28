//---------------------------------------------------------------------------
#ifndef UAIEMOGASpea3H
#define UAIEMOGASpea3H
//---------------------------------------------------------------------------
#include "UAIEMOGABase.h"
#include "UAIEMOGASpea2.h"
//---------------------------------------------------------------------------
class TAIEMultiobjectiveGeneticAlgorithmSPEA3 : public TAIEMultiobjectiveGeneticAlgorithmSPEA2
{
 private:
		TQuickList<TAIEMultiobjectiveGeneticSolution*> MArchive;

		virtual void __stdcall CopyBestSolutionsToArchive(TQuickList<TAIEMultiobjectiveGeneticSolution*>* AArchive,
														  TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);

		void __stdcall AddAuxiliarySolutionsToArchive(TQuickList<TAIEMultiobjectiveGeneticSolution*>* NArchive,
												TQuickList<TAIEMultiobjectiveGeneticSolution*>* XArchive);
		void __stdcall ExchangeDominatedSolutionsInArchive(TQuickList<TAIEMultiobjectiveGeneticSolution*>* NArchive,
												TQuickList<TAIEMultiobjectiveGeneticSolution*>* XArchive);
		void __stdcall FullAlignDistancesBetweenSolutionsInArchive(TQuickList<TAIEMultiobjectiveGeneticSolution*>* NArchive,
												TQuickList<TAIEMultiobjectiveGeneticSolution*>* XArchive);
		bool __stdcall StepAlignDistancesBetweenSolutionsInArchive(TQuickList<TAIEMultiobjectiveGeneticSolution*>* NArchive,
												TQuickList<TAIEMultiobjectiveGeneticSolution*>* XArchive);

		bool __stdcall FindNearestNeighourhood(TAIEMultiobjectiveGeneticSolution* ASolution,
											   TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation,
											   double& ADistance, int& AIndex);
		bool __stdcall FindFarthestNeighourhood(TAIEMultiobjectiveGeneticSolution* ASolution,
												TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation,
												double& ADistance, int& AIndex);
		void __stdcall ClearDominatedSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* NArchive);

//		double __stdcall SumDistanceToKNearestNeighbourhoods(TAIEMultiobjectiveGeneticSolution* ASolution,
//															 TQuickList<TAIEMultiobjectiveGeneticSolution*>& APopulation, int K);

 public:
		__stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA3(void);
		virtual __stdcall ~TAIEMultiobjectiveGeneticAlgorithmSPEA3(void);
};
//---------------------------------------------------------------------------
#endif
