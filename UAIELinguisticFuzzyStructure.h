//---------------------------------------------------------------------------
#ifndef UAIELinguisticFuzzyStructureH
#define UAIELinguisticFuzzyStructureH
//---------------------------------------------------------------------------
#include "aievcl.h"
#include "aievcltools.h"

#include "IAIELinguisticFuzzyStructure.h"
//---------------------------------------------------------------------------
class TAIELinguisticFuzzyOptions;
class TAIELinguisticFuzzySet;
class TAIELinguisticFuzzyCollection;
class TAIELinguisticFuzzyAttr;
class TAIELinguisticFuzzyRule;
class TAIELinguisticFuzzyRulebase;
class TAIELinguisticFuzzyStructure;
class TAIENumaNumeralDataSet;
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyOptions
//---------------------------------------------------------------------------
class TAIELinguisticFuzzyOptions : public IAIELinguisticFuzzyOptions
{
 friend class TAIELinguisticFuzzyStructure;
 private:
		TAIEMembershipFunction FDefaultMembershipFunction;
		bool FDefaultCoreEnabled;
		bool FDefaultContrastEnabled;
		bool FDefaultSFP;
		int FIntegralProbeCount;
		int FDefaultFuzzySetCount;
		TAIEFuzzyTNormType FDefaultTNormType;
		TAIEFuzzySNormType FDefaultSNormType;
		TAIEAggregationOperator FDefaultAggregationOperator;
		TAIEImplicationOperator FDefaultImplicationOperator;
		TAIEInferenceMode FDefaultInferenceMode;
		TAIEDefuzzificationMethod FDefaultDefuzzificationMethod;

 public:
		__stdcall TAIELinguisticFuzzyOptions(void);
		__stdcall ~TAIELinguisticFuzzyOptions(void);
		void __stdcall Assign(TAIELinguisticFuzzyOptions* Source);
		void __stdcall SaveToFile(IAIEBinaryFile* File);
		void __stdcall LoadFromFile(IAIEBinaryFile* File);

		TAIEMembershipFunction __stdcall GetDefaultMembershipFunction(void);
		void __stdcall SetDefaultMembershipFunction(TAIEMembershipFunction ADefaultMembershipFunction);
		bool __stdcall GetDefaultCoreEnabled(void);
		void __stdcall SetDefaultCoreEnabled(bool ADefaultCoreEnabled);
		bool __stdcall GetDefaultContrastEnabled(void);
		void __stdcall SetDefaultContrastEnabled(bool ADefaultContrastEnabled);
		bool __stdcall GetDefaultSFP(void);
		void __stdcall SetDefaultSFP(bool ADefaultSFP);
		int __stdcall GetIntegralProbeCount(void);
		void __stdcall SetIntegralProbeCount(int AIntegralProbeCount);
		int __stdcall GetDefaultFuzzySetCount(void);
		void __stdcall SetDefaultFuzzySetCount(int ADefaultFuzzySetCount);
		TAIEFuzzyTNormType __stdcall GetDefaultTNormType(void);
		void __stdcall SetDefaultTNormType(TAIEFuzzyTNormType ATNormType);
		TAIEFuzzySNormType __stdcall GetDefaultSNormType(void);
		void __stdcall SetDefaultSNormType(TAIEFuzzySNormType ASNormType);
		TAIEInferenceMode __stdcall GetDefaultInferenceMode(void);
		void __stdcall SetDefaultInferenceMode(TAIEInferenceMode ADefaultInferenceMode);
		TAIEImplicationOperator __stdcall GetDefaultImplicationOperator(void);
		void __stdcall SetDefaultImplicationOperator(TAIEImplicationOperator ADefaultImplicationOperator);
		TAIEAggregationOperator __stdcall GetDefaultAggregationOperator(void);
		void __stdcall SetDefaultAggregationOperator(TAIEAggregationOperator ADefaultAggregationOperator);
		TAIEDefuzzificationMethod __stdcall GetDefaultDefuzzificationMethod(void);
		void __stdcall SetDefaultDefuzzificationMethod(TAIEDefuzzificationMethod ADefuzzificationMethod);
};
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyAttr
//---------------------------------------------------------------------------
class TAIELinguisticFuzzyAttr : public IAIELinguisticFuzzyAttr
{
 friend class TAIELinguisticFuzzyStructure;
 friend class TAIELinguisticFuzzyRulebase;
 friend class TAIELinguisticFuzzyRule;
 private:
		int FNumaNodeIndex;
		TNumaVector<int> FAssignedFuzzyRuleCount; //liczba regu³ rozmytych wykorzystuj¹cych atrybut w poszczególnych bazach regu³

		char*  FName;
		TAIEColumnKind FKind;
		TAIEColumnType FType;
		double FMinValue;
		double FMaxValue;

 public:
		__stdcall TAIELinguisticFuzzyAttr(int ANumaNodeIndex, int AFuzzyRulebaseCount,
										  const char* AName, TAIEColumnKind AKind, TAIEColumnType AType,
										  double AMinValue, double AMaxValue);
		__stdcall ~TAIELinguisticFuzzyAttr(void);
		void __stdcall Assign(TAIELinguisticFuzzyAttr* Source);
		void __stdcall SaveToFile(IAIEBinaryFile* File);
		void __stdcall LoadFromFile(IAIEBinaryFile* File);

		const char* __stdcall GetName(void);
		TAIEColumnType __stdcall GetType(void);
		TAIEColumnKind __stdcall GetKind(void);
		double __stdcall GetMinValue(void);
		double __stdcall GetMaxValue(void);
		const char* __stdcall GetTypeName(void);
		const char* __stdcall GetKindName(void);
		bool __stdcall GetIsNominalType(void);
		bool __stdcall GetIsActive(void);
		int __stdcall GetAssignedFuzzyRuleCount(int AFuzzyRulebaseIndex);
};
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyStructure
//---------------------------------------------------------------------------
class TAIELinguisticFuzzyStructure : public IAIELinguisticFuzzyStructure
{
 private:
        TAIESystem* FSystem;
		TAIELinguisticFuzzyOptions* FOptions;
		int FNumaNodeIndex;
		bool FIsInitialized;
		int FTag;

	 	IAIENumaThreads* FThreads;

		// maksymalna i minimalna liczba regu³ w bazach regu³
		//(ograniczenie górne = liczbie rekordów podczas inicjalizacji struktury)
		//(ograniczenie dolne = 1)
		int FMaxFuzzyRuleCount;
		int FMinFuzzyRuleCount;

        //rozmiar tablicy s-norm (liczba wyszystkich zbiorów rozmytych w atrybutach wyjœciowych)
		int FSNormTableCount;
        //rozmiar tablicy b³êdów (liczba wyjœæ ci¹g³ych + liczba zbiorów rozmytych dla wejœæ dyskretnych)
		int FErrorTableCount;

		TNumaVector<TAIELinguisticFuzzyAttr> FInputFuzzyAttr;                //atrybuty wejœciowe
		TNumaVector<TAIELinguisticFuzzyAttr> FOutputFuzzyAttr;               //atrybuty wyjœciowe
		TNumaVector<TAIELinguisticFuzzyCollection>  FFuzzyCollection;        //pierwotne zestawy zbiorów rozmytych dla wszystkich atrybutów (wejœciowych i wyjœciowych razem)
		TNumaVector<TAIELinguisticFuzzyCollection*> FCurrentFuzzyCollection; //bie¿¹ce zestawy zbiorów rozmytych dla atrybutów (mo¿liwe ustawienie jednego zestawu z tablicy FFuzzyCollection do kilku komórek tablicy FCurrentFuzzyCollection)
		TNumaVector<TAIELinguisticFuzzyCollection*> FEnabledFuzzyCollection; //wykorzystane zestawy zbiorów rozmytych (rozmiar tablicy równy liczbie u¿ytych zestawów)

		TNumaVector<TAIELinguisticFuzzyRulebase> FFuzzyRulebase;

		void __stdcall SetFuzzyAttrCount(int AInputCount, int AOutputCount);
		void __stdcall SetFuzzyRulebaseCount(int ACount);

 public:
		__stdcall TAIELinguisticFuzzyStructure(TAIESystem* ASystem, int ANumaNodeIndex, bool OwnThread = true);
		__stdcall ~TAIELinguisticFuzzyStructure(void);
		void __stdcall Assign(IAIELinguisticFuzzyStructure* Source);
		IAIELinguisticFuzzyStructure* __stdcall Clone(void);

		void __stdcall SaveToFile(IAIEBinaryFile* File);
		void __stdcall LoadFromFile(IAIEBinaryFile* File);
		void __stdcall Free(void);
		void __stdcall PrepareFuzzyRulebase(void);

		IAIELinguisticFuzzyOptions* __stdcall GetOptions(void);
		bool __stdcall GetIsInitialized(void);

		int  __stdcall GetMaxFuzzyRuleCount(void);
		int  __stdcall GetMinFuzzyRuleCount(void);

		int  __stdcall GetSNormTableCount(void);
		int  __stdcall GetErrorTableCount(void);
		double __stdcall GetFuzzySystemComplexity(void);
		int __stdcall GetEnabledFuzzyRuleCount(void);
		int __stdcall GetEnabledFuzzySetCount(void);
		int __stdcall GetEnabledInpFuzzyAttrCount(void);
		int __stdcall GetEnabledOutFuzzyAttrCount(void);

		double __stdcall GetNormEnabledFuzzyAttrCount(void);
		double __stdcall GetNormEnabledFuzzySetCount(void);

		int __stdcall GetInputFuzzyAttrCount(void);
		IAIELinguisticFuzzyAttr* __stdcall GetInputFuzzyAttr(int AIndex);
		IAIELinguisticFuzzyCollection* __stdcall GetInputFuzzyCollection(int AIndex);

		int __stdcall GetOutputFuzzyAttrCount(void);
		IAIELinguisticFuzzyAttr* __stdcall GetOutputFuzzyAttr(int AIndex);
		IAIELinguisticFuzzyCollection* __stdcall GetOutputFuzzyCollection(int AIndex);

		int __stdcall GetFuzzyCollectionCount(void);
		IAIELinguisticFuzzyCollection* __stdcall GetFuzzyCollection(int AIndex);
		IAIELinguisticFuzzyCollection* __stdcall GetFuzzyCollectionByName(const char* AName);

		IAIELinguisticFuzzyCollection* __stdcall GetCurrentFuzzyCollection(int AIndex);
		void __stdcall SetCurrentFuzzyCollection(int AIndex, IAIELinguisticFuzzyCollection* AFuzzyCollection);

		int __stdcall GetEnabledFuzzyCollectionCount(void);
		IAIELinguisticFuzzyCollection* __stdcall GetEnabledFuzzyCollection(int AIndex);

		int __stdcall GetFuzzyRulebaseCount(void);
		IAIELinguisticFuzzyRulebase* __stdcall GetFuzzyRulebase(int AIndex);

		int __stdcall GetTag(void);
		void __stdcall SetTag(int ATag);

		void __stdcall Initialize(IAIEDataSet* DataSet);
		void __stdcall Clear(void);

		void __stdcall BeginCalcRuleStrengths(void);
		void __stdcall EndCalcRuleStrengths(IAIEDataSet* InputDataSet);

		void __stdcall Execute(IAIEDataSet* InputDataSet, IAIEDataSet* OutputDataSet, bool CalclulateRuleStrengths = false);

		void __stdcall ExecuteRecord(IAIEDataSet* InputDataSet, IAIEDataSet* OutputDataSet, int RecordIndex, bool CalclulateRuleStrengths = false);
		void __stdcall ExecuteRecord(TAIENumaNumeralDataSet* InputDataSet, TAIENumaNumeralDataSet* OutputDataSet, int RecordIndex, bool CalclulateRuleStrengths = false);

		void __stdcall ExecuteRecord(IAIEDataSet* InputDataSet, IAIEDataSet* OutputDataSet, double* SNormTable, int RecordIndex, bool CalclulateRuleStrengths = false);
		void __stdcall ExecuteRecord(TAIENumaNumeralDataSet* InputDataSet, TAIENumaNumeralDataSet* OutputDataSet, double* SNormTable, int RecordIndex, bool CalclulateRuleStrengths = false);

		double __stdcall ExecuteError(IAIEDataSet* InputDataSet, IAIEDataSet* OutputDataSet, double* YTols, double* YErrors, int* YOutputsInTol, int& AVectorsInTol, double& ANormalizedError);

		double __stdcall ExecuteRecordError(IAIEDataSet* InputDataSet, IAIEDataSet* OutputDataSet, double* SNormTable, int RecordIndex, double* YTols, double* YErrors, int* YOutputsInTol, int& AVectorsInTol, double& ANormalizedError);
		double __stdcall ExecuteRecordError(TAIENumaNumeralDataSet* InputDataSet, TAIENumaNumeralDataSet* OutputDataSet, double* SNormTable, int RecordIndex, double* YTols, double* YErrors, int* YOutputsInTol, int& AVectorsInTol, double& ANormalizedError);
};
//---------------------------------------------------------------------------
#endif
