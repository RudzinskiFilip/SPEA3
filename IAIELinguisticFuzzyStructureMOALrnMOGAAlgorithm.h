//---------------------------------------------------------------------------
#ifndef IAIELinguisticFuzzyStructureMOALrnMOGAAlgorithmH
#define IAIELinguisticFuzzyStructureMOALrnMOGAAlgorithmH
//---------------------------------------------------------------------------
// PARAMETRY UCZENIA - ALGORYTM GENETYCZNY
//---------------------------------------------------------------------------
#define MOGA_TYPE_VEGA		0
#define MOGA_TYPE_SPEA		1
#define MOGA_TYPE_SPEA2		2
#define MOGA_TYPE_NSGA		3
#define MOGA_TYPE_NSGA2		4
#define MOGA_TYPE_ENSGA2	5
#define MOGA_TYPE_SPEA3		6

//---------------------------------------------------------------------------
class IAIELinguisticFuzzyStructureMOALrnMOGAAlgorithm
{
 public:
	 virtual int __stdcall GetType(void) = 0;
	 virtual void __stdcall SetType(int AType) = 0;

	 virtual int __stdcall GetPopulationSize(void) = 0;
	 virtual int __stdcall GetPopulationSizePerObjective(void) = 0;
	 virtual void __stdcall SetPopulationSizePerObjective(int APopulationSizePerObjective) = 0;
	 virtual int __stdcall GetMaxParetoSize(void) = 0;
	 virtual void __stdcall SetMaxParetoSize(int AMaxParetoSize) = 0;
	 virtual int __stdcall GetSelectMethod(void) = 0;
	 virtual void __stdcall SetSelectMethod(int ASelectMethod) = 0;
	 virtual int __stdcall GetSelectionPressure(void) = 0;
	 virtual void __stdcall SetSelectionPressure(int ASelectionPressure) = 0;

	 virtual bool __stdcall GetIsConstCrossFactor(void) = 0;
	 virtual void __stdcall SetIsConstCrossFactor(bool AIsConstCrossFactor) = 0;
	 virtual double __stdcall GetConstCrossFactor(void) = 0;
	 virtual void __stdcall SetConstCrossFactor(double AConstCrossFactor) = 0;
	 virtual IAIEFunction* __stdcall GetFunctCrossFactor(void) = 0;
	 virtual bool __stdcall GetIsConstMutationFactor(void) = 0;
	 virtual void __stdcall SetIsConstMutationFactor(bool AIsConstMutationFactor) = 0;
	 virtual double __stdcall GetConstMutationFactor(void) = 0;
	 virtual void __stdcall SetConstMutationFactor(double AConstMutationFactor) = 0;
	 virtual IAIEFunction* __stdcall GetFunctMutationFactor(void) = 0;

	 virtual bool __stdcall GetElitistStrategy(void) = 0;
	 virtual void __stdcall SetElitistStrategy(bool AElitistStrategy) = 0;

	 virtual double __stdcall GetCurrentCrossFactor(void) = 0;
	 virtual double __stdcall GetCurrentMutationFactor(void) = 0;

	 virtual double __stdcall GetCurrentMaxFitnessValue(int ObjectiveIndex) = 0;
	 virtual double __stdcall GetCurrentAvgFitnessValue(int ObjectiveIndex) = 0;
	 virtual double __stdcall GetCurrentMinFitnessValue(int ObjectiveIndex) = 0;
	 virtual double __stdcall GetCurrentFitnessValue(int ChromosomeIndex, int ObjectiveIndex) = 0;

	 virtual int __stdcall GetParetoObjectiveCount(void) = 0;
	 virtual const char* __stdcall GetParetoObjectiveName(int ObjectiveIndex) = 0;

 #ifdef __BORLANDC__
	 __property int Type = {read=GetType, write=SetType};

	 __property int PopulationSize = {read=GetPopulationSize};
	 __property int PopulationSizePerObjective = {read=GetPopulationSizePerObjective, write=SetPopulationSizePerObjective};
	 __property int MaxParetoSize = {read=GetMaxParetoSize, write=SetMaxParetoSize};

	 __property int SelectMethod = {read=GetSelectMethod, write=SetSelectMethod};
	 __property int SelectionPressure = {read=GetSelectionPressure, write=SetSelectionPressure};
	 __property bool ElitistStrategy = {read=GetElitistStrategy, write=SetElitistStrategy};

	 __property bool IsConstCrossFactor = {read=GetIsConstCrossFactor, write=SetIsConstCrossFactor};
	 __property double ConstCrossFactor = {read=GetConstCrossFactor, write=SetConstCrossFactor};
	 __property IAIEFunction* FunctCrossFactor = {read=GetFunctCrossFactor};

	 __property bool IsConstMutationFactor = {read=GetIsConstMutationFactor, write=SetIsConstMutationFactor};
	 __property double ConstMutationFactor = {read=GetConstMutationFactor, write=SetConstMutationFactor};
	 __property IAIEFunction* FunctMutationFactor = {read=GetFunctMutationFactor};

	 // - bie¿¹ce parametry uczenia (w trakcie uczenia)
	 __property double CurrentCrossFactor = {read=GetCurrentCrossFactor};
	 __property double CurrentMutationFactor = {read=GetCurrentMutationFactor};

	 __property double CurrentMaxFitnessValue[int ObjectiveIndex] = {read=GetCurrentMaxFitnessValue};
	 __property double CurrentAvgFitnessValue[int ObjectiveIndex] = {read=GetCurrentAvgFitnessValue};
	 __property double CurrentMinFitnessValue[int ObjectiveIndex] = {read=GetCurrentMinFitnessValue};
	 __property double CurrentFitnessValue[int ChromosomeIndex][int ObjectiveIndex] = {read=GetCurrentFitnessValue};

	 __property int ParetoObjectiveCount = {read=GetParetoObjectiveCount};
	 __property const char* ParetoObjectiveName = {read=GetParetoObjectiveName};
 #endif
};
//---------------------------------------------------------------------------
#endif