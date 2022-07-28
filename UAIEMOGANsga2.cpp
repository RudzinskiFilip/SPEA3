//---------------------------------------------------------------------------
#include <stdlib.h>
#include <math.h>
#pragma hdrstop

#include "UAIEMOGANsga2.h"
#include "UAIERandomPermutation.h"
//---------------------------------------------------------------------------
int __stdcall COMPARE_CROWDING_DISTANCE(TAIEMultiobjectiveGeneticSolution* const& S1, TAIEMultiobjectiveGeneticSolution* const& S2)
{
 if (S1->GetRank() > S2->GetRank()) return +1;
 if (S1->GetRank() < S2->GetRank()) return -1;
 if (S1->GetDistance() < S2->GetDistance()) return +1;
 if (S1->GetDistance() > S2->GetDistance()) return -1;
 return 0;
}
//---------------------------------------------------------------------------
int __stdcall COMPARE_OBJECT(TAIEMultiobjectiveGeneticSolution* const& S1, TAIEMultiobjectiveGeneticSolution* const& S2, int ObjectiveIndex)
{
 if (S1->GetObjectiveValue(ObjectiveIndex) > S2->GetObjectiveValue(ObjectiveIndex)) return +1;
 if (S1->GetObjectiveValue(ObjectiveIndex) < S2->GetObjectiveValue(ObjectiveIndex)) return -1;
 return 0;
}
//---------------------------------------------------------------------------
// TAIEMultiobjectiveGeneticAlgorithmNSGA2
//---------------------------------------------------------------------------
__stdcall TAIEMultiobjectiveGeneticAlgorithmNSGA2::TAIEMultiobjectiveGeneticAlgorithmNSGA2(void)
			: TAIEMultiobjectiveGeneticAlgorithm()
{
}
//---------------------------------------------------------------------------
__stdcall TAIEMultiobjectiveGeneticAlgorithmNSGA2::~TAIEMultiobjectiveGeneticAlgorithmNSGA2(void)
{
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmNSGA2::ExecuteNextGeneration(void)
{
 // - utworzenie populacji potomnej Q i populacji R (sumy Q i P)
 TQuickList<TAIEMultiobjectiveGeneticSolution*> QPopulation;
 TQuickList<TAIEMultiobjectiveGeneticSolution*> RPopulation;

 // - wyznaczenie populacji potomnej
 SelectSolutions(&QPopulation, &FPopulation);
 CrossSolutions(&QPopulation);
 MutateSolutions(&QPopulation);
 EvaluateSolutions(&QPopulation);
 EvaluateMinMaxAvg();

 // - przepisanie populacji bie¿¹cej P i potomnej Q do populacji R
 for (int q = 0; q < FPopulation.GetCount(); ++q) RPopulation.Add(FPopulation.GetData(q));
 for (int q = 0; q < QPopulation.GetCount(); ++q) RPopulation.Add(QPopulation.GetData(q));

 // - sortowanie populacji R
 FastNondominatedSort(&RPopulation, &FFronts);

 // - wybranie najlepszych rozwi¹zañ z kolejnych frontów i zapisanie ich do P
 int i = 0;
 int j = 0;
 while (j + FFronts.GetData(i)->GetCount() < FPopulation.GetCount()) {
	  for (int k = 0; k < FFronts.GetData(i)->GetCount(); ++k) FPopulation.SetData(j++, FFronts.GetData(i)->GetData(k));
	  ++i;
 }
 int k = 0;

 if (j < FPopulation.GetCount()) {
	 CrowdingDistanceAssignment(FFronts.GetData(i));
	 FFronts.GetData(i)->Sort(COMPARE_CROWDING_DISTANCE);

	 while (j < FPopulation.GetCount()) FPopulation.SetData(j++, FFronts.GetData(i)->GetData(k++));
 }

 // - usuwanie zbêdnych, niewybranych rozwi¹zañ
 //while (k < FFronts[i]->GetCount()) SolutionDelete((*FFronts[i])[k++]);
 //while (++i < FFronts.GetCount()) ClearSolutions(*(FFronts[i]));

 while (k < FFronts.GetData(i)->GetCount()) {SolutionDelete(FFronts.GetData(i)->GetData(k)); FFronts.GetData(i)->Delete(k);}
 ++i;
 while (i < FFronts.GetCount()) {ClearSolutions(FFronts.GetData(i)); FFronts.Delete(i);}
}
//---------------------------------------------------------------------------
// Selekcja rozwi¹zañ do populacji rodzicielskiej
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmNSGA2::SelectSolutions(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* AQ,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* AP)
{
 // - losowanie rozwi¹zañ z populacji (selekcja turniejowa)
 TAIERandomPermutation RP(AP->GetCount());

 for (int i = 0; i < AP->GetCount(); ++i) {
	  RP.Initialize(FRG.RandomInt());

	  TAIEMultiobjectiveGeneticSolution* WSolution = AP->GetData(RP.Random());
	  for (int i = 0; i < FSelectionPressure; ++i) {
		   TAIEMultiobjectiveGeneticSolution* LSolution = AP->GetData(RP.Random());
		   if (COMPARE_CROWDING_DISTANCE(WSolution, LSolution) < 0) WSolution = LSolution;
	  }

	  AQ->Add(SolutionClone(WSolution));
 }
}
//---------------------------------------------------------------------------
// Obliczanie odleg³oœci pomiêdzy rozwi¹zaniami
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmNSGA2::CrowdingDistanceAssignment(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 int L = APopulation->GetCount();
 for (int i = 0; i < APopulation->GetCount(); ++i) APopulation->GetData(i)->FDistance = 0;
 for (int i = 0; i < FObjectiveCount; ++i) {
	  double Range = GetObjectiveMaxValue(i) - GetObjectiveMinValue(i);
	  if (Range == 0.0) continue;
	  APopulation->Sort(COMPARE_OBJECT, i);
	  APopulation->GetData(0)->FDistance = MAX_REAL_VALUE;
	  APopulation->GetData(L - 1)->FDistance = MAX_REAL_VALUE;
	  for (int j = 1; j < L - 1; ++j) APopulation->GetData(j)->FDistance +=
		  (APopulation->GetData(j + 1)->FObjectiveValues[i] - APopulation->GetData(j - 1)->FObjectiveValues[i]) / Range;
 }
}
//---------------------------------------------------------------------------

