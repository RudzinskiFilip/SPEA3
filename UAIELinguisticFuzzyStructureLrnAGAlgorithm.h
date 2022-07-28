//---------------------------------------------------------------------------
#ifndef UAIELinguisticFuzzyStructureLrnAGAlgorithmH
#define UAIELinguisticFuzzyStructureLrnAGAlgorithmH
//---------------------------------------------------------------------------
// PARAMETRY UCZENIA - ALGORYTM GENETYCZNY
//---------------------------------------------------------------------------
#include "IAIELinguisticFuzzyStructureLrnAGAlgorithm.h"
//---------------------------------------------------------------------------
class TAIEGeneticAlgorithm;
//---------------------------------------------------------------------------
class TAIELinguisticFuzzyStructureLrnAGAlgorithm : public IAIELinguisticFuzzyStructureLrnAGAlgorithm
{
 private:
		TAIESystem* FSystem;
		TAIELinguisticFuzzyStructureLrnAlgorithm* FOwner;
		IAIENumaThreads* FThreadsLrn;
		IAIEDataSet* FLrnDataSet;
		IAIEDataSet* FTstDataSet;
		TAIELinguisticFuzzyStructure* FFuzzyStructure;

		TAIEGeneticAlgorithm* FGeneticAlgorithm;
		int FPopulationSize;
		int FSelectMethod;
		int FSelectionPressure;
		bool FIsConstCrossFactor;
		double FConstCrossFactor;
		IAIEFunction* FFunctCrossFactor;
		bool FIsConstMutationFactor;
		double FConstMutationFactor;
		IAIEFunction* FFunctMutationFactor;
		bool FElitistStrategy;

		TVector<double> FCurrentFitnessValue;
		double FCurrentMinFitnessValue;
		double FCurrentAvgFitnessValue;
		double FCurrentMaxFitnessValue;
		double FCurrentCrossFactor;
		double FCurrentMutationFactor;

		bool FEpochMonitoringEnabled;
		TVector<TVector<double>*> FEpochMonitoringFitnessValue;
		double FEpochMonitoringMaxFitnessValue;
		double FEpochMonitoringMinFitnessValue;

		void __stdcall ChromosomeToStructure(void);
		void __stdcall SummaryLrnErros(void);
		void __stdcall SummaryTstErros(void);

 public:
		__stdcall TAIELinguisticFuzzyStructureLrnAGAlgorithm(TAIELinguisticFuzzyStructureLrnAlgorithm* AOwner, TAIESystem* ASystem);
		__stdcall ~TAIELinguisticFuzzyStructureLrnAGAlgorithm(void);
		void __stdcall LoadFromFile(IAIEBinaryFile* File);
		void __stdcall SaveToFile(IAIEBinaryFile* File);

		void __stdcall Prepare(void);
		void __stdcall Finish(void);
		void __stdcall ExecuteNextIteration(void);

		int __stdcall GetPopulationSize(void);
		void __stdcall SetPopulationSize(int APopulationSize);
		int __stdcall GetSelectMethod(void);
		void __stdcall SetSelectMethod(int ASelectMethod);
		int __stdcall GetSelectionPressure(void);
		void __stdcall SetSelectionPressure(int ASelectionPressure);

		bool __stdcall GetIsConstCrossFactor(void);
		void __stdcall SetIsConstCrossFactor(bool AIsConstCrossFactor);
		double __stdcall GetConstCrossFactor(void);
		void __stdcall SetConstCrossFactor(double AConstCrossFactor);
		IAIEFunction* __stdcall GetFunctCrossFactor(void);

		bool __stdcall GetIsConstMutationFactor(void);
		void __stdcall SetIsConstMutationFactor(bool AIsConstMutationFactor);
		double __stdcall GetConstMutationFactor(void);
		void __stdcall SetConstMutationFactor(double AConstMutationFactor);
		IAIEFunction* __stdcall GetFunctMutationFactor(void);

		bool __stdcall GetElitistStrategy(void);
		void __stdcall SetElitistStrategy(bool AElitistStrategy);

		double __stdcall GetCurrentCrossFactor(void);
		double __stdcall GetCurrentMutationFactor(void);

		double __stdcall GetCurrentFitnessValue(int AIndex);
		double __stdcall GetCurrentMaxFitnessValue(void);
		double __stdcall GetCurrentAvgFitnessValue(void);
		double __stdcall GetCurrentMinFitnessValue(void);

		bool __stdcall   GetEpochMonitoringEnabled(void);
		void __stdcall   SetEpochMonitoringEnabled(bool AEpochMonitoringEnabled);
		int  __stdcall   GetEpochMonitoringCount(void);
		double __stdcall GetEpochMonitoringFitnessValue(int EpochIndex, int Index);
		double __stdcall GetEpochMonitoringMaxFitnessValue(void);
		double __stdcall GetEpochMonitoringMinFitnessValue(void);
};
//---------------------------------------------------------------------------
#endif
