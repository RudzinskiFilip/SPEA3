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
#include "UAIERandomPermutation.h"
#include "UAIERandomGenerator64.h"
#include "UAIEGeneticAlgorithm.h"
#include "UAIEStopConditions.h"
#include "UAIEInitConditions.h"
#include "UAIENumaDevice.h"
#include "UAIENumaNode.h"
#include "UAIENumaUnit.h"
#include "UAIENumaThreads.h"
#include "UAIEFunction.h"
#include "UAIELinguisticFuzzySet.h"
#include "UAIELinguisticFuzzyRule.h"
#include "UAIELinguisticFuzzyStructure.h"

#include "UAIEDataSet.h"
#include "UAIEDataSetRecord.h"
#include "UAIEDataSetColumn.h"
#include "UAIENumaNumeralDataSet.h"
#include "UAIENumaNumeralDataSetRecord.h"

#include "UAIELinguisticFuzzyStructure.h"
#include "UAIELinguisticFuzzyStructureLrnAlgorithm.h"
#include "UAIELinguisticFuzzyStructureLrnAGAlgorithm.h"
#include "UAIELinguisticFuzzyStructureGeneticOperators1.h"
#include "UAIELinguisticFuzzyStructureGeneticOperators2.h"
//---------------------------------------------------------------------------
BEGIN_NAMESPACE(NAIELinguisticFuzzyStructureLrnAGAlgorithm)
//---------------------------------------------------------------------------
// PROCEDURY I STRUKTURY DO PRZELICZANIA ODPOWIEDZI NA DANE UCZ¥CE
// (PROCEDURY ALGORYTMU I STRUKTURY GENETYCZNEGO)
//---------------------------------------------------------------------------
// Struktura przechowuj¹ca zbiory danych ucz¹cych
// (jeden zbiór dla ka¿dego wêz³a)
//  odpowiedŸ sieci na dane ucz¹ce przeliczana jest indywidualnie dla ka¿dego
//  chromosomu (TAIEUnitDataLrn), bez równoleg³ego przeliczania rekordów
//---------------------------------------------------------------------------
struct TAIENodeDataLrn
{
 TAIESystem* System;
 TAIENumaNumeralDataSet* DataSet;
 TAIELinguisticFuzzyStructure* FuzzyStructure;

 bool OnlyFullFuzzyRule;			//parametry uczenia
 bool OnlyTrueFuzzyRule;
 double RulebasePriority;
 bool BinaryEncoding;
 bool OC_FuzzyRuleCount;
 bool OC_FuzzyLength;
 bool OC_FuzzyAttrCount;
 bool OC_FuzzySetCount;
 bool OC_AllFuzzyAttrCount;
 bool OC_AllFuzzySetCount;

 int FSC;
 int RBCount;                       //indeksy baz regu³, atrybutów, itp. podlegaj¹cych optymalizacji
 int* RB;
 int IOCount;
 int* IO;
 int MPCount;
 int* MP;
 int MTCount;
 int* MT;

 double* LrnTols;					//zadane tolerancje dla danych ucz¹cych
};
//---------------------------------------------------------------------------
// Struktura chromosomu
//---------------------------------------------------------------------------
struct TAIEUnitDataLrn
{
 TAIESystem* System;
 IAIENumaThreads* Threads;
 TAIENumaNumeralDataSet* DataSet;
 TAIELinguisticFuzzyStructure* FuzzyStructure;

 double* LrnErrors;					//suma kwadratów b³êdów na poszczególnych wyjœciach dla danych ucz¹cych
 int* 	 LrnOutputsInTol;			//liczba rekordów danych ucz¹cych dla których dane wyjœcie by³o w tolerancji
 int	 LrnVectorsInTol;			//liczba wektorów danych ucz¹cych w tolerancji
 double  LrnError;
 double  NormalizedError;			//normalizowany b³¹d odpowiedzi (0.0 - 1.0)
 int*    MinFuzzyRuleCount;
 int*    MaxFuzzyRuleCount;
 double  FuzzyStructureComplexity;	  //œrednia z³o¿onoœæ wszystkich baz regu³
 double* NormFuzzyRulebaseFuzzyRuleCount;
 double* NormFuzzyRulebaseFuzzyRuleComplexity;
 double* NormFuzzyRulebaseFuzzyAttrCount;
 double* NormFuzzyRulebaseFuzzySetCount;

 // - dane wspólne dla chromosomów (po jednym egzemplarzu na wêze³)
 bool OnlyFullFuzzyRule;
 bool OnlyTrueFuzzyRule;
 double RulebasePriority;
 bool BinaryEncoding;
 bool OC_FuzzyRuleCount;
 bool OC_FuzzyLength;
 bool OC_FuzzyAttrCount;
 bool OC_FuzzySetCount;
 bool OC_AllFuzzyAttrCount;
 bool OC_AllFuzzySetCount;

 int FSC;
 int RBCount;
 int* RB;
 int IOCount;
 int* IO;
 int MPCount;
 int* MP;
 int MTCount;
 int* MT;

 unsigned int* BinaryFuzzySetParamsVector;	//wektor binarnie kodowanych parametrów zbiorów rozmytych
 double* BinaryFuzzySetMaxVector;		//wektor maksymalnych wartoœci parametrów zbiorów rozmytych
 double* BinaryFuzzySetMinVector;		//wektor maksymalnych wartoœci parametrów zbiorów rozmytych

 double* LrnTols;					//zadane tolerancje dla danych ucz¹cych
};
//---------------------------------------------------------------------------
// Procedura wielow¹tkowa uruchamiana na pocz¹tku procesu uczenia dla ka¿dego wêz³a
//---------------------------------------------------------------------------
void __stdcall NumaNodePrepareLrn(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 //PROCEDURA URUCHAMIANA DLA WÊZ£ÓW NA POCZ¥TKU PROCESU UCZENIA (kopiowanie danych do wêz³ów)
 //odczytanie pomocniczych wskaŸników i utworzenie struktur danych
 TAIELinguisticFuzzyStructureLrnAlgorithm* LAlgorithm = (TAIELinguisticFuzzyStructureLrnAlgorithm*)AThreads->GetData();
 TAIELinguisticFuzzyStructure* LFuzzyStructure = dynamic_cast<TAIELinguisticFuzzyStructure*>(LAlgorithm->GetFuzzyStructure());

 TAIENodeDataLrn* LNodeData = (TAIENodeDataLrn*)AThreads->LocalAllocMemory(ANodeIndex, sizeof(TAIENodeDataLrn));
 AThreads->SetNodeData(ANodeIndex, LNodeData);
 LNodeData->System = dynamic_cast<TAIESystem*>(AThreads->GetNumaDevice()->GetSystem());
 LNodeData->FuzzyStructure = LFuzzyStructure;
 LNodeData->OnlyFullFuzzyRule = LAlgorithm->GetOnlyFullFuzzyRule();
 LNodeData->OnlyTrueFuzzyRule = LAlgorithm->GetOnlyTrueFuzzyRule();
 LNodeData->RulebasePriority = LAlgorithm->GetRulebasePriority();
 LNodeData->BinaryEncoding = LAlgorithm->GetBinaryEncoding();
 LNodeData->OC_FuzzyRuleCount = LAlgorithm->GetOC_FuzzyRuleCount();
 LNodeData->OC_FuzzyLength = LAlgorithm->GetOC_FuzzyLength();
 LNodeData->OC_FuzzyAttrCount = LAlgorithm->GetOC_FuzzyAttrCount();
 LNodeData->OC_FuzzySetCount = LAlgorithm->GetOC_FuzzySetCount();
 LNodeData->OC_AllFuzzyAttrCount = LAlgorithm->GetOC_AllFuzzyAttrCount();
 LNodeData->OC_AllFuzzySetCount = LAlgorithm->GetOC_AllFuzzySetCount();

 // - przygotowanie zbioru danych ucz¹cych do jednego wêz³a
 TAIEDataSet* LrnDataSet = dynamic_cast<TAIEDataSet*>(LAlgorithm->GetLrnDataSet());
 LNodeData->DataSet = new TAIENumaNumeralDataSet(LNodeData->System, ANodeIndex);

 int LColumnCount = LrnDataSet->GetColumnCount();
 int LRecordCount = LrnDataSet->GetRecordCount();
 LNodeData->DataSet->Reset(LColumnCount, LRecordCount);
 for (int i = 0; i < LColumnCount; ++i) LNodeData->DataSet->GetDataSetColumn(i)->SetKind(LrnDataSet->GetDataSetColumn(i)->GetKind());

 // - sprawdzenie ile baz regu³ podlega optymalizacji
 LNodeData->RBCount = 0;
 LNodeData->IOCount = 0;
 for (int i = 0; i < LFuzzyStructure->GetFuzzyRulebaseCount(); ++i) {
	  if ((LAlgorithm->GetInitConditionsRB()->GetMode(i) == 2) || (LAlgorithm->GetInitConditionsRB()->GetMode(i) == 3)) LNodeData->RBCount++;
	  if ((LAlgorithm->GetInitConditionsIO()->GetMode(i) == 2) || (LAlgorithm->GetInitConditionsIO()->GetMode(i) == 3)) LNodeData->IOCount++;
 }
 LNodeData->RB = (int*)AThreads->LocalAllocMemory(ANodeIndex, LNodeData->RBCount * sizeof(int));
 LNodeData->IO = (int*)AThreads->LocalAllocMemory(ANodeIndex, LNodeData->IOCount * sizeof(int));
 for (int i = 0, k = 0, l = 0; i < LFuzzyStructure->GetFuzzyRulebaseCount(); ++i) {
	  if ((LAlgorithm->GetInitConditionsRB()->GetMode(i) == 2) || (LAlgorithm->GetInitConditionsRB()->GetMode(i) == 3)) LNodeData->RB[k++] = i;
	  if ((LAlgorithm->GetInitConditionsIO()->GetMode(i) == 2) || (LAlgorithm->GetInitConditionsIO()->GetMode(i) == 3)) LNodeData->IO[l++] = i;
 }

 // - sprawdzenie ile atrybutów podlega optymalizacji
 LNodeData->FSC = 0;
 LNodeData->MPCount = 0;
 LNodeData->MTCount = 0;
 for (int i = 0; i < LFuzzyStructure->GetEnabledFuzzyCollectionCount(); ++i) {
	  IAIELinguisticFuzzyCollection* FuzzyCollection = LFuzzyStructure->GetEnabledFuzzyCollection(i);
	  if (FuzzyCollection->GetIsNominalType()) continue;
	  if ((LAlgorithm->GetInitConditionsMP()->GetMode(i) == 2) || (LAlgorithm->GetInitConditionsMP()->GetMode(i) == 3)) {LNodeData->MPCount++; LNodeData->FSC += FuzzyCollection->GetFuzzySetCount();}
	  if ((LAlgorithm->GetInitConditionsMT()->GetMode(i) == 2) || (LAlgorithm->GetInitConditionsMT()->GetMode(i) == 3)) LNodeData->MTCount++;
 }
 LNodeData->MP = (int*)AThreads->LocalAllocMemory(ANodeIndex, LNodeData->MPCount * sizeof(int));
 LNodeData->MT = (int*)AThreads->LocalAllocMemory(ANodeIndex, LNodeData->MTCount * sizeof(int));
 for (int i = 0, k = 0, l = 0; i < LFuzzyStructure->GetEnabledFuzzyCollectionCount(); ++i) {
	  IAIELinguisticFuzzyCollection* FuzzyCollection = LFuzzyStructure->GetEnabledFuzzyCollection(i);
	  if (FuzzyCollection->GetIsNominalType()) continue;
	  if ((LAlgorithm->GetInitConditionsMP()->GetMode(i) == 2) || (LAlgorithm->GetInitConditionsMP()->GetMode(i) == 3)) LNodeData->MP[k++] = i;
	  if ((LAlgorithm->GetInitConditionsMT()->GetMode(i) == 2) || (LAlgorithm->GetInitConditionsMT()->GetMode(i) == 3)) LNodeData->MT[l++] = i;
 }

 // - tworzenie lokalnych danych wymaganych do uczenia
 LNodeData->LrnTols = (double*)AThreads->LocalAllocMemory(ANodeIndex, LFuzzyStructure->GetErrorTableCount() * sizeof(double));
 for (int i = 0; i < LFuzzyStructure->GetErrorTableCount(); ++i) LNodeData->LrnTols[i] = LAlgorithm->GetLrnStopCondition()->GetTol(i);
}
//---------------------------------------------------------------------------
// Procedura wielow¹tkowa uruchamiana po zakoñczeniu procesu uczenia dla ka¿dego wêz³a
//---------------------------------------------------------------------------
void __stdcall NumaNodeFinishLrn(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 //PROCEDURA URUCHAMIANA DLA WÊZ£ÓW NA KOÑCU PROCESU UCZENIA (usuwanie danych z wêz³ów)
 //odczytanie pomocniczych wskaŸników
 TAIENodeDataLrn* LNodeData = (TAIENodeDataLrn*)AThreads->GetNodeData(ANodeIndex);

 delete LNodeData->DataSet;
 AThreads->LocalFreeMemory(LNodeData->RB);
 AThreads->LocalFreeMemory(LNodeData->IO);
 AThreads->LocalFreeMemory(LNodeData->MP);
 AThreads->LocalFreeMemory(LNodeData->MT);
 AThreads->LocalFreeMemory(LNodeData->LrnTols);
 AThreads->LocalFreeMemory(LNodeData);
}
//---------------------------------------------------------------------------
// Procedura wielow¹tkowa uruchamiana na pocz¹tku ka¿dej epoki uczenia procesu uczenia dla ka¿dego wêz³a
//---------------------------------------------------------------------------
void __stdcall NumaNodeUpdateLrn(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 //PROCEDURA URUCHAMIANA DLA WÊZ£ÓW NA POCZ¥TKU PROCESU UCZENIA (kopiowanie danych do wêz³ów)
 //odczytanie pomocniczych wskaŸników i utworzenie struktur danych
 TAIELinguisticFuzzyStructureLrnAlgorithm* LAlgorithm = (TAIELinguisticFuzzyStructureLrnAlgorithm*)AThreads->GetData();
 TAIELinguisticFuzzyStructure* LFuzzyStructure = dynamic_cast<TAIELinguisticFuzzyStructure*>(LAlgorithm->GetFuzzyStructure());
 TAIENodeDataLrn* LNodeData = (TAIENodeDataLrn*)AThreads->GetNodeData(ANodeIndex);

 // - kopiowanie ca³ego zbioru danych ucz¹cych do jednego wêz³a
 TAIEDataSet* LrnDataSet = dynamic_cast<TAIEDataSet*>(LAlgorithm->GetIsNoiseInputs() ? LAlgorithm->GetNoiseSet() : LAlgorithm->GetLrnDataSet());
 TAIENumaNumeralDataSet* NumDataSet = LNodeData->DataSet;
 int LColumnCount = LrnDataSet->GetColumnCount();
 int LRecordCount = LrnDataSet->GetRecordCount();
 for (int i = 0; i < LColumnCount; ++i) {
	  switch (LrnDataSet->GetDataSetColumn(i)->GetType()) {
		   case ctDouble:  for (int j = 0; j < LRecordCount; ++j)
								 NumDataSet->GetDataSetRecord(j)->SetValueAsDouble(i, LrnDataSet->GetDataSetRecord(j)->GetValueAsDouble(i));
						   break;
		   case ctInteger: for (int j = 0; j < LRecordCount; ++j)
								 NumDataSet->GetDataSetRecord(j)->SetValueAsDouble(i, LrnDataSet->GetDataSetRecord(j)->GetValueAsInteger(i));
						   break;
		   case ctBoolean: for (int j = 0; j < LRecordCount; ++j)
								 NumDataSet->GetDataSetRecord(j)->SetValueAsDouble(i, LrnDataSet->GetDataSetRecord(j)->GetValueAsBoolean(i));
						   break;
		   case ctString:  {
							IAIELinguisticFuzzyCollection* FuzzyCollection = LFuzzyStructure->GetCurrentFuzzyCollection(i);
							for (int j = 0; j < LRecordCount; ++j) {
								 int FuzzySetIndex = FuzzyCollection->GetFuzzySetIndexByName(LrnDataSet->GetDataSetRecord(j)->GetValueAsString(i));
								 NumDataSet->GetDataSetRecord(j)->SetValueAsDouble(i, FuzzySetIndex);
							}
						   }
						   break;
	  }
 }
}
//---------------------------------------------------------------------------
// Inicjowanie pojedynczego chromosomu
//---------------------------------------------------------------------------
void __stdcall ChromosomeInit(IAIEGeneticAlgorithm* GA, void*& AChromosomeHandle, int ANodeIndex, int AUnitIndex, int AThreadIndex)
{
 IAIENumaThreads* AThreads = (IAIENumaThreads*)GA->GetData();
 TAIENodeDataLrn* LNodeData = (TAIENodeDataLrn*)AThreads->GetNodeData(ANodeIndex);
 TAIELinguisticFuzzyStructureLrnAlgorithm* LAlgorithm = (TAIELinguisticFuzzyStructureLrnAlgorithm*)((IAIENumaThreads*)GA->GetData())->GetData();

 TAIEUnitDataLrn* LUnitData = (TAIEUnitDataLrn*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, sizeof(TAIEUnitDataLrn));
 LUnitData->System = LNodeData->System;
 LUnitData->Threads = AThreads;
 LUnitData->DataSet = LNodeData->DataSet;
 LUnitData->LrnTols = LNodeData->LrnTols;
 LUnitData->OnlyFullFuzzyRule = LNodeData->OnlyFullFuzzyRule;
 LUnitData->OnlyTrueFuzzyRule = LNodeData->OnlyTrueFuzzyRule;
 LUnitData->RulebasePriority = LNodeData->RulebasePriority;
 LUnitData->BinaryEncoding = LNodeData->BinaryEncoding;
 LUnitData->OC_FuzzyRuleCount = LNodeData->OC_FuzzyRuleCount;
 LUnitData->OC_FuzzyLength = LNodeData->OC_FuzzyLength;
 LUnitData->OC_FuzzyAttrCount = LNodeData->OC_FuzzyAttrCount;
 LUnitData->OC_FuzzySetCount = LNodeData->OC_FuzzySetCount;
 LUnitData->OC_AllFuzzyAttrCount = LNodeData->OC_AllFuzzyAttrCount;
 LUnitData->OC_AllFuzzySetCount = LNodeData->OC_AllFuzzySetCount;
 LUnitData->FSC = LNodeData->FSC;
 LUnitData->RBCount = LNodeData->RBCount;
 LUnitData->RB = LNodeData->RB;
 LUnitData->IOCount = LNodeData->IOCount;
 LUnitData->IO = LNodeData->IO;
 LUnitData->MPCount = LNodeData->MPCount;
 LUnitData->MP = LNodeData->MP;
 LUnitData->MTCount = LNodeData->MTCount;
 LUnitData->MT = LNodeData->MT;
 LUnitData->FuzzyStructure = new TAIELinguisticFuzzyStructure(LUnitData->System, ANodeIndex, false);
 LUnitData->FuzzyStructure->Assign(LNodeData->FuzzyStructure);

 LUnitData->LrnErrors = (double*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, LUnitData->FuzzyStructure->GetErrorTableCount() * sizeof(double));
 LUnitData->LrnOutputsInTol = (int*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, LUnitData->FuzzyStructure->GetErrorTableCount() * sizeof(int));

 int LFuzzyRulebaseCount = LUnitData->FuzzyStructure->GetFuzzyRulebaseCount();
 LUnitData->MinFuzzyRuleCount = (int*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, LFuzzyRulebaseCount * sizeof(int));
 LUnitData->MaxFuzzyRuleCount = (int*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, LFuzzyRulebaseCount * sizeof(int));
 LUnitData->NormFuzzyRulebaseFuzzyRuleComplexity = (double*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, LFuzzyRulebaseCount * sizeof(double));
 LUnitData->NormFuzzyRulebaseFuzzyRuleCount = (double*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, LFuzzyRulebaseCount * sizeof(double));
 LUnitData->NormFuzzyRulebaseFuzzyAttrCount = (double*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, LFuzzyRulebaseCount * sizeof(double));
 LUnitData->NormFuzzyRulebaseFuzzySetCount = (double*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, LFuzzyRulebaseCount * sizeof(double));

 for (int i = 0; i < LFuzzyRulebaseCount; ++i) {
	  IAIELinguisticFuzzyRulebase* LFuzzyRulebase = LUnitData->FuzzyStructure->GetFuzzyRulebase(i);
	  if (LAlgorithm->GetMaxMinRuleCountEnabled()) {
		  LUnitData->MinFuzzyRuleCount[i] = MAX(LFuzzyRulebase->GetMinFuzzyRuleCount(), LAlgorithm->GetMinRuleCountValue());
		  LUnitData->MaxFuzzyRuleCount[i] = MIN(LFuzzyRulebase->GetMaxFuzzyRuleCount(), LAlgorithm->GetMaxRuleCountValue());
	  } else {
		  LUnitData->MinFuzzyRuleCount[i] = LFuzzyRulebase->GetMinFuzzyRuleCount();
		  LUnitData->MaxFuzzyRuleCount[i] = LFuzzyRulebase->GetMaxFuzzyRuleCount();
	  }
 }

 if (LUnitData->BinaryEncoding) {
	 LUnitData->BinaryFuzzySetParamsVector = (unsigned int*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, 6 * LUnitData->FSC * sizeof(unsigned int));
	 LUnitData->BinaryFuzzySetMaxVector = (double*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, 6 * LUnitData->FSC * sizeof(double));
	 LUnitData->BinaryFuzzySetMinVector = (double*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, 6 * LUnitData->FSC * sizeof(double));

	 int BinaryValueIndex = 0;
	 for (int i = 0; i < LUnitData->MPCount; ++i) {
		 IAIELinguisticFuzzyCollection* FuzzyCollection = LUnitData->FuzzyStructure->GetEnabledFuzzyCollection(LUnitData->MP[i]);
		 int LFuzzySetCount = FuzzyCollection->GetFuzzySetCount();
		 if (!LFuzzySetCount) continue;

		 double LMin = FuzzyCollection->GetMinValue();
		 double LMax = FuzzyCollection->GetMaxValue();
		 double LRange = LMax - LMin;
		 //double LWidth = LRange / LFuzzySetCount;
		 for (int i = 0; i < LFuzzySetCount; ++i) {
			  IAIELinguisticFuzzySet* FuzzySet = FuzzyCollection->GetFuzzySet(i);

			  LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex] = LMax;
			  LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex] = LMin;
			  LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetLCenter(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]);
			  ++BinaryValueIndex;
			  LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex] = LMax;
			  LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex] = LMin;
			  LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetRCenter(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]);
			  ++BinaryValueIndex;
			  LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex] = LRange;
			  LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex] = 0.0;
			  LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetLAlpha(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]);
			  ++BinaryValueIndex;
			  LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex] = LRange;
			  LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex] = 0.0;
			  LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetRAlpha(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]);
			  ++BinaryValueIndex;
			  LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex] = +0.5;
			  LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex] = -0.5;
			  LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetLContrast(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]);
			  ++BinaryValueIndex;
			  LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex] = +0.5;
			  LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex] = -0.5;
			  LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetRContrast(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]);
			  ++BinaryValueIndex;
		 }
	 }
 } else {
	 LUnitData->BinaryFuzzySetParamsVector = NULL;
	 LUnitData->BinaryFuzzySetMaxVector = NULL;
	 LUnitData->BinaryFuzzySetMinVector = NULL;
 }

 AChromosomeHandle = LUnitData;
}
//---------------------------------------------------------------------------
void __stdcall ChromosomeFree(IAIEGeneticAlgorithm* GA, void* AChromosomeHandle)
{
 TAIEUnitDataLrn* LUnitData = (TAIEUnitDataLrn*)AChromosomeHandle;
 IAIENumaThreads* AThreads = LUnitData->Threads;

 delete LUnitData->FuzzyStructure;

 if (LUnitData->BinaryEncoding) {
	 AThreads->PrivateFreeMemory(LUnitData->BinaryFuzzySetParamsVector);
	 AThreads->PrivateFreeMemory(LUnitData->BinaryFuzzySetMaxVector);
	 AThreads->PrivateFreeMemory(LUnitData->BinaryFuzzySetMinVector);
 }
 AThreads->PrivateFreeMemory(LUnitData->LrnErrors);
 AThreads->PrivateFreeMemory(LUnitData->LrnOutputsInTol);
 AThreads->PrivateFreeMemory(LUnitData->MinFuzzyRuleCount);
 AThreads->PrivateFreeMemory(LUnitData->MaxFuzzyRuleCount);
 AThreads->PrivateFreeMemory(LUnitData->NormFuzzyRulebaseFuzzyRuleComplexity);
 AThreads->PrivateFreeMemory(LUnitData->NormFuzzyRulebaseFuzzyRuleCount);
 AThreads->PrivateFreeMemory(LUnitData->NormFuzzyRulebaseFuzzyAttrCount);
 AThreads->PrivateFreeMemory(LUnitData->NormFuzzyRulebaseFuzzySetCount);
 AThreads->PrivateFreeMemory(LUnitData);
}
//---------------------------------------------------------------------------
void __stdcall ChromosomeRand(IAIEGeneticAlgorithm* GA, void* AChromosomeHandle)
{
 TAIEUnitDataLrn* LUnitData = (TAIEUnitDataLrn*)AChromosomeHandle;
 IAIERandomGenerator64* RandomGenerator = GA->GetRandomGenerator();
 TAIELinguisticFuzzyStructureLrnAlgorithm* LAlgorithm = (TAIELinguisticFuzzyStructureLrnAlgorithm*)((IAIENumaThreads*)GA->GetData())->GetData();
 TAIELinguisticFuzzyStructure* LFuzzyStructure = LUnitData->FuzzyStructure;

 // - losowanie parametrów zbiorów rozmytych
 int BinaryValueIndex = 0;
 for (int i = 0; i < LFuzzyStructure->GetEnabledFuzzyCollectionCount(); ++i) {
	  IAIELinguisticFuzzyCollection* FuzzyCollection = LFuzzyStructure->GetEnabledFuzzyCollection(i);
	  if (FuzzyCollection->GetIsNominalType()) continue;
	  if ((LAlgorithm->GetInitConditionsMP()->GetMode(i) != 1) && (LAlgorithm->GetInitConditionsMP()->GetMode(i) != 3)) continue;

	  int LFuzzySetCount = FuzzyCollection->GetFuzzySetCount();
	  if (!LFuzzySetCount) continue;

	  double LMin = FuzzyCollection->GetMinValue();
	  double LMax = FuzzyCollection->GetMaxValue();
	  double LRange = LMax - LMin;
	  double LWidth = LRange / LFuzzySetCount;
	  double LCenter = LMin;
	  for (int i = 0; i < LFuzzySetCount; ++i) {
		   IAIELinguisticFuzzySet* FuzzySet = FuzzyCollection->GetFuzzySet(i);
		   FuzzySet->SetLCenter(MIN(MAX(LCenter + RandomGenerator->RandomDbl(0.0, 1.0) * LWidth, LMin), LMax));
		   FuzzySet->SetRCenter(MIN(MAX(LCenter + RandomGenerator->RandomDbl(0.0, 1.0) * LWidth, LMin), LMax));
		   FuzzySet->SetLAlpha(LRange * RandomGenerator->RandomDbl(0.0, 1.0));
		   FuzzySet->SetRAlpha(LRange * RandomGenerator->RandomDbl(0.0, 1.0));
		   FuzzySet->SetLContrast(RandomGenerator->RandomDbl(-0.5, 5.0));
		   FuzzySet->SetRContrast(RandomGenerator->RandomDbl(-0.5, 5.0));
		   LCenter += LWidth;

		   if (LUnitData->BinaryEncoding) {
			   LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetLCenter(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]); ++BinaryValueIndex;
			   LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetRCenter(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]); ++BinaryValueIndex;
			   LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetLAlpha(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]); ++BinaryValueIndex;
			   LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetRAlpha(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]); ++BinaryValueIndex;
			   LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetLContrast(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]); ++BinaryValueIndex;
			   LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetRContrast(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]); ++BinaryValueIndex;
		   }
	  }
 }

 // - losowanie typów funkcji przynale¿noœci
 for (int i = 0; i < LFuzzyStructure->GetEnabledFuzzyCollectionCount(); ++i) {
	  IAIELinguisticFuzzyCollection* FuzzyCollection = LFuzzyStructure->GetEnabledFuzzyCollection(i);
	  if (FuzzyCollection->GetIsNominalType()) continue;
	  if ((LAlgorithm->GetInitConditionsMT()->GetMode(i) != 1) && (LAlgorithm->GetInitConditionsMT()->GetMode(i) != 3)) continue;

	  for (int i = 0; i < FuzzyCollection->GetFuzzySetCount(); ++i) {
		   IAIELinguisticFuzzySet* FuzzySet = FuzzyCollection->GetFuzzySet(i);
		   switch (RandomGenerator->RandomInt(1, 4)) {
				case 1: FuzzySet->SetMembershipFunction(fuzzyMembershipFunction1); break;
				case 2: FuzzySet->SetMembershipFunction(fuzzyMembershipFunction2); break;
				case 3: FuzzySet->SetMembershipFunction(fuzzyMembershipFunction3); break;
				case 4: FuzzySet->SetMembershipFunction(fuzzyMembershipFunction4); break;
		   }
		   //FuzzySet->SetContrastEnabled(RandomGenerator->RandomInt(0, 1) == 1); //tymczasowo
		   FuzzySet->SetCoreEnabled(RandomGenerator->RandomInt(0, 1) == 1);
	  }
 }

 // - losowanie zestawu regu³
 for (int i = 0; i < LFuzzyStructure->GetFuzzyRulebaseCount(); ++i) {
	  IAIELinguisticFuzzyRulebase* LFuzzyRulebase = LFuzzyStructure->GetFuzzyRulebase(i);
	  if ((LAlgorithm->GetInitConditionsRB()->GetMode(i) != 1) && (LAlgorithm->GetInitConditionsRB()->GetMode(i) != 3)) continue;

	  // -- dodatkowe ograniczenie maksymalnej liczby regu³
	  int LMinFuzzyRuleCount = LFuzzyRulebase->GetMinFuzzyRuleCount();
	  if (LAlgorithm->GetMaxMinRuleCountEnabled())
		  LMinFuzzyRuleCount = MIN(MAX(LFuzzyRulebase->GetMinFuzzyRuleCount(), LAlgorithm->GetMinRuleCountValue()), LFuzzyRulebase->GetMaxFuzzyRuleCount());

	  //UWAGA! Losowanie liczby regu³ z przedzia³u LMinFuzzyRuleCount - 2 * LMinFuzzyRuleCount, gdy¿
	  // losowanie z przedzia³u LMinFuzzyRuleCount - LMaxFuzzyRuleCount generuje systemy o bardzo du¿ej
	  // liczbie regu³ i prowadzi do bardzo wolnych obliczeñ w pocz¹tkowej fazie ewolucji
	  LFuzzyRulebase->SetFuzzyRuleCount((int)RandomGenerator->RandomInt(LMinFuzzyRuleCount, 2 * LMinFuzzyRuleCount));
	  for (int i = 0; i < LFuzzyRulebase->GetFuzzyRuleCount(); ++i) {
		   IAIELinguisticFuzzyRule* FuzzyRule = LFuzzyRulebase->GetFuzzyRule(i);
		   FuzzyRule->SetEnabled(true);

		   if (LUnitData->OnlyFullFuzzyRule) {
			   for (int j = 0; j < LFuzzyStructure->GetInputFuzzyAttrCount(); ++j) {
					int LFuzzySetCount = LFuzzyStructure->GetCurrentFuzzyCollection(j)->GetFuzzySetCount();
					FuzzyRule->SetInpFuzzySetIndex(j, (int)RandomGenerator->RandomInt(0, LFuzzySetCount - 1));
					FuzzyRule->SetInpFuzzySetNegated(j, (LUnitData->OnlyTrueFuzzyRule ? false : ((int)RandomGenerator->RandomInt(0, 1) == 1)));
			   }
		   } else {
			   int LAttrCount = (int)RandomGenerator->RandomInt(1, LFuzzyStructure->GetInputFuzzyAttrCount());
			   for (int i = 0; i < LFuzzyStructure->GetInputFuzzyAttrCount(); ++i) FuzzyRule->SetInpFuzzySetIndex(i, -1);
			   while(LAttrCount-- > 0) {
					int k = (int)RandomGenerator->RandomInt(0, LFuzzyStructure->GetInputFuzzyAttrCount() - 1);
					while (FuzzyRule->GetInpFuzzySetIndex(k) >= 0) k = (int)RandomGenerator->RandomInt(0, LFuzzyStructure->GetInputFuzzyAttrCount() - 1);
					int LFuzzySetCount = LFuzzyStructure->GetCurrentFuzzyCollection(k)->GetFuzzySetCount();
					FuzzyRule->SetInpFuzzySetIndex(k, (int)RandomGenerator->RandomInt(0, LFuzzySetCount - 1));
					FuzzyRule->SetInpFuzzySetNegated(k, (LUnitData->OnlyTrueFuzzyRule ? false : ((int)RandomGenerator->RandomInt(0, 1) == 1)));
			   }
		   }
		   int LFuzzySetCount = LFuzzyStructure->GetCurrentFuzzyCollection(LFuzzyRulebase->GetFuzzyAttrIndex())->GetFuzzySetCount();
		   FuzzyRule->SetOutFuzzySetIndex((int)RandomGenerator->RandomInt(0, LFuzzySetCount - 1));
	  }
 }

 // - losowanie operatorów modu³u wnioskowania
 for (int i = 0; i < LFuzzyStructure->GetFuzzyRulebaseCount(); ++i) {
	 IAIELinguisticFuzzyRulebase* LFuzzyRulebase = LFuzzyStructure->GetFuzzyRulebase(i);
	 if ((LAlgorithm->GetInitConditionsIO()->GetMode(i) == 1) || (LAlgorithm->GetInitConditionsIO()->GetMode(i) == 3)) InferenceOperatorsChange(LFuzzyRulebase, RandomGenerator);
 }
}
//---------------------------------------------------------------------------
void __stdcall CrossOverOperator(IAIEGeneticAlgorithm* GA, void* AChromosomeHandle1, void* AChromosomeHandle2)
{
 // OPERATOR KRZY¯OWANIA DWUPUNKTOWEGO (REPREZENTACJA ZMIENNOPRZECINKOWA)
 TAIEUnitDataLrn* LUnitData1 = (TAIEUnitDataLrn*)AChromosomeHandle1;
 TAIEUnitDataLrn* LUnitData2 = (TAIEUnitDataLrn*)AChromosomeHandle2;
 IAIERandomGenerator64* RandomGenerator = GA->GetRandomGenerator();

 if (!RandomGenerator->RandomInt(0, 1) && LUnitData1->RBCount) {
	 // - wymiana regu³ we wszystkich bazach regu³
	 int RFBIndex = (int)RandomGenerator->RandomInt(0, LUnitData1->RBCount - 1);
	 switch (RandomGenerator->RandomInt(0, 4)) {
		 case 0: RulebaseExchange1(LUnitData1->FuzzyStructure->GetFuzzyRulebase(LUnitData1->RB[RFBIndex]), LUnitData2->FuzzyStructure->GetFuzzyRulebase(LUnitData2->RB[RFBIndex]), RandomGenerator); break;
		 case 1: RulebaseExchange2(LUnitData1->FuzzyStructure->GetFuzzyRulebase(LUnitData1->RB[RFBIndex]), LUnitData2->FuzzyStructure->GetFuzzyRulebase(LUnitData2->RB[RFBIndex]), RandomGenerator); break;
		 case 2: RulebaseExchange3(LUnitData1->FuzzyStructure->GetFuzzyRulebase(LUnitData1->RB[RFBIndex]), LUnitData2->FuzzyStructure->GetFuzzyRulebase(LUnitData2->RB[RFBIndex]), RandomGenerator); break;
		 case 3: RulebaseExchange4(LUnitData1->FuzzyStructure->GetFuzzyRulebase(LUnitData1->RB[RFBIndex]), LUnitData2->FuzzyStructure->GetFuzzyRulebase(LUnitData2->RB[RFBIndex]), RandomGenerator); break;
		 case 4: RulebaseExchange5(LUnitData1->FuzzyStructure->GetFuzzyRulebase(LUnitData1->RB[RFBIndex]), LUnitData2->FuzzyStructure->GetFuzzyRulebase(LUnitData2->RB[RFBIndex]), RandomGenerator); break;
	 }
 } else
 if (LUnitData1->MPCount) {
	 // - wymiana parametrów zbiorów rozmytych we wszystkich zestawach zbiorów rozmytych
	 if (LUnitData1->BinaryEncoding) {
		 BinaryAttrExchange1((char*)LUnitData1->BinaryFuzzySetParamsVector, 6 * LUnitData1->FSC * sizeof(int),
							 (char*)LUnitData2->BinaryFuzzySetParamsVector, 6 * LUnitData2->FSC * sizeof(int), RandomGenerator);
	 } else {
		for (int i = 0; i < LUnitData1->MPCount; ++i)
			 AttrExchange(LUnitData1->FuzzyStructure->GetEnabledFuzzyCollection(LUnitData1->MP[i]),
						  LUnitData2->FuzzyStructure->GetEnabledFuzzyCollection(LUnitData1->MP[i]), RandomGenerator);
	 }
 }
}
//---------------------------------------------------------------------------
void __stdcall MutationOperator(IAIEGeneticAlgorithm* GA, void* AChromosomeHandle)
{
 TAIEUnitDataLrn* LUnitData = (TAIEUnitDataLrn*)AChromosomeHandle;
 IAIERandomGenerator64* RandomGenerator = GA->GetRandomGenerator();

 // - sprawdzenie ile jest mo¿liwych wyborów operacji
 int OperationCount = 0;
 if (LUnitData->RBCount > 0) ++OperationCount;
 if (LUnitData->IOCount > 0) ++OperationCount;
 if (LUnitData->MPCount > 0) ++OperationCount;
 if (LUnitData->MTCount > 0) ++OperationCount;
 if (!OperationCount) return;

 // - wybór operacji
 int Operation = -1;
 int R = (int)RandomGenerator->RandomInt(0, OperationCount - 1);
 if ((LUnitData->RBCount > 0) && (Operation < 0)) if (R-- == 0) Operation = 0;
 if ((LUnitData->IOCount > 0) && (Operation < 0)) if (R-- == 0) Operation = 1;
 if ((LUnitData->MPCount > 0) && (Operation < 0)) if (R-- == 0) Operation = 2;
 if ((LUnitData->MTCount > 0) && (Operation < 0)) if (R-- == 0) Operation = 3;
 if (Operation < 0) return;

 switch (Operation) {
	case 0: { //zmiana liczby regu³ lub przes³anki w regule
			 int FuzzyRuleIndex;
			 int FRBIndex = (int)RandomGenerator->RandomInt(0, LUnitData->RBCount - 1);
			 IAIELinguisticFuzzyRulebase* LFuzzyRulebase = LUnitData->FuzzyStructure->GetFuzzyRulebase(FRBIndex);
			 switch (RandomGenerator->RandomInt(0, 3)) {
				case 0: if (LFuzzyRulebase->GetFuzzyRuleCount() < LUnitData->MaxFuzzyRuleCount[FRBIndex]) {RuleInsert(LFuzzyRulebase, LUnitData->OnlyFullFuzzyRule, LUnitData->OnlyTrueFuzzyRule, RandomGenerator);} break;
				case 1: if (LFuzzyRulebase->GetFuzzyRuleCount() > LUnitData->MinFuzzyRuleCount[FRBIndex]) {RuleDelete(LFuzzyRulebase, RandomGenerator);} break;
				case 2:
				case 3: FuzzyRuleIndex = (int)RandomGenerator->RandomInt(0, LFuzzyRulebase->GetFuzzyRuleCount() - 1);
						RuleFuzzySetChange(LFuzzyRulebase->GetFuzzyRule(FuzzyRuleIndex), LUnitData->OnlyFullFuzzyRule, LUnitData->OnlyTrueFuzzyRule, RandomGenerator); break;
				case 4:
				case 5: RuleFuzzyAttrChange(LFuzzyRulebase->GetFuzzyRule((int)RandomGenerator->RandomInt(0, LFuzzyRulebase->GetFuzzyRuleCount() - 1)), RandomGenerator); break;
			 }
			}
			break;
	case 1: InferenceOperatorsChange(LUnitData->FuzzyStructure->GetFuzzyRulebase(LUnitData->IO[(int)RandomGenerator->RandomInt(0, LUnitData->IOCount - 1)]), RandomGenerator);
			break;
	case 2: if (LUnitData->BinaryEncoding) {
				switch (RandomGenerator->RandomInt(0, 5)) {
					case 0: BinaryAttrMutation1((char*)LUnitData->BinaryFuzzySetParamsVector, 6 * LUnitData->FSC * sizeof(int), RandomGenerator); break;
					case 1: BinaryAttrMutation2((char*)LUnitData->BinaryFuzzySetParamsVector, 6 * LUnitData->FSC * sizeof(int), RandomGenerator); break;
					case 2: BinaryAttrMutation3((char*)LUnitData->BinaryFuzzySetParamsVector, 6 * LUnitData->FSC * sizeof(int), RandomGenerator); break;
					case 3: BinaryAttrMutation4((char*)LUnitData->BinaryFuzzySetParamsVector, 6 * LUnitData->FSC * sizeof(int), RandomGenerator); break;
					case 4: BinaryAttrMutation5((char*)LUnitData->BinaryFuzzySetParamsVector, 6 * LUnitData->FSC * sizeof(int), RandomGenerator); break;
					case 5: BinaryAttrMutation6((char*)LUnitData->BinaryFuzzySetParamsVector, 6 * LUnitData->FSC * sizeof(int), RandomGenerator); break;
				}
			} else {
				MembershipParamsChange(LUnitData->FuzzyStructure->GetCurrentFuzzyCollection(LUnitData->MP[(int)RandomGenerator->RandomInt(0, LUnitData->MPCount - 1)]), RandomGenerator);
			}
			break;
	case 3: MembershipTypeChange(LUnitData->FuzzyStructure->GetCurrentFuzzyCollection(LUnitData->MT[(int)RandomGenerator->RandomInt(0, LUnitData->MTCount - 1)]), RandomGenerator);
			break;
 }
}
//---------------------------------------------------------------------------
void __stdcall ChromosomeCopy(IAIEGeneticAlgorithm* GA, void* ADstChromosomeHandle, void* ASrcChromosomeHandle)
{
 TAIEUnitDataLrn* SrcUnitData = (TAIEUnitDataLrn*)ASrcChromosomeHandle;
 TAIEUnitDataLrn* DstUnitData = (TAIEUnitDataLrn*)ADstChromosomeHandle;

 // - kopiowanie struktury
 DstUnitData->FuzzyStructure->Assign(SrcUnitData->FuzzyStructure);

 int LFuzzyRulebaseCount = DstUnitData->FuzzyStructure->GetFuzzyRulebaseCount();
 int LErrorTableCount = DstUnitData->FuzzyStructure->GetErrorTableCount();
 memcpy(DstUnitData->LrnErrors, SrcUnitData->LrnErrors, LErrorTableCount * sizeof(double));
 memcpy(DstUnitData->LrnOutputsInTol, SrcUnitData->LrnOutputsInTol, LErrorTableCount * sizeof(int));
 memcpy(DstUnitData->MinFuzzyRuleCount, SrcUnitData->MinFuzzyRuleCount, LFuzzyRulebaseCount * sizeof(int));
 memcpy(DstUnitData->MaxFuzzyRuleCount, SrcUnitData->MaxFuzzyRuleCount, LFuzzyRulebaseCount * sizeof(int));
 memcpy(DstUnitData->NormFuzzyRulebaseFuzzyRuleComplexity, SrcUnitData->NormFuzzyRulebaseFuzzyRuleComplexity, LFuzzyRulebaseCount * sizeof(double));
 memcpy(DstUnitData->NormFuzzyRulebaseFuzzyRuleCount, SrcUnitData->NormFuzzyRulebaseFuzzyRuleCount, LFuzzyRulebaseCount * sizeof(double));
 memcpy(DstUnitData->NormFuzzyRulebaseFuzzyAttrCount, SrcUnitData->NormFuzzyRulebaseFuzzyAttrCount, LFuzzyRulebaseCount * sizeof(double));
 memcpy(DstUnitData->NormFuzzyRulebaseFuzzySetCount, SrcUnitData->NormFuzzyRulebaseFuzzySetCount, LFuzzyRulebaseCount * sizeof(double));

 if (SrcUnitData->BinaryEncoding) {
	 memcpy(DstUnitData->BinaryFuzzySetParamsVector, SrcUnitData->BinaryFuzzySetParamsVector, 6 * SrcUnitData->FSC * sizeof(unsigned int));
	 memcpy(DstUnitData->BinaryFuzzySetMaxVector, SrcUnitData->BinaryFuzzySetMaxVector, 6 * SrcUnitData->FSC * sizeof(double));
	 memcpy(DstUnitData->BinaryFuzzySetMinVector, SrcUnitData->BinaryFuzzySetMinVector, 6 * SrcUnitData->FSC * sizeof(double));
 }

 DstUnitData->LrnError = SrcUnitData->LrnError;
 DstUnitData->NormalizedError = SrcUnitData->NormalizedError;
 DstUnitData->LrnVectorsInTol = SrcUnitData->LrnVectorsInTol;
 DstUnitData->FuzzyStructureComplexity = SrcUnitData->FuzzyStructureComplexity;
}
//---------------------------------------------------------------------------
double __stdcall FitnessFunction(IAIEGeneticAlgorithm* GA, void* AChromosomeHandle)
{
 TAIEUnitDataLrn* LUnitData = (TAIEUnitDataLrn*)AChromosomeHandle;
 TAIELinguisticFuzzyStructure* FuzzyStructure = LUnitData->FuzzyStructure;

 //usuwanie regu³ powtarzaj¹cych siê i pustych
 // oraz - dla klasyfikatora - sprawdzanie, czy dla wszystkich klas wystêpuj¹ regu³y
 //        (co najmniej po jednej regule na klasê)
 int LFuzzyRulebaseCount = FuzzyStructure->GetFuzzyRulebaseCount();
 int LInputFuzzyAttrCount = FuzzyStructure->GetInputFuzzyAttrCount();

 for (int i = 0; i < LFuzzyRulebaseCount; ++i) {
	  IAIELinguisticFuzzyRulebase* LFuzzyRulebase = FuzzyStructure->GetFuzzyRulebase(i);

	  for (int j = 0; j < LFuzzyRulebase->GetFuzzyRuleCount(); ++j) {
		   if (LFuzzyRulebase->GetFuzzyRuleCount() <= LUnitData->MinFuzzyRuleCount[i]) break;

		   // -- usuwanie regul pustych
		   if (LFuzzyRulebase->GetFuzzyRule(j)->GetIsEmpty()) {LFuzzyRulebase->DeleteFuzzyRule(j--); continue;}

		   // -- usuwanie regul identycznych (sprzecznych)
		   for (int jj = j+1; jj < LFuzzyRulebase->GetFuzzyRuleCount(); ++jj) {
				if (LFuzzyRulebase->GetFuzzyRuleCount() <= LUnitData->MinFuzzyRuleCount[i]) break;

				IAIELinguisticFuzzyRule* LFuzzyRule1 = LFuzzyRulebase->GetFuzzyRule(j);
				IAIELinguisticFuzzyRule* LFuzzyRule2 = LFuzzyRulebase->GetFuzzyRule(jj);

				bool IsEqual = true;
				for (int k = 0; IsEqual && (k < LInputFuzzyAttrCount); ++k)
					IsEqual = IsEqual && (LFuzzyRule1->GetInpFuzzySetIndex(k) == LFuzzyRule2->GetInpFuzzySetIndex(k));
				if (IsEqual) LFuzzyRulebase->DeleteFuzzyRule(jj--);
		   }
	  }

	  // - dodanie regu³y dla klasy (je¿eli nie istnieje co najmniej jedna regu³a dla danej klasy)
	  if (LFuzzyRulebase->GetIsNominalType()) {
		  int LFuzzyAttrIndex = LFuzzyRulebase->GetFuzzyAttrIndex();
		  int LClassCount = FuzzyStructure->GetCurrentFuzzyCollection(LFuzzyAttrIndex)->GetFuzzySetCount();
		  for (int j = 0; j < LClassCount; ++j)
			   if (LFuzzyRulebase->GetEnabledFuzzyRuleCountOfFuzzySet(LFuzzyAttrIndex, j) == 0) {
				   IAIELinguisticFuzzyRule* FuzzyRule = LFuzzyRulebase->AppendFuzzyRule();
				   if (LUnitData->OnlyFullFuzzyRule) for (int k = 1; k < LInputFuzzyAttrCount; ++k) FuzzyRule->SetInpFuzzySetIndex(k, 0);
				   FuzzyRule->SetInpFuzzySetIndex(0, 0);
				   FuzzyRule->SetOutFuzzySetIndex(j);
				   FuzzyRule->SetEnabled(true);
			   }

	  }
 }

 if (LUnitData->BinaryEncoding) {
	 //przepisanie zakodowanych binarnie parametrów do struktury rozmytej
	 int BinaryValueIndex = 0;
	 for (int i = 0; i < LUnitData->MPCount; ++i) {
		  IAIELinguisticFuzzyCollection* FuzzyCollection = LUnitData->FuzzyStructure->GetEnabledFuzzyCollection(LUnitData->MP[i]);
		  int LFuzzySetCount = FuzzyCollection->GetFuzzySetCount();
		  if (!LFuzzySetCount) continue;

		  for (int i = 0; i < LFuzzySetCount; ++i) {
			   IAIELinguisticFuzzySet* FuzzySet = FuzzyCollection->GetFuzzySet(i);
			   FuzzySet->SetLCenter(BinaryDecode(LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex])); ++BinaryValueIndex;
			   FuzzySet->SetRCenter(BinaryDecode(LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex])); ++BinaryValueIndex;
			   FuzzySet->SetLAlpha(BinaryDecode(LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex])); ++BinaryValueIndex;
			   FuzzySet->SetRAlpha(BinaryDecode(LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex])); ++BinaryValueIndex;
			   FuzzySet->SetLContrast(BinaryDecode(LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex])); ++BinaryValueIndex;
			   FuzzySet->SetRContrast(BinaryDecode(LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex])); ++BinaryValueIndex;
		  }
	 }

	 BinaryValueIndex = 0;
	 for (int i = 0; i < LUnitData->MPCount; ++i) {
		  IAIELinguisticFuzzyCollection* FuzzyCollection = LUnitData->FuzzyStructure->GetEnabledFuzzyCollection(LUnitData->MP[i]);
		  int LFuzzySetCount = FuzzyCollection->GetFuzzySetCount();
		  if (!LFuzzySetCount) continue;

		  for (int i = 0; i < LFuzzySetCount; ++i) {
			   IAIELinguisticFuzzySet* FuzzySet = FuzzyCollection->GetFuzzySet(i);
			   LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetLCenter(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]); ++BinaryValueIndex;
			   LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetRCenter(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]); ++BinaryValueIndex;
			   BinaryValueIndex += 4;
		  }
	 }
 }

 //przeliczenie b³êdu odpowiedzi dla danych ucz¹cych
 LUnitData->NormalizedError = 0.0;
 LUnitData->LrnError = LUnitData->FuzzyStructure->ExecuteError(LUnitData->DataSet, NULL, LUnitData->LrnTols, LUnitData->LrnErrors, LUnitData->LrnOutputsInTol, LUnitData->LrnVectorsInTol, LUnitData->NormalizedError);

 //przeliczanie z³o¿onoœci z uwzglêdnieniem ograniczenia liczby regu³
 LUnitData->FuzzyStructureComplexity = 0.0;

 double ParamCount = 0;
 if (LUnitData->OC_FuzzyRuleCount) ++ParamCount;
 if (LUnitData->OC_FuzzyLength) ++ParamCount;
 if (LUnitData->OC_FuzzyAttrCount) ++ParamCount;
 if (LUnitData->OC_FuzzySetCount) ++ParamCount;

 if (ParamCount > 0.0)
 for (int i = 0; i < LFuzzyRulebaseCount; ++i) {
	  IAIELinguisticFuzzyRulebase* LFuzzyRulebase = FuzzyStructure->GetFuzzyRulebase(i);

	  LUnitData->NormFuzzyRulebaseFuzzyRuleCount[i] = LFuzzyRulebase->GetNormEnabledFuzzyRuleCount(LUnitData->MinFuzzyRuleCount[i], LUnitData->MaxFuzzyRuleCount[i]);
	  LUnitData->NormFuzzyRulebaseFuzzyRuleComplexity[i] = LFuzzyRulebase->GetNormEnabledFuzzyRuleComplexity();
	  LUnitData->NormFuzzyRulebaseFuzzyAttrCount[i] = LFuzzyRulebase->GetNormEnabledFuzzyAttrCount();
	  LUnitData->NormFuzzyRulebaseFuzzySetCount[i] = LFuzzyRulebase->GetNormEnabledFuzzySetCount();

	  // - znormalizowana z³o¿onoœæ bazy regu³
	  LUnitData->FuzzyStructureComplexity += ((LUnitData->OC_FuzzyRuleCount ? LUnitData->NormFuzzyRulebaseFuzzyRuleCount[i] : 0.0) +
											  (LUnitData->OC_FuzzyLength ? LUnitData->NormFuzzyRulebaseFuzzyRuleComplexity[i] : 0.0) +
											  (LUnitData->OC_FuzzyAttrCount ? LUnitData->NormFuzzyRulebaseFuzzyAttrCount[i] : 0.0) +
											  (LUnitData->OC_FuzzySetCount ? LUnitData->NormFuzzyRulebaseFuzzySetCount[i] : 0.0)) / ParamCount;
 }

 ParamCount = 0.0;
 if (LUnitData->OC_AllFuzzyAttrCount) ++ParamCount;
 if (LUnitData->OC_AllFuzzySetCount) ++ParamCount;

 if (LFuzzyRulebaseCount > 1) {
	 if (ParamCount > 0.0) {
		 LUnitData->FuzzyStructureComplexity += ((LUnitData->OC_AllFuzzyAttrCount ? FuzzyStructure->GetNormEnabledFuzzyAttrCount() : 0.0) +
											     (LUnitData->OC_AllFuzzySetCount ? FuzzyStructure->GetNormEnabledFuzzySetCount() : 0.0)) / ParamCount;
		 LUnitData->FuzzyStructureComplexity /= LFuzzyRulebaseCount + 1;
	 } else {
		 LUnitData->FuzzyStructureComplexity /= LFuzzyRulebaseCount;
	 }
 } 
 
 return 1.0 - ((1.0 - LUnitData->RulebasePriority) * LUnitData->NormalizedError + LUnitData->RulebasePriority * LUnitData->FuzzyStructureComplexity);
}
//---------------------------------------------------------------------------
END_NAMESPACE(NAIELinguisticFuzzyStructureLrnAGAlgorithm)
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyStructureLrnAGAlgorithm
//---------------------------------------------------------------------------
using namespace NAIELinguisticFuzzyStructureLrnAGAlgorithm;
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::TAIELinguisticFuzzyStructureLrnAGAlgorithm(TAIELinguisticFuzzyStructureLrnAlgorithm* AOwner, TAIESystem* ASystem)
{
 FOwner = AOwner;
 FSystem = ASystem;
 FPopulationSize = 100;
 FSelectMethod = 1;
 FSelectionPressure = 2;
 FIsConstCrossFactor = true;
 FConstCrossFactor = 0.7;
 FElitistStrategy = true;

 FFunctCrossFactor = FSystem->InitFunction();
 FFunctCrossFactor->SetCaption("Prawdopodobieñstwo krzy¿owania");
 FFunctCrossFactor->SetTitle("Przebieg zmiennoœci prawdopodobieñstwa krzy¿owania");
 FFunctCrossFactor->SetDescription("Zmiana prawdopodobieñstwa krzy¿owania chromosomów nastêpuje po zakoñczeniu ka¿dego kolejnego pokolenia procesu uczenia.");
 FFunctCrossFactor->GetAxisX()->SetTitle("Numer pokolenia uczenia");
 FFunctCrossFactor->GetAxisX()->SetType(cfaInteger);
 FFunctCrossFactor->GetAxisY()->SetTitle("Prawd. krzy¿owania");
 FFunctCrossFactor->GetAxisY()->SetType(cfaReal);
 FFunctCrossFactor->GetAxisY()->SetMin(0.0);
 FFunctCrossFactor->GetAxisY()->SetMax(1.0);
 FFunctCrossFactor->GetScenario(1)->GetAttr()->SetY(0, 0.8);
 FFunctCrossFactor->GetScenario(1)->GetAttr()->SetY(1, 0.1);
 FFunctCrossFactor->GetScenario(1)->GetAttr()->SendToDefault();
 FFunctCrossFactor->GetScenario(1)->SetCurrent(true);
 FFunctCrossFactor->GetScenario(1)->SetDefault(true);

 FIsConstMutationFactor = true;
 FConstMutationFactor = 0.2;
 FFunctMutationFactor = FSystem->InitFunction();
 FFunctMutationFactor->SetCaption("Prawdopodobieñstwo mutacji");
 FFunctMutationFactor->SetTitle("Przebieg zmiennoœci prawdopodobieñstwa mutacji");
 FFunctMutationFactor->SetDescription("Zmiana prawdopodobieñstwa mutacji chromosomów nastêpuje po zakoñczeniu ka¿dego kolejnego pokolenia procesu uczenia.");
 FFunctMutationFactor->GetAxisX()->SetTitle("Numer pokolenia uczenia");
 FFunctMutationFactor->GetAxisX()->SetType(cfaInteger);
 FFunctMutationFactor->GetAxisY()->SetTitle("Prawd. mutacji");
 FFunctMutationFactor->GetAxisY()->SetType(cfaReal);
 FFunctMutationFactor->GetAxisY()->SetMin(0.0);
 FFunctMutationFactor->GetAxisY()->SetMax(1.0);
 FFunctMutationFactor->GetScenario(1)->GetAttr()->SetY(0, 0.1);
 FFunctMutationFactor->GetScenario(1)->GetAttr()->SetY(1, 0.8);
 FFunctMutationFactor->GetScenario(1)->GetAttr()->SendToDefault();
 FFunctMutationFactor->GetScenario(1)->SetCurrent(true);
 FFunctMutationFactor->GetScenario(1)->SetDefault(true);

 FCurrentCrossFactor = 0.0;
 FCurrentMutationFactor = 0.0;
 FCurrentFitnessValue.Resize(0);
 FCurrentMinFitnessValue = 0.0;
 FCurrentAvgFitnessValue = 0.0;
 FCurrentMaxFitnessValue = 0.0;

 FEpochMonitoringEnabled = 0;
 FEpochMonitoringFitnessValue.Resize(0);
 FEpochMonitoringMaxFitnessValue = 0.0;
 FEpochMonitoringMinFitnessValue = 0.0;
}
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::~TAIELinguisticFuzzyStructureLrnAGAlgorithm(void)
{
 FCurrentFitnessValue.Resize(0);
 for (int i = 0; i < FEpochMonitoringFitnessValue.GetCount(); ++i) delete FEpochMonitoringFitnessValue[i];
 FEpochMonitoringFitnessValue.Resize(0);

 FFunctCrossFactor->Free();
 FFunctMutationFactor->Free();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::LoadFromFile(IAIEBinaryFile* File)
{
 FPopulationSize = File->LoadInteger();
 FSelectMethod = File->LoadInteger();
 FSelectionPressure = File->LoadInteger();
 FIsConstCrossFactor = File->LoadBoolean();
 FConstCrossFactor = File->LoadDouble();
 FFunctCrossFactor->LoadFromFile(File);
 FIsConstMutationFactor = File->LoadBoolean();
 FConstMutationFactor = File->LoadDouble();
 FFunctMutationFactor->LoadFromFile(File);
 FElitistStrategy = File->LoadBoolean();
 FCurrentCrossFactor = File->LoadDouble();
 FCurrentMutationFactor = File->LoadDouble();

 FCurrentFitnessValue.Resize(FPopulationSize);
 File->LoadData((char*)FCurrentFitnessValue.GetData(), sizeof(double) * FPopulationSize);
 FCurrentMinFitnessValue = File->LoadDouble();
 FCurrentAvgFitnessValue = File->LoadDouble();
 FCurrentMaxFitnessValue = File->LoadDouble();

 FEpochMonitoringEnabled = File->LoadBoolean();
 for (int i = 0; i < FEpochMonitoringFitnessValue.GetCount(); ++i) delete FEpochMonitoringFitnessValue[i];
 FEpochMonitoringFitnessValue.Resize(File->LoadInteger());
 for (int i = 0; i < FEpochMonitoringFitnessValue.GetCount(); ++i)
	  File->LoadData((char*)(FEpochMonitoringFitnessValue[i] = new TVector<double>(FPopulationSize))->GetData(), sizeof(double) * FPopulationSize);
 FEpochMonitoringMaxFitnessValue = File->LoadDouble();
 FEpochMonitoringMinFitnessValue = File->LoadDouble();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::SaveToFile(IAIEBinaryFile* File)
{
 File->SaveInteger(FPopulationSize);
 File->SaveInteger(FSelectMethod);
 File->SaveInteger(FSelectionPressure);
 File->SaveBoolean(FIsConstCrossFactor);
 File->SaveDouble(FConstCrossFactor);
 FFunctCrossFactor->SaveToFile(File);
 File->SaveBoolean(FIsConstMutationFactor);
 File->SaveDouble(FConstMutationFactor);
 FFunctMutationFactor->SaveToFile(File);
 File->SaveBoolean(FElitistStrategy);
 File->SaveDouble(FCurrentCrossFactor);
 File->SaveDouble(FCurrentMutationFactor);

 File->SaveData((char*)FCurrentFitnessValue.GetData(), sizeof(double) * FPopulationSize);
 File->SaveDouble(FCurrentMinFitnessValue);
 File->SaveDouble(FCurrentAvgFitnessValue);
 File->SaveDouble(FCurrentMaxFitnessValue);

 File->SaveBoolean(FEpochMonitoringEnabled);
 File->SaveInteger(FEpochMonitoringFitnessValue.GetCount());
 for (int i = 0; i < FEpochMonitoringFitnessValue.GetCount(); ++i)
	  File->SaveData((char*)FEpochMonitoringFitnessValue[i]->GetData(), sizeof(double) * FPopulationSize);
 File->SaveDouble(FEpochMonitoringMaxFitnessValue);
 File->SaveDouble(FEpochMonitoringMinFitnessValue);
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetPopulationSize(void)
{
 return FPopulationSize;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::SetPopulationSize(int APopulationSize)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FPopulationSize = APopulationSize;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetSelectMethod(void)
{
 return FSelectMethod;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::SetSelectMethod(int ASelectMethod)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FSelectMethod = ASelectMethod;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetSelectionPressure(void)
{
 return FSelectionPressure;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::SetSelectionPressure(int ASelectionPressure)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FSelectionPressure = ASelectionPressure;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetIsConstCrossFactor(void)
{
 return FIsConstCrossFactor;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::SetIsConstCrossFactor(bool AIsConstCrossFactor)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FIsConstCrossFactor = AIsConstCrossFactor;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetConstCrossFactor(void)
{
 return FConstCrossFactor;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::SetConstCrossFactor(double AConstCrossFactor)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FConstCrossFactor = AConstCrossFactor;
}
//---------------------------------------------------------------------------
IAIEFunction* __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetFunctCrossFactor(void)
{
 return FFunctCrossFactor;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetIsConstMutationFactor(void)
{
 return FIsConstMutationFactor;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::SetIsConstMutationFactor(bool AIsConstMutationFactor)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FIsConstMutationFactor = AIsConstMutationFactor;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetConstMutationFactor(void)
{
 return FConstMutationFactor;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::SetConstMutationFactor(double AConstMutationFactor)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FConstMutationFactor = AConstMutationFactor;
}
//---------------------------------------------------------------------------
IAIEFunction* __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetFunctMutationFactor(void)
{
 return FFunctMutationFactor;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetElitistStrategy(void)
{
 return FElitistStrategy;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::SetElitistStrategy(bool AElitistStrategy)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FElitistStrategy = AElitistStrategy;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetCurrentCrossFactor(void)
{
 return FCurrentCrossFactor;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetCurrentMutationFactor(void)
{
 return FCurrentMutationFactor;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetCurrentFitnessValue(int AIndex)
{
 return FCurrentFitnessValue[AIndex];
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetCurrentMaxFitnessValue(void)
{
 return FCurrentMaxFitnessValue;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetCurrentAvgFitnessValue(void)
{
 return FCurrentAvgFitnessValue;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetCurrentMinFitnessValue(void)
{
 return FCurrentMinFitnessValue;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetEpochMonitoringEnabled(void)
{
 return FEpochMonitoringEnabled;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::SetEpochMonitoringEnabled(bool AEpochMonitoringEnabled)
{
 if (FOwner->GetIsPrepared()) return;
 FEpochMonitoringEnabled = AEpochMonitoringEnabled;
}
//---------------------------------------------------------------------------
int  __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetEpochMonitoringCount(void)
{
 return FEpochMonitoringFitnessValue.GetCount();
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetEpochMonitoringFitnessValue(int EpochIndex, int Index)
{
 return FEpochMonitoringFitnessValue[EpochIndex]->GetData()[Index];
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetEpochMonitoringMaxFitnessValue(void)
{
 return FEpochMonitoringMaxFitnessValue;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::GetEpochMonitoringMinFitnessValue(void)
{
 return FEpochMonitoringMinFitnessValue;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::Prepare(void)
{
 FLrnDataSet = FOwner->GetLrnDataSet();
 FTstDataSet = FOwner->GetTstDataSet();
 FFuzzyStructure = dynamic_cast<TAIELinguisticFuzzyStructure*>(FOwner->GetFuzzyStructure());

 //przeliczenie wskaŸników dla iteracji pocz¹tkowej
 if (FIsConstCrossFactor) {
	 FCurrentCrossFactor = FConstCrossFactor;
 } else {
	 FFunctCrossFactor->GetAxisX()->SetMin(0);
	 FFunctCrossFactor->GetAxisX()->SetMax(FOwner->GetLrnStopCondition()->GetEpochNumber());
	 FFunctCrossFactor->GetCurrentScenario()->Prepare();
	 FCurrentCrossFactor = FFunctCrossFactor->GetCurrentScenario()->Calculate(FOwner->FIterationIndex);
 }

 if (FIsConstMutationFactor) {
	 FCurrentMutationFactor = FConstMutationFactor;
 } else {
	 FFunctMutationFactor->GetAxisX()->SetMin(0);
	 FFunctMutationFactor->GetAxisX()->SetMax(FOwner->GetLrnStopCondition()->GetEpochNumber());
	 FFunctMutationFactor->GetCurrentScenario()->Prepare();
	 FCurrentMutationFactor = FFunctMutationFactor->GetCurrentScenario()->Calculate(FOwner->FIterationIndex);
 }

 FCurrentFitnessValue.Resize(FPopulationSize);
 FEpochMonitoringMaxFitnessValue = MIN_REAL_VALUE;
 FEpochMonitoringMinFitnessValue = MAX_REAL_VALUE;
 for (int i = 0; i < FEpochMonitoringFitnessValue.GetCount(); ++i) delete FEpochMonitoringFitnessValue[i];
 FEpochMonitoringFitnessValue.Resize(0);

 FThreadsLrn = FSystem->InitNumaThreads();
 FThreadsLrn->SetData(FOwner);
 FThreadsLrn->ExecuteNodeThreads(NumaNodePrepareLrn);
 FThreadsLrn->ExecuteNodeThreads(NumaNodeUpdateLrn);

 FGeneticAlgorithm = new TAIEGeneticAlgorithm(FSystem);
 FGeneticAlgorithm->SetData(FThreadsLrn);
 FGeneticAlgorithm->SetPopulationSize(FPopulationSize);
 FGeneticAlgorithm->SetSelectionMethod(FSelectMethod);
 FGeneticAlgorithm->SetSelectionPressure(FSelectionPressure);
 FGeneticAlgorithm->SetElitistStrategy(FElitistStrategy);
 FGeneticAlgorithm->SetCrossOverProbability(FCurrentCrossFactor);
 FGeneticAlgorithm->SetMutationProbability(FCurrentMutationFactor);
 FGeneticAlgorithm->SetChromosomeInit(ChromosomeInit);
 FGeneticAlgorithm->SetChromosomeFree(ChromosomeFree);
 FGeneticAlgorithm->SetChromosomeRand(ChromosomeRand);
 FGeneticAlgorithm->SetMutationOperator(MutationOperator);
 FGeneticAlgorithm->SetCrossOverOperator(CrossOverOperator);
 FGeneticAlgorithm->SetChromosomeCopy(ChromosomeCopy);
 FGeneticAlgorithm->SetFitnessFunction(FitnessFunction);
 FGeneticAlgorithm->Prepare(FOwner->GetRandomGeneratorSeedEnabled() ? FOwner->GetRandomGeneratorSeedValue() : GetTickCount());
 FGeneticAlgorithm->FirstGeneration();

 ChromosomeToStructure();

 SummaryLrnErros();
 SummaryTstErros();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::ExecuteNextIteration(void)
{
 //aktualizacja bie¿¹cych parametrów uczenia
 FCurrentCrossFactor = FIsConstCrossFactor ? FConstCrossFactor : FFunctCrossFactor->GetCurrentScenario()->Calculate(FOwner->FIterationIndex);
 FCurrentMutationFactor = FIsConstMutationFactor ? FConstMutationFactor : FFunctMutationFactor->GetCurrentScenario()->Calculate(FOwner->FIterationIndex);

 if (FOwner->GetIsNoiseInputs()) FThreadsLrn->ExecuteNodeThreads(NumaNodeUpdateLrn);

 FGeneticAlgorithm->SetCrossOverProbability(FCurrentCrossFactor);
 FGeneticAlgorithm->SetMutationProbability(FCurrentMutationFactor);
 FGeneticAlgorithm->NextGeneration();

 ChromosomeToStructure();

 SummaryLrnErros();
 SummaryTstErros();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::Finish(void)
{
 FGeneticAlgorithm->Free();

 FThreadsLrn->ExecuteNodeThreads(NumaNodeFinishLrn);
 FThreadsLrn->Free();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::ChromosomeToStructure(void)
{
 TAIEUnitDataLrn* LData = (TAIEUnitDataLrn*)FGeneticAlgorithm->GetBestChromosome();

 FFuzzyStructure->Assign(LData->FuzzyStructure);

 for (int i = 0; i < FPopulationSize; ++i) FCurrentFitnessValue[i] = FGeneticAlgorithm->GetCurrentFitnessValue(i);
 FCurrentMinFitnessValue = FGeneticAlgorithm->GetCurrentMinFitnessValue();
 FCurrentAvgFitnessValue = FGeneticAlgorithm->GetCurrentAvgFitnessValue();
 FCurrentMaxFitnessValue = FGeneticAlgorithm->GetCurrentMaxFitnessValue();

 FOwner->FCurrentFuzzyStructureComplexity = LData->FuzzyStructureComplexity;
 for (int i = 0; i < FOwner->FCurrentNormFuzzyRulebaseFuzzyRuleComplexity.GetCount(); ++i) {
	  FOwner->FCurrentNormFuzzyRulebaseFuzzyRuleComplexity[i] = LData->NormFuzzyRulebaseFuzzyRuleComplexity[i];
	  FOwner->FCurrentNormFuzzyRulebaseFuzzyRuleCount[i] = LData->NormFuzzyRulebaseFuzzyRuleCount[i];
	  FOwner->FCurrentNormFuzzyRulebaseFuzzyAttrCount[i] = LData->NormFuzzyRulebaseFuzzyAttrCount[i];
	  FOwner->FCurrentNormFuzzyRulebaseFuzzySetCount[i] = LData->NormFuzzyRulebaseFuzzySetCount[i];
 }

 if (FEpochMonitoringEnabled) {
	 int EpochIndex = FEpochMonitoringFitnessValue.GetCount();
	 FEpochMonitoringFitnessValue.Resize(EpochIndex + 1);
	 FEpochMonitoringFitnessValue[EpochIndex] = new TVector<double>(FPopulationSize);

	 double* LValues = FEpochMonitoringFitnessValue[EpochIndex]->GetData();
	 for (int i = 0; i < FPopulationSize; ++i) LValues[i] = FCurrentFitnessValue[i];

	 FEpochMonitoringMaxFitnessValue = MAX(FEpochMonitoringMaxFitnessValue, FGeneticAlgorithm->GetCurrentMaxFitnessValue());
	 FEpochMonitoringMinFitnessValue = MIN(FEpochMonitoringMinFitnessValue, FGeneticAlgorithm->GetCurrentMinFitnessValue());
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::SummaryLrnErros()
{
 //kopiowanie b³êdu z najlepszego chromosomu
 int LRecordCount = FLrnDataSet->GetRecordCount();
 if (!LRecordCount) {
	FOwner->SetCurrentLrnError(0.0);
	FOwner->SetCurrentNormError(0.0);
	FOwner->SetCurrentLrnRecordCountInTol(0);

	FOwner->GetLrnCurrentErrors()->SetDefaultVectorsInTol(0);
	for (int i = 0; i < FFuzzyStructure->GetErrorTableCount(); ++i) {
		FOwner->GetLrnCurrentErrors()->SetMinError(i, 0.0);
		FOwner->GetLrnCurrentErrors()->SetTol(i, FOwner->GetLrnStopCondition()->GetTol(i));
		FOwner->GetLrnCurrentErrors()->SetVectorsInTol(i, 0);
	}
	return;
 }

 TAIEUnitDataLrn* LData = (TAIEUnitDataLrn*)FGeneticAlgorithm->GetBestChromosome();
 FOwner->SetCurrentLrnError(LData->LrnError);
 FOwner->SetCurrentNormError(LData->NormalizedError);
 FOwner->SetCurrentLrnRecordCountInTol(LData->LrnVectorsInTol);

 FOwner->GetLrnCurrentErrors()->SetDefaultVectorsInTol(LRecordCount);
 for (int i = 0; i < FFuzzyStructure->GetErrorTableCount(); ++i) {
	  FOwner->GetLrnCurrentErrors()->SetMinError(i, SQRT(LData->LrnErrors[i] / LRecordCount));
	  FOwner->GetLrnCurrentErrors()->SetTol(i, FOwner->GetLrnStopCondition()->GetTol(i));
	  FOwner->GetLrnCurrentErrors()->SetVectorsInTol(i, LData->LrnOutputsInTol[i]);
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm::SummaryTstErros()
{
 int LErrorTableCount = FFuzzyStructure->GetErrorTableCount();
 int LRecordCount = FTstDataSet->GetRecordCount();
 if (!LRecordCount) {
	FOwner->SetCurrentTstError(0.0);
	FOwner->SetCurrentTstRecordCountInTol(0);

	FOwner->GetTstCurrentErrors()->SetDefaultVectorsInTol(0);
	for (int i = 0; i < LErrorTableCount; ++i) {
		FOwner->GetTstCurrentErrors()->SetMinError(i, 0.0);
		FOwner->GetTstCurrentErrors()->SetTol(i, FOwner->GetTstStopCondition()->GetTol(i));
		FOwner->GetTstCurrentErrors()->SetVectorsInTol(i, 0);
	}
	return;
 }

 double* LTstTols = new double[LErrorTableCount];
 double* LTstErrors = new double[LErrorTableCount];
 int* LTstOutputsInTol = new int[LErrorTableCount];
 for (int i = 0; i < LErrorTableCount; ++i) LTstTols[i] = FOwner->GetTstStopCondition()->GetTol(i);

 int    LTstVectorsInTol = 0;
 double LNormalizedError = 0.0;
 double LTstError = FFuzzyStructure->ExecuteError(FTstDataSet, NULL, LTstTols, LTstErrors, LTstOutputsInTol, LTstVectorsInTol, LNormalizedError);

 FOwner->SetCurrentTstError(LTstError);
 FOwner->SetCurrentTstRecordCountInTol(LTstVectorsInTol);

 FOwner->GetTstCurrentErrors()->SetDefaultVectorsInTol(LRecordCount);
 for (int i = 0; i < LErrorTableCount; ++i) {
	  FOwner->GetTstCurrentErrors()->SetMinError(i, SQRT(LTstErrors[i] / LRecordCount));
	  FOwner->GetTstCurrentErrors()->SetTol(i, LTstTols[i]);
	  FOwner->GetTstCurrentErrors()->SetVectorsInTol(i, LTstOutputsInTol[i]);
 }

 delete [] LTstTols;
 delete [] LTstErrors;
 delete [] LTstOutputsInTol;
}
//---------------------------------------------------------------------------
