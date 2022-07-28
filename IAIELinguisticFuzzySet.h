//---------------------------------------------------------------------------
#ifndef IAIELinguisticFuzzySetH
#define IAIELinguisticFuzzySetH
//---------------------------------------------------------------------------
#include "IAIEDataSet.h"
#include "IAIEDataSetColumn.h"
#include "IAIELinguisticFuzzyStructure.h"
//---------------------------------------------------------------------------
// IAIELinguisticFuzzyStructure.h
// Copyright 2009-2010 by Filip Rudziñski
// -
// Linguistic fuzzy set and fuzzy collection interface
//---------------------------------------------------------------------------
// IAIELinguisticFuzzySet
//---------------------------------------------------------------------------
// Interfejs zbioru rozmytego:
//  Funkcje:
//  ExecuteFuzzyValue - przeliczenie wartoœci ostrej na wartoœæ rozmyt¹
//  W³asnoœci:
//  Name - Nazwa zbioru (wartoœæ lingwistyczna) (np. Small, Medium, Large)
//  MembershipFunction - Rodzaj funkcji przynale¿noœci zbioru rozmytego
//  Parametr funkcji przynale¿noœci:
//  Center - wartoœæ dla której funkcja przyjmuje 1 w œrodku przedzialu okreslonego parametrem Core
//  Core   - szerokoœæ przedzia³u dla którego funkcja przyjmuje 1
//  LContrast - kontrast "lewej czêœci" zbioru rozmytego
//  RContrast - kontrast "prawej czêœci" zbioru rozmytego
//  LAlpha - szerokoœæ "lewego boku" (odcinka od Center do lewego punktu, w którym funkcja przyjmuje wartoœæ 0.1
//  RAlpha - szerokoœæ "prawgo boku" (odcinka od Center do prawego punktu, w którym funkcja przyjmuje wartoœæ 0.1
//           (parametry LAlpha i RAlpha nie maj¹ znaczenia dla funkcji skrajnych, umieszczonych odpowiednio po lewej i prawej stronie)
//	Area   - pole powierzchni pod funkcja przynale¿noœci
//	LArea  - pole powierzchni pod funkcja przynale¿noœci po lewej stronie od centrum
//	RArea  - pole powierzchni pod funkcja przynale¿noœci po prawej stronie od centrum
//---------------------------------------------------------------------------
class IAIELinguisticFuzzySet
{
 public:
		virtual const char* __stdcall GetName(void) = 0;
		virtual void __stdcall SetName(const char* AName) = 0;
		virtual TAIEMembershipFunction __stdcall GetMembershipFunction(void) = 0;
		virtual void __stdcall SetMembershipFunction(TAIEMembershipFunction AMembershipFunction) = 0;
		virtual bool __stdcall GetCoreEnabled(void) = 0;
		virtual void __stdcall SetCoreEnabled(bool ACoreEnabled) = 0;
		virtual bool __stdcall GetContrastEnabled(void) = 0;
		virtual void __stdcall SetContrastEnabled(bool AContrastEnabled) = 0;
		virtual double __stdcall GetLCenter(void) = 0;
		virtual void __stdcall SetLCenter(double AValue) = 0;
		virtual double __stdcall GetRCenter(void) = 0;
		virtual void __stdcall SetRCenter(double AValue) = 0;
		virtual double __stdcall GetLContrast(void) = 0;
		virtual void __stdcall SetLContrast(double AValue) = 0;
		virtual double __stdcall GetRContrast(void) = 0;
		virtual void __stdcall SetRContrast(double AValue) = 0;
		virtual double __stdcall GetLAlpha(void) = 0;
		virtual void __stdcall SetLAlpha(double AValue) = 0;
		virtual double __stdcall GetRAlpha(void) = 0;
		virtual void __stdcall SetRAlpha(double AValue) = 0;
		virtual double __stdcall GetArea(void) = 0;
		virtual double __stdcall GetLArea(void) = 0;
		virtual double __stdcall GetRArea(void) = 0;
		virtual double __stdcall GetCenter(void) = 0;
		virtual double __stdcall GetCore(void) = 0;
		virtual bool __stdcall GetIsActive(void) = 0;
		virtual int __stdcall GetAssignedFuzzyAntecedentCount(int AFuzzyRulebaseIndex) = 0;

		virtual double __stdcall ExecuteFuzzyValue(double CrispValue) = 0;

 #ifdef __BORLANDC__
		__property const char* Name = {read=GetName, write=SetName};
		__property bool CoreEnabled   = {read=GetCoreEnabled, write=SetCoreEnabled};
		__property bool ContrastEnabled   = {read=GetContrastEnabled, write=SetContrastEnabled};
		__property double Center = {read=GetCenter};
		__property double Core = {read=GetCore};
		__property double LCenter = {read=GetLCenter, write=SetLCenter};
		__property double RCenter = {read=GetRCenter, write=SetRCenter};
		__property double LContrast = {read=GetLContrast, write=SetLContrast};
		__property double RContrast = {read=GetRContrast, write=SetRContrast};
		__property double LAlpha = {read=GetLAlpha, write=SetLAlpha};
		__property double RAlpha = {read=GetRAlpha, write=SetRAlpha};
		__property double Area = {read=GetArea};
		__property double LArea = {read=GetLArea};
		__property double RArea = {read=GetRArea};
		__property bool IsActive = {read=GetIsActive};
		__property int AssignedFuzzyAntecedentCount[int AFuzzyRulebaseIndex] = {read=GetAssignedFuzzyAntecedentCount};

		__property TAIEMembershipFunction MembershipFunction = {read=GetMembershipFunction, write=SetMembershipFunction};
 #endif
};
//---------------------------------------------------------------------------
// IAIELinguisticFuzzyCollection
//---------------------------------------------------------------------------
// Interfejs zestawu zbiorów rozmytych:
// W³asnoœci:
// Name - nazwa zestawu, koresponduj¹ca z nazw¹ wejœcia systemu
// Type - typ danych
// Kind - rodzaj interfejsu (wejœciowy, wyjœciowy)
// MinValue - minimalna dopuszczalna wartoœæ ostra
// MaxValue - maksymalna dopuszczalna wartoœæ ostra
// TypeName - nazwa tekstowa typu danych
// KindName - nazwa tekstowa typu interfejsu danych
// IsNominalType - TRUE je¿eli typ danych jest ³añcuchem znaków lub logicznym
// IsActive - TRUE je¿eli zestaw jest pod³¹czony do co najmniej jednego z wejœæ/wyjœæ systemu
// FuzzySetCount - liczba zbiorów rozmytych w zestawie
// FuzzySet[int FuzzySetIndex] - wskaŸnik do zbioru rozmytego o numerze FuzzySetIndex
// FuzzySetIndexByName[const char* FuzzySetName] - numer zbioru rozmytego w zestawie wyznaczony na podstawie jego nazwy FuzzySetName
// MembershipFunction - typ funkcji przynale¿noœci zbiorów rozmytych
//---------------------------------------------------------------------------
class IAIELinguisticFuzzyCollection
{
 public:
		virtual int __stdcall GetIndex(void) = 0;
		virtual const char* __stdcall GetName(void) = 0;
		virtual TAIEColumnType __stdcall GetType(void) = 0;
		virtual TAIEColumnKind __stdcall GetKind(void) = 0;
		virtual double __stdcall GetMinValue(void) = 0;
		virtual double __stdcall GetMaxValue(void) = 0;
		virtual const char* __stdcall GetTypeName(void) = 0;
		virtual const char* __stdcall GetKindName(void) = 0;
		virtual bool __stdcall GetIsNominalType(void) = 0;

		virtual int __stdcall GetFuzzySetCount(void) = 0;
		virtual void __stdcall SetFuzzySetCount(int ACount) = 0;
		virtual IAIELinguisticFuzzySet* __stdcall GetFuzzySet(int AFuzzySetIndex) = 0;
		virtual int __stdcall GetFuzzySetIndexByName(const char* FuzzySetName) = 0;
		virtual bool __stdcall GetIsSFP(void) = 0;
		virtual void __stdcall SetIsSFP(bool ASFP) = 0;

 #ifdef __BORLANDC__
		__property int Index = {read=GetIndex};
		__property const char* Name = {read=GetName};
		__property double MinValue = {read=GetMinValue};
		__property double MaxValue = {read=GetMaxValue};
		__property TAIEColumnType Type = {read=GetType};
		__property TAIEColumnKind Kind = {read=GetKind};
		__property const char* TypeName = {read=GetTypeName};
		__property const char* KindName = {read=GetKindName};
		__property bool IsNominalType = {read=GetIsNominalType};

		__property int FuzzySetCount = {read=GetFuzzySetCount, write=SetFuzzySetCount};
		__property IAIELinguisticFuzzySet* FuzzySet[int FuzzySetIndex] = {read=GetFuzzySet};
		__property int FuzzySetIndexByName[const char* FuzzySetName] = {read=GetFuzzySetIndexByName};
		__property bool IsSFP = {read=GetIsSFP, write=SetIsSFP};
 #endif __BORLANDC__
};
//---------------------------------------------------------------------------
#endif
