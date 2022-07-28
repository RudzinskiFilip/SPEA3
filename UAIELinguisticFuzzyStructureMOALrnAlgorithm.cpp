//---------------------------------------------------------------------------
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "UTemplates.h"
#include "UDynamicPointers.h"
#include "UAIESystem.h"
#include "UAIEBinaryFile.h"
#include "UAIEComputingTimer.h"
#include "UAIEStopConditions.h"
#include "UAIEInitConditions.h"
#include "UAIERandomPermutation.h"
#include "UAIERandomGenerator64.h"

#include "UAIEDataSet.h"
#include "UAIEDataSetRecord.h"
#include "UAIEDataSetColumn.h"
#include "UAIECommonDataSet.h"
#include "UAIECommonDataSetRecord.h"

#include "UAIELinguisticFuzzyStructure.h"
#include "UAIELinguisticFuzzyStructureMOALrnAlgorithm.h"
#include "UAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm.h"
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::TAIELinguisticFuzzyStructureMOALrnAlgorithm(TAIESystem* ASystem)
{
 FSystem = ASystem;
 FLrnDataSet = 0;
 FTstDataSet = 0;
 FInitFuzzyStructure = new TAIELinguisticFuzzyStructure(FSystem, 0, false);
 FNoiseSet = 0;
 FIsNoiseInputs = false;
 FNoiseInputs = 10.0;
 FSequentialLearning = false;
 FRandomOrdering = true;
 FRandomInitializing = true;
 FRandomGeneratorSeedEnabled = true;
 FRandomGeneratorSeedValue = 0;
 FRandomGeneratorRangeEnabled = false;
 FRandomGeneratorRangeMin = -1.0;
 FRandomGeneratorRangeMax = +1.0;
 FAlgorithmType = 0;
 FComputingTimer = new TAIEComputingTimer();
 FNoiseGenerator = new TAIERandomGenerator64();
 FRandomGenerator = new TAIERandomGenerator64();
 FMOGA = new TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm(this, FSystem);

 FLrnStopCondition = new TAIEStopConditions();
 FLrnStopCondition->SetEpochNumber(100);
 FLrnStopCondition->SetDefaultMinError(0.01);
 FLrnStopCondition->SetDefaultTol(0.1);
 FLrnStopCondition->SetDefaultVectorsInTol(100);
 FLrnStopCondition->SetUseDefaultConditions(true);

 FTstStopCondition = new TAIEStopConditions();
 FTstStopCondition->SetEpochNumber(100);
 FTstStopCondition->SetDefaultMinError(0.02);
 FTstStopCondition->SetDefaultTol(0.2);
 FTstStopCondition->SetDefaultVectorsInTol(100);
 FTstStopCondition->SetUseDefaultConditions(true);

 FLrnCurrentErrors = new TAIEStopConditions();
 FLrnCurrentErrors->SetUseDefaultConditions(false);
 FTstCurrentErrors = new TAIEStopConditions();
 FTstCurrentErrors->SetUseDefaultConditions(false);

 FInitConditionsMP = new TAIEInitConditions();
 FInitConditionsMP->SetDefaultMode(3);
 FInitConditionsMT = new TAIEInitConditions();
 FInitConditionsMT->SetDefaultMode(0);
 FInitConditionsRB = new TAIEInitConditions();
 FInitConditionsRB->SetDefaultMode(3);
 FInitConditionsIO = new TAIEInitConditions();
 FInitConditionsIO->SetDefaultMode(0);

 FMaxMinRuleCountEnabled = false;
 FMaxRuleCountValue = 10;
 FMinRuleCountValue = 1;
 FRulebasePriority = 0.0;
 FOnlyFullFuzzyRule = false;
 FOnlyTrueFuzzyRule = true;
 FBinaryEncoding = false;

 FOC_FuzzyRuleCount = false;
 FOC_FuzzyLength = true;
 FOC_FuzzyAttrCount = true;
 FOC_FuzzySetCount = true;
 FOC_AllFuzzyAttrCount = true;
 FOC_AllFuzzySetCount = true;

 FTag = 0;

 FIsPrepared = false;
 FIsExecuting = false;
 FIsWaiting = false;
 FIsFinished = false;

 FIterationIndex = 0;

 FParetoSolutions.Resize(0);
}
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::~TAIELinguisticFuzzyStructureMOALrnAlgorithm(void)
{
 Clear();

 delete FNoiseGenerator;
 delete FComputingTimer;
 delete FMOGA;
 delete FLrnStopCondition;
 delete FTstStopCondition;
 delete FLrnCurrentErrors;
 delete FTstCurrentErrors;
 delete FInitConditionsRB;
 delete FInitConditionsIO;
 delete FInitConditionsMP;
 delete FInitConditionsMT;
 delete FInitFuzzyStructure;
 delete FRandomGenerator;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::Free(void)
{
 delete this;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::LoadFromFile(IAIEBinaryFile* File)
{
 File->CheckMark("MARK_LINQUISTICFUZZYSTRUCTURE_LEARNINGALGORITHM_BEGIN");
 FIsPrepared = File->LoadBoolean();
 FIsExecuting = File->LoadBoolean();
 FIsWaiting = File->LoadBoolean();
 FIsFinished = File->LoadBoolean();
 FIterationIndex = File->LoadInteger();
 FComputingTimer->LoadFromFile(File);

 FIsNoiseInputs = File->LoadBoolean();
 FNoiseInputs = File->LoadDouble();
 FSequentialLearning = File->LoadBoolean();
 FRandomOrdering = File->LoadBoolean();
 FRandomInitializing = File->LoadBoolean();
 FRandomGeneratorSeedEnabled = File->LoadBoolean();
 FRandomGeneratorSeedValue = File->LoadInteger();
 FRandomGeneratorRangeEnabled = File->LoadBoolean();
 FRandomGeneratorRangeMin = File->LoadDouble();
 FRandomGeneratorRangeMax = File->LoadDouble();
 FTag = File->LoadInteger();

 SetParetoSize(File->LoadInteger());
 for (int i = 0; i < FParetoSolutions.GetCount(); ++i) {
	  FParetoSolutions[i]->FuzzyStructure->LoadFromFile(File);
	  FParetoSolutions[i]->NormError = File->LoadDouble();
	  FParetoSolutions[i]->LrnError = File->LoadDouble();
	  FParetoSolutions[i]->TstError = File->LoadDouble();
	  FParetoSolutions[i]->LrnRecordCountInTol = File->LoadInteger();
	  FParetoSolutions[i]->TstRecordCountInTol = File->LoadInteger();
	  FParetoSolutions[i]->FuzzyStructureComplexity = File->LoadDouble();
	  FParetoSolutions[i]->LrnErrors->LoadFromFile(File);
	  FParetoSolutions[i]->TstErrors->LoadFromFile(File);

	  FParetoSolutions[i]->NormFuzzyRulebaseFuzzyRuleComplexity.Resize(File->LoadInteger());
	  File->LoadData((char*)FParetoSolutions[i]->NormFuzzyRulebaseFuzzyRuleComplexity.GetData(), sizeof(double) * FParetoSolutions[i]->NormFuzzyRulebaseFuzzyRuleComplexity.GetCount());
	  FParetoSolutions[i]->NormFuzzyRulebaseFuzzyRuleCount.Resize(File->LoadInteger());
	  File->LoadData((char*)FParetoSolutions[i]->NormFuzzyRulebaseFuzzyRuleCount.GetData(), sizeof(double) * FParetoSolutions[i]->NormFuzzyRulebaseFuzzyRuleCount.GetCount());
	  FParetoSolutions[i]->NormFuzzyRulebaseFuzzyAttrCount.Resize(File->LoadInteger());
	  File->LoadData((char*)FParetoSolutions[i]->NormFuzzyRulebaseFuzzyAttrCount.GetData(), sizeof(double) * FParetoSolutions[i]->NormFuzzyRulebaseFuzzyAttrCount.GetCount());
	  FParetoSolutions[i]->NormFuzzyRulebaseFuzzySetCount.Resize(File->LoadInteger());
	  File->LoadData((char*)FParetoSolutions[i]->NormFuzzyRulebaseFuzzySetCount.GetData(), sizeof(double) * FParetoSolutions[i]->NormFuzzyRulebaseFuzzySetCount.GetCount());
	  FParetoSolutions[i]->ParetoObjects.Resize(File->LoadInteger());
	  File->LoadData((char*)FParetoSolutions[i]->ParetoObjects.GetData(), sizeof(double) * FParetoSolutions[i]->ParetoObjects.GetCount());
 }

 FLrnStopCondition->LoadFromFile(File);
 FTstStopCondition->LoadFromFile(File);
 FLrnCurrentErrors->LoadFromFile(File);
 FTstCurrentErrors->LoadFromFile(File);

 FInitConditionsRB->LoadFromFile(File);
 FInitConditionsIO->LoadFromFile(File);
 FInitConditionsMP->LoadFromFile(File);
 FInitConditionsMT->LoadFromFile(File);

 FAlgorithmType = File->LoadInteger();
 FMaxMinRuleCountEnabled = File->LoadBoolean();
 FMaxRuleCountValue = File->LoadInteger();
 FMinRuleCountValue = File->LoadInteger();
 FRulebasePriority = File->LoadDouble();
 FOnlyFullFuzzyRule = File->LoadBoolean();
 FOnlyTrueFuzzyRule = File->LoadBoolean();

 FOC_FuzzyRuleCount = File->LoadBoolean();
 FOC_FuzzyLength = File->LoadBoolean();
 FOC_FuzzyAttrCount = File->LoadBoolean();
 FOC_FuzzySetCount = File->LoadBoolean();
 FOC_AllFuzzyAttrCount = File->LoadBoolean();
 FOC_AllFuzzySetCount = File->LoadBoolean();

 FMOGA->LoadFromFile(File);
 File->CheckMark("MARK_LINQUISTICFUZZYSTRUCTURE_LEARNINGALGORITHM_END");
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SaveToFile(IAIEBinaryFile* File)
{
 File->SaveString("MARK_LINQUISTICFUZZYSTRUCTURE_LEARNINGALGORITHM_BEGIN");
 File->SaveBoolean(FIsPrepared);
 File->SaveBoolean(FIsExecuting);
 File->SaveBoolean(FIsWaiting);
 File->SaveBoolean(FIsFinished);
 File->SaveInteger(FIterationIndex);
 FComputingTimer->SaveToFile(File);

 File->SaveBoolean(FIsNoiseInputs);
 File->SaveDouble(FNoiseInputs);
 File->SaveBoolean(FSequentialLearning);
 File->SaveBoolean(FRandomOrdering);
 File->SaveBoolean(FRandomInitializing);
 File->SaveBoolean(FRandomGeneratorSeedEnabled);
 File->SaveInteger(FRandomGeneratorSeedValue);
 File->SaveBoolean(FRandomGeneratorRangeEnabled);
 File->SaveDouble(FRandomGeneratorRangeMin);
 File->SaveDouble(FRandomGeneratorRangeMax);
 File->SaveInteger(FTag);

 File->SaveInteger(FParetoSolutions.GetCount());
 for (int i = 0; i < FParetoSolutions.GetCount(); ++i) {
	  FParetoSolutions[i]->FuzzyStructure->SaveToFile(File);
	  File->SaveDouble(FParetoSolutions[i]->NormError);
	  File->SaveDouble(FParetoSolutions[i]->LrnError);
	  File->SaveDouble(FParetoSolutions[i]->TstError);
	  File->SaveInteger(FParetoSolutions[i]->LrnRecordCountInTol);
	  File->SaveInteger(FParetoSolutions[i]->TstRecordCountInTol);
	  File->SaveDouble(FParetoSolutions[i]->FuzzyStructureComplexity);
	  FParetoSolutions[i]->LrnErrors->SaveToFile(File);
	  FParetoSolutions[i]->TstErrors->SaveToFile(File);

	  File->SaveInteger(FParetoSolutions[i]->NormFuzzyRulebaseFuzzyRuleComplexity.GetCount());
	  File->SaveData((char*)FParetoSolutions[i]->NormFuzzyRulebaseFuzzyRuleComplexity.GetData(), sizeof(double) * FParetoSolutions[i]->NormFuzzyRulebaseFuzzyRuleComplexity.GetCount());
	  File->SaveInteger(FParetoSolutions[i]->NormFuzzyRulebaseFuzzyRuleCount.GetCount());
	  File->SaveData((char*)FParetoSolutions[i]->NormFuzzyRulebaseFuzzyRuleCount.GetData(), sizeof(double) * FParetoSolutions[i]->NormFuzzyRulebaseFuzzyRuleCount.GetCount());
	  File->SaveInteger(FParetoSolutions[i]->NormFuzzyRulebaseFuzzyAttrCount.GetCount());
	  File->SaveData((char*)FParetoSolutions[i]->NormFuzzyRulebaseFuzzyAttrCount.GetData(), sizeof(double) * FParetoSolutions[i]->NormFuzzyRulebaseFuzzyAttrCount.GetCount());
	  File->SaveInteger(FParetoSolutions[i]->NormFuzzyRulebaseFuzzySetCount.GetCount());
	  File->SaveData((char*)FParetoSolutions[i]->NormFuzzyRulebaseFuzzySetCount.GetData(), sizeof(double) * FParetoSolutions[i]->NormFuzzyRulebaseFuzzySetCount.GetCount());
	  File->SaveInteger(FParetoSolutions[i]->ParetoObjects.GetCount());
	  File->SaveData((char*)FParetoSolutions[i]->ParetoObjects.GetData(), sizeof(double) * FParetoSolutions[i]->ParetoObjects.GetCount());
 }

 FLrnStopCondition->SaveToFile(File);
 FTstStopCondition->SaveToFile(File);
 FLrnCurrentErrors->SaveToFile(File);
 FTstCurrentErrors->SaveToFile(File);

 FInitConditionsRB->SaveToFile(File);
 FInitConditionsIO->SaveToFile(File);
 FInitConditionsMP->SaveToFile(File);
 FInitConditionsMT->SaveToFile(File);

 File->SaveInteger(FAlgorithmType);
 File->SaveBoolean(FMaxMinRuleCountEnabled);
 File->SaveInteger(FMaxRuleCountValue);
 File->SaveInteger(FMinRuleCountValue);
 File->SaveDouble(FRulebasePriority);
 File->SaveBoolean(FOnlyFullFuzzyRule);
 File->SaveBoolean(FOnlyTrueFuzzyRule);

 File->SaveBoolean(FOC_FuzzyRuleCount);
 File->SaveBoolean(FOC_FuzzyLength);
 File->SaveBoolean(FOC_FuzzyAttrCount);
 File->SaveBoolean(FOC_FuzzySetCount);
 File->SaveBoolean(FOC_AllFuzzyAttrCount);
 File->SaveBoolean(FOC_AllFuzzySetCount);

 FMOGA->SaveToFile(File);
 File->SaveString("MARK_LINQUISTICFUZZYSTRUCTURE_LEARNINGALGORITHM_END");
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::Prepare(void)
{
 if (!FLrnDataSet) FSystem->RaiseException("Brak zbioru danych ucz¹cych!");
 if (!FTstDataSet) FSystem->RaiseException("Brak zbioru danych testowych!");
 if (!FInitFuzzyStructure) FSystem->RaiseException("Brak inicjuj¹cego systemu regu³owo-rozmytego!");
 if (FIsPrepared) return;
 Clear();

 FComputingTimer->Start();

 FNoiseSet = FIsNoiseInputs ? new TAIECommonDataSet(FSystem) : 0;
 FNoiseGenerator->Initialize(FRandomGeneratorSeedEnabled ? FRandomGeneratorSeedValue : GetTickCount());

 FIterationIndex = 0;
 FIsPrepared = true;
 FIsExecuting = false;
 FIsFinished = false;

 switch (FAlgorithmType) {
	case LFS_LRN_ALG_MOGA: FMOGA->Prepare();
						   break;
 }
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::ExecuteNextIteration(void)
{
 FIsExecuting = true;
 ++FIterationIndex;
 FComputingTimer->Stop();

 // - je¿eli w³¹czone jest zaszumianie próbek ucz¹cych,
 //   przygotowywane s¹ dane ucz¹ce zastêpcze z wprowadzonym szumem
 if (FIsNoiseInputs) {
     double LNoiseValue = FNoiseInputs / 100.0;
	 int LRecordCount = FLrnDataSet->GetRecordCount();
	 int LColumnCount = FLrnDataSet->GetColumnCount();
	 for (int i = 0; i < LRecordCount; ++i) {
		  double* LData = (double*)FLrnDataSet->GetDataSetRecord(i)->GetData();
		  double* LNoise = (double*)FNoiseSet->GetDataSetRecord(i)->GetData();
		  for (int j = 0; j < LColumnCount; ++j) LNoise[j] = LData[j] + FNoiseGenerator->RandomDbl(-LNoiseValue * LData[j], +LNoiseValue * LData[j]);
	 }
 }

 switch (FAlgorithmType) {
	case LFS_LRN_ALG_MOGA: FMOGA->ExecuteNextIteration(); break;
 }

 // - sprawdzenie warunku stopu
 switch (FLrnStopCondition->GetStopMode()) {
	 case 0: if (FIterationIndex < FLrnStopCondition->GetEpochNumber()) return true;
			 break;
	 /*
	 case 1: if (FLrnStopCondition->GetUseDefaultConditions()) {
				 if (FCurrentLrnError > FLrnStopCondition->GetDefaultMinError()) return true;
			 } else {
				 for (int i = 0; i < FLrnCurrentErrors->GetCount(); ++i)
					  if (FLrnCurrentErrors->GetMinError(i) > FLrnStopCondition->GetMinError(i)) return true;
			 }
			 break;
	 case 2: if (FTstStopCondition->GetUseDefaultConditions()) {
				 if (FCurrentTstError > FTstStopCondition->GetDefaultMinError()) return true;
			 } else {
				 for (int i = 0; i < FTstCurrentErrors->GetCount(); ++i)
					  if (FTstCurrentErrors->GetMinError(i) > FTstStopCondition->GetMinError(i)) return true;
			 }
			 break;
	 case 3: if (FLrnStopCondition->GetUseDefaultConditions()) {
				 if ((100.0 * FCurrentLrnRecordCountInTol) / FLrnDataSet->GetRecordCount() < FLrnStopCondition->GetDefaultVectorsInTol()) return true;
			 } else {
				 int LLrnRecordCount = FLrnDataSet->GetRecordCount();
				 for (int i = 0; i < FLrnCurrentErrors->GetCount(); ++i)
					  if ((100.0 * FLrnCurrentErrors->GetVectorsInTol(i)) / LLrnRecordCount < FLrnStopCondition->GetVectorsInTol(i)) return true;
			 }
			 break;
	 case 4: if (FTstStopCondition->GetUseDefaultConditions()) {
				 if ((100.0 * FCurrentTstRecordCountInTol) / FTstDataSet->GetRecordCount() < FTstStopCondition->GetDefaultVectorsInTol()) return true;
			 } else {
				 int LTstRecordCount = FTstDataSet->GetRecordCount();
				 for (int i = 0; i < FTstCurrentErrors->GetCount(); ++i)
					  if ((100.0 * FTstCurrentErrors->GetVectorsInTol(i)) / LTstRecordCount < FTstStopCondition->GetVectorsInTol(i)) return true;
			 }
			 break;
	 case 5: if (FLrnStopCondition->GetUseDefaultConditions()) {
				 if ((100.0 * FCurrentLrnRecordCountInTol) / FLrnDataSet->GetRecordCount() < FLrnStopCondition->GetDefaultVectorsInTol()) return true;
				 if ((100.0 * FCurrentTstRecordCountInTol) / FTstDataSet->GetRecordCount() < FTstStopCondition->GetDefaultVectorsInTol()) return true;
			 } else {
				 int LLrnRecordCount = FLrnDataSet->GetRecordCount();
				 int LTstRecordCount = FTstDataSet->GetRecordCount();
				 for (int i = 0; i < FLrnCurrentErrors->GetCount(); ++i)
					  if ((100.0 * FLrnCurrentErrors->GetVectorsInTol(i)) / LLrnRecordCount < FLrnStopCondition->GetVectorsInTol(i)) return true;
				 for (int i = 0; i < FTstCurrentErrors->GetCount(); ++i)
					  if ((100.0 * FTstCurrentErrors->GetVectorsInTol(i)) / LTstRecordCount < FTstStopCondition->GetVectorsInTol(i)) return true;
			 }
			 break;
	 */
 }
 return false;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::Finish(void)
{
 if (FIsFinished) return;

 __try
 {
	switch (FAlgorithmType) {
		case LFS_LRN_ALG_MOGA: FMOGA->Finish(); break;
	}
 }
 __finally
 {
	if (FNoiseSet) delete FNoiseSet;
	FIsExecuting = false;
	FIsFinished = true;
	FComputingTimer->Stop();
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::Clear(void)
{
 SetParetoSize(0);

 FIsPrepared = false;
 FIsExecuting = false;
 FIsFinished = false;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetIsPrepared(void)
{
 return FIsPrepared;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetIsExecuting(void)
{
 return FIsExecuting;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetIsWaiting(void)
{
 return FIsWaiting;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetIsFinished(void)
{
 return FIsFinished;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetIterationIndex(void)
{
 return FIterationIndex;
}
//---------------------------------------------------------------------------
IAIEComputingTimer* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetComputingTimer(void)
{
 return FComputingTimer;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetIsNoiseInputs(void)
{
 return FIsNoiseInputs;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetIsNoiseInputs(bool AIsNoiseInputs)
{
 if (FIsPrepared || FIsExecuting) return;
 FIsNoiseInputs = AIsNoiseInputs;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetNoiseInputs(void)
{
 return FNoiseInputs;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetNoiseInputs(double ANoiseInputs)
{
 if (FIsPrepared || FIsExecuting) return;
 FNoiseInputs = ANoiseInputs;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetSequentialLearning(void)
{
 return FSequentialLearning;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetSequentialLearning(bool ASequentialLearning)
{
 if (FIsPrepared || FIsExecuting) return;
 FSequentialLearning = ASequentialLearning;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetRandomOrdering(void)
{
 return FRandomOrdering;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetRandomOrdering(bool ARandomOrdering)
{
 if (FIsPrepared || FIsExecuting) return;
 FRandomOrdering = ARandomOrdering;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetRandomInitializing(void)
{
 return FRandomInitializing;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetRandomInitializing(bool ARandomInitializing)
{
 if (FIsPrepared || FIsExecuting) return;
 FRandomInitializing = ARandomInitializing;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetRandomGeneratorSeedEnabled(void)
{
 return FRandomGeneratorSeedEnabled;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetRandomGeneratorSeedEnabled(bool ARandomGeneratorSeedEnabled)
{
 if (FIsPrepared || FIsExecuting) return;
 FRandomGeneratorSeedEnabled = ARandomGeneratorSeedEnabled;
}
//---------------------------------------------------------------------------
int __stdcall  TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetRandomGeneratorSeedValue(void)
{
 return FRandomGeneratorSeedValue;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetRandomGeneratorSeedValue(int ARandomGeneratorSeedValue)
{
 if (FIsPrepared || FIsExecuting) return;
 FRandomGeneratorSeedValue = ARandomGeneratorSeedValue;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetRandomGeneratorRangeEnabled(void)
{
 return FRandomGeneratorRangeEnabled;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetRandomGeneratorRangeEnabled(bool ARandomGeneratorRangeEnabled)
{
 if (FIsPrepared || FIsExecuting) return;
 FRandomGeneratorRangeEnabled = ARandomGeneratorRangeEnabled;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetRandomGeneratorRangeMin(void)
{
 return FRandomGeneratorRangeMin;
}
//---------------------------------------------------------------------------
void __stdcall   TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetRandomGeneratorRangeMin(double ARandomGeneratorRangeMin)
{
 if (FIsPrepared || FIsExecuting) return;
 FRandomGeneratorRangeMin = ARandomGeneratorRangeMin;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetRandomGeneratorRangeMax(void)
{
 return FRandomGeneratorRangeMax;
}
//---------------------------------------------------------------------------
void __stdcall   TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetRandomGeneratorRangeMax(double ARandomGeneratorRangeMax)
{
 if (FIsPrepared || FIsExecuting) return;
 FRandomGeneratorRangeMax = ARandomGeneratorRangeMax;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetTag(void)
{
 return FTag;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetTag(int ATag)
{
 FTag = ATag;
}
//---------------------------------------------------------------------------
IAIEDataSet* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetLrnDataSet(void)
{
 return FLrnDataSet;
}
//---------------------------------------------------------------------------
IAIEDataSet* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetTstDataSet(void)
{
 return FTstDataSet;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetLrnDataSet(IAIEDataSet* ALrnDataSet)
{
 FLrnDataSet = dynamic_cast<TAIEDataSet*>(ALrnDataSet);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetTstDataSet(IAIEDataSet* ATstDataSet)
{
 FTstDataSet = dynamic_cast<TAIEDataSet*>(ATstDataSet);
}
//---------------------------------------------------------------------------
IAIEDataSet* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetNoiseSet(void)
{
 return FNoiseSet;
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyStructure* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetInitFuzzyStructure(void)
{
 return FInitFuzzyStructure;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetAlgorithmType(void)
{
 return FAlgorithmType;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetAlgorithmType(int AAlgorithmType)
{
 if (FIsPrepared || FIsExecuting) return;

 FAlgorithmType = AAlgorithmType;
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetMOGA(void)
{
 return FMOGA;
}
//---------------------------------------------------------------------------
IAIEStopConditions* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetLrnStopCondition(void)
{
 return FLrnStopCondition;
}
//---------------------------------------------------------------------------
IAIEStopConditions* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetTstStopCondition(void)
{
 return FTstStopCondition;
}
//---------------------------------------------------------------------------
IAIEStopConditions* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetLrnCurrentErrors(void)
{
 return FLrnCurrentErrors;
}
//---------------------------------------------------------------------------
IAIEStopConditions* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetTstCurrentErrors(void)
{
 return FTstCurrentErrors;
}
//---------------------------------------------------------------------------
IAIEInitConditions* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetInitConditionsRB(void)
{
 return FInitConditionsRB;
}
//---------------------------------------------------------------------------
IAIEInitConditions* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetInitConditionsIO(void)
{
 return FInitConditionsIO;
}
//---------------------------------------------------------------------------
IAIEInitConditions* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetInitConditionsMP(void)
{
 return FInitConditionsMP;
}
//---------------------------------------------------------------------------
IAIEInitConditions* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetInitConditionsMT(void)
{
 return FInitConditionsMT;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetMaxMinRuleCountEnabled(void)
{
 return FMaxMinRuleCountEnabled;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetMaxMinRuleCountEnabled(bool AMaxMinRuleCountEnabled)
{
 if (FIsPrepared || FIsExecuting) return;
 FMaxMinRuleCountEnabled = AMaxMinRuleCountEnabled;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetMaxRuleCountValue(void)
{
 return FMaxRuleCountValue;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetMaxRuleCountValue(int AMaxRuleCountValue)
{
 if (FIsPrepared || FIsExecuting) return;
 FMaxRuleCountValue = AMaxRuleCountValue;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetMinRuleCountValue(void)
{
 return FMinRuleCountValue;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetMinRuleCountValue(int AMinRuleCountValue)
{
 if (FIsPrepared || FIsExecuting) return;
 FMinRuleCountValue = AMinRuleCountValue;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetRulebasePriority(void)
{
 return FRulebasePriority;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetRulebasePriority(double ARulebasePriority)
{
 if (FIsPrepared || FIsExecuting) return;
 FRulebasePriority = ARulebasePriority;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetOnlyFullFuzzyRule(void)
{
 return FOnlyFullFuzzyRule;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetOnlyFullFuzzyRule(bool AOnlyFullFuzzyRule)
{
 if (FIsPrepared || FIsExecuting) return;
 FOnlyFullFuzzyRule = AOnlyFullFuzzyRule;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetOnlyTrueFuzzyRule(void)
{
 return FOnlyTrueFuzzyRule;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetOnlyTrueFuzzyRule(bool AOnlyTrueFuzzyRule)
{
 if (FIsPrepared || FIsExecuting) return;
 FOnlyTrueFuzzyRule = AOnlyTrueFuzzyRule;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetBinaryEncoding(void)
{
 return FBinaryEncoding;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetBinaryEncoding(bool ABinaryEncoding)
{
 if (FIsPrepared || FIsExecuting) return;
 FBinaryEncoding = ABinaryEncoding;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetOC_FuzzyRuleCount(void)
{
 return FOC_FuzzyRuleCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetOC_FuzzyRuleCount(bool AOC_FuzzyRuleCount)
{
 if (FIsPrepared || FIsExecuting) return;
 FOC_FuzzyRuleCount = AOC_FuzzyRuleCount;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetOC_FuzzyLength(void)
{
 return FOC_FuzzyLength;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetOC_FuzzyLength(bool AOC_FuzzyLength)
{
 if (FIsPrepared || FIsExecuting) return;
 FOC_FuzzyLength = AOC_FuzzyLength;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetOC_FuzzyAttrCount(void)
{
 return FOC_FuzzyAttrCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetOC_FuzzyAttrCount(bool AOC_FuzzyAttrCount)
{
 if (FIsPrepared || FIsExecuting) return;
 FOC_FuzzyAttrCount = AOC_FuzzyAttrCount;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetOC_FuzzySetCount(void)
{
 return FOC_FuzzySetCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetOC_FuzzySetCount(bool AOC_FuzzySetCount)
{
 if (FIsPrepared || FIsExecuting) return;
 FOC_FuzzySetCount = AOC_FuzzySetCount;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetOC_AllFuzzyAttrCount(void)
{
 return FOC_AllFuzzyAttrCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetOC_AllFuzzyAttrCount(bool AOC_AllFuzzyAttrCount)
{
 if (FIsPrepared || FIsExecuting) return;
 FOC_AllFuzzyAttrCount = AOC_AllFuzzyAttrCount;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetOC_AllFuzzySetCount(void)
{
 return FOC_AllFuzzySetCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetOC_AllFuzzySetCount(bool AOC_AllFuzzySetCount)
{
 if (FIsPrepared || FIsExecuting) return;
 FOC_AllFuzzySetCount = AOC_AllFuzzySetCount;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetParetoSize(void)
{
 if (!FIsPrepared) return 0;
 return FParetoSolutions.GetCount();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::SetParetoSize(int AParetoSize)
{
 int FParetoSize = FParetoSolutions.GetCount();
 for (int i = AParetoSize; i < FParetoSize; ++i) {
	  delete FParetoSolutions[i]->FuzzyStructure;
	  delete FParetoSolutions[i]->LrnErrors;
	  delete FParetoSolutions[i]->TstErrors;
	  delete FParetoSolutions[i];
 }
 FParetoSolutions.Resize(AParetoSize);
 for (int i = FParetoSize; i < AParetoSize; ++i) {
      FParetoSolutions[i] = new TParetoSolution();
	  FParetoSolutions[i]->FuzzyStructure = new TAIELinguisticFuzzyStructure(FSystem, 0, false);
	  FParetoSolutions[i]->LrnErrors = new TAIEStopConditions();
	  FParetoSolutions[i]->TstErrors = new TAIEStopConditions();
 }
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyStructure* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetFuzzyStructure(int ParetoIndex)
{
 if (!FIsPrepared) return NULL;
 return FParetoSolutions[ParetoIndex]->FuzzyStructure;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetNormError(int ParetoIndex)
{
 if (!FIsPrepared) return 0.0;
 return FParetoSolutions[ParetoIndex]->NormError;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetLrnError(int ParetoIndex)
{
 if (!FIsPrepared) return 0.0;
 return FParetoSolutions[ParetoIndex]->LrnError;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetTstError(int ParetoIndex)
{
 if (!FIsPrepared) return 0.0;
 return FParetoSolutions[ParetoIndex]->TstError;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetLrnRecordCountInTol(int ParetoIndex)
{
 if (!FIsPrepared) return 0;
 return FParetoSolutions[ParetoIndex]->LrnRecordCountInTol;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetTstRecordCountInTol(int ParetoIndex)
{
 if (!FIsPrepared) return 0;
 return FParetoSolutions[ParetoIndex]->TstRecordCountInTol;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetFuzzySystemComplexity(int ParetoIndex)
{
 if (!FIsPrepared) return 0.0;
 return FParetoSolutions[ParetoIndex]->FuzzyStructureComplexity;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetNormFuzzyRulebaseFuzzyRuleComplexity(int ParetoIndex, int RulebaseIndex)
{
 if (!FIsPrepared) return 0.0;
 return FParetoSolutions[ParetoIndex]->NormFuzzyRulebaseFuzzyRuleComplexity[RulebaseIndex];
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetNormFuzzyRulebaseFuzzyRuleCount(int ParetoIndex, int RulebaseIndex)
{
 if (!FIsPrepared) return 0.0;
 return FParetoSolutions[ParetoIndex]->NormFuzzyRulebaseFuzzyRuleCount[RulebaseIndex];
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetNormFuzzyRulebaseFuzzyAttrCount(int ParetoIndex, int RulebaseIndex)
{
 if (!FIsPrepared) return 0.0;
 return FParetoSolutions[ParetoIndex]->NormFuzzyRulebaseFuzzyAttrCount[RulebaseIndex];
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetNormFuzzyRulebaseFuzzySetCount(int ParetoIndex, int RulebaseIndex)
{
 if (!FIsPrepared) return 0.0;
 return FParetoSolutions[ParetoIndex]->NormFuzzyRulebaseFuzzySetCount[RulebaseIndex];
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetParetoObjectiveCount(void)
{
 switch (FAlgorithmType) {
		case LFS_LRN_ALG_MOGA: return FMOGA->GetParetoObjectiveCount();
 }
 return 0;
}
//---------------------------------------------------------------------------
const char* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetParetoObjectiveName(int ObjectiveIndex)
{
 switch (FAlgorithmType) {
		case LFS_LRN_ALG_MOGA: return FMOGA->GetParetoObjectiveName(ObjectiveIndex);
 }
 return 0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetParetoObjectiveValue(int ParetoIndex, int ObjectiveIndex)
{
 return FParetoSolutions[ParetoIndex]->ParetoObjects[ObjectiveIndex];
}
//---------------------------------------------------------------------------
TAIERandomGenerator64* __stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm::GetRandomGenerator(void)
{
 return FRandomGenerator;
}
//---------------------------------------------------------------------------

