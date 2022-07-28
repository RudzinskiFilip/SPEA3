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
#include "UAIEDataMatrix.h"
#include "UAIEDataMatrixColumn.h"
#include "UAIEDataMatrixRecord.h"
#include "UAIEDataMatrixNumaEx.h"
#include "UAIENumeralDataSet.h"
#include "UAIENumeralDataSetRecord.h"
#include "UAIENumeralDataSetColumn.h"
#include "UAIELinguisticFuzzyRule.h"
#include "UAIELinguisticFuzzySet.h"
#include "UAIELinguisticFuzzyStructure.h"

//---------------------------------------------------------------------------
// UWAGA!
// Konstrukcje typu:
//  new (&FFuzzyRule[i]) TAIELinguisticFuzzyRule( ...);
// mog¹ sprawiæ problemy nastêpuj¹cego rodzaju:
// - po odczytaniu wskaŸnika do regu³y, np. IAIELinguisticFuzzyRule* R = &FFuzzyRule[0];
//   oraz (w kolejnym kroku) zmianie liczby regu³,
//   wskaŸnik do tej regu³y bêdzie nieaktualny i odwo³ania do regu³y bêd¹
//   generowaæ trudne do wykrycia b³êdy!
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//BEGIN_NAMESPACE(NAIELinguisticFuzzyStructure)
//---------------------------------------------------------------------------
//END_NAMESPACE(NAIELinguisticFuzzyStructure)
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyOptions
//---------------------------------------------------------------------------
//using namespace NAIELinguisticFuzzyStructure;
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyRule::TAIELinguisticFuzzyRule(TAIELinguisticFuzzyRulebase* AOwner, int ANumaNodeIndex,
										  int AInpFuzzyAttrCount, TNumaVector<TAIELinguisticFuzzyAttr>& AInputFuzzyAttr,
										  int AOutFuzzyAttrIndex, TAIELinguisticFuzzyAttr& AOutputFuzzyAttr,
										  TAIEFuzzyTNormType ATNormType, int& AEnabledFuzzyRuleCount,
										  TNumaVector<TAIELinguisticFuzzyCollection*>& ACurrentFuzzyCollection)
		: FInputFuzzyAttr(AInputFuzzyAttr),
		  FOutputFuzzyAttr(AOutputFuzzyAttr),
		  FEnabledFuzzyRuleCount(AEnabledFuzzyRuleCount),
		  FCurrentFuzzyCollection(ACurrentFuzzyCollection)
{
 FOwner = AOwner;
 FNumaNodeIndex = ANumaNodeIndex;
 FInpFuzzyAttrCount = AInpFuzzyAttrCount;
 FOutFuzzyAttrIndex = AOutFuzzyAttrIndex;
 FInpFuzzySetIndex.SetNumaNodeIndex(FNumaNodeIndex);
 FInpFuzzySetNegated.SetNumaNodeIndex(FNumaNodeIndex);
 FInpFuzzySetIndex.Clear();

 FEnabled = true;
 FLocked = false;
 FLength = FInpFuzzyAttrCount;
 FInpFuzzySetIndex.Resize(FInpFuzzyAttrCount);
 FInpFuzzySetNegated.Resize(FInpFuzzyAttrCount);
 FOutFuzzySetIndex = 0;

 FStrength = 0.0;
 FFireCount = 0;

 SetTNormType(ATNormType);

 for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
	  FCurrentFuzzyCollection[i]->FFuzzySet[FInpFuzzySetIndex[i]].FAssignedFuzzyAntecedentCount[FOutFuzzyAttrIndex]++;
	  FInputFuzzyAttr[i].FAssignedFuzzyRuleCount[FOutFuzzyAttrIndex]++;
 }
 FCurrentFuzzyCollection[FInpFuzzyAttrCount + FOutFuzzyAttrIndex]->FFuzzySet[FOutFuzzySetIndex].FAssignedFuzzyAntecedentCount[FOutFuzzyAttrIndex]++;
 FOutputFuzzyAttr.FAssignedFuzzyRuleCount[FOutFuzzyAttrIndex]++;
 FEnabledFuzzyRuleCount++;
}
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyRule::~TAIELinguisticFuzzyRule(void)
{
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
	  FCurrentFuzzyCollection[i]->FFuzzySet[FInpFuzzySetIndex[i]].FAssignedFuzzyAntecedentCount[FOutFuzzyAttrIndex]--;
	  FInputFuzzyAttr[i].FAssignedFuzzyRuleCount[FOutFuzzyAttrIndex]--;
 }
 FCurrentFuzzyCollection[FInpFuzzyAttrCount + FOutFuzzyAttrIndex]->FFuzzySet[FOutFuzzySetIndex].FAssignedFuzzyAntecedentCount[FOutFuzzyAttrIndex]--;
 FOutputFuzzyAttr.FAssignedFuzzyRuleCount[FOutFuzzyAttrIndex]--;
 FEnabledFuzzyRuleCount--;

 FInpFuzzySetIndex.Resize(0);
 FInpFuzzySetNegated.Resize(0);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRule::Assign(TAIELinguisticFuzzyRule* Source)
{
 FInpFuzzyAttrCount = Source->FInpFuzzyAttrCount;
 FOutFuzzyAttrIndex = Source->FOutFuzzyAttrIndex;

 FInpFuzzySetIndex.Resize(FInpFuzzyAttrCount);
 FInpFuzzySetNegated.Resize(FInpFuzzyAttrCount);
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) {
	  SetInpFuzzySetIndex(i, Source->GetInpFuzzySetIndex(i));
	  SetInpFuzzySetNegated(i, Source->GetInpFuzzySetNegated(i));
 }
 SetOutFuzzySetIndex(Source->GetOutFuzzySetIndex());
 SetEnabled(Source->FEnabled);

 FLocked = Source->FLocked;
 FLength = Source->FLength;
 SetTNormType(Source->FTNormType);

 FStrength = Source->FStrength;
 FFireCount = Source->FFireCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRule::SetTNormType(TAIEFuzzyTNormType ATNormType)
{
 FTNormType = ATNormType;
 switch (FTNormType) {
	   case fuzzyTNorm1: FExecuteTNorm = &TAIELinguisticFuzzyRule::ExecuteTNorm1;
						 FExecuteDataTNorm = &TAIELinguisticFuzzyRule::ExecuteDataTNorm1;
						 break;
	   case fuzzyTNorm2: FExecuteTNorm = &TAIELinguisticFuzzyRule::ExecuteTNorm2;
						 FExecuteDataTNorm = &TAIELinguisticFuzzyRule::ExecuteDataTNorm2;
						 break;
	   case fuzzyTNorm3: FExecuteTNorm = &TAIELinguisticFuzzyRule::ExecuteTNorm3;
						 FExecuteDataTNorm = &TAIELinguisticFuzzyRule::ExecuteDataTNorm3;
						 break;
	   case fuzzyTNorm4: FExecuteTNorm = &TAIELinguisticFuzzyRule::ExecuteTNorm4;
						 FExecuteDataTNorm = &TAIELinguisticFuzzyRule::ExecuteDataTNorm4;
						 break;
	   case fuzzyTNorm5: FExecuteTNorm = &TAIELinguisticFuzzyRule::ExecuteTNorm5;
						 FExecuteDataTNorm = &TAIELinguisticFuzzyRule::ExecuteDataTNorm5;
						 break;
	   case fuzzyTNorm6: FExecuteTNorm = &TAIELinguisticFuzzyRule::ExecuteTNorm6;
						 FExecuteDataTNorm = &TAIELinguisticFuzzyRule::ExecuteDataTNorm6;
						 break;
 }
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyRulebase* __stdcall TAIELinguisticFuzzyRule::GetFuzzyRulebase(void)
{
 return FOwner;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyRule::GetEnabled(void)
{
 return FEnabled;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRule::SetEnabled(bool AEnabled)
{
 if (FEnabled == AEnabled) return;
 FEnabled = AEnabled;
 if (FEnabled) {
	for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
		 FCurrentFuzzyCollection[i]->FFuzzySet[FInpFuzzySetIndex[i]].FAssignedFuzzyAntecedentCount[FOutFuzzyAttrIndex]++;
		 FInputFuzzyAttr[i].FAssignedFuzzyRuleCount[FOutFuzzyAttrIndex]++;
	}
	FCurrentFuzzyCollection[FInpFuzzyAttrCount + FOutFuzzyAttrIndex]->FFuzzySet[FOutFuzzySetIndex].FAssignedFuzzyAntecedentCount[FOutFuzzyAttrIndex]++;
	FOutputFuzzyAttr.FAssignedFuzzyRuleCount[FOutFuzzyAttrIndex]++;
	FEnabledFuzzyRuleCount++;
 } else {
	for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
		 FCurrentFuzzyCollection[i]->FFuzzySet[FInpFuzzySetIndex[i]].FAssignedFuzzyAntecedentCount[FOutFuzzyAttrIndex]--;
		 FInputFuzzyAttr[i].FAssignedFuzzyRuleCount[FOutFuzzyAttrIndex]--;
	}
	FCurrentFuzzyCollection[FInpFuzzyAttrCount + FOutFuzzyAttrIndex]->FFuzzySet[FOutFuzzySetIndex].FAssignedFuzzyAntecedentCount[FOutFuzzyAttrIndex]--;
	FOutputFuzzyAttr.FAssignedFuzzyRuleCount[FOutFuzzyAttrIndex]--;
	FEnabledFuzzyRuleCount--;
 }
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyRule::GetLocked(void)
{
 return FLocked;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRule::SetLocked(bool ALocked)
{
 if (FLocked == ALocked) return;
 FLocked = ALocked;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyRule::GetIsEmpty(void)
{
 return FLength == 0;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRule::GetLength(void)
{
 return FLength;
}
//---------------------------------------------------------------------------
const char* __stdcall TAIELinguisticFuzzyRule::GetText(void)
{
 FRuleAsciiText = "IF ";
 bool IsFirst = true;
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) {
	  int FuzzySetIndex = FInpFuzzySetIndex[i];
	  if (FuzzySetIndex < 0) continue;
	  if (!IsFirst) FRuleAsciiText += " AND "; else IsFirst = false;
	  FRuleAsciiText += FInputFuzzyAttr[i].GetName();
	  FRuleAsciiText += FInpFuzzySetNegated[i] ? " is not " : " is ";
	  FRuleAsciiText += FCurrentFuzzyCollection[i]->GetFuzzySet(FuzzySetIndex)->GetName();
 }
 FRuleAsciiText += " THEN ";
 FRuleAsciiText += AnsiString(FOutputFuzzyAttr.GetName()) + " is " + AnsiString(FCurrentFuzzyCollection[FOutFuzzyAttrIndex + FInpFuzzyAttrCount]->GetFuzzySet(FOutFuzzySetIndex)->GetName());
 return FRuleAsciiText.c_str();
}
//---------------------------------------------------------------------------
const char* __stdcall TAIELinguisticFuzzyRule::GetPlainText(void)
{
 FRulePlainText = "{\\rtf\\ansi\\ansicpg1250{\\fonttbl\\f0\\fnil Tahoma;}\\fs16";
 FRulePlainText += "\\b IF \\b0 ";
 bool IsFirst = true;
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) {
	  int FuzzySetIndex = FInpFuzzySetIndex[i];
	  if (FuzzySetIndex < 0) continue;
	  if (!IsFirst) FRulePlainText += " \\b AND \\b0 "; else IsFirst = false;
	  FRulePlainText += FInputFuzzyAttr[i].GetName();
	  FRulePlainText += FInpFuzzySetNegated[i] ? " \\i is \\i0 \\b not \\b0 " : " \\i is \\i0 ";
	  FRulePlainText += FCurrentFuzzyCollection[i]->GetFuzzySet(FuzzySetIndex)->GetName();
 }
 FRulePlainText += " \\b THEN \\b0 ";
 FRulePlainText += AnsiString(FOutputFuzzyAttr.GetName()) + " \\i is \\i0 " + AnsiString(FCurrentFuzzyCollection[FOutFuzzyAttrIndex + FInpFuzzyAttrCount]->GetFuzzySet(FOutFuzzySetIndex)->GetName());
 FRulePlainText += "}";
 return FRulePlainText.c_str();
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRule::GetInpFuzzyAttrCount(void)
{
 return FInpFuzzyAttrCount;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRule::GetInpFuzzySetCount(int AttrIndex)
{
 return FCurrentFuzzyCollection[AttrIndex]->GetFuzzySetCount();
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRule::GetInpFuzzySetIndex(int AttrIndex)
{
 return FInpFuzzySetIndex[AttrIndex];
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRule::SetInpFuzzySetIndex(int AAttrIndex, int AFuzzySetIndex)
{
 if (FInpFuzzySetIndex[AAttrIndex] == AFuzzySetIndex) return;
 if (AFuzzySetIndex >= FCurrentFuzzyCollection[AAttrIndex]->GetFuzzySetCount()) AFuzzySetIndex = FCurrentFuzzyCollection[AAttrIndex]->GetFuzzySetCount() - 1;

 // - aktualizacja IsActive dla FuzzySet
 if (FInpFuzzySetIndex[AAttrIndex] >= 0) {
	 FCurrentFuzzyCollection[AAttrIndex]->FFuzzySet[FInpFuzzySetIndex[AAttrIndex]].FAssignedFuzzyAntecedentCount[FOutFuzzyAttrIndex]--;
	 FInputFuzzyAttr[AAttrIndex].FAssignedFuzzyRuleCount[FOutFuzzyAttrIndex]--;
	 FLength--;
 }

 FInpFuzzySetIndex[AAttrIndex] = AFuzzySetIndex;

 // - aktualizacja IsActive dla FuzzySet
 if (FInpFuzzySetIndex[AAttrIndex] >= 0) {
	 FCurrentFuzzyCollection[AAttrIndex]->FFuzzySet[FInpFuzzySetIndex[AAttrIndex]].FAssignedFuzzyAntecedentCount[FOutFuzzyAttrIndex]++;
	 FInputFuzzyAttr[AAttrIndex].FAssignedFuzzyRuleCount[FOutFuzzyAttrIndex]++;
	 FLength++;
 }
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyRule::GetInpFuzzySetNegated(int AttrIndex)
{
 return FInpFuzzySetNegated[AttrIndex];
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRule::SetInpFuzzySetNegated(int AAttrIndex, bool AFuzzySetNegated)
{
 if (FInpFuzzySetNegated[AAttrIndex] == AFuzzySetNegated) return;
 FInpFuzzySetNegated[AAttrIndex] = AFuzzySetNegated;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRule::GetOutFuzzySetIndex(void)
{
 return FOutFuzzySetIndex;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRule::GetOutFuzzySetCount(void)
{
 return FCurrentFuzzyCollection[FInpFuzzyAttrCount + FOutFuzzyAttrIndex]->GetFuzzySetCount();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRule::SetOutFuzzySetIndex(int AFuzzySetIndex)
{
 if ((FOutFuzzySetIndex == AFuzzySetIndex) || (AFuzzySetIndex < 0)) return;
 // - aktualizacja IsActive dla FuzzySet
 FCurrentFuzzyCollection[FInpFuzzyAttrCount + FOutFuzzyAttrIndex]->FFuzzySet[FOutFuzzySetIndex].FAssignedFuzzyAntecedentCount[FOutFuzzyAttrIndex]--;

 FOutFuzzySetIndex = AFuzzySetIndex;

 // - aktualizacja IsActive dla FuzzySet
 FCurrentFuzzyCollection[FInpFuzzyAttrCount + FOutFuzzyAttrIndex]->FFuzzySet[FOutFuzzySetIndex].FAssignedFuzzyAntecedentCount[FOutFuzzyAttrIndex]++;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::GetStrength(void)
{
 return FStrength;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRule::SetStrength(double AStrength)
{
 FStrength = AStrength;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRule::GetFireCount(void)
{
 return FFireCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRule::SetFireCount(int AFireCount)
{
 FFireCount = AFireCount;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::ExecuteTNorm(IAIEDataSetRecord* Record)
{
 return (this->*FExecuteTNorm)(Record);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::ExecuteTNorm1(IAIEDataSetRecord* Record)
{
// Wyznaczenie rozk³adu t-normy regu³y dla rekordu - metoda Zadeha
 double Y = 1.0;
 double X = -1.0;  //wartoœæ pomocna w sprawdzeniu, czy jakakolwiek przes³anka wystêpuje w regule
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
	  X = FCurrentFuzzyCollection[i]->ExecuteFuzzyValue(Record, i, FInpFuzzySetIndex[i]);
	  if (FInpFuzzySetNegated[i]) X = 1.0 - X;
	  Y = MIN(X, Y);
 }
 return (X >= 0.0) ? Y : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::ExecuteTNorm2(IAIEDataSetRecord* Record)
{
// Wyznaczenie rozk³adu t-normy regu³y dla rekordu - metoda algebraiczna
 double Y = 1.0;
 double X = -1.0; //wartoœæ pomocna w sprawdzeniu, czy jakakolwiek przes³anka wystêpuje w regule
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
	  X = FCurrentFuzzyCollection[i]->ExecuteFuzzyValue(Record, i, FInpFuzzySetIndex[i]);
	  if (FInpFuzzySetNegated[i]) X = 1.0 - X;
	  Y *= X;
 }
 return (X >= 0.0) ? Y : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::ExecuteTNorm3(IAIEDataSetRecord* Record)
{
// Wyznaczenie rozk³adu t-normy regu³y dla rekordu - metoda £ukasiewicza
 double Y = 1.0;
 double X = -1.0;  //wartoœæ pomocna w sprawdzeniu, czy jakakolwiek przes³anka wystêpuje w regule
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
	  X = FCurrentFuzzyCollection[i]->ExecuteFuzzyValue(Record, i, FInpFuzzySetIndex[i]);
	  if (FInpFuzzySetNegated[i]) X = 1.0 - X;
	  Y = MAX(X + Y - 1.0, 0.0);
 }
 return (X >= 0.0) ? Y : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::ExecuteTNorm4(IAIEDataSetRecord* Record)
{
// Wyznaczenie rozk³adu t-normy regu³y dla rekordu - metoda Fodora
 double Y = 1.0;
 double X = -1.0;  //wartoœæ pomocna w sprawdzeniu, czy jakakolwiek przes³anka wystêpuje w regule
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
	  X = FCurrentFuzzyCollection[i]->ExecuteFuzzyValue(Record, i, FInpFuzzySetIndex[i]);
	  if (FInpFuzzySetNegated[i]) X = 1.0 - X;
	  Y = (X + Y > 1.0) ? MIN(X, Y) : 0.0;
 }
 return (X >= 0.0) ? Y : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::ExecuteTNorm5(IAIEDataSetRecord* Record)
{
// Wyznaczenie rozk³adu t-normy regu³y dla rekordu - metoda drastyczna
 double Y = 1.0;
 double X = -1.0;  //wartoœæ pomocna w sprawdzeniu, czy jakakolwiek przes³anka wystêpuje w regule
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
	  X = FCurrentFuzzyCollection[i]->ExecuteFuzzyValue(Record, i, FInpFuzzySetIndex[i]);
	  if (FInpFuzzySetNegated[i]) X = 1.0 - X;
	  Y = (MAX(X, Y) == 1.0) ? MIN(X, Y) : 0.0;
 }
 return (X >= 0.0) ? Y : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::ExecuteTNorm6(IAIEDataSetRecord* Record)
{
// Wyznaczenie rozk³adu t-normy regu³y dla rekordu - metoda Einsteina
 double Y = 1.0;
 double X = -1.0; //wartoœæ pomocna w sprawdzeniu, czy jakakolwiek przes³anka wystêpuje w regule
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
	  X = FCurrentFuzzyCollection[i]->ExecuteFuzzyValue(Record, i, FInpFuzzySetIndex[i]);
	  if (FInpFuzzySetNegated[i]) X = 1.0 - X;
	  Y = (Y * X) / (2.0 - (X + Y - X * Y));
 }
 return (X >= 0) ? Y : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::ExecuteDataTNorm(double* Record)
{
 return (this->*FExecuteDataTNorm)(Record);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::ExecuteDataTNorm1(double* Record)
{
// Wyznaczenie rozk³adu t-normy regu³y dla rekordu - metoda Zadeha
 double Y = 1.0;
 double X = -1.0;  //wartoœæ pomocna w sprawdzeniu, czy jakakolwiek przes³anka wystêpuje w regule
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
	  X = FCurrentFuzzyCollection[i]->ExecuteDataFuzzyValue(Record, i, FInpFuzzySetIndex[i]);
	  if (FInpFuzzySetNegated[i]) X = 1.0 - X;
	  Y = MIN(X, Y);
 }
 return (X >= 0.0) ? Y : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::ExecuteDataTNorm2(double* Record)
{
// Wyznaczenie rozk³adu t-normy regu³y dla rekordu - metoda algebraiczna
 double Y = 1.0;
 double X = -1.0; //wartoœæ pomocna w sprawdzeniu, czy jakakolwiek przes³anka wystêpuje w regule
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
	  X = FCurrentFuzzyCollection[i]->ExecuteDataFuzzyValue(Record, i, FInpFuzzySetIndex[i]);
	  if (FInpFuzzySetNegated[i]) X = 1.0 - X;
	  Y *= X;
 }
 return (X >= 0.0) ? Y : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::ExecuteDataTNorm3(double* Record)
{
// Wyznaczenie rozk³adu t-normy regu³y dla rekordu - metoda £ukasiewicza
 double Y = 1.0;
 double X = -1.0;  //wartoœæ pomocna w sprawdzeniu, czy jakakolwiek przes³anka wystêpuje w regule
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
	  X = FCurrentFuzzyCollection[i]->ExecuteDataFuzzyValue(Record, i, FInpFuzzySetIndex[i]);
	  if (FInpFuzzySetNegated[i]) X = 1.0 - X;
	  Y = MAX(X + Y - 1.0, 0.0);
 }
 return (X >= 0.0) ? Y : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::ExecuteDataTNorm4(double* Record)
{
// Wyznaczenie rozk³adu t-normy regu³y dla rekordu - metoda Fodora
 double Y = 1.0;
 double X = -1.0;  //wartoœæ pomocna w sprawdzeniu, czy jakakolwiek przes³anka wystêpuje w regule
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
	  X = FCurrentFuzzyCollection[i]->ExecuteDataFuzzyValue(Record, i, FInpFuzzySetIndex[i]);
	  if (FInpFuzzySetNegated[i]) X = 1.0 - X;
	  Y = (X + Y > 1.0) ? MIN(X, Y) : 0.0;
 }
 return (X >= 0.0) ? Y : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::ExecuteDataTNorm5(double* Record)
{
// Wyznaczenie rozk³adu t-normy regu³y dla rekordu - metoda drastyczna
 double Y = 1.0;
 double X = -1.0;  //wartoœæ pomocna w sprawdzeniu, czy jakakolwiek przes³anka wystêpuje w regule
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
	  X = FCurrentFuzzyCollection[i]->ExecuteDataFuzzyValue(Record, i, FInpFuzzySetIndex[i]);
	  if (FInpFuzzySetNegated[i]) X = 1.0 - X;
	  Y = (MAX(X, Y) == 1.0) ? MIN(X, Y) : 0.0;
 }
 return (X >= 0.0) ? Y : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::ExecuteDataTNorm6(double* Record)
{
// Wyznaczenie rozk³adu t-normy regu³y dla rekordu - metoda Einsteina
 double Y = 1.0;
 double X = -1.0; //wartoœæ pomocna w sprawdzeniu, czy jakakolwiek przes³anka wystêpuje w regule
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) if (FInpFuzzySetIndex[i] >= 0) {
	  X = FCurrentFuzzyCollection[i]->ExecuteDataFuzzyValue(Record, i, FInpFuzzySetIndex[i]);
	  if (FInpFuzzySetNegated[i]) X = 1.0 - X;
	  Y = (Y * X) / (2.0 - (X + Y - X * Y));
 }
 return (X >= 0) ? Y : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRule::GetFuzzyRuleComplexity(void)
{
 // Wyznaczenie z³o¿onoœci regu³y
 int LCount = 0;
 for (int j = 0; j < FInpFuzzyAttrCount; ++j) if (FInpFuzzySetIndex[j] >= 0) ++LCount;
 return (LCount && (FInpFuzzyAttrCount > 1)) ? (double)(LCount - 1) / (double)(FInpFuzzyAttrCount - 1) : 1.0;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRule::SaveToFile(IAIEBinaryFile* File)
{
 File->SaveInteger(FInpFuzzyAttrCount);
 File->SaveInteger(FOutFuzzyAttrIndex);

 for (int i = 0; i < FInpFuzzyAttrCount; ++i) {
	  File->SaveInteger(GetInpFuzzySetIndex(i));
	  File->SaveBoolean(GetInpFuzzySetNegated(i));
 }
 File->SaveInteger(FOutFuzzySetIndex);

 File->SaveBoolean(FEnabled);
 File->SaveBoolean(FLocked);
 File->SaveInteger(FLength);
 File->SaveInteger(FTNormType);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRule::LoadFromFile(IAIEBinaryFile* File)
{
 FInpFuzzyAttrCount = File->LoadInteger();
 FOutFuzzyAttrIndex = File->LoadInteger();

 FInpFuzzySetIndex.Resize(FInpFuzzyAttrCount);
 FInpFuzzySetNegated.Resize(FInpFuzzyAttrCount);
 for (int i = 0; i < FInpFuzzyAttrCount; ++i) {
	  SetInpFuzzySetIndex(i, File->LoadInteger());
	  SetInpFuzzySetNegated(i, File->LoadBoolean());
 }
 SetOutFuzzySetIndex(File->LoadInteger());

 SetEnabled(File->LoadBoolean());
 FLocked = File->LoadBoolean();
 FLength = File->LoadInteger();
 SetTNormType((TAIEFuzzyTNormType)File->LoadInteger());
}
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyRulebase
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyRulebase::TAIELinguisticFuzzyRulebase(TAIELinguisticFuzzyStructure* AOwner, TAIESystem* ASystem,
		int ANumaNodeIndex, int AInpFuzzyAttrCount, int AOutFuzzyAttrIndex, TAIEColumnType AType,
		TAIEFuzzyTNormType ATNormType,
		TAIEFuzzySNormType ASNormType,
		TAIEImplicationOperator AImplicationOperator,
		TAIEAggregationOperator AAggregationOperator,
		TAIEInferenceMode AInferenceMode,
		TAIEDefuzzificationMethod ADefuzzificationMethod,
		TNumaVector<TAIELinguisticFuzzyAttr>& AInputFuzzyAttr,
		TNumaVector<TAIELinguisticFuzzyAttr>& AOutputFuzzyAttr,
		TNumaVector<TAIELinguisticFuzzyCollection*>& ACurrentFuzzyCollection,
		int& AIntegralProbeCount)
	  : FInputFuzzyAttr(AInputFuzzyAttr),
		FOutputFuzzyAttr(AOutputFuzzyAttr),
		FCurrentFuzzyCollection(ACurrentFuzzyCollection),
		FIntegralProbeCount(AIntegralProbeCount)
{
 FOwner = AOwner;
 FSystem = ASystem;
 FNumaNodeIndex = ANumaNodeIndex;
 FInpFuzzyAttrCount = AInpFuzzyAttrCount;
 FOutFuzzyAttrIndex = AOutFuzzyAttrIndex;
 FType = AType;
 FTNormType = ATNormType;
 FSNormType = ASNormType;
 FImplicationOperator = AImplicationOperator;
 FAggregationOperator = AAggregationOperator;
 FInferenceMode = AInferenceMode;
 FDefuzzificationMethod = ADefuzzificationMethod;
 if (GetIsNominalType()) FInferenceMode = fuzzyInferenceModeRAW;

 FEnabledFuzzyRuleCount = 0;
 FFuzzyRule.Resize(FFuzzyRuleCount = 0);
 FMinFuzzyRuleCount = 0;
 FMaxFuzzyRuleCount = 0;

 UpdateInferenceFunctionPointers();
}
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyRulebase::~TAIELinguisticFuzzyRulebase(void)
{
 for (int i = 0; i < FFuzzyRuleCount; ++i) delete FFuzzyRule[i];
 FFuzzyRule.Resize(FFuzzyRuleCount = 0);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRulebase::Assign(TAIELinguisticFuzzyRulebase* Source)
{
 FMinFuzzyRuleCount = Source->FMinFuzzyRuleCount;
 FMaxFuzzyRuleCount = Source->FMaxFuzzyRuleCount;
 FInpFuzzyAttrCount = Source->FInpFuzzyAttrCount;
 FOutFuzzyAttrIndex = Source->FOutFuzzyAttrIndex;

 SetFuzzyRuleCount(Source->FFuzzyRuleCount);
 for (int i = 0; i < FFuzzyRuleCount; ++i) FFuzzyRule[i]->Assign(Source->FFuzzyRule[i]);
 FType = Source->FType;
 FEnabledFuzzyRuleCount = Source->FEnabledFuzzyRuleCount;

 FTNormType = Source->FTNormType;
 FSNormType = Source->FSNormType;
 FImplicationOperator = Source->FImplicationOperator;
 FAggregationOperator = Source->FAggregationOperator;
 FInferenceMode = Source->FInferenceMode;
 FDefuzzificationMethod = Source->FDefuzzificationMethod;

 UpdateInferenceFunctionPointers();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRulebase::UpdateInferenceFunctionPointers(void)
{
 //aktualizacja wskaŸników do funkcji (w zale¿noœci od konfiguracji systemu)
 switch (FSNormType) {
	   case fuzzySNorm1: FExecuteSNorm = &TAIELinguisticFuzzyRulebase::ExecuteSNorm1;
						 FExecuteDataSNorm = &TAIELinguisticFuzzyRulebase::ExecuteDataSNorm1;
						 break;
	   case fuzzySNorm2: FExecuteSNorm = &TAIELinguisticFuzzyRulebase::ExecuteSNorm2;
						 FExecuteDataSNorm = &TAIELinguisticFuzzyRulebase::ExecuteDataSNorm2;
						 break;
	   case fuzzySNorm3: FExecuteSNorm = &TAIELinguisticFuzzyRulebase::ExecuteSNorm3;
						 FExecuteDataSNorm = &TAIELinguisticFuzzyRulebase::ExecuteDataSNorm3;
						 break;
	   case fuzzySNorm4: FExecuteSNorm = &TAIELinguisticFuzzyRulebase::ExecuteSNorm4;
						 FExecuteDataSNorm = &TAIELinguisticFuzzyRulebase::ExecuteDataSNorm4;
						 break;
	   case fuzzySNorm5: FExecuteSNorm = &TAIELinguisticFuzzyRulebase::ExecuteSNorm5;
						 FExecuteDataSNorm = &TAIELinguisticFuzzyRulebase::ExecuteDataSNorm5;
						 break;
	   case fuzzySNorm6: FExecuteSNorm = &TAIELinguisticFuzzyRulebase::ExecuteSNorm6;
						 FExecuteDataSNorm = &TAIELinguisticFuzzyRulebase::ExecuteDataSNorm6;
						 break;
 }

 // - ustawienia operatora implikacji
 switch (FImplicationOperator) {
	   case fuzzyImplicationOperator1:  FExecuteImplication = &TAIELinguisticFuzzyRulebase::ExecuteImplication1; break;
	   case fuzzyImplicationOperator2:  FExecuteImplication = &TAIELinguisticFuzzyRulebase::ExecuteImplication2; break;
	   case fuzzyImplicationOperator3:  FExecuteImplication = &TAIELinguisticFuzzyRulebase::ExecuteImplication3; break;
	   case fuzzyImplicationOperator4:  FExecuteImplication = &TAIELinguisticFuzzyRulebase::ExecuteImplication4; break;
	   case fuzzyImplicationOperator5:  FExecuteImplication = &TAIELinguisticFuzzyRulebase::ExecuteImplication5; break;
	   case fuzzyImplicationOperator6:  FExecuteImplication = &TAIELinguisticFuzzyRulebase::ExecuteImplication6; break;
	   case fuzzyImplicationOperator7:  FExecuteImplication = &TAIELinguisticFuzzyRulebase::ExecuteImplication7; break;
	   case fuzzyImplicationOperator8:  FExecuteImplication = &TAIELinguisticFuzzyRulebase::ExecuteImplication8; break;
	   case fuzzyImplicationOperator9:  FExecuteImplication = &TAIELinguisticFuzzyRulebase::ExecuteImplication9; break;
	   case fuzzyImplicationOperator10: FExecuteImplication = &TAIELinguisticFuzzyRulebase::ExecuteImplication10; break;
	   case fuzzyImplicationOperator11: FExecuteImplication = &TAIELinguisticFuzzyRulebase::ExecuteImplication11; break;
	   case fuzzyImplicationOperator12: FExecuteImplication = &TAIELinguisticFuzzyRulebase::ExecuteImplication12; break;
 }

 // -- ustawienia metody wyostrzania
 switch (FDefuzzificationMethod) {
	 case fuzzyDefuzzificationCOG:  FExecuteCrispValue = &TAIELinguisticFuzzyRulebase::ExecuteCrispValueCOG; break;
	 case fuzzyDefuzzificationCOA:  FExecuteCrispValue = &TAIELinguisticFuzzyRulebase::ExecuteCrispValueCOA; break;
	 case fuzzyDefuzzificationMCOA: FExecuteCrispValue = &TAIELinguisticFuzzyRulebase::ExecuteCrispValueMCOA; break;
	 case fuzzyDefuzzificationFOM:  FExecuteCrispValue = &TAIELinguisticFuzzyRulebase::ExecuteCrispValueFOM; break;
	 case fuzzyDefuzzificationMOM:  FExecuteCrispValue = &TAIELinguisticFuzzyRulebase::ExecuteCrispValueMOM; break;
	 case fuzzyDefuzzificationLOM:  FExecuteCrispValue = &TAIELinguisticFuzzyRulebase::ExecuteCrispValueLOM; break;
 }

 // - ustawienia trybu pracy, operatorów agregacji
 if (FInferenceMode == fuzzyInferenceModeFATI) {
	 // -- ustawienia dla trybu FATI
	 FExecuteOutput = &TAIELinguisticFuzzyRulebase::ExecuteOutputFATI;
	 switch (FAggregationOperator) {
		 case fuzzyAggregationOperator1: FExecuteMshpDegree  = &TAIELinguisticFuzzyRulebase::ExecuteMshpDegree1; break;
		 case fuzzyAggregationOperator2: FExecuteMshpDegree  = &TAIELinguisticFuzzyRulebase::ExecuteMshpDegree2; break;
		 case fuzzyAggregationOperator3: FExecuteMshpDegree  = &TAIELinguisticFuzzyRulebase::ExecuteMshpDegree3; break;
		 case fuzzyAggregationOperator4: FExecuteMshpDegree  = &TAIELinguisticFuzzyRulebase::ExecuteMshpDegree4; break;
		 case fuzzyAggregationOperator5: FExecuteMshpDegree  = &TAIELinguisticFuzzyRulebase::ExecuteMshpDegree5; break;
	 }
 } else
 if (FInferenceMode == fuzzyInferenceModeFITA) {
	 // -- ustawienia dla trybu FITA
	 FExecuteMshpDegree  = &TAIELinguisticFuzzyRulebase::ExecuteMshpDegree0;
	 switch (FAggregationOperator) {
		 case fuzzyAggregationOperator1: FExecuteOutput  = &TAIELinguisticFuzzyRulebase::ExecuteOutputFITA1; break;
		 case fuzzyAggregationOperator2: FExecuteOutput  = &TAIELinguisticFuzzyRulebase::ExecuteOutputFITA2; break;
		 case fuzzyAggregationOperator3: FExecuteOutput  = &TAIELinguisticFuzzyRulebase::ExecuteOutputFITA3; break;
		 case fuzzyAggregationOperator4: FExecuteOutput  = &TAIELinguisticFuzzyRulebase::ExecuteOutputFITA4; break;
		 case fuzzyAggregationOperator5: FExecuteOutput  = &TAIELinguisticFuzzyRulebase::ExecuteOutputFITA5; break;
	 }
 } else {
	 // -- ustawienia dla trybu RAW (implikacja i agregacja Mamdaniego,
	 //	   defuzyfikacja przez minimalizacjê b³êdu pomiêdzy histogramem s-morm,
	 //	   a histogramem aktywacji zbiorów wyjœciowych dla zadanej ostrej wartoœci wyjœciowej)
	 FExecuteOutput = &TAIELinguisticFuzzyRulebase::ExecuteOutputFATI;
	 FExecuteMshpDegree = &TAIELinguisticFuzzyRulebase::ExecuteMshpDegree1;
	 FExecuteCrispValue = &TAIELinguisticFuzzyRulebase::ExecuteCrispValueRAW;
 }
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyStructure* __stdcall TAIELinguisticFuzzyRulebase::GetFuzzyStructure(void)
{
 return FOwner;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::GetFuzzyAttrIndex(void)
{
 return FInpFuzzyAttrCount + FOutFuzzyAttrIndex;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::GetOutFuzzyAttrIndex(void)
{
 return FOutFuzzyAttrIndex;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::GetInpFuzzyAttrCount(void)
{
 return FInpFuzzyAttrCount;
}
//---------------------------------------------------------------------------
TAIEColumnType __stdcall TAIELinguisticFuzzyRulebase::GetType(void)
{
 return FType;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyRulebase::GetIsNominalType(void)
{
 return (FType == ctString) || (FType == ctBoolean);
}
//---------------------------------------------------------------------------
TAIEFuzzyTNormType __stdcall TAIELinguisticFuzzyRulebase::GetTNormType(void)
{
 return FTNormType;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRulebase::SetTNormType(TAIEFuzzyTNormType ATNormType)
{
 FTNormType = ATNormType;
 for (int i = 0; i < FFuzzyRuleCount; ++i) FFuzzyRule[i]->SetTNormType(FTNormType);
// for (int i = 0; i < FFuzzyRuleCount; ++i) FFuzzyRule[i].SetTNormType(FTNormType);
}
//---------------------------------------------------------------------------
TAIEFuzzySNormType __stdcall TAIELinguisticFuzzyRulebase::GetSNormType(void)
{
 return FSNormType;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRulebase::SetSNormType(TAIEFuzzySNormType ASNormType)
{
 FSNormType = ASNormType;
 UpdateInferenceFunctionPointers();
}
//---------------------------------------------------------------------------
TAIEImplicationOperator __stdcall TAIELinguisticFuzzyRulebase::GetImplicationOperator(void)
{
 if (GetInferenceMode() == fuzzyInferenceModeRAW) return fuzzyImplicationOperator1;
 return FImplicationOperator;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRulebase::SetImplicationOperator(TAIEImplicationOperator AImplicationOperator)
{
 if (GetIsNominalType() && (AImplicationOperator != fuzzyImplicationOperator1)) return;
 FImplicationOperator = AImplicationOperator;
 UpdateInferenceFunctionPointers();
}
//---------------------------------------------------------------------------
TAIEAggregationOperator __stdcall TAIELinguisticFuzzyRulebase::GetAggregationOperator(void)
{
 if (GetInferenceMode() == fuzzyInferenceModeRAW) return fuzzyAggregationOperator1;
 return FAggregationOperator;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRulebase::SetAggregationOperator(TAIEAggregationOperator AAggregationOperator)
{
 if (GetIsNominalType() && (AAggregationOperator != fuzzyAggregationOperator1)) return;
 FAggregationOperator = AAggregationOperator;
 UpdateInferenceFunctionPointers();
}
//---------------------------------------------------------------------------
TAIEInferenceMode __stdcall TAIELinguisticFuzzyRulebase::GetInferenceMode(void)
{
 if (GetIsNominalType()) return fuzzyInferenceModeRAW;
 return FInferenceMode;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRulebase::SetInferenceMode(TAIEInferenceMode AInferenceMode)
{
 if (GetIsNominalType()) return;
 FInferenceMode = AInferenceMode;
 UpdateInferenceFunctionPointers();
}
//---------------------------------------------------------------------------
TAIEDefuzzificationMethod __stdcall TAIELinguisticFuzzyRulebase::GetDefuzzificationMethod(void)
{
 if (GetInferenceMode() == fuzzyInferenceModeRAW) return (TAIEDefuzzificationMethod)-1;
 return FDefuzzificationMethod;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRulebase::SetDefuzzificationMethod(TAIEDefuzzificationMethod ADefuzzificationMethod)
{
 if (GetIsNominalType()) return;
 FDefuzzificationMethod = ADefuzzificationMethod;
 UpdateInferenceFunctionPointers();
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::GetMinFuzzyRuleCount(void)
{
 return FMinFuzzyRuleCount;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::GetMaxFuzzyRuleCount(void)
{
 return FMaxFuzzyRuleCount;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::GetFuzzyRuleCount(void)
{
 return FFuzzyRuleCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRulebase::SetFuzzyRuleCount(int ACount)
{
 if (ACount > FMaxFuzzyRuleCount) FSystem->RaiseException("Liczba regu³ nie mo¿e byæ wiêksza od " + IntToStr(FMaxFuzzyRuleCount) + "!");
 if (ACount < FMinFuzzyRuleCount) FSystem->RaiseException("Liczba regu³ nie mo¿e byæ mniejsza od " + IntToStr(FMinFuzzyRuleCount) + "!");

 int LCount = FFuzzyRuleCount;
 //if (LCount > ACount) for (int i = ACount; i < LCount; ++i) FFuzzyRule[i].~TAIELinguisticFuzzyRule();
 if (LCount > ACount) for (int i = ACount; i < LCount; ++i) delete FFuzzyRule[i];
 FFuzzyRule.Resize(FFuzzyRuleCount = ACount);
 if (LCount < ACount) for (int i = LCount; i < ACount; ++i)
		  FFuzzyRule[i] = new TAIELinguisticFuzzyRule(this, FNumaNodeIndex, FInpFuzzyAttrCount, FInputFuzzyAttr,
													   FOutFuzzyAttrIndex, FOutputFuzzyAttr[FOutFuzzyAttrIndex],
													   FTNormType, FEnabledFuzzyRuleCount, FCurrentFuzzyCollection);
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::GetEnabledFuzzyRuleCount(void)
{
 return FEnabledFuzzyRuleCount;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::GetEnabledInpFuzzyAttrCount(void)
{
 int LCount = 0;
 for (int i = 0; i < FInputFuzzyAttr.GetCount(); ++i)
	  if (FInputFuzzyAttr[i].GetAssignedFuzzyRuleCount(FOutFuzzyAttrIndex) != 0) ++LCount;
 return LCount;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::GetEnabledOutFuzzyAttrCount(void)
{
 int LCount = 0;
 for (int i = 0; i < FOutputFuzzyAttr.GetCount(); ++i)
	  if (FOutputFuzzyAttr[i].GetAssignedFuzzyRuleCount(FOutFuzzyAttrIndex) != 0) ++LCount;
 return LCount;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::GetEnabledFuzzySetCount(void)
{
 int LCount = 0;
 for (int i = 0; i < FOwner->GetEnabledFuzzyCollectionCount(); ++i) {
	  IAIELinguisticFuzzyCollection* LFuzzyCollection = FOwner->GetEnabledFuzzyCollection(i);
	  if (LFuzzyCollection->GetIsNominalType() && (i >= FInpFuzzyAttrCount)) continue;

	  for (int j = 0; j < LFuzzyCollection->GetFuzzySetCount(); ++j)
		   if (LFuzzyCollection->GetFuzzySet(j)->GetAssignedFuzzyAntecedentCount(FOutFuzzyAttrIndex) != 0) ++LCount;
 }

 return LCount;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::GetEnabledFuzzyRuleCountOfFuzzySet(int AFuzzyAttrIndex, int AFuzzySetIndex)
{
 int LCount = 0;
 if (AFuzzyAttrIndex == FInpFuzzyAttrCount + FOutFuzzyAttrIndex) {
	 for (int i = 0; i < FFuzzyRuleCount; ++i)
		  if (FFuzzyRule[i]->GetEnabled()) if (FFuzzyRule[i]->GetOutFuzzySetIndex() == AFuzzySetIndex) ++LCount;
 } else
 if (AFuzzyAttrIndex < FInpFuzzyAttrCount) {
	 for (int i = 0; i < FFuzzyRuleCount; ++i)
		  if (FFuzzyRule[i]->GetEnabled()) if (FFuzzyRule[i]->GetInpFuzzySetIndex(AFuzzyAttrIndex) == AFuzzySetIndex) ++LCount;
 }
 return LCount;
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyRule* __stdcall TAIELinguisticFuzzyRulebase::GetFuzzyRule(int ARuleIndex)
{
 //return &FFuzzyRule[ARuleIndex];
 return FFuzzyRule[ARuleIndex];
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyRule* __stdcall TAIELinguisticFuzzyRulebase::AppendFuzzyRule(void)
{
 if (FFuzzyRuleCount >= FMaxFuzzyRuleCount) FSystem->RaiseException("Liczba regu³ nie mo¿e byæ wiêksza od " + IntToStr(FMaxFuzzyRuleCount) + "!");

 FFuzzyRule.Resize(++FFuzzyRuleCount);
 //return new (&FFuzzyRule[FFuzzyRuleCount - 1]) TAIELinguisticFuzzyRule(this, FNumaNodeIndex, FInpFuzzyAttrCount, FInputFuzzyAttr,
 return FFuzzyRule[FFuzzyRuleCount - 1] = new TAIELinguisticFuzzyRule(this, FNumaNodeIndex, FInpFuzzyAttrCount, FInputFuzzyAttr,
																			 FOutFuzzyAttrIndex, FOutputFuzzyAttr[FOutFuzzyAttrIndex],
																			 FTNormType, FEnabledFuzzyRuleCount, FCurrentFuzzyCollection);
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyRule* __stdcall TAIELinguisticFuzzyRulebase::InsertFuzzyRule(int Index)
{
 if (FFuzzyRuleCount >= FMaxFuzzyRuleCount) FSystem->RaiseException("Liczba regu³ nie mo¿e byæ wiêksza od " + IntToStr(FMaxFuzzyRuleCount) + "!");

 if (Index >= FFuzzyRuleCount) return AppendFuzzyRule();
 FFuzzyRule.Insert(Index, 1); ++FFuzzyRuleCount;
 //return new (&FFuzzyRule[Index]) TAIELinguisticFuzzyRule(this, FNumaNodeIndex, FInpFuzzyAttrCount, FInputFuzzyAttr,
 return FFuzzyRule[Index] = new TAIELinguisticFuzzyRule(this, FNumaNodeIndex, FInpFuzzyAttrCount, FInputFuzzyAttr,
														 FOutFuzzyAttrIndex, FOutputFuzzyAttr[FOutFuzzyAttrIndex],
														 FTNormType, FEnabledFuzzyRuleCount, FCurrentFuzzyCollection);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRulebase::DeleteFuzzyRule(int Index)
{
 if (FFuzzyRuleCount <= FMinFuzzyRuleCount) FSystem->RaiseException("Liczba regu³ nie mo¿e byæ mniejsza od " + IntToStr(FMinFuzzyRuleCount) + "!");
 //FFuzzyRule[Index].~TAIELinguisticFuzzyRule();
 delete FFuzzyRule[Index];
 FFuzzyRule.Delete(Index, 1); --FFuzzyRuleCount;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRulebase::SaveToFile(IAIEBinaryFile* File)
{
 File->SaveString("FUZZY_RULEBASE_MARK");
 File->SaveInteger(FInpFuzzyAttrCount);
 File->SaveInteger(FOutFuzzyAttrIndex);
 File->SaveInteger(FType);
 File->SaveInteger(FMinFuzzyRuleCount);
 File->SaveInteger(FMaxFuzzyRuleCount);

 File->SaveInteger(FFuzzyRuleCount);
 //for (int i = 0; i < FFuzzyRuleCount; ++i) FFuzzyRule[i].SaveToFile(File);
 for (int i = 0; i < FFuzzyRuleCount; ++i) FFuzzyRule[i]->SaveToFile(File);

 File->SaveInteger(FTNormType);
 File->SaveInteger(FSNormType);
 File->SaveInteger(FImplicationOperator);
 File->SaveInteger(FAggregationOperator);
 File->SaveInteger(FInferenceMode);
 File->SaveInteger(FDefuzzificationMethod);

 File->SaveInteger(FEnabledFuzzyRuleCount);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRulebase::LoadFromFile(IAIEBinaryFile* File)
{
 File->CheckMark("FUZZY_RULEBASE_MARK");
 FInpFuzzyAttrCount = File->LoadInteger();
 FOutFuzzyAttrIndex = File->LoadInteger();
 FType = (TAIEColumnType)File->LoadInteger();
 FMinFuzzyRuleCount = File->LoadInteger();
 FMaxFuzzyRuleCount = File->LoadInteger();

 SetFuzzyRuleCount(File->LoadInteger());
 //for (int i = 0; i < FFuzzyRuleCount; ++i) FFuzzyRule[i].LoadFromFile(File);
 for (int i = 0; i < FFuzzyRuleCount; ++i) FFuzzyRule[i]->LoadFromFile(File);

 FTNormType = (TAIEFuzzyTNormType)File->LoadInteger();
 FSNormType = (TAIEFuzzySNormType)File->LoadInteger();
 FImplicationOperator = (TAIEImplicationOperator)File->LoadInteger();
 FAggregationOperator = (TAIEAggregationOperator)File->LoadInteger();
 FInferenceMode = (TAIEInferenceMode)File->LoadInteger();
 FDefuzzificationMethod = (TAIEDefuzzificationMethod)File->LoadInteger();
 if (GetIsNominalType()) FInferenceMode = fuzzyInferenceModeRAW;

 FEnabledFuzzyRuleCount = File->LoadInteger();

 UpdateInferenceFunctionPointers();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyRulebase::PrepareFuzzyRulebase(void)
{
 // Wyznaczenie pomocniczych danych w celu obliczeñ
 // - wyznaczenie zestawu i liczby zbiorów rozmytych dla wyjœciowego atrybutu
 FOutFuzzyCollection = FCurrentFuzzyCollection[FOutFuzzyAttrIndex + FInpFuzzyAttrCount];
 FOutFuzzySetCount = FOutFuzzyCollection->GetFuzzySetCount();
 FOutMinValue = FOutFuzzyCollection->GetMinValue();
 FOutMaxValue = FOutFuzzyCollection->GetMaxValue();

 for (int i = 0; i < FFuzzyRuleCount; ++i) {
	  TAIELinguisticFuzzyRule* LFuzzyRule = FFuzzyRule[i];
	  for (int j = 0; j < FInpFuzzyAttrCount; ++j) if (LFuzzyRule->GetInpFuzzySetIndex(j) >= 0) {
		   FCurrentFuzzyCollection[j]->FFuzzySet[LFuzzyRule->GetInpFuzzySetIndex(j)].FAssignedFuzzyAntecedentCount[FOutFuzzyAttrIndex]++;
		   FInputFuzzyAttr[j].FAssignedFuzzyRuleCount[FOutFuzzyAttrIndex]++;
	  }
	  FCurrentFuzzyCollection[FInpFuzzyAttrCount + FOutFuzzyAttrIndex]->FFuzzySet[LFuzzyRule->GetOutFuzzySetIndex()].FAssignedFuzzyAntecedentCount[FOutFuzzyAttrIndex]++;
	  FOutputFuzzyAttr[FOutFuzzyAttrIndex].FAssignedFuzzyRuleCount[FOutFuzzyAttrIndex]++;
 }

 // - wyznaczenie dopuszczalnej liczby regu³
 double LCount = 1;
 for (int i = 0; i < FInpFuzzyAttrCount; ++i)
	  if ((LCount *= FCurrentFuzzyCollection[i]->GetFuzzySetCount() + 1) > 1E6) break;

 FMinFuzzyRuleCount = MAX((GetIsNominalType() ? FOutFuzzySetCount : 1), FOwner->GetMinFuzzyRuleCount());
 FMaxFuzzyRuleCount = MIN(MAX((int)LCount - 1, FMinFuzzyRuleCount), FOwner->GetMaxFuzzyRuleCount());

 if (FFuzzyRuleCount > FMaxFuzzyRuleCount) SetFuzzyRuleCount(FMaxFuzzyRuleCount);
 if (FFuzzyRuleCount < FMinFuzzyRuleCount) SetFuzzyRuleCount(FMinFuzzyRuleCount);
}
//---------------------------------------------------------------------------
// Wyznaczenie rozk³adu s-norm dla rekordu w tabeli tymczasowej,
// wszystkich regu³ po zgrupowaniu wzglêdem tych samych nastêpników
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::ExecuteSNorm(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex)
{
 memset(SNormTable, 0, FOutFuzzySetCount * sizeof(double));
 return (this->*FExecuteSNorm)(Record, SNormTable, RuleIndex);
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::ExecuteSNorm1(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex)
{
// Wyznaczenie rozk³adu s-norm dla rekordu w tabeli tymczasowej - metoda Zadeha
 int ID, BID = -1; double BY = -1.0;
 TAIELinguisticFuzzyRule* LFuzzyRule;
 //for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = &FFuzzyRule[i])->GetEnabled()) {
 for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = FFuzzyRule[i])->GetEnabled()) {
	  double& Y = SNormTable[ID = LFuzzyRule->GetOutFuzzySetIndex()];
	  double  X = LFuzzyRule->ExecuteTNorm(Record);

	  Y = MAX(X, Y);

	  if (BY < Y) {BY = Y; BID = ID; RuleIndex = i;}
 }
 return BID;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::ExecuteSNorm2(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex)
{
// Wyznaczenie rozk³adu s-norm dla rekordu w tabeli tymczasowej - metoda probabilistyczna
 int ID, BID = -1; double BY = -1.0;
 TAIELinguisticFuzzyRule* LFuzzyRule;
 //for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = &FFuzzyRule[i])->GetEnabled()) {
 for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = FFuzzyRule[i])->GetEnabled()) {
	  double& Y = SNormTable[ID = LFuzzyRule->GetOutFuzzySetIndex()];
	  double  X = LFuzzyRule->ExecuteTNorm(Record);

	  Y = X + Y - X * Y;

	  if (BY < Y) {BY = Y; BID = ID; RuleIndex = i;}
 }
 return BID;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::ExecuteSNorm3(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex)
{
// Wyznaczenie rozk³adu s-norm dla rekordu w tabeli tymczasowej - metoda £ukasiewicza
 int ID, BID = -1; double BY = -1.0;
 TAIELinguisticFuzzyRule* LFuzzyRule;
 //for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = &FFuzzyRule[i])->GetEnabled()) {
 for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = FFuzzyRule[i])->GetEnabled()) {
	  double& Y = SNormTable[ID = LFuzzyRule->GetOutFuzzySetIndex()];
	  double  X = LFuzzyRule->ExecuteTNorm(Record);

	  Y = MIN(X + Y, 1.0);

	  if (BY < Y) {BY = Y; BID = ID; RuleIndex = i;}
 }
 return BID;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::ExecuteSNorm4(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex)
{
// Wyznaczenie rozk³adu s-norm dla rekordu w tabeli tymczasowej - metoda Fodora
 int ID, BID = -1; double BY = -1.0;
 TAIELinguisticFuzzyRule* LFuzzyRule;
 //for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = &FFuzzyRule[i])->GetEnabled()) {
 for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = FFuzzyRule[i])->GetEnabled()) {
	  double& Y = SNormTable[ID = LFuzzyRule->GetOutFuzzySetIndex()];
	  double  X = LFuzzyRule->ExecuteTNorm(Record);

	  Y = (X + Y < 1.0) ? MAX(X, Y) : 1.0;

	  if (BY < Y) {BY = Y; BID = ID; RuleIndex = i;}
 }
 return BID;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::ExecuteSNorm5(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex)
{
// Wyznaczenie rozk³adu s-norm dla rekordu w tabeli tymczasowej - metoda drastyczna
 int ID, BID = -1; double BY = -1.0;
 TAIELinguisticFuzzyRule* LFuzzyRule;
 //for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = &FFuzzyRule[i])->GetEnabled()) {
 for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = FFuzzyRule[i])->GetEnabled()) {
	  double& Y = SNormTable[ID = LFuzzyRule->GetOutFuzzySetIndex()];
	  double  X = LFuzzyRule->ExecuteTNorm(Record);

	  Y = (MIN(X, Y) == 0.0) ? MAX(X, Y) : 1.0;

	  if (BY < Y) {BY = Y; BID = ID; RuleIndex = i;}
 }
 return BID;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::ExecuteSNorm6(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex)
{
// Wyznaczenie rozk³adu s-norm dla rekordu w tabeli tymczasowej - metoda Einsteina
 int ID, BID = -1; double BY = -1.0;
 TAIELinguisticFuzzyRule* LFuzzyRule;
 //for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = &FFuzzyRule[i])->GetEnabled()) {
 for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = FFuzzyRule[i])->GetEnabled()) {
	  double& Y = SNormTable[ID = LFuzzyRule->GetOutFuzzySetIndex()];
	  double  X = LFuzzyRule->ExecuteTNorm(Record);

	  Y = (X + Y) / (1 + X * Y);

	  if (BY < Y) {BY = Y; BID = ID; RuleIndex = i;}
 }
 return BID;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::ExecuteDataSNorm(double* Record, double* SNormTable, int& RuleIndex)
{
 memset(SNormTable, 0, FOutFuzzySetCount * sizeof(double));
 return (this->*FExecuteDataSNorm)(Record, SNormTable, RuleIndex);
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::ExecuteDataSNorm1(double* Record, double* SNormTable, int& RuleIndex)
{
// Wyznaczenie rozk³adu s-norm dla rekordu w tabeli tymczasowej - metoda Zadeha
 int ID, BID = -1; double BY = -1.0;
 TAIELinguisticFuzzyRule* LFuzzyRule;
 //for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = &FFuzzyRule[i])->GetEnabled()) {
 for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = FFuzzyRule[i])->GetEnabled()) {
	  double& Y = SNormTable[ID = LFuzzyRule->GetOutFuzzySetIndex()];
	  double  X = LFuzzyRule->ExecuteDataTNorm(Record);

	  Y = MAX(X, Y);

	  if (BY < Y) {BY = Y; BID = ID; RuleIndex = i;}
 }
 return BID;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::ExecuteDataSNorm2(double* Record, double* SNormTable, int& RuleIndex)
{
// Wyznaczenie rozk³adu s-norm dla rekordu w tabeli tymczasowej - metoda probabilistyczna
 int ID, BID = -1; double BY = -1.0;
 TAIELinguisticFuzzyRule* LFuzzyRule;
 //for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = &FFuzzyRule[i])->GetEnabled()) {
 for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = FFuzzyRule[i])->GetEnabled()) {
	  double& Y = SNormTable[ID = LFuzzyRule->GetOutFuzzySetIndex()];
	  double  X = LFuzzyRule->ExecuteDataTNorm(Record);

	  Y = X + Y - X * Y;

	  if (BY < Y) {BY = Y; BID = ID; RuleIndex = i;}
 }
 return BID;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::ExecuteDataSNorm3(double* Record, double* SNormTable, int& RuleIndex)
{
// Wyznaczenie rozk³adu s-norm dla rekordu w tabeli tymczasowej - metoda £ukasiewicza
 int ID, BID = -1; double BY = -1.0;
 TAIELinguisticFuzzyRule* LFuzzyRule;
 //for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = &FFuzzyRule[i])->GetEnabled()) {
 for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = FFuzzyRule[i])->GetEnabled()) {
	  double& Y = SNormTable[ID = LFuzzyRule->GetOutFuzzySetIndex()];
	  double  X = LFuzzyRule->ExecuteDataTNorm(Record);

	  Y = MIN(X + Y, 1.0);

	  if (BY < Y) {BY = Y; BID = ID; RuleIndex = i;}
 }
 return BID;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::ExecuteDataSNorm4(double* Record, double* SNormTable, int& RuleIndex)
{
// Wyznaczenie rozk³adu s-norm dla rekordu w tabeli tymczasowej - metoda Fodora
 int ID, BID = -1; double BY = -1.0;
 TAIELinguisticFuzzyRule* LFuzzyRule;
 //for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = &FFuzzyRule[i])->GetEnabled()) {
 for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = FFuzzyRule[i])->GetEnabled()) {
	  double& Y = SNormTable[ID = LFuzzyRule->GetOutFuzzySetIndex()];
	  double  X = LFuzzyRule->ExecuteDataTNorm(Record);

	  Y = (X + Y < 1.0) ? MAX(X, Y) : 1.0;

	  if (BY < Y) {BY = Y; BID = ID; RuleIndex = i;}
 }
 return BID;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::ExecuteDataSNorm5(double* Record, double* SNormTable, int& RuleIndex)
{
// Wyznaczenie rozk³adu s-norm dla rekordu w tabeli tymczasowej - metoda drastyczna
 int ID, BID = -1; double BY = -1.0;
 TAIELinguisticFuzzyRule* LFuzzyRule;
 //for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = &FFuzzyRule[i])->GetEnabled()) {
 for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = FFuzzyRule[i])->GetEnabled()) {
	  double& Y = SNormTable[ID = LFuzzyRule->GetOutFuzzySetIndex()];
	  double  X = LFuzzyRule->ExecuteDataTNorm(Record);

	  Y = (MIN(X, Y) == 0.0) ? MAX(X, Y) : 1.0;

	  if (BY < Y) {BY = Y; BID = ID; RuleIndex = i;}
 }
 return BID;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyRulebase::ExecuteDataSNorm6(double* Record, double* SNormTable, int& RuleIndex)
{
// Wyznaczenie rozk³adu s-norm dla rekordu w tabeli tymczasowej - metoda Einsteina
 int ID, BID = -1; double BY = -1.0;
 TAIELinguisticFuzzyRule* LFuzzyRule;
 //for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = &FFuzzyRule[i])->GetEnabled()) {
 for (int i = 0; i < FFuzzyRuleCount; ++i) if ((LFuzzyRule = FFuzzyRule[i])->GetEnabled()) {
	  double& Y = SNormTable[ID = LFuzzyRule->GetOutFuzzySetIndex()];
	  double  X = LFuzzyRule->ExecuteDataTNorm(Record);

	  Y = (X + Y) / (1 + X * Y);

	  if (BY < Y) {BY = Y; BID = ID; RuleIndex = i;}
 }
 return BID;
}
//---------------------------------------------------------------------------
// Wyznaczenie implikacji dla zadanego wyjœcia Y (wartoœci wynikowej funkcji przynale¿noœci dla wyjœciowego atrybutu w punkcie Y)
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteImplication(double X, double Y)
{
 return (this->*FExecuteImplication)(X, Y);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteImplication1(double X, double Y)
{
 //implikacja Mamdaniego (minimum)
 return MIN(X, Y);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteImplication2(double X, double Y)
{
 //implikacja Larsena
 return X * Y;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteImplication3(double X, double Y)
{
 //implikacja £ukasiewicza
 return MIN(1.0 - X + Y, 1.0);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteImplication4(double X, double Y)
{
 //implikacja Fodora
 return (X > Y) ? MAX(1.0 - X, Y) : 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteImplication5(double X, double Y)
{
 //implikacja Reichenbacha
 return 1.0 - X + X * Y;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteImplication6(double X, double Y)
{
 //implikacja Kleene'a - Dienesa
 return MAX(1.0 - X, Y);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteImplication7(double X, double Y)
{
 //implikacja Zadeha
 return MAX(1.0 - X, MIN(X, Y));
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteImplication8(double X, double Y)
{
 //implikacja Goguena
 return (X != 0.0) ? MIN(Y / X, 1.0) : 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteImplication9(double X, double Y)
{
 //implikacja Godela
 return (X > Y) ? Y : 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteImplication10(double X, double Y)
{
 //implikacja Reschera
 return (X > Y) ? 0.0 : 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteImplication11(double X, double Y)
{
 //implikacja Yagera
 return ((X == 0.0) && (Y == 0.0)) ? 1.0 : pow(Y, X);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteImplication12(double X, double Y)
{
 //implikacja wymuszaj¹ca
 return X * (1.0 - ABS(X - Y));
}
//---------------------------------------------------------------------------
// Wyznaczenie wartoœci rozmytej dla zadanej ostrej wartoœci wyjœcia CY,
// (wartoœci wynikowej funkcji przynale¿noœci dla wyjœciowego atrybutu w punkcie CY,
//  po dokonaniu agregacji regu³ z ró¿nymi nastêpnikami)
// (gdy FuzzySetIndex >= 0 -> praca w trybie FITA, w przeciwnym przypadku w trybie FATI)
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteMshpDegree(double CY, double* SNormTable, int FuzzySetIndex)
{
 return (this->*FExecuteMshpDegree)(CY, SNormTable, FuzzySetIndex);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteMshpDegree0(double CY, double* SNormTable, int FuzzySetIndex)
{
 //przeliczenie dla jednego zbioru rozmytego, dla trybu FITA
 double X = SNormTable[FuzzySetIndex];
 double Y = FOutFuzzyCollection->GetFuzzySet(FuzzySetIndex)->ExecuteFuzzyValue(CY);
 return (this->*FExecuteImplication)(X, Y);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteMshpDegree1(double CY, double* SNormTable, int FuzzySetIndex)
{
 //operator agregacji Maksimum, dla trybu FATI
 double X, Y, FY = 0.0;
 for (int j = 0; j < FOutFuzzySetCount; ++j) {
	  X = SNormTable[j];
	  Y = FOutFuzzyCollection->GetFuzzySet(j)->ExecuteFuzzyValue(CY);
	  FY = MAX(FY, (this->*FExecuteImplication)(X, Y));
 }
 return FY;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteMshpDegree2(double CY, double* SNormTable, int FuzzySetIndex)
{
 //operator agregacji œrednia arytmetyczna, dla trybu FATI
 double X, Y, FY = 0.0;
 for (int j = 0; j < FOutFuzzySetCount; ++j) {
	  X = SNormTable[j];
	  Y = FOutFuzzyCollection->GetFuzzySet(j)->ExecuteFuzzyValue(CY);
	  FY += (this->*FExecuteImplication)(X, Y);
 }
 return FY / FOutFuzzySetCount;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteMshpDegree3(double CY, double* SNormTable, int FuzzySetIndex)
{
 //operator agregacji œrednia kwadratowa, dla trybu FATI
 double X, Y, FY = 0.0;
 for (int j = 0; j < FOutFuzzySetCount; ++j) {
	  X = SNormTable[j];
	  Y = FOutFuzzyCollection->GetFuzzySet(j)->ExecuteFuzzyValue(CY);
	  FY += SQR((this->*FExecuteImplication)(X, Y));
 }
 return sqrt(FY / FOutFuzzySetCount);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteMshpDegree4(double CY, double* SNormTable, int FuzzySetIndex)
{
 //operator agregacji œrednia geometryczna, dla trybu FATI
 double X, Y, FY = 1.0;
 for (int j = 0; j < FOutFuzzySetCount; ++j) {
	  X = SNormTable[j];
	  Y = FOutFuzzyCollection->GetFuzzySet(j)->ExecuteFuzzyValue(CY);
	  FY *= (this->*FExecuteImplication)(X, Y);
 }
 return pow(FY, 1.0 / FOutFuzzySetCount);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteMshpDegree5(double CY, double* SNormTable, int FuzzySetIndex)
{
 //operator agregacji minimum, dla trybu FATI
 double X, Y, FY = 1.0;
 for (int j = 0; j < FOutFuzzySetCount; ++j) {
	  X = SNormTable[j];
	  Y = FOutFuzzyCollection->GetFuzzySet(j)->ExecuteFuzzyValue(CY);
	  FY = MIN(FY, (this->*FExecuteImplication)(X, Y));
 }
 return FY;
}
//---------------------------------------------------------------------------
// Wyznaczenie wartoœci wyjœcia ci¹g³ego - defuzyfikacja
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteCrispValue(double* SNormTable, int FuzzySetIndex)
{
 return (this->*FExecuteCrispValue)(SNormTable, FuzzySetIndex);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteCrispValueCOG(double* SNormTable, int FuzzySetIndex)
{
// Wyznaczenie wartoœci wyjœcia ci¹g³ego - defuzyfikacja metod¹ œrodka ciê¿koœci
 double X1 = FOutMinValue; //wartoœæ wyjœciowa po lewej stronie (po stronie MinExtValue)
 double X2 = FOutMaxValue; //wartoœæ wyjœciowa po prawej stronie (po stronie MaxExtValue)
 const int Count = FIntegralProbeCount / 2;
 const double D = (X2 - X1) / FIntegralProbeCount;

 double A1 = 0, A2 = 0;
 double B1 = 0, B2 = 0;
 double Y1 = (this->*FExecuteMshpDegree)(X1, SNormTable, FuzzySetIndex);
 double Y2 = (this->*FExecuteMshpDegree)(X2, SNormTable, FuzzySetIndex);
 for (int i = 2; i < Count; ++i) {
	  Y1 = (this->*FExecuteMshpDegree)((X1+=D), SNormTable, FuzzySetIndex);
	  Y2 = (this->*FExecuteMshpDegree)((X2-=D), SNormTable, FuzzySetIndex);
	  A1 += X1 * Y1;
	  A2 += X2 * Y2;
	  B1 += Y1;
	  B2 += Y2;
 }
 return (B1 + B2 != 0.0) ? (A1 + A2) / (B1 + B2) : (X1 + X2) / 2.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteCrispValueCOA(double* SNormTable, int FuzzySetIndex)
{
// Wyznaczenie wartoœci wyjœcia ci¹g³ego - defuzyfikacja metod¹ œrodka powierzchni
 double X1 = FOutMinValue; //wartoœæ wyjœciowa po lewej stronie (po stronie MinExtValue)
 double X2 = FOutMaxValue; //wartoœæ wyjœciowa po prawej stronie (po stronie MaxExtValue)
 const double D = (X2 - X1) / FIntegralProbeCount;

 double B1 = (this->*FExecuteMshpDegree)(X1, SNormTable, FuzzySetIndex); //pole lewej (po stronie MinValue)
 double B2 = (this->*FExecuteMshpDegree)(X2, SNormTable, FuzzySetIndex); //pole prawej (po stronie MaxValue)
 for (int i = 0; i < FIntegralProbeCount; ++i) {
	  if (B1 < B2) {
		  if ((X1 += D) > X2) break;
		  B1 += (this->*FExecuteMshpDegree)(X1, SNormTable, FuzzySetIndex);
	  } else
	  if (B1 > B2) {
		  if (X1 > (X2 -= D)) break;
		  B2 += (this->*FExecuteMshpDegree)(X2, SNormTable, FuzzySetIndex);
	  } else {
		  if ((X1 += D) > (X2 -= D)) break;
		  B1 += (this->*FExecuteMshpDegree)(X1, SNormTable, FuzzySetIndex);
		  B2 += (this->*FExecuteMshpDegree)(X2, SNormTable, FuzzySetIndex);
	  }
 }
 return (X1 + X2) / 2.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteCrispValueMCOA(double* SNormTable, int FuzzySetIndex)
{
// Wyznaczenie wartoœci wyjœcia ci¹g³ego - defuzyfikacja zmodyfikowan¹ metod¹ œrodka powierzchni
 double X1 = FOutMinValue; //wartoœæ wyjœciowa po lewej stronie (po stronie MinExtValue)
 double X2 = FOutMaxValue; //wartoœæ wyjœciowa po prawej stronie (po stronie MaxExtValue)
 if (FOutFuzzySetCount > 2) {
	 X1 -= FOutFuzzyCollection->GetFuzzySet(0)->GetArea();
	 X2 += FOutFuzzyCollection->GetFuzzySet(FOutFuzzySetCount - 1)->GetArea();
 }

 const double D = (X2 - X1) / FIntegralProbeCount;
 double B1 = (this->*FExecuteMshpDegree)(X1, SNormTable, FuzzySetIndex); //pole lewej (po stronie MinValue)
 double B2 = (this->*FExecuteMshpDegree)(X2, SNormTable, FuzzySetIndex); //pole prawej (po stronie MaxValue)
 for (int i = 0; i < FIntegralProbeCount; ++i) {
	  if (B1 < B2) {
		  if ((X1 += D) > X2) break;
		  B1 += (this->*FExecuteMshpDegree)(X1, SNormTable, FuzzySetIndex);
	  } else
	  if (B1 > B2) {
		  if (X1 > (X2 -= D)) break;
		  B2 += (this->*FExecuteMshpDegree)(X2, SNormTable, FuzzySetIndex);
	  } else {
		  if ((X1 += D) > (X2 -= D)) break;
		  B1 += (this->*FExecuteMshpDegree)(X1, SNormTable, FuzzySetIndex);
		  B2 += (this->*FExecuteMshpDegree)(X2, SNormTable, FuzzySetIndex);
	  }
 }
 return MIN(MAX((X1 + X2) / 2.0, FOutMinValue), FOutMaxValue);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteCrispValueFOM(double* SNormTable, int FuzzySetIndex)
{
// Wyznaczenie wartoœci wyjœcia ci¹g³ego - defuzyfikacja metod¹ pierwszego maksimum
 double X1 = FOutMinValue; //wartoœæ wyjœciowa po lewej stronie (po stronie MinExtValue)
 double X2 = FOutMaxValue; //wartoœæ wyjœciowa po prawej stronie (po stronie MaxExtValue)
 const double D = (X2 - X1) / FIntegralProbeCount;

 double Y1 = (this->*FExecuteMshpDegree)(X1, SNormTable, FuzzySetIndex);
 double MX1 = X1, M1 = Y1;
 for (int i = 0; i < FIntegralProbeCount; ++i) {
	  Y1 = (this->*FExecuteMshpDegree)((X1 += D), SNormTable, FuzzySetIndex);
	  if (M1 < Y1) {M1 = Y1; MX1 = X1;}
 }
 return MX1;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteCrispValueMOM(double* SNormTable, int FuzzySetIndex)
{
// Wyznaczenie wartoœci wyjœcia ci¹g³ego - defuzyfikacja metod¹ œrodka maksimum
 double X1 = FOutMinValue; //wartoœæ wyjœciowa po lewej stronie (po stronie MinExtValue)
 double X2 = FOutMaxValue; //wartoœæ wyjœciowa po prawej stronie (po stronie MaxExtValue)
 const double D = (X2 - X1) / FIntegralProbeCount;

 double Y1 = (this->*FExecuteMshpDegree)(X1, SNormTable, FuzzySetIndex);
 double Y2 = (this->*FExecuteMshpDegree)(X2, SNormTable, FuzzySetIndex);
 double MX1 = X1, M1 = Y1;
 double MX2 = X2, M2 = Y2;
 for (int i = 0; i < FIntegralProbeCount; ++i) {
	  Y1 = (this->*FExecuteMshpDegree)((X1 += D), SNormTable, FuzzySetIndex);
	  Y2 = (this->*FExecuteMshpDegree)((X2 -= D), SNormTable, FuzzySetIndex);
	  if (M1 < Y1) {M1 = Y1; MX1 = X1;}
	  if (M2 < Y2) {M2 = Y2; MX2 = X2;}
	  if (MX1 > MX2) break;
 }
 return (MX1 + MX2) / 2.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteCrispValueLOM(double* SNormTable, int FuzzySetIndex)
{
// Wyznaczenie wartoœci wyjœcia ci¹g³ego - defuzyfikacja metod¹ ostatniego maksimum
 double X1 = FOutMinValue; //wartoœæ wyjœciowa po lewej stronie (po stronie MinExtValue)
 double X2 = FOutMaxValue; //wartoœæ wyjœciowa po prawej stronie (po stronie MaxExtValue)
 const double D = (X2 - X1) / FIntegralProbeCount;

 double Y2 = (this->*FExecuteMshpDegree)(X2, SNormTable, FuzzySetIndex);
 double MX2 = X2, M2 = Y2;
 for (int i = 0; i < FIntegralProbeCount; ++i) {
	  Y2 = (this->*FExecuteMshpDegree)((X2 -= D), SNormTable, FuzzySetIndex);
	  if (M2 < Y2) {M2 = Y2; MX2 = X2;}
 }
 return MX2;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteCrispValueRAW(double* SNormTable, int FuzzySetIndex)
{
// Wyznaczenie wartoœci wyjœcia ci¹g³ego - defuzyfikacja metod¹ optymalizacji b³êdu s-norm
 double X1 = FOutMinValue; //wartoœæ wyjœciowa po lewej stronie (po stronie MinExtValue)
 double X2 = FOutMaxValue; //wartoœæ wyjœciowa po prawej stronie (po stronie MaxExtValue)
 const int Count = FIntegralProbeCount / 2;
 const double D = (X2 - X1) / FIntegralProbeCount;

 double ME1 = 1.0;
 double ME2 = 1.0;
 double MX1 = X1;
 double MX2 = X2;
 double E1, E2, E, X;
 if (FuzzySetIndex < 0) {
	 for (int i = 0; i < Count; ++i) {
		  E1 = E2 = 0.0;
		  for (int k = 0; k < FOutFuzzySetCount; ++k) {
			   E1 += ABS(SNormTable[k] - FOutFuzzyCollection->GetFuzzySet(k)->ExecuteFuzzyValue(X1));
			   E2 += ABS(SNormTable[k] - FOutFuzzyCollection->GetFuzzySet(k)->ExecuteFuzzyValue(X2));
		  }
		  if (ME1 > E1) {ME1 = E1; MX1 = X1;}
		  if (ME2 > E2) {ME2 = E2; MX2 = X2;}
		  X1 += D;
		  X2 -= D;
	 }

	 if (ME1 == ME2) {
		  X = (MX1 + MX2) / 2.0;
		  E = 0.0;
		  for (int k = 0; k < FOutFuzzySetCount; ++k)
			   E += ABS(SNormTable[k] - FOutFuzzyCollection->GetFuzzySet(k)->ExecuteFuzzyValue(X));
		  if (ME1 == E) MX1 = MX2 = X;
	 }
 } else {
	 for (int i = 0; i < Count; ++i) {
		  E1 = ABS(SNormTable[FuzzySetIndex] - FOutFuzzyCollection->GetFuzzySet(FuzzySetIndex)->ExecuteFuzzyValue(X1));
		  E2 = ABS(SNormTable[FuzzySetIndex] - FOutFuzzyCollection->GetFuzzySet(FuzzySetIndex)->ExecuteFuzzyValue(X2));
		  if (ME1 > E1) {ME1 = E1; MX1 = X1;}
		  if (ME2 > E2) {ME2 = E2; MX2 = X2;}
		  X1 += D;
		  X2 -= D;
	 }

	 if (ME1 == ME2) {
		 X = (MX1 + MX2) / 2.0;
		 E = ABS(SNormTable[FuzzySetIndex] - FOutFuzzyCollection->GetFuzzySet(FuzzySetIndex)->ExecuteFuzzyValue(X));
		 if (ME1 == E) MX1 = MX2 = X;
	 }
 }

 return (ME1 < ME2) ? MX1 : MX2;
}
//---------------------------------------------------------------------------
// Wyznaczenie wartoœci wyjœcia ci¹g³ego
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteOutput(double* SNormTable)
{
 return (this->*FExecuteOutput)(SNormTable);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteOutputFATI(double* SNormTable)
{
 return (this->*FExecuteCrispValue)(SNormTable, -1);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteOutputFITA1(double* SNormTable)
{
 //operator agregacji Maksimum, dla trybu FITA
 double MCY = 0.0;
 double MY = 0.0;
 for (int i = 0; i < FOutFuzzySetCount; ++i) {
	  double CY = (this->*FExecuteCrispValue)(SNormTable, i);
	  double Y = (this->*FExecuteMshpDegree)(CY, SNormTable, i);

	  if (MY < Y) {MY = Y; MCY = CY;}
 }
 return MCY;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteOutputFITA2(double* SNormTable)
{
 //operator agregacji wa¿ona œrednia arytmetyczna, dla trybu FITA
 double MCY = 0.0;
 double MCM = 0.0;
 for (int i = 0; i < FOutFuzzySetCount; ++i) {
	  double CY = (this->*FExecuteCrispValue)(SNormTable, i);
	  double Y = (this->*FExecuteMshpDegree)(CY, SNormTable, i);

	  MCY += CY * Y;
	  MCM += Y;
 }
 return (MCM > 0) ? MCY / MCM : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteOutputFITA3(double* SNormTable)
{
 //operator agregacji wa¿ona œrednia kwadratowa, dla trybu FITA
 double MCY = 0.0;
 double MCM = 0.0;
 for (int i = 0; i < FOutFuzzySetCount; ++i) {
	  double CY = (this->*FExecuteCrispValue)(SNormTable, i);
	  double Y = (this->*FExecuteMshpDegree)(CY, SNormTable, i);

	  MCY += SQR(CY) * Y;
	  MCM += Y;
 }
 return (MCM > 0) ? sqrt(MCY / MCM) : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteOutputFITA4(double* SNormTable)
{
 //operator agregacji wa¿ona œrednia geometryczna, dla trybu FITA
 double MCY = 1.0;
 double MCM = 1.0;
 for (int i = 0; i < FOutFuzzySetCount; ++i) {
	  double CY = (this->*FExecuteCrispValue)(SNormTable, i);
	  double Y = (this->*FExecuteMshpDegree)(CY, SNormTable, i);

	  MCY += log(CY) * Y;     					//MCY *= pow(CY, Y)
	  MCM += Y;
 }
 return (MCM > 0) ? exp(MCY / MCM) : 0.0;       //pow(MCY, 1.0 / MCM);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::ExecuteOutputFITA5(double* SNormTable)
{
 //operator agregacji wa¿one minimum, dla trybu FITA
 double MCY = 1.0;
 double MY = 1.0;
 for (int i = 0; i < FOutFuzzySetCount; ++i) {
	  double CY = (this->*FExecuteCrispValue)(SNormTable, i);
	  double Y = (this->*FExecuteMshpDegree)(CY, SNormTable, i);

	  if (MY > Y) {MY = Y; MCY = CY;}
 }
 return MCY;
}
//---------------------------------------------------------------------------
// Wyznaczanie z³o¿onoœci systemu
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::GetFuzzyRulebaseComplexity(void)
{
 return (/*GetNormEnabledFuzzyRuleCount(FMinFuzzyRuleCount, FMaxFuzzyRuleCount) +*/
		 GetNormEnabledFuzzyRuleComplexity() +
		 GetNormEnabledFuzzyAttrCount() +
		 GetNormEnabledFuzzySetCount()) / 3.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::GetNormEnabledFuzzyRuleComplexity(void)
{
 // Œrednia z³ozonoœæ regu³ w bazie
 if (!FEnabledFuzzyRuleCount) return 1.0;

 double LNormEnabledFuzzyRuleComplexity = 0.0;
 for (int i = 0; i < FFuzzyRuleCount; ++i)
	  //if (FFuzzyRule[i].GetEnabled())
	  if (FFuzzyRule[i]->GetEnabled())
		  //LNormEnabledFuzzyRuleComplexity += FFuzzyRule[i].GetFuzzyRuleComplexity();
		  LNormEnabledFuzzyRuleComplexity += FFuzzyRule[i]->GetFuzzyRuleComplexity();

 return LNormEnabledFuzzyRuleComplexity / FEnabledFuzzyRuleCount;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::GetNormEnabledFuzzyRuleCount(double MinFuzzyRuleCount, double MaxFuzzyRuleCount)
{
 // Normalizowana liczba regu³, liczona wzglêdem maksymalnej i minimalnej liczby regu³ wynikaj¹cej z iloœci przes³anek w regu³ach
 if (!FEnabledFuzzyRuleCount) return 1.0;

 double LNormEnabledFuzzyRuleCount = (FEnabledFuzzyRuleCount - MinFuzzyRuleCount) / (double)(MaxFuzzyRuleCount - MinFuzzyRuleCount);
 if (LNormEnabledFuzzyRuleCount < 0.0) LNormEnabledFuzzyRuleCount = 1.0;

 return LNormEnabledFuzzyRuleCount;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::GetNormEnabledFuzzyAttrCount(void)
{
 // Normalizowana liczba w³¹czonych wejœæ
 if (FInpFuzzyAttrCount == 1) return 0.0;

 double LNormEnabledFuzzyAttrCount = (FInpFuzzyAttrCount > 1) ? ((double)(GetEnabledInpFuzzyAttrCount() - 1) / (double)(FInpFuzzyAttrCount - 1)) : 0.0;
 if (LNormEnabledFuzzyAttrCount < 0.0) LNormEnabledFuzzyAttrCount = 1.0;

 return LNormEnabledFuzzyAttrCount;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyRulebase::GetNormEnabledFuzzySetCount(void)
{
 // Normalizowana liczba wykorzystanych zbiorów rozmytych
 double LFuzzySetCount = 0.0;
 double LNormEnabledFuzzySetCount = 0.0;

 //dla klasyfikacji ECG
// if (FOwner->GetEnabledFuzzyCollectionCount()) {
//	  IAIELinguisticFuzzyCollection* LFuzzyCollection = FOwner->GetEnabledFuzzyCollection(0);

 for (int i = 0; i < FOwner->GetEnabledFuzzyCollectionCount(); ++i) {
	  IAIELinguisticFuzzyCollection* LFuzzyCollection = FOwner->GetEnabledFuzzyCollection(i);
	  if (LFuzzyCollection->GetIsNominalType() && (i >= FInpFuzzyAttrCount)) continue;

	  LFuzzySetCount += LFuzzyCollection->GetFuzzySetCount();
	  for (int j = 0; j < LFuzzyCollection->GetFuzzySetCount(); ++j)
		   if (LFuzzyCollection->GetFuzzySet(j)->GetAssignedFuzzyAntecedentCount(FOutFuzzyAttrIndex) != 0) ++LNormEnabledFuzzySetCount;
 }

 LNormEnabledFuzzySetCount = (LFuzzySetCount > 1.0) ? (LNormEnabledFuzzySetCount - 1.0) / (LFuzzySetCount - 1.0) : 0.0;
 if (LNormEnabledFuzzySetCount < 0.0) LNormEnabledFuzzySetCount = 1.0;

 return LNormEnabledFuzzySetCount;
}
//---------------------------------------------------------------------------
