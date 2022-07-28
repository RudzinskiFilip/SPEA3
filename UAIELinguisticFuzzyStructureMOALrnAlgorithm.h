//---------------------------------------------------------------------------
#ifndef UAIELinguisticFuzzyStructureMOALrnAlgorithmH
#define UAIELinguisticFuzzyStructureMOALrnAlgorithmH
//---------------------------------------------------------------------------
#include "IAIELinguisticFuzzyStructureMOALrnAlgorithm.h"
//---------------------------------------------------------------------------
class IAIENumaThreads;
class TAIESystem;
class TAIEStopConditions;
class TAIEInitConditions;
class TAIEComputingTimer;
class TAIERandomPermutation;
class TAIEDataSet;
class TAIELinguisticFuzzyStructure;
class TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm;
//---------------------------------------------------------------------------
struct TParetoSolution
{
	TAIELinguisticFuzzyStructure* FuzzyStructure;

	double NormError;
	double LrnError;
	double TstError;
	int LrnRecordCountInTol;
	int TstRecordCountInTol;

	double FuzzyStructureComplexity;
	TVector<double> NormFuzzyRulebaseFuzzyRuleComplexity;
	TVector<double> NormFuzzyRulebaseFuzzyRuleCount;
	TVector<double> NormFuzzyRulebaseFuzzyAttrCount;
	TVector<double> NormFuzzyRulebaseFuzzySetCount;

	TAIEStopConditions* LrnErrors;
	TAIEStopConditions* TstErrors;

	TVector<double> ParetoObjects;
};
//---------------------------------------------------------------------------
class TAIELinguisticFuzzyStructureMOALrnAlgorithm : public IAIELinguisticFuzzyStructureMOALrnAlgorithm
{
 friend class TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm;
 private:
		TAIESystem* FSystem;
		TAIEDataSet* FLrnDataSet;
		TAIEDataSet* FTstDataSet;
		TAIEDataSet* FNoiseSet;
		TAIELinguisticFuzzyStructure* FInitFuzzyStructure;
		TAIERandomPermutation* FRandomPermutation;
		TAIERandomGenerator64* FNoiseGenerator;
		TAIERandomGenerator64* FRandomGenerator;
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
		TVector<TParetoSolution*> FParetoSolutions;

		int FAlgorithmType;
		TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm* FMOGA;

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
		__stdcall TAIELinguisticFuzzyStructureMOALrnAlgorithm(TAIESystem* ASystem);
		__stdcall ~TAIELinguisticFuzzyStructureMOALrnAlgorithm(void);
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

		IAIELinguisticFuzzyStructure* __stdcall GetInitFuzzyStructure(void);

		int __stdcall GetAlgorithmType(void);
		void __stdcall SetAlgorithmType(int AAlgorithmType);

		IAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm* __stdcall GetMOGA(void);

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

		int __stdcall GetParetoSize(void);
		void __stdcall SetParetoSize(int AParetoSize);

		IAIELinguisticFuzzyStructure* __stdcall GetFuzzyStructure(int ParetoIndex);
		double __stdcall GetNormError(int ParetoIndex);
		double __stdcall GetLrnError(int ParetoIndex);
		double __stdcall GetTstError(int ParetoIndex);
		int __stdcall GetLrnRecordCountInTol(int ParetoIndex);
		int __stdcall GetTstRecordCountInTol(int ParetoIndex);

		double __stdcall GetFuzzySystemComplexity(int ParetoIndex);
		double __stdcall GetNormFuzzyRulebaseFuzzyRuleComplexity(int ParetoIndex, int RulebaseIndex);
		double __stdcall GetNormFuzzyRulebaseFuzzyRuleCount(int ParetoIndex, int RulebaseIndex);
		double __stdcall GetNormFuzzyRulebaseFuzzyAttrCount(int ParetoIndex, int RulebaseIndex);
		double __stdcall GetNormFuzzyRulebaseFuzzySetCount(int ParetoIndex, int RulebaseIndex);

		int __stdcall GetParetoObjectiveCount(void);
		const char* __stdcall GetParetoObjectiveName(int ObjectiveIndex);
		double __stdcall GetParetoObjectiveValue(int ParetoIndex, int ObjectiveIndex);

		TAIERandomGenerator64* __stdcall GetRandomGenerator(void);
};
//---------------------------------------------------------------------------
#endif
