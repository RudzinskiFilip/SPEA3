//---------------------------------------------------------------------------
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#pragma hdrstop

#include "UAIEMOGABase.h"
#include "UAIERandomPermutation.h"
//---------------------------------------------------------------------------
__int64 GID = 0;
//---------------------------------------------------------------------------
// TAIEMultiobjectiveGeneticSolution
//---------------------------------------------------------------------------
__stdcall TAIEMultiobjectiveGeneticSolution::TAIEMultiobjectiveGeneticSolution(TAIEMultiobjectiveGeneticAlgorithm* AOwner, int AObjectiveCount)
{
 FOwner = AOwner;

 FObjectiveValues.Resize(AObjectiveCount);
 FRank = 0;
 FFitnessValue = 0;
 FDistance = 0;
 FData = NULL;
}
//---------------------------------------------------------------------------
__stdcall TAIEMultiobjectiveGeneticSolution::TAIEMultiobjectiveGeneticSolution(TAIEMultiobjectiveGeneticSolution& ASource)
{
 FOwner = ASource.FOwner;

 FObjectiveValues = ASource.FObjectiveValues;
 FFrontIndex = ASource.FFrontIndex;
 FFitnessValue = ASource.FFitnessValue;
 FRank = ASource.FRank;
 FDistance = ASource.FDistance;
 FData = NULL;

 for (int i = 0; i < ASource.FDominatingSolutions.GetCount(); ++i) FDominatingSolutions.Add(ASource.FDominatingSolutions.GetData(i));
 for (int i = 0; i < ASource.FDominatedSolutions.GetCount(); ++i) FDominatedSolutions.Add(ASource.FDominatedSolutions.GetData(i));
}
//---------------------------------------------------------------------------
__stdcall TAIEMultiobjectiveGeneticSolution::~TAIEMultiobjectiveGeneticSolution(void)
{
}
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticSolution::GetObjectiveValue(int AObjectiveIndex)
{
 return FObjectiveValues[AObjectiveIndex];
}
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticSolution::GetFitnessValue(void)
{
 return FFitnessValue;
}
//---------------------------------------------------------------------------
int    __stdcall TAIEMultiobjectiveGeneticSolution::GetFrontIndex(void)
{
 return FFrontIndex;
}
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticSolution::GetDistance(void)
{
 return FDistance;
}
//---------------------------------------------------------------------------
int	   __stdcall TAIEMultiobjectiveGeneticSolution::GetRank(void)
{
 return FRank;
}
//---------------------------------------------------------------------------
void* __stdcall TAIEMultiobjectiveGeneticSolution::GetData(void)
{
 return FData;
}
//---------------------------------------------------------------------------
int    __stdcall TAIEMultiobjectiveGeneticSolution::GetDominatingSolutionCount(void)
{
 return FDominatingSolutions.GetCount();
}
//---------------------------------------------------------------------------
TAIEMultiobjectiveGeneticSolution* __stdcall TAIEMultiobjectiveGeneticSolution::GetDominatingSolution(int ASolutionIndex)
{
 return FDominatingSolutions.GetData(ASolutionIndex);
}
//---------------------------------------------------------------------------
int    __stdcall TAIEMultiobjectiveGeneticSolution::GetDominatedSolutionCount(void)
{
 return FDominatedSolutions.GetCount();
}
//---------------------------------------------------------------------------
TAIEMultiobjectiveGeneticSolution* __stdcall TAIEMultiobjectiveGeneticSolution::GetDominatedSolution(int ASolutionIndex)
{
 return FDominatedSolutions.GetData(ASolutionIndex);
}
//---------------------------------------------------------------------------
// Zwraca true je¿eli dane rozwi¹zanie dominuje ASolution (dominating)
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticSolution::Dominates(TAIEMultiobjectiveGeneticSolution* ASolution)
{
 bool IsNotEqual = false;
 for (int i = 0; i < FObjectiveValues.GetCount(); ++i) {
	  if (FObjectiveValues[i] > ASolution->FObjectiveValues[i]) return false;
	  IsNotEqual = IsNotEqual || (FObjectiveValues[i] < ASolution->FObjectiveValues[i]);
 }
 return IsNotEqual;
}
//---------------------------------------------------------------------------
// Zwraca true je¿eli dane rozwi¹zanie dominuje ASolution lub jest mu równowa¿ne (dominating and covering)
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticSolution::DominatesOrEquals(TAIEMultiobjectiveGeneticSolution* ASolution)
{
 for (int i = 0; i < FObjectiveValues.GetCount(); ++i)
	  if (FObjectiveValues[i] > ASolution->FObjectiveValues[i]) return false;
 return true;
}
//---------------------------------------------------------------------------
// Zwraca true je¿eli dane rozwi¹zanie jest równowa¿ne rozwi¹zaniu ASolution (covering)
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticSolution::Equals(TAIEMultiobjectiveGeneticSolution* ASolution)
{
 for (int i = 0; i < FObjectiveValues.GetCount(); ++i)
	  if (FObjectiveValues[i] != ASolution->FObjectiveValues[i]) return false;
 return true;
}
//---------------------------------------------------------------------------
// Zwraca true je¿eli dane rozwi¹zanie e-dominuje ASolution (e-dominating)
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticSolution::EpsilonDominates(TAIEMultiobjectiveGeneticSolution* ASolution)
{
 bool IsNotEqual = false;
 for (int i = 0; i < FObjectiveValues.GetCount(); ++i) {
	  double LValue =  - FOwner->GetObjectiveEpsilon(i, FObjectiveValues[i]);
	  if (LValue > ASolution->FObjectiveValues[i]) return false;
	  IsNotEqual = IsNotEqual || (LValue < ASolution->FObjectiveValues[i]);
 }
 return IsNotEqual;
}
//---------------------------------------------------------------------------
// Zwraca true je¿eli dane rozwi¹zanie e-dominuje ASolution lub jest mu równowa¿ne (e-dominating and covering)
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticSolution::EpsilonDominatesOrEquals(TAIEMultiobjectiveGeneticSolution* ASolution)
{
 for (int i = 0; i < FObjectiveValues.GetCount(); ++i)
	  if (FObjectiveValues[i] - FOwner->GetObjectiveEpsilon(i, FObjectiveValues[i]) > ASolution->FObjectiveValues[i]) return false;
 return true;
}
//---------------------------------------------------------------------------
// Zwraca true je¿eli dane rozwi¹zanie jest zdominowane przez co najmniej jedno rozwi¹zanie w populacji
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticSolution::IsDominated(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 for (int i = 0; i < APopulation->GetCount(); ++i) if (APopulation->GetData(i)->Dominates(this)) return true;
 return false;
}
//---------------------------------------------------------------------------
// Zwraca true je¿eli dane rozwi¹zanie jest zdominowane lub równowa¿ne co najmniej jednemu rozwi¹zaniu w populacji
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticSolution::IsDominatedOrEqual(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 for (int i = 0; i < APopulation->GetCount(); ++i) if (APopulation->GetData(i)->DominatesOrEquals(this)) return true;
 return false;
}
//---------------------------------------------------------------------------
// Zwraca true je¿eli dane rozwi¹zanie jest e-zdominowane przez co najmniej jedno rozwi¹zanie w populacji
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticSolution::IsEpsilonDominated(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 for (int i = 0; i < APopulation->GetCount(); ++i) if (APopulation->GetData(i)->EpsilonDominates(this)) return true;
 return false;
}
//---------------------------------------------------------------------------
// Zwraca true je¿eli dane rozwi¹zanie jest e-zdominowane lub równowa¿ne co najmniej jednemu rozwi¹zaniu w populacji
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticSolution::IsEpsilonDominatedOrEqual(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 for (int i = 0; i < APopulation->GetCount(); ++i) if (APopulation->GetData(i)->EpsilonDominatesOrEquals(this)) return true;
 return false;
}
//---------------------------------------------------------------------------
// TAIEMultiobjectiveGeneticAlgorithm
//---------------------------------------------------------------------------
__stdcall TAIEMultiobjectiveGeneticAlgorithm::TAIEMultiobjectiveGeneticAlgorithm(void)
{
 FCrossoverProbability = 0.7;
 FMutationProbability = 0.1;
 FObjectiveCount = 2;
 FPopulationSize = 100;
 FSelectionPressure = 2;
 FGenerationIndex = 0;

 FIsPrepared = false;
 FIsExecuted = false;
}
//---------------------------------------------------------------------------
__stdcall TAIEMultiobjectiveGeneticAlgorithm::~TAIEMultiobjectiveGeneticAlgorithm(void)
{
 Clear();
}
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticAlgorithm::Prepare(int Seed)
{
 if (FIsPrepared) return false;

 if (!FSolutionInit) return false;
 if (!FSolutionRand) return false;
 if (!FSolutionCopy) return false;
 if (!FSolutionFree) return false;
 if (!FCrossoverOperator) return false;
 if (!FMutationOperator) return false;
 if (!FFitnessFunction) return false;
 srand(Seed);

 FPopulation.Resize(FPopulationSize);
 for (int i = 0; i < FPopulationSize; ++i) FPopulation.SetData(i, SolutionNew());

 FObjectiveMaxValues.Resize(FObjectiveCount);
 FObjectiveAvgValues.Resize(FObjectiveCount);
 FObjectiveMinValues.Resize(FObjectiveCount);
 FFinalObjectiveMaxValues.Resize(FObjectiveCount);
 FFinalObjectiveAvgValues.Resize(FObjectiveCount);
 FFinalObjectiveMinValues.Resize(FObjectiveCount);

 FGenerationIndex = 0;
 FIsPrepared = true;
 return true;
}
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticAlgorithm::Clear(void)
{
 if (!FIsPrepared) return false;

 for (int i = 0; i < FPopulation.GetCount(); ++i) SolutionDelete(FPopulation.GetData(i));
 for (int i = 0; i < FFronts.GetCount(); ++i) delete FFronts.GetData(i);
 FFronts.Clear();
 FPopulation.Clear();
 FObjectiveMaxValues.Resize(0);
 FObjectiveAvgValues.Resize(0);
 FObjectiveMinValues.Resize(0);
 FFinalObjectiveMaxValues.Resize(0);
 FFinalObjectiveAvgValues.Resize(0);
 FFinalObjectiveMinValues.Resize(0);

 FSolutionInit = NULL;
 FSolutionRand = NULL;
 FSolutionCopy = NULL;
 FSolutionFree = NULL;
 FCrossoverOperator = NULL;
 FMutationOperator = NULL;
 FFitnessFunction = NULL;

 FGenerationIndex = 0;
 FIsExecuted = false;
 FIsPrepared = false;
 return true;
}
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticAlgorithm::FirstGeneration(void)
{
 if (!FIsPrepared || FIsExecuted) return false;
 FIsExecuted = true;
 ++FGenerationIndex;

 RandomizeSolutions(&FPopulation);
 EvaluateSolutions(&FPopulation);

 ExecuteFirstGeneration();

 FastNondominatedSort(&FPopulation, &FFronts);
 EvaluateMinMaxAvg();
 return true;
}
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticAlgorithm::NextGeneration(void)
{
 if (!FIsPrepared || !FIsExecuted) return false;
 ++FGenerationIndex;

 ExecuteNextGeneration();

 FastNondominatedSort(&FPopulation, &FFronts);
 EvaluateMinMaxAvg();
 return true;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::ExecuteFirstGeneration(void)
{
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::ExecuteNextGeneration(void)
{
}
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetIsPrepared(void)
{
 return FIsPrepared;
}
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetIsExecuted(void)
{
 return FIsExecuted;
}
//---------------------------------------------------------------------------
int __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetSelectionMethod(void)
{
 return 0;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetSelectionMethod(int ASelectionMethod)
{
 //nie u¿ywane
}
//---------------------------------------------------------------------------
int __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetSelectionPressure(void)
{
 return FSelectionPressure;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetSelectionPressure(int ASelectionPressure)
{
 if (!FIsPrepared) FSelectionPressure = ASelectionPressure;
}
//---------------------------------------------------------------------------
int __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetPopulationSize(void)
{
 return FPopulationSize;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetPopulationSize(int APopulationSize)
{
 if (!FIsPrepared) FPopulationSize = APopulationSize;
}
//---------------------------------------------------------------------------
int __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetFrontMaxSize(void)
{
 return 0;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetFrontMaxSize(int AFrontMaxSize)
{
}
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetCrossoverProbability(void)
{
 return FCrossoverProbability;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetCrossoverProbability(double ACrossoverProbability)
{
 if (!FIsPrepared) FCrossoverProbability = ACrossoverProbability;
}
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetMutationProbability(void)
{
 return FMutationProbability;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetMutationProbability(double AMutationProbability)
{
 if (!FIsPrepared) FMutationProbability = AMutationProbability;
}
//---------------------------------------------------------------------------
int __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetSolutionCount(void)
{
 return FPopulation.GetCount();
}
//---------------------------------------------------------------------------
IAIEMultiobjectiveGeneticSolution* __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetSolution(int ASolutionIndex)
{
 return FPopulation.GetData(ASolutionIndex);
}
//---------------------------------------------------------------------------
int __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetFinalSolutionCount(void)
{
 return FFronts.GetData(0)->GetCount();
}
//---------------------------------------------------------------------------
IAIEMultiobjectiveGeneticSolution* __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetFinalSolution(int ASolutionIndex)
{
 return FFronts.GetData(0)->GetData(ASolutionIndex);
}
//---------------------------------------------------------------------------
int __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetFrontCount(void)
{
 return FFronts.GetCount();
}
//---------------------------------------------------------------------------
int __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetFrontSolutionCount(int AFrontIndex)
{
 return FFronts.GetData(AFrontIndex)->GetCount();
}
//---------------------------------------------------------------------------
IAIEMultiobjectiveGeneticSolution* __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetFrontSolution(int AFrontIndex, int ASolutionIndex)
{
 return FFronts.GetData(AFrontIndex)->GetData(ASolutionIndex);
}
//---------------------------------------------------------------------------
int __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetObjectiveCount(void)
{
 return FObjectiveCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetObjectiveCount(int AObjectiveCount)
{
 if (!FIsPrepared) FObjectiveCount = AObjectiveCount;
}
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetObjectiveMaxValue(int AObjectiveIndex)
{
 return FObjectiveMaxValues.GetData(AObjectiveIndex);
}
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetObjectiveAvgValue(int AObjectiveIndex)
{
 return FObjectiveAvgValues.GetData(AObjectiveIndex);
}
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetObjectiveMinValue(int AObjectiveIndex)
{
 return FObjectiveMinValues.GetData(AObjectiveIndex);
}
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetObjectiveEpsilon(int AObjectiveIndex, double AValue)
{
 // - w ogólnym przypadku operator e-dominowania jest równowa¿ny klasycznemu operatorowi dominowania
 return 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetFinalObjectiveMaxValue(int AObjectiveIndex)
{
 return FFinalObjectiveMaxValues.GetData(AObjectiveIndex);
}
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetFinalObjectiveAvgValue(int AObjectiveIndex)
{
 return FFinalObjectiveAvgValues.GetData(AObjectiveIndex);
}
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetFinalObjectiveMinValue(int AObjectiveIndex)
{
 return FFinalObjectiveMinValues.GetData(AObjectiveIndex);
}
//---------------------------------------------------------------------------
int __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetGenerationIndex(void)
{
 return FGenerationIndex;
}
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetDiversityFactor(void)
{
 // - wyznaczenie wartoœci wspó³czynnika zró¿nicowania frontu rozwi¹zañ
 //   (wariancja odleg³oœci do najbli¿szego s¹siada dla wszystkich rozwi¹zañ)
 if (GetFinalSolutionCount() == 1) return 0.0;

 double Avg = 0.0;
 double Div = 0.0;
 for (int i = 0; i < GetFinalSolutionCount(); ++i) {
	  // -- wyznaczanie najblizszego s¹siada w archiwum
	  int k = -1;
	  double DMin = MAX_REAL_VALUE;
	  for (int l = 0; l < GetFinalSolutionCount(); ++l) {
		   if (l == i) continue;
		   double D = NormalizedDistanceBetweenSolutions(dynamic_cast<TAIEMultiobjectiveGeneticSolution*>(GetFinalSolution(i)),
														 dynamic_cast<TAIEMultiobjectiveGeneticSolution*>(GetFinalSolution(l)),
														 &FFinalObjectiveMinValues, &FFinalObjectiveMaxValues);
		   if (DMin > D) {DMin = D; k = l;}
	  }
	  Avg += DMin;
	  Div += SQR(DMin);
 }
 Avg /= GetFinalSolutionCount();
 Div /= GetFinalSolutionCount();
 return Div - SQR(Avg);
}
//---------------------------------------------------------------------------
void* __stdcall TAIEMultiobjectiveGeneticAlgorithm::GetData(void)
{
 return FData;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetData(void* AData)
{
 FData = AData;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetSolutionInit(TAIEMOGASolutionInit ASolutionInit)
{
 if (!FIsPrepared) FSolutionInit = ASolutionInit;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetSolutionRand(TAIEMOGASolutionRand ASolutionRand)
{
 if (!FIsPrepared) FSolutionRand = ASolutionRand;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetSolutionCopy(TAIEMOGASolutionCopy ASolutionCopy)
{
 if (!FIsPrepared) FSolutionCopy = ASolutionCopy;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetSolutionFree(TAIEMOGASolutionFree ASolutionFree)
{
 if (!FIsPrepared) FSolutionFree = ASolutionFree;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetCrossoverOperator(TAIEMOGACrossoverOperator ACrossoverOperator)
{
 if (!FIsPrepared) FCrossoverOperator = ACrossoverOperator;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetMutationOperator(TAIEMOGAMutationOperator AMutationOperator)
{
 if (!FIsPrepared) FMutationOperator = AMutationOperator;
}
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SetFitnessFunction(TAIEMOGAFitnessFunction AFitnessFunction)
{
 if (!FIsPrepared) FFitnessFunction = AFitnessFunction;
}
//---------------------------------------------------------------------------
// Ocena rozwi¹zañ w populacji - obliczanie wartoœci funkcji kryteriów
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::EvaluateSolutions(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 for (int i = 0; i < APopulation->GetCount(); ++i)
	  FFitnessFunction(this, APopulation->GetData(i)->FData, FObjectiveCount, APopulation->GetData(i)->FObjectiveValues.GetData());
}
//---------------------------------------------------------------------------
// Losowanie rozwi¹zañ w populacji
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::RandomizeSolutions(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 for (int i = 0; i < APopulation->GetCount(); ++i) FSolutionRand(this, APopulation->GetData(i)->FData);
}
//---------------------------------------------------------------------------
// Kopiowanie wszystkich rozwi¹zañ z populacji Ÿród³owej do docelowej
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::CopySolutions(
	TQuickList< TAIEMultiobjectiveGeneticSolution* >* ADstPopulation,
	TQuickList< TAIEMultiobjectiveGeneticSolution* >* ASrcPopulation)
{
 for (int i = 0; i < ASrcPopulation->GetCount(); ++i) ADstPopulation->Add(SolutionClone(ASrcPopulation->GetData(i)));
}
//---------------------------------------------------------------------------
// Kopiowanie rozwi¹zañ niezdominowanych z populacji Ÿród³owej do docelowej
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::CopyNondominatedSolutions(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* ADstPopulation,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* ASrcPopulation)
{
 for (int i = 0; i < ASrcPopulation->GetCount(); ++i)
	  if (!ASrcPopulation->GetData(i)->IsDominated(ASrcPopulation)) ADstPopulation->Add(SolutionClone(ASrcPopulation->GetData(i)));
}
//---------------------------------------------------------------------------
// Czyszczenie populacji
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::ClearSolutions(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 for (int i = 0; i < APopulation->GetCount(); ++i) SolutionDelete(APopulation->GetData(i));
 APopulation->Clear();
}
//---------------------------------------------------------------------------
// Usuwanie wszystkich zdominowanych rozwi¹zañ z archiwum
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::ClearDominatedSolutions(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 for (int i = 0; i < APopulation->GetCount(); ++i)
	  for (int j = i+1; j < APopulation->GetCount(); ++j)
		   if (APopulation->GetData(i)->Dominates(APopulation->GetData(j))) {
			   SolutionDelete(APopulation->GetData(j));
			   APopulation->Delete(j--);
		   } else
		   if (APopulation->GetData(j)->Dominates(APopulation->GetData(i))) {
			   SolutionDelete(APopulation->GetData(i));
			   APopulation->Delete(i--);
			   break;
		   }
}
//---------------------------------------------------------------------------
// Usuwanie wszystkich zdominowanych i zdublowanych rozwi¹zañ z archiwum
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::ClearDominatedOrEqualSolutions(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 for (int i = 0; i < APopulation->GetCount(); ++i)
	  for (int j = i+1; j < APopulation->GetCount(); ++j)
		   if (APopulation->GetData(i)->DominatesOrEquals(APopulation->GetData(j))) {
			   SolutionDelete(APopulation->GetData(j));
			   APopulation->Delete(j--);
		   } else
		   if (APopulation->GetData(j)->DominatesOrEquals(APopulation->GetData(i))) {
			   SolutionDelete(APopulation->GetData(i));
			   APopulation->Delete(i--);
			   break;
		   }
}
//---------------------------------------------------------------------------
// Usuwanie wszystkich zdominowanych rozwi¹zañ z archiwum
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::ClearEpsilonDominatedSolutions(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 for (int i = 0; i < APopulation->GetCount(); ++i)
	  for (int j = i+1; j < APopulation->GetCount(); ++j)
		   if (APopulation->GetData(i)->EpsilonDominates(APopulation->GetData(j))) {
			   SolutionDelete(APopulation->GetData(j));
			   APopulation->Delete(j--);
		   } else
		   if (APopulation->GetData(j)->EpsilonDominates(APopulation->GetData(i))) {
			   SolutionDelete(APopulation->GetData(i));
			   APopulation->Delete(i--);
			   break;
		   }
}
//---------------------------------------------------------------------------
// Usuwanie wszystkich zdominowanych i zdublowanych rozwi¹zañ z archiwum
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::ClearEpsilonDominatedOrEqualSolutions(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 for (int i = 0; i < APopulation->GetCount(); ++i)
	  for (int j = i+1; j < APopulation->GetCount(); ++j)
		   if (APopulation->GetData(i)->EpsilonDominatesOrEquals(APopulation->GetData(j))) {
			   SolutionDelete(APopulation->GetData(j));
			   APopulation->Delete(j--);
		   } else
		   if (APopulation->GetData(j)->EpsilonDominatesOrEquals(APopulation->GetData(i))) {
			   SolutionDelete(APopulation->GetData(i));
			   APopulation->Delete(i--);
			   break;
		   }
}
//---------------------------------------------------------------------------
// Operacja krzy¿owania rozwi¹zañ w populacji
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::CrossSolutions(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 TAIERandomPermutation RP(APopulation->GetCount());

 int LCount = APopulation->GetCount() / 2;
 for (int i = 0; i < LCount; ++i) {
	  int I1 = RP.Random();
	  int I2 = RP.Random();
	  if (FCrossoverProbability > (double)rand() / RAND_MAX) FCrossoverOperator(this, APopulation->GetData(I1)->FData, APopulation->GetData(I2)->FData);
 }
}
//---------------------------------------------------------------------------
// Operacja mutacji rozwi¹zañ w populacji
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::MutateSolutions(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 for (int i = 0; i < APopulation->GetCount(); ++i)
	  if (FMutationProbability > (double)rand() / RAND_MAX) FMutationOperator(this, APopulation->GetData(i)->FData);
}
//---------------------------------------------------------------------------
// Utworzenie nowego rozwi¹zania
//---------------------------------------------------------------------------
TAIEMultiobjectiveGeneticSolution* __stdcall TAIEMultiobjectiveGeneticAlgorithm::SolutionNew(void)
{
 TAIEMultiobjectiveGeneticSolution* NSolution = new TAIEMultiobjectiveGeneticSolution(this, FObjectiveCount);
 FSolutionInit(this, NSolution->FData);
 return NSolution;
}
//---------------------------------------------------------------------------
// Klonowanie rozwi¹zania
//---------------------------------------------------------------------------
TAIEMultiobjectiveGeneticSolution* __stdcall TAIEMultiobjectiveGeneticAlgorithm::SolutionClone(TAIEMultiobjectiveGeneticSolution* ASolution)
{
 TAIEMultiobjectiveGeneticSolution* NSolution = new TAIEMultiobjectiveGeneticSolution(*ASolution);
 FSolutionInit(this, NSolution->FData);
 FSolutionCopy(this, NSolution->FData, ASolution->FData);
 return NSolution;
}
//---------------------------------------------------------------------------
// Usuwanie rozwi¹zania
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::SolutionDelete(TAIEMultiobjectiveGeneticSolution* ASolution)
{
 FSolutionFree(this, ASolution->FData);
 delete ASolution;
}
//---------------------------------------------------------------------------
// Obliczanie odleg³oœci pomiêdzy rozwi¹zaniami
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticAlgorithm::DistanceBetweenSolutions(
	TAIEMultiobjectiveGeneticSolution* ASolution1,
	TAIEMultiobjectiveGeneticSolution* ASolution2)
{
 double LDistance = 0.0;
 for (int i = 0; i < ASolution1->FObjectiveValues.GetCount(); ++i)
	  LDistance += SQR(ASolution1->FObjectiveValues[i] - ASolution2->FObjectiveValues[i]);
 return SQRT(LDistance) / ASolution1->FObjectiveValues.GetCount();
}
//---------------------------------------------------------------------------
// Obliczanie odleg³oœci pomiêdzy rozwi¹zaniami
//---------------------------------------------------------------------------
double __stdcall TAIEMultiobjectiveGeneticAlgorithm::NormalizedDistanceBetweenSolutions(
	TAIEMultiobjectiveGeneticSolution* ASolution1,
	TAIEMultiobjectiveGeneticSolution* ASolution2,
	TQuickList<double>* AObjectiveMaxValues,
	TQuickList<double>* AObjectiveMinValues)
{
 double LDistance = 0.0;
 for (int i = 0; i < ASolution1->FObjectiveValues.GetCount(); ++i) {
	  double d = AObjectiveMaxValues->GetData(i) - AObjectiveMinValues->GetData(i);
	  if (d == 0.0) return 0.0;
	  double x1 = (ASolution1->FObjectiveValues[i] - AObjectiveMinValues->GetData(i)) / d;
	  double x2 = (ASolution2->FObjectiveValues[i] - AObjectiveMinValues->GetData(i)) / d;
	  LDistance += SQR(x1 - x2);
	  //if (LDistance < ABS(x1 - x2)) LDistance = ABS(x1 - x2);
 }
 return SQRT(LDistance) / ASolution1->FObjectiveValues.GetCount();
 //return LDistance;
}
//---------------------------------------------------------------------------
// Obliczanie min max i œrednich wartoœci funkcji kryteriów
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::EvaluateMinMaxAvg(void)
{
 for (int j = 0; j < FObjectiveCount; ++j) {
	  FObjectiveMaxValues.SetData(j, MIN_REAL_VALUE);
	  FObjectiveAvgValues.SetData(j, 0.0);
	  FObjectiveMinValues.SetData(j, MAX_REAL_VALUE);

	  FFinalObjectiveMaxValues.SetData(j, MIN_REAL_VALUE);
	  FFinalObjectiveAvgValues.SetData(j, 0.0);
	  FFinalObjectiveMinValues.SetData(j, MAX_REAL_VALUE);
 }

 for (int i = 0; i < FPopulation.GetCount(); ++i) {
	  for (int j = 0; j < FObjectiveCount; ++j) {
		   if (FObjectiveMaxValues.GetData(j) < FPopulation.GetData(i)->FObjectiveValues[j]) FObjectiveMaxValues.SetData(j, FPopulation.GetData(i)->FObjectiveValues[j]);
		   FObjectiveAvgValues.SetData(j, FObjectiveAvgValues.GetData(j) + FPopulation.GetData(i)->FObjectiveValues[j]);
		   if (FObjectiveMinValues.GetData(j) > FPopulation.GetData(i)->FObjectiveValues[j]) FObjectiveMinValues.SetData(j, FPopulation.GetData(i)->FObjectiveValues[j]);
	  }
 }
 for (int i = 0; i < GetFinalSolutionCount(); ++i) {
	  for (int j = 0; j < FObjectiveCount; ++j) {
		   double Value = dynamic_cast<TAIEMultiobjectiveGeneticSolution*>(GetFinalSolution(i))->FObjectiveValues[j];
		   if (FFinalObjectiveMaxValues.GetData(j) < Value) FFinalObjectiveMaxValues.SetData(j, Value);
		   FFinalObjectiveAvgValues.SetData(j, FFinalObjectiveAvgValues.GetData(j) + Value);
		   if (FFinalObjectiveMinValues.GetData(j) > Value) FFinalObjectiveMinValues.SetData(j, Value);
	  }
 }

 for (int j = 0; j < FObjectiveCount; ++j) {
	  FObjectiveAvgValues.SetData(j, FObjectiveAvgValues.GetData(j) / FPopulation.GetCount());
	  FFinalObjectiveAvgValues.SetData(j, FFinalObjectiveAvgValues.GetData(j) / GetFinalSolutionCount());
 }
}
//---------------------------------------------------------------------------
// Tworzenie kolejnych frontów rozwi¹zañ (pierwszy front zawiera rozwi¹zania niezdominowane)
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithm::FastNondominatedSort(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation,
	TQuickList< TQuickList<TAIEMultiobjectiveGeneticSolution*>* >* AFront)
{
 for (int i = 0; i < AFront->GetCount(); ++i) delete AFront->GetData(i); AFront->Clear();

 TQuickList<TAIEMultiobjectiveGeneticSolution*>* F = new TQuickList<TAIEMultiobjectiveGeneticSolution*>();
 AFront->Add(F);

 for (int p = 0; p < APopulation->GetCount(); ++p) {
	  TAIEMultiobjectiveGeneticSolution* P = APopulation->GetData(p);
	  P->FDominatingSolutions.Clear();
	  P->FDominatedSolutions.Clear();
	  P->FFrontIndex = 0;
	  for (int q = 0; q < APopulation->GetCount(); ++q) {
		   TAIEMultiobjectiveGeneticSolution* Q = APopulation->GetData(q);
		   if (P->Dominates(Q)) {
			   P->FDominatedSolutions.Add(Q);
			   Q->FDominatingSolutions.Add(P);
		   } else
		   if (Q->Dominates(P)) {
			   Q->FDominatedSolutions.Add(P);
			   P->FDominatingSolutions.Add(Q);
			   P->FFrontIndex++;
		   }
	  }

	  if (P->FDominatingSolutions.GetCount() == 0) {
		   P->FRank = 1;
		   F->Add(P);
	  }
 }

 int LFrontIndex = 1;
 while (F->GetCount()) {
		TQuickList<TAIEMultiobjectiveGeneticSolution*>* Q = new TQuickList<TAIEMultiobjectiveGeneticSolution*>();
		for (int p = 0; p < F->GetCount(); ++p) {
			 TQuickList<TAIEMultiobjectiveGeneticSolution*>* S = &(F->GetData(p)->FDominatedSolutions);
			 for (int q = 0; q < S->GetCount(); ++q) {
				  if (--(S->GetData(q)->FFrontIndex) == 0) {
					  S->GetData(q)->FRank = LFrontIndex + 1;
					  Q->Add(S->GetData(q));
				  }
			 }
		}
		++LFrontIndex;
		F = Q;
		AFront->Add(F);
 }

/*
 for (int i = 0; i < AFront->GetCount(); ++i) delete AFront->GetData(i); AFront->Clear();

 TQuickList<TAIEMultiobjectiveGeneticSolution*>* F = new TQuickList<TAIEMultiobjectiveGeneticSolution*>();

 for (int p = 0; p < APopulation->GetCount(); ++p) {
	  TAIEMultiobjectiveGeneticSolution* S = APopulation->GetData(p);
	  S->FDominatingSolutions.Clear();
	  S->FDominatedSolutions.Clear();
 }

 for (int p = 0; p < APopulation->GetCount(); ++p) {
	  for (int q = p+1; q < APopulation->GetCount(); ++q)
		   if (APopulation->GetData(p)->Dominates(APopulation->GetData(q))) {
			   APopulation->GetData(p)->FDominatedSolutions.Add(APopulation->GetData(q));
			   APopulation->GetData(q)->FDominatingSolutions.Add(APopulation->GetData(p));
		   } else
		   if (APopulation->GetData(q)->Dominates(APopulation->GetData(p))) {
			   APopulation->GetData(q)->FDominatedSolutions.Add(APopulation->GetData(p));
			   APopulation->GetData(p)->FDominatingSolutions.Add(APopulation->GetData(q));
		   }
 }

 int LFrontIndex = 0;
 for (int p = 0; p < APopulation->GetCount(); ++p) {
	  APopulation->GetData(p)->FRank = APopulation->GetData(p)->FDominatingSolutions.GetCount();
	  if (APopulation->GetData(p)->FRank == 0) {
		   APopulation->GetData(p)->FFrontIndex = LFrontIndex;
		   F->Add(APopulation->GetData(p));
	  }
 }

 while (F->GetCount()) {
		AFront->Add(F);

		TQuickList<TAIEMultiobjectiveGeneticSolution*>* Q = new TQuickList<TAIEMultiobjectiveGeneticSolution*>();
		++LFrontIndex;
		for (int p = 0; p < F->GetCount(); ++p) {
			 TQuickList<TAIEMultiobjectiveGeneticSolution*>* S = &(F->GetData(p)->FDominatedSolutions);
			 for (int q = 0; q < S->GetCount(); ++q) {
				  if (--(S->GetData(q)->FRank) == 0) {
					  S->GetData(q)->FFrontIndex = LFrontIndex;
					  Q->Add(S->GetData(q));
				  }
			 }
		}
		F = Q;
 }

 delete F;
 */
}
//---------------------------------------------------------------------------

