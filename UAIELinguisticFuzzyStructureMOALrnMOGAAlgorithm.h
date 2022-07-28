//---------------------------------------------------------------------------
#ifndef UAIELinguisticFuzzyStructureMOALrnMOGAAlgorithmH
#define UAIELinguisticFuzzyStructureMOALrnMOGAAlgorithmH
//---------------------------------------------------------------------------
// PARAMETRY UCZENIA - ALGORYTM GENETYCZNY
//---------------------------------------------------------------------------
#include "IAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm.h"
//---------------------------------------------------------------------------
class IAIEMultiobjectiveGeneticAlgorithm;
//---------------------------------------------------------------------------
class TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm : public IAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm
{
 private:
		TAIESystem* FSystem;
		TAIELinguisticFuzzyStructureMOALrnAlgorithm* FOwner;
		IAIENumaThreads* FThreadsLrn;
		IAIEDataSet* FLrnDataSet;
		IAIEDataSet* FTstDataSet;

		IAIEMultiobjectiveGeneticAlgorithm* FGeneticAlgorithm;
		int FType;
		int FPopulationSize;
		int FPopulationSizePerObjective;
		int FMaxParetoSize;
		int FSelectMethod;
		int FSelectionPressure;
		bool FIsConstCrossFactor;
		double FConstCrossFactor;
		IAIEFunction* FFunctCrossFactor;
		bool FIsConstMutationFactor;
		double FConstMutationFactor;
		IAIEFunction* FFunctMutationFactor;
		bool FElitistStrategy;

		double FCurrentCrossFactor;
		double FCurrentMutationFactor;

		int FParetoObjectiveCount;
		TVector<const char*> FParetoObjectiveName;

		TVector< TVector<double> > FCurrentFitnessValue;
		TVector<double> FCurrentMinFitnessValue;
		TVector<double> FCurrentAvgFitnessValue;
		TVector<double> FCurrentMaxFitnessValue;

		void __stdcall ChromosomeToStructure(void);
		void __stdcall SummaryLrnErros(void);
		void __stdcall SummaryTstErros(void);

 public:
		__stdcall TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm(TAIELinguisticFuzzyStructureMOALrnAlgorithm* AOwner, TAIESystem* ASystem);
		__stdcall ~TAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm(void);
		void __stdcall LoadFromFile(IAIEBinaryFile* File);
		void __stdcall SaveToFile(IAIEBinaryFile* File);

		void __stdcall Prepare(void);
		void __stdcall Finish(void);
		void __stdcall ExecuteNextIteration(void);

		int __stdcall GetType(void);
		void __stdcall SetType(int AType);

		int __stdcall GetPopulationSize(void);
		int __stdcall GetPopulationSizePerObjective(void);
		void __stdcall SetPopulationSizePerObjective(int APopulationSizePerObjective);
		int __stdcall GetMaxParetoSize(void);
		void __stdcall SetMaxParetoSize(int AMaxParetoSize);

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

		double __stdcall GetCurrentFitnessValue(int AChromosomeIndex, int AObjectiveIndex);
		double __stdcall GetCurrentMaxFitnessValue(int AObjectiveIndex);
		double __stdcall GetCurrentAvgFitnessValue(int AObjectiveIndex);
		double __stdcall GetCurrentMinFitnessValue(int AObjectiveIndex);

		int __stdcall GetParetoObjectiveCount(void);
		const char* __stdcall GetParetoObjectiveName(int ObjectiveIndex);
};
//---------------------------------------------------------------------------
#endif
