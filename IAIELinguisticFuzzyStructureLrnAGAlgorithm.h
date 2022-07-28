//---------------------------------------------------------------------------
#ifndef IAIELinguisticFuzzyStructureLrnAGAlgorithmH
#define IAIELinguisticFuzzyStructureLrnAGAlgorithmH
//---------------------------------------------------------------------------
// PARAMETRY UCZENIA - ALGORYTM GENETYCZNY
//---------------------------------------------------------------------------
class IAIELinguisticFuzzyStructureLrnAGAlgorithm
{
 public:
	 virtual int __stdcall GetPopulationSize(void) = 0;
	 virtual void __stdcall SetPopulationSize(int APopulationSize) = 0;
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

	 virtual double __stdcall GetCurrentMaxFitnessValue(void) = 0;
	 virtual double __stdcall GetCurrentAvgFitnessValue(void) = 0;
	 virtual double __stdcall GetCurrentMinFitnessValue(void) = 0;
	 virtual double __stdcall GetCurrentFitnessValue(int Index) = 0;

	 virtual bool __stdcall GetEpochMonitoringEnabled(void) = 0;
	 virtual void __stdcall SetEpochMonitoringEnabled(bool AEpochMonitoringEnabled) = 0;
	 virtual int  __stdcall GetEpochMonitoringCount(void) = 0;
	 virtual double __stdcall GetEpochMonitoringFitnessValue(int PopIndex, int Index) = 0;
	 virtual double __stdcall GetEpochMonitoringMaxFitnessValue(void) = 0;
	 virtual double __stdcall GetEpochMonitoringMinFitnessValue(void) = 0;

 #ifdef __BORLANDC__
	 __property int PopulationSize = {read=GetPopulationSize, write=SetPopulationSize};

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

	 __property double CurrentMaxFitnessValue = {read=GetCurrentMaxFitnessValue};
	 __property double CurrentAvgFitnessValue = {read=GetCurrentAvgFitnessValue};
	 __property double CurrentMinFitnessValue = {read=GetCurrentMinFitnessValue};
	 __property double CurrentFitnessValue[int Index] = {read=GetCurrentFitnessValue};

	 __property bool   EpochMonitoringEnabled = {read=GetEpochMonitoringEnabled, write=SetEpochMonitoringEnabled};
	 __property int    EpochMonitoringCount = {read=GetEpochMonitoringCount};
	 __property double EpochMonitoringFitnessValue[int EpochIndex][int Index] = {read=GetEpochMonitoringFitnessValue};
	 __property double EpochMonitoringMaxFitnessValue = {read=GetEpochMonitoringMaxFitnessValue};
	 __property double EpochMonitoringMinFitnessValue = {read=GetEpochMonitoringMinFitnessValue};
 #endif
};
//---------------------------------------------------------------------------
#endif