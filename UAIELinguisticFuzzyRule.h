//---------------------------------------------------------------------------
#ifndef UAIELinguisticFuzzyRuleH
#define UAIELinguisticFuzzyRuleH
//---------------------------------------------------------------------------
#include "aievcl.h"
#include "aievcltools.h"

#include "IAIELinguisticFuzzyRule.h"
//---------------------------------------------------------------------------
class TAIELinguisticFuzzyCollection;
class TAIELinguisticFuzzyStructure;
class TAIELinguisticFuzzyRulebase;
class TAIELinguisticFuzzyAttr;
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyRule
//---------------------------------------------------------------------------
class TAIELinguisticFuzzyRule : public IAIELinguisticFuzzyRule
{
 typedef double (__stdcall TAIELinguisticFuzzyRule::*TAIEExecuteTNorm)(IAIEDataSetRecord* Record);
 typedef double (__stdcall TAIELinguisticFuzzyRule::*TAIEExecuteDataTNorm)(double* Record);
 private:
        TAIELinguisticFuzzyRulebase* FOwner;
		TNumaVector<TAIELinguisticFuzzyCollection*>& FCurrentFuzzyCollection;
		TNumaVector<TAIELinguisticFuzzyAttr>& FInputFuzzyAttr;
		TAIELinguisticFuzzyAttr& FOutputFuzzyAttr;
		int& FEnabledFuzzyRuleCount;
		int  FInpFuzzyAttrCount;
		int  FOutFuzzyAttrIndex;
		int  FNumaNodeIndex;
		AnsiString FRuleAsciiText;
		AnsiString FRulePlainText;

		TNumaVector<int>  FInpFuzzySetIndex;
		TNumaVector<bool> FInpFuzzySetNegated;
		int  FOutFuzzySetIndex;

		TAIEFuzzyTNormType FTNormType;
		bool FEnabled;
		bool FLocked;
		int  FLength;

		double FStrength;
		int FFireCount;

		TAIEExecuteTNorm FExecuteTNorm;
		double __stdcall ExecuteTNorm1(IAIEDataSetRecord* Record);
		double __stdcall ExecuteTNorm2(IAIEDataSetRecord* Record);
		double __stdcall ExecuteTNorm3(IAIEDataSetRecord* Record);
		double __stdcall ExecuteTNorm4(IAIEDataSetRecord* Record);
		double __stdcall ExecuteTNorm5(IAIEDataSetRecord* Record);
		double __stdcall ExecuteTNorm6(IAIEDataSetRecord* Record);

		TAIEExecuteDataTNorm FExecuteDataTNorm;
		double __stdcall ExecuteDataTNorm1(double* Record);
		double __stdcall ExecuteDataTNorm2(double* Record);
		double __stdcall ExecuteDataTNorm3(double* Record);
		double __stdcall ExecuteDataTNorm4(double* Record);
		double __stdcall ExecuteDataTNorm5(double* Record);
		double __stdcall ExecuteDataTNorm6(double* Record);

 public:
		__stdcall TAIELinguisticFuzzyRule(TAIELinguisticFuzzyRulebase* AOwner, int ANumaNodeIndex,
										  int AInpFuzzyAttrCount, TNumaVector<TAIELinguisticFuzzyAttr>& AInputFuzzyAttr,
										  int AOutFuzzyAttrIndex, TAIELinguisticFuzzyAttr& AOutputFuzzyAttr,
										  TAIEFuzzyTNormType ATNormType, int& AEnabledFuzzyRuleCount,
										  TNumaVector<TAIELinguisticFuzzyCollection*>& ACurrentFuzzyCollection);
		__stdcall ~TAIELinguisticFuzzyRule(void);
		void __stdcall Assign(TAIELinguisticFuzzyRule* Source);
		void __stdcall SaveToFile(IAIEBinaryFile* File);
		void __stdcall LoadFromFile(IAIEBinaryFile* File);

		double __stdcall ExecuteTNorm(IAIEDataSetRecord* Record);
		double __stdcall ExecuteDataTNorm(double* Record);

		double __stdcall GetFuzzyRuleComplexity(void);

		IAIELinguisticFuzzyRulebase* __stdcall GetFuzzyRulebase(void);
		bool __stdcall GetEnabled(void);
		void __stdcall SetEnabled(bool AEnabled);
		bool __stdcall GetLocked(void);
		void __stdcall SetLocked(bool ALocked);
		bool __stdcall GetIsEmpty(void);
		int __stdcall GetLength(void);
		const char* __stdcall GetText(void);
		const char* __stdcall GetPlainText(void);
		double __stdcall GetStrength(void);
		void __stdcall SetStrength(double AStrength);
		int __stdcall GetFireCount(void);
		void __stdcall SetFireCount(int AFireCount);

		int __stdcall GetInpFuzzyAttrCount(void);
		int __stdcall GetInpFuzzySetCount(int AAttrIndex);
		int __stdcall GetInpFuzzySetIndex(int AAttrIndex);
		void __stdcall SetInpFuzzySetIndex(int AAttrIndex, int AFuzzySetIndex);
		bool __stdcall GetInpFuzzySetNegated(int AAttrIndex);
		void __stdcall SetInpFuzzySetNegated(int AAttrIndex, bool AInpFuzzySetNegated);

		int __stdcall GetOutFuzzySetIndex(void);
		void __stdcall SetOutFuzzySetIndex(int AAFuzzySetIndex);
		int __stdcall GetOutFuzzySetCount(void);

		void __stdcall SetTNormType(TAIEFuzzyTNormType ATNormType);
};
//---------------------------------------------------------------------------
// TAIELinguisticFuzzyRulebase
//---------------------------------------------------------------------------
class TAIELinguisticFuzzyRulebase : public IAIELinguisticFuzzyRulebase
{
 typedef int    (__stdcall TAIELinguisticFuzzyRulebase::*TAIEExecuteSNorm)(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex);
 typedef int    (__stdcall TAIELinguisticFuzzyRulebase::*TAIEExecuteDataSNorm)(double* Record, double* SNormTable, int& RuleIndex);
 typedef double (__stdcall TAIELinguisticFuzzyRulebase::*TAIEExecuteImplication)(double X, double Y);
 typedef double (__stdcall TAIELinguisticFuzzyRulebase::*TAIEExecuteMshpDegree)(double CY, double* SNormTable, int FuzzySetIndex);
 typedef double (__stdcall TAIELinguisticFuzzyRulebase::*TAIEExecuteCrispValue)(double* SNormTable, int FuzzySetIndex);
 typedef double (__stdcall TAIELinguisticFuzzyRulebase::*TAIEExecuteOutput)(double* SNormTable);
 private:
		TAIELinguisticFuzzyStructure* FOwner;
		TAIESystem* FSystem;
		int  FNumaNodeIndex;

		TNumaVector<TAIELinguisticFuzzyAttr>& FInputFuzzyAttr;
		TNumaVector<TAIELinguisticFuzzyAttr>& FOutputFuzzyAttr;
		TNumaVector<TAIELinguisticFuzzyCollection*>& FCurrentFuzzyCollection;
		int  FInpFuzzyAttrCount;
		int  FOutFuzzyAttrIndex;
		int  FOutFuzzySetCount;
		TAIEColumnType FType;

		int  FEnabledFuzzyRuleCount;
		int& FIntegralProbeCount;

		int  FMinFuzzyRuleCount;
		int  FMaxFuzzyRuleCount;
		int  FFuzzyRuleCount;
		TNumaVector<TAIELinguisticFuzzyRule*> FFuzzyRule;

		//zestaw zbiorów rozmytych dla wyjœciowego atrybutu
		TAIELinguisticFuzzyCollection* FOutFuzzyCollection;
		double FOutMinValue;
		double FOutMaxValue;

		//Parametry: t-normy, s-normy i metoda defuzyfikacji wyjœcia ci¹g³ego
		TAIEFuzzyTNormType FTNormType;
		TAIEFuzzySNormType FSNormType;
		TAIEImplicationOperator FImplicationOperator;
		TAIEAggregationOperator FAggregationOperator;
		TAIEInferenceMode FInferenceMode;
		TAIEDefuzzificationMethod FDefuzzificationMethod;

		//wskaŸniki do funkcji i prototypy funkcji ustawiane w zale¿noœci od parametrów
		TAIEExecuteSNorm FExecuteSNorm;
		int __stdcall ExecuteSNorm1(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex);
		int __stdcall ExecuteSNorm2(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex);
		int __stdcall ExecuteSNorm3(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex);
		int __stdcall ExecuteSNorm4(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex);
		int __stdcall ExecuteSNorm5(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex);
		int __stdcall ExecuteSNorm6(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex);

		TAIEExecuteDataSNorm FExecuteDataSNorm;
		int __stdcall ExecuteDataSNorm1(double* Record, double* SNormTable, int& RuleIndex);
		int __stdcall ExecuteDataSNorm2(double* Record, double* SNormTable, int& RuleIndex);
		int __stdcall ExecuteDataSNorm3(double* Record, double* SNormTable, int& RuleIndex);
		int __stdcall ExecuteDataSNorm4(double* Record, double* SNormTable, int& RuleIndex);
		int __stdcall ExecuteDataSNorm5(double* Record, double* SNormTable, int& RuleIndex);
		int __stdcall ExecuteDataSNorm6(double* Record, double* SNormTable, int& RuleIndex);

		//wskaŸniki fo funkcji wyznaczaj¹cych implikacje rozmyte
		TAIEExecuteImplication FExecuteImplication;
		double __stdcall ExecuteImplication1(double X, double Y);
		double __stdcall ExecuteImplication2(double X, double Y);
		double __stdcall ExecuteImplication3(double X, double Y);
		double __stdcall ExecuteImplication4(double X, double Y);
		double __stdcall ExecuteImplication5(double X, double Y);
		double __stdcall ExecuteImplication6(double X, double Y);
		double __stdcall ExecuteImplication7(double X, double Y);
		double __stdcall ExecuteImplication8(double X, double Y);
		double __stdcall ExecuteImplication9(double X, double Y);
		double __stdcall ExecuteImplication10(double X, double Y);
		double __stdcall ExecuteImplication11(double X, double Y);
		double __stdcall ExecuteImplication12(double X, double Y);

		TAIEExecuteMshpDegree  FExecuteMshpDegree;
		double __stdcall ExecuteMshpDegree0(double CY, double* SNormTable, int FuzzySetIndex = -1);
		double __stdcall ExecuteMshpDegree1(double CY, double* SNormTable, int FuzzySetIndex = -1);
		double __stdcall ExecuteMshpDegree2(double CY, double* SNormTable, int FuzzySetIndex = -1);
		double __stdcall ExecuteMshpDegree3(double CY, double* SNormTable, int FuzzySetIndex = -1);
		double __stdcall ExecuteMshpDegree4(double CY, double* SNormTable, int FuzzySetIndex = -1);
		double __stdcall ExecuteMshpDegree5(double CY, double* SNormTable, int FuzzySetIndex = -1);

		TAIEExecuteCrispValue FExecuteCrispValue;
		double __stdcall ExecuteCrispValueCOG(double* SNormTable, int FuzzySetIndex = -1);
		double __stdcall ExecuteCrispValueCOA(double* SNormTable, int FuzzySetIndex = -1);
		double __stdcall ExecuteCrispValueMCOA(double* SNormTable, int FuzzySetIndex = -1);
		double __stdcall ExecuteCrispValueFOM(double* SNormTable, int FuzzySetIndex = -1);
		double __stdcall ExecuteCrispValueMOM(double* SNormTable, int FuzzySetIndex = -1);
		double __stdcall ExecuteCrispValueLOM(double* SNormTable, int FuzzySetIndex = -1);

		double __stdcall ExecuteCrispValueRAW(double* SNormTable, int FuzzySetIndex = -1);

		TAIEExecuteOutput FExecuteOutput;
		double __stdcall ExecuteOutputFATI(double* SNormTable);
		double __stdcall ExecuteOutputFITA1(double* SNormTable);
		double __stdcall ExecuteOutputFITA2(double* SNormTable);
		double __stdcall ExecuteOutputFITA3(double* SNormTable);
		double __stdcall ExecuteOutputFITA4(double* SNormTable);
		double __stdcall ExecuteOutputFITA5(double* SNormTable);

		void __stdcall UpdateInferenceFunctionPointers(void);

 public:
		__stdcall TAIELinguisticFuzzyRulebase(TAIELinguisticFuzzyStructure* FOwner, TAIESystem* ASystem, int ANumaNodeIndex,
											  int AInpFuzzyAttrCount, int AOutFuzzyAttrIndex, TAIEColumnType FType,
											  TAIEFuzzyTNormType ATNormType,
											  TAIEFuzzySNormType ASNormType,
											  TAIEImplicationOperator AImplicationOperator,
											  TAIEAggregationOperator AAggregationOperator,
											  TAIEInferenceMode AInferenceMode,
											  TAIEDefuzzificationMethod ADefuzzificationMethod,
											  TNumaVector<TAIELinguisticFuzzyAttr>& AInputFuzzyAttr,
											  TNumaVector<TAIELinguisticFuzzyAttr>& AOutputFuzzyAttr,
											  TNumaVector<TAIELinguisticFuzzyCollection*>& ACurrentFuzzyCollection,
											  int& AIntegralProbeCount);
		__stdcall ~TAIELinguisticFuzzyRulebase(void);
		void __stdcall Assign(TAIELinguisticFuzzyRulebase* Source);
		void __stdcall SaveToFile(IAIEBinaryFile* File);
		void __stdcall LoadFromFile(IAIEBinaryFile* File);
		void __stdcall PrepareFuzzyRulebase(void);

		int __stdcall    ExecuteSNorm(IAIEDataSetRecord* Record, double* SNormTable, int& RuleIndex);
		int __stdcall    ExecuteDataSNorm(double* Record, double* SNormTable, int& RuleIndex);
		double __stdcall ExecuteImplication(double X, double Y);
		double __stdcall ExecuteCrispValue(double* SNormTable, int FuzzySetIndex = -1);
		double __stdcall ExecuteMshpDegree(double CY, double* SNormTable, int FuzzySetIndex = -1);
		double __stdcall ExecuteOutput(double* SNormTable);

		IAIELinguisticFuzzyRule* __stdcall AppendFuzzyRule(void);
		IAIELinguisticFuzzyRule* __stdcall InsertFuzzyRule(int Index);
		void __stdcall DeleteFuzzyRule(int Index);

		IAIELinguisticFuzzyStructure* __stdcall GetFuzzyStructure(void);
		int __stdcall GetFuzzyAttrIndex(void);
		int __stdcall GetOutFuzzyAttrIndex(void);
		int __stdcall GetInpFuzzyAttrCount(void);
		TAIEColumnType __stdcall GetType(void);
		bool __stdcall GetIsNominalType(void);

		TAIEFuzzyTNormType __stdcall GetTNormType(void);
		void __stdcall SetTNormType(TAIEFuzzyTNormType ATNormType);
		TAIEFuzzySNormType __stdcall GetSNormType(void);
		void __stdcall SetSNormType(TAIEFuzzySNormType ASNormType);
		TAIEImplicationOperator __stdcall GetImplicationOperator(void);
		void __stdcall SetImplicationOperator(TAIEImplicationOperator AImplicationOperator);
		TAIEAggregationOperator __stdcall GetAggregationOperator(void);
		void __stdcall SetAggregationOperator(TAIEAggregationOperator AAggregationOperator);
		TAIEInferenceMode __stdcall GetInferenceMode(void);
		void __stdcall SetInferenceMode(TAIEInferenceMode AInferenceMode);
		TAIEDefuzzificationMethod __stdcall GetDefuzzificationMethod(void);
		void __stdcall SetDefuzzificationMethod(TAIEDefuzzificationMethod ADefuzzificationMethod);

		int __stdcall GetMinFuzzyRuleCount(void);
		int __stdcall GetMaxFuzzyRuleCount(void);
		int __stdcall GetFuzzyRuleCount(void);
		void __stdcall SetFuzzyRuleCount(int ACount);
		IAIELinguisticFuzzyRule* __stdcall GetFuzzyRule(int AIndex);

		int __stdcall GetEnabledFuzzyRuleCount(void);
		int __stdcall GetEnabledFuzzySetCount(void);
		int __stdcall GetEnabledInpFuzzyAttrCount(void);
		int __stdcall GetEnabledOutFuzzyAttrCount(void);
		int __stdcall GetEnabledFuzzyRuleCountOfFuzzySet(int AFuzzyAttrIndex, int AFuzzySetIndex);

		double __stdcall GetFuzzyRulebaseComplexity(void);

		double __stdcall GetNormEnabledFuzzyRuleComplexity(void);
		double __stdcall GetNormEnabledFuzzyRuleCount(double MinFuzzyRuleCount, double MaxFuzzyRuleCount);
		double __stdcall GetNormEnabledFuzzyAttrCount(void);
		double __stdcall GetNormEnabledFuzzySetCount(void);
};
//---------------------------------------------------------------------------
#endif
