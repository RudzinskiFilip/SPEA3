//---------------------------------------------------------------------------
#define _USE_MATH_DEFINES
#include <math.h>
#include <new>

#include "UTemplates.h"
#include "UAIERandomGenerator64.h"
#include "UAIEMultilayerPerceptron.h"
#include "UAIESystem.h"
#include "UAIENumaDevice.h"
#include "UAIENumaNode.h"
#include "UAIENumaUnit.h"
#include "UAIENumaThreads.h"
#include "UAIEBinaryFile.h"
#include "UAIEExceptions.h"
#include "UAIEDataSet.h"
#include "UAIEDataSetRecord.h"
#include "UAIEDataSetColumn.h"
#include "UAIEDataSetStats.h"
#include "UAIEDataMatrix.h"
#include "UAIEDataMatrixColumn.h"
#include "UAIEDataMatrixRecord.h"
#include "UAIEDataMatrixNumaEx.h"
#include "UAIENumeralDataSet.h"
#include "UAIENumeralDataSetRecord.h"
#include "UAIENumeralDataSetColumn.h"
#include "UAIENumaNumeralDataSet.h"
#include "UAIENumaNumeralDataSetRecord.h"
#include "UAIENumaNumeralDataSetColumn.h"
#include "UAIELinguisticFuzzySet.h"
#include "UAIELinguisticFuzzyRule.h"
#include "UAIELinguisticFuzzyStructure.h"
//---------------------------------------------------------------------------
BEGIN_NAMESPACE(NAIELinguisticFuzzyStructure)
//---------------------------------------------------------------------------
// Funkcje wielow¹tkowe przeliczaj¹ce odpowiedŸ systemu
//---------------------------------------------------------------------------
struct TAIENodeData
{
 //dane sta³e, nie zmieniaj¹ce siê podczas przeliczania odpowiedzi
 TAIESystem* System;
 TAIELinguisticFuzzyStructure* FuzzyStructure;
 IAIEDataSet* InpDataSet;
 IAIEDataSet* OutDataSet;

 double*  YTols;                  //zadane tolerancje dla danych testowych
};
//---------------------------------------------------------------------------
struct TAIEUnitData
{
 //dane sta³e, nie zmieniaj¹ce siê podczas przeliczania odpowiedzi
 double*  YErrors;                //suma kwadratów b³êdów na poszczególnych wyjœciach dla danych ucz¹cych
 int* 	  YOutputsInTol;          //liczba rekordów danych ucz¹cych dla których dane wyjœcie by³o w tolerancji
 int	  VectorsInTol;          //liczba wektorów danych ucz¹cych w tolerancji

 double	  Error;                 //suma kwadratów b³êdów dla danych ucz¹cych
 double   NormalizedError;		 //suma kwadratów normalizowanych b³êdów dla danych ucz¹cych
};
//---------------------------------------------------------------------------
// Procedura wielow¹tkowa przygotowuj¹ca dane do przeliczania odpowiedŸ sieci (dla ka¿dego wêz³a)
//---------------------------------------------------------------------------
void __stdcall NumaNodePrepare(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 TAIELinguisticFuzzyStructure* LFuzzyStructure = (TAIELinguisticFuzzyStructure*)AThreads->GetData();

 TAIENodeData* LNodeData = (TAIENodeData*)AThreads->LocalAllocMemory(ANodeIndex, sizeof(TAIENodeData));
 LNodeData->System = dynamic_cast<TAIESystem*>(AThreads->GetNumaDevice()->GetSystem());
 LNodeData->FuzzyStructure = new TAIELinguisticFuzzyStructure(LNodeData->System, ANodeIndex, false);
 LNodeData->YTols = (double*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, LFuzzyStructure->GetErrorTableCount() * sizeof(double));
 AThreads->SetNodeData(ANodeIndex, LNodeData);
}
//---------------------------------------------------------------------------
// Procedura wielow¹tkowa przygotowuj¹ca dane do przeliczania odpowiedŸ sieci (dla ka¿dego wêz³a)
//---------------------------------------------------------------------------
void __stdcall NumaNodeUpdate(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 TAIELinguisticFuzzyStructure* LFuzzyStructure = (TAIELinguisticFuzzyStructure*)AThreads->GetData();

 TAIENodeData* LNodeData = (TAIENodeData*)AThreads->GetNodeData(ANodeIndex);
 LNodeData->FuzzyStructure->Assign(LFuzzyStructure);
}
//---------------------------------------------------------------------------
// Procedura wielow¹tkowa uruchamiana po zakoñczeniu przeliczania odpowiedzi sieci (dla ka¿dego wêz³a)
//---------------------------------------------------------------------------
void __stdcall NumaNodeFinish(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 TAIENodeData* LNodeData = (TAIENodeData*)AThreads->GetNodeData(ANodeIndex);

 AThreads->PrivateFreeMemory(LNodeData->YTols);
 delete LNodeData->FuzzyStructure;

 AThreads->LocalFreeMemory(LNodeData);
}
//---------------------------------------------------------------------------
// Procedura wielow¹tkowa uruchamiana na pocz¹tku procesu uczenia dla ka¿dego w¹tku
//---------------------------------------------------------------------------
void __stdcall NumaUnitPrepare(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 //odczytanie pomocniczych wskaŸników i utworzenie struktur danych
 TAIENodeData* LNodeData = (TAIENodeData*)AThreads->GetNodeData(ANodeIndex);

 //tworzenie struktury danych dla ka¿dego w¹tku
 TAIEUnitData* LUnitData = (TAIEUnitData*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, sizeof(TAIEUnitData));
 AThreads->SetUnitData(ANodeIndex, AUnitIndex, LUnitData);

 LUnitData->YErrors = (double*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, LNodeData->FuzzyStructure->GetErrorTableCount() * sizeof(double));
 LUnitData->YOutputsInTol = (int*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, LNodeData->FuzzyStructure->GetErrorTableCount() * sizeof(int));
}
//---------------------------------------------------------------------------
// Procedura wielow¹tkowa uruchamiana na koñcu procesu uczenia dla ka¿dego w¹tku
//---------------------------------------------------------------------------
void __stdcall NumaUnitFinish(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 //odczytanie pomocniczych wskaŸników i utworzenie struktur danych
 TAIEUnitData* LUnitData = (TAIEUnitData*)AThreads->GetUnitData(ANodeIndex, AUnitIndex);

 AThreads->PrivateFreeMemory(LUnitData->YErrors);
 AThreads->PrivateFreeMemory(LUnitData->YOutputsInTol);

 AThreads->PrivateFreeMemory(LUnitData);
}
//---------------------------------------------------------------------------
// Procedura wielow¹tkowa przeliczaj¹ca czêœciow¹ odpowiedŸ sieci (dla ka¿dego w¹tku)
//---------------------------------------------------------------------------
void __stdcall NumaUnitExecute(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 TAIENodeData* LNodeData = (TAIENodeData*)AThreads->GetNodeData(ANodeIndex);
 IAIEDataSet*  LInpDataSet = LNodeData->InpDataSet;
 IAIEDataSet*  LOutDataSet = LNodeData->OutDataSet;

 int LThreadCount = AThreads->GetThreadCount();
 int LRecordCount = LInpDataSet->GetRecordCount() / LThreadCount;
 int LRecordIndex = AThreadIndex * LRecordCount;
 if (AThreadIndex == LThreadCount - 1) LRecordCount += LInpDataSet->GetRecordCount() % LThreadCount;

 if (dynamic_cast<TAIENumaNumeralDataSet*>(LInpDataSet)) {
	TAIENumaNumeralDataSet* NInpDataSet = dynamic_cast<TAIENumaNumeralDataSet*>(LInpDataSet);
	TAIENumaNumeralDataSet* NOutDataSet = dynamic_cast<TAIENumaNumeralDataSet*>(LOutDataSet);

	TNumaVector<double> SNormTable(ANodeIndex, LNodeData->FuzzyStructure->GetSNormTableCount());
	for (int k = 0; k < LRecordCount; ++k)
		 LNodeData->FuzzyStructure->ExecuteRecord(NInpDataSet, NOutDataSet, SNormTable.GetData(), k + LRecordIndex);
 } else {
	TNumaVector<double> SNormTable(ANodeIndex, LNodeData->FuzzyStructure->GetSNormTableCount());
	for (int k = 0; k < LRecordCount; ++k)
		 LNodeData->FuzzyStructure->ExecuteRecord(LInpDataSet, LOutDataSet, SNormTable.GetData(), k + LRecordIndex);
 }
}
//---------------------------------------------------------------------------
// Procedura wielow¹tkowa uruchamiana na pocz¹tku procesu uczenia dla ka¿dego w¹tku
//---------------------------------------------------------------------------
void __stdcall NumaUnitExecuteError(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 //odczytanie pomocniczych wskaŸników i utworzenie struktur danych
 TAIENodeData* LNodeData = (TAIENodeData*)AThreads->GetNodeData(ANodeIndex);
 TAIEUnitData* LUnitData = (TAIEUnitData*)AThreads->GetUnitData(ANodeIndex, AUnitIndex);

 int LThreadCount = AThreads->GetThreadCount();
 int LRecordCount = LNodeData->InpDataSet->GetRecordCount() / LThreadCount;
 int LRecordIndex = AThreadIndex * LRecordCount;
 if (AThreadIndex == LThreadCount - 1) LRecordCount += LNodeData->InpDataSet->GetRecordCount() % LThreadCount;

 memset(LUnitData->YErrors, 0, LNodeData->FuzzyStructure->GetErrorTableCount() * sizeof(double));
 memset(LUnitData->YOutputsInTol, 0, LNodeData->FuzzyStructure->GetErrorTableCount() * sizeof(int));
 LUnitData->VectorsInTol = 0;
 LUnitData->Error = 0.0;
 LUnitData->NormalizedError = 0.0;

 TNumaVector<double> SNormTable(ANodeIndex, LNodeData->FuzzyStructure->GetSNormTableCount());
 for (int k = 0; k < LRecordCount; ++k)
	  LUnitData->Error += LNodeData->FuzzyStructure->ExecuteRecordError(LNodeData->InpDataSet, LNodeData->OutDataSet, SNormTable.GetData(), k + LRecordIndex,
				LNodeData->YTols, LUnitData->YErrors, LUnitData->YOutputsInTol, LUnitData->VectorsInTol, LUnitData->NormalizedError);
}
//---------------------------------------------------------------------------
END_NAMESPACE(NAIELinguisticFuzzyStructure)
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyOptions
//---------------------------------------------------------------------------
using namespace NAIELinguisticFuzzyStructure;
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyOptions::TAIELinguisticFuzzyOptions(void)
{
 FDefaultMembershipFunction = fuzzyMembershipFunction1;
 FDefaultCoreEnabled = false;
 FDefaultContrastEnabled = false;
 FDefaultFuzzySetCount = 3;
 FIntegralProbeCount = 2000;
 FDefaultTNormType = fuzzyTNorm1;
 FDefaultSNormType = fuzzySNorm1;
 FDefaultImplicationOperator = fuzzyImplicationOperator1;
 FDefaultAggregationOperator = fuzzyAggregationOperator1;
 FDefaultInferenceMode = fuzzyInferenceModeFATI;
 FDefaultDefuzzificationMethod = fuzzyDefuzzificationCOG;
 FDefaultSFP = false;
}
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyOptions::~TAIELinguisticFuzzyOptions(void)
{
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::Assign(TAIELinguisticFuzzyOptions* Source)
{
 FDefaultMembershipFunction = Source->FDefaultMembershipFunction;
 FDefaultCoreEnabled = Source->FDefaultCoreEnabled;
 FDefaultContrastEnabled = Source->FDefaultContrastEnabled;
 FDefaultFuzzySetCount = Source->FDefaultFuzzySetCount;
 FDefaultTNormType = Source->FDefaultTNormType;
 FDefaultSNormType = Source->FDefaultSNormType;
 FDefaultImplicationOperator = Source->FDefaultImplicationOperator;
 FDefaultAggregationOperator = Source->FDefaultAggregationOperator;
 FDefaultInferenceMode = Source->FDefaultInferenceMode;
 FDefaultDefuzzificationMethod = Source->FDefaultDefuzzificationMethod;
 FIntegralProbeCount = Source->FIntegralProbeCount;
 FDefaultSFP = Source->FDefaultSFP;
}
//---------------------------------------------------------------------------
TAIEMembershipFunction __stdcall TAIELinguisticFuzzyOptions::GetDefaultMembershipFunction(void)
{
 return FDefaultMembershipFunction;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::SetDefaultMembershipFunction(TAIEMembershipFunction ADefaultMembershipFunction)
{
 if (FDefaultMembershipFunction == ADefaultMembershipFunction) return;
 FDefaultMembershipFunction = ADefaultMembershipFunction;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyOptions::GetDefaultCoreEnabled(void)
{
 return FDefaultCoreEnabled;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::SetDefaultCoreEnabled(bool ADefaultCoreEnabled)
{
 if (FDefaultCoreEnabled == ADefaultCoreEnabled) return;
 FDefaultCoreEnabled = ADefaultCoreEnabled;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyOptions::GetDefaultContrastEnabled(void)
{
 return FDefaultContrastEnabled;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::SetDefaultContrastEnabled(bool ADefaultContrastEnabled)
{
 if (FDefaultContrastEnabled == ADefaultContrastEnabled) return;
 FDefaultContrastEnabled = ADefaultContrastEnabled;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyOptions::GetDefaultSFP(void)
{
 return FDefaultSFP;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::SetDefaultSFP(bool ADefaultSFP)
{
 if (FDefaultSFP == ADefaultSFP) return;
 FDefaultSFP = ADefaultSFP;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyOptions::GetDefaultFuzzySetCount(void)
{
 return FDefaultFuzzySetCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::SetDefaultFuzzySetCount(int ADefaultFuzzySetCount)
{
 if (FDefaultFuzzySetCount == ADefaultFuzzySetCount) return;
 FDefaultFuzzySetCount = ADefaultFuzzySetCount;
}
//---------------------------------------------------------------------------
TAIEFuzzyTNormType __stdcall TAIELinguisticFuzzyOptions::GetDefaultTNormType(void)
{
 return FDefaultTNormType;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::SetDefaultTNormType(TAIEFuzzyTNormType ADefaultTNormType)
{
 if (FDefaultTNormType == ADefaultTNormType) return;
 FDefaultTNormType = ADefaultTNormType;
}
//---------------------------------------------------------------------------
TAIEFuzzySNormType __stdcall TAIELinguisticFuzzyOptions::GetDefaultSNormType(void)
{
 return FDefaultSNormType;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::SetDefaultSNormType(TAIEFuzzySNormType ADefaultSNormType)
{
 if (FDefaultSNormType == ADefaultSNormType) return;
 FDefaultSNormType = ADefaultSNormType;
}
//---------------------------------------------------------------------------
TAIEInferenceMode __stdcall TAIELinguisticFuzzyOptions::GetDefaultInferenceMode(void)
{
 return FDefaultInferenceMode;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::SetDefaultInferenceMode(TAIEInferenceMode ADefaultInferenceMode)
{
 if (FDefaultInferenceMode == ADefaultInferenceMode) return;
 FDefaultInferenceMode = ADefaultInferenceMode;
 if (FDefaultInferenceMode == fuzzyInferenceModeRAW) {
	 FDefaultImplicationOperator = fuzzyImplicationOperator1;
	 FDefaultAggregationOperator = fuzzyAggregationOperator1;
	 FDefaultDefuzzificationMethod = fuzzyDefuzzificationCOA;
 }
}
//---------------------------------------------------------------------------
TAIEImplicationOperator __stdcall TAIELinguisticFuzzyOptions::GetDefaultImplicationOperator(void)
{
 if (FDefaultInferenceMode == fuzzyInferenceModeRAW) return fuzzyImplicationOperator1;
 return FDefaultImplicationOperator;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::SetDefaultImplicationOperator(TAIEImplicationOperator ADefaultImplicationOperator)
{
 if (FDefaultImplicationOperator == ADefaultImplicationOperator) return;
 if (FDefaultInferenceMode == fuzzyInferenceModeRAW) return;
 FDefaultImplicationOperator = ADefaultImplicationOperator;
}
//---------------------------------------------------------------------------
TAIEAggregationOperator __stdcall TAIELinguisticFuzzyOptions::GetDefaultAggregationOperator(void)
{
 if (FDefaultInferenceMode == fuzzyInferenceModeRAW) return fuzzyAggregationOperator1;
 return FDefaultAggregationOperator;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::SetDefaultAggregationOperator(TAIEAggregationOperator ADefaultAggregationOperator)
{
 if (FDefaultAggregationOperator == ADefaultAggregationOperator) return;
 if (FDefaultInferenceMode == fuzzyInferenceModeRAW) return;
 FDefaultAggregationOperator = ADefaultAggregationOperator;
}
//---------------------------------------------------------------------------
TAIEDefuzzificationMethod __stdcall TAIELinguisticFuzzyOptions::GetDefaultDefuzzificationMethod(void)
{
 if (FDefaultInferenceMode == fuzzyInferenceModeRAW) return (TAIEDefuzzificationMethod)-1;
 return FDefaultDefuzzificationMethod;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::SetDefaultDefuzzificationMethod(TAIEDefuzzificationMethod ADefaultDefuzzificationMethod)
{
 if (FDefaultDefuzzificationMethod == ADefaultDefuzzificationMethod) return;
 if (FDefaultInferenceMode == fuzzyInferenceModeRAW) return;
 FDefaultDefuzzificationMethod = ADefaultDefuzzificationMethod;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyOptions::GetIntegralProbeCount(void)
{
 return FIntegralProbeCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::SetIntegralProbeCount(int AIntegralProbeCount)
{
 if (AIntegralProbeCount < 500) return;
 FIntegralProbeCount = AIntegralProbeCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::SaveToFile(IAIEBinaryFile* File)
{
 File->SaveString("FUZZY_STRUCTUREOPTIONS_MARK");
 File->SaveInteger(FDefaultMembershipFunction);
 File->SaveBoolean(FDefaultCoreEnabled);
 File->SaveBoolean(FDefaultContrastEnabled);
 File->SaveInteger(FDefaultFuzzySetCount);
 File->SaveInteger(FDefaultTNormType);
 File->SaveInteger(FDefaultSNormType);
 File->SaveInteger(FDefaultImplicationOperator);
 File->SaveInteger(FDefaultAggregationOperator);
 File->SaveInteger(FDefaultInferenceMode);
 File->SaveInteger(FDefaultDefuzzificationMethod);
 File->SaveBoolean(FDefaultSFP);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyOptions::LoadFromFile(IAIEBinaryFile* File)
{
 File->CheckMark("FUZZY_STRUCTUREOPTIONS_MARK");
 FDefaultMembershipFunction = (TAIEMembershipFunction)File->LoadInteger();
 FDefaultCoreEnabled = File->LoadBoolean();
 FDefaultContrastEnabled = File->LoadBoolean();
 FDefaultFuzzySetCount = File->LoadInteger();
 FDefaultTNormType = (TAIEFuzzyTNormType)File->LoadInteger();
 FDefaultSNormType = (TAIEFuzzySNormType)File->LoadInteger();
 FDefaultImplicationOperator = (TAIEImplicationOperator)File->LoadInteger();
 FDefaultAggregationOperator = (TAIEAggregationOperator)File->LoadInteger();
 FDefaultInferenceMode = (TAIEInferenceMode)File->LoadInteger();
 FDefaultDefuzzificationMethod = (TAIEDefuzzificationMethod)File->LoadInteger();
 FDefaultSFP = File->LoadBoolean();
}
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyAttr
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyAttr::TAIELinguisticFuzzyAttr(int ANumaNodeIndex, int AFuzzyRulebaseCount,
				const char* AName, TAIEColumnKind AKind, TAIEColumnType AType, double AMinValue, double AMaxValue)
{
 FNumaNodeIndex = ANumaNodeIndex;
 FAssignedFuzzyRuleCount.SetNumaNodeIndex(ANumaNodeIndex);
 FAssignedFuzzyRuleCount.Resize(AFuzzyRulebaseCount);

 FName = strdup(AName);
 FKind = AKind;
 FType = AType;
 FMinValue = AMinValue;
 FMaxValue = AMaxValue;
}
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyAttr::~TAIELinguisticFuzzyAttr(void)
{
 FAssignedFuzzyRuleCount.Resize(0);
 free(FName);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyAttr::Assign(TAIELinguisticFuzzyAttr* Source)
{
 free(FName); FName = strdup(Source->FName);
 FKind = Source->FKind;
 FType = Source->FType;
 FMinValue = Source->FMinValue;
 FMaxValue = Source->FMaxValue;
}
//---------------------------------------------------------------------------
const char* __stdcall TAIELinguisticFuzzyAttr::GetName(void)
{
 return FName;
}
//---------------------------------------------------------------------------
TAIEColumnKind __stdcall TAIELinguisticFuzzyAttr::GetKind(void)
{
 return FKind;
}
//---------------------------------------------------------------------------
TAIEColumnType __stdcall TAIELinguisticFuzzyAttr::GetType(void)
{
 return FType;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyAttr::GetIsNominalType(void)
{
 return (FType == ctString) || (FType == ctBoolean);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyAttr::GetMinValue(void)
{
 return FMinValue;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyAttr::GetMaxValue(void)
{
 return FMaxValue;
}
//---------------------------------------------------------------------------
const char* __stdcall TAIELinguisticFuzzyAttr::GetTypeName(void)
{
 if (FType == ctInteger) return "liczba ca³kowita";
 if (FType == ctDouble) return "liczba rzeczywista";
 if (FType == ctBoolean) return "wartoœæ logiczna";
 return "ci¹g znaków";
}
//---------------------------------------------------------------------------
const char* __stdcall TAIELinguisticFuzzyAttr::GetKindName(void)
{
 if (FKind == ckOutput) return "wyjœciowy";
 if (FKind == ckInput) return "wejœciowy";
 return "komentarz";
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyAttr::GetAssignedFuzzyRuleCount(int AFuzzyRulebaseIndex)
{
 //liczba regu³ wykorzystuj¹cych atrybut w bazie regu³ o indeksie AFuzzyRulebaseIndex
 return FAssignedFuzzyRuleCount[AFuzzyRulebaseIndex];
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyAttr::GetIsActive(void)
{
 int LCount = FAssignedFuzzyRuleCount.GetCount();
 while (LCount--) if (FAssignedFuzzyRuleCount[LCount] != 0) return true;
 return false;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyAttr::SaveToFile(IAIEBinaryFile* File)
{
 File->SaveDouble(FMinValue);
 File->SaveDouble(FMaxValue);
 File->SaveString(FName);
 File->SaveInteger(FType);
 File->SaveInteger(FKind);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyAttr::LoadFromFile(IAIEBinaryFile* File)
{
 FMinValue = File->LoadDouble();
 FMaxValue = File->LoadDouble();
 free(FName); FName = strdup(File->LoadString());
 FType = (TAIEColumnType)File->LoadInteger();
 FKind = (TAIEColumnKind)File->LoadInteger();
}
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyStructure
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyStructure::TAIELinguisticFuzzyStructure(TAIESystem* ASystem, int ANumaNodeIndex, bool OwnThread)
{
 FSystem = ASystem;
 FNumaNodeIndex = ANumaNodeIndex;
 FOptions = new TAIELinguisticFuzzyOptions();

 FInputFuzzyAttr.SetNumaNodeIndex(ANumaNodeIndex);
 FOutputFuzzyAttr.SetNumaNodeIndex(ANumaNodeIndex);
 FFuzzyCollection.SetNumaNodeIndex(ANumaNodeIndex);
 FCurrentFuzzyCollection.SetNumaNodeIndex(ANumaNodeIndex);
 FEnabledFuzzyCollection.SetNumaNodeIndex(ANumaNodeIndex);
 FInputFuzzyAttr.Resize(0);
 FOutputFuzzyAttr.Resize(0);
 FFuzzyCollection.Resize(0);
 FCurrentFuzzyCollection.Resize(0);
 FEnabledFuzzyCollection.Resize(0);

 FMaxFuzzyRuleCount = 1;
 FMinFuzzyRuleCount = 1;
 FSNormTableCount = 0;
 FErrorTableCount = 0;
 FFuzzyRulebase.SetNumaNodeIndex(ANumaNodeIndex);
 FFuzzyRulebase.Resize(0);

 FIsInitialized = false;

 //Tworzenie w¹tków przeliczaj¹cych odpowiedŸ sieci
 if (OwnThread) {
	FThreads = FSystem->InitNumaThreads();
	FThreads->SetData(this);
 } else {
	FThreads = NULL;
 }
}
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyStructure::~TAIELinguisticFuzzyStructure(void)
{
 if (FThreads) {
	FThreads->Free();
 }

 Clear();
 delete FOptions;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::Free(void)
{
 delete this;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::Assign(IAIELinguisticFuzzyStructure* Source)
{
 TAIELinguisticFuzzyStructure* LSource = dynamic_cast<TAIELinguisticFuzzyStructure*>(Source);
 FOptions->Assign(LSource->FOptions);

 Clear();

 SetFuzzyAttrCount(LSource->FInputFuzzyAttr.GetCount(), LSource->FOutputFuzzyAttr.GetCount());

 // - kopiowanie atrybutów wejœciowych i wyjœciowych oraz zestawów zbiorów rozmytych
 for (int i = 0; i < FInputFuzzyAttr.GetCount(); ++i) {
	  FInputFuzzyAttr[i].Assign(&(LSource->FInputFuzzyAttr[i]));
	  FFuzzyCollection[i].Assign(&(LSource->FFuzzyCollection[i]));
 }
 for (int i = 0, j = FInputFuzzyAttr.GetCount(); i < FOutputFuzzyAttr.GetCount(); ++i, ++j) {
	  FOutputFuzzyAttr[i].Assign(&(LSource->FOutputFuzzyAttr[i]));
	  FFuzzyCollection[j].Assign(&(LSource->FFuzzyCollection[j]));
 }

 // - ustawianie zestawów bie¿¹cych i aktywnych
 FCurrentFuzzyCollection.Resize(LSource->FCurrentFuzzyCollection.GetCount());
 FEnabledFuzzyCollection.Resize(LSource->FEnabledFuzzyCollection.GetCount());
 for (int i = 0; i < FCurrentFuzzyCollection.GetCount(); ++i) {
	  const char* LName = LSource->FCurrentFuzzyCollection[i]->GetName();
	  FCurrentFuzzyCollection[i] = dynamic_cast<TAIELinguisticFuzzyCollection*>(GetFuzzyCollectionByName(LName));
 }
 for (int i = 0; i < FEnabledFuzzyCollection.GetCount(); ++i) {
	  const char* LName = LSource->FEnabledFuzzyCollection[i]->GetName();
	  FEnabledFuzzyCollection[i] = dynamic_cast<TAIELinguisticFuzzyCollection*>(GetFuzzyCollectionByName(LName));
 }

 // - kopiowanie baz regu³
 SetFuzzyRulebaseCount(LSource->FFuzzyRulebase.GetCount());
 for (int i = 0; i < FFuzzyRulebase.GetCount(); ++i)   FFuzzyRulebase[i].Assign(&(LSource->FFuzzyRulebase[i]));

 FMaxFuzzyRuleCount = LSource->FMaxFuzzyRuleCount;
 FMinFuzzyRuleCount = LSource->FMinFuzzyRuleCount;
 FIsInitialized = LSource->FIsInitialized;
 FTag = LSource->FTag;

 PrepareFuzzyRulebase();
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyStructure* __stdcall TAIELinguisticFuzzyStructure::Clone(void)
{
 TAIELinguisticFuzzyStructure* LSystem = new TAIELinguisticFuzzyStructure(FSystem, FNumaNodeIndex);
 LSystem->Assign(this);
 return LSystem;
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyOptions* __stdcall TAIELinguisticFuzzyStructure::GetOptions(void)
{
 return FOptions;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructure::GetIsInitialized(void)
{
 return FIsInitialized;
}
//---------------------------------------------------------------------------
int  __stdcall TAIELinguisticFuzzyStructure::GetMaxFuzzyRuleCount(void)
{
 return FMaxFuzzyRuleCount;
}
//---------------------------------------------------------------------------
int  __stdcall TAIELinguisticFuzzyStructure::GetMinFuzzyRuleCount(void)
{
 return FMinFuzzyRuleCount;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructure::GetFuzzyCollectionCount(void)
{
 return FFuzzyCollection.GetCount();
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyCollection* __stdcall TAIELinguisticFuzzyStructure::GetFuzzyCollection(int AIndex)
{
 return &FFuzzyCollection[AIndex];
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyCollection* __stdcall TAIELinguisticFuzzyStructure::GetFuzzyCollectionByName(const char* AName)
{
 for (int i = 0; i < FFuzzyCollection.GetCount(); ++i)
	  if (!strcmp(FFuzzyCollection[i].GetName(), AName)) return &FFuzzyCollection[i];
 return NULL;
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyCollection* __stdcall TAIELinguisticFuzzyStructure::GetCurrentFuzzyCollection(int AIndex)
{
 return FCurrentFuzzyCollection[AIndex];
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyCollection* __stdcall TAIELinguisticFuzzyStructure::GetInputFuzzyCollection(int AIndex)
{
 return FCurrentFuzzyCollection[AIndex];
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyCollection* __stdcall TAIELinguisticFuzzyStructure::GetOutputFuzzyCollection(int AIndex)
{
 return FCurrentFuzzyCollection[AIndex + FInputFuzzyAttr.GetCount()];
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::SetCurrentFuzzyCollection(int AIndex, IAIELinguisticFuzzyCollection* AFuzzyCollection)
{
 TAIELinguisticFuzzyCollection* LCurrentFuzzyCollection = dynamic_cast<TAIELinguisticFuzzyCollection*>(AFuzzyCollection);
 if (!LCurrentFuzzyCollection) FSystem->RaiseException("Nie mo¿na zmieniæ zestawu zbiorów rozmytych!");

 if (FCurrentFuzzyCollection[AIndex] == LCurrentFuzzyCollection) return;
 if (FCurrentFuzzyCollection[AIndex]->GetIsNominalType())
	 FSystem->RaiseException("Nie mo¿na zmieniæ zestawu zbiorów rozmytych atrybutu typu \"" + AnsiString(FCurrentFuzzyCollection[AIndex]->GetTypeName()) + "\"!");

 if (FCurrentFuzzyCollection[AIndex]->GetType() != LCurrentFuzzyCollection->GetType())
	FSystem->RaiseException("Nie mo¿na zmieniæ zestawu zbiorów rozmytych!\n"
							"Typ danych atrybutu jest ró¿ny od typu danych zestawu zbiorów rozmytych.");
 if (FCurrentFuzzyCollection[AIndex]->GetMinValue() != LCurrentFuzzyCollection->GetMinValue())
	FSystem->RaiseException("Nie mo¿na zmieniæ zestawu zbiorów rozmytych!\n"
							"Zakresy dopuszczalnych wartoœci atrybutów \"" + AnsiString(LCurrentFuzzyCollection->GetName()) + "\" i \"" + AnsiString(FCurrentFuzzyCollection[AIndex]->GetName()) + "\" s¹ ró¿ne.");
 if (FCurrentFuzzyCollection[AIndex]->GetMaxValue() != LCurrentFuzzyCollection->GetMaxValue())
	FSystem->RaiseException("Nie mo¿na zmieniæ zestawu zbiorów rozmytych!\n"
							"Zakresy dopuszczalnych wartoœci atrybutów \"" + AnsiString(LCurrentFuzzyCollection->GetName()) + "\" i \"" + AnsiString(FCurrentFuzzyCollection[AIndex]->GetName()) + "\" s¹ ró¿ne.");

 FCurrentFuzzyCollection[AIndex] = LCurrentFuzzyCollection;

 // - aktualizacja tablicy FEnabledFuzzyCollection
 int LEnabledCount = 0;
 for (int i = 0; i < FFuzzyCollection.GetCount(); ++i) if (&FFuzzyCollection[i] == FCurrentFuzzyCollection[i]) ++LEnabledCount;
 FEnabledFuzzyCollection.Resize(LEnabledCount);
 for (int i = 0, j = 0; i < FFuzzyCollection.GetCount(); ++i) if (&FFuzzyCollection[i] == FCurrentFuzzyCollection[i]) FEnabledFuzzyCollection[j++] = FCurrentFuzzyCollection[i];

 PrepareFuzzyRulebase();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::SetFuzzyAttrCount(int AInputCount, int AOutputCount)
{
 for (int i = 0; i < FInputFuzzyAttr.GetCount(); ++i)  FInputFuzzyAttr[i].~TAIELinguisticFuzzyAttr();
 for (int i = 0; i < FOutputFuzzyAttr.GetCount(); ++i) FOutputFuzzyAttr[i].~TAIELinguisticFuzzyAttr();
 for (int i = 0; i < FFuzzyCollection.GetCount(); ++i) FFuzzyCollection[i].~TAIELinguisticFuzzyCollection();

 FInputFuzzyAttr.Resize(AInputCount);
 FOutputFuzzyAttr.Resize(AOutputCount);
 FFuzzyCollection.Resize(AInputCount + AOutputCount);
 FCurrentFuzzyCollection.Resize(FFuzzyCollection.GetCount());
 FEnabledFuzzyCollection.Resize(FFuzzyCollection.GetCount());

 // - budowanie atrybutów wejœciowych
 for (int i = 0; i < AInputCount; ++i) {
	  // -- tworzenie atrybutu i kolekcji zbiorów rozmytych
	  new (&FInputFuzzyAttr[i]) TAIELinguisticFuzzyAttr(FNumaNodeIndex, AOutputCount, "", ckInput, ctDouble, 0.0, 0.0);
	  new (&FFuzzyCollection[i]) TAIELinguisticFuzzyCollection(this, FNumaNodeIndex, i, "", ckInput, ctDouble, 0.0, 0.0, FOptions->GetDefaultMembershipFunction(), FOptions->GetDefaultCoreEnabled(), FOptions->GetDefaultContrastEnabled(), FOptions->GetDefaultSFP());
	  FCurrentFuzzyCollection[i] = &FFuzzyCollection[i];
	  FEnabledFuzzyCollection[i] = &FFuzzyCollection[i];
 }

 // - budowanie atrybutów wyjœciowych
 for (int i = 0, j = AInputCount; i < AOutputCount; ++i, ++j) {
	  // -- tworzenie atrybutu i kolekcji zbiorów rozmytych
	  new (&FOutputFuzzyAttr[i]) TAIELinguisticFuzzyAttr(FNumaNodeIndex, AOutputCount, "", ckOutput, ctDouble, 0.0, 0.0);
	  new (&FFuzzyCollection[j]) TAIELinguisticFuzzyCollection(this, FNumaNodeIndex, j, "", ckOutput, ctDouble, 0.0, 0.0, FOptions->GetDefaultMembershipFunction(), FOptions->GetDefaultCoreEnabled(), FOptions->GetDefaultContrastEnabled(), FOptions->GetDefaultSFP());
	  FCurrentFuzzyCollection[j] = &FFuzzyCollection[j];
	  FEnabledFuzzyCollection[j] = &FFuzzyCollection[j];
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::SetFuzzyRulebaseCount(int ACount)
{
 int LCount = FFuzzyRulebase.GetCount();
 if (LCount > ACount) for (int i = ACount; i < LCount; ++i) FFuzzyRulebase[i].~TAIELinguisticFuzzyRulebase();
 FFuzzyRulebase.Resize(ACount);
 if (LCount < ACount) for (int i = LCount; i < ACount; ++i) new (&FFuzzyRulebase[i]) TAIELinguisticFuzzyRulebase(this,
																  FSystem, FNumaNodeIndex, FInputFuzzyAttr.GetCount(),
																  i, FOutputFuzzyAttr[i].GetType(),
																  FOptions->GetDefaultTNormType(), FOptions->GetDefaultSNormType(),
																  FOptions->GetDefaultImplicationOperator(), FOptions->GetDefaultAggregationOperator(),
																  FOptions->GetDefaultInferenceMode(), FOptions->GetDefaultDefuzzificationMethod(),
																  FInputFuzzyAttr, FOutputFuzzyAttr, FCurrentFuzzyCollection, FOptions->FIntegralProbeCount);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::PrepareFuzzyRulebase(void)
{
 if (!FIsInitialized) return;

 for (int j = 0; j < FInputFuzzyAttr.GetCount(); ++j) FInputFuzzyAttr[j].FAssignedFuzzyRuleCount.Clear();
 for (int j = 0; j < FOutputFuzzyAttr.GetCount(); ++j) FOutputFuzzyAttr[j].FAssignedFuzzyRuleCount.Clear();

 for (int i = 0; i < FEnabledFuzzyCollection.GetCount(); ++i) {
	  TAIELinguisticFuzzyCollection* LEnabledFuzzyCollection = FEnabledFuzzyCollection[i];
	  for (int j = 0; j < LEnabledFuzzyCollection->GetFuzzySetCount(); ++j)
		   LEnabledFuzzyCollection->FFuzzySet[j].FAssignedFuzzyAntecedentCount.Clear();
 }

 FSNormTableCount = 0;
 FErrorTableCount = 0;
 for (int i = 0; i < FFuzzyRulebase.GetCount(); ++i) {
	  FFuzzyRulebase[i].PrepareFuzzyRulebase();
	  FSNormTableCount += FCurrentFuzzyCollection[FFuzzyRulebase[i].GetFuzzyAttrIndex()]->GetFuzzySetCount();
	  FErrorTableCount += FFuzzyRulebase[i].GetIsNominalType() ? FCurrentFuzzyCollection[FFuzzyRulebase[i].GetFuzzyAttrIndex()]->GetFuzzySetCount() : 1;
 }
}
//---------------------------------------------------------------------------
int  __stdcall TAIELinguisticFuzzyStructure::GetSNormTableCount(void)
{
 return FSNormTableCount;
}
//---------------------------------------------------------------------------
int  __stdcall TAIELinguisticFuzzyStructure::GetErrorTableCount(void)
{
 return FErrorTableCount;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructure::GetEnabledFuzzyCollectionCount(void)
{
 return FEnabledFuzzyCollection.GetCount();
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyCollection* __stdcall TAIELinguisticFuzzyStructure::GetEnabledFuzzyCollection(int AIndex)
{
 return FEnabledFuzzyCollection[AIndex];
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructure::GetInputFuzzyAttrCount(void)
{
 return FInputFuzzyAttr.GetCount();
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyAttr* __stdcall TAIELinguisticFuzzyStructure::GetInputFuzzyAttr(int AIndex)
{
 return &(FInputFuzzyAttr[AIndex]);
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructure::GetOutputFuzzyAttrCount(void)
{
 return FOutputFuzzyAttr.GetCount();
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyAttr* __stdcall TAIELinguisticFuzzyStructure::GetOutputFuzzyAttr(int AIndex)
{
 return &(FOutputFuzzyAttr[AIndex]);
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructure::GetFuzzyRulebaseCount(void)
{
 return FFuzzyRulebase.GetCount();
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyRulebase* __stdcall TAIELinguisticFuzzyStructure::GetFuzzyRulebase(int AIndex)
{
 return &FFuzzyRulebase[AIndex];
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructure::GetTag(void)
{
 return FTag;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::SetTag(int ATag)
{
 FTag = ATag;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::SaveToFile(IAIEBinaryFile* File)
{
 File->SaveString("FUZZY_OPTIONS_MARK");
 FOptions->SaveToFile(File);

 File->SaveString("FUZZY_DATABASE_MARK");
 File->SaveInteger(FInputFuzzyAttr.GetCount());
 File->SaveInteger(FOutputFuzzyAttr.GetCount());
 for (int i = 0; i < FInputFuzzyAttr.GetCount(); ++i)  FInputFuzzyAttr[i].SaveToFile(File);
 for (int i = 0; i < FOutputFuzzyAttr.GetCount(); ++i) FOutputFuzzyAttr[i].SaveToFile(File);
 for (int i = 0; i < FFuzzyCollection.GetCount(); ++i) FFuzzyCollection[i].SaveToFile(File);

 File->SaveInteger(FCurrentFuzzyCollection.GetCount());
 File->SaveInteger(FEnabledFuzzyCollection.GetCount());
 for (int i = 0; i < FCurrentFuzzyCollection.GetCount(); ++i) File->SaveInteger(FCurrentFuzzyCollection[i]->GetIndex());
 for (int i = 0; i < FEnabledFuzzyCollection.GetCount(); ++i) File->SaveInteger(FEnabledFuzzyCollection[i]->GetIndex());

 File->SaveString("FUZZY_STRUCTURE_MARK");
 File->SaveInteger(FFuzzyRulebase.GetCount());
 for (int i = 0; i < FFuzzyRulebase.GetCount(); ++i) FFuzzyRulebase[i].SaveToFile(File);

 File->SaveInteger(FMinFuzzyRuleCount);
 File->SaveInteger(FMaxFuzzyRuleCount);
 File->SaveBoolean(FIsInitialized);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::LoadFromFile(IAIEBinaryFile* File)
{
 File->CheckMark("FUZZY_OPTIONS_MARK");
 FOptions->LoadFromFile(File);

 File->CheckMark("FUZZY_DATABASE_MARK");
 int LInputCount = File->LoadInteger();
 int LOutputCount = File->LoadInteger();
 SetFuzzyAttrCount(LInputCount, LOutputCount);
 for (int i = 0; i < FInputFuzzyAttr.GetCount(); ++i)  FInputFuzzyAttr[i].LoadFromFile(File);
 for (int i = 0; i < FOutputFuzzyAttr.GetCount(); ++i) FOutputFuzzyAttr[i].LoadFromFile(File);
 for (int i = 0; i < FFuzzyCollection.GetCount(); ++i) FFuzzyCollection[i].LoadFromFile(File);

 FCurrentFuzzyCollection.Resize(File->LoadInteger());
 FEnabledFuzzyCollection.Resize(File->LoadInteger());
 for (int i = 0; i < FCurrentFuzzyCollection.GetCount(); ++i) FCurrentFuzzyCollection[i] = &FFuzzyCollection[File->LoadInteger()];
 for (int i = 0; i < FEnabledFuzzyCollection.GetCount(); ++i) FEnabledFuzzyCollection[i] = &FFuzzyCollection[File->LoadInteger()];

 File->CheckMark("FUZZY_STRUCTURE_MARK");
 SetFuzzyRulebaseCount(File->LoadInteger());
 for (int i = 0; i < FFuzzyRulebase.GetCount(); ++i) FFuzzyRulebase[i].LoadFromFile(File);

 FMinFuzzyRuleCount = File->LoadInteger();
 FMaxFuzzyRuleCount = File->LoadInteger();
 FIsInitialized = File->LoadBoolean();

 PrepareFuzzyRulebase();
}
//---------------------------------------------------------------------------
// Wyznaczanie z³o¿onoœci systemu
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructure::GetFuzzySystemComplexity(void)
{
 // - œrednia arytmetyczna z³o¿onoœci wszystkich baz regu³
 double LFuzzySystemComplexity = 0.0;
 for (int i = 0; i < FFuzzyRulebase.GetCount(); ++i) LFuzzySystemComplexity += FFuzzyRulebase[i].GetFuzzyRulebaseComplexity();
 LFuzzySystemComplexity /= FFuzzyRulebase.GetCount();
 if (FFuzzyRulebase.GetCount() > 1) (LFuzzySystemComplexity += GetNormEnabledFuzzyAttrCount() + GetNormEnabledFuzzySetCount()) /= 3.0;
 return LFuzzySystemComplexity;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructure::GetEnabledInpFuzzyAttrCount(void)
{
 int LCount = 0;
 for (int i = 0; i < FInputFuzzyAttr.GetCount(); ++i) {
	  int* LAssignedFuzzyRuleCount = FInputFuzzyAttr[i].FAssignedFuzzyRuleCount.GetData();
	  for (int k = 0; k < FFuzzyRulebase.GetCount(); ++k) if (LAssignedFuzzyRuleCount[k] != 0) {++LCount; break;}
 }
 return LCount;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructure::GetEnabledOutFuzzyAttrCount(void)
{
 int LCount = 0;
 for (int i = 0; i < FOutputFuzzyAttr.GetCount(); ++i) {
	  int* LAssignedFuzzyRuleCount = FOutputFuzzyAttr[i].FAssignedFuzzyRuleCount.GetData();
	  for (int k = 0; k < FFuzzyRulebase.GetCount(); ++k) if (LAssignedFuzzyRuleCount[k] != 0) {++LCount; break;}
 }
 return LCount;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructure::GetEnabledFuzzyRuleCount(void)
{
 int LFuzzyRuleCount = 0;
 for (int k = 0; k < FFuzzyRulebase.GetCount(); ++k) LFuzzyRuleCount += FFuzzyRulebase[k].GetEnabledFuzzyRuleCount();
 return LFuzzyRuleCount;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructure::GetEnabledFuzzySetCount(void)
{
 int LInpFuzzyAttrCount =  FInputFuzzyAttr.GetCount();
 int LCount = 0;

 for (int i = 0; i < FEnabledFuzzyCollection.GetCount(); ++i) {
	  TAIELinguisticFuzzyCollection* LFuzzyCollection = FEnabledFuzzyCollection[i];
	  if (LFuzzyCollection->GetIsNominalType() && (i >= LInpFuzzyAttrCount)) continue;
	  for (int j = 0; j < LFuzzyCollection->FFuzzySet.GetCount(); ++j) {
		   int* LAssignedFuzzyAntecedentCount = LFuzzyCollection->FFuzzySet[j].FAssignedFuzzyAntecedentCount.GetData();
		   for (int k = 0; k < FFuzzyRulebase.GetCount(); ++k) if (LAssignedFuzzyAntecedentCount[k] != 0) {++LCount; break;}
	  }
 }

//na potrzeby klasyfikacji ECG
/*
 if (FEnabledFuzzyCollection.GetCount()) {
	  TAIELinguisticFuzzyCollection* LFuzzyCollection = FEnabledFuzzyCollection[0];
	  for (int j = 0; j < LFuzzyCollection->FFuzzySet.GetCount(); ++j) {
		   int* LAssignedFuzzyAntecedentCount = LFuzzyCollection->FFuzzySet[j].FAssignedFuzzyAntecedentCount.GetData();
		   for (int k = 0; k < FFuzzyRulebase.GetCount(); ++k) if (LAssignedFuzzyAntecedentCount[k] != 0) {++LCount; break;}
	  }
 }
*/
 return LCount;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructure::GetNormEnabledFuzzyAttrCount(void)
{
 // Normalizowana liczba w³¹czonych wejœæ
 int LInpFuzzyAttrCount =  FInputFuzzyAttr.GetCount();
 if (LInpFuzzyAttrCount == 1) return 0.0;

 double LNormEnabledFuzzyAttrCount = (LInpFuzzyAttrCount > 1) ? ((double)(GetEnabledInpFuzzyAttrCount() - 1) / (double)(LInpFuzzyAttrCount - 1)) : 0.0;
 if (LNormEnabledFuzzyAttrCount < 0.0) LNormEnabledFuzzyAttrCount = 1.0;

 return LNormEnabledFuzzyAttrCount;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructure::GetNormEnabledFuzzySetCount(void)
{
 // Normalizowana liczba wykorzystanych zbiorów rozmytych
 int LInpFuzzyAttrCount =  FInputFuzzyAttr.GetCount();
 double LFuzzySetCount = 0.0;
 double LNormEnabledFuzzySetCount = 0.0;

 for (int i = 0; i < FEnabledFuzzyCollection.GetCount(); ++i) {
	  TAIELinguisticFuzzyCollection* LFuzzyCollection = FEnabledFuzzyCollection[i];
	  if (LFuzzyCollection->GetIsNominalType() && (i >= LInpFuzzyAttrCount)) continue;
	  LFuzzySetCount += LFuzzyCollection->FFuzzySet.GetCount();
	  for (int j = 0; j < LFuzzyCollection->FFuzzySet.GetCount(); ++j) {
		   int* LAssignedFuzzyAntecedentCount = LFuzzyCollection->FFuzzySet[j].FAssignedFuzzyAntecedentCount.GetData();
		   for (int k = 0; k < FFuzzyRulebase.GetCount(); ++k) if (LAssignedFuzzyAntecedentCount[k] != 0) {++LNormEnabledFuzzySetCount; break;}
	  }
 }
 LNormEnabledFuzzySetCount = (LFuzzySetCount > 1.0) ? (LNormEnabledFuzzySetCount - 1.0) / (LFuzzySetCount - 1.0) : 0.0;
 if (LNormEnabledFuzzySetCount < 0.0) LNormEnabledFuzzySetCount = 1.0;

//Na potrzeby klasyfikacji ECG
/*
 if (FEnabledFuzzyCollection.GetCount()) {
	  TAIELinguisticFuzzyCollection* LFuzzyCollection = FEnabledFuzzyCollection[0];
	  LFuzzySetCount += LFuzzyCollection->FFuzzySet.GetCount();
	  for (int j = 0; j < LFuzzyCollection->FFuzzySet.GetCount(); ++j) {
		   int* LAssignedFuzzyAntecedentCount = LFuzzyCollection->FFuzzySet[j].FAssignedFuzzyAntecedentCount.GetData();
		   for (int k = 0; k < FFuzzyRulebase.GetCount(); ++k) if (LAssignedFuzzyAntecedentCount[k] != 0) {++LNormEnabledFuzzySetCount; break;}
	  }
 }
 LNormEnabledFuzzySetCount = (LFuzzySetCount > 1.0) ? (LNormEnabledFuzzySetCount - 1.0) / (LFuzzySetCount - 1.0) : 0.0;
 if (LNormEnabledFuzzySetCount < 0.0) LNormEnabledFuzzySetCount = 1.0;
*/

 return LNormEnabledFuzzySetCount;
}
//---------------------------------------------------------------------------
// Inicjowanie systemu
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::Initialize(IAIEDataSet* DataSet)
{
 Clear();

 // - aktualizacja statystyk zbioru danych odniesienia
 IAIEDataSetStats* LStats = DataSet->GetStats();
 LStats->Update();

 // - ustalenie górnego i dolnego ograniczenia liczby regu³ w systemie
 FMaxFuzzyRuleCount = DataSet->GetRecordCount();
 FMinFuzzyRuleCount = 1;

 // - ustalenie wymiarowoœci struktur
 FInputFuzzyAttr.Resize(DataSet->GetInputColumnCount());
 FOutputFuzzyAttr.Resize(DataSet->GetOutputColumnCount());
 FFuzzyCollection.Resize(FInputFuzzyAttr.GetCount() + FOutputFuzzyAttr.GetCount());
 FCurrentFuzzyCollection.Resize(FFuzzyCollection.GetCount());
 FEnabledFuzzyCollection.Resize(FFuzzyCollection.GetCount());

 // - budowanie atrybutów wejœciowych
 for (int i = 0; i < FInputFuzzyAttr.GetCount(); ++i) {
	  IAIEDataSetColumn* LColumn = DataSet->GetDataSetInputColumn(i);
	  int LColumnIndex = LColumn->GetIndex();
	  const char* LName = LColumn->GetName();
	  TAIEColumnKind LKind = LColumn->GetKind();
	  TAIEColumnType LType = LColumn->GetType();

	  if (LColumn->GetIsNominalType()) {
		  // -- atrybut nominalny (liczba zbiorów równa liczbie wartoœci lingwistycznych)
		  int LFuzzySetCount = LStats->GetStringValueCount(LColumnIndex);
		  double LMinValue = 0.0;
		  double LMaxValue = (double)LFuzzySetCount - 1.0;

		  // -- tworzenie atrybutu i kolekcji zbiorów rozmytych
		  new (&FInputFuzzyAttr[i]) TAIELinguisticFuzzyAttr(FNumaNodeIndex, DataSet->GetOutputColumnCount(), LName, LKind, LType, LMinValue, LMaxValue);
		  new (&FFuzzyCollection[i]) TAIELinguisticFuzzyCollection(this, FNumaNodeIndex, i, LName, LKind, LType, LMinValue, LMaxValue, fuzzyMembershipFunction0, false, false, false);
		  FCurrentFuzzyCollection[i] = &FFuzzyCollection[i];
		  FEnabledFuzzyCollection[i] = &FFuzzyCollection[i];

		  // -- tworzenie zbiorów rozmytych dla danych nominalnych
		  FFuzzyCollection[i].SetFuzzySetCount(LFuzzySetCount);
		  for (int k = 0; k < LFuzzySetCount; ++k) {
			   IAIELinguisticFuzzySet* LFuzzySet = FFuzzyCollection[i].GetFuzzySet(k);
			   LFuzzySet->SetName(LStats->GetStringValue(LColumnIndex, k));
			   LFuzzySet->SetLCenter(k);
			   LFuzzySet->SetRCenter(k);
		  }
	  } else {
		  // -- atrybut numeryczny (liczba zbiorów domyœlna)
		  int LFuzzySetCount = FOptions->GetDefaultFuzzySetCount();
		  double LMinValue = LStats->GetMinValue(LColumnIndex);
		  double LMaxValue = LStats->GetMaxValue(LColumnIndex);

// Na potrzeby klasyfikacji ECG
//		  double LMinValue = -6.5;
//		  double LMaxValue = +5.4;

		  // -- tworzenie atrybutu i kolekcji zbiorów rozmytych
		  new (&FInputFuzzyAttr[i]) TAIELinguisticFuzzyAttr(FNumaNodeIndex, DataSet->GetOutputColumnCount(), LName, LKind, LType, LMinValue, LMaxValue);
		  new (&FFuzzyCollection[i]) TAIELinguisticFuzzyCollection(this, FNumaNodeIndex, i, LName, LKind, LType, LMinValue, LMaxValue, FOptions->GetDefaultMembershipFunction(), FOptions->GetDefaultCoreEnabled(), FOptions->GetDefaultContrastEnabled(), FOptions->GetDefaultSFP());
		  FCurrentFuzzyCollection[i] = &FFuzzyCollection[i];
		  FEnabledFuzzyCollection[i] = &FFuzzyCollection[i];

		  // -- tworzenie zbiorów rozmytych dla danych numerycznych
		  FFuzzyCollection[i].SetFuzzySetCount(LFuzzySetCount);
	  }
 }

 // - budowanie atrybutów wyjœciowych
 for (int i = 0, j = FInputFuzzyAttr.GetCount(); i < FOutputFuzzyAttr.GetCount(); ++i, ++j) {
	  IAIEDataSetColumn* LColumn = DataSet->GetDataSetOutputColumn(i);
	  int LColumnIndex = LColumn->GetIndex();
	  const char* LName = LColumn->GetName();
	  TAIEColumnKind LKind = LColumn->GetKind();
	  TAIEColumnType LType = LColumn->GetType();

	  if (LColumn->GetIsNominalType()) {
		  // -- atrybut nominalny (liczba zbiorów równa liczbie wartoœci lingwistycznych)
		  int LFuzzySetCount = LStats->GetStringValueCount(LColumnIndex);
		  double LMinValue = 0.0;
		  double LMaxValue = (double)LFuzzySetCount - 1.0;

		  // -- tworzenie atrybutu i kolekcji zbiorów rozmytych
		  new (&FOutputFuzzyAttr[i]) TAIELinguisticFuzzyAttr(FNumaNodeIndex, DataSet->GetOutputColumnCount(), LName, LKind, LType, LMinValue, LMaxValue);
		  new (&FFuzzyCollection[j]) TAIELinguisticFuzzyCollection(this, FNumaNodeIndex, j, LName, LKind, LType, LMinValue, LMaxValue, fuzzyMembershipFunction0, false, false, false);
		  FCurrentFuzzyCollection[j] = &FFuzzyCollection[j];
		  FEnabledFuzzyCollection[j] = &FFuzzyCollection[j];

		  // -- tworzenie zbiorów rozmytych dla danych nominalnych
		  FFuzzyCollection[j].SetFuzzySetCount(LFuzzySetCount);
		  for (int k = 0; k < LFuzzySetCount; ++k) {
			   IAIELinguisticFuzzySet* LFuzzySet = FFuzzyCollection[j].GetFuzzySet(k);
			   LFuzzySet->SetName(LStats->GetStringValue(LColumnIndex, k));
			   LFuzzySet->SetRCenter(k);
			   LFuzzySet->SetLCenter(k);
		  }
	  } else {
		  // -- atrybut numeryczny (liczba zbiorów domyœlna)
		  int LFuzzySetCount = FOptions->GetDefaultFuzzySetCount();
		  double LMinValue = LStats->GetMinValue(LColumnIndex);
		  double LMaxValue = LStats->GetMaxValue(LColumnIndex);

		  // -- tworzenie atrybutu i kolekcji zbiorów rozmytych
		  new (&FOutputFuzzyAttr[i]) TAIELinguisticFuzzyAttr(FNumaNodeIndex, DataSet->GetOutputColumnCount(), LName, LKind, LType, LMinValue, LMaxValue);
		  new (&FFuzzyCollection[j]) TAIELinguisticFuzzyCollection(this, FNumaNodeIndex, j, LName, LKind, LType, LMinValue, LMaxValue, FOptions->GetDefaultMembershipFunction(), FOptions->GetDefaultCoreEnabled(), FOptions->GetDefaultContrastEnabled(), FOptions->GetDefaultSFP());
		  FCurrentFuzzyCollection[j] = &FFuzzyCollection[j];
		  FEnabledFuzzyCollection[j] = &FFuzzyCollection[j];

		  // -- tworzenie zbiorów rozmytych dla danych numerycznych
		  FFuzzyCollection[j].SetFuzzySetCount(LFuzzySetCount);
	  }
 }

 // - tworzenie baz regu³ dla ka¿dego atrybutu wyjœciowego
 SetFuzzyRulebaseCount(FOutputFuzzyAttr.GetCount());
 FIsInitialized = true;
 PrepareFuzzyRulebase();

 // - inicjowanie regu³ (liczba regu³ równa liczbie zbiorów rozmytych dla atrybutu wyjœciowego
 TAIERandomGenerator64* RandomGenerator = new TAIERandomGenerator64();
 int LInpFuzzyAttrCount = FInputFuzzyAttr.GetCount();
 for (int k = 0; k < FFuzzyRulebase.GetCount(); ++k) {
	  int LRuleCount = FCurrentFuzzyCollection[LInpFuzzyAttrCount + k]->GetFuzzySetCount();
	  FFuzzyRulebase[k].SetFuzzyRuleCount(LRuleCount);

	  for (int i = 0; i < LRuleCount; ++i) {
		   IAIELinguisticFuzzyRule* LFuzzyRule = FFuzzyRulebase[k].GetFuzzyRule(i);
		   LFuzzyRule->SetOutFuzzySetIndex(i);
		   for (int j = 0; j < LInpFuzzyAttrCount; ++j)
				LFuzzyRule->SetInpFuzzySetIndex(j, (int)RandomGenerator->RandomInt(0, FCurrentFuzzyCollection[j]->GetFuzzySetCount() - 1));
	  }
 }
 delete RandomGenerator;

// na potrzeby klasyfikacji ECG
// for (int i = 1; i < GetFuzzyCollectionCount() - 1; ++i) SetCurrentFuzzyCollection(i, GetFuzzyCollection(0));
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::Clear(void)
{
 SetFuzzyRulebaseCount(0);

 for (int i = 0; i < FInputFuzzyAttr.GetCount(); ++i)  FInputFuzzyAttr[i].~TAIELinguisticFuzzyAttr();
 for (int i = 0; i < FOutputFuzzyAttr.GetCount(); ++i) FOutputFuzzyAttr[i].~TAIELinguisticFuzzyAttr();
 for (int i = 0; i < FFuzzyCollection.GetCount(); ++i) FFuzzyCollection[i].~TAIELinguisticFuzzyCollection();

 FInputFuzzyAttr.Resize(0);
 FOutputFuzzyAttr.Resize(0);
 FFuzzyCollection.Resize(0);
 FCurrentFuzzyCollection.Resize(0);
 FEnabledFuzzyCollection.Resize(0);

 FMaxFuzzyRuleCount = 1;
 FMinFuzzyRuleCount = 1;

 FIsInitialized = false;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::BeginCalcRuleStrengths(void)
{
 for (int i = 0; i < FFuzzyRulebase.GetCount(); ++i) {
	  TAIELinguisticFuzzyRulebase* LFuzzyRulebase = &FFuzzyRulebase[i];
	  for (int j = 0; j < LFuzzyRulebase->GetFuzzyRuleCount(); ++j) {
		   LFuzzyRulebase->FuzzyRule[j]->Strength = 0.0;
		   LFuzzyRulebase->FuzzyRule[j]->FireCount = 0;
	  }
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::EndCalcRuleStrengths(IAIEDataSet* InputDataSet)
{
 for (int i = 0; i < FFuzzyRulebase.GetCount(); ++i) {
	  TAIELinguisticFuzzyRulebase* LFuzzyRulebase = &FFuzzyRulebase[i];
	  for (int j = 0; j < LFuzzyRulebase->GetFuzzyRuleCount(); ++j)
		   LFuzzyRulebase->FuzzyRule[j]->Strength /= InputDataSet->RecordCount;
 }
}
//---------------------------------------------------------------------------
// Przeliczanie odpowiedzi systemu
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::Execute(IAIEDataSet* InputDataSet, IAIEDataSet* OutputDataSet, bool CalclulateRuleStrengths)
{
 if (!FIsInitialized) return;
 if (!InputDataSet->GetRecordCount()) return;
 /*
 if (FThreads) {
	FThreads->ExecuteNodeThreads(NumaNodePrepare);

	int LNodeCount = FThreads->GetNumaDevice()->GetNumaNodeCount();
	for (int i = 0; i < LNodeCount; ++i) {
		TAIENodeData* LNodeData = (TAIENodeData*)FThreads->GetNodeData(i);
		LNodeData->InpDataSet = InputDataSet;
		LNodeData->OutDataSet = OutputDataSet;
	}
	FThreads->ExecuteNodeThreads(NumaNodeUpdate);

	FThreads->ExecuteNodeThreads(NumaUnitPrepare);
	FThreads->ExecuteUnitThreads(NumaUnitExecute);

	FThreads->ExecuteNodeThreads(NumaUnitFinish);
	FThreads->ExecuteNodeThreads(NumaNodeFinish);
 } else {
 */
	TNumaVector<double> SNormTable(FNumaNodeIndex, FSNormTableCount);

	int Size = InputDataSet->GetRecordCount();
	double* T = new double[Size];
	double AvgT = 0.0;
	double MinT = 0.0;
	double MaxT = 0.0;
	double StdT = 0.0;
	__int64 ctr1 = 0, ctr2 = 0, freq = 0;

	for (int i = 0; i < Size; ++i) {
		 QueryPerformanceCounter((LARGE_INTEGER *)&ctr1);
		 ExecuteRecord(InputDataSet, OutputDataSet, SNormTable.GetData(), i, CalclulateRuleStrengths);
		 QueryPerformanceCounter((LARGE_INTEGER *)&ctr2);
		 QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

		 T[i] = (ctr2 - ctr1) * (1.0 / freq);
		 if (!i) {MinT = T[i]; MaxT = T[i];}

		 if (MinT > T[i]) MinT = T[i];
		 if (MaxT < T[i]) MaxT = T[i];
	}
	AvgT = AVG<double>(Size, T);
	StdT = DEV<double>(Size, T);
// }
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::ExecuteRecord(IAIEDataSet* InputDataSet, IAIEDataSet* OutputDataSet, int RecordIndex, bool CalclulateRuleStrengths)
{
 TNumaVector<double> SNormTable(FNumaNodeIndex, FSNormTableCount);
 ExecuteRecord(InputDataSet, OutputDataSet, SNormTable.GetData(), RecordIndex, CalclulateRuleStrengths);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::ExecuteRecord(TAIENumaNumeralDataSet* InputDataSet, TAIENumaNumeralDataSet* OutputDataSet, int RecordIndex, bool CalclulateRuleStrengths)
{
 TNumaVector<double> SNormTable(FNumaNodeIndex, FSNormTableCount);
 ExecuteRecord(InputDataSet, OutputDataSet, SNormTable.GetData(), RecordIndex, CalclulateRuleStrengths);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::ExecuteRecord(IAIEDataSet* InputDataSet, IAIEDataSet* OutputDataSet, double* SNormTable, int RecordIndex, bool CalclulateRuleStrengths)
{
 IAIEDataSetRecord* InputRecord = InputDataSet->GetDataSetRecord(RecordIndex);
 IAIEDataSetRecord* OutputRecord = OutputDataSet->GetDataSetRecord(RecordIndex);

 for (int i = 0, k = 0; i < FFuzzyRulebase.GetCount(); ++i, ++k) {
	  TAIELinguisticFuzzyRulebase*   LFuzzyRulebase = &FFuzzyRulebase[i];
	  TAIELinguisticFuzzyCollection* LFuzzyCollection = FCurrentFuzzyCollection[LFuzzyRulebase->GetFuzzyAttrIndex()];

	  if (LFuzzyRulebase->GetFuzzyRuleCount()) {
		  // - wyznaczenie najlepszej regu³y (aktywacja regu³y)
		  int LRuleIndex;
		  int LFuzzySetIndex = LFuzzyRulebase->ExecuteSNorm(InputRecord, SNormTable, LRuleIndex);
		  LFuzzyRulebase->FuzzyRule[LRuleIndex]->FireCount++;

		  if (CalclulateRuleStrengths)
			  for (int r = 0; r < LFuzzyRulebase->GetFuzzyRuleCount(); ++r) {
				   IAIELinguisticFuzzyRule* Rule = LFuzzyRulebase->FuzzyRule[r];
				   double  X = Rule->ExecuteTNorm(InputRecord);
				   Rule->Strength += X;
			  }

		  // - wyznaczanie wyjœcia
		  switch (LFuzzyCollection->GetType()) {
			 case ctString:  OutputRecord->SetValueAsString(k, (LFuzzySetIndex >= 0) ? LFuzzyCollection->GetFuzzySet(LFuzzySetIndex)->GetName() : ""); break;
			 case ctBoolean: OutputRecord->SetValueAsString(k, (LFuzzySetIndex >= 0) ? LFuzzyCollection->GetFuzzySet(LFuzzySetIndex)->GetName() : ""); break;
			 case ctDouble:  OutputRecord->SetValueAsDouble(k, LFuzzyRulebase->ExecuteOutput(SNormTable)); break;
			 case ctInteger: OutputRecord->SetValueAsInteger(k, ROUND(LFuzzyRulebase->ExecuteOutput(SNormTable))); break;
		  }

		  // - zapamiêtanie rozk³adu s-norm dla nominalnych danych
		  if (LFuzzyRulebase->GetIsNominalType())
			  for (int j = 0; j < LFuzzyCollection->GetFuzzySetCount(); ++j) OutputRecord->SetValueAsDouble(++k, SNormTable[j]);
	  } else {
		  // - wyznaczanie wyjœcia
		  switch (LFuzzyCollection->GetType()) {
			 case ctString:  OutputRecord->SetValueAsString(k, ""); break;
			 case ctBoolean: OutputRecord->SetValueAsBoolean(k, false); break;
			 case ctDouble:  OutputRecord->SetValueAsDouble(k, 0.0); break;
			 case ctInteger: OutputRecord->SetValueAsInteger(k, 0); break;
		  }

		  // - zapamiêtanie rozk³adu s-norm dla nominalnych danych
		  if (LFuzzyRulebase->GetIsNominalType())
			  for (int j = 0; j < LFuzzyCollection->GetFuzzySetCount(); ++j) OutputRecord->SetValueAsDouble(++k, 0.0);
	  }

	  SNormTable += LFuzzyCollection->GetFuzzySetCount();
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructure::ExecuteRecord(TAIENumaNumeralDataSet* InputDataSet, TAIENumaNumeralDataSet* OutputDataSet, double* SNormTable, int RecordIndex, bool CalclulateRuleStrengths)
{
 IAIEDataSetRecord* InputRecord = InputDataSet->GetDataSetRecord(RecordIndex);
 IAIEDataSetRecord* OutputRecord = OutputDataSet->GetDataSetRecord(RecordIndex);
 double* Inputs = (double*)InputRecord->GetData();
 double* Outputs = (double*)OutputRecord->GetData();

 for (int i = 0, k = 0; i < FFuzzyRulebase.GetCount(); ++i, ++k) {
	  TAIELinguisticFuzzyRulebase*   LFuzzyRulebase = &FFuzzyRulebase[i];
	  TAIELinguisticFuzzyCollection* LFuzzyCollection = FCurrentFuzzyCollection[LFuzzyRulebase->GetFuzzyAttrIndex()];

	  if (LFuzzyRulebase->GetFuzzyRuleCount()) {
		  // - wyznaczenie najlepszej regu³y (aktywacja regu³y)
		  int LRuleIndex;
		  int LFuzzySetIndex = LFuzzyRulebase->ExecuteDataSNorm(Inputs, SNormTable, LRuleIndex);
		  LFuzzyRulebase->FuzzyRule[LRuleIndex]->FireCount++;

		  if (CalclulateRuleStrengths)
			  for (int r = 0; r < LFuzzyRulebase->GetFuzzyRuleCount(); ++r) {
				   IAIELinguisticFuzzyRule* Rule = LFuzzyRulebase->FuzzyRule[r];
				   double  X = Rule->ExecuteTNorm(InputRecord);
				   Rule->Strength += X;
		  	  }

		  // - wyznaczanie wyjœcia
		  switch (LFuzzyCollection->GetType()) {
			 case ctString:  Outputs[k] = LFuzzySetIndex; break;
			 case ctBoolean: Outputs[k] = LFuzzySetIndex; break;
			 case ctDouble:  Outputs[k] = LFuzzyRulebase->ExecuteOutput(SNormTable); break;
			 case ctInteger: Outputs[k] = ROUND(LFuzzyRulebase->ExecuteOutput(SNormTable)); break;
		  }

		  // - zapamiêtanie rozk³adu s-norm dla nominalnych danych
		  if (LFuzzyRulebase->GetIsNominalType())
			  for (int j = 0; j < LFuzzyCollection->GetFuzzySetCount(); ++j) Outputs[++k] = SNormTable[j];
	  } else {
		  // - wyznaczanie wyjœcia
		  switch (LFuzzyCollection->GetType()) {
			 case ctString:  Outputs[k] = -1; break;
			 case ctBoolean: Outputs[k] = 0.0; break;
			 case ctDouble:  Outputs[k] = 0.0; break;
			 case ctInteger: Outputs[k] = 0; break;
		  }

		  // - zapamiêtanie rozk³adu s-norm dla nominalnych danych
		  if (LFuzzyRulebase->GetIsNominalType())
			  for (int j = 0; j < LFuzzyCollection->GetFuzzySetCount(); ++j) Outputs[++k] = 0.0;
	  }

	  SNormTable += LFuzzyCollection->GetFuzzySetCount();
 }
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructure::ExecuteError(IAIEDataSet* InputDataSet, IAIEDataSet* OutputDataSet,
	double* YTols, double* YErrors, int* YOutputsInTol, int& AVectorsInTol, double& ANormalizedError)
{
 if (!FIsInitialized) return 0.0;
 if (!InputDataSet->GetRecordCount()) return 0.0;

 double Error = 0.0;
 ANormalizedError = 0.0;
 AVectorsInTol = 0;
 memset(YErrors, 0, FErrorTableCount * sizeof(double));
 memset(YOutputsInTol, 0, FErrorTableCount * sizeof(int));

 if (FThreads) {
	FThreads->ExecuteNodeThreads(NumaNodePrepare);

	int LNodeCount = FThreads->GetNumaDevice()->GetNumaNodeCount();
	for (int i = 0; i < LNodeCount; ++i) {
		TAIENodeData* LNodeData = (TAIENodeData*)FThreads->GetNodeData(i);
		LNodeData->InpDataSet = InputDataSet;
		LNodeData->OutDataSet = OutputDataSet;
		memcpy(LNodeData->YTols, YTols, FErrorTableCount * sizeof(double));
	}

	FThreads->ExecuteNodeThreads(NumaNodeUpdate);

	FThreads->ExecuteUnitThreads(NumaUnitPrepare);
	FThreads->ExecuteUnitThreads(NumaUnitExecuteError);

	int LThreadCount = FThreads->GetThreadCount();
	for (int t = 0; t < LThreadCount; ++t) {
		TAIEUnitData* LData = (TAIEUnitData*)FThreads->GetThreadData(t);
		for (int i = 0; i < FErrorTableCount; ++i) {
			YErrors[i] += LData->YErrors[i];
			YOutputsInTol[i] += LData->YOutputsInTol[i];
		}
		Error += LData->Error;
		ANormalizedError += LData->NormalizedError;
		AVectorsInTol += LData->VectorsInTol;
	}

	FThreads->ExecuteUnitThreads(NumaUnitFinish);
	FThreads->ExecuteNodeThreads(NumaNodeFinish);
 } else
 if (dynamic_cast<TAIENumaNumeralDataSet*>(InputDataSet)) {
	TAIENumaNumeralDataSet* LInputDataSet = dynamic_cast<TAIENumaNumeralDataSet*>(InputDataSet);
	TAIENumaNumeralDataSet* LOutputDataSet = dynamic_cast<TAIENumaNumeralDataSet*>(OutputDataSet);

	TNumaVector<double> SNormTable(FNumaNodeIndex, FSNormTableCount);
	for (int i = 0; i < InputDataSet->GetRecordCount(); ++i)
		 Error += ExecuteRecordError(LInputDataSet, LOutputDataSet, SNormTable.GetData(), i, YTols, YErrors, YOutputsInTol, AVectorsInTol, ANormalizedError);
 } else {
	TNumaVector<double> SNormTable(FNumaNodeIndex, FSNormTableCount);
	for (int i = 0; i < InputDataSet->GetRecordCount(); ++i)
		 Error += ExecuteRecordError(InputDataSet, OutputDataSet, SNormTable.GetData(), i, YTols, YErrors, YOutputsInTol, AVectorsInTol, ANormalizedError);
 }

 ANormalizedError = SQRT(ANormalizedError / InputDataSet->GetRecordCount());
 return SQRT(Error / InputDataSet->GetRecordCount());
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructure::ExecuteRecordError(IAIEDataSet* InputDataSet, IAIEDataSet* OutputDataSet, double* SNormTable, int RecordIndex,
	double* YTols, double* YErrors, int* YOutputsInTol, int& AVectorsInTol, double& ANormalizedError)
{
 IAIEDataSetRecord* InputRecord = InputDataSet->GetDataSetRecord(RecordIndex);
 IAIEDataSetRecord* OutputRecord = OutputDataSet ? OutputDataSet->GetDataSetRecord(RecordIndex) : NULL;

 double LError = 0.0;
 double LNormalizedError = 0.0;
 bool   LVectorInTol = true;
 for (int i = 0, k = 0; i < FFuzzyRulebase.GetCount(); ++i, ++k) {
	  TAIELinguisticFuzzyRulebase*   LFuzzyRulebase = &FFuzzyRulebase[i];
	  TAIELinguisticFuzzyCollection* LFuzzyCollection = FCurrentFuzzyCollection[LFuzzyRulebase->GetFuzzyAttrIndex()];
	  int LFuzzySetCount = LFuzzyCollection->GetFuzzySetCount();

	  if (LFuzzyRulebase->GetFuzzyRuleCount()) {
		  // - wyznaczenie najlepszej regu³y (aktywacja regu³y)
		  int LRuleIndex;
		  int LFuzzySetIndex = LFuzzyRulebase->ExecuteSNorm(InputRecord, SNormTable, LRuleIndex);

		  if (LFuzzyRulebase->GetIsNominalType()) {
			  // -- wyznaczanie wyjœcia
			  if (OutputRecord) OutputRecord->SetValueAsString(k, (LFuzzySetIndex >= 0) ? LFuzzyCollection->GetFuzzySet(LFuzzySetIndex)->GetName() : "");

			  // -- wyznaczanie b³êdów
			  int OFuzzySetIndex = LFuzzyCollection->GetFuzzySetIndexByName(InputRecord->GetOutputValueAsString(i));
			  if (OFuzzySetIndex != LFuzzySetIndex) LVectorInTol = false;

			  double RError = 0.0;
			  for (int j = 0; j < LFuzzySetCount; ++j) {
				   double LDelta = (OFuzzySetIndex == j) ? 1.0 - SNormTable[j] : SNormTable[j];
				   if ((LDelta <= YTols[j]) || (YTols[j] == 0.0)) ++YOutputsInTol[j]; else LVectorInTol = false;
				   LDelta = SQR(LDelta);
				   YErrors[j] += LDelta;
				   RError += LDelta;

				   // --- zapamiêtanie rozk³adu s-norm
				   if (OutputRecord) OutputRecord->SetValueAsDouble(++k, SNormTable[j]); else ++k;
			  }
			  RError /= LFuzzySetCount;
			  LNormalizedError += RError;
			  LError += RError;

			  YTols += LFuzzySetCount;
			  YErrors += LFuzzySetCount;
			  YOutputsInTol += LFuzzySetCount;
		  } else {
			  // -- wyznaczanie wyjœcia
			  double LOutput = LFuzzyRulebase->ExecuteOutput(SNormTable);
			  if (OutputRecord) switch (LFuzzyCollection->GetType()) {
				   case ctDouble:  OutputRecord->SetValueAsDouble(k, LOutput); break;
				   case ctInteger: OutputRecord->SetValueAsInteger(k, ROUND(LOutput)); break;
			  }

			  // -- wyznaczanie b³êdów
			  double LDelta = ABS(InputRecord->GetOutputValueAsDouble(i) - LOutput);
			  if ((LDelta <= YTols[0]) || (YTols[0] == 0.0)) ++YOutputsInTol[0]; else LVectorInTol = false;
			  LNormalizedError += SQR(LDelta / (LFuzzyCollection->GetMaxValue() - LFuzzyCollection->GetMinValue()));
			  LDelta = SQR(LDelta);
			  YErrors[0] += LDelta;
			  LError += LDelta;

			  YTols += 1;
			  YErrors += 1;
			  YOutputsInTol += 1;
		  }
	  }

	  SNormTable += LFuzzyCollection->GetFuzzySetCount();
 }

 if (LVectorInTol) ++AVectorsInTol;
 ANormalizedError += LNormalizedError / FFuzzyRulebase.GetCount();
 return LError / FFuzzyRulebase.GetCount();
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructure::ExecuteRecordError(TAIENumaNumeralDataSet* InputDataSet, TAIENumaNumeralDataSet* OutputDataSet, double* SNormTable, int RecordIndex,
	double* YTols, double* YErrors, int* YOutputsInTol, int& AVectorsInTol, double& ANormalizedError)
{
 IAIEDataSetRecord* InputRecord = InputDataSet->GetDataSetRecord(RecordIndex);
 IAIEDataSetRecord* OutputRecord = OutputDataSet ? OutputDataSet->GetDataSetRecord(RecordIndex) : NULL;
 double* Inputs = (double*)InputRecord->GetData();
 double* DOutputs = Inputs + InputDataSet->GetInputColumnCount();
 double* OOutputs = OutputRecord ? (double*)OutputRecord->GetData() : NULL;

 double LError = 0.0;
 double LNormalizedError = 0.0;
 bool   LVectorInTol = true;
 for (int i = 0, k = 0; i < FFuzzyRulebase.GetCount(); ++i, ++k) {
	  TAIELinguisticFuzzyRulebase*   LFuzzyRulebase = &FFuzzyRulebase[i];
	  TAIELinguisticFuzzyCollection* LFuzzyCollection = FCurrentFuzzyCollection[LFuzzyRulebase->GetFuzzyAttrIndex()];
	  int LFuzzySetCount = LFuzzyCollection->GetFuzzySetCount();

	  if (LFuzzyRulebase->GetFuzzyRuleCount()) {
		  // - wyznaczenie najlepszej regu³y (aktywacja regu³y)
		  int LRuleIndex;
		  int LFuzzySetIndex = LFuzzyRulebase->ExecuteDataSNorm(Inputs, SNormTable, LRuleIndex);

		  if (LFuzzyRulebase->GetIsNominalType()) {
			  // -- wyznaczanie wyjœcia
			  if (OOutputs) OOutputs[k] = LFuzzySetIndex;

			  // -- wyznaczanie b³êdów
			  int OFuzzySetIndex = (int)DOutputs[i];
			  if (OFuzzySetIndex != LFuzzySetIndex) LVectorInTol = false;

			  double RError = 0.0;
			  for (int j = 0; j < LFuzzySetCount; ++j) {
				   double LDelta = (OFuzzySetIndex == j) ? 1.0 - SNormTable[j] : SNormTable[j];
				   if ((LDelta <= YTols[j]) || (YTols[j] == 0.0)) ++YOutputsInTol[j]; else LVectorInTol = false;
				   LDelta = SQR(LDelta);
				   YErrors[j] += LDelta;
				   RError += LDelta;

				   // --- zapamiêtanie rozk³adu s-norm
				   if (OOutputs) OOutputs[++k] = SNormTable[j]; else ++k;
			  }
			  RError /= LFuzzySetCount;
			  LNormalizedError += RError;
			  LError += RError;

			  YTols += LFuzzySetCount;
			  YErrors += LFuzzySetCount;
			  YOutputsInTol += LFuzzySetCount;
		  } else {
			  // -- wyznaczanie wyjœcia
			  double LOutput = LFuzzyRulebase->ExecuteOutput(SNormTable);
			  if (OOutputs) switch (LFuzzyCollection->GetType()) {
				   case ctDouble:  OOutputs[k] = LOutput; break;
				   case ctInteger: OOutputs[k] = ROUND(LOutput); break;
			  }

			  // -- wyznaczanie b³êdów
			  double LDelta = ABS(DOutputs[i] - LOutput);
			  if ((LDelta <= YTols[0]) || (YTols[0] == 0.0)) ++YOutputsInTol[0]; else LVectorInTol = false;
			  LNormalizedError += SQR(LDelta / (LFuzzyCollection->GetMaxValue() - LFuzzyCollection->GetMinValue()));
			  LDelta = SQR(LDelta);
			  YErrors[0] += LDelta;
			  LError += LDelta;

			  YTols += 1;
			  YErrors += 1;
			  YOutputsInTol += 1;
		  }
	  }

	  SNormTable += LFuzzySetCount;
 }
 if (LVectorInTol) ++AVectorsInTol;
 ANormalizedError += LNormalizedError / FFuzzyRulebase.GetCount();
 return LError / FFuzzyRulebase.GetCount();
}
//---------------------------------------------------------------------------

