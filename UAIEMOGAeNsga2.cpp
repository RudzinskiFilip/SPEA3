//---------------------------------------------------------------------------
#include <stdlib.h>
#include <math.h>
#pragma hdrstop

#include "UAIEMOGAeNsga2.h"
#include "UAIERandomPermutation.h"
//---------------------------------------------------------------------------
// TAIEMultiobjectiveGeneticAlgorithmENSGA2
//---------------------------------------------------------------------------
__stdcall TAIEMultiobjectiveGeneticAlgorithmENSGA2::TAIEMultiobjectiveGeneticAlgorithmENSGA2(void)
			: TAIEMultiobjectiveGeneticAlgorithmNSGA2()
{
 FGridSize = 40;
}
//---------------------------------------------------------------------------
__stdcall TAIEMultiobjectiveGeneticAlgorithmENSGA2::~TAIEMultiobjectiveGeneticAlgorithmENSGA2(void)
{
 ClearSolutions(&FArchive);
}
//---------------------------------------------------------------------------
int __stdcall TAIEMultiobjectiveGeneticAlgorithmENSGA2::GetGridSize(void)
{
 return FGridSize;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmENSGA2::SetGridSize(int AGridSize)
{
 if (!GetIsPrepared()) FGridSize = AGridSize;
}
//---------------------------------------------------------------------------
int __stdcall TAIEMultiobjectiveGeneticAlgorithmENSGA2::GetFinalSolutionCount(void)
{
 return FArchive.GetCount();
}
//---------------------------------------------------------------------------
TAIEMultiobjectiveGeneticSolution* __stdcall TAIEMultiobjectiveGeneticAlgorithmENSGA2::GetFinalSolution(int ASolutionIndex)
{
 return FArchive.GetData(ASolutionIndex);
}
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticAlgorithmENSGA2::GetObjectiveEpsilon(int AObjectiveIndex, double AValue)
{
 // Epsilon jest niezale¿ne od AValue
 return (GetFinalObjectiveMaxValue(AObjectiveIndex) - GetFinalObjectiveMinValue(AObjectiveIndex)) / (2 * FGridSize);
 //return (GetObjectiveMaxValue(AObjectiveIndex) - GetObjectiveMinValue(AObjectiveIndex)) / (2 * FGridSize);
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmENSGA2::ExecuteFirstGeneration(void)
{
 // - czyszczenie archiwum
 ClearSolutions(&FArchive);
 // - kopiowanie niezdominowanych rozwi¹zañ do archiwum
 CopyNondominatedSolutions(&FArchive, &FPopulation);
 // - usuwanie zdominowanych rozwi¹zañ z archiwum
 ClearDominatedOrEqualSolutions(&FArchive);
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmENSGA2::ExecuteNextGeneration(void)
{
 TAIEMultiobjectiveGeneticAlgorithmNSGA2::ExecuteNextGeneration();

 // - kopiowanie niezdominowanych rozwi¹zañ do archiwum
 CopyNondominatedSolutions(&FArchive, &FPopulation);
 // - usuwanie zdominowanych rozwi¹zañ z archiwum
 ClearEpsilonDominatedOrEqualSolutions(&FArchive);
}
//---------------------------------------------------------------------------

