//---------------------------------------------------------------------------
#include <stdlib.h>
#include <math.h>
#pragma hdrstop

#include "UAIEMOGASpea2.h"
#include "UAIERandomPermutation.h"
//---------------------------------------------------------------------------
int __stdcall COMPARE_DOUBLE(const double& A, const double& B)
{
 if (A > B) return +1;
 if (A < B) return -1;
 return 0;
}
//---------------------------------------------------------------------------
int __stdcall COMPARE_FITNESS(TAIEMultiobjectiveGeneticSolution* const& A, TAIEMultiobjectiveGeneticSolution* const& B)
{
 if (A->GetFitnessValue() > B->GetFitnessValue()) return +1;
 if (A->GetFitnessValue() < B->GetFitnessValue()) return -1;
 return 0;
}
//---------------------------------------------------------------------------
// TAIEMultiobjectiveGeneticAlgorithmSPEA2
//---------------------------------------------------------------------------
__stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA2::TAIEMultiobjectiveGeneticAlgorithmSPEA2(void)
			: TAIEMultiobjectiveGeneticAlgorithm()
{
 FFrontMaxSize = 50;
}
//---------------------------------------------------------------------------
__stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA2::~TAIEMultiobjectiveGeneticAlgorithmSPEA2(void)
{
 ClearSolutions(&FArchive);
}
//---------------------------------------------------------------------------
int __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA2::GetFrontMaxSize(void)
{
 return FFrontMaxSize;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA2::SetFrontMaxSize(int AFrontMaxSize)
{
 if (!GetIsPrepared()) FFrontMaxSize = AFrontMaxSize;
}
//---------------------------------------------------------------------------
int __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA2::GetFinalSolutionCount(void)
{
 return FArchive.GetCount();
}
//---------------------------------------------------------------------------
TAIEMultiobjectiveGeneticSolution* __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA2::GetFinalSolution(int ASolutionIndex)
{
 return FArchive.GetData(ASolutionIndex);
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA2::ExecuteFirstGeneration(void)
{
 // - czyszczenie archiwum
 ClearSolutions(&FArchive);
 // - obliczanie funkcji dopasowania
 EvaluateFitness(&FArchive, &FPopulation);
 // - kopiowanie niezdominowanych rozwi¹zañ do archiwum
 CopyBestSolutionsToArchive(&FArchive, &FPopulation);
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA2::ExecuteNextGeneration(void)
{
 // - wyznaczenie populacji potomnej
 SelectSolutions(&FArchive, &FPopulation);
 CrossSolutions(&FPopulation);
 MutateSolutions(&FPopulation);
 EvaluateSolutions(&FPopulation);
 EvaluateMinMaxAvg();

 // - ocena rozwi¹zañ
 EvaluateFitness(&FArchive, &FPopulation);
 // - kopiowanie niezdominowanych rozwi¹zañ do archiwum
 CopyBestSolutionsToArchive(&FArchive, &FPopulation);
}
//---------------------------------------------------------------------------
// Selekcja rozwi¹zañ do populacji rodzicielskiej
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA2::SelectSolutions(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* AArchive,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 // - utworzenie populacji rodzicielskiej
 TVector<TAIEMultiobjectiveGeneticSolution*> LPopulation;

 // - losowanie rozwi¹zañ z populacji i z archiwum (selekcja turniejowa)
 for (int i = 0; i < APopulation->GetCount(); ++i) {
	  TAIERandomPermutation RP(AArchive->GetCount() + APopulation->GetCount());
	  RP.Initialize(rand());

	  int x = RP.Random();
	  TAIEMultiobjectiveGeneticSolution* WSolution = (x < AArchive->GetCount()) ? AArchive->GetData(x) : APopulation->GetData(x - AArchive->GetCount());
	  for (int j = 0; j < FSelectionPressure; ++j) {
		   x = RP.Random();
		   TAIEMultiobjectiveGeneticSolution* LSolution = (x < AArchive->GetCount()) ? AArchive->GetData(x) : APopulation->GetData(x - AArchive->GetCount());
		   if (WSolution->FFitnessValue > LSolution->FFitnessValue) WSolution = LSolution;
	  }
	  LPopulation.Add(SolutionClone(WSolution));
 }

 // - zamiana poprzedniej populacji na populacjê rodzicielsk¹
 for (int i = 0; i < APopulation->GetCount(); ++i) {
	  TAIEMultiobjectiveGeneticSolution* Tmp = APopulation->GetData(i);
	  APopulation->SetData(i, LPopulation[i]);
	  LPopulation[i] = Tmp;
	  //SWAP(LPopulation[i], APopulation[i]);
	  SolutionDelete(LPopulation[i]);
 }
}
//---------------------------------------------------------------------------
// Obliczanie funkcji dopasowania dla ka¿dego rozwi¹zania w archium i w populacji
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA2::EvaluateFitness(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* AArchive,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 // - obliczenie czêœci S funkcji dopasowania dla rozwi¹zañ w archiwum i populacji
 for (int i = 0; i < AArchive->GetCount(); ++i) {
	  AArchive->GetData(i)->FStrength = 0.0;
	  for (int j = 0; j < AArchive->GetCount(); ++j)
		   if (AArchive->GetData(i)->Dominates(AArchive->GetData(j))) AArchive->GetData(i)->FStrength++;
	  for (int j = 0; j < APopulation->GetCount(); ++j)
		   if (AArchive->GetData(i)->Dominates(APopulation->GetData(j))) AArchive->GetData(i)->FStrength++;
 }
 for (int i = 0; i < APopulation->GetCount(); ++i) {
	  APopulation->GetData(i)->FStrength = 0.0;
	  for (int j = 0; j < AArchive->GetCount(); ++j)
		   if (APopulation->GetData(i)->Dominates(AArchive->GetData(j))) APopulation->GetData(i)->FStrength++;
	  for (int j = 0; j < APopulation->GetCount(); ++j)
		   if (APopulation->GetData(i)->Dominates(APopulation->GetData(j))) APopulation->GetData(i)->FStrength++;
 }

 // - obliczenie czêœci R funkcji dopasowania dla rozwi¹zañ w archiwum i populacji
 for (int i = 0; i < AArchive->GetCount(); ++i) {
	  AArchive->GetData(i)->FFitnessValue = 0.0;
	  for (int j = 0; j < AArchive->GetCount(); ++j)
		   if (AArchive->GetData(j)->Dominates(AArchive->GetData(i))) AArchive->GetData(i)->FFitnessValue += AArchive->GetData(j)->FStrength;
	  for (int j = 0; j < APopulation->GetCount(); ++j)
		   if (APopulation->GetData(j)->Dominates(AArchive->GetData(i))) AArchive->GetData(i)->FFitnessValue += APopulation->GetData(j)->FStrength;
 }
 for (int i = 0; i < APopulation->GetCount(); ++i) {
	  APopulation->GetData(i)->FFitnessValue = 0.0;
	  for (int j = 0; j < AArchive->GetCount(); ++j)
		   if (AArchive->GetData(j)->Dominates(APopulation->GetData(i))) APopulation->GetData(i)->FFitnessValue += AArchive->GetData(j)->FStrength;
	  for (int j = 0; j < APopulation->GetCount(); ++j)
		   if (APopulation->GetData(j)->Dominates(APopulation->GetData(i))) APopulation->GetData(i)->FFitnessValue += APopulation->GetData(j)->FStrength;
 }

 // - obliczenie czêœci D funkcji dopasowania dla rozwi¹zañ w archiwum i populacji
 for (int i = 0; i < AArchive->GetCount(); ++i) {
	  AArchive->GetData(i)->FDistance = DistanceToKNearestNeighbour(AArchive->GetData(i), AArchive, APopulation);
	  AArchive->GetData(i)->FFitnessValue += 1.0 / (AArchive->GetData(i)->FDistance + 2.0);
 }
 for (int i = 0; i < APopulation->GetCount(); ++i) {
	  APopulation->GetData(i)->FDistance = DistanceToKNearestNeighbour(APopulation->GetData(i), AArchive, APopulation);
	  APopulation->GetData(i)->FFitnessValue += 1.0 / (APopulation->GetData(i)->FDistance + 2.0);
 }
}
//---------------------------------------------------------------------------
// Selekcja rozwi¹zañ z poprzedniego archiwum i z populacji do nowego archiwum
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA2::CopyBestSolutionsToArchive(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* AArchive,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 // - utworzenie nowego archiwum
 TQuickList<TAIEMultiobjectiveGeneticSolution*> NArchive;

 // - dodanie do nowego archiwum wszystkich niezdominowanych rozwi¹zañ
 //   z poprzedniego archiwum i z populacji (których dopasowanie jest mniejse od 1)
 for (int i = 0; i < AArchive->GetCount(); ++i) if (AArchive->GetData(i)->FFitnessValue < 1.0) NArchive.Add(AArchive->GetData(i));
 for (int i = 0; i < APopulation->GetCount(); ++i) if (APopulation->GetData(i)->FFitnessValue < 1.0) NArchive.Add(APopulation->GetData(i));

 // - wyrównywanie rozmiaru archiwum
 if (NArchive.GetCount() < FFrontMaxSize) {
	 // -- je¿eli archiwum jest zbyt ma³e, to uzupe³nianie najlepszymi
	 //    rozwi¹zaniami zdominowanymi z populacji i z poprzedniego archiwum
	 TQuickList<TAIEMultiobjectiveGeneticSolution*> LSolutions;
	 for (int i = 0; i < AArchive->GetCount(); ++i) if (AArchive->GetData(i)->FFitnessValue >= 1.0) LSolutions.Add(AArchive->GetData(i));
	 for (int i = 0; i < APopulation->GetCount(); ++i) if (APopulation->GetData(i)->FFitnessValue >= 1.0) LSolutions.Add(APopulation->GetData(i));
	 LSolutions.Sort(COMPARE_FITNESS);
	 for (int i = 0; (i < LSolutions.GetCount()) && (NArchive.GetCount() < FFrontMaxSize); ++i) NArchive.Add(LSolutions.GetData(i));
 } else
 while(NArchive.GetCount() > FFrontMaxSize) {
	 // -- je¿eli archiwum jest zbyt du¿e, to usuwanie takich rozwi¹zañ,
	 //    które s¹ po³o¿one najbli¿ej pozosta³ych (w przestrzeni wartoœci funkcji kryteriów)

	 // --- wybranie pary rozwi¹añ po³o¿onych najbli¿ej siebie
	 double LMinDistance = MAX_REAL_VALUE; int i_min = -1; int j_min = -1;
	 for (int i = 0; i < NArchive.GetCount(); ++i)
		  for (int j = i+1; j < NArchive.GetCount(); ++j) {
			   double LDistance = DistanceBetweenSolutions(NArchive.GetData(i), NArchive.GetData(j));
			   if (LMinDistance > LDistance) {LMinDistance = LDistance; i_min = i; j_min = j;}
		  }

	 // --- wybranie spoœród pary rozwi¹zañ takiego, które jest po³o¿one
	 //     najbli¿ej pozosta³ych rozwi¹zañ
	 double LMinDistance1 = MAX_REAL_VALUE;
	 double LMinDistance2 = MAX_REAL_VALUE;
	 for (int i = 0; i < NArchive.GetCount(); ++i)
		  if ((i != i_min) && (i != j_min)) {
			   double LDistance1 = DistanceBetweenSolutions(NArchive.GetData(i_min), NArchive.GetData(i));
			   double LDistance2 = DistanceBetweenSolutions(NArchive.GetData(j_min), NArchive.GetData(i));
			   if (LMinDistance1 > LDistance1) LMinDistance1 = LDistance1;
			   if (LMinDistance2 > LDistance2) LMinDistance2 = LDistance2;
		  }

	 // --- usuwanie rozwi¹zania z archiwum
	 if (LMinDistance2 > LMinDistance1) NArchive.Delete(i_min); else NArchive.Delete(j_min);
 }

 // - podmiana poprzedniego archiwum na nowe
 for (int i = 0; i < NArchive.GetCount(); ++i) NArchive.SetData(i, SolutionClone(NArchive.GetData(i)));
 for (int i = 0; i < AArchive->GetCount(); ++i) SolutionDelete(AArchive->GetData(i));
 AArchive->Clear();
 for (int i = 0; i < NArchive.GetCount(); ++i) AArchive->Add(NArchive.GetData(i));
}
//---------------------------------------------------------------------------
// Obliczanie odleg³oœci od danego rozwi¹zania do jego k-najbli¿szego s¹siada
// w archiwum i w populacji
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA2::DistanceToKNearestNeighbour(
	TAIEMultiobjectiveGeneticSolution* ASolution,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* AArchive,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 int k = (int)SQRT((double)(AArchive->GetCount() + APopulation->GetCount()));

 TQuickList<double> LDistances;
 for (int i = 0; i < AArchive->GetCount(); ++i)
	  if (AArchive->GetData(i) != ASolution) LDistances.Add(DistanceBetweenSolutions(ASolution, AArchive->GetData(i)));
 for (int i = 0; i < APopulation->GetCount(); ++i)
	  if (APopulation->GetData(i) != ASolution) LDistances.Add(DistanceBetweenSolutions(ASolution, APopulation->GetData(i)));
 LDistances.Sort(COMPARE_DOUBLE);
 return LDistances.GetData(k);
}
//---------------------------------------------------------------------------

