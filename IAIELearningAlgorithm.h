//---------------------------------------------------------------------------
#ifndef IAIELearningAlgorithmH
#define IAIELearningAlgorithmH
//---------------------------------------------------------------------------
#include "IAIEIterativeAlgorithm.h"
//---------------------------------------------------------------------------
class IAIELearningAlgorithm : public IAIEIterativeAlgorithm
{
 public:
	 virtual bool __stdcall GetSequentialLearning(void) = 0;
	 virtual void __stdcall SetSequentialLearning(bool ASequentialLearning) = 0;
	 virtual bool __stdcall GetRandomOrdering(void) = 0;
	 virtual void __stdcall SetRandomOrdering(bool ARandomOrdering) = 0;
	 virtual bool __stdcall GetRandomInitializing(void) = 0;
	 virtual void __stdcall SetRandomInitializing(bool ARandomInitializing) = 0;
	 virtual bool __stdcall GetRandomGeneratorSeedEnabled(void) = 0;
	 virtual void __stdcall SetRandomGeneratorSeedEnabled(bool ARandomGeneratorSeedEnabled) = 0;
	 virtual int __stdcall  GetRandomGeneratorSeedValue(void) = 0;
	 virtual void __stdcall SetRandomGeneratorSeedValue(int ARandomGeneratorSeedValue) = 0;
	 virtual bool __stdcall GetRandomGeneratorRangeEnabled(void) = 0;
	 virtual void __stdcall SetRandomGeneratorRangeEnabled(bool ARandomGeneratorRangeEnabled) = 0;
	 virtual double __stdcall  GetRandomGeneratorRangeMin(void) = 0;
	 virtual void __stdcall    SetRandomGeneratorRangeMin(double ARandomGeneratorRangeMin) = 0;
	 virtual double __stdcall  GetRandomGeneratorRangeMax(void) = 0;
	 virtual void __stdcall    SetRandomGeneratorRangeMax(double ARandomGeneratorRangeMax) = 0;

 #ifdef __BORLANDC__
	 __property bool SequentialLearning = {read=GetSequentialLearning, write=SetSequentialLearning};
	 __property bool RandomOrdering = {read=GetRandomOrdering, write=SetRandomOrdering};
	 __property bool RandomInitializing = {read=GetRandomInitializing, write=SetRandomInitializing};
	 __property bool RandomGeneratorSeedEnabled = {read=GetRandomGeneratorSeedEnabled, write=SetRandomGeneratorSeedEnabled};
	 __property int  RandomGeneratorSeedValue = {read=GetRandomGeneratorSeedValue, write=SetRandomGeneratorSeedValue};
	 __property bool RandomGeneratorRangeEnabled = {read=GetRandomGeneratorRangeEnabled, write=SetRandomGeneratorRangeEnabled};
	 __property double RandomGeneratorRangeMin = {read=GetRandomGeneratorRangeMin, write=SetRandomGeneratorRangeMin};
	 __property double RandomGeneratorRangeMax = {read=GetRandomGeneratorRangeMax, write=SetRandomGeneratorRangeMax};
 #endif
};
//---------------------------------------------------------------------------
#endif