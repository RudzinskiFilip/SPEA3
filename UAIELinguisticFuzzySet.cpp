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
#include "UAIELinguisticFuzzySet.h"
#include "UAIELinguisticFuzzyRule.h"
#include "UAIELinguisticFuzzyStructure.h"
//---------------------------------------------------------------------------
//BEGIN_NAMESPACE(NAIELinguisticFuzzyStructure)
//---------------------------------------------------------------------------
//END_NAMESPACE(NAIELinguisticFuzzyStructure)
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyOptions
//---------------------------------------------------------------------------
//using namespace NAIELinguisticFuzzyStructure;
//---------------------------------------------------------------------------
// TAIELinguisticFuzzySet
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzySet::TAIELinguisticFuzzySet(TAIELinguisticFuzzyCollection* AOwner,
	int ACount, int AIndex, int ANumaNodeIndex, int AFuzzyRulebaseCount,
	TAIELinguisticFuzzySetKind AKind, double AMaxValue, double AMinValue,
	TAIEMembershipFunction AMembershipFunction, bool ACoreEnabled, bool AContrastEnabled)
		: FFuzzySet(AOwner->FFuzzySet)
{
 FOwner = AOwner;
 FIndex = AIndex;

 FNumaNodeIndex = ANumaNodeIndex;
 FAssignedFuzzyAntecedentCount.SetNumaNodeIndex(ANumaNodeIndex);
 FAssignedFuzzyAntecedentCount.Resize(AFuzzyRulebaseCount);

 FMaxValue = AMaxValue;
 FMinValue = AMinValue;
 FKind = AKind;
 FName = strdup("");
 FLCenter = 0.0;
 FRCenter = 0.0;
 FLAlpha = 0.0;
 FRAlpha = 0.0;
 FLContrast = 1.0;
 FRContrast = 1.0;
 FCoreEnabled = ACoreEnabled;
 FContrastEnabled = AContrastEnabled;

 F1L = - M_PI;
 F1R = - M_PI;
 F3L = 1.0;  //d³ugoœæ boku od lewego centrum do lewego punktu, w którym funkcja przyjmuje wartoœæ 0.0 (0.01 dla funkcji gaussowskiej)
 F3R = 1.0;  //d³ugoœæ boku od prawego centrum do prawego punktu, w którym funkcja przyjmuje wartoœæ 0.0 (0.01 dla funkcji gaussowskiej)
 F5L = M_PI_2;
 F5R = M_PI_2;
 FCL = 1.0;
 FCR = 1.0;

 SetMembershipFunction(AMembershipFunction);
}
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzySet::~TAIELinguisticFuzzySet(void)
{
 FAssignedFuzzyAntecedentCount.Resize(0);
 free(FName);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::Assign(TAIELinguisticFuzzySet* Source)
{
 SetName(Source->FName);
 FMaxValue = Source->FMaxValue;
 FMinValue = Source->FMinValue;
 FKind = Source->FKind;
 FLCenter = Source->FLCenter;
 FRCenter = Source->FRCenter;
 FLAlpha = Source->FLAlpha;
 FRAlpha = Source->FRAlpha;
 FLContrast = Source->FLContrast;
 FRContrast = Source->FRContrast;
 FCoreEnabled = Source->FCoreEnabled;
 FContrastEnabled = Source->FContrastEnabled;

 F1L = Source->F1L;
 F1R = Source->F1R;
 F3L = Source->F3L;
 F3R = Source->F3R;
 F5L = Source->F5L;
 F5R = Source->F5R;
 FCL = Source->FCL;
 FCR = Source->FCR;

 SetMembershipFunction(Source->FMembershipFunction);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::SetKind(TAIELinguisticFuzzySetKind AKind)
{
 FKind = AKind;
 SetMembershipFunction(FMembershipFunction);
}
//---------------------------------------------------------------------------
const char* __stdcall TAIELinguisticFuzzySet::GetName(void)
{
 return FName;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::SetName(const char* AName)
{
 free(FName);
 FName = strdup(AName);
}
//---------------------------------------------------------------------------
TAIEMembershipFunction __stdcall TAIELinguisticFuzzySet::GetMembershipFunction(void)
{
 return FMembershipFunction;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::SetMembershipFunction(TAIEMembershipFunction AMembershipFunction)
{
 FMembershipFunction = AMembershipFunction;

 switch (FMembershipFunction) {
	  case fuzzyMembershipFunction0: FFunction = &TAIELinguisticFuzzySet::SingletonFunction;
									 break;
	  case fuzzyMembershipFunction1: if (!FCoreEnabled) {
										if (FKind == fskNormal) FFunction = &TAIELinguisticFuzzySet::NMembershipFunction1; else
										if (FKind == fskRight)  FFunction = &TAIELinguisticFuzzySet::RMembershipFunction1; else
																FFunction = &TAIELinguisticFuzzySet::LMembershipFunction1;
									 } else {
										if (FKind == fskNormal) FFunction = &TAIELinguisticFuzzySet::NMembershipFunction2; else
										if (FKind == fskRight)  FFunction = &TAIELinguisticFuzzySet::RMembershipFunction2; else
																FFunction = &TAIELinguisticFuzzySet::LMembershipFunction2;
									 }
									 break;
	  case fuzzyMembershipFunction2: if (!FCoreEnabled) {
										if (FKind == fskNormal) FFunction = &TAIELinguisticFuzzySet::NMembershipFunction3; else
										if (FKind == fskRight)  FFunction = &TAIELinguisticFuzzySet::RMembershipFunction3; else
																FFunction = &TAIELinguisticFuzzySet::LMembershipFunction3;
									 } else {
										if (FKind == fskNormal) FFunction = &TAIELinguisticFuzzySet::NMembershipFunction4; else
										if (FKind == fskRight)  FFunction = &TAIELinguisticFuzzySet::RMembershipFunction4; else
																FFunction = &TAIELinguisticFuzzySet::LMembershipFunction4;
									 }
									 break;
	  case fuzzyMembershipFunction3: if (!FCoreEnabled) {
										if (FKind == fskNormal) FFunction = &TAIELinguisticFuzzySet::NMembershipFunction5; else
										if (FKind == fskRight)  FFunction = &TAIELinguisticFuzzySet::RMembershipFunction5; else
																FFunction = &TAIELinguisticFuzzySet::LMembershipFunction5;
									 } else {
										if (FKind == fskNormal) FFunction = &TAIELinguisticFuzzySet::NMembershipFunction6; else
										if (FKind == fskRight)  FFunction = &TAIELinguisticFuzzySet::RMembershipFunction6; else
																FFunction = &TAIELinguisticFuzzySet::LMembershipFunction6;
									 }
									 break;
	  case fuzzyMembershipFunction4: if (!FCoreEnabled) {
										if (FKind == fskNormal) FFunction = &TAIELinguisticFuzzySet::NMembershipFunction7; else
										if (FKind == fskRight)  FFunction = &TAIELinguisticFuzzySet::RMembershipFunction7; else
																FFunction = &TAIELinguisticFuzzySet::LMembershipFunction7;
									 } else {
										if (FKind == fskNormal) FFunction = &TAIELinguisticFuzzySet::NMembershipFunction8; else
										if (FKind == fskRight)  FFunction = &TAIELinguisticFuzzySet::RMembershipFunction8; else
																FFunction = &TAIELinguisticFuzzySet::LMembershipFunction8;
									 }
									 break;
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::Prepare(void)
{
 if (FOwner->FSFP) {
	 if (FIndex > 0) {
		 F3L = (FCoreEnabled ? FLCenter : FRCenter) - (FFuzzySet[FIndex-1].FCoreEnabled ? FFuzzySet[FIndex-1].FRCenter : (FIndex == 1) ? FMinValue : FFuzzySet[FIndex-1].FRCenter);
		 FFuzzySet[FIndex-1].F3R = F3L;
		 if (F3L != 0.0) {
			 FFuzzySet[FIndex-1].F1R = F1L = -M_PI / SQR(F3L);
			 FFuzzySet[FIndex-1].F5R = F5L = M_PI / F3L;
		 } else {
			 FFuzzySet[FIndex-1].F1R = F1L = 0.0;
			 FFuzzySet[FIndex-1].F5R = F5L = 0.0;
		 }
	 }
	 if (FIndex < FFuzzySet.GetCount()-1) {
		 F3R = (FFuzzySet[FIndex+1].FCoreEnabled ? FFuzzySet[FIndex+1].FLCenter : ((FIndex == FFuzzySet.GetCount()-1) ? FMaxValue : FFuzzySet[FIndex+1].FRCenter)) - (FCoreEnabled ? FRCenter : ((FIndex == 0) ? FMinValue : FRCenter));
		 FFuzzySet[FIndex+1].F3L = F3R;
		 if (F3R != 0.0) {
			 FFuzzySet[FIndex+1].F1L = F1R = -M_PI / SQR(F3R);
			 FFuzzySet[FIndex+1].F5L = F5R = M_PI / F3R;
		 } else {
			 FFuzzySet[FIndex+1].F1L = F1R = 0.0;
			 FFuzzySet[FIndex+1].F5L = F5R = 0.0;
		 }
	 }
 } else {
	 F3L = FLAlpha;
	 F3R = FRAlpha;
	 if (F3L != 0.0) {F1L = -M_PI / SQR(F3L); F5L = M_PI / F3L;} else F5L = F1L = 0.0;
	 if (F3R != 0.0) {F1R = -M_PI / SQR(F3R); F5R = M_PI / F3R;} else F5R = F1R = 0.0;
 }
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzySet::GetCoreEnabled(void)
{
 return FCoreEnabled;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::SetCoreEnabled(bool ACoreEnabled)
{
 if (FCoreEnabled == ACoreEnabled) return;
 FCoreEnabled = ACoreEnabled;
 SetMembershipFunction(FMembershipFunction);
 Prepare();
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzySet::GetContrastEnabled(void)
{
 return FContrastEnabled;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::SetContrastEnabled(bool AContrastEnabled)
{
 if (FContrastEnabled == AContrastEnabled) return;
 FContrastEnabled = AContrastEnabled;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::GetLCenter(void)
{
 return FLCenter;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::SetLCenter(double AValue)
{
 if (FLCenter == AValue) return;
 if (AValue < FMinValue) AValue = FMinValue;
 if (AValue > FMaxValue) AValue = FMaxValue;
 FLCenter = AValue;
 if (AValue > FRCenter) SetRCenter(AValue);
				   else	if (FIndex > 0) if (FFuzzySet[FIndex-1].FRCenter > AValue) FFuzzySet[FIndex-1].SetRCenter(AValue);
 Prepare();
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::GetRCenter(void)
{
 return FRCenter;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::SetRCenter(double AValue)
{
 if (FRCenter == AValue) return;
 if (AValue < FMinValue) AValue = FMinValue;
 if (AValue > FMaxValue) AValue = FMaxValue;
 FRCenter = AValue;
 if (AValue < FLCenter) SetLCenter(AValue);
				   else	if (FIndex < FFuzzySet.GetCount()-1) if (FFuzzySet[FIndex+1].FLCenter < AValue) FFuzzySet[FIndex+1].SetLCenter(AValue);
 Prepare();
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::GetLContrast(void)
{
 return FLContrast;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::SetLContrast(double AValue)
{
 if (FLContrast == AValue) return;
 if (AValue < 0.01) AValue = 0.01;
 FLContrast = AValue;
 FCL = pow(2.0, FLContrast - 1.0);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::GetRContrast(void)
{
 return FRContrast;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::SetRContrast(double AValue)
{
 if (FRContrast == AValue) return;
 if (AValue < 0.01) AValue = 0.01;
 FRContrast = AValue;
 FCR = pow(2.0, FRContrast - 1.0);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::GetLAlpha(void)
{
 return F3L;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::SetLAlpha(double AValue)
{
 if (FLAlpha == AValue) return;
 if (AValue < 0.0) AValue = 0.0;
 FLAlpha = AValue;
 if (!FOwner->FSFP) Prepare();
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::GetRAlpha(void)
{
 return F3R;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::SetRAlpha(double AValue)
{
 if (FRAlpha == AValue) return;
 if (AValue < 0.0) AValue = 0.0;
 FRAlpha = AValue;
 if (!FOwner->FSFP) Prepare();
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::GetArea(void)
{
 // Wyznaczenie pola powierzchni zbioru
 switch (FKind) {
	case fskRight: return 0.5 * F3L + GetCore();
	case fskLeft:  return 0.5 * F3R + GetCore();
 }
 return 0.5 * (F3L + F3R) + GetCore();
 //return 0.5 * (F3L + F3R) + 2.0 * GetCore();
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::GetLArea(void)
{
 // Wyznaczenie pola powierzchni lewej strony zbioru
 switch (FKind) {
	case fskRight: return 0.5 * (F3L + GetCore());
	case fskLeft:  return 0.5 * GetCore();
 }
 return 0.5 * (F3L + GetCore());
 //return 0.5 * F3L + GetCore();
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::GetRArea(void)
{
 // Wyznaczenie pola powierzchni prawej strony zbioru
 switch (FKind) {
	case fskRight: return 0.5 * GetCore();
	case fskLeft:  return 0.5 * (F3R + GetCore());
 }
 return 0.5 * (F3R + GetCore());
 //return 0.5 * F3R + GetCore();
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::GetCenter(void)
{
 switch (FKind) {
	case fskRight: return FMaxValue;
	case fskLeft:  return FMinValue;
 }
 return FCoreEnabled ? (FLCenter + FRCenter) / 2.0 : FRCenter;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::GetCore(void)
{
 //po³owa rdzenia
 switch (FKind) {
	case fskRight: return FCoreEnabled ? (FMaxValue - FLCenter) : 0.0;
	case fskLeft:  return FCoreEnabled ? (FRCenter - FMinValue) : 0.0;
 }
 return FCoreEnabled ? (FRCenter - FLCenter) : 0.0;
 //return FCoreEnabled ? (FLCenter - FRCenter) / 2.0 : 0.0;
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzySet::GetIsActive(void)
{
 int LCount = FAssignedFuzzyAntecedentCount.GetCount();
 while (LCount--) if (FAssignedFuzzyAntecedentCount[LCount] != 0) return true;
 return false;
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzySet::GetAssignedFuzzyAntecedentCount(int AFuzzyRulebaseIndex)
{
 return FAssignedFuzzyAntecedentCount[AFuzzyRulebaseIndex];
}
//---------------------------------------------------------------------------
// Funkcja singletonowa (dla atrybutów nominalnych)
double __stdcall TAIELinguisticFuzzySet::SingletonFunction(double Value)
{
 return Value == FRCenter ? 1.0 : 0.0;
}
//---------------------------------------------------------------------------
// Funkcja gaussowska (L, R - skrajne, N - œrodkowa)
double __stdcall TAIELinguisticFuzzySet::LMembershipFunction1(double Value)
{
 return F1R != 0.0 ? EXP(F1R * SQR(Value - FMinValue)) : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::NMembershipFunction1(double Value)
{
 if (Value > FRCenter) return F1R != 0.0 ? EXP(F1R * SQR(Value - FRCenter)) : 0.0;
 if (Value < FRCenter) return F1L != 0.0 ? EXP(F1L * SQR(Value - FRCenter)) : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::RMembershipFunction1(double Value)
{
 return F1L != 0.0 ? EXP(F1L * SQR(FMaxValue - Value)) : 0.0;
}
//---------------------------------------------------------------------------
// Funkcja gausowska z rdzeniem (L, R - skrajne, N - œrodkowa)
double __stdcall TAIELinguisticFuzzySet::LMembershipFunction2(double Value)
{
 if (Value > FRCenter) return F1R != 0.0 ? EXP(F1R * SQR(Value - FRCenter)) : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::NMembershipFunction2(double Value)
{
 if (Value > FRCenter) return F1R != 0.0 ? EXP(F1R * SQR(Value - FRCenter)) : 0.0;
 if (Value < FLCenter) return F1L != 0.0 ? EXP(F1L * SQR(Value - FLCenter)) : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::RMembershipFunction2(double Value)
{
 if (Value < FLCenter) return F1L != 0.0 ? EXP(F1L * SQR(Value - FLCenter)) : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
// Funkcja trójk¹tna (L, R - skrajne, N - œrodkowa)
double __stdcall TAIELinguisticFuzzySet::LMembershipFunction3(double Value)
{
 return (Value < FMinValue + F3R) ? 1.0 - (Value - FMinValue) / F3R : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::NMembershipFunction3(double Value)
{
 if (Value > FRCenter) return (Value < FRCenter + F3R) ? 1.0 - (Value - FRCenter) / F3R : 0.0;
 if (Value < FRCenter) return (Value > FRCenter - F3L) ? 1.0 - (FRCenter - Value) / F3L : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::RMembershipFunction3(double Value)
{
 return (Value > FMaxValue - F3L) ? 1.0 - (FMaxValue - Value) / F3L : 0.0;
}
//---------------------------------------------------------------------------
// Funkcja trójk¹tna z rdzeniem (trapezoidalna) (L, R - skrajne, N - œrodkowa)
double __stdcall TAIELinguisticFuzzySet::LMembershipFunction4(double Value)
{
 if (Value > FRCenter) return (Value < FRCenter + F3R) ? 1.0 - (Value - FRCenter) / F3R : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::NMembershipFunction4(double Value)
{
 if (Value > FRCenter) return (Value < FRCenter + F3R) ? 1.0 - (Value - FRCenter) / F3R : 0.0;
 if (Value < FLCenter) return (Value > FLCenter - F3L) ? 1.0 - (FLCenter - Value) / F3L : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::RMembershipFunction4(double Value)
{
 if (Value < FLCenter) return (Value > FLCenter - F3L) ? 1.0 - (FLCenter - Value) / F3L : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
// Funkcja cosinusoidalna (L, R - skrajne, N - œrodkowa)
double __stdcall TAIELinguisticFuzzySet::LMembershipFunction5(double Value)
{
 return (Value < FMinValue + F3R) ? 0.5 * cos(F5R * (Value - FMinValue)) + 0.5 : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::NMembershipFunction5(double Value)
{
 if (Value > FRCenter) return (Value < FRCenter + F3R) ? 0.5 * cos(F5R * (Value - FRCenter)) + 0.5 : 0.0;
 if (Value < FRCenter) return (Value > FRCenter - F3L) ? 0.5 * cos(F5L * (FRCenter - Value)) + 0.5 : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::RMembershipFunction5(double Value)
{
 return (Value > FMaxValue - F3L) ? 0.5 * cos(F5L * (FMaxValue - Value)) + 0.5 : 0.0;
}
//---------------------------------------------------------------------------
// Funkcja cosinusoidalna z rdzeniem (L, R - skrajne, N - œrodkowa)
double __stdcall TAIELinguisticFuzzySet::LMembershipFunction6(double Value)
{
 if (Value > FRCenter) return (Value < FRCenter + F3R) ? 0.5 * cos(F5R * (Value - FRCenter)) + 0.5 : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::NMembershipFunction6(double Value)
{
 if (Value > FRCenter) return (Value < FRCenter + F3R) ? 0.5 * cos(F5R * (Value - FRCenter)) + 0.5 : 0.0;
 if (Value < FLCenter) return (Value > FLCenter - F3L) ? 0.5 * cos(F5L * (FLCenter - Value)) + 0.5 : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::RMembershipFunction6(double Value)
{
 if (Value < FLCenter) return (Value > FLCenter - F3L) ? 0.5 * cos(F5L * (FLCenter - Value)) + 0.5 : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
// Funkcja prostok¹tna (L, R - skrajne, N - œrodkowa)
double __stdcall TAIELinguisticFuzzySet::LMembershipFunction7(double Value)
{
 return (Value < FMinValue + 0.5*F3R) ? 1.0 : 0.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::NMembershipFunction7(double Value)
{
 if (Value > FRCenter) return (Value < FRCenter + 0.5*F3R) ? 1.0 : 0.0;
 if (Value < FRCenter) return (Value > FRCenter - 0.5*F3L) ? 1.0 : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::RMembershipFunction7(double Value)
{
 return (Value > FMaxValue - 0.5*F3L) ? 1.0 : 0.0;
}
//---------------------------------------------------------------------------
// Funkcja prostok¹tna z rdzeniem (L, R - skrajne, N - œrodkowa)
double __stdcall TAIELinguisticFuzzySet::LMembershipFunction8(double Value)
{
 if (Value > FRCenter) return (Value < FRCenter + 0.5*F3R) ? 1.0 : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::NMembershipFunction8(double Value)
{
 if (Value > FRCenter) return (Value < FRCenter + 0.5*F3R) ? 1.0 : 0.0;
 if (Value < FLCenter) return (Value > FLCenter - 0.5*F3L) ? 1.0 : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::RMembershipFunction8(double Value)
{
 if (Value < FLCenter) return (Value > FLCenter - 0.5*F3L) ? 1.0 : 0.0;
 return 1.0;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzySet::ExecuteFuzzyValue(double CrispValue)
{
 if (FContrastEnabled) {
	 double FuzzyValue = (this->*FFunction)(CrispValue);
	 if (FuzzyValue < 0.5) return (CrispValue < FLCenter) ? (FCL * pow(FuzzyValue, FLContrast)) : (FCR * pow(FuzzyValue, FRContrast));
					  else return (CrispValue < FLCenter) ? (1.0 - FCL * pow(1.0 - FuzzyValue, FLContrast)) : (1.0 - FCR * pow(1.0 - FuzzyValue, FRContrast));
 }
 return (this->*FFunction)(CrispValue);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::SaveToFile(IAIEBinaryFile* File)
{
 File->SaveString(FName);
 File->SaveDouble(FLCenter);
 File->SaveDouble(FRCenter);
 File->SaveDouble(FLAlpha);
 File->SaveDouble(FRAlpha);
 File->SaveDouble(FLContrast);
 File->SaveDouble(FRContrast);
 File->SaveBoolean(FCoreEnabled);
 File->SaveBoolean(FContrastEnabled);

 File->SaveDouble(FLCenter);
 File->SaveDouble(FRCenter);
 File->SaveDouble(F1R);
 File->SaveDouble(F1L);
 File->SaveDouble(F3R);
 File->SaveDouble(F3L);
 File->SaveDouble(F5R);
 File->SaveDouble(F5L);
 File->SaveDouble(FCL);
 File->SaveDouble(FCR);

 File->SaveInteger((int)FMembershipFunction);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzySet::LoadFromFile(IAIEBinaryFile* File)
{
 SetName(File->LoadString());
 File->LoadDouble();
 File->LoadDouble();
 FLAlpha = File->LoadDouble();
 FRAlpha = File->LoadDouble();
 FLContrast = File->LoadDouble();
 FRContrast = File->LoadDouble();
 FCoreEnabled = File->LoadBoolean();
 FContrastEnabled = File->LoadBoolean();

 FLCenter = File->LoadDouble();
 FRCenter = File->LoadDouble();
 F1R = File->LoadDouble();
 F1L = File->LoadDouble();
 F3R = File->LoadDouble();
 F3L = File->LoadDouble();
 F5R = File->LoadDouble();
 F5L = File->LoadDouble();
 FCL = File->LoadDouble();
 FCR = File->LoadDouble();

 SetMembershipFunction((TAIEMembershipFunction)File->LoadInteger());
}
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyCollection
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyCollection::TAIELinguisticFuzzyCollection(TAIELinguisticFuzzyStructure* AOwner,
			int ANumaNodeIndex, int AIndex, const char* AName, TAIEColumnKind AKind, TAIEColumnType AType,
			double AMinValue, double AMaxValue, TAIEMembershipFunction ADefaultMembershipFunction,
			bool ADefaultCoreEnabled, bool ADefaultContrastEnabled, bool ADefaultSFP)
{
 FOwner = AOwner;
 FNumaNodeIndex = ANumaNodeIndex;
 FIndex = AIndex;
 FFuzzySet.SetNumaNodeIndex(ANumaNodeIndex);
 FDefaultMembershipFunction = ADefaultMembershipFunction;
 FDefaultCoreEnabled = ADefaultCoreEnabled;
 FDefaultContrastEnabled = ADefaultContrastEnabled;
 FSFP = ADefaultSFP;
 FName = strdup(AName);
 FMinValue = AMinValue;
 FMaxValue = AMaxValue;
 FKind = AKind;
 FType = AType;
 switch (FType) {
	case ctDouble:  FExecuteFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteFuzzyValue1;
					FExecuteDataFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue1;
					break;
	case ctString:  FExecuteFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteFuzzyValue2;
					FExecuteDataFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue2;
					break;
	case ctInteger: FExecuteFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteFuzzyValue3;
					FExecuteDataFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue3;
					break;
	case ctBoolean: FExecuteFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteFuzzyValue4;
                    FExecuteDataFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue4;
					break;
 }

 FFuzzySet.Resize(0);
}
//---------------------------------------------------------------------------
__stdcall TAIELinguisticFuzzyCollection::~TAIELinguisticFuzzyCollection(void)
{
 for (int i = 0; i < FFuzzySet.GetCount(); ++i) FFuzzySet[i].~TAIELinguisticFuzzySet();
 FFuzzySet.Resize(0);
 free(FName);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyCollection::Assign(TAIELinguisticFuzzyCollection* Source)
{
 FDefaultMembershipFunction = Source->FDefaultMembershipFunction;
 FDefaultCoreEnabled = Source->FDefaultCoreEnabled;
 FDefaultContrastEnabled = Source->FDefaultContrastEnabled;
 FSFP = Source->FSFP;
 free(FName); FName = strdup(Source->FName);
 FMinValue = Source->FMinValue;
 FMaxValue = Source->FMaxValue;
 FKind = Source->FKind;
 FType = Source->FType;
 switch (FType) {
	case ctDouble:  FExecuteFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteFuzzyValue1;
					FExecuteDataFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue1;
					break;
	case ctString:  FExecuteFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteFuzzyValue2;
					FExecuteDataFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue2;
					break;
	case ctInteger: FExecuteFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteFuzzyValue3;
					FExecuteDataFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue3;
					break;
	case ctBoolean: FExecuteFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteFuzzyValue4;
                    FExecuteDataFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue4;
					break;
 }

 SetFuzzySetCount(Source->FFuzzySet.GetCount());
 for (int i = 0; i < Source->FFuzzySet.GetCount(); ++i) FFuzzySet[i].Assign(&Source->FFuzzySet[i]);
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyCollection::GetIndex(void)
{
 return FIndex;
}
//---------------------------------------------------------------------------
const char* __stdcall TAIELinguisticFuzzyCollection::GetName(void)
{
 return FName;
}
//---------------------------------------------------------------------------
TAIEColumnKind __stdcall TAIELinguisticFuzzyCollection::GetKind(void)
{
 return FKind;
}
//---------------------------------------------------------------------------
TAIEColumnType __stdcall TAIELinguisticFuzzyCollection::GetType(void)
{
 return FType;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyCollection::GetMinValue(void)
{
 return FMinValue;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyCollection::GetMaxValue(void)
{
 return FMaxValue;
}
//---------------------------------------------------------------------------
const char* __stdcall TAIELinguisticFuzzyCollection::GetTypeName(void)
{
 if (FType == ctInteger) return "liczba ca³kowita";
 if (FType == ctDouble) return "liczba rzeczywista";
 if (FType == ctBoolean) return "wartoœæ logiczna";
 return "ci¹g znaków";
}
//---------------------------------------------------------------------------
const char* __stdcall TAIELinguisticFuzzyCollection::GetKindName(void)
{
 if (FKind == ckOutput) return "wyjœciowy";
 if (FKind == ckInput) return "wejœciowy";
 return "komentarz";
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyCollection::GetIsNominalType(void)
{
 return (FType == ctString) || (FType == ctBoolean);
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyCollection::GetFuzzySetCount(void)
{
 return FFuzzySet.GetCount();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyCollection::SetFuzzySetCount(int AFuzzySetCount)
{
 if (FFuzzySet.GetCount() == AFuzzySetCount) return;
 if (AFuzzySetCount < 0) return;

 // - korekta bazy regu³
 for (int k = 0; k < FOwner->GetFuzzyRulebaseCount(); ++k) {
	  IAIELinguisticFuzzyRulebase* LFuzzyRulebase = FOwner->GetFuzzyRulebase(k);
	  for (int i = 0; i < LFuzzyRulebase->GetFuzzyRuleCount(); ++i) {
		   IAIELinguisticFuzzyRule* LFuzzyRule = LFuzzyRulebase->GetFuzzyRule(i);
		   for (int j = 0; j < LFuzzyRule->GetInpFuzzyAttrCount(); ++j) {
				if (FOwner->GetCurrentFuzzyCollection(j) != this) continue;
				if (LFuzzyRule->GetInpFuzzySetIndex(j) >= AFuzzySetCount) LFuzzyRule->SetInpFuzzySetIndex(j, 0);
		   }
		   if (FOwner->GetCurrentFuzzyCollection(LFuzzyRule->GetInpFuzzyAttrCount() + k) != this) continue;
		   if (LFuzzyRule->GetOutFuzzySetIndex() >= AFuzzySetCount) LFuzzyRule->SetOutFuzzySetIndex(0);
	  }
 }

 // - zmiana liczby zbiorrów rozmytych
 int LFuzzySetCount = FFuzzySet.GetCount();
 if (AFuzzySetCount < LFuzzySetCount)
	 for (int i = AFuzzySetCount; i < LFuzzySetCount; ++i) FFuzzySet[i].~TAIELinguisticFuzzySet();
 FFuzzySet.Resize(AFuzzySetCount);
 if (LFuzzySetCount < AFuzzySetCount)
	 for (int i = LFuzzySetCount; i < AFuzzySetCount; ++i)
		  new (&FFuzzySet[i]) TAIELinguisticFuzzySet(this, AFuzzySetCount, i, FNumaNodeIndex, FOwner->GetOutputFuzzyAttrCount(),
													 fskNormal, FMaxValue, FMinValue,
													 FDefaultMembershipFunction, FDefaultCoreEnabled, FDefaultContrastEnabled);

 // - korekta typów funkcji przynale¿noœci w zbiorach rozmytych
 if (FFuzzySet.GetCount() == 1)  FFuzzySet[0].SetKind(fskNormal);
 if (FFuzzySet.GetCount() >= 2) {
	 FFuzzySet[0].SetKind(fskLeft);
	 for (int i = 1; i < FFuzzySet.GetCount() - 1; ++i) FFuzzySet[i].SetKind(fskNormal);
	 FFuzzySet[FFuzzySet.GetCount() - 1].SetKind(fskRight);
 }

 //ustalenie pocz¹tkowych wartoœci parametrów zbiorów
 if (AFuzzySetCount == 1) {
	 FFuzzySet[0].SetName("Medium");
	 FFuzzySet[0].SetLCenter((FMaxValue + FMinValue) / 2.0);
	 FFuzzySet[0].SetRCenter(FFuzzySet[0].GetLCenter());
	 FFuzzySet[0].SetLAlpha((FMaxValue - FMinValue) / (1.6 * AFuzzySetCount));
	 FFuzzySet[0].SetRAlpha(FFuzzySet[0].GetLAlpha());
 } else {
	 for (int i = 0; i < AFuzzySetCount; ++i) {
		  if (i == 0) {
			  FFuzzySet[i].SetName("Small");
			  FFuzzySet[i].SetLCenter(FMinValue);
			  FFuzzySet[i].SetRCenter(FMinValue);
			  FFuzzySet[i].SetLAlpha((FMaxValue - FMinValue) / (1.6 * AFuzzySetCount));
			  FFuzzySet[i].SetRAlpha((FMaxValue - FMinValue) / (1.6 * AFuzzySetCount));
		  } else
		  if (i == AFuzzySetCount - 1) {
			  FFuzzySet[i].SetName("Large");
			  FFuzzySet[i].SetLCenter(FMaxValue);
			  FFuzzySet[i].SetRCenter(FMaxValue);
			  FFuzzySet[i].SetLAlpha((FMaxValue - FMinValue) / (1.6 * AFuzzySetCount));
			  FFuzzySet[i].SetRAlpha((FMaxValue - FMinValue) / (1.6 * AFuzzySetCount));
		  } else {
			  FFuzzySet[i].SetName((AFuzzySetCount == 3) ? "Medium" : AnsiString("Medium" + IntToStr(i)).c_str());
			  FFuzzySet[i].SetLCenter(FMinValue + i * (FMaxValue - FMinValue) / (AFuzzySetCount - 1));
			  FFuzzySet[i].SetRCenter(FMinValue + i * (FMaxValue - FMinValue) / (AFuzzySetCount - 1));
			  FFuzzySet[i].SetLAlpha((FMaxValue - FMinValue) / (1.6 * AFuzzySetCount));
			  FFuzzySet[i].SetRAlpha((FMaxValue - FMinValue) / (1.6 * AFuzzySetCount));
		  }
	 }
 }

 FOwner->PrepareFuzzyRulebase();
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzySet* __stdcall TAIELinguisticFuzzyCollection::GetFuzzySet(int FuzzySetIndex)
{
 return &FFuzzySet[FuzzySetIndex];
}
//---------------------------------------------------------------------------
int __stdcall TAIELinguisticFuzzyCollection::GetFuzzySetIndexByName(const char* FuzzySetName)
{
 for (int i = 0; i < FFuzzySet.GetCount(); ++i) if (!strcmp((char*)FFuzzySet[i].GetName(), (char*)FuzzySetName)) return i;
 return -1;
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyCollection::ExecuteFuzzyValue1(IAIEDataSetRecord* Record, int AttrIndex, int FuzzySetIndex)
{
 return FFuzzySet[FuzzySetIndex].ExecuteFuzzyValue(Record->GetInputValueAsDouble(AttrIndex));
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyCollection::ExecuteFuzzyValue2(IAIEDataSetRecord* Record, int AttrIndex, int FuzzySetIndex)
{
 return FFuzzySet[FuzzySetIndex].ExecuteFuzzyValue(GetFuzzySetIndexByName(Record->GetInputValueAsString(AttrIndex)));
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyCollection::ExecuteFuzzyValue3(IAIEDataSetRecord* Record, int AttrIndex, int FuzzySetIndex)
{
 return FFuzzySet[FuzzySetIndex].ExecuteFuzzyValue(Record->GetInputValueAsInteger(AttrIndex));
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyCollection::ExecuteFuzzyValue4(IAIEDataSetRecord* Record, int AttrIndex, int FuzzySetIndex)
{
 return FFuzzySet[FuzzySetIndex].ExecuteFuzzyValue(Record->GetInputValueAsBoolean(AttrIndex));
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyCollection::ExecuteFuzzyValue(IAIEDataSetRecord* Record, int AttrIndex, int FuzzySetIndex)
{
 return (this->*FExecuteFuzzyValue)(Record, AttrIndex, FuzzySetIndex);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue1(double* Record, int AttrIndex, int FuzzySetIndex)
{
 return FFuzzySet[FuzzySetIndex].ExecuteFuzzyValue(Record[AttrIndex]);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue2(double* Record, int AttrIndex, int FuzzySetIndex)
{
 return FFuzzySet[FuzzySetIndex].ExecuteFuzzyValue(Record[AttrIndex]);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue3(double* Record, int AttrIndex, int FuzzySetIndex)
{
 return FFuzzySet[FuzzySetIndex].ExecuteFuzzyValue(Record[AttrIndex]);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue4(double* Record, int AttrIndex, int FuzzySetIndex)
{
 return FFuzzySet[FuzzySetIndex].ExecuteFuzzyValue(Record[AttrIndex]);
}
//---------------------------------------------------------------------------
double __stdcall TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue(double* Record, int AttrIndex, int FuzzySetIndex)
{
 return (this->*FExecuteDataFuzzyValue)(Record, AttrIndex, FuzzySetIndex);
}
//---------------------------------------------------------------------------
bool __stdcall TAIELinguisticFuzzyCollection::GetIsSFP(void)
{
 return FSFP;
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyCollection::SetIsSFP(bool ASFP)
{
 FSFP = ASFP;
 for (int i = 0; i < FFuzzySet.GetCount(); ++i) FFuzzySet[i].Prepare();
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyCollection::SaveToFile(IAIEBinaryFile* File)
{
 File->SaveInteger(FDefaultMembershipFunction);
 File->SaveBoolean(FDefaultCoreEnabled);
 File->SaveBoolean(FDefaultContrastEnabled);
 File->SaveBoolean(FSFP);
 File->SaveDouble(FMinValue);
 File->SaveDouble(FMaxValue);
 File->SaveString(FName);
 File->SaveInteger(FKind);
 File->SaveInteger(FType);
 File->SaveInteger(FFuzzySet.GetCount());
 for (int i = 0; i < FFuzzySet.GetCount(); ++i) FFuzzySet[i].SaveToFile(File);
}
//---------------------------------------------------------------------------
void __stdcall TAIELinguisticFuzzyCollection::LoadFromFile(IAIEBinaryFile* File)
{
 FDefaultMembershipFunction = (TAIEMembershipFunction)File->LoadInteger();
 FDefaultCoreEnabled = File->LoadBoolean();
 FDefaultContrastEnabled = File->LoadBoolean();
 FSFP = File->LoadBoolean();
 FMinValue = File->LoadDouble();
 FMaxValue = File->LoadDouble();
 free(FName); FName = strdup(File->LoadString());
 FKind = (TAIEColumnKind)File->LoadInteger();
 FType = (TAIEColumnType)File->LoadInteger();
 switch (FType) {
	case ctDouble:  FExecuteFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteFuzzyValue1;
					FExecuteDataFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue1;
					break;
	case ctString:  FExecuteFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteFuzzyValue2;
					FExecuteDataFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue2;
					break;
	case ctInteger: FExecuteFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteFuzzyValue3;
					FExecuteDataFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue3;
					break;
	case ctBoolean: FExecuteFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteFuzzyValue4;
					FExecuteDataFuzzyValue = &TAIELinguisticFuzzyCollection::ExecuteDataFuzzyValue4;
					break;
 }

 SetFuzzySetCount(File->LoadInteger());
 for (int i = 0; i < FFuzzySet.GetCount(); ++i) FFuzzySet[i].LoadFromFile(File);
}
//---------------------------------------------------------------------------
