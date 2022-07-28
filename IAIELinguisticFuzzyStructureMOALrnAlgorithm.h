//---------------------------------------------------------------------------
#ifndef IAIELinguisticFuzzyStructureMOALrnAlgorithmH
#define IAIELinguisticFuzzyStructureMOALrnAlgorithmH
//---------------------------------------------------------------------------
class IAIEStopConditions;
class IAIEInitConditions;
//---------------------------------------------------------------------------
#include "IAIELearningAlgorithm.h"
#include "IAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm.h"
//---------------------------------------------------------------------------
#define LFS_LRN_ALG_MOGA	0
//---------------------------------------------------------------------------
class IAIELinguisticFuzzyStructureMOALrnAlgorithm : public IAIELearningAlgorithm
{
 public:
	 virtual IAIEDataSet* __stdcall GetLrnDataSet(void) = 0;
	 virtual void __stdcall SetLrnDataSet(IAIEDataSet* ALrnDataSet) = 0;
	 virtual IAIEDataSet* __stdcall GetTstDataSet(void) = 0;
	 virtual void __stdcall SetTstDataSet(IAIEDataSet* ATstDataSet) = 0;

	 virtual IAIELinguisticFuzzyStructure* __stdcall GetInitFuzzyStructure(void) = 0;

	 virtual int __stdcall GetAlgorithmType(void) = 0;
	 virtual void __stdcall SetAlgorithmType(int AAlgorithmType) = 0;
	 virtual IAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm* __stdcall GetMOGA(void) = 0;

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

	 virtual int __stdcall GetParetoSize(void) = 0;

	 virtual IAIELinguisticFuzzyStructure* __stdcall GetFuzzyStructure(int ParetoIndex) = 0;
	 virtual double __stdcall GetNormError(int ParetoIndex) = 0;
	 virtual double __stdcall GetLrnError(int ParetoIndex) = 0;
	 virtual double __stdcall GetTstError(int ParetoIndex) = 0;
	 virtual int __stdcall GetLrnRecordCountInTol(int ParetoIndex) = 0;
	 virtual int __stdcall GetTstRecordCountInTol(int ParetoIndex) = 0;

	 virtual double __stdcall GetFuzzySystemComplexity(int ParetoIndex) = 0;
	 virtual double __stdcall GetNormFuzzyRulebaseFuzzyRuleComplexity(int ParetoIndex, int RulebaseIndex) = 0;
	 virtual double __stdcall GetNormFuzzyRulebaseFuzzyRuleCount(int ParetoIndex, int RulebaseIndex) = 0;
	 virtual double __stdcall GetNormFuzzyRulebaseFuzzyAttrCount(int ParetoIndex, int RulebaseIndex) = 0;
	 virtual double __stdcall GetNormFuzzyRulebaseFuzzySetCount(int ParetoIndex, int RulebaseIndex) = 0;

	 virtual int __stdcall GetParetoObjectiveCount(void) = 0;
	 virtual const char* __stdcall GetParetoObjectiveName(int ObjectiveIndex) = 0;
	 virtual double __stdcall GetParetoObjectiveValue(int ParetoIndex, int ObjectiveIndex) = 0;

 #ifdef __BORLANDC__
	 __property IAIEDataSet* LrnDataSet = {read=GetLrnDataSet, write=SetLrnDataSet};
	 __property IAIEDataSet* TstDataSet = {read=GetTstDataSet, write=SetTstDataSet};
	 __property int AlgorithmType = {read=GetAlgorithmType, write=SetAlgorithmType};
	 __property IAIELinguisticFuzzyStructure* InitFuzzyStructure = {read=GetInitFuzzyStructure};
	 __property IAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm* MOGA = {read=GetMOGA};

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

	 __property int ParetoSize = {read=GetParetoSize};
	 __property IAIELinguisticFuzzyStructure* FuzzyStructure[int ParetoIndex] = {read=GetFuzzyStructure};
	 __property double NormError[int ParetoIndex] = {read=GetNormError};
	 __property double LrnError[int ParetoIndex] = {read=GetLrnError};
	 __property double TstError[int ParetoIndex] = {read=GetTstError};
	 __property int LrnRecordCountInTol[int ParetoIndex] = {read=GetLrnRecordCountInTol};
	 __property int TstRecordCountInTol[int ParetoIndex] = {read=GetTstRecordCountInTol};

	 __property double FuzzySystemComplexity[int ParetoIndex] = {read=GetFuzzySystemComplexity};
	 __property double NormFuzzyRulebaseFuzzyRuleComplexity[int ParetoIndex][int RulebaseIndex] = {read=GetNormFuzzyRulebaseFuzzyRuleComplexity};
	 __property double NormFuzzyRulebaseFuzzyRuleCount[int ParetoIndex][int RulebaseIndex] = {read=GetNormFuzzyRulebaseFuzzyRuleCount};
	 __property double NormFuzzyRulebaseFuzzyAttrCount[int ParetoIndex][int RulebaseIndex] = {read=GetNormFuzzyRulebaseFuzzyAttrCount};
	 __property double NormFuzzyRulebaseFuzzySetCount[int ParetoIndex][int RulebaseIndex] = {read=GetNormFuzzyRulebaseFuzzySetCount};

	 __property int ParetoObjectiveCount = {read=GetParetoObjectiveCount};
	 __property const char* ParetoObjectiveName[int ObjectiveIndex] = {read=GetParetoObjectiveName};
	 __property double ParetoObjectiveValue[int ParetoIndex][int ObjectiveIndex] = {read=GetParetoObjectiveValue};
 #endif
};
//---------------------------------------------------------------------------
#endif