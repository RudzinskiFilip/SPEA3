//---------------------------------------------------------------------------
#ifndef UAIEMOGABaseH
#define UAIEMOGABaseH
//---------------------------------------------------------------------------
#include "UTemplates.h"
#include "UDynamicPointers.h"
#include "IAIEMultiobjectiveGeneticAlgorithm.h"
//---------------------------------------------------------------------------
class TAIEGeneticAlgorithm;

//---------------------------------------------------------------------------
//typedef int (__stdcall *TAIEMOGASolutionObjectiveCount)(void);
//typedef int (__stdcall *TAIEMOGASolutionParamCount)(void);
//typedef const char* (__stdcall *TAIEMOGASolutionObjectiveName)(int AObjectiveIndex);
//typedef const char* (__stdcall *TAIEMOGASolutionParamName)(int AParamIndex);
//typedef double (__stdcall *TAIEMOGASolutionParamValue)(void*& ASolutionHandle, int AParamIndex);
//---------------------------------------------------------------------------
class TAIEMultiobjectiveGeneticSolution : public IAIEMultiobjectiveGeneticSolution
{
 friend class TAIEMultiobjectiveGeneticAlgorithm;
 friend class TAIEMultiobjectiveGeneticAlgorithmVEGA;
 friend class TAIEMultiobjectiveGeneticAlgorithmSPEA;
 friend class TAIEMultiobjectiveGeneticAlgorithmSPEA2;
 friend class TAIEMultiobjectiveGeneticAlgorithmNSGA;
 friend class TAIEMultiobjectiveGeneticAlgorithmNSGA2;
 friend class TAIEMultiobjectiveGeneticAlgorithmENSGA2;
 friend class TAIEMultiobjectiveGeneticAlgorithmSPEA3;

 private:
		TAIEMultiobjectiveGeneticAlgorithm* FOwner;

		TVector<double> FObjectiveValues;
		int     FFrontIndex;
		double  FFitnessValue;

		int		FRank;              //pozycja w rankingu (dla procedury FastNondominatedSort)
		double  FStrength;			//dla algorytmu SPEA2
		double  FDistance;			//dystans dla algorytmu NSGA2 i SPEA2
		void*   FData;

		TQuickList<TAIEMultiobjectiveGeneticSolution*> FDominatingSolutions;    //rozwi¹zania dominuj¹ce nad danym rozwi¹zaniem
		TQuickList<TAIEMultiobjectiveGeneticSolution*> FDominatedSolutions;     //rozwi¹zania zdominowane przez dane rozwi¹zanie

 private:
		__stdcall TAIEMultiobjectiveGeneticSolution(TAIEMultiobjectiveGeneticAlgorithm* AOwner, int AObjectiveCount);
		__stdcall TAIEMultiobjectiveGeneticSolution(TAIEMultiobjectiveGeneticSolution& ASource);
		__stdcall ~TAIEMultiobjectiveGeneticSolution(void);

 public:
		double __stdcall GetObjectiveValue(int AObjectiveIndex);
		double __stdcall GetFitnessValue(void);
		int    __stdcall GetFrontIndex(void);
		double __stdcall GetDistance(void);
		int	   __stdcall GetRank(void);
		void*  __stdcall GetData(void);

		int    __stdcall GetDominatingSolutionCount(void);
		TAIEMultiobjectiveGeneticSolution* __stdcall GetDominatingSolution(int ASolutionIndex);

		int    __stdcall GetDominatedSolutionCount(void);
		TAIEMultiobjectiveGeneticSolution* __stdcall GetDominatedSolution(int ASolutionIndex);

		bool __stdcall Dominates(TAIEMultiobjectiveGeneticSolution* ASolution);
		bool __stdcall DominatesOrEquals(TAIEMultiobjectiveGeneticSolution* ASolution);
		bool __stdcall Equals(TAIEMultiobjectiveGeneticSolution* ASolution);

		bool __stdcall EpsilonDominates(TAIEMultiobjectiveGeneticSolution* ASolution);
		bool __stdcall EpsilonDominatesOrEquals(TAIEMultiobjectiveGeneticSolution* ASolution);

		bool __stdcall IsDominated(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);
		bool __stdcall IsDominatedOrEqual(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);

		bool __stdcall IsEpsilonDominated(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);
		bool __stdcall IsEpsilonDominatedOrEqual(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);
};
//---------------------------------------------------------------------------
class TAIEMultiobjectiveGeneticAlgorithm : public IAIEMultiobjectiveGeneticAlgorithm
{
 private:
 		void* FData;
		bool FIsPrepared;
		bool FIsExecuted;

		TQuickList<double> FObjectiveMaxValues;
		TQuickList<double> FObjectiveAvgValues;
		TQuickList<double> FObjectiveMinValues;

		TAIEMOGASolutionInit FSolutionInit;
		TAIEMOGASolutionRand FSolutionRand;
		TAIEMOGASolutionCopy FSolutionCopy;
		TAIEMOGASolutionFree FSolutionFree;
		TAIEMOGACrossoverOperator FCrossoverOperator;
		TAIEMOGAMutationOperator  FMutationOperator;
		TAIEMOGAFitnessFunction FFitnessFunction;

 protected:
		int FObjectiveCount;
		int FPopulationSize;
		int FSelectionPressure;
		double FCrossoverProbability;
		double FMutationProbability;
		int FGenerationIndex;

		TQuickList<double> FFinalObjectiveMaxValues;
		TQuickList<double> FFinalObjectiveAvgValues;
		TQuickList<double> FFinalObjectiveMinValues;

		TQuickList<TAIEMultiobjectiveGeneticSolution*> FPopulation;
		TQuickList< TQuickList<TAIEMultiobjectiveGeneticSolution*>* > FFronts;

		void __stdcall EvaluateSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);
		void __stdcall EvaluateMinMaxAvg(void);
		void __stdcall RandomizeSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);
		void __stdcall CopySolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* ADstPopulation,
									 TQuickList<TAIEMultiobjectiveGeneticSolution*>* ASrcPopulation);
		void __stdcall CopyNondominatedSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* ADstPopulation,
												 TQuickList<TAIEMultiobjectiveGeneticSolution*>* ASrcPopulation);
		void __stdcall ClearSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);
		void __stdcall ClearDominatedSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);
		void __stdcall ClearDominatedOrEqualSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);
		void __stdcall ClearEpsilonDominatedSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);
		void __stdcall ClearEpsilonDominatedOrEqualSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);

		void __stdcall CrossSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);
		void __stdcall MutateSolutions(TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation);

		TAIEMultiobjectiveGeneticSolution* __stdcall SolutionNew(void);
		TAIEMultiobjectiveGeneticSolution* __stdcall SolutionClone(TAIEMultiobjectiveGeneticSolution* ASolution);
		void __stdcall SolutionDelete(TAIEMultiobjectiveGeneticSolution* ASolution);

		double __stdcall DistanceBetweenSolutions(TAIEMultiobjectiveGeneticSolution* ASolution1, TAIEMultiobjectiveGeneticSolution* ASolution2);
		double __stdcall NormalizedDistanceBetweenSolutions(TAIEMultiobjectiveGeneticSolution* ASolution1, TAIEMultiobjectiveGeneticSolution* ASolution2,
															TQuickList<double>* AObjectiveMaxValues, TQuickList<double>* AObjectiveMinValues);

		void __stdcall FastNondominatedSort(TQuickList< TAIEMultiobjectiveGeneticSolution* >* APopulation,
											TQuickList< TQuickList<TAIEMultiobjectiveGeneticSolution*>* >* AFronts);

 protected:
		virtual void __stdcall ExecuteFirstGeneration(void);
		virtual void __stdcall ExecuteNextGeneration(void);

 public:
		__stdcall TAIEMultiobjectiveGeneticAlgorithm(void);
		virtual __stdcall ~TAIEMultiobjectiveGeneticAlgorithm(void);

		bool __stdcall Prepare(int Seed);
		bool __stdcall FirstGeneration(void);
		bool __stdcall NextGeneration(void);
		bool __stdcall Clear(void);

		bool __stdcall GetIsPrepared(void);
		bool __stdcall GetIsExecuted(void);

		int __stdcall GetSelectionMethod(void);
		void __stdcall SetSelectionMethod(int ASelectionMethod);
		int __stdcall GetSelectionPressure(void);
		void __stdcall SetSelectionPressure(int ASelectionPressure);
		int __stdcall GetPopulationSize(void);
		void __stdcall SetPopulationSize(int APopulationSize);
		virtual int __stdcall GetFrontMaxSize(void);
		virtual void __stdcall SetFrontMaxSize(int AFrontMaxSize);
		double __stdcall GetCrossoverProbability(void);
		void __stdcall SetCrossoverProbability(double ACrossoverProbability);
		double __stdcall GetMutationProbability(void);
		void __stdcall SetMutationProbability(double AMutationProbability);

		int __stdcall GetSolutionCount(void);
		IAIEMultiobjectiveGeneticSolution* __stdcall GetSolution(int ASolutionIndex);

		virtual int __stdcall GetFinalSolutionCount(void);
		virtual IAIEMultiobjectiveGeneticSolution* __stdcall GetFinalSolution(int ASolutionIndex);

		int __stdcall GetFrontCount(void);
		int __stdcall GetFrontSolutionCount(int AFrontIndex);
		IAIEMultiobjectiveGeneticSolution* __stdcall GetFrontSolution(int AFrontIndex, int ASolutionIndex);

		int __stdcall GetObjectiveCount(void);
		void __stdcall SetObjectiveCount(int AObjectiveCount);

		double __stdcall GetObjectiveMaxValue(int AObjectiveIndex);
		double __stdcall GetObjectiveAvgValue(int AObjectiveIndex);
		double __stdcall GetObjectiveMinValue(int AObjectiveIndex);
		virtual double __stdcall GetObjectiveEpsilon(int AObjectiveIndex, double AValue);

		double __stdcall GetFinalObjectiveMaxValue(int AObjectiveIndex);
		double __stdcall GetFinalObjectiveAvgValue(int AObjectiveIndex);
		double __stdcall GetFinalObjectiveMinValue(int AObjectiveIndex);

		int __stdcall  GetGenerationIndex(void);
		double __stdcall GetDiversityFactor(void);

		void* __stdcall GetData(void);
		void __stdcall SetData(void* AData);

		void __stdcall SetSolutionInit(TAIEMOGASolutionInit ASolutionInit);
		void __stdcall SetSolutionRand(TAIEMOGASolutionRand ASolutionRand);
		void __stdcall SetSolutionCopy(TAIEMOGASolutionCopy ASolutionCopy);
		void __stdcall SetSolutionFree(TAIEMOGASolutionFree ASolutionFree);
		void __stdcall SetCrossoverOperator(TAIEMOGACrossoverOperator ACrossoverOperator);
		void __stdcall SetMutationOperator(TAIEMOGAMutationOperator AMutationOperator);
		void __stdcall SetFitnessFunction(TAIEMOGAFitnessFunction AFitnessFunction);
};
//---------------------------------------------------------------------------
#endif
