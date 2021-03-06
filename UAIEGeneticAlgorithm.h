//---------------------------------------------------------------------------
#ifndef UAIEGeneticAlgorithmH
#define UAIEGeneticAlgorithmH
//---------------------------------------------------------------------------
#include "IAIEGeneticAlgorithm.h"
//---------------------------------------------------------------------------
struct TAIEGeneticChromosome
{
 void*  Data;				//dane zewnętrzne chromosomu
 int    Index;				//indeks położenia w zbiorczej tablicy chromosomów
 double FitnessValue;		//wartość funkcji przystosowania
 int    FitnessID;			//identyfikator chromosomu (wartość 0 oznacza konieczność przeliczenia FitnessValue)
};
//---------------------------------------------------------------------------
class TAIEGeneticAlgorithm : virtual public IAIEGeneticAlgorithm
{
 private:
		TAIESystem* FSystem;
		IAIENumaThreads* FThreads;
		bool FIsPrepared;
		bool FIsExecuting;
		bool FIsFinished;
		TAIERandomGenerator64* FRandomGenerator;
		TAIERandomPermutation* FRandomPermutation;
		void* FData;

		// - wartości dopasowania chromosomów
		double FMaxFitnessValue;    //dopasowanie najlepszego chromosomu
		double FAvgFitnessValue;   //dopasowanie średnie chromosomów
		double FMinFitnessValue;   //dopasowanie najgorszego chromosomu
		int FMaxFitnessValueIndex;
		int FMinFitnessValueIndex;

		// - pomocnicze dane
		TVector<TAIEGeneticChromosome*> FChromosomeObjects;  //tablica wszystkich chromosomów (aktualna tylko w trakcie pracy algorytmu)
		TVector<TAIEGeneticChromosome*> FRankTable;	//pomocnicza tablica chromosomów dla rankingowej metody selekcji
		TVector<int>    FChromosomeSelctionCount;  	//liczba kopii poszczególnych chromosomów w populacji potomków
		TVector<double> FRouletteFitnessTable;  	//dodatnie wartości względnego dopasowania chromosomów (tylko dla selekcji metodą ruletki)
		TAIEGeneticChromosome FBestChromosome;		//kontener najlepszego chromosomu (tylko dla strategii elitarnej)
		int FFitnessID;								//licznik identyfikatorów chromosomów

		//PARAMETRY ALGORYTMU
		int  FSelectionMethod;
		int  FPopulationSize;
		int  FSelectionPressure;
		double FCrossOverProbability;
		double FMutationProbability;
		bool FElitistStrategy;

		// WSKAŹNIKI DO PROCEDUR OPERUJĄCYCH NA CHROMOSOMACH
		TAIEGAChromosomeInit FChromosomeInit;
		TAIEGAChromosomeRand FChromosomeRand;
		TAIEGAChromosomeCopy FChromosomeCopy;
		TAIEGAChromosomeFree FChromosomeFree;
		TAIEGACrossOverOperator FCrossoverOperator;
		TAIEGAMutationOperator  FMutationOperator;
		TAIEGAFitnessFunction FFitnessFunction;

		void __stdcall ComputeFitnessFunction(void);
		void __stdcall PreselectionRoulette(void);
		void __stdcall PreselectionTournament(void);
		void __stdcall PreselectionRank(void);
		void __stdcall PostselectionChromosomes(void);
		void __stdcall SelectNewChromosomes(void);
		void __stdcall CrossoverChromosomes(void);
		void __stdcall MutateChromosomes(void);

 public:
		__stdcall TAIEGeneticAlgorithm(TAIESystem* ASystem);
		__stdcall ~TAIEGeneticAlgorithm(void);
		void __stdcall Free(void);

		void __stdcall Prepare(int Seed);
		void __stdcall FirstGeneration(void);
		void __stdcall NextGeneration(void);
		void __stdcall Clear(void);

		bool __stdcall GetIsPrepared(void);
		bool __stdcall GetIsExecuting(void);
		bool __stdcall GetIsFinished(void);

		int __stdcall GetSelectionMethod(void);
		void __stdcall SetSelectionMethod(int ASelectionMethod);
		int __stdcall GetPopulationSize(void);
		void __stdcall SetPopulationSize(int APopulationSize);
		int __stdcall GetSelectionPressure(void);
		void __stdcall SetSelectionPressure(int ASelectionPressure);
		double __stdcall GetCrossOverProbability(void);
		void __stdcall SetCrossOverProbability(double ACrossOverProbability);
		double __stdcall GetMutationProbability(void);
		void __stdcall SetMutationProbability(double AMutationProbability);
		bool __stdcall GetElitistStrategy(void);
		void __stdcall SetElitistStrategy(bool AElitistStrategy);

		IAIERandomGenerator64* __stdcall GetRandomGenerator(void);

		int __stdcall GetChromosomeCount(void);
		void* __stdcall GetChromosome(int AIndex);
		void* __stdcall GetBestChromosome(void);

		double __stdcall GetCurrentFitnessValue(int AIndex);
		double __stdcall GetCurrentMaxFitnessValue(void);
		double __stdcall GetCurrentAvgFitnessValue(void);
		double __stdcall GetCurrentMinFitnessValue(void);

		void* __stdcall GetData(void);
		void __stdcall SetData(void* AData);

		TAIEGAChromosomeInit __stdcall GetChromosomeInit(void);
		void __stdcall SetChromosomeInit(TAIEGAChromosomeInit AChromosomeInit);
		TAIEGAChromosomeRand __stdcall GetChromosomeRand(void);
		void __stdcall SetChromosomeRand(TAIEGAChromosomeRand AChromosomeRand);
		TAIEGAChromosomeCopy __stdcall GetChromosomeCopy(void);
		void __stdcall SetChromosomeCopy(TAIEGAChromosomeCopy AChromosomeCopy);
		TAIEGAChromosomeFree __stdcall GetChromosomeFree(void);
		void __stdcall SetChromosomeFree(TAIEGAChromosomeFree AChromosomeFree);
		TAIEGACrossOverOperator __stdcall GetCrossOverOperator(void);
		void __stdcall SetCrossOverOperator(TAIEGACrossOverOperator ACrossOverOperator);
		TAIEGAMutationOperator __stdcall GetMutationOperator(void);
		void __stdcall SetMutationOperator(TAIEGAMutationOperator AMutationOperator);
		TAIEGAFitnessFunction __stdcall GetFitnessFunction(void);
		void __stdcall SetFitnessFunction(TAIEGAFitnessFunction AFitnessFunction);
};
//---------------------------------------------------------------------------
#endif
