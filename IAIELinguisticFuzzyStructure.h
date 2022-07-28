//---------------------------------------------------------------------------
#ifndef IAIELinguisticFuzzyStructureH
#define IAIELinguisticFuzzyStructureH
//---------------------------------------------------------------------------
#include "IAIEDataSet.h"
#include "IAIEDataSetColumn.h"
//---------------------------------------------------------------------------
// IAIELinguisticFuzzyStructure.h
// Copyright 2009-2010 by Filip Rudziñski
// -
// Linguistic fuzzy structure interface
//---------------------------------------------------------------------------
class IAIELinguisticFuzzyOptions;
class IAIELinguisticFuzzySet;
class IAIELinguisticFuzzyCollection;
class IAIELinguisticFuzzyAttr;
class IAIELinguisticFuzzyRule;
class IAIELinguisticFuzzyRulebase;
class IAIELinguisticFuzzyStructure;
//---------------------------------------------------------------------------
// Funkcje przyale¿noœci:
//---------------------------------------------------------------------------
// fuzzyMembershipFunction0 - funkcja singletonowa
// fuzzyMembershipFunction1 - funkcja gaussowska
// fuzzyMembershipFunction2 - funkcja trójk¹tna
// fuzzyMembershipFunction3 - funkcja cosinusoidalna
// fuzzyMembershipFunction4 - funkcja prostok¹tna (nierozmyty zbiór)
enum TAIEMembershipFunction {fuzzyMembershipFunction0,
							 fuzzyMembershipFunction1, fuzzyMembershipFunction2,
							 fuzzyMembershipFunction3, fuzzyMembershipFunction4};

//---------------------------------------------------------------------------
// Rodziaje operatorów iloczynu zbiorow rozmytych (t-normy)
//---------------------------------------------------------------------------
// fuzzyTNorm1 - norma Zadeha
// fuzzyTNorm2 - norma algebraiczna
// fuzzyTNorm3 - norma £ukasiewicza
// fuzzyTNorm4 - norma Fodora
// fuzzyTNorm5 - norma Drastyczna
// fuzzyTNorm6 - norma Einsteina
enum TAIEFuzzyTNormType {fuzzyTNorm1, fuzzyTNorm2, fuzzyTNorm3, fuzzyTNorm4, fuzzyTNorm5, fuzzyTNorm6};

//---------------------------------------------------------------------------
// Rodziaje operatorów sumy zbiorow rozmytych (s-normy)
//---------------------------------------------------------------------------
// fuzzySNorm1 - norma Zadeha
// fuzzySNorm2 - norma probabilistyczna
// fuzzySNorm3 - norma £ukasiewicza
// fuzzySNorm4 - norma Fodora
// fuzzySNorm5 - norma Drastyczna
// fuzzySNorm6 - norma Einsteina
enum TAIEFuzzySNormType {fuzzySNorm1, fuzzySNorm2, fuzzySNorm3, fuzzySNorm4, fuzzySNorm5, fuzzySNorm6};

//---------------------------------------------------------------------------
// Operator implikacji
//---------------------------------------------------------------------------
// fuzzyImplicationOperator1 - operator Mamdaniego (minimum)
// fuzzyImplicationOperator2 - operator £ukasiewicza
// fuzzyImplicationOperator3 - operator Fodora
// fuzzyImplicationOperator4 - operator Reichenbacha
// fuzzyImplicationOperator5 - operator Kleene'a-Dienesa
// fuzzyImplicationOperator6 - operator Zadeha
// fuzzyImplicationOperator7 - operator Goguena
// fuzzyImplicationOperator8 - operator Godela
// fuzzyImplicationOperator9 - operator Reschera
enum TAIEImplicationOperator {fuzzyImplicationOperator1, fuzzyImplicationOperator2, fuzzyImplicationOperator3,
							  fuzzyImplicationOperator4, fuzzyImplicationOperator5, fuzzyImplicationOperator6,
							  fuzzyImplicationOperator7, fuzzyImplicationOperator8, fuzzyImplicationOperator9,
							  fuzzyImplicationOperator10, fuzzyImplicationOperator11, fuzzyImplicationOperator12};

//---------------------------------------------------------------------------
// Operator agregacji
//---------------------------------------------------------------------------
// fuzzyAggregationOperator1 - operator maksimum
// fuzzyAggregationOperator2 - operator œrednia arytmetyczna
// fuzzyAggregationOperator3 - operator œrednia kwadratowa
// fuzzyAggregationOperator4 - operator œrednia geometryczna
// fuzzyAggregationOperator5 - operator minimum
// (dla trybu FITA operatory s¹ wa¿one)
enum TAIEAggregationOperator {fuzzyAggregationOperator1, fuzzyAggregationOperator2, fuzzyAggregationOperator3,
							  fuzzyAggregationOperator4, fuzzyAggregationOperator5};

//---------------------------------------------------------------------------
// Tryb pracy modu³u wnioskowania
//---------------------------------------------------------------------------
// fuzzyInferenceModeFATI - agregacja przed wnioskowaniem
// fuzzyInferenceModeFITA - wnioskowanie przed agregacj¹
// fuzzyInferenceModeRAW -  wnioskowanie bez agregacji - wartoœæ ostr¹ oblicza siê metod¹ odwrotnej defuzyfikacji
enum TAIEInferenceMode {fuzzyInferenceModeFATI, fuzzyInferenceModeFITA, fuzzyInferenceModeRAW};

//---------------------------------------------------------------------------
// Metody obliczania wyjœcia ostrego:
//---------------------------------------------------------------------------
// fuzzyDefuzzificationCOG - metoda œrodka ciê¿koœci
// fuzzyDefuzzificationCOA - metoda œrodka powierzchni
// fuzzyDefuzzificationMCOA - zmodyfikowana metoda œrodka powierzchni
// fuzzyDefuzzificationFOM - metoda pierwszego maksimum
// fuzzyDefuzzificationMOM - metoda œrodka maksimum
// fuzzyDefuzzificationLOM - metoda ostatniego maksimum
enum TAIEDefuzzificationMethod {fuzzyDefuzzificationCOG, fuzzyDefuzzificationCOA, fuzzyDefuzzificationMCOA,
								fuzzyDefuzzificationFOM, fuzzyDefuzzificationMOM, fuzzyDefuzzificationLOM};

//---------------------------------------------------------------------------
// IAIELinguisticFuzzyOptions
//---------------------------------------------------------------------------
// Domyœlne parametry systemu rozmytego, wykorzystywane podczas inicjalizacji systemu:
// W³asnoœci:
// DefaultMembershipFunction - typ funkcji przynale¿noœci
// DefaultFuzzySetCount - liczba zbiorów rozmytych w zestawach zbiorów
// DefaultTNormType - operator koniunkcji
// DefaultSNormType - operator alternatywy
// DefaultAggregationOperator - operator agregacji
// DefaultImplicationOperator - operator implikacji
// DefaultInferenceMode - model wnioskowania
// DefaultDefuzzificationMethod - metoda defuzyfikacji
// IntegralProbeCount - liczba próbek przedzia³u próbkowania podczas obliczania ca³ek
//						(parametr nie jest zapisywany do pliku)
//---------------------------------------------------------------------------
class IAIELinguisticFuzzyOptions
{
 public:
		virtual TAIEMembershipFunction __stdcall GetDefaultMembershipFunction(void) = 0;
		virtual void __stdcall SetDefaultMembershipFunction(TAIEMembershipFunction ADefaultMembershipFunction) = 0;
		virtual bool __stdcall GetDefaultCoreEnabled(void) = 0;
		virtual void __stdcall SetDefaultCoreEnabled(bool ADefaultCoreEnabled) = 0;
		virtual bool __stdcall GetDefaultContrastEnabled(void) = 0;
		virtual void __stdcall SetDefaultContrastEnabled(bool ADefaultContrastEnabled) = 0;
		virtual bool __stdcall GetDefaultSFP(void) = 0;
		virtual void __stdcall SetDefaultSFP(bool ADefaultSFP) = 0;
		virtual int __stdcall GetDefaultFuzzySetCount(void) = 0;
		virtual void __stdcall SetDefaultFuzzySetCount(int ADefaultFuzzySetCount) = 0;
		virtual TAIEFuzzyTNormType __stdcall GetDefaultTNormType(void) = 0;
		virtual void __stdcall SetDefaultTNormType(TAIEFuzzyTNormType ATNormType) = 0;
		virtual TAIEFuzzySNormType __stdcall GetDefaultSNormType(void) = 0;
		virtual void __stdcall SetDefaultSNormType(TAIEFuzzySNormType ASNormType) = 0;
		virtual TAIEInferenceMode __stdcall GetDefaultInferenceMode(void) = 0;
		virtual void __stdcall SetDefaultInferenceMode(TAIEInferenceMode ADefaultInferenceMode) = 0;
		virtual TAIEImplicationOperator __stdcall GetDefaultImplicationOperator(void) = 0;
		virtual void __stdcall SetDefaultImplicationOperator(TAIEImplicationOperator ADefaultImplicationOperator) = 0;
		virtual TAIEAggregationOperator __stdcall GetDefaultAggregationOperator(void) = 0;
		virtual void __stdcall SetDefaultAggregationOperator(TAIEAggregationOperator ADefaultAggregationOperator) = 0;
		virtual TAIEDefuzzificationMethod __stdcall GetDefaultDefuzzificationMethod(void) = 0;
		virtual void __stdcall SetDefaultDefuzzificationMethod(TAIEDefuzzificationMethod ADefuzzificationMethod) = 0;
		virtual int __stdcall GetIntegralProbeCount(void) = 0;
		virtual void __stdcall SetIntegralProbeCount(int AIntegralProbeCount) = 0;

 #ifdef __BORLANDC__
		__property TAIEMembershipFunction DefaultMembershipFunction = {read=GetDefaultMembershipFunction, write=SetDefaultMembershipFunction};
		__property bool DefaultCoreEnabled = {read=GetDefaultCoreEnabled, write=SetDefaultCoreEnabled};
		__property bool DefaultContrastEnabled = {read=GetDefaultContrastEnabled, write=SetDefaultContrastEnabled};
		__property int  DefaultFuzzySetCount = {read=GetDefaultFuzzySetCount, write=SetDefaultFuzzySetCount};
		__property int  DefaultSFP = {read=GetDefaultSFP, write=SetDefaultSFP};

		__property TAIEFuzzyTNormType DefaultTNormType = {read=GetDefaultTNormType, write=SetDefaultTNormType};
		__property TAIEFuzzySNormType DefaultSNormType = {read=GetDefaultSNormType, write=SetDefaultSNormType};
		__property TAIEAggregationOperator DefaultAggregationOperator = {read=GetDefaultAggregationOperator, write=SetDefaultAggregationOperator};
		__property TAIEImplicationOperator DefaultImplicationOperator = {read=GetDefaultImplicationOperator, write=SetDefaultImplicationOperator};
		__property TAIEInferenceMode DefaultInferenceMode = {read=GetDefaultInferenceMode, write=SetDefaultInferenceMode};
		__property TAIEDefuzzificationMethod DefaultDefuzzificationMethod = {read=GetDefaultDefuzzificationMethod, write=SetDefaultDefuzzificationMethod};

		__property int IntegralProbeCount = {read=GetIntegralProbeCount, write=SetIntegralProbeCount};
 #endif
};
//---------------------------------------------------------------------------
// IAIELinguisticFuzzyAttr
//---------------------------------------------------------------------------
// Interfejs atrybutu wejœciowego lub wyjœciowego:
// W³asnoœci:
// Name - nazwa atrybutu, inaczej nazwa wejœcia/wyjœcia systemu
// Type - typ danych wejœciowych/wyjœciowych
// Kind - rodzaj interfejsu (wejœciowy, wyjœciowy)
// MinValue - minimalna dopuszczalna wartoœæ ostra
// MaxValue - maksymalna dopuszczalna wartoœæ ostra
// TypeName - nazwa tekstowa typu danych
// KindName - nazwa tekstowa typu interfejsu danych
// IsNominalType - TRUE je¿eli typ danych jest ³añcuchem znaków lub logicznym
//---------------------------------------------------------------------------
class IAIELinguisticFuzzyAttr
{
 public:
		virtual const char* __stdcall GetName(void) = 0;
		virtual TAIEColumnType __stdcall GetType(void) = 0;
		virtual TAIEColumnKind __stdcall GetKind(void) = 0;
		virtual const char* __stdcall GetTypeName(void) = 0;
		virtual const char* __stdcall GetKindName(void) = 0;
		virtual bool __stdcall GetIsNominalType(void) = 0;
		virtual double __stdcall GetMinValue(void) = 0;
		virtual double __stdcall GetMaxValue(void) = 0;
		virtual bool __stdcall GetIsActive(void) = 0;
		virtual int __stdcall GetAssignedFuzzyRuleCount(int AFuzzyRulebaseIndex) = 0;

 #ifdef __BORLANDC__
		//w³asnoœci niezale¿ne od pod³¹czonego zestawu zbiorów rozmytych
		__property const char* Name = {read=GetName};
		__property TAIEColumnType Type = {read=GetType};
		__property TAIEColumnKind Kind = {read=GetKind};
		__property const char* TypeName = {read=GetTypeName};
		__property const char* KindName = {read=GetKindName};
		__property bool IsNominalType = {read=GetIsNominalType};
		__property double MinValue = {read=GetMinValue};
		__property double MaxValue = {read=GetMaxValue};

		__property bool IsActive = {read=GetIsActive};
		__property int AssignedFuzzyRuleCount[int AFuzzyRulebaseIndex] = {read=GetAssignedFuzzyRuleCount};
 #endif
};
//---------------------------------------------------------------------------
// IAIELinguisticFuzzyStructure
//---------------------------------------------------------------------------
// Interfejs systemu rozmytego:
// Funkcje:
// SaveToFile - zapis systemu do pliku
// LoadFromFile - odczyt systemu z pliku
// Initialize - inicjowanie systemu na podstawie danych zawartych w interfejsie wejœciowym
// Clear - usuwanie systemu
// W³asnoœci:
// Options - domyslne parametry systemu, wykorzystywane podczas inicjowania systemu
// FuzzyCollectionCount - liczba zestawów zbiorów rozmytych (równa sumie liczby wejœæ i wyjœæ systemu)
// FuzzyCollection[int Index] - wskaŸnik do pierwotnego zestawu zbiorów rozmytych o numerze Index
// FuzzyCollectionByName[UnicodeString Name] - wskaŸnik do zestawu zbiorów rozmytych o nazwie Name
// CurrentFuzzyCollection[int Index] - wskaŸnik do bie¿¹cego zestawu zbiorów rozmytych o numerze Index
// EnabledFuzzyCollectionCount - liczba zestawów zbiorów rozmytych pod³¹czonych do atrybutów wejœciowych lub wyjœciowych
// EnabledFuzzyCollection[int Index] - wskaŸnik do pod³¹czonego zestawu zbiorów rozmytych
// InputAttrCount - liczba atrybutów wejœciowych (liczba wejœæ)
// InputAttr[int Index] - wskaŸnik do atrybutu wejœciowego o numerze Index
// OutputAttrCount - liczba atrybutów wyjœciowych (liczba wyjœæ)
// OutputAttr[int Index] - wskaŸnik do atrybutu wyjœciowego o numerze Index
// FuzzyRulebaseCount - liczba baz regu³ (równa liczbie wyjœæ systemu)
// FuzzyRulebase[int Index] - wska¿nik do bazy regu³ o numerze Index
// FuzzyRulebaseComplexity - wspó³czynnik z³o¿onoœci baz regu³
// AllFuzzySetCount - liczba wszystkich zbiorów rozmytych w systemie
//                    (rozmiar tablicy si³ regu³ (s-norm) przekazywanej jako parametr funkcji ComputeOutput)
// NomFuzzySetCount - liczba zbiorów rozmytych w atrybutach nominalnych
//					  (liczba kolumn rekordu MembershipRecord, przekazywanego jako parametr funkcji ComputeOutput)
// Tag - pomocnicze pole
//---------------------------------------------------------------------------
class IAIELinguisticFuzzyStructure
{
 public:
		virtual void __stdcall SaveToFile(IAIEBinaryFile* File) = 0;
		virtual void __stdcall LoadFromFile(IAIEBinaryFile* File) = 0;
		virtual void __stdcall Free(void) = 0;

		virtual IAIELinguisticFuzzyOptions* __stdcall GetOptions(void) = 0;
		virtual bool __stdcall GetIsInitialized(void) = 0;

		virtual int __stdcall GetTag(void) = 0;
		virtual void __stdcall SetTag(int ATag) = 0;

		virtual int __stdcall GetInputFuzzyAttrCount(void) = 0;
		virtual IAIELinguisticFuzzyAttr* __stdcall GetInputFuzzyAttr(int AIndex) = 0;
		virtual IAIELinguisticFuzzyCollection* __stdcall GetInputFuzzyCollection(int AIndex) = 0;

		virtual int __stdcall GetOutputFuzzyAttrCount(void) = 0;
		virtual IAIELinguisticFuzzyAttr* __stdcall GetOutputFuzzyAttr(int AIndex) = 0;
		virtual IAIELinguisticFuzzyCollection* __stdcall GetOutputFuzzyCollection(int AIndex) = 0;

		virtual int __stdcall GetFuzzyCollectionCount(void) = 0;
		virtual IAIELinguisticFuzzyCollection* __stdcall GetFuzzyCollection(int AIndex) = 0;
		virtual IAIELinguisticFuzzyCollection* __stdcall GetFuzzyCollectionByName(const char* AName) = 0;

		virtual IAIELinguisticFuzzyCollection* __stdcall GetCurrentFuzzyCollection(int AIndex) = 0;
		virtual void __stdcall SetCurrentFuzzyCollection(int AIndex, IAIELinguisticFuzzyCollection* AFuzzyCollection) = 0;

		virtual int __stdcall GetEnabledFuzzyCollectionCount(void) = 0;
		virtual IAIELinguisticFuzzyCollection* __stdcall GetEnabledFuzzyCollection(int AIndex) = 0;

		virtual int __stdcall GetFuzzyRulebaseCount(void) = 0;
		virtual IAIELinguisticFuzzyRulebase* __stdcall GetFuzzyRulebase(int AIndex) = 0;

		virtual int __stdcall GetSNormTableCount(void) = 0;
		virtual int __stdcall GetErrorTableCount(void) = 0;
		virtual double __stdcall GetFuzzySystemComplexity(void) = 0;
		virtual int __stdcall GetEnabledFuzzyRuleCount(void) = 0;
		virtual int __stdcall GetEnabledFuzzySetCount(void) = 0;
		virtual int __stdcall GetEnabledInpFuzzyAttrCount(void) = 0;
		virtual int __stdcall GetEnabledOutFuzzyAttrCount(void) = 0;

		virtual double __stdcall GetNormEnabledFuzzyAttrCount(void) = 0;
		virtual double __stdcall GetNormEnabledFuzzySetCount(void) = 0;

		virtual void __stdcall Initialize(IAIEDataSet* DataSet) = 0;
		virtual void __stdcall Clear(void) = 0;

		virtual void __stdcall BeginCalcRuleStrengths(void) = 0;
		virtual void __stdcall EndCalcRuleStrengths(IAIEDataSet* InputDataSet) = 0;

		virtual IAIELinguisticFuzzyStructure* __stdcall Clone(void) = 0;
		virtual void __stdcall Assign(IAIELinguisticFuzzyStructure* Source) = 0;

		virtual void __stdcall Execute(IAIEDataSet* InputDataSet, IAIEDataSet* OutputDataSet, bool CalclulateRuleStrengths = false) = 0;
		virtual void __stdcall ExecuteRecord(IAIEDataSet* InputDataSet, IAIEDataSet* OutputDataSet, int RecordIndex, bool CalclulateRuleStrengths = false) = 0;

 #ifdef __BORLANDC__
		__property IAIELinguisticFuzzyOptions* Options = {read=GetOptions};
		__property bool IsInitialized = {read=GetIsInitialized};
		__property int Tag = {read=GetTag, write=SetTag};

		__property int InputFuzzyAttrCount = {read=GetInputFuzzyAttrCount};
		__property IAIELinguisticFuzzyAttr* InputFuzzyAttr[int Index] = {read=GetInputFuzzyAttr};
		__property IAIELinguisticFuzzyCollection* InputFuzzyCollection[int Index] = {read=GetInputFuzzyCollection};

		__property int OutputFuzzyAttrCount = {read=GetOutputFuzzyAttrCount};
		__property IAIELinguisticFuzzyAttr* OutputFuzzyAttr[int Index] = {read=GetOutputFuzzyAttr};
		__property IAIELinguisticFuzzyCollection* OutputFuzzyCollection[int Index] = {read=GetOutputFuzzyCollection};

		__property int FuzzyCollectionCount = {read=GetFuzzyCollectionCount};
		__property IAIELinguisticFuzzyCollection* FuzzyCollection[int Index] = {read=GetFuzzyCollection};
		__property IAIELinguisticFuzzyCollection* FuzzyCollectionByName[const char* Name] = {read=GetFuzzyCollectionByName};

		__property IAIELinguisticFuzzyCollection* CurrentFuzzyCollection[int Index] = {read=GetCurrentFuzzyCollection, write=SetCurrentFuzzyCollection};

		__property int EnabledFuzzyCollectionCount = {read=GetEnabledFuzzyCollectionCount};
		__property IAIELinguisticFuzzyCollection* EnabledFuzzyCollection[int Index] = {read=GetEnabledFuzzyCollection};

		__property int FuzzyRulebaseCount = {read=GetFuzzyRulebaseCount};
		__property IAIELinguisticFuzzyRulebase* FuzzyRulebase[int Index] = {read=GetFuzzyRulebase};

		__property int SNormTableCount = {read=GetSNormTableCount};
		__property int ErrorTableCount = {read=GetErrorTableCount};
		__property double FuzzySystemComplexity = {read=GetFuzzySystemComplexity};

		__property int EnabledFuzzyRuleCount = {read=GetEnabledFuzzyRuleCount};
		__property int EnabledFuzzySetCount = {read=GetEnabledFuzzySetCount};
		__property int EnabledInpFuzzyAttrCount = {read=GetEnabledInpFuzzyAttrCount};
		__property int EnabledOutFuzzyAttrCount = {read=GetEnabledOutFuzzyAttrCount};
 #endif
};
//---------------------------------------------------------------------------
#endif
