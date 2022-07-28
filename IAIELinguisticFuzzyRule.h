//---------------------------------------------------------------------------
#ifndef IAIELinguisticFuzzyRuleH
#define IAIELinguisticFuzzyRuleH
//---------------------------------------------------------------------------
#include "IAIEDataSet.h"
#include "IAIEDataSetColumn.h"
#include "IAIELinguisticFuzzyStructure.h"
//---------------------------------------------------------------------------
// IAIELinguisticFuzzyRule.h
// Copyright 2009-2010 by Filip Rudziñski
// -
// Linguistic fuzzy rule interface
//---------------------------------------------------------------------------
// IAIELinguisticFuzzyRule
//---------------------------------------------------------------------------
// Interfejs regu³y systemu rozmytego:
// Funkcje:
// ExecuteTNorm - przelicza wartoœæ t-normy z wykorzystaniem operatora koniunkcji
//				  (wartoœæ zwana "si³¹" regu³y
// W³asnoœci:
// FuzzyRulebase - wskaŸnik do bazy regu³
// Index - numer regu³y w bazie regu³
// Enabled - TRUE je¿eli regu³a jest w³¹czona (uwzglêdniana w mechaniŸmie wnioskowania)
// Locked - TRUE je¿eli regu³a nie podlega modyfikacji podczas uczenia (regu³a ekspercka)
// Text - tekstowa postaæ regu³y
// PlainText - tekstowa postaæ regu³y
// IsEmpty - TRUE je¿eli regu³a nie posiada przeslanek
// InpFuzzyAttrEnabledCount - liczba przes³anek (d³ugoœæ regu³y)
//---------------------------------------------------------------------------
class IAIELinguisticFuzzyStructure;
//---------------------------------------------------------------------------
class IAIELinguisticFuzzyRule
{
 public:
		virtual IAIELinguisticFuzzyRulebase* __stdcall GetFuzzyRulebase(void) = 0;
		virtual bool __stdcall GetEnabled(void) = 0;
		virtual void __stdcall SetEnabled(bool AEnabled) = 0;
		virtual bool __stdcall GetLocked(void) = 0;
		virtual void __stdcall SetLocked(bool ALocked) = 0;
		virtual bool __stdcall GetIsEmpty(void) = 0;
		virtual int  __stdcall GetLength(void) = 0;
		virtual const char* __stdcall GetText(void) = 0;
		virtual const char* __stdcall GetPlainText(void) = 0;
		virtual double __stdcall GetStrength(void) = 0;
		virtual void __stdcall SetStrength(double AStrength) = 0;
		virtual int __stdcall GetFireCount(void) = 0;
		virtual void __stdcall SetFireCount(int AFireCount) = 0;

		virtual int __stdcall GetInpFuzzyAttrCount(void) = 0;
		virtual int __stdcall GetInpFuzzySetCount(int AAttrIndex) = 0;
		virtual int __stdcall GetInpFuzzySetIndex(int AAttrIndex) = 0;
		virtual void __stdcall SetInpFuzzySetIndex(int AAttrIndex, int AFuzzySetIndex) = 0;
		virtual bool __stdcall GetInpFuzzySetNegated(int AAttrIndex) = 0;
		virtual void __stdcall SetInpFuzzySetNegated(int AAttrIndex, bool AInpFuzzySetNegated) = 0;

		virtual int __stdcall GetOutFuzzySetIndex(void) = 0;
		virtual void __stdcall SetOutFuzzySetIndex(int AFuzzySetIndex) = 0;
		virtual int __stdcall GetOutFuzzySetCount(void) = 0;

		virtual double __stdcall ExecuteTNorm(IAIEDataSetRecord* Record) = 0;
		virtual double __stdcall ExecuteDataTNorm(double* Record) = 0;

 #ifdef __BORLANDC__
		__property IAIELinguisticFuzzyRulebase* FuzzyRulebase = {read=GetFuzzyRulebase};
		__property bool Enabled = {read=GetEnabled, write=SetEnabled};
		__property bool Locked = {read=GetLocked, write=SetLocked};
		__property bool IsEmpty = {read=GetIsEmpty};
		__property int  Length = {read=GetLength};
		__property const char* Text = {read=GetText};
		__property const char* PlainText = {read=GetPlainText};

		__property int InpFuzzyAttrCount = {read=GetInpFuzzyAttrCount};
		__property int InpFuzzySetCount[int AttrIndex] = {read=GetInpFuzzySetCount};
		__property int InpFuzzySetIndex[int AttrIndex] = {read=GetInpFuzzySetIndex, write=SetInpFuzzySetIndex};
		__property bool InpFuzzySetNegated[int AttrIndex] = {read=GetInpFuzzySetNegated, write=SetInpFuzzySetNegated};
		__property int OutFuzzySetIndex = {read=GetOutFuzzySetIndex, write=SetOutFuzzySetIndex};
		__property int OutFuzzySetCount = {read=GetOutFuzzySetCount};
		__property double Strength = {read=GetStrength, write=SetStrength};
		__property int FireCount = {read=GetFireCount, write=SetFireCount};
 #endif
};
//---------------------------------------------------------------------------
// IAIELinguisticFuzzyRulebase
//---------------------------------------------------------------------------
// Interfejs bazy regu³ systemu rozmytego
// Funkcje:
// AppendFuzzyRule - dodaje regu³ê do bazy regu³
// InsertFuzzyRule - wstawia regu³ê do bazy regu³ pod numerem Index
// DeleteFuzzyRule - usuwa regu³ê z bazy regul o numerze Index
// W³asnoœci:
// FuzzyStructure wskaxnik do struktury systemu rozmytego
// Index - numer regu³y
// Type - typ danych atrybutu wyjœciowego (koresponduj¹cego z baz¹ regu³)
// IsNominalType - TRUE jezeli typ danych atrybutu wyjœciowego (koresponduj¹cego z baz¹ regu³) jest
//					³añcuchem znaków lub logicznym
// TNormType - operator koniunkcji
// SNormType - operator alternatywy
// ImplicationOperator - operator implikacji
// AggregationOperator - operator agregacji
// InferenceMode - model wnioskowania
// DefuzzificationMethod - metoda defuzyfikacji
// RuleCount - liczba regu³ w bazie regu³
// Rule[int Index] - wskaŸnik do regu³y o numerze Index
// ActiveRuleCount - liczba w³¹czonych regu³
//---------------------------------------------------------------------------
class IAIELinguisticFuzzyRulebase
{
 public:
		virtual IAIELinguisticFuzzyStructure* __stdcall GetFuzzyStructure(void) = 0;
		virtual int __stdcall GetFuzzyAttrIndex(void) = 0;
		virtual int __stdcall GetOutFuzzyAttrIndex(void) = 0;
		virtual int __stdcall GetInpFuzzyAttrCount(void) = 0;
		virtual TAIEColumnType __stdcall GetType(void) = 0;
		virtual bool __stdcall GetIsNominalType(void) = 0;

		virtual TAIEFuzzyTNormType __stdcall GetTNormType(void) = 0;
		virtual void __stdcall SetTNormType(TAIEFuzzyTNormType ATNormType) = 0;
		virtual TAIEFuzzySNormType __stdcall GetSNormType(void) = 0;
		virtual void __stdcall SetSNormType(TAIEFuzzySNormType ASNormType) = 0;
		virtual TAIEImplicationOperator __stdcall GetImplicationOperator(void) = 0;
		virtual void __stdcall SetImplicationOperator(TAIEImplicationOperator AImplicationOperator) = 0;
		virtual TAIEAggregationOperator __stdcall GetAggregationOperator(void) = 0;
		virtual void __stdcall SetAggregationOperator(TAIEAggregationOperator AAggregationOperator) = 0;
		virtual TAIEInferenceMode __stdcall GetInferenceMode(void) = 0;
		virtual void __stdcall SetInferenceMode(TAIEInferenceMode AInferenceMode) = 0;
		virtual TAIEDefuzzificationMethod __stdcall GetDefuzzificationMethod(void) = 0;
		virtual void __stdcall SetDefuzzificationMethod(TAIEDefuzzificationMethod ADefuzzificationMethod) = 0;

		virtual int __stdcall GetMinFuzzyRuleCount(void) = 0;
		virtual int __stdcall GetMaxFuzzyRuleCount(void) = 0;
		virtual int __stdcall GetFuzzyRuleCount(void) = 0;
		virtual void __stdcall SetFuzzyRuleCount(int ACount) = 0;
		virtual IAIELinguisticFuzzyRule* __stdcall GetFuzzyRule(int AIndex) = 0;

		virtual int __stdcall GetEnabledFuzzyRuleCount(void) = 0;
		virtual int __stdcall GetEnabledFuzzySetCount(void) = 0;
		virtual int __stdcall GetEnabledInpFuzzyAttrCount(void) = 0;
		virtual int __stdcall GetEnabledOutFuzzyAttrCount(void) = 0;
		virtual int __stdcall GetEnabledFuzzyRuleCountOfFuzzySet(int AFuzzyAttrIndex, int AFuzzySetIndex) = 0;

		virtual double __stdcall GetFuzzyRulebaseComplexity(void) = 0;

		virtual double __stdcall GetNormEnabledFuzzyRuleComplexity(void) = 0;
		virtual double __stdcall GetNormEnabledFuzzyRuleCount(double MinFuzzyRuleCount, double MaxFuzzyRuleCount) = 0;
		virtual double __stdcall GetNormEnabledFuzzyAttrCount(void) = 0;
		virtual double __stdcall GetNormEnabledFuzzySetCount(void) = 0;

		virtual IAIELinguisticFuzzyRule* __stdcall AppendFuzzyRule(void) = 0;
		virtual IAIELinguisticFuzzyRule* __stdcall InsertFuzzyRule(int Index) = 0;
		virtual void __stdcall DeleteFuzzyRule(int Index) = 0;

		virtual int __stdcall    ExecuteSNorm(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex) = 0;
		virtual int __stdcall    ExecuteDataSNorm(double* Record, double* SNormTable, int& RuleIndex) = 0;
		virtual double __stdcall ExecuteImplication(double X, double Y) = 0;
		virtual double __stdcall ExecuteCrispValue(double* SNormTable, int FuzzySetIndex = -1) = 0;
		virtual double __stdcall ExecuteMshpDegree(double CY, double* SNormTable, int FuzzySetIndex = -1) = 0;
		virtual double __stdcall ExecuteOutput(double* SNormTable) = 0;

 #ifdef __BORLANDC__
		__property IAIELinguisticFuzzyStructure* FuzzyStructure = {read=GetFuzzyStructure};
		__property int FuzzyAttrIndex = {read=GetFuzzyAttrIndex};
		__property int OutFuzzyAttrIndex = {read=GetOutFuzzyAttrIndex};
		__property int InpFuzzyAttrCount = {read=GetInpFuzzyAttrCount};
		__property TAIEColumnType Type = {read=GetType};
		__property bool IsNominalType = {read=GetIsNominalType};

		__property TAIEFuzzyTNormType TNormType = {read=GetTNormType, write=SetTNormType};
		__property TAIEFuzzySNormType SNormType = {read=GetSNormType, write=SetSNormType};
		__property TAIEImplicationOperator ImplicationOperator = {read=GetImplicationOperator, write=SetImplicationOperator};
		__property TAIEAggregationOperator AggregationOperator = {read=GetAggregationOperator, write=SetAggregationOperator};
		__property TAIEInferenceMode InferenceMode = {read=GetInferenceMode, write=SetInferenceMode};
		__property TAIEDefuzzificationMethod DefuzzificationMethod = {read=GetDefuzzificationMethod, write=SetDefuzzificationMethod};

		__property int MinFuzzyRuleCount = {read=GetMinFuzzyRuleCount};
		__property int MaxFuzzyRuleCount = {read=GetMaxFuzzyRuleCount};
		__property int FuzzyRuleCount = {read=GetFuzzyRuleCount, write=SetFuzzyRuleCount};
		__property IAIELinguisticFuzzyRule* FuzzyRule[int Index] = {read=GetFuzzyRule};

		__property double FuzzyRulebaseComplexity = {read=GetFuzzyRulebaseComplexity};
		__property int EnabledFuzzyRuleCount = {read=GetEnabledFuzzyRuleCount};
		__property int EnabledFuzzySetCount = {read=GetEnabledFuzzySetCount};
		__property int EnabledInpFuzzyAttrCount = {read=GetEnabledInpFuzzyAttrCount};
		__property int EnabledOutFuzzyAttrCount = {read=GetEnabledOutFuzzyAttrCount};
 #endif
};
//---------------------------------------------------------------------------
#endif
