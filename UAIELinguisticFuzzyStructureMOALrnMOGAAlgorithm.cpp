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
#include "UAIEMOGAVega.h"
#include "UAIEMOGASpea.h"
#include "UAIEMOGASpea2.h"
#include "UAIEMOGANsga.h"
#include "UAIEMOGANsga2.h"
#include "UAIEMOGAeNsga2.h"
#include "UAIEMOGASpea3.h"
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
#include "UAIELinguisticFuzzyStructureMOALrnAlgorithm.h"
#include "UAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm.h"
#include "UAIELinguisticFuzzyStructureGeneticOperators1.h"
#include "UAIELinguisticFuzzyStructureGeneticOperators2.h"
//---------------------------------------------------------------------------
BEGIN_NAMESPACE(NAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm)
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
 TAIELinguisticFuzzyStructureMOALrnAlgorithm* LAlgorithm = (TAIELinguisticFuzzyStructureMOALrnAlgorithm*)AThreads->GetData();
 TAIELinguisticFuzzyStructure* LFuzzyStructure = dynamic_cast<TAIELinguisticFuzzyStructure*>(LAlgorithm->GetInitFuzzyStructure());

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
 TAIELinguisticFuzzyStructureMOALrnAlgorithm* LAlgorithm = (TAIELinguisticFuzzyStructureMOALrnAlgorithm*)AThreads->GetData();
 TAIELinguisticFuzzyStructure* LFuzzyStructure = dynamic_cast<TAIELinguisticFuzzyStructure*>(LAlgorithm->GetInitFuzzyStructure());
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
void __stdcall ChromosomeInit(IAIEMultiobjectiveGeneticAlgorithm* GA, void*& AChromosomeHandle)//, int ANodeIndex, int AUnitIndex, int AThreadIndex)
{
 int ANodeIndex = 0;
 int AUnitIndex = 0;
 int AThreadIndex = 0;

 IAIENumaThreads* AThreads = (IAIENumaThreads*)GA->GetData();
 TAIENodeDataLrn* LNodeData = (TAIENodeDataLrn*)AThreads->GetNodeData(ANodeIndex);
 TAIELinguisticFuzzyStructureMOALrnAlgorithm* LAlgorithm = (TAIELinguisticFuzzyStructureMOALrnAlgorithm*)((IAIENumaThreads*)GA->GetData())->GetData();

 TAIEUnitDataLrn* LUnitData = new TAIEUnitDataLrn();//(TAIEUnitDataLrn*)AThreads->PrivateAllocMemory(ANodeIndex, AUnitIndex, sizeof(TAIEUnitDataLrn));
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
		 double LWidth = LRange / LFuzzySetCount;
		 for (int i = 0; i < LFuzzySetCount; ++i) {
			  IAIELinguisticFuzzySet* FuzzySet = FuzzyCollection->GetFuzzySet(i);

			  LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex] = LMax;
			  LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex] = LMin;
			  LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetLCenter(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]);
			  ++BinaryValueIndex;
			  LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex] = LWidth * 1.0;
			  LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex] = LWidth * 0.0;
			  LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetRCenter(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]);
			  ++BinaryValueIndex;
			  LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex] = LRange * 1.0;
			  LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex] = LRange * 0.0;
			  LUnitData->BinaryFuzzySetParamsVector[BinaryValueIndex] = BinaryEncode(FuzzySet->GetLAlpha(), LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex], LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex]);
			  ++BinaryValueIndex;
			  LUnitData->BinaryFuzzySetMaxVector[BinaryValueIndex] = LRange * 1.0;
			  LUnitData->BinaryFuzzySetMinVector[BinaryValueIndex] = LRange * 0.0;
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
void __stdcall ChromosomeFree(IAIEMultiobjectiveGeneticAlgorithm* GA, void* AChromosomeHandle)
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
void __stdcall ChromosomeRand(IAIEMultiobjectiveGeneticAlgorithm* GA, void* AChromosomeHandle)
{
 TAIEUnitDataLrn* LUnitData = (TAIEUnitDataLrn*)AChromosomeHandle;
 TAIELinguisticFuzzyStructureMOALrnAlgorithm* LAlgorithm = (TAIELinguisticFuzzyStructureMOALrnAlgorithm*)((IAIENumaThreads*)GA->GetData())->GetData();
 TAIELinguisticFuzzyStructure* LFuzzyStructure = LUnitData->FuzzyStructure;
 IAIERandomGenerator64* RandomGenerator = LAlgorithm->GetRandomGenerator();

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
void __stdcall CrossOverOperator(IAIEMultiobjectiveGeneticAlgorithm* GA, void* AChromosomeHandle1, void* AChromosomeHandle2)
{
 // OPERATOR KRZY¯OWANIA DWUPUNKTOWEGO (REPREZENTACJA ZMIENNOPRZECINKOWA)
 TAIEUnitDataLrn* LUnitData1 = (TAIEUnitDataLrn*)AChromosomeHandle1;
 TAIEUnitDataLrn* LUnitData2 = (TAIEUnitDataLrn*)AChromosomeHandle2;
 TAIELinguisticFuzzyStructureMOALrnAlgorithm* LAlgorithm = (TAIELinguisticFuzzyStructureMOALrnAlgorithm*)((IAIENumaThreads*)GA->GetData())->GetData();
 IAIERandomGenerator64* RandomGenerator = LAlgorithm->GetRandomGenerator();

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
void __stdcall MutationOperator(IAIEMultiobjectiveGeneticAlgorithm* GA, void* AChromosomeHandle)
{
 TAIEUnitDataLrn* LUnitData = (TAIEUnitDataLrn*)AChromosomeHandle;
 TAIELinguisticFuzzyStructureMOALrnAlgorithm* LAlgorithm = (TAIELinguisticFuzzyStructureMOALrnAlgorithm*)((IAIENumaThreads*)GA->GetData())->GetData();
 IAIERandomGenerator64* RandomGenerator = LAlgorithm->GetRandomGenerator();

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
void __stdcall ChromosomeCopy(IAIEMultiobjectiveGeneticAlgorithm* GA, void* ADstChromosomeHandle, void* ASrcChromosomeHandle)
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
void __stdcall FitnessFunction(IAIEMultiobjectiveGeneticAlgorithm* GA, void* AChromosomeHandle,
		int ParetoObjectiveCount, double* ParetoObjectiveValues)
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
		   if (LFuzzyRulebase->GetFuzzyRuleCount() <= LUnitData->MinFuzzyRuleCount[i]) continue;

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

 // ponowna weryfikacja czy s¹ puste lub sprzeczne regu³y
 // usuniêcie takich mo¿e siê nie powieŸæ ze wzglêdu na warunek (1 regu³a na klasê)
 bool LRuleBaseValid = true;
 for (int i = 0; i < LFuzzyRulebaseCount; ++i) {
	  IAIELinguisticFuzzyRulebase* LFuzzyRulebase = FuzzyStructure->GetFuzzyRulebase(i);

	  for (int j = 0; j < LFuzzyRulebase->GetFuzzyRuleCount(); ++j) {
		   // -- sprawdzenie czy sa puste regu³y
		   if (LFuzzyRulebase->GetFuzzyRule(j)->GetIsEmpty()) {LRuleBaseValid = false; break;}

		   // -- sprawdzenie czy s¹ regu³y identyczne (sprzeczne)
		   for (int jj = j+1; jj < LFuzzyRulebase->GetFuzzyRuleCount(); ++jj) {
				IAIELinguisticFuzzyRule* LFuzzyRule1 = LFuzzyRulebase->GetFuzzyRule(j);
				IAIELinguisticFuzzyRule* LFuzzyRule2 = LFuzzyRulebase->GetFuzzyRule(jj);

				bool IsEqual = true;
				for (int k = 0; IsEqual && (k < LInputFuzzyAttrCount); ++k)
					IsEqual = IsEqual && (LFuzzyRule1->GetInpFuzzySetIndex(k) == LFuzzyRule2->GetInpFuzzySetIndex(k));
				if (IsEqual) {LRuleBaseValid = false; break;}
		   }

           if (!LRuleBaseValid) break;
	  }
 }

 if (!LRuleBaseValid) {
	 for (int i = 0; i < ParetoObjectiveCount; ++i) ParetoObjectiveValues[i] = 1.0;
	 return;
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

// IAIELinguisticFuzzyRulebase* LFuzzyRulebase = FuzzyStructure->GetFuzzyRulebase(0);
// if (LFuzzyRulebase->EnabledFuzzyRuleCount == 2)
//	 if (LFuzzyRulebase->EnabledInpFuzzyAttrCount == 1)
//		 if (LFuzzyRulebase->EnabledFuzzySetCount == 2) {
//			 double A = LFuzzyRulebase->FuzzyRulebaseComplexity;
//		 }

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

 //Kryterium dok³adnoœci - Q_RMSE
 ParetoObjectiveValues[0] = LUnitData->NormalizedError;

 //Kryterium dok³adnoœci - Q_ACC + Q_RMSE
 //ParetoObjectiveValues[0] = (1.0 - (double)LUnitData->LrnVectorsInTol / (double)LUnitData->DataSet->GetRecordCount()) + LUnitData->NormalizedError;

 //Kryterium dok³adnoœci - Q_ACC
 //ParetoObjectiveValues[0] = 1.0 - (double)LUnitData->LrnVectorsInTol / (double)LUnitData->DataSet->GetRecordCount();

 //Kryterium z³o¿onoœci dla MOEA 2-kryterialny
 ParetoObjectiveValues[1] = LUnitData->FuzzyStructureComplexity;

 //Kryteria z³o¿onoœci dla MOEA 3-kryterialny
 /*
 ParetoObjectiveValues[1] = 0;
 ParetoObjectiveValues[2] = 0;
 for (int i = 0; i < LFuzzyRulebaseCount; ++i) {
	  ParetoObjectiveValues[1] += LUnitData->NormFuzzyRulebaseFuzzyRuleCount[i] / LFuzzyRulebaseCount;
	  ParetoObjectiveValues[2] += (LUnitData->NormFuzzyRulebaseFuzzyRuleComplexity[i] +
								   LUnitData->NormFuzzyRulebaseFuzzyAttrCount[i] +
								   LUnitData->NormFuzzyRulebaseFuzzySetCount[i]) / 3.0;
 }
 */

 //Kryteria z³o¿onoœci dla MOEA 5-kryterialny
 /*
 ParetoObjectiveValues[1] = 0;
 ParetoObjectiveValues[2] = 0;
 ParetoObjectiveValues[3] = 0;
 ParetoObjectiveValues[4] = 0;
 for (int i = 0; i < LFuzzyRulebaseCount; ++i) {
	  ParetoObjectiveValues[1] += LUnitData->NormFuzzyRulebaseFuzzyRuleCount[i] / LFuzzyRulebaseCount;
	  ParetoObjectiveValues[2] += LUnitData->NormFuzzyRulebaseFuzzyRuleComplexity[i] / LFuzzyRulebaseCount;
	  ParetoObjectiveValues[3] += LUnitData->NormFuzzyRulebaseFuzzyAttrCount[i] / LFuzzyRulebaseCount;
	  ParetoObjectiveValues[4] += LUnitData->NormFuzzyRulebaseFuzzySetCount[i] / LFuzzyRulebaseCount;
 }
 */

 //Kryteria z³o¿onosci dla MOEA wielokryterialnego (dla wielu baz regu³)
 /*
 for (int i = 0; i < LFuzzyRulebaseCount; ++i) {
	  IAIELinguisticFuzzyRulebase* LFuzzyRulebase = FuzzyStructure->GetFuzzyRulebase(i);
	  ParetoObjectiveValues[1] += LFuzzyRulebase->GetNormEnabledFuzzyRuleComplexity();
	  ParetoObjectiveValues[2] += LFuzzyRulebase->GetNormEnabledFuzzyAttrCount();
	  ParetoObjectiveValues[3] += LFuzzyRulebase->GetNormEnabledFuzzySetCount();
 }
 ParetoObjectiveValues[1] /= LFuzzyRulebaseCount;
 ParetoObjectiveValues[2] /= LFuzzyRulebaseCount;
 ParetoObjectiveValues[3] /= LFuzzyRulebaseCount;
 */
}
//---------------------------------------------------------------------------
END_NAMESPACE(NAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm)
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm
//---------------------------------------------------------------------------
using namespace NAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm;
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm(TAIELinguisticFuzzyStructureMOALrnAlgorithm* AOwner, TAIESystem* ASystem)
{
 FOwner = AOwner;
 FSystem = ASystem;
 FPopulationSize = 0;
 FMaxParetoSize = 10;
 FPopulationSizePerObjective = 0;
 FSelectMethod = 1;
 FSelectionPressure = 2;
 FIsConstCrossFactor = true;
 FConstCrossFactor = 0.7;
 FElitistStrategy = true;

 // liczba kryteriów optymalizacji i nazwy funkcji celów (2 kryteria)
 FParetoObjectiveCount = 2;
 FParetoObjectiveName.Resize(FParetoObjectiveCount);
 FParetoObjectiveName[0] = "b³¹d odpowiedzi systemu";
 FParetoObjectiveName[1] = "z³o¿onoœæ systemu";

 // liczba kryteriów optymalizacji i nazwy funkcji celów (2 kryteria)
// FParetoObjectiveCount = 3;
// FParetoObjectiveName.Resize(FParetoObjectiveCount);
// FParetoObjectiveName[0] = "b³¹d odpowiedzi systemu";
// FParetoObjectiveName[1] = "liczba regu³";
// FParetoObjectiveName[2] = "z³o¿onoœæ systemu";

 // liczba kryteriów optymalizacji i nazwy funkcji celów (5 kryteriów)
// FParetoObjectiveCount = 5;
// FParetoObjectiveName.Resize(FParetoObjectiveCount);
// FParetoObjectiveName[0] = "b³¹d odpowiedzi systemu";
// FParetoObjectiveName[1] = "liczba regu³";
// FParetoObjectiveName[2] = "œr. d³ugoœæ regu³y";
// FParetoObjectiveName[3] = "liczbya atrybutów";
// FParetoObjectiveName[4] = "liczba zbiorów rozmytych";

 //Liczba kryteriów dla MOEA wielokryterialnego (wersja dla wielu baz regu³)
// FParetoObjectiveCount = 4;
// FParetoObjectiveName.Resize(FParetoObjectiveCount);
// FParetoObjectiveName[0] = "b³¹d odpowiedzi systemu";
// FParetoObjectiveName[1] = "œr. d³ugoœæ regu³y";
// FParetoObjectiveName[2] = "œr. liczba atrybutów";
// FParetoObjectiveName[3] = "œr. liczba zbiorów rozmytych";

 FCurrentMinFitnessValue.Resize(FParetoObjectiveCount);
 FCurrentAvgFitnessValue.Resize(FParetoObjectiveCount);
 FCurrentMaxFitnessValue.Resize(FParetoObjectiveCount);

 SetPopulationSizePerObjective(50);

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
}
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::~TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm(void)
{
 SetPopulationSizePerObjective(0);

 FFunctCrossFactor->Free();
 FFunctMutationFactor->Free();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::LoadFromFile(IAIEBinaryFile* File)
{
 FType = File->LoadInteger();
 FPopulationSizePerObjective = File->LoadInteger();
 FMaxParetoSize = File->LoadInteger();
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

 FPopulationSize = FPopulationSizePerObjective * FParetoObjectiveCount;
 FCurrentFitnessValue.Resize(FPopulationSize);
 for (int i = 0; i < FPopulationSize; ++i) FCurrentFitnessValue[i].Resize(FParetoObjectiveCount);

 for (int i = 0; i < FPopulationSize; ++i)
	  File->LoadData((char*)FCurrentFitnessValue[i].GetData(), sizeof(double) * FParetoObjectiveCount);

 File->LoadData((char*)FCurrentMinFitnessValue.GetData(), sizeof(double) * FParetoObjectiveCount);
 File->LoadData((char*)FCurrentAvgFitnessValue.GetData(), sizeof(double) * FParetoObjectiveCount);
 File->LoadData((char*)FCurrentMaxFitnessValue.GetData(), sizeof(double) * FParetoObjectiveCount);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::SaveToFile(IAIEBinaryFile* File)
{
 File->SaveInteger(FType);
 File->SaveInteger(FPopulationSizePerObjective);
 File->SaveInteger(FMaxParetoSize);
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

 for (int i = 0; i < FPopulationSize; ++i)
	  File->SaveData((char*)FCurrentFitnessValue[i].GetData(), sizeof(double) * FParetoObjectiveCount);

 File->SaveData((char*)FCurrentMinFitnessValue.GetData(), sizeof(double) * FParetoObjectiveCount);
 File->SaveData((char*)FCurrentAvgFitnessValue.GetData(), sizeof(double) * FParetoObjectiveCount);
 File->SaveData((char*)FCurrentMaxFitnessValue.GetData(), sizeof(double) * FParetoObjectiveCount);
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetType(void)
{
 return FType;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::SetType(int AType)
{
 if (FOwner->GetIsPrepared()) return;
 FType = AType;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetPopulationSize(void)
{
 return FPopulationSize;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetPopulationSizePerObjective(void)
{
 return FPopulationSizePerObjective;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::SetPopulationSizePerObjective(int APopulationSizePerObjective)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;

 for (int i = 0; i < FPopulationSize; ++i) FCurrentFitnessValue[i].Resize(0);
 FPopulationSize = APopulationSizePerObjective * FParetoObjectiveCount;
 FCurrentFitnessValue.Resize(FPopulationSize);
 for (int i = 0; i < FPopulationSize; ++i) FCurrentFitnessValue[i].Resize(FParetoObjectiveCount);
 FPopulationSizePerObjective = APopulationSizePerObjective;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetMaxParetoSize(void)
{
 return FMaxParetoSize;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::SetMaxParetoSize(int AMaxParetoSize)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FMaxParetoSize = AMaxParetoSize;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetSelectMethod(void)
{
 return FSelectMethod;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::SetSelectMethod(int ASelectMethod)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FSelectMethod = ASelectMethod;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetSelectionPressure(void)
{
 return FSelectionPressure;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::SetSelectionPressure(int ASelectionPressure)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FSelectionPressure = ASelectionPressure;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetIsConstCrossFactor(void)
{
 return FIsConstCrossFactor;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::SetIsConstCrossFactor(bool AIsConstCrossFactor)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FIsConstCrossFactor = AIsConstCrossFactor;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetConstCrossFactor(void)
{
 return FConstCrossFactor;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::SetConstCrossFactor(double AConstCrossFactor)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FConstCrossFactor = AConstCrossFactor;
}
//---------------------------------------------------------------------------
IAIEFunction* __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetFunctCrossFactor(void)
{
 return FFunctCrossFactor;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetIsConstMutationFactor(void)
{
 return FIsConstMutationFactor;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::SetIsConstMutationFactor(bool AIsConstMutationFactor)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FIsConstMutationFactor = AIsConstMutationFactor;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetConstMutationFactor(void)
{
 return FConstMutationFactor;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::SetConstMutationFactor(double AConstMutationFactor)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FConstMutationFactor = AConstMutationFactor;
}
//---------------------------------------------------------------------------
IAIEFunction* __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetFunctMutationFactor(void)
{
 return FFunctMutationFactor;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetElitistStrategy(void)
{
 return FElitistStrategy;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::SetElitistStrategy(bool AElitistStrategy)
{
 if (FOwner->GetIsPrepared() || FOwner->GetIsExecuting()) return;
 FElitistStrategy = AElitistStrategy;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetCurrentCrossFactor(void)
{
 return FCurrentCrossFactor;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetCurrentMutationFactor(void)
{
 return FCurrentMutationFactor;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetCurrentFitnessValue(int AChromosomeIndex, int AObjectiveIndex)
{
 return FCurrentFitnessValue[AChromosomeIndex][AObjectiveIndex];
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetCurrentMaxFitnessValue(int AObjectiveIndex)
{
 return FCurrentMaxFitnessValue[AObjectiveIndex];
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetCurrentAvgFitnessValue(int AObjectiveIndex)
{
 return FCurrentAvgFitnessValue[AObjectiveIndex];
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetCurrentMinFitnessValue(int AObjectiveIndex)
{
 return FCurrentMinFitnessValue[AObjectiveIndex];
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetParetoObjectiveCount(void)
{
 return FParetoObjectiveCount;
}
//---------------------------------------------------------------------------
const char* __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::GetParetoObjectiveName(int ObjectiveIndex)
{
 return FParetoObjectiveName[ObjectiveIndex];
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::Prepare(void)
{
 FLrnDataSet = FOwner->GetLrnDataSet();
 FTstDataSet = FOwner->GetTstDataSet();

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

 FThreadsLrn = FSystem->InitNumaThreads();
 FThreadsLrn->SetData(FOwner);
 FThreadsLrn->ExecuteNodeThreads(NumaNodePrepareLrn);
 FThreadsLrn->ExecuteNodeThreads(NumaNodeUpdateLrn);

 switch (FType) {
		case MOGA_TYPE_SPEA:  FGeneticAlgorithm = new TAIEMultiobjectiveGeneticAlgorithmSPEA(); break;
		case MOGA_TYPE_SPEA2: FGeneticAlgorithm = new TAIEMultiobjectiveGeneticAlgorithmSPEA2(); break;
		case MOGA_TYPE_NSGA:  FGeneticAlgorithm = new TAIEMultiobjectiveGeneticAlgorithmNSGA(); break;
		case MOGA_TYPE_NSGA2: FGeneticAlgorithm = new TAIEMultiobjectiveGeneticAlgorithmNSGA2(); break;
		case MOGA_TYPE_ENSGA2: FGeneticAlgorithm = new TAIEMultiobjectiveGeneticAlgorithmENSGA2(); break;
		case MOGA_TYPE_SPEA3: FGeneticAlgorithm = new TAIEMultiobjectiveGeneticAlgorithmSPEA3(); break;
		default:			  FGeneticAlgorithm = new TAIEMultiobjectiveGeneticAlgorithmVEGA(); break;
 }
 FGeneticAlgorithm->SetData(FThreadsLrn);
 FGeneticAlgorithm->SetObjectiveCount(FParetoObjectiveCount);
 FGeneticAlgorithm->SetFrontMaxSize(FMaxParetoSize);
 FGeneticAlgorithm->SetPopulationSize(FPopulationSize);
 FGeneticAlgorithm->SetSelectionMethod(FSelectMethod);
 FGeneticAlgorithm->SetSelectionPressure(FSelectionPressure);
 FGeneticAlgorithm->SetCrossoverProbability(FCurrentCrossFactor);
 FGeneticAlgorithm->SetMutationProbability(FCurrentMutationFactor);
 FGeneticAlgorithm->SetSolutionInit(ChromosomeInit);
 FGeneticAlgorithm->SetSolutionRand(ChromosomeRand);
 FGeneticAlgorithm->SetSolutionCopy(ChromosomeCopy);
 FGeneticAlgorithm->SetSolutionFree(ChromosomeFree);
 FGeneticAlgorithm->SetMutationOperator(MutationOperator);
 FGeneticAlgorithm->SetCrossoverOperator(CrossOverOperator);
 FGeneticAlgorithm->SetFitnessFunction(FitnessFunction);
 FGeneticAlgorithm->Prepare(FOwner->GetRandomGeneratorSeedEnabled() ? FOwner->GetRandomGeneratorSeedValue() : GetTickCount());
 FGeneticAlgorithm->FirstGeneration();

 ChromosomeToStructure();

 SummaryLrnErros();
 SummaryTstErros();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::ExecuteNextIteration(void)
{
 //aktualizacja bie¿¹cych parametrów uczenia
 FCurrentCrossFactor = FIsConstCrossFactor ? FConstCrossFactor : FFunctCrossFactor->GetCurrentScenario()->Calculate(FOwner->FIterationIndex);
 FCurrentMutationFactor = FIsConstMutationFactor ? FConstMutationFactor : FFunctMutationFactor->GetCurrentScenario()->Calculate(FOwner->FIterationIndex);

 if (FOwner->GetIsNoiseInputs()) FThreadsLrn->ExecuteNodeThreads(NumaNodeUpdateLrn);

 FGeneticAlgorithm->SetCrossoverProbability(FCurrentCrossFactor);
 FGeneticAlgorithm->SetMutationProbability(FCurrentMutationFactor);

 FGeneticAlgorithm->NextGeneration();

 ChromosomeToStructure();

 SummaryLrnErros();
 SummaryTstErros();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::Finish(void)
{
 switch (FType) {
		case MOGA_TYPE_SPEA:  delete dynamic_cast<TAIEMultiobjectiveGeneticAlgorithmSPEA*>(FGeneticAlgorithm); break;
		case MOGA_TYPE_SPEA2: delete dynamic_cast<TAIEMultiobjectiveGeneticAlgorithmSPEA2*>(FGeneticAlgorithm); break;
		case MOGA_TYPE_NSGA:  delete dynamic_cast<TAIEMultiobjectiveGeneticAlgorithmNSGA*>(FGeneticAlgorithm); break;
		case MOGA_TYPE_NSGA2: delete dynamic_cast<TAIEMultiobjectiveGeneticAlgorithmNSGA2*>(FGeneticAlgorithm); break;
		case MOGA_TYPE_ENSGA2: delete dynamic_cast<TAIEMultiobjectiveGeneticAlgorithmENSGA2*>(FGeneticAlgorithm); break;
		case MOGA_TYPE_SPEA3: delete dynamic_cast<TAIEMultiobjectiveGeneticAlgorithmSPEA3*>(FGeneticAlgorithm); break;
		default:			  delete dynamic_cast<TAIEMultiobjectiveGeneticAlgorithmVEGA*>(FGeneticAlgorithm); break;
 }

 FThreadsLrn->ExecuteNodeThreads(NumaNodeFinishLrn);
 FThreadsLrn->Free();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::ChromosomeToStructure(void)
{
 FOwner->SetParetoSize(FGeneticAlgorithm->GetFinalSolutionCount());
 for (int i = 0; i < FGeneticAlgorithm->GetFinalSolutionCount(); ++i) {
	  TAIEUnitDataLrn* LData = (TAIEUnitDataLrn*)FGeneticAlgorithm->GetFinalSolution(i)->GetData();
	  TParetoSolution* LSolution = FOwner->FParetoSolutions[i];
	  LSolution->FuzzyStructure->Assign(LData->FuzzyStructure);
	  LSolution->FuzzyStructureComplexity = LData->FuzzyStructureComplexity;

	  LSolution->NormFuzzyRulebaseFuzzyRuleComplexity.Resize(LData->FuzzyStructure->GetFuzzyRulebaseCount());
	  LSolution->NormFuzzyRulebaseFuzzyRuleCount.Resize(LData->FuzzyStructure->GetFuzzyRulebaseCount());
	  LSolution->NormFuzzyRulebaseFuzzyAttrCount.Resize(LData->FuzzyStructure->GetFuzzyRulebaseCount());
	  LSolution->NormFuzzyRulebaseFuzzySetCount.Resize(LData->FuzzyStructure->GetFuzzyRulebaseCount());

	  for (int k = 0; k < LData->FuzzyStructure->GetFuzzyRulebaseCount(); ++k) {
		   LSolution->NormFuzzyRulebaseFuzzyRuleComplexity[k] = LData->NormFuzzyRulebaseFuzzyRuleComplexity[k];
		   LSolution->NormFuzzyRulebaseFuzzyRuleCount[k] = LData->NormFuzzyRulebaseFuzzyRuleCount[k];
		   LSolution->NormFuzzyRulebaseFuzzyAttrCount[k] = LData->NormFuzzyRulebaseFuzzyAttrCount[k];
		   LSolution->NormFuzzyRulebaseFuzzySetCount[k] = LData->NormFuzzyRulebaseFuzzySetCount[k];
	  }

	  LSolution->ParetoObjects.Resize(FGeneticAlgorithm->GetObjectiveCount());
	  for (int k = 0; k < FGeneticAlgorithm->GetObjectiveCount(); ++k)
		   LSolution->ParetoObjects[k] = FGeneticAlgorithm->GetFinalSolution(i)->GetObjectiveValue(k);
 }

 for (int k = 0; k < FParetoObjectiveCount; ++k) {
	  for (int i = 0; i < FPopulationSize; ++i)
		   FCurrentFitnessValue[i][k] = FGeneticAlgorithm->GetSolution(i)->GetObjectiveValue(k);
	  FCurrentMinFitnessValue[k] = FGeneticAlgorithm->GetObjectiveMaxValue(k);
	  FCurrentAvgFitnessValue[k] = FGeneticAlgorithm->GetObjectiveAvgValue(k);
	  FCurrentMaxFitnessValue[k] = FGeneticAlgorithm->GetObjectiveMinValue(k);
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::SummaryLrnErros()
{
 //kopiowanie b³êdów z najlepszych chromosomów w pareto froncie
 for (int i = 0; i < FGeneticAlgorithm->GetFinalSolutionCount(); ++i) {
	  TAIEUnitDataLrn* LData = (TAIEUnitDataLrn*)FGeneticAlgorithm->GetFinalSolution(i)->GetData();
	  TParetoSolution* LSolution = FOwner->FParetoSolutions[i];

	  int LErrorTableCount = LSolution->FuzzyStructure->GetErrorTableCount();
	  int LRecordCount = FLrnDataSet->GetRecordCount();
	  if (!LRecordCount) {
		   LSolution->LrnError = 0.0;
		   LSolution->NormError = 0.0;
		   LSolution->LrnRecordCountInTol = 0;

		   LSolution->LrnErrors->SetDefaultVectorsInTol(0);
		   for (int i = 0; i < LErrorTableCount; ++i) {
				LSolution->LrnErrors->SetMinError(i, 0.0);
				LSolution->LrnErrors->SetTol(i, FOwner->GetLrnStopCondition()->GetTol(i));
				LSolution->LrnErrors->SetVectorsInTol(i, 0);
		   }
		   continue;
	  }

	  LSolution->LrnError = LData->LrnError;
	  LSolution->NormError = LData->NormalizedError;
	  LSolution->LrnRecordCountInTol = LData->LrnVectorsInTol;

	  LSolution->LrnErrors->SetDefaultVectorsInTol(LRecordCount);
	  for (int i = 0; i < LErrorTableCount; ++i) {
		   LSolution->LrnErrors->SetMinError(i, SQRT(LData->LrnErrors[i] / LRecordCount));
		   LSolution->LrnErrors->SetTol(i, FOwner->GetLrnStopCondition()->GetTol(i));
		   LSolution->LrnErrors->SetVectorsInTol(i, LData->LrnOutputsInTol[i]);
	  }
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm::SummaryTstErros()
{
 //kopiowanie b³êdów z najlepszych chromosomów w pareto froncie
 for (int i = 0; i < FGeneticAlgorithm->GetFinalSolutionCount(); ++i) {
	  TParetoSolution* LSolution = FOwner->FParetoSolutions[i];

	  int LErrorTableCount = LSolution->FuzzyStructure->GetErrorTableCount();
	  int LRecordCount = FTstDataSet->GetRecordCount();
	  if (!LRecordCount) {
		   LSolution->TstError = 0.0;
		   LSolution->NormError = 0.0;
		   LSolution->TstRecordCountInTol = 0;

		   LSolution->TstErrors->SetDefaultVectorsInTol(0);
		   for (int i = 0; i < LErrorTableCount; ++i) {
				LSolution->TstErrors->SetMinError(i, 0.0);
				LSolution->TstErrors->SetTol(i, FOwner->GetTstStopCondition()->GetTol(i));
				LSolution->TstErrors->SetVectorsInTol(i, 0);
		   }
		   continue;
	  }

	  double* LTstTols = new double[LErrorTableCount];
	  double* LTstErrors = new double[LErrorTableCount];
	  int* LTstOutputsInTol = new int[LErrorTableCount];
	  for (int i = 0; i < LErrorTableCount; ++i) LTstTols[i] = FOwner->GetTstStopCondition()->GetTol(i);

	  int    LTstVectorsInTol = 0;
	  double LNormalizedError = 0.0;
	  double LTstError = LSolution->FuzzyStructure->ExecuteError(FTstDataSet, NULL, LTstTols, LTstErrors, LTstOutputsInTol, LTstVectorsInTol, LNormalizedError);

	  LSolution->TstError = LTstError;
	  LSolution->TstRecordCountInTol = LTstVectorsInTol;

	  LSolution->TstErrors->SetDefaultVectorsInTol(LRecordCount);
	  for (int i = 0; i < LErrorTableCount; ++i) {
		   LSolution->TstErrors->SetMinError(i, SQRT(LTstErrors[i] / LRecordCount));
		   LSolution->TstErrors->SetTol(i, LTstTols[i]);
		   LSolution->TstErrors->SetVectorsInTol(i, LTstOutputsInTol[i]);
	  }

	  delete [] LTstTols;
	  delete [] LTstErrors;
	  delete [] LTstOutputsInTol;
 }
}
//---------------------------------------------------------------------------

