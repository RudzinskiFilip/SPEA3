//---------------------------------------------------------------------------
#ifndef UAIELinguisticFuzzyStructureLrnAlgorithmH
#define UAIELinguisticFuzzyStructureLrnAlgorithmH
//---------------------------------------------------------------------------
#include "IAIELinguisticFuzzyStructureLrnAlgorithm.h"
//---------------------------------------------------------------------------
class IAIENumaThreads;
class TAIESystem;
class TAIEStopConditions;
class TAIEInitConditions;
class TAIEComputingTimer;
class TAIERandomPermutation;
class TAIEDataSet;
class TAIELinguisticFuzzyStructure;
class TAIELinguisticFuzzyStructureLrnAGAlgorithm;
//---------------------------------------------------------------------------
class TAIELinguisticFuzzyStructureLrnAlgorithm : public IAIELinguisticFuzzyStructureLrnAlgorithm
{
 friend class TAIELinguisticFuzzyStructureLrnAGAlgorithm;
 private:
		TAIESystem* FSystem;
		TAIEDataSet* FLrnDataSet;
		TAIEDataSet* FTstDataSet;
		TAIEDataSet* FNoiseSet;
		TAIELinguisticFuzzyStructure* FFuzzyStructure;
		TAIERandomPermutation* FRandomPermutation;
		TAIERandomGenerator64* FNoiseGenerator;
		bool FIsNoiseInputs;
		double FNoiseInputs;
		bool FSequentialLearning;
		bool FRandomOrdering;
		bool FRandomInitializing;
		bool FRandomGeneratorSeedEnabled;
		int FRandomGeneratorSeedValue;
		bool FRandomGeneratorRangeEnabled;
		double FRandomGeneratorRangeMin;
		double FRandomGeneratorRangeMax;
		int FTag;

		TAIEComputingTimer* FComputingTimer;
		int FIterationIndex;
		bool FIsPrepared;
		bool FIsExecuting;
		bool FIsWaiting;
		bool FIsFinished;

		// bie¿¹ce parametry uczenia
		double FCurrentNormError;
		double FCurrentLrnError;
		double FCurrentTstError;
		int FCurrentLrnRecordCountInTol;
		int FCurrentTstRecordCountInTol;

		double FCurrentFuzzyStructureComplexity;
		TVector<double> FCurrentNormFuzzyRulebaseFuzzyRuleComplexity;
		TVector<double> FCurrentNormFuzzyRulebaseFuzzyRuleCount;
		TVector<double> FCurrentNormFuzzyRulebaseFuzzyAttrCount;
		TVector<double> FCurrentNormFuzzyRulebaseFuzzySetCount;

		int FAlgorithmType;
		TAIELinguisticFuzzyStructureLrnAGAlgorithm* FAG;

		TAIEStopConditions* FLrnStopCondition;
		TAIEStopConditions* FTstStopCondition;
		TAIEStopConditions* FLrnCurrentErrors;
		TAIEStopConditions* FTstCurrentErrors;

		TAIEInitConditions* FInitConditionsRB;
		TAIEInitConditions* FInitConditionsIO;
		TAIEInitConditions* FInitConditionsMP;
		TAIEInitConditions* FInitConditionsMT;

		bool FMaxMinRuleCountEnabled;	//w³¹cz ograniczenie maksymalnej liczby regu³
		int FMaxRuleCountValue;		//maksymalna liczba regu³
		int FMinRuleCountValue;		//minimalna liczba regu³
		double FRulebasePriority;	//priorytet bazy regu³
		bool FOnlyFullFuzzyRule;	//w³¹cz blokowanie wy³¹czenia przes³anek
		bool FOnlyTrueFuzzyRule;	//w³¹cz blokowanie negowania przes³anek
		bool FBinaryEncoding;		//w³¹cz kodowanie binarne parametrów zbiorów rozmytych

		bool FOC_FuzzyRuleCount;
		bool FOC_FuzzyLength;
		bool FOC_FuzzyAttrCount;
		bool FOC_FuzzySetCount;
		bool FOC_AllFuzzyAttrCount;
		bool FOC_AllFuzzySetCount;

 public:
		__stdcall TAIELinguisticFuzzyStructureLrnAlgorithm(TAIESystem* ASystem);
		__stdcall ~TAIELinguisticFuzzyStructureLrnAlgorithm(void);
		void __stdcall Free(void);
		void __stdcall LoadFromFile(IAIEBinaryFile* File);
		void __stdcall SaveToFile(IAIEBinaryFile* File);

		void __stdcall Prepare(void);
		void __stdcall Finish(void);
		void __stdcall Clear(void);
		bool __stdcall ExecuteNextIteration(void);

		bool __stdcall GetIsPrepared(void);
		bool __stdcall GetIsExecuting(void);
		bool __stdcall GetIsWaiting(void);
		bool __stdcall GetIsFinished(void);
		int __stdcall GetIterationIndex(void);
		IAIEComputingTimer* __stdcall GetComputingTimer(void);

		bool __stdcall GetIsNoiseInputs(void);
		void __stdcall SetIsNoiseInputs(bool AIsNoiseInputs);
		double __stdcall GetNoiseInputs(void);
		void __stdcall SetNoiseInputs(double ANoiseInputs);
		bool __stdcall GetSequentialLearning(void);
		void __stdcall SetSequentialLearning(bool ASequentialLearning);
		bool __stdcall GetRandomOrdering(void);
		void __stdcall SetRandomOrdering(bool ARandomOrdering);
		bool __stdcall GetRandomInitializing(void);
		void __stdcall SetRandomInitializing(bool ARandomInitializing);
		bool __stdcall GetRandomGeneratorSeedEnabled(void);
		void __stdcall SetRandomGeneratorSeedEnabled(bool ARandomGeneratorSeedEnabled);
		int __stdcall  GetRandomGeneratorSeedValue(void);
		void __stdcall SetRandomGeneratorSeedValue(int ARandomGeneratorSeedValue);
		bool __stdcall GetRandomGeneratorRangeEnabled(void);
		void __stdcall SetRandomGeneratorRangeEnabled(bool ARandomGeneratorRangeEnabled);
		double __stdcall GetRandomGeneratorRangeMin(void);
		void __stdcall   SetRandomGeneratorRangeMin(double ARandomGeneratorRangeMin);
		double __stdcall GetRandomGeneratorRangeMax(void);
		void __stdcall   SetRandomGeneratorRangeMax(double ARandomGeneratorRangeMax);

		int __stdcall GetTag(void);
		void __stdcall SetTag(int ATag);

		IAIEDataSet* __stdcall GetLrnDataSet(void);
		void __stdcall SetLrnDataSet(IAIEDataSet* ALrnDataSet);
		IAIEDataSet* __stdcall GetTstDataSet(void);
		void __stdcall SetTstDataSet(IAIEDataSet* ATstDataSet);
		IAIEDataSet* __stdcall GetNoiseSet(void);

		IAIELinguisticFuzzyStructure* __stdcall GetFuzzyStructure(void);

		int __stdcall GetAlgorithmType(void);
		void __stdcall SetAlgorithmType(int AAlgorithmType);

		IAIELinguisticFuzzyStructureLrnAGAlgorithm* __stdcall GetAG(void);

		IAIEStopConditions* __stdcall GetLrnStopCondition(void);
		IAIEStopConditions* __stdcall GetTstStopCondition(void);
		IAIEStopConditions* __stdcall GetLrnCurrentErrors(void);
		IAIEStopConditions* __stdcall GetTstCurrentErrors(void);

		IAIEInitConditions* __stdcall GetInitConditionsRB(void);
		IAIEInitConditions* __stdcall GetInitConditionsIO(void);
		IAIEInitConditions* __stdcall GetInitConditionsMP(void);
		IAIEInitConditions* __stdcall GetInitConditionsMT(void);

		bool __stdcall GetMaxMinRuleCountEnabled(void);
		void __stdcall SetMaxMinRuleCountEnabled(bool AMaxMinRuleCountEnabled);
		int __stdcall GetMaxRuleCountValue(void);
		void __stdcall SetMaxRuleCountValue(int AMaxRuleCountValue);
		int __stdcall GetMinRuleCountValue(void);
		void __stdcall SetMinRuleCountValue(int AMinRuleCountValue);
		double __stdcall GetRulebasePriority(void);
		void __stdcall SetRulebasePriority(double ARulebasePriority);
		bool __stdcall GetOnlyFullFuzzyRule(void);
		void __stdcall SetOnlyFullFuzzyRule(bool AOnlyFullFuzzyRule);
		bool __stdcall GetOnlyTrueFuzzyRule(void);
		void __stdcall SetOnlyTrueFuzzyRule(bool AOnlyTrueFuzzyRule);
		bool __stdcall GetBinaryEncoding(void);
		void __stdcall SetBinaryEncoding(bool ABinaryEncoding);

		bool __stdcall GetOC_FuzzyRuleCount(void);
		void __stdcall SetOC_FuzzyRuleCount(bool AOC_FuzzyRuleCount);
		bool __stdcall GetOC_FuzzyLength(void);
		void __stdcall SetOC_FuzzyLength(bool AOC_FuzzyLength);
		bool __stdcall GetOC_FuzzyAttrCount(void);
		void __stdcall SetOC_FuzzyAttrCount(bool AOC_FuzzyAttrCount);
		bool __stdcall GetOC_FuzzySetCount(void);
		void __stdcall SetOC_FuzzySetCount(bool AOC_FuzzySetCount);
		bool __stdcall GetOC_AllFuzzyAttrCount(void);
		void __stdcall SetOC_AllFuzzyAttrCount(bool AOC_AllFuzzyAttrCount);
		bool __stdcall GetOC_AllFuzzySetCount(void);
		void __stdcall SetOC_AllFuzzySetCount(bool AOC_AllFuzzySetCount);

		double __stdcall GetCurrentNormError(void);
		void __stdcall SetCurrentNormError(double ACurrentNormError);
		double __stdcall GetCurrentLrnError(void);
		void __stdcall SetCurrentLrnError(double ACurrentLrnError);
		double __stdcall GetCurrentTstError(void);
		void __stdcall SetCurrentTstError(double ACurrentTstError);
		int __stdcall GetCurrentLrnRecordCountInTol(void);
		void __stdcall SetCurrentLrnRecordCountInTol(int ACurrentLrnRecordCountInTol);
		int __stdcall GetCurrentTstRecordCountInTol(void);
		void __stdcall SetCurrentTstRecordCountInTol(int ACurrentTstRecordCountInTol);

		double __stdcall GetCurrentFuzzySystemComplexity(void);
		double __stdcall GetCurrentNormFuzzyRulebaseFuzzyRuleComplexity(int Index);
		double __stdcall GetCurrentNormFuzzyRulebaseFuzzyRuleCount(int Index);
		double __stdcall GetCurrentNormFuzzyRulebaseFuzzyAttrCount(int Index);
		double __stdcall GetCurrentNormFuzzyRulebaseFuzzySetCount(int Index);
};
//---------------------------------------------------------------------------
#endif
