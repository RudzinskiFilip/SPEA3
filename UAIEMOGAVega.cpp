//---------------------------------------------------------------------------
#include <stdlib.h>
#pragma hdrstop

#include "UAIEMOGAVega.h"
#include "UAIERandomPermutation.h"
//---------------------------------------------------------------------------
__stdcall TAIEMultiobjectiveGeneticAlgorithmVEGA::TAIEMultiobjectiveGeneticAlgorithmVEGA(void)
			: TAIEMultiobjectiveGeneticAlgorithm()
{
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmVEGA::ExecuteNextGeneration(void)
{
 SelectSolutions(&FPopulation);
 CrossSolutions(&FPopulation);
 MutateSolutions(&FPopulation);
 EvaluateSolutions(&FPopulation);
}
//---------------------------------------------------------------------------
// Selekcja rozwi¹zañ do populacji rodzicielskiej
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmVEGA::SelectSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 // - utworzenie populacji rodzicielskiej
 TQuickList<TAIEMultiobjectiveGeneticSolution*> LPopulation;

 // - losowanie rozwi¹zañ z populacji (selekcja turniejowa)
 int LSubPopulationSize = APopulation->GetCount() / FObjectiveCount;
 TAIERandomPermutation RP(LSubPopulationSize);

 for (int i = 0; i < APopulation->GetCount(); ++i) {
	  int LObjectiveIndex = i / LSubPopulationSize;
	  if (LObjectiveIndex >= FObjectiveCount) LObjectiveIndex = FObjectiveCount - 1;

	  RP.Initialize(rand());

	  int w = RP.Random() + LObjectiveIndex * LSubPopulationSize;
	  double LObjectiveValue = APopulation->GetData(w)->FObjectiveValues[LObjectiveIndex];
	  for (int j = 0; j < FSelectionPressure; ++j) {
		   int k = RP.Random() + LObjectiveIndex * LSubPopulationSize;
		   if (LObjectiveValue > APopulation->GetData(k)->FObjectiveValues[LObjectiveIndex]) {
			   LObjectiveValue = APopulation->GetData(k)->FObjectiveValues[LObjectiveIndex];
			   w = k;
		   }
	  }

	  LPopulation.Add(SolutionClone(APopulation->GetData(w)));
 }

 // - zamiana poprzedniej populacji na populacjê rodzicielsk¹
 for (int i = 0; i < APopulation->GetCount(); ++i) {
	  TAIEMultiobjectiveGeneticSolution* Tmp = APopulation->GetData(i);
	  APopulation->SetData(i, LPopulation.GetData(i));
	  LPopulation.SetData(i, Tmp);
	  //SWAP(LPopulation[i], APopulation[i]);
	  SolutionDelete(LPopulation.GetData(i));
 }
}
//---------------------------------------------------------------------------
