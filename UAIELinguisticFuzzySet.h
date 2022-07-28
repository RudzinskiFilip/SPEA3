//---------------------------------------------------------------------------
#ifndef UAIELinguisticFuzzySetH
#define UAIELinguisticFuzzySetH
//---------------------------------------------------------------------------
#include "aievcl.h"
#include "aievcltools.h"

#include "IAIELinguisticFuzzySet.h"
//---------------------------------------------------------------------------
class TAIELinguisticFuzzySet;
class TAIELinguisticFuzzyCollection;
class TAIELinguisticFuzzyStructure;
//---------------------------------------------------------------------------
enum TAIELinguisticFuzzySetKind {fskLeft, fskNormal, fskRight};
//---------------------------------------------------------------------------
// TAIELinguisticFuzzySet
//---------------------------------------------------------------------------
class TAIELinguisticFuzzySet : public IAIELinguisticFuzzySet
{
 friend class TAIELinguisticFuzzyRule;
 friend class TAIELinguisticFuzzyRulebase;
 friend class TAIELinguisticFuzzyStructure;

 typedef double (__stdcall TAIELinguisticFuzzySet::*TAIELinguisticFuzzySetFunction)(double Value);

 private:
		TAIELinguisticFuzzyCollection* FOwner;
		TNumaVector<TAIELinguisticFuzzySet>& FFuzzySet;
		int FIndex;

		int FNumaNodeIndex;
		TNumaVector<int> FAssignedFuzzyAntecedentCount; //liczba przes³anek wykorzystuj¹cych zbiór rozmyty w poszczególnych bazach regu³

		// - parametry zmienne po utworzeniu struktury
		//   (np. zmienne podczas uczenia)
		char*  FName;
		bool   FCoreEnabled;
		bool   FContrastEnabled;
		double FLCenter;
		double FRCenter;
		double FLAlpha;
		double FRAlpha;
		double FLContrast;
		double FRContrast;

		//zmienne pomocnicze (optymalizacja szybkoœci obliczeñ)
		double FMaxValue;
		double FMinValue;
		double F1L, F1R;
		double F3L, F3R;
		double F5L, F5R;
		double FCL, FCR;

		//wskaŸnik do funkcji przynale¿noœci
		TAIELinguisticFuzzySetKind FKind;
		TAIEMembershipFunction FMembershipFunction;
		TAIELinguisticFuzzySetFunction FFunction;

        //funkcja singletonowa (dla atrybutów nominalnych)
		double __stdcall SingletonFunction(double Value);

		//funkcja gaussowska (L, R - skrajne, N - œrodkowa)
		double __stdcall LMembershipFunction1(double Value);
		double __stdcall NMembershipFunction1(double Value);
		double __stdcall RMembershipFunction1(double Value);

		//funkcja gausowska z rdzeniem (L, R - skrajne, N - œrodkowa)
		double __stdcall LMembershipFunction2(double Value);
		double __stdcall NMembershipFunction2(double Value);
		double __stdcall RMembershipFunction2(double Value);

		//funkcja trójk¹tna (L, R - skrajne, N - œrodkowa)
		double __stdcall LMembershipFunction3(double Value);
		double __stdcall NMembershipFunction3(double Value);
		double __stdcall RMembershipFunction3(double Value);

		//funkcja trójk¹tna z rdzeniem (trapezoidalna) (L, R - skrajne, N - œrodkowa)
		double __stdcall LMembershipFunction4(double Value);
		double __stdcall NMembershipFunction4(double Value);
		double __stdcall RMembershipFunction4(double Value);

		//funkcja cosinusoidalna (L, R - skrajne, N - œrodkowa)
		double __stdcall LMembershipFunction5(double Value);
		double __stdcall NMembershipFunction5(double Value);
		double __stdcall RMembershipFunction5(double Value);

		//funkcja cosinusoidalna z rdzeniem (L, R - skrajne, N - œrodkowa)
		double __stdcall LMembershipFunction6(double Value);
		double __stdcall NMembershipFunction6(double Value);
		double __stdcall RMembershipFunction6(double Value);

		//funkcja prostok¹tna (L, R - skrajne, N - œrodkowa)
		double __stdcall LMembershipFunction7(double Value);
		double __stdcall NMembershipFunction7(double Value);
		double __stdcall RMembershipFunction7(double Value);

		//funkcja prostok¹tna z rdzeniem (L, R - skrajne, N - œrodkowa)
		double __stdcall LMembershipFunction8(double Value);
		double __stdcall NMembershipFunction8(double Value);
		double __stdcall RMembershipFunction8(double Value);

 public:
		__stdcall TAIELinguisticFuzzySet(TAIELinguisticFuzzyCollection* AOwner,
			int ACount, int FIndex, int ANumaNodeIndex, int AFuzzyRulebaseCount,
			TAIELinguisticFuzzySetKind AKind, double AMaxValue, double AMinValue,
			TAIEMembershipFunction AMembershipFunction, bool ACoreEnabled, bool AContrastEnabled);
		__stdcall ~TAIELinguisticFuzzySet(void);
		void __stdcall Assign(TAIELinguisticFuzzySet* Source);
		void __stdcall SaveToFile(IAIEBinaryFile* File);
		void __stdcall LoadFromFile(IAIEBinaryFile* File);
		void __stdcall Prepare(void);

		double __stdcall ExecuteFuzzyValue(double CrispValue);

		void __stdcall SetKind(TAIELinguisticFuzzySetKind AKind);
		const char* __stdcall GetName(void);
		void __stdcall SetName(const char* AName);
		bool __stdcall GetCoreEnabled(void);
		void __stdcall SetCoreEnabled(bool ACoreEnabled);
		bool __stdcall GetContrastEnabled(void);
		void __stdcall SetContrastEnabled(bool AContrastEnabled);
		double __stdcall GetLCenter(void);
		void __stdcall SetLCenter(double AValue);
		double __stdcall GetRCenter(void);
		void __stdcall SetRCenter(double AValue);
		double __stdcall GetLContrast(void);
		void __stdcall SetLContrast(double AValue);
		double __stdcall GetRContrast(void);
		void __stdcall SetRContrast(double AValue);
		double __stdcall GetLAlpha(void);
		void __stdcall SetLAlpha(double AValue);
		double __stdcall GetRAlpha(void);
		void __stdcall SetRAlpha(double AValue);
		double __stdcall GetArea(void);
		double __stdcall GetLArea(void);
		double __stdcall GetRArea(void);
		double __stdcall GetCenter(void);
		double __stdcall GetCore(void);
		bool __stdcall GetIsActive(void);
		int __stdcall GetAssignedFuzzyAntecedentCount(int AFuzzyRulebaseIndex);

		TAIEMembershipFunction __stdcall GetMembershipFunction(void);
		void __stdcall SetMembershipFunction(TAIEMembershipFunction AMembershipFunction);
};
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyCollection
//---------------------------------------------------------------------------
class TAIELinguisticFuzzyCollection : public IAIELinguisticFuzzyCollection
{
 friend class TAIELinguisticFuzzySet;
 friend class TAIELinguisticFuzzyRule;
 friend class TAIELinguisticFuzzyRulebase;
 friend class TAIELinguisticFuzzyStructure;

 typedef double (__stdcall TAIELinguisticFuzzyCollection::*TAIEExecuteFuzzyValue)(IAIEDataSetRecord* Record, int AttrIndex, int FuzzySetIndex);
 typedef double (__stdcall TAIELinguisticFuzzyCollection::*TAIEExecuteDataFuzzyValue)(double* Record, int AttrIndex, int FuzzySetIndex);
 private:
		// - parametry sta³e po utworzeniu struktury
		//   (zale¿ne jedynie od tworz¹cego zbioru danych)
		TAIELinguisticFuzzyStructure* FOwner;
		TAIEMembershipFunction FDefaultMembershipFunction;
		bool   FDefaultCoreEnabled;
		bool   FDefaultContrastEnabled;
		int    FNumaNodeIndex;
		int    FIndex;
		char*  FName;
		TAIEColumnKind FKind;
		TAIEColumnType FType;
		double FMinValue;
		double FMaxValue;
		bool   FSFP;	//strong fuzzy partitioning

		TNumaVector<TAIELinguisticFuzzySet> FFuzzySet;

		//funkcje przeliczaj¹ce wartoœæ rozmyt¹
		TAIEExecuteFuzzyValue FExecuteFuzzyValue;
		double __stdcall ExecuteFuzzyValue1(IAIEDataSetRecord* Record, int AttrIndex, int FuzzySetIndex);
		double __stdcall ExecuteFuzzyValue2(IAIEDataSetRecord* Record, int AttrIndex, int FuzzySetIndex);
		double __stdcall ExecuteFuzzyValue3(IAIEDataSetRecord* Record, int AttrIndex, int FuzzySetIndex);
		double __stdcall ExecuteFuzzyValue4(IAIEDataSetRecord* Record, int AttrIndex, int FuzzySetIndex);

		TAIEExecuteDataFuzzyValue FExecuteDataFuzzyValue;
		double __stdcall ExecuteDataFuzzyValue1(double* Record, int AttrIndex, int FuzzySetIndex);
		double __stdcall ExecuteDataFuzzyValue2(double* Record, int AttrIndex, int FuzzySetIndex);
		double __stdcall ExecuteDataFuzzyValue3(double* Record, int AttrIndex, int FuzzySetIndex);
		double __stdcall ExecuteDataFuzzyValue4(double* Record, int AttrIndex, int FuzzySetIndex);

 public:
		__stdcall TAIELinguisticFuzzyCollection(TAIELinguisticFuzzyStructure* AOwner, int ANumaNodeIndex, int AIndex, const char* AName,
												TAIEColumnKind AKind, TAIEColumnType AType, double AMinValue, double AMaxValue,
												TAIEMembershipFunction ADefaultMembershipFunction, bool ADefaultCoreEnabled,
												bool ADefaultContrastEnabled, bool ADefaultSFP);
		__stdcall ~TAIELinguisticFuzzyCollection(void);
		void __stdcall Assign(TAIELinguisticFuzzyCollection* Source);
		void __stdcall SaveToFile(IAIEBinaryFile* File);
		void __stdcall LoadFromFile(IAIEBinaryFile* File);

		double __stdcall ExecuteFuzzyValue(IAIEDataSetRecord* Record, int AttrIndex, int FuzzySetIndex);
		double __stdcall ExecuteDataFuzzyValue(double* Record, int AttrIndex, int FuzzySetIndex);

        int __stdcall GetIndex(void);
		const char* __stdcall GetName(void);
		TAIEColumnType __stdcall GetType(void);
		TAIEColumnKind __stdcall GetKind(void);
		double __stdcall GetMinValue(void);
		double __stdcall GetMaxValue(void);
		const char* __stdcall GetTypeName(void);
		const char* __stdcall GetKindName(void);
		bool __stdcall GetIsNominalType(void);

		int __stdcall GetFuzzySetCount(void);
		void __stdcall SetFuzzySetCount(int ACount);
		IAIELinguisticFuzzySet* __stdcall GetFuzzySet(int AFuzzySetIndex);
		int __stdcall GetFuzzySetIndexByName(const char* FuzzySetName);

		bool __stdcall GetIsSFP(void);
		void __stdcall SetIsSFP(bool ASFP);
};
//---------------------------------------------------------------------------
#endif
