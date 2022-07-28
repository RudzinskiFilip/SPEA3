//---------------------------------------------------------------------------
#ifndef IAIEMultiobjectiveGeneticAlgorithmH
#define IAIEMultiobjectiveGeneticAlgorithmH
//---------------------------------------------------------------------------
class IAIERandomGenerator64;
class IAIEMultiobjectiveGeneticAlgorithm;
//---------------------------------------------------------------------------
typedef void (__stdcall *TAIEMOGASolutionInit)(IAIEMultiobjectiveGeneticAlgorithm* GA, void*& ChromosomeHandle);
typedef void (__stdcall *TAIEMOGASolutionRand)(IAIEMultiobjectiveGeneticAlgorithm* GA, void* ChromosomeHandle);
typedef void (__stdcall *TAIEMOGASolutionCopy)(IAIEMultiobjectiveGeneticAlgorithm* GA, void* DstChromosomeHandle, void* SrcChromosomeHandle);
typedef void (__stdcall *TAIEMOGASolutionFree)(IAIEMultiobjectiveGeneticAlgorithm* GA, void* ChromosomeHandle);
typedef void (__stdcall *TAIEMOGAMutationOperator)(IAIEMultiobjectiveGeneticAlgorithm* GA, void* ChromosomeHandle);
typedef void (__stdcall *TAIEMOGACrossoverOperator)(IAIEMultiobjectiveGeneticAlgorithm* GA, void* ChromosomeHandle1, void* ChromosomeHandle2);
typedef void (__stdcall *TAIEMOGAFitnessFunction)(IAIEMultiobjectiveGeneticAlgorithm* GA, void* ChromosomeHandle, int ParetoObjectiveCount, double* ParetoObjectiveValues);
//---------------------------------------------------------------------------
class IAIEMultiobjectiveGeneticSolution
{
 public:
		virtual double __stdcall GetObjectiveValue(int AObjectiveIndex) = 0;
		virtual double __stdcall GetFitnessValue(void) = 0;
		virtual int    __stdcall GetFrontIndex(void) = 0;
		virtual void*  __stdcall GetData(void) = 0;

		virtual int    __stdcall GetDominatingSolutionCount(void) = 0;
		virtual IAIEMultiobjectiveGeneticSolution* __stdcall GetDominatingSolution(int ASolutionIndex) = 0;

		virtual int    __stdcall GetDominatedSolutionCount(void) = 0;
		virtual IAIEMultiobjectiveGeneticSolution* __stdcall GetDominatedSolution(int ASolutionIndex) = 0;
};
//---------------------------------------------------------------------------
class IAIEMultiobjectiveGeneticAlgorithm
{
 public:
		virtual bool __stdcall Prepare(int Seed) = 0;
		virtual bool __stdcall FirstGeneration(void) = 0;
		virtual bool __stdcall NextGeneration(void) = 0;
		virtual bool __stdcall Clear(void) = 0;

		virtual bool __stdcall GetIsPrepared(void) = 0;
		virtual bool __stdcall GetIsExecuted(void) = 0;

		virtual int __stdcall GetSelectionMethod(void) = 0;
		virtual void __stdcall SetSelectionMethod(int ASelectionMethod) = 0;
		virtual int __stdcall GetSelectionPressure(void) = 0;
		virtual void __stdcall SetSelectionPressure(int ASelectionPressure) = 0;
		virtual int __stdcall GetPopulationSize(void) = 0;
		virtual void __stdcall SetPopulationSize(int APopulationSize) = 0;
		virtual int __stdcall GetFrontMaxSize(void) = 0;
		virtual void __stdcall SetFrontMaxSize(int AFrontMaxSize) = 0;
		virtual double __stdcall GetCrossoverProbability(void) = 0;
		virtual void __stdcall SetCrossoverProbability(double ACrossOverProbability) = 0;
		virtual double __stdcall GetMutationProbability(void) = 0;
		virtual void __stdcall SetMutationProbability(double AMutationProbability) = 0;

		virtual int __stdcall GetSolutionCount(void) = 0;
		virtual IAIEMultiobjectiveGeneticSolution* __stdcall GetSolution(int ASolutionIndex) = 0;

		virtual int __stdcall GetFinalSolutionCount(void) = 0;
		virtual IAIEMultiobjectiveGeneticSolution* __stdcall GetFinalSolution(int ASolutionIndex) = 0;

		virtual int __stdcall GetFrontCount(void) = 0;
		virtual int __stdcall GetFrontSolutionCount(int AFrontIndex) = 0;
		virtual IAIEMultiobjectiveGeneticSolution* __stdcall GetFrontSolution(int AFrontIndex, int ASolutionIndex) = 0;

		virtual int __stdcall GetObjectiveCount(void) = 0;
		virtual void __stdcall SetObjectiveCount(int AObjectiveCount) = 0;

		virtual double __stdcall GetObjectiveMaxValue(int AObjectiveIndex) = 0;
		virtual double __stdcall GetObjectiveAvgValue(int AObjectiveIndex) = 0;
		virtual double __stdcall GetObjectiveMinValue(int AObjectiveIndex) = 0;

		virtual double __stdcall GetFinalObjectiveMaxValue(int AObjectiveIndex) = 0;
		virtual double __stdcall GetFinalObjectiveAvgValue(int AObjectiveIndex) = 0;
		virtual double __stdcall GetFinalObjectiveMinValue(int AObjectiveIndex) = 0;

		virtual double __stdcall GetDiversityFactor(void) = 0;

		virtual void* __stdcall GetData(void) = 0;
		virtual void __stdcall SetData(void* AData) = 0;

		//virtual TAIEMOGAChromosomeInit __stdcall GetChromosomeInit(void) = 0;
		virtual void __stdcall SetSolutionInit(TAIEMOGASolutionInit ASolutionInit) = 0;
		//virtual TAIEMOGAChromosomeRand __stdcall GetChromosomeRand(void) = 0;
		virtual void __stdcall SetSolutionRand(TAIEMOGASolutionRand ASolutionRand) = 0;
		//virtual TAIEMOGAChromosomeCopy __stdcall GetChromosomeCopy(void) = 0;
		virtual void __stdcall SetSolutionCopy(TAIEMOGASolutionCopy ASolutionCopy) = 0;
		//virtual TAIEMOGAChromosomeFree __stdcall GetChromosomeFree(void) = 0;
		virtual void __stdcall SetSolutionFree(TAIEMOGASolutionFree ASolutionFree) = 0;
		//virtual TAIEMOGACrossOverOperator __stdcall GetCrossOverOperator(void) = 0;
		virtual void __stdcall SetCrossoverOperator(TAIEMOGACrossoverOperator ACrossOverOperator) = 0;
		//virtual TAIEMOGAMutationOperator __stdcall GetMutationOperator(void) = 0;
		virtual void __stdcall SetMutationOperator(TAIEMOGAMutationOperator AMutationOperator) = 0;
		//virtual TAIEMOGAFitnessFunction __stdcall GetFitnessFunction(void) = 0;
		virtual void __stdcall SetFitnessFunction(TAIEMOGAFitnessFunction AFitnessFunction) = 0;

 #ifdef __BORLANDC__
		__property bool IsPrepared = {read=GetIsPrepared};
		__property bool IsExecuted = {read=GetIsExecuted};

		__property int SelectionMethod = {read=GetSelectionMethod, write=SetSelectionMethod};
		__property int SelectionPressure = {read=GetSelectionPressure, write=SetSelectionPressure};
		__property int PopulationSize = {read=GetPopulationSize, write=SetPopulationSize};
		__property int FrontMaxSize = {read=GetFrontMaxSize, write=SetFrontMaxSize};
		__property double CrossOverProbability = {read=GetCrossoverProbability, write=SetCrossoverProbability};
		__property double MutationProbability = {read=GetMutationProbability, write=SetMutationProbability};

		__property int SolutionCount = {read=GetSolutionCount};
		__property IAIEMultiobjectiveGeneticSolution* Solution[int SolutionIndex] = {read=GetSolution};

		__property int FinalSolutionCount = {read=GetFinalSolutionCount};
		__property IAIEMultiobjectiveGeneticSolution* FinalSolution[int SolutionIndex] = {read=GetFinalSolution};

		__property int FrontCount = {read=GetFrontCount};
		__property int FrontSolutionCount[int FrontIndex] = {read=GetFrontSolutionCount};
		__property IAIEMultiobjectiveGeneticSolution* FrontSolution[int AFrontIndex][int ASolutionIndex] = {read=GetFrontSolution};

		__property int ObjectiveCount = {read=GetObjectiveCount, write=SetObjectiveCount};

		__property double ObjectiveMaxValue[int ObjectiveIndex] = {read=GetObjectiveMaxValue};
		__property double ObjectiveAvgValue[int ObjectiveIndex] = {read=GetObjectiveAvgValue};
		__property double ObjectiveMinValue[int ObjectiveIndex] = {read=GetObjectiveMinValue};

		__property double FinalObjectiveMaxValue[int ParetoIndex] = {read=GetFinalObjectiveMaxValue};
		__property double FinalObjectiveAvgValue[int ParetoIndex] = {read=GetFinalObjectiveAvgValue};
		__property double FinalObjectiveMinValue[int ParetoIndex] = {read=GetFinalObjectiveMinValue};

		__property void* Data = {read=GetData, write=SetData};

		//__property TAIEMOGAChromosomeInit ChromosomeInit = {read=GetChromosomeInit, write=SetChromosomeInit};
		//__property TAIEMOGAChromosomeRand ChromosomeRand = {read=GetChromosomeRand, write=SetChromosomeRand};
		//__property TAIEMOGAChromosomeCopy ChromosomeCopy = {read=GetChromosomeCopy, write=SetChromosomeCopy};
		//__property TAIEMOGAChromosomeFree ChromosomeFree = {read=GetChromosomeFree, write=SetChromosomeFree};
		//__property TAIEMOGACrossOverOperator CrossOverOperator = {read=GetCrossOverOperator, write=SetCrossOverOperator};
		//__property TAIEMOGAMutationOperator MutationOperator = {read=GetMutationOperator, write=SetMutationOperator};
		//__property TAIEMOGAFitnessFunction FitnessFunction = {read=GetFitnessFunction, write=SetFitnessFunction};

 #endif
};
//---------------------------------------------------------------------------
#endif
