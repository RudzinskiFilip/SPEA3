//---------------------------------------------------------------------------
#ifndef IAIEGeneticAlgorithmH
#define IAIEGeneticAlgorithmH
//---------------------------------------------------------------------------
class IAIERandomGenerator64;
class IAIEGeneticAlgorithm;
//---------------------------------------------------------------------------
typedef void (__stdcall *TAIEGAChromosomeInit)(IAIEGeneticAlgorithm* GA, void*& ChromosomeHandle, int NodeIndex, int UnitIndex, int ThreadIndex);
typedef void (__stdcall *TAIEGAChromosomeFree)(IAIEGeneticAlgorithm* GA, void* ChromosomeHandle);
typedef void (__stdcall *TAIEGAChromosomeRand)(IAIEGeneticAlgorithm* GA, void* ChromosomeHandle);
typedef void (__stdcall *TAIEGAMutationOperator)(IAIEGeneticAlgorithm* GA, void* ChromosomeHandle);
typedef void (__stdcall *TAIEGACrossOverOperator)(IAIEGeneticAlgorithm* GA, void* ChromosomeHandle1, void* ChromosomeHandle2);
typedef void (__stdcall *TAIEGAChromosomeCopy)(IAIEGeneticAlgorithm* GA, void* DstChromosomeHandle, void* SrcChromosomeHandle);
typedef double (__stdcall *TAIEGAFitnessFunction)(IAIEGeneticAlgorithm* GA, void* ChromosomeHandle);
//---------------------------------------------------------------------------
class IAIEGeneticAlgorithm
{
 public:
		virtual void __stdcall Prepare(int Seed) = 0;
		virtual void __stdcall FirstGeneration(void) = 0;
		virtual void __stdcall NextGeneration(void) = 0;
		virtual void __stdcall Clear(void) = 0;

		virtual bool __stdcall GetIsPrepared(void) = 0;
		virtual bool __stdcall GetIsExecuting(void) = 0;
		virtual bool __stdcall GetIsFinished(void) = 0;

		virtual int __stdcall GetSelectionMethod(void) = 0;
		virtual void __stdcall SetSelectionMethod(int ASelectionMethod) = 0;
		virtual int __stdcall GetPopulationSize(void) = 0;
		virtual void __stdcall SetPopulationSize(int APopulationSize) = 0;
		virtual int __stdcall GetSelectionPressure(void) = 0;
		virtual void __stdcall SetSelectionPressure(int ASelectionPressure) = 0;
		virtual double __stdcall GetCrossOverProbability(void) = 0;
		virtual void __stdcall SetCrossOverProbability(double ACrossOverProbability) = 0;
		virtual double __stdcall GetMutationProbability(void) = 0;
		virtual void __stdcall SetMutationProbability(double AMutationProbability) = 0;
		virtual bool __stdcall GetElitistStrategy(void) = 0;
		virtual void __stdcall SetElitistStrategy(bool AElitistStrategy) = 0;

		virtual IAIERandomGenerator64* __stdcall GetRandomGenerator(void) = 0;

		virtual int __stdcall GetChromosomeCount(void) = 0;
		virtual void* __stdcall GetChromosome(int AIndex) = 0;
		virtual void* __stdcall GetBestChromosome(void) = 0;

		virtual double __stdcall GetCurrentFitnessValue(int AIndex) = 0;
		virtual double __stdcall GetCurrentMaxFitnessValue(void) = 0;
		virtual double __stdcall GetCurrentAvgFitnessValue(void) = 0;
		virtual double __stdcall GetCurrentMinFitnessValue(void) = 0;

		virtual void* __stdcall GetData(void) = 0;
		virtual void __stdcall SetData(void* AData) = 0;

		virtual TAIEGAChromosomeInit __stdcall GetChromosomeInit(void) = 0;
		virtual void __stdcall SetChromosomeInit(TAIEGAChromosomeInit AChromosomeInit) = 0;
		virtual TAIEGAChromosomeRand __stdcall GetChromosomeRand(void) = 0;
		virtual void __stdcall SetChromosomeRand(TAIEGAChromosomeRand AChromosomeRand) = 0;
		virtual TAIEGAChromosomeCopy __stdcall GetChromosomeCopy(void) = 0;
		virtual void __stdcall SetChromosomeCopy(TAIEGAChromosomeCopy AChromosomeCopy) = 0;
		virtual TAIEGAChromosomeFree __stdcall GetChromosomeFree(void) = 0;
		virtual void __stdcall SetChromosomeFree(TAIEGAChromosomeFree AChromosomeFree) = 0;
		virtual TAIEGACrossOverOperator __stdcall GetCrossOverOperator(void) = 0;
		virtual void __stdcall SetCrossOverOperator(TAIEGACrossOverOperator ACrossOverOperator) = 0;
		virtual TAIEGAMutationOperator __stdcall GetMutationOperator(void) = 0;
		virtual void __stdcall SetMutationOperator(TAIEGAMutationOperator AMutationOperator) = 0;
		virtual TAIEGAFitnessFunction __stdcall GetFitnessFunction(void) = 0;
		virtual void __stdcall SetFitnessFunction(TAIEGAFitnessFunction AFitnessFunction) = 0;

 #ifdef __BORLANDC__
		__property bool IsPrepared = {read=GetIsPrepared};
		__property bool IsExecuting = {read=GetIsExecuting};
		__property bool IsFinished = {read=GetIsFinished};

		__property int SelectionMethod = {read=GetSelectionMethod, write=SetSelectionMethod};
		__property int PopulationSize = {read=GetPopulationSize, write=SetPopulationSize};
		__property int SelectionPressure = {read=GetSelectionPressure, write=SetSelectionPressure};
		__property double CrossOverProbability = {read=GetCrossOverProbability, write=SetCrossOverProbability};
		__property double MutationProbability = {read=GetMutationProbability, write=SetMutationProbability};
		__property bool ElitistStrategy = {read=GetElitistStrategy, write=SetElitistStrategy};

		__property IAIERandomGenerator64* RandomGenerator = {read=GetRandomGenerator};

		__property int ChromosomeCount = {read=GetChromosomeCount};
		__property void* Chromosome[int Index] = {read=GetChromosome};
		__property void* BestChromosome = {read=GetBestChromosome};

		__property double CurrentFitnessValue[int Index] = {read=GetCurrentFitnessValue};
		__property double CurrentMaxFitnessValue = {read=GetCurrentMaxFitnessValue};
		__property double CurrentAvgFitnessValue = {read=GetCurrentAvgFitnessValue};
		__property double CurrentMinFitnessValue = {read=GetCurrentMinFitnessValue};

		__property void* Data = {read=GetData, write=SetData};

		__property TAIEGAChromosomeInit ChromosomeInit = {read=GetChromosomeInit, write=SetChromosomeInit};
		__property TAIEGAChromosomeRand ChromosomeRand = {read=GetChromosomeRand, write=SetChromosomeRand};
		__property TAIEGAChromosomeCopy ChromosomeCopy = {read=GetChromosomeCopy, write=SetChromosomeCopy};
		__property TAIEGAChromosomeFree ChromosomeFree = {read=GetChromosomeFree, write=SetChromosomeFree};
		__property TAIEGACrossOverOperator CrossOverOperator = {read=GetCrossOverOperator, write=SetCrossOverOperator};
		__property TAIEGAMutationOperator MutationOperator = {read=GetMutationOperator, write=SetMutationOperator};
		__property TAIEGAFitnessFunction FitnessFunction = {read=GetFitnessFunction, write=SetFitnessFunction};

 #endif
};
//---------------------------------------------------------------------------
#endif
