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
#include "UAIELinguisticFuzzyStructureLrnAlgorithm.h"
#include "UAIELinguisticFuzzyStructureLrnAGAlgorithm.h"
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::TAIELinguisticFuzzyStructureLrnAlgorithm(TAIESystem* ASystem)
{
 FSystem = ASystem;
 FLrnDataSet = 0;
 FTstDataSet = 0;
 FFuzzyStructure = new TAIELinguisticFuzzyStructure(FSystem, 0, false);
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
 FAG = new TAIELinguisticFuzzyStructureLrnAGAlgorithm(this, FSystem);

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

 FCurrentNormError = 0.0;
 FCurrentLrnError = 0.0;
 FCurrentTstError = 0.0;
 FCurrentLrnRecordCountInTol = 0;
 FCurrentTstRecordCountInTol = 0;
 FCurrentFuzzyStructureComplexity = 0.0;
 FCurrentNormFuzzyRulebaseFuzzyRuleComplexity.Resize(0);
 FCurrentNormFuzzyRulebaseFuzzyRuleCount.Resize(0);
 FCurrentNormFuzzyRulebaseFuzzyAttrCount.Resize(0);
 FCurrentNormFuzzyRulebaseFuzzySetCount.Resize(0);
}
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::~TAIELinguisticFuzzyStructureLrnAlgorithm(void)
{
 delete FNoiseGenerator;
 delete FComputingTimer;
 delete FAG;
 delete FLrnStopCondition;
 delete FTstStopCondition;
 delete FLrnCurrentErrors;
 delete FTstCurrentErrors;
 delete FInitConditionsRB;
 delete FInitConditionsIO;
 delete FInitConditionsMP;
 delete FInitConditionsMT;
 delete FFuzzyStructure;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::Free(void)
{
 delete this;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::LoadFromFile(IAIEBinaryFile* File)
{
 File->CheckMark("MARK_LINGUISTICFUZZYSTRUCTURE_LEARNINGALGORITHM_BEGIN");
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

 FCurrentNormError = File->LoadDouble();
 FCurrentLrnError = File->LoadDouble();
 FCurrentTstError = File->LoadDouble();
 FCurrentLrnRecordCountInTol = File->LoadInteger();
 FCurrentTstRecordCountInTol = File->LoadInteger();

 FCurrentFuzzyStructureComplexity = File->LoadDouble();

 FCurrentNormFuzzyRulebaseFuzzyRuleComplexity.Resize(File->LoadInteger());
 File->LoadData((char*)FCurrentNormFuzzyRulebaseFuzzyRuleComplexity.GetData(), sizeof(double) * FCurrentNormFuzzyRulebaseFuzzyRuleComplexity.GetCount());
 FCurrentNormFuzzyRulebaseFuzzyRuleCount.Resize(File->LoadInteger());
 File->LoadData((char*)FCurrentNormFuzzyRulebaseFuzzyRuleCount.GetData(), sizeof(double) * FCurrentNormFuzzyRulebaseFuzzyRuleCount.GetCount());
 FCurrentNormFuzzyRulebaseFuzzyAttrCount.Resize(File->LoadInteger());
 File->LoadData((char*)FCurrentNormFuzzyRulebaseFuzzyAttrCount.GetData(), sizeof(double) * FCurrentNormFuzzyRulebaseFuzzyAttrCount.GetCount());
 FCurrentNormFuzzyRulebaseFuzzySetCount.Resize(File->LoadInteger());
 File->LoadData((char*)FCurrentNormFuzzyRulebaseFuzzySetCount.GetData(), sizeof(double) * FCurrentNormFuzzyRulebaseFuzzySetCount.GetCount());

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

 FAG->LoadFromFile(File);
 File->CheckMark("MARK_LINGUISTICFUZZYSTRUCTURE_LEARNINGALGORITHM_END");
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SaveToFile(IAIEBinaryFile* File)
{
 File->SaveString("MARK_LINGUISTICFUZZYSTRUCTURE_LEARNINGALGORITHM_BEGIN");
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

 File->SaveDouble(FCurrentNormError);
 File->SaveDouble(FCurrentLrnError);
 File->SaveDouble(FCurrentTstError);
 File->SaveInteger(FCurrentLrnRecordCountInTol);
 File->SaveInteger(FCurrentTstRecordCountInTol);

 File->SaveDouble(FCurrentFuzzyStructureComplexity);
 File->SaveInteger(FCurrentNormFuzzyRulebaseFuzzyRuleComplexity.GetCount());
 File->SaveData((char*)FCurrentNormFuzzyRulebaseFuzzyRuleComplexity.GetData(), sizeof(double) * FCurrentNormFuzzyRulebaseFuzzyRuleComplexity.GetCount());
 File->SaveInteger(FCurrentNormFuzzyRulebaseFuzzyRuleCount.GetCount());
 File->SaveData((char*)FCurrentNormFuzzyRulebaseFuzzyRuleCount.GetData(), sizeof(double) * FCurrentNormFuzzyRulebaseFuzzyRuleCount.GetCount());
 File->SaveInteger(FCurrentNormFuzzyRulebaseFuzzyAttrCount.GetCount());
 File->SaveData((char*)FCurrentNormFuzzyRulebaseFuzzyAttrCount.GetData(), sizeof(double) * FCurrentNormFuzzyRulebaseFuzzyAttrCount.GetCount());
 File->SaveInteger(FCurrentNormFuzzyRulebaseFuzzySetCount.GetCount());
 File->SaveData((char*)FCurrentNormFuzzyRulebaseFuzzySetCount.GetData(), sizeof(double) * FCurrentNormFuzzyRulebaseFuzzySetCount.GetCount());

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

 FAG->SaveToFile(File);
 File->SaveString("MARK_LINGUISTICFUZZYSTRUCTURE_LEARNINGALGORITHM_END");
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::Prepare(void)
{
 if (!FLrnDataSet) FSystem->RaiseException("Brak zbioru danych ucz¹cych!");
 if (!FTstDataSet) FSystem->RaiseException("Brak zbioru danych testowych!");
 if (!FFuzzyStructure) FSystem->RaiseException("Brak systemu regu³owo-rozmytego!");
 if (FIsPrepared) return;
 FComputingTimer->Start();

 FNoiseSet = FIsNoiseInputs ? new TAIECommonDataSet(FSystem) : 0;
 FNoiseGenerator->Initialize(FRandomGeneratorSeedEnabled ? FRandomGeneratorSeedValue : GetTickCount());

 FCurrentFuzzyStructureComplexity = 0.0;
 FCurrentNormFuzzyRulebaseFuzzyRuleComplexity.Resize(FFuzzyStructure->GetFuzzyRulebaseCount());
 FCurrentNormFuzzyRulebaseFuzzyRuleCount.Resize(FFuzzyStructure->GetFuzzyRulebaseCount());
 FCurrentNormFuzzyRulebaseFuzzyAttrCount.Resize(FFuzzyStructure->GetFuzzyRulebaseCount());
 FCurrentNormFuzzyRulebaseFuzzySetCount.Resize(FFuzzyStructure->GetFuzzyRulebaseCount());

 FIterationIndex = 0;
 FIsPrepared = true;
 FIsExecuting = false;
 FIsFinished = false;

 switch (FAlgorithmType) {
	case LFS_LRN_ALG_AG: FAG->Prepare(); break;
 }
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::ExecuteNextIteration(void)
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
	case LFS_LRN_ALG_AG: FAG->ExecuteNextIteration(); break;
 }

 // - sprawdzenie warunku stopu
 switch (FLrnStopCondition->GetStopMode()) {
	 case 0: if (FIterationIndex < FLrnStopCondition->GetEpochNumber()) return true;
			 break;
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
 }
 return false;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::Finish(void)
{
 if (FIsFinished) return;

 __try
 {
	switch (FAlgorithmType) {
		case LFS_LRN_ALG_AG: FAG->Finish(); break;
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
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::Clear(void)
{
 FIsPrepared = false;
 FIsExecuting = false;
 FIsFinished = false;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetIsPrepared(void)
{
 return FIsPrepared;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetIsExecuting(void)
{
 return FIsExecuting;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetIsWaiting(void)
{
 return FIsWaiting;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetIsFinished(void)
{
 return FIsFinished;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetIterationIndex(void)
{
 return FIterationIndex;
}
//---------------------------------------------------------------------------
IAIEComputingTimer* __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetComputingTimer(void)
{
 return FComputingTimer;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetIsNoiseInputs(void)
{
 return FIsNoiseInputs;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetIsNoiseInputs(bool AIsNoiseInputs)
{
 if (FIsPrepared || FIsExecuting) return;
 FIsNoiseInputs = AIsNoiseInputs;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetNoiseInputs(void)
{
 return FNoiseInputs;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetNoiseInputs(double ANoiseInputs)
{
 if (FIsPrepared || FIsExecuting) return;
 FNoiseInputs = ANoiseInputs;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetSequentialLearning(void)
{
 return FSequentialLearning;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetSequentialLearning(bool ASequentialLearning)
{
 if (FIsPrepared || FIsExecuting) return;
 FSequentialLearning = ASequentialLearning;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetRandomOrdering(void)
{
 return FRandomOrdering;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetRandomOrdering(bool ARandomOrdering)
{
 if (FIsPrepared || FIsExecuting) return;
 FRandomOrdering = ARandomOrdering;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetRandomInitializing(void)
{
 return FRandomInitializing;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetRandomInitializing(bool ARandomInitializing)
{
 if (FIsPrepared || FIsExecuting) return;
 FRandomInitializing = ARandomInitializing;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetRandomGeneratorSeedEnabled(void)
{
 return FRandomGeneratorSeedEnabled;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetRandomGeneratorSeedEnabled(bool ARandomGeneratorSeedEnabled)
{
 if (FIsPrepared || FIsExecuting) return;
 FRandomGeneratorSeedEnabled = ARandomGeneratorSeedEnabled;
}
//---------------------------------------------------------------------------
int __stdcall  TAIELinguisticFuzzyStructureLrnAlgorithm::GetRandomGeneratorSeedValue(void)
{
 return FRandomGeneratorSeedValue;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetRandomGeneratorSeedValue(int ARandomGeneratorSeedValue)
{
 if (FIsPrepared || FIsExecuting) return;
 FRandomGeneratorSeedValue = ARandomGeneratorSeedValue;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetRandomGeneratorRangeEnabled(void)
{
 return FRandomGeneratorRangeEnabled;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetRandomGeneratorRangeEnabled(bool ARandomGeneratorRangeEnabled)
{
 if (FIsPrepared || FIsExecuting) return;
 FRandomGeneratorRangeEnabled = ARandomGeneratorRangeEnabled;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetRandomGeneratorRangeMin(void)
{
 return FRandomGeneratorRangeMin;
}
//---------------------------------------------------------------------------
void __stdcall   TAIELinguisticFuzzyStructureLrnAlgorithm::SetRandomGeneratorRangeMin(double ARandomGeneratorRangeMin)
{
 if (FIsPrepared || FIsExecuting) return;
 FRandomGeneratorRangeMin = ARandomGeneratorRangeMin;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetRandomGeneratorRangeMax(void)
{
 return FRandomGeneratorRangeMax;
}
//---------------------------------------------------------------------------
void __stdcall   TAIELinguisticFuzzyStructureLrnAlgorithm::SetRandomGeneratorRangeMax(double ARandomGeneratorRangeMax)
{
 if (FIsPrepared || FIsExecuting) return;
 FRandomGeneratorRangeMax = ARandomGeneratorRangeMax;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetTag(void)
{
 return FTag;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetTag(int ATag)
{
 FTag = ATag;
}
//---------------------------------------------------------------------------
IAIEDataSet* __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetLrnDataSet(void)
{
 return FLrnDataSet;
}
//---------------------------------------------------------------------------
IAIEDataSet* __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetTstDataSet(void)
{
 return FTstDataSet;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetLrnDataSet(IAIEDataSet* ALrnDataSet)
{
 FLrnDataSet = dynamic_cast<TAIEDataSet*>(ALrnDataSet);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetTstDataSet(IAIEDataSet* ATstDataSet)
{
 FTstDataSet = dynamic_cast<TAIEDataSet*>(ATstDataSet);
}
//---------------------------------------------------------------------------
IAIEDataSet* __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetNoiseSet(void)
{
 return FNoiseSet;
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyStructure* __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetFuzzyStructure(void)
{
 return FFuzzyStructure;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetAlgorithmType(void)
{
 return FAlgorithmType;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetAlgorithmType(int AAlgorithmType)
{
 if (FIsPrepared || FIsExecuting) return;
 FAlgorithmType = AAlgorithmType;
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyStructureLrnAGAlgorithm* __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetAG(void)
{
 return FAG;
}
//---------------------------------------------------------------------------
IAIEStopConditions* __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetLrnStopCondition(void)
{
 return FLrnStopCondition;
}
//---------------------------------------------------------------------------
IAIEStopConditions* __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetTstStopCondition(void)
{
 return FTstStopCondition;
}
//---------------------------------------------------------------------------
IAIEStopConditions* __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetLrnCurrentErrors(void)
{
 return FLrnCurrentErrors;
}
//---------------------------------------------------------------------------
IAIEStopConditions* __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetTstCurrentErrors(void)
{
 return FTstCurrentErrors;
}
//---------------------------------------------------------------------------
IAIEInitConditions* __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetInitConditionsRB(void)
{
 return FInitConditionsRB;
}
//---------------------------------------------------------------------------
IAIEInitConditions* __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetInitConditionsIO(void)
{
 return FInitConditionsIO;
}
//---------------------------------------------------------------------------
IAIEInitConditions* __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetInitConditionsMP(void)
{
 return FInitConditionsMP;
}
//---------------------------------------------------------------------------
IAIEInitConditions* __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetInitConditionsMT(void)
{
 return FInitConditionsMT;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetMaxMinRuleCountEnabled(void)
{
 return FMaxMinRuleCountEnabled;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetMaxMinRuleCountEnabled(bool AMaxMinRuleCountEnabled)
{
 if (FIsPrepared || FIsExecuting) return;
 FMaxMinRuleCountEnabled = AMaxMinRuleCountEnabled;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetMaxRuleCountValue(void)
{
 return FMaxRuleCountValue;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetMaxRuleCountValue(int AMaxRuleCountValue)
{
 if (FIsPrepared || FIsExecuting) return;
 FMaxRuleCountValue = AMaxRuleCountValue;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetMinRuleCountValue(void)
{
 return FMinRuleCountValue;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetMinRuleCountValue(int AMinRuleCountValue)
{
 if (FIsPrepared || FIsExecuting) return;
 FMinRuleCountValue = AMinRuleCountValue;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetRulebasePriority(void)
{
 return FRulebasePriority;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetRulebasePriority(double ARulebasePriority)
{
 if (FIsPrepared || FIsExecuting) return;
 FRulebasePriority = ARulebasePriority;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetOnlyFullFuzzyRule(void)
{
 return FOnlyFullFuzzyRule;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetOnlyFullFuzzyRule(bool AOnlyFullFuzzyRule)
{
 if (FIsPrepared || FIsExecuting) return;
 FOnlyFullFuzzyRule = AOnlyFullFuzzyRule;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetOnlyTrueFuzzyRule(void)
{
 return FOnlyTrueFuzzyRule;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetOnlyTrueFuzzyRule(bool AOnlyTrueFuzzyRule)
{
 if (FIsPrepared || FIsExecuting) return;
 FOnlyTrueFuzzyRule = AOnlyTrueFuzzyRule;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetBinaryEncoding(void)
{
 return FBinaryEncoding;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetBinaryEncoding(bool ABinaryEncoding)
{
 if (FIsPrepared || FIsExecuting) return;
 FBinaryEncoding = ABinaryEncoding;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetOC_FuzzyRuleCount(void)
{
 return FOC_FuzzyRuleCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetOC_FuzzyRuleCount(bool AOC_FuzzyRuleCount)
{
 if (FIsPrepared || FIsExecuting) return;
 FOC_FuzzyRuleCount = AOC_FuzzyRuleCount;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetOC_FuzzyLength(void)
{
 return FOC_FuzzyLength;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetOC_FuzzyLength(bool AOC_FuzzyLength)
{
 if (FIsPrepared || FIsExecuting) return;
 FOC_FuzzyLength = AOC_FuzzyLength;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetOC_FuzzyAttrCount(void)
{
 return FOC_FuzzyAttrCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetOC_FuzzyAttrCount(bool AOC_FuzzyAttrCount)
{
 if (FIsPrepared || FIsExecuting) return;
 FOC_FuzzyAttrCount = AOC_FuzzyAttrCount;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetOC_FuzzySetCount(void)
{
 return FOC_FuzzySetCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetOC_FuzzySetCount(bool AOC_FuzzySetCount)
{
 if (FIsPrepared || FIsExecuting) return;
 FOC_FuzzySetCount = AOC_FuzzySetCount;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetOC_AllFuzzyAttrCount(void)
{
 return FOC_AllFuzzyAttrCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetOC_AllFuzzyAttrCount(bool AOC_AllFuzzyAttrCount)
{
 if (FIsPrepared || FIsExecuting) return;
 FOC_AllFuzzyAttrCount = AOC_AllFuzzyAttrCount;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetOC_AllFuzzySetCount(void)
{
 return FOC_AllFuzzySetCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetOC_AllFuzzySetCount(bool AOC_AllFuzzySetCount)
{
 if (FIsPrepared || FIsExecuting) return;
 FOC_AllFuzzySetCount = AOC_AllFuzzySetCount;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetCurrentNormError(void)
{
 return FCurrentNormError;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetCurrentNormError(double ACurrentNormError)
{
 FCurrentNormError = ACurrentNormError;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetCurrentLrnError(void)
{
 return FCurrentLrnError;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetCurrentLrnError(double ACurrentLrnError)
{
 FCurrentLrnError = ACurrentLrnError;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetCurrentTstError(void)
{
 return FCurrentTstError;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetCurrentTstError(double ACurrentTstError)
{
 FCurrentTstError = ACurrentTstError;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetCurrentLrnRecordCountInTol(void)
{
 return FCurrentLrnRecordCountInTol;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetCurrentLrnRecordCountInTol(int ACurrentLrnRecordCountInTol)
{
 FCurrentLrnRecordCountInTol = ACurrentLrnRecordCountInTol;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetCurrentTstRecordCountInTol(void)
{
 return FCurrentTstRecordCountInTol;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::SetCurrentTstRecordCountInTol(int ACurrentTstRecordCountInTol)
{
 FCurrentTstRecordCountInTol = ACurrentTstRecordCountInTol;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetCurrentFuzzySystemComplexity(void)
{
 return FCurrentFuzzyStructureComplexity;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetCurrentNormFuzzyRulebaseFuzzyRuleComplexity(int Index)
{
 if (Index >= FCurrentNormFuzzyRulebaseFuzzyRuleComplexity.GetCount()) return 0.0;
 return FCurrentNormFuzzyRulebaseFuzzyRuleComplexity[Index];
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetCurrentNormFuzzyRulebaseFuzzyRuleCount(int Index)
{
 if (Index >= FCurrentNormFuzzyRulebaseFuzzyRuleCount.GetCount()) return 0;
 return FCurrentNormFuzzyRulebaseFuzzyRuleCount[Index];
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetCurrentNormFuzzyRulebaseFuzzyAttrCount(int Index)
{
 if (Index >= FCurrentNormFuzzyRulebaseFuzzyAttrCount.GetCount()) return 0;
 return FCurrentNormFuzzyRulebaseFuzzyAttrCount[Index];
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAlgorithm::GetCurrentNormFuzzyRulebaseFuzzySetCount(int Index)
{
 if (Index >= FCurrentNormFuzzyRulebaseFuzzySetCount.GetCount()) return 0;
 return FCurrentNormFuzzyRulebaseFuzzySetCount[Index];
}
//---------------------------------------------------------------------------

