//---------------------------------------------------------------------------
#ifndef IAIELinguisticFuzzyStructureLrnAlgorithmH
#define IAIELinguisticFuzzyStructureLrnAlgorithmH
//---------------------------------------------------------------------------
class IAIEStopConditions;
class IAIEInitConditions;
//---------------------------------------------------------------------------
#include "IAIELearningAlgorithm.h"
#include "IAIELinguisticFuzzyStructureLrnAGAlgorithm.h"
//---------------------------------------------------------------------------
#define LFS_LRN_ALG_AG	0
//---------------------------------------------------------------------------
class IAIELinguisticFuzzyStructureLrnAlgorithm : public IAIELearningAlgorithm
{
 public:
	 virtual IAIEDataSet* __stdcall GetLrnDataSet(void) = 0;
	 virtual void __stdcall SetLrnDataSet(IAIEDataSet* ALrnDataSet) = 0;
	 virtual IAIEDataSet* __stdcall GetTstDataSet(void) = 0;
	 virtual void __stdcall SetTstDataSet(IAIEDataSet* ATstDataSet) = 0;

	 virtual IAIELinguisticFuzzyStructure* __stdcall GetFuzzyStructure(void) = 0;

	 virtual int __stdcall GetAlgorithmType(void) = 0;
	 virtual void __stdcall SetAlgorithmType(int AAlgorithmType) = 0;
	 virtual IAIELinguisticFuzzyStructureLrnAGAlgorithm* __stdcall GetAG(void) = 0;

	 virtual IAIEStopConditions* __stdcall GetLrnStopCondition(void) = 0;
	 virtual IAIEStopConditions* __stdcall GetTstStopCondition(void) = 0;
	 virtual IAIEStopConditions* __stdcall GetLrnCurrentErrors(void) = 0;
	 virtual IAIEStopConditions* __stdcall GetTstCurrentErrors(void) = 0;

	 virtual IAIEInitConditions* __stdcall GetInitConditionsRB(void) = 0;
	 virtual IAIEInitConditions* __stdcall GetInitConditionsIO(void) = 0;
	 virtual IAIEInitConditions* __stdcall GetInitConditionsMP(void) = 0;
	 virtual IAIEInitConditions* __stdcall GetInitConditionsMT(void) = 0;

	 virtual bool __stdcall GetMaxMinRuleCountEnabled(void) = 0;
	 virtual void __stdcall SetMaxMinRuleCountEnabled(bool AMaxMinRuleCountEnabled) = 0;
	 virtual int __stdcall GetMaxRuleCountValue(void) = 0;
	 virtual void __stdcall SetMaxRuleCountValue(int AMaxRuleCountValue) = 0;
	 virtual int __stdcall GetMinRuleCountValue(void) = 0;
	 virtual void __stdcall SetMinRuleCountValue(int AMinRuleCountValue) = 0;
	 virtual double __stdcall GetRulebasePriority(void) = 0;
	 virtual void __stdcall SetRulebasePriority(double ARulebasePriority) = 0;
	 virtual bool __stdcall GetOnlyFullFuzzyRule(void) = 0;
	 virtual void __stdcall SetOnlyFullFuzzyRule(bool AOnlyFullFuzzyRule) = 0;
	 virtual bool __stdcall GetOnlyTrueFuzzyRule(void) = 0;
	 virtual void __stdcall SetOnlyTrueFuzzyRule(bool AOnlyTrueFuzzyRule) = 0;
	 virtual bool __stdcall GetBinaryEncoding(void) = 0;
	 virtual void __stdcall SetBinaryEncoding(bool ABinaryEncoding) = 0;

	 virtual bool __stdcall GetOC_FuzzyRuleCount(void) = 0;
	 virtual void __stdcall SetOC_FuzzyRuleCount(bool AOC_FuzzyRuleCount) = 0;
	 virtual bool __stdcall GetOC_FuzzyLength(void) = 0;
	 virtual void __stdcall SetOC_FuzzyLength(bool AOC_FuzzyLength) = 0;
	 virtual bool __stdcall GetOC_FuzzyAttrCount(void) = 0;
	 virtual void __stdcall SetOC_FuzzyAttrCount(bool AOC_FuzzyAttrCount) = 0;
	 virtual bool __stdcall GetOC_FuzzySetCount(void) = 0;
	 virtual void __stdcall SetOC_FuzzySetCount(bool AOC_FuzzySetCount) = 0;
	 virtual bool __stdcall GetOC_AllFuzzyAttrCount(void) = 0;
	 virtual void __stdcall SetOC_AllFuzzyAttrCount(bool AOC_AllFuzzyAttrCount) = 0;
	 virtual bool __stdcall GetOC_AllFuzzySetCount(void) = 0;
	 virtual void __stdcall SetOC_AllFuzzySetCount(bool AOC_AllFuzzySetCount) = 0;

	 virtual bool __stdcall GetIsNoiseInputs(void) = 0;
	 virtual void __stdcall SetIsNoiseInputs(bool AIsNoiseInputs) = 0;
	 virtual double __stdcall GetNoiseInputs(void) = 0;
	 virtual void __stdcall SetNoiseInputs(double ANoiseInputs) = 0;

	 virtual double __stdcall GetCurrentNormError(void) = 0;
	 virtual double __stdcall GetCurrentLrnError(void) = 0;
	 virtual double __stdcall GetCurrentTstError(void) = 0;
	 virtual int __stdcall GetCurrentLrnRecordCountInTol(void) = 0;
	 virtual int __stdcall GetCurrentTstRecordCountInTol(void) = 0;

	 virtual double __stdcall GetCurrentFuzzySystemComplexity(void) = 0;
	 virtual double __stdcall GetCurrentNormFuzzyRulebaseFuzzyRuleComplexity(int Index) = 0;
	 virtual double __stdcall GetCurrentNormFuzzyRulebaseFuzzyRuleCount(int Index) = 0;
	 virtual double __stdcall GetCurrentNormFuzzyRulebaseFuzzyAttrCount(int Index) = 0;
	 virtual double __stdcall GetCurrentNormFuzzyRulebaseFuzzySetCount(int Index) = 0;

 #ifdef __BORLANDC__
	 __property IAIEDataSet* LrnDataSet = {read=GetLrnDataSet, write=SetLrnDataSet};
	 __property IAIEDataSet* TstDataSet = {read=GetTstDataSet, write=SetTstDataSet};
	 __property IAIELinguisticFuzzyStructure* FuzzyStructure = {read=GetFuzzyStructure};
	 __property int AlgorithmType = {read=GetAlgorithmType, write=SetAlgorithmType};
	 __property IAIELinguisticFuzzyStructureLrnAGAlgorithm* AG = {read=GetAG};

	 // WARUNKI STOPU DLA KA¯DEGO Z WYJŒÆ SIECI NEURONOWEJ NIEZALE¯NIE
	 __property IAIEStopConditions* LrnStopCondition = {read=GetLrnStopCondition};
	 __property IAIEStopConditions* TstStopCondition = {read=GetTstStopCondition};

	 // BIE¯¥CE B£ÊDY ODPOWIEDZI SIECI DLA KA¯DEGO Z WYJŒÆ NIEZALE¯NIE
	 __property IAIEStopConditions* LrnCurrentErrors = {read=GetLrnCurrentErrors};
	 __property IAIEStopConditions* TstCurrentErrors = {read=GetTstCurrentErrors};

	 __property IAIEInitConditions* InitConditionsRB = {read=GetInitConditionsRB};
	 __property IAIEInitConditions* InitConditionsIO = {read=GetInitConditionsIO};
	 __property IAIEInitConditions* InitConditionsMP = {read=GetInitConditionsMP};
	 __property IAIEInitConditions* InitConditionsMT = {read=GetInitConditionsMT};

	 __property int MaxRuleCountValue = {read=GetMaxRuleCountValue, write=SetMaxRuleCountValue};
	 __property int MinRuleCountValue = {read=GetMinRuleCountValue, write=SetMinRuleCountValue};
	 __property bool MaxMinRuleCountEnabled = {read=GetMaxMinRuleCountEnabled, write=SetMaxMinRuleCountEnabled};
	 __property bool OnlyFullFuzzyRule = {read=GetOnlyFullFuzzyRule, write=SetOnlyFullFuzzyRule};
	 __property bool OnlyTrueFuzzyRule = {read=GetOnlyTrueFuzzyRule, write=SetOnlyTrueFuzzyRule};
	 __property double RulebasePriority = {read=GetRulebasePriority, write=SetRulebasePriority};
	 __property bool BinaryEncoding = {read=GetBinaryEncoding, write=SetBinaryEncoding};

	 // kryteria optymalizacji
	 __property bool OC_FuzzyRuleCount = {read=GetOC_FuzzyRuleCount, write=SetOC_FuzzyRuleCount};
	 __property bool OC_FuzzyLength = {read=GetOC_FuzzyLength, write=SetOC_FuzzyLength};
	 __property bool OC_FuzzyAttrCount = {read=GetOC_FuzzyAttrCount, write=SetOC_FuzzyAttrCount};
	 __property bool OC_FuzzySetCount = {read=GetOC_FuzzySetCount, write=SetOC_FuzzySetCount};
	 __property bool OC_AllFuzzyAttrCount = {read=GetOC_AllFuzzyAttrCount, write=SetOC_AllFuzzyAttrCount};
	 __property bool OC_AllFuzzySetCount = {read=GetOC_AllFuzzySetCount, write=SetOC_AllFuzzySetCount};

	 __property bool IsNoiseInputs = {read=GetIsNoiseInputs, write=SetIsNoiseInputs};
	 __property double NoiseInputs = {read=GetNoiseInputs, write=SetNoiseInputs};

	 __property double CurrentNormError = {read=GetCurrentNormError};
	 __property double CurrentLrnError = {read=GetCurrentLrnError};
	 __property double CurrentTstError = {read=GetCurrentTstError};
	 __property int CurrentLrnRecordCountInTol = {read=GetCurrentLrnRecordCountInTol};
	 __property int CurrentTstRecordCountInTol = {read=GetCurrentTstRecordCountInTol};

	 __property double CurrentFuzzySystemComplexity = {read=GetCurrentFuzzySystemComplexity};
	 __property double CurrentNormFuzzyRulebaseFuzzyRuleComplexity[int Index] = {read=GetCurrentNormFuzzyRulebaseFuzzyRuleComplexity};
	 __property double CurrentNormFuzzyRulebaseFuzzyRuleCount[int Index] = {read=GetCurrentNormFuzzyRulebaseFuzzyRuleCount};
	 __property double CurrentNormFuzzyRulebaseFuzzyAttrCount[int Index] = {read=GetCurrentNormFuzzyRulebaseFuzzyAttrCount};
	 __property double CurrentNormFuzzyRulebaseFuzzySetCount[int Index] = {read=GetCurrentNormFuzzyRulebaseFuzzySetCount};
 #endif
};
//---------------------------------------------------------------------------
#endif