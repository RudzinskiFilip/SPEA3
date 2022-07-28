//---------------------------------------------------------------------------
#include <windows.h>

#include "UTemplates.h"
#include "UDynamicPointers.h"
#include "UAIESystem.h"
#include "UAIEBinaryFile.h"
#include "UAIEComputingTimer.h"
#include "UAIEStopConditions.h"
#include "UAIERandomPermutation.h"
#include "UAIERandomGenerator64.h"
#include "UAIEInfo.h"
#include "UAIENumaDevice.h"
#include "UAIENumaNode.h"
#include "UAIENumaUnit.h"
#include "UAIENumaThreads.h"

#include "UAIEGeneticAlgorithm.h"
//---------------------------------------------------------------------------
struct TAIENodeData
{
 TAIEGeneticAlgorithm* GA;

 int     ChromosomeCount;		//liczba chromosom�w przetwarzanych przez w�ze�
 int*    ChromosomeIndexMin;	//zakresy indeks�w dla poszczeg�lnych w�tk�w
 int*    ChromosomeIndexMax;
 TAIEGeneticChromosome* ChromosomeObjects;		//tabela wska�nik�w do chromosom�w przetwarzanych przez w�ze�
 TAIEGeneticChromosome* ChromosomeParents;		//tabela wska�nik�w do chromosom�w rodzicielskich przetwarzanych przez w�ze�

 TAIEGAChromosomeInit ChromosomeInit;	//wska�niki do procedur, kt�re mog� by� uruchamiane r�wnolegle
 TAIEGAChromosomeCopy ChromosomeCopy;
 TAIEGAChromosomeFree ChromosomeFree;
 TAIEGAFitnessFunction FitnessFunction;
};
//---------------------------------------------------------------------------
// Procedura wielow�tkowa uruchamiana na pocz�tku procesu ewolucji dla ka�dego w�z�a
//---------------------------------------------------------------------------
void __stdcall NumaNodePrepare(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 //PROCEDURA URUCHAMIANA DLA W�Z��W NA POCZ�TKU PROCESU EWOLUCJI (tworzenie danych dla w�z��w)
 // - odczytanie pomocniczych wska�nik�w i utworzenie struktur danych
 TAIENodeData* LNodeData = (TAIENodeData*)AThreads->LocalAllocMemory(ANodeIndex, sizeof(TAIENodeData));
 AThreads->SetNodeData(ANodeIndex, LNodeData);

 // - kopiowanie podtawowych danych
 TAIEGeneticAlgorithm* GA = (TAIEGeneticAlgorithm*)AThreads->GetData();
 LNodeData->GA = GA;

 int PopulationSize = GA->GetPopulationSize();
 LNodeData->ChromosomeInit = GA->GetChromosomeInit();
 LNodeData->ChromosomeCopy = GA->GetChromosomeCopy();
 LNodeData->ChromosomeFree = GA->GetChromosomeFree();
 LNodeData->FitnessFunction = GA->GetFitnessFunction();

 // - ustalenie liczby chromosom�w dla w�z�a
 int LNodeCount = AThreads->GetNumaDevice()->GetNumaNodeCount();
 int LUnitCount = AThreads->GetNumaDevice()->GetNumaNode(ANodeIndex)->GetNumaUnitCount();
 LNodeData->ChromosomeCount = PopulationSize / LNodeCount;
 if (ANodeIndex == LNodeCount - 1) LNodeData->ChromosomeCount += PopulationSize % LNodeCount;

 // - tworzenie cz�ciowej populacji chromosom�w i generator�w losowych
 LNodeData->ChromosomeObjects = (TAIEGeneticChromosome*)AThreads->LocalAllocMemory(ANodeIndex, LNodeData->ChromosomeCount * sizeof(TAIEGeneticChromosome));
 LNodeData->ChromosomeParents = (TAIEGeneticChromosome*)AThreads->LocalAllocMemory(ANodeIndex, LNodeData->ChromosomeCount * sizeof(TAIEGeneticChromosome));

 // - ustalenie zakresu indeks�w chromosom�w przetwarzanych przez kolejne w�tki w w�le
 LNodeData->ChromosomeIndexMin = (int*)AThreads->LocalAllocMemory(ANodeIndex, LUnitCount * sizeof(int));
 LNodeData->ChromosomeIndexMax = (int*)AThreads->LocalAllocMemory(ANodeIndex, LUnitCount * sizeof(int));
 for (int i = 0; i < LUnitCount; ++i) {
	  int LChrmCount = LNodeData->ChromosomeCount / LUnitCount;
	  LNodeData->ChromosomeIndexMin[i] = LChrmCount * i;
	  if (i == LUnitCount - 1) LChrmCount += LNodeData->ChromosomeCount % LUnitCount;
	  LNodeData->ChromosomeIndexMax[i] = LNodeData->ChromosomeIndexMin[i] + LChrmCount - 1;
 }
}
//---------------------------------------------------------------------------
// Procedura wielow�tkowa uruchamiana po zako�czeniu procesu ewolucji dla ka�dego w�z�a
//---------------------------------------------------------------------------
void __stdcall NumaNodeFinish(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 //PROCEDURA URUCHAMIANA DLA W�Z��W NA KO�CU PROCESU EWOLUCJI (usuwanie danych z w�z��w)
 // - odczytanie pomocniczych wska�nik�w
 TAIENodeData* LNodeData = (TAIENodeData*)AThreads->GetNodeData(ANodeIndex);

 AThreads->LocalFreeMemory(LNodeData->ChromosomeIndexMin);
 AThreads->LocalFreeMemory(LNodeData->ChromosomeIndexMax);
 AThreads->LocalFreeMemory(LNodeData->ChromosomeObjects);
 AThreads->LocalFreeMemory(LNodeData->ChromosomeParents);
 AThreads->LocalFreeMemory(LNodeData);
}
//---------------------------------------------------------------------------
// Procedura wielow�tkowa uruchamiana dla ka�dego w�tku (inicjowanie populacji chromosom�w)
//---------------------------------------------------------------------------
void __stdcall NumaUnitInitializePopulation(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 // - odczytanie pomocniczych wska�nik�w i utworzenie struktur danych
 TAIENodeData* LNodeData = (TAIENodeData*)AThreads->GetNodeData(ANodeIndex);

 for (int i = LNodeData->ChromosomeIndexMin[AUnitIndex]; i <= LNodeData->ChromosomeIndexMax[AUnitIndex]; ++i) {
	  TAIEGeneticChromosome* CH = &(LNodeData->ChromosomeObjects[i]);
	  LNodeData->ChromosomeInit(LNodeData->GA, CH->Data, ANodeIndex, AUnitIndex, AThreadIndex);
	  CH->FitnessID = 0;
 }
}
//---------------------------------------------------------------------------
// Procedura wielow�tkowa uruchamiana dla ka�dego w�tku (usuwanie populacji chromosom�w z pami�ci)
//---------------------------------------------------------------------------
void __stdcall NumaUnitDeletePopulation(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 // - odczytanie pomocniczych wska�nik�w i utworzenie struktur danych
 TAIENodeData* LNodeData = (TAIENodeData*)AThreads->GetNodeData(ANodeIndex);

 for (int i = LNodeData->ChromosomeIndexMin[AUnitIndex]; i <= LNodeData->ChromosomeIndexMax[AUnitIndex]; ++i)
	  LNodeData->ChromosomeFree(LNodeData->GA, LNodeData->ChromosomeObjects[i].Data);
}
//---------------------------------------------------------------------------
// Procedura wielow�tkowa uruchamiana dla ka�dego w�tku (obliczanie dopasowania chromosom�w)
//---------------------------------------------------------------------------
void __stdcall NumaUnitFitnessPopulation(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 // - odczytanie pomocniczych wska�nik�w i utworzenie struktur danych
 TAIENodeData* LNodeData = (TAIENodeData*)AThreads->GetNodeData(ANodeIndex);

 for (int i = LNodeData->ChromosomeIndexMin[AUnitIndex]; i <= LNodeData->ChromosomeIndexMax[AUnitIndex]; ++i) {
	  TAIEGeneticChromosome* T = &(LNodeData->ChromosomeObjects[i]);
	  if (!T->FitnessID) T->FitnessValue = LNodeData->FitnessFunction(LNodeData->GA, T->Data);
 }
}
//---------------------------------------------------------------------------
// Procedura wielow�tkowa uruchamiana dla ka�dego w�tku (wymiana chromosom�w potomnych na chromosomy rodzicielskie)
//---------------------------------------------------------------------------
void __stdcall NumaUnitExchangePopulation(IAIENumaThreads* AThreads, int ANodeIndex, int AUnitIndex, int AThreadIndex, bool& ATerminated)
{
 // - odczytanie pomocniczych wska�nik�w i utworzenie struktur danych
 TAIENodeData* LNodeData = (TAIENodeData*)AThreads->GetNodeData(ANodeIndex);

 // - tworzenie cz�ciowej populacji chromosom�w
 for (int i = LNodeData->ChromosomeIndexMin[AUnitIndex]; i <= LNodeData->ChromosomeIndexMax[AUnitIndex]; ++i) {
	  TAIEGeneticChromosome* P = &(LNodeData->ChromosomeParents[i]);
	  if (P->Data) {
	  	  TAIEGeneticChromosome* T = &(LNodeData->ChromosomeObjects[i]);
		  LNodeData->ChromosomeCopy(LNodeData->GA, T->Data, P->Data);
		  T->FitnessValue = P->FitnessValue;
		  T->FitnessID = P->FitnessID;
	  }
 }
}
//---------------------------------------------------------------------------
// TAIEGeneticAlgorithm
//---------------------------------------------------------------------------
__stdcall TAIEGeneticAlgorithm::TAIEGeneticAlgorithm(TAIESystem* ASystem)
{
 FSystem = ASystem;
 FRandomGenerator = new TAIERandomGenerator64();

 FSelectionMethod = 0;
 FPopulationSize = 0;
 FSelectionPressure = 2;
 FCrossOverProbability = 0.7;
 FMutationProbability = 0.1;
 FElitistStrategy = false;

 FChromosomeInit = NULL;
 FChromosomeRand = NULL;
 FChromosomeCopy = NULL;
 FChromosomeFree = NULL;
 FCrossoverOperator = NULL;
 FMutationOperator = NULL;
 FFitnessFunction = NULL;

 FChromosomeObjects.Resize(0);
 FRankTable.Resize(0);

 FMaxFitnessValue = 0;
 FMinFitnessValue = 0;
 FAvgFitnessValue = 0;
 FMaxFitnessValueIndex = 0;
 FMinFitnessValueIndex = 0;

 FIsPrepared = false;
 FIsExecuting = false;
 FIsFinished = false;
}
//---------------------------------------------------------------------------
__stdcall TAIEGeneticAlgorithm::~TAIEGeneticAlgorithm(void)
{
 if (FIsPrepared) Clear();
 delete FRandomGenerator;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::Free(void)
{
 if (FIsExecuting) FSystem->RaiseException("Nie mo�na usun�� obiektu steruj�cego algorytmem w trakcie trwania procesu ewolucji!");
 delete this;
}
//---------------------------------------------------------------------------
bool __stdcall TAIEGeneticAlgorithm::GetIsPrepared(void)
{
 return FIsPrepared;
}
//---------------------------------------------------------------------------
bool __stdcall TAIEGeneticAlgorithm::GetIsExecuting(void)
{
 return FIsExecuting;
}
//---------------------------------------------------------------------------
bool __stdcall TAIEGeneticAlgorithm::GetIsFinished(void)
{
 return FIsFinished;
}
//---------------------------------------------------------------------------
int __stdcall TAIEGeneticAlgorithm::GetSelectionMethod(void)
{
 return FSelectionMethod;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SetSelectionMethod(int ASelectionMethod)
{
 if (FIsPrepared || FIsExecuting) FSystem->RaiseException("Nie mo�na zmieni� parametr�w algorytmu w trakcie trwania procesu ewolucji!");
 FSelectionMethod = ASelectionMethod;
}
//---------------------------------------------------------------------------
int __stdcall TAIEGeneticAlgorithm::GetPopulationSize(void)
{
 return FPopulationSize;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SetPopulationSize(int APopulationSize)
{
 if (FIsPrepared || FIsExecuting) FSystem->RaiseException("Nie mo�na zmieni� parametr�w algorytmu w trakcie trwania procesu ewolucji!");
 if (APopulationSize < 2) FSystem->RaiseException("Liczba chromosom�w w populacji nie mo�e by� mniejsza od 2!");
 FPopulationSize = APopulationSize;
}
//---------------------------------------------------------------------------
int __stdcall TAIEGeneticAlgorithm::GetSelectionPressure(void)
{
 return FSelectionPressure;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SetSelectionPressure(int ASelectionPressure)
{
 if (FIsPrepared || FIsExecuting) FSystem->RaiseException("Nie mo�na zmieni� parametr�w algorytmu w trakcie trwania procesu ewolucji!");
 if (FSelectionPressure < 0) FSystem->RaiseException("Nap�r selekcyjny nie mo�e by� mniejszy od 0!");
 FSelectionPressure = ASelectionPressure;
}
//---------------------------------------------------------------------------
double __stdcall TAIEGeneticAlgorithm::GetCrossOverProbability(void)
{
 return FCrossOverProbability;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SetCrossOverProbability(double ACrossOverProbability)
{
 FCrossOverProbability = ACrossOverProbability;
}
//---------------------------------------------------------------------------
double __stdcall TAIEGeneticAlgorithm::GetMutationProbability(void)
{
 return FMutationProbability;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SetMutationProbability(double AMutationProbability)
{
 FMutationProbability = AMutationProbability;
}
//---------------------------------------------------------------------------
bool __stdcall TAIEGeneticAlgorithm::GetElitistStrategy(void)
{
 return FElitistStrategy;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SetElitistStrategy(bool AElitistStrategy)
{
 if (FIsPrepared || FIsExecuting) FSystem->RaiseException("Nie mo�na zmieni� parametr�w algorytmu w trakcie trwania procesu ewolucji!");
 FElitistStrategy = AElitistStrategy;
}
//---------------------------------------------------------------------------
IAIERandomGenerator64* __stdcall TAIEGeneticAlgorithm::GetRandomGenerator(void)
{
 return FRandomGenerator;
}
//---------------------------------------------------------------------------
int __stdcall TAIEGeneticAlgorithm::GetChromosomeCount(void)
{
 return FChromosomeObjects.GetCount();
}
//---------------------------------------------------------------------------
void* __stdcall TAIEGeneticAlgorithm::GetChromosome(int AIndex)
{
 if (!FIsPrepared) FSystem->RaiseException("Pr�ba odczytania chromosomu zako�czy�a si� niepowodzeniem!\nProces ewolucji nie zosta� uruchomiony!");
 if ((AIndex < 0) || (AIndex >= FChromosomeObjects.GetCount())) FSystem->RaiseException("Pr�ba odczytania chromosomu zako�czy�a si� niepowodzeniem!\nPrzekroczenie dopuszczalnej warto�ci indeksu (" + IntToStr(AIndex) + ").");
 return FChromosomeObjects[AIndex]->Data;
}
//---------------------------------------------------------------------------
void* __stdcall TAIEGeneticAlgorithm::GetBestChromosome(void)
{
 if (!FIsPrepared) FSystem->RaiseException("Pr�ba odczytania najlepszego chromosomu zako�czy�a si� niepowodzeniem!\nProces ewolucji nie zosta� uruchomiony!");
 return FChromosomeObjects[FMaxFitnessValueIndex]->Data;
}
//---------------------------------------------------------------------------
double __stdcall TAIEGeneticAlgorithm::GetCurrentFitnessValue(int AIndex)
{
 if (!FIsPrepared) FSystem->RaiseException("Pr�ba odczytania warto�ci funkcji dopasowania zako�czy�a si� niepowodzeniem!\n"
								   "Proces ewolucji nie zosta� uruchomiony!");
 if ((AIndex < 0) || (AIndex >= FChromosomeObjects.GetCount())) FSystem->RaiseException("Pr�ba odczytania warto�ci funkcji dopasowania zako�czy�a si� niepowodzeniem!\nPrzekroczenie dopuszczalnej warto�ci indeksu (" + IntToStr(AIndex) + ").");
 return FChromosomeObjects[AIndex]->FitnessValue;
}
//---------------------------------------------------------------------------
double __stdcall TAIEGeneticAlgorithm::GetCurrentMaxFitnessValue(void)
{
 return FMaxFitnessValue;
}
//---------------------------------------------------------------------------
double __stdcall TAIEGeneticAlgorithm::GetCurrentAvgFitnessValue(void)
{
 return FAvgFitnessValue;
}
//---------------------------------------------------------------------------
double __stdcall TAIEGeneticAlgorithm::GetCurrentMinFitnessValue(void)
{
 return FMinFitnessValue;
}
//---------------------------------------------------------------------------
TAIEGAChromosomeInit __stdcall TAIEGeneticAlgorithm::GetChromosomeInit(void)
{
 return FChromosomeInit;
}
//---------------------------------------------------------------------------
void* __stdcall TAIEGeneticAlgorithm::GetData(void)
{
 return FData;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SetData(void* AData)
{
 FData = AData;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SetChromosomeInit(TAIEGAChromosomeInit AChromosomeInit)
{
 if (FIsPrepared || FIsExecuting) FSystem->RaiseException("Nie mo�na zmieni� parametr�w algorytmu w trakcie trwania procesu ewolucji!");
 FChromosomeInit = AChromosomeInit;
}
//---------------------------------------------------------------------------
TAIEGAChromosomeRand __stdcall TAIEGeneticAlgorithm::GetChromosomeRand(void)
{
 return FChromosomeRand;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SetChromosomeRand(TAIEGAChromosomeRand AChromosomeRand)
{
 if (FIsPrepared || FIsExecuting) FSystem->RaiseException("Nie mo�na zmieni� parametr�w algorytmu w trakcie trwania procesu ewolucji!");
 FChromosomeRand = AChromosomeRand;
}
//---------------------------------------------------------------------------
TAIEGAChromosomeCopy __stdcall TAIEGeneticAlgorithm::GetChromosomeCopy(void)
{
 return FChromosomeCopy;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SetChromosomeCopy(TAIEGAChromosomeCopy AChromosomeCopy)
{
 if (FIsPrepared || FIsExecuting) FSystem->RaiseException("Nie mo�na zmieni� parametr�w algorytmu w trakcie trwania procesu ewolucji!");
 FChromosomeCopy = AChromosomeCopy;
}
//---------------------------------------------------------------------------
TAIEGAChromosomeFree __stdcall TAIEGeneticAlgorithm::GetChromosomeFree(void)
{
 return FChromosomeFree;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SetChromosomeFree(TAIEGAChromosomeFree AChromosomeFree)
{
 if (FIsPrepared || FIsExecuting) FSystem->RaiseException("Nie mo�na zmieni� parametr�w algorytmu w trakcie trwania procesu ewolucji!");
 FChromosomeFree = AChromosomeFree;
}
//---------------------------------------------------------------------------
TAIEGACrossOverOperator __stdcall TAIEGeneticAlgorithm::GetCrossOverOperator(void)
{
 return FCrossoverOperator;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SetCrossOverOperator(TAIEGACrossOverOperator ACrossoverOperator)
{
 if (FIsPrepared || FIsExecuting) FSystem->RaiseException("Nie mo�na zmieni� parametr�w algorytmu w trakcie trwania procesu ewolucji!");
 FCrossoverOperator = ACrossoverOperator;
}
//---------------------------------------------------------------------------
TAIEGAMutationOperator __stdcall TAIEGeneticAlgorithm::GetMutationOperator(void)
{
 return FMutationOperator;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SetMutationOperator(TAIEGAMutationOperator AMutationOperator)
{
 if (FIsPrepared || FIsExecuting) FSystem->RaiseException("Nie mo�na zmieni� parametr�w algorytmu w trakcie trwania procesu ewolucji!");
 FMutationOperator = AMutationOperator;
}
//---------------------------------------------------------------------------
TAIEGAFitnessFunction __stdcall TAIEGeneticAlgorithm::GetFitnessFunction(void)
{
 return FFitnessFunction;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SetFitnessFunction(TAIEGAFitnessFunction AFitnessFunction)
{
 if (FIsPrepared || FIsExecuting) FSystem->RaiseException("Nie mo�na zmieni� parametr�w algorytmu w trakcie trwania procesu ewolucji!");
 FFitnessFunction = AFitnessFunction;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::Prepare(int Seed)
{
 if (FIsPrepared) FSystem->RaiseException("Pr�ba uruchomienia algorytmu genetycznego zako�czy�a si� niepowodzeniem!\nAlgorytm jest ju� uruchomiony!");
 if ((FSelectionMethod == 1) && (FSelectionPressure < 1)) FSystem->RaiseException("Nap�r selekcyjny dla metody turniejowej nie mo�e by� mniejszy od 2!");
 if ((FSelectionMethod == 1) && (FSelectionPressure > FPopulationSize)) FSystem->RaiseException("Nap�r selekcyjny dla metody turniejowej nie mo�e by� wi�kszy od rozmiaru populacji!");

 // Sprawdzenie parametr�w algorytmu
 if (!FChromosomeInit) FSystem->RaiseException("Pr�ba przygotowania algorytmu do uruchomienia zako�czy�a si� niepowodzeniem!\nFunkcja \"ChromosomeInit\" nie zosta�a okre�lona!");
 if (!FChromosomeCopy) FSystem->RaiseException("Pr�ba przygotowania algorytmu do uruchomienia zako�czy�a si� niepowodzeniem!\nFunkcja \"ChromosomeCopy\" nie zosta�a okre�lona!");
 if (!FChromosomeRand) FSystem->RaiseException("Pr�ba przygotowania algorytmu do uruchomienia zako�czy�a si� niepowodzeniem!\nFunkcja \"ChromosomeRand\" nie zosta�a okre�lona!");
 if (!FChromosomeFree) FSystem->RaiseException("Pr�ba przygotowania algorytmu do uruchomienia zako�czy�a si� niepowodzeniem!\nFunkcja \"ChromosomeFree\" nie zosta�a okre�lona!");
 if (!FCrossoverOperator) FSystem->RaiseException("Pr�ba przygotowania algorytmu do uruchomienia zako�czy�a si� niepowodzeniem!\nFunkcja \"CrossOverOperator\" nie zosta�a okre�lona!");
 if (!FMutationOperator) FSystem->RaiseException("Pr�ba przygotowania algorytmu do uruchomienia zako�czy�a si� niepowodzeniem!\nFunkcja \"MutationOperator\" nie zosta�a okre�lona!");
 if (!FFitnessFunction) FSystem->RaiseException("Pr�ba przygotowania algorytmu do uruchomienia zako�czy�a si� niepowodzeniem!\nFunkcja \"FitnessFunction\" nie zosta�a okre�lona!");

 // Uruchomienie algorytmu
 FIsPrepared = true;
 FIsExecuting = false;
 FChromosomeObjects.Resize(FPopulationSize);
 FRankTable.Resize(FPopulationSize);
 FChromosomeSelctionCount.Resize(FPopulationSize);
 FRouletteFitnessTable.Resize(FPopulationSize);
 FMaxFitnessValueIndex = -1;
 FMinFitnessValueIndex = -1;
 FFitnessID = 0;
 FRandomPermutation = new TAIERandomPermutation(FPopulationSize);
 FRandomPermutation->Initialize(Seed);
 FRandomGenerator->Initialize(Seed);

 FThreads = FSystem->InitNumaThreads();
 FThreads->SetData(this);
 FThreads->ExecuteNodeThreads(NumaNodePrepare);

 // - inicjowanie populacji chromosom�w
 FThreads->ExecuteUnitThreads(NumaUnitInitializePopulation);
 FChromosomeInit(this, FBestChromosome.Data, 0, 0, 0);
 FBestChromosome.FitnessID = 0;

 // - aktualiacja tablicy chromosom�w
 int LChromosomeIndex = 0;
 int LNodeCount = FThreads->GetNumaDevice()->GetNumaNodeCount();
 for (int LNodeIndex = 0; LNodeIndex < LNodeCount; ++LNodeIndex) {
	  TAIENodeData* LNodeData = (TAIENodeData*)FThreads->GetNodeData(LNodeIndex);
	  for (int i = 0; i < LNodeData->ChromosomeCount; ++i) {
		   FChromosomeObjects[LChromosomeIndex] = &(LNodeData->ChromosomeObjects[i]);
		   FChromosomeObjects[LChromosomeIndex]->Index = LChromosomeIndex++;
	  }
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::FirstGeneration(void)
{
 if (!FIsPrepared) FSystem->RaiseException("Pr�ba przeliczenia nast�pnego pokolenia zako�czy�a si� niepowodzeniem!\nAlgorytm genetyczny nie jest uruchomiony!");

 // - losowanie pocz�tkowej populacji
 for (int i = 0; i < FPopulationSize; ++i) FChromosomeRand(this, FChromosomeObjects[i]->Data);

 // - przeliczanie funkcji dopasowania chromosom�w
 ComputeFitnessFunction();
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::NextGeneration(void)
{
 if (!FIsPrepared) FSystem->RaiseException("Pr�ba przeliczenia nast�pnego pokolenia zako�czy�a si� niepowodzeniem!\nAlgorytm genetyczny nie jest uruchomiony!");

 SelectNewChromosomes();
 CrossoverChromosomes();
 MutateChromosomes();
 ComputeFitnessFunction();
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::Clear(void)
{
 if (!FIsPrepared) FSystem->RaiseException("Algorytm zosta� ju� przygotowany do ponownego uruchomienia!");
 if (FIsExecuting) FSystem->RaiseException("Pr�ba przygotowania algorytmu do ponownego uruchomienia zako�czy�a si� niepowodzeniem!\nProces ewolucji nie zosta� zako�czony!");

 FChromosomeFree(this, FBestChromosome.Data);
 FThreads->ExecuteUnitThreads(NumaUnitDeletePopulation);
 FThreads->ExecuteNodeThreads(NumaNodeFinish);
 FThreads->Free();

 delete FRandomPermutation;

 FChromosomeObjects.Resize(0);
 FRankTable.Resize(0);

 FChromosomeInit = NULL;
 FChromosomeRand = NULL;
 FChromosomeCopy = NULL;
 FChromosomeFree = NULL;
 FCrossoverOperator = NULL;
 FMutationOperator = NULL;
 FFitnessFunction = NULL;

 FIsPrepared = false;
 FIsFinished = false;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::ComputeFitnessFunction(void)
{
 // - przeliczenie dopasowania chromosom�w
 FThreads->ExecuteUnitThreads(NumaUnitFitnessPopulation);

 // - odczytanie indeksu najlepszego chromosomu
 FAvgFitnessValue = 0;
 FMaxFitnessValue = MIN_REAL_VALUE;
 FMinFitnessValue = MAX_REAL_VALUE;

 for (int i = 0; i < FPopulationSize; ++i) {
	  double LFitnessValue = FChromosomeObjects[i]->FitnessValue;
	  if (!FChromosomeObjects[i]->FitnessID) FChromosomeObjects[i]->FitnessID = ++FFitnessID;

	  FAvgFitnessValue += LFitnessValue;
	  if (FMinFitnessValue > LFitnessValue) {FMinFitnessValue = LFitnessValue; FMinFitnessValueIndex = i;}
	  if (FMaxFitnessValue < LFitnessValue) {FMaxFitnessValue = LFitnessValue; FMaxFitnessValueIndex = i;}
 }
 FAvgFitnessValue = FAvgFitnessValue / FPopulationSize;

 // - ochroma najlepszego chromosomu (strategia elitarna)
 if (FElitistStrategy) {
	if (!FBestChromosome.FitnessID) {
		FChromosomeCopy(this, FBestChromosome.Data, FChromosomeObjects[FMaxFitnessValueIndex]->Data);
		FBestChromosome.FitnessValue = FMaxFitnessValue;
		FBestChromosome.FitnessID = FChromosomeObjects[FMaxFitnessValueIndex]->FitnessID;
	} else
	if (FMaxFitnessValue > FBestChromosome.FitnessValue) {
		FChromosomeCopy(this, FBestChromosome.Data, FChromosomeObjects[FMaxFitnessValueIndex]->Data);
		FBestChromosome.FitnessValue = FMaxFitnessValue;
		FBestChromosome.FitnessID = FChromosomeObjects[FMaxFitnessValueIndex]->FitnessID;
	} else
	if (FMaxFitnessValue < FBestChromosome.FitnessValue) {
		FChromosomeCopy(this, FChromosomeObjects[FMaxFitnessValueIndex]->Data, FBestChromosome.Data);
		FChromosomeObjects[FMaxFitnessValueIndex]->FitnessValue = FBestChromosome.FitnessValue;
		FChromosomeObjects[FMaxFitnessValueIndex]->FitnessID = FBestChromosome.FitnessID;
		FMaxFitnessValue = FBestChromosome.FitnessValue;
	}
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::PreselectionRoulette(void)
{
 double D = FMaxFitnessValue - FMinFitnessValue;
 if (D == 0.0) {
	 FRouletteFitnessTable[0] = 1.0;
	 for (int i = 1; i < FPopulationSize; ++i) FRouletteFitnessTable[i] = FRouletteFitnessTable[i-1] + 1.0;
 } else {
	 FRouletteFitnessTable[0] = (FChromosomeObjects[0]->FitnessValue - FMinFitnessValue) / D;
	 for (int i = 1; i < FPopulationSize; ++i) FRouletteFitnessTable[i] = FRouletteFitnessTable[i-1] + (FChromosomeObjects[i]->FitnessValue - FMinFitnessValue) / D;
 }

 for (int k = 0; k < FPopulationSize; ++k) {
	  double LRandomValue = FRandomGenerator->RandomDbl(0.0, FRouletteFitnessTable[FPopulationSize - 1]);
	  int i = 0;
	  for (; i < FPopulationSize; ++i) if (LRandomValue <= FRouletteFitnessTable[i]) break;
	  ++FChromosomeSelctionCount[i];
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::PreselectionTournament(void)
{
 for (int k = 0; k < FPopulationSize; ++k) {
	  FRandomPermutation->Reset();
	  int LChromosomeIndex = FRandomPermutation->Random();
	  double LFitnessValue = FChromosomeObjects[LChromosomeIndex]->FitnessValue;
	  for (int i = FSelectionPressure - 1; i > 0; --i) {
		   int XChromosomeIndex = FRandomPermutation->Random();
		   if (LFitnessValue < FChromosomeObjects[XChromosomeIndex]->FitnessValue) {
			   LFitnessValue = FChromosomeObjects[XChromosomeIndex]->FitnessValue;
			   LChromosomeIndex = XChromosomeIndex;
		   } else
		   if (LFitnessValue == FChromosomeObjects[XChromosomeIndex]->FitnessValue)
		   	   LChromosomeIndex = MIN(LChromosomeIndex, XChromosomeIndex);
	  }
	  ++FChromosomeSelctionCount[LChromosomeIndex];
 }
}
//---------------------------------------------------------------------------
int __stdcall ChromosomeRankSort(TAIEGeneticChromosome* const& CH1, TAIEGeneticChromosome* const& CH2)
{
 if (CH1->FitnessValue > CH2->FitnessValue) return +1;
 if (CH1->FitnessValue < CH2->FitnessValue) return -1;
 return 0;
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::PreselectionRank(void)
{
 const double FSelectionPressure = 2.0; //nap�r selekcyjny (od 1.0 do 2.0)

 memcpy(FRankTable.GetData(), FChromosomeObjects.GetData(), FPopulationSize * sizeof(TAIEGeneticChromosome*));
 FRankTable.Sort(ChromosomeRankSort);

 FRouletteFitnessTable[0] = (2.0 - FSelectionPressure) / FPopulationSize;
 for (int i = 1; i < FPopulationSize; ++i)
	  FRouletteFitnessTable[i] = FRouletteFitnessTable[i-1] + (2.0 - FSelectionPressure) / FPopulationSize + (2.0 * (FSelectionPressure - 1.0) * i) / (FPopulationSize * (FPopulationSize - 1.0));

 for (int k = 0; k < FPopulationSize; ++k) {
	  double LRandomValue = FRandomGenerator->RandomDbl(0.0, 1.0);
	  int i = 0;
	  for (; i < FPopulationSize; ++i) if (LRandomValue <= FRouletteFitnessTable[i]) break;
	  ++FChromosomeSelctionCount[FRankTable[i]->Index];
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::PostselectionChromosomes(void)
{
 // - przygotowanie do kopiowania chromosom�w z populacji rodzicielskiej do populacji potomk�w
 int SChromosomeIndex = 0;
 int LChromosomeIndex = 0;
 int LNodeCount = FThreads->GetNumaDevice()->GetNumaNodeCount();
 for (int LNodeIndex = 0; LNodeIndex < LNodeCount; ++LNodeIndex) {
	  TAIENodeData* LNodeData = (TAIENodeData*)FThreads->GetNodeData(LNodeIndex);
	  for (int i = 0; i < LNodeData->ChromosomeCount; ++i) {
		   if (FChromosomeSelctionCount[SChromosomeIndex++]) {
			   // -- je�eli chromosom jest rodziciem, w�wczas jest przenoszony do nast�pnego pokolenia
			   TAIEGeneticChromosome* P = &(LNodeData->ChromosomeParents[i]);
			   P->Data = NULL;
		   } else {
			   // -- je�eli chromosom nie jest rodziciem, w�wczas jest zast�powany przez jednego z rodzic�w w nast�pnym pokoleniu
			   while (FChromosomeSelctionCount[LChromosomeIndex] < 2) if (++LChromosomeIndex >= FPopulationSize) return;
			   TAIEGeneticChromosome* P = &(LNodeData->ChromosomeParents[i]);
			   TAIEGeneticChromosome* T = FChromosomeObjects[LChromosomeIndex];
			   P->Data = T->Data;
			   P->FitnessValue = T->FitnessValue;
			   P->FitnessID = T->FitnessID;
			   FChromosomeSelctionCount[LChromosomeIndex]--;
		   }
	  }
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::SelectNewChromosomes(void)
{
 // - wst�pne wyznaczenie chromosom�w do kopiowania
 FChromosomeSelctionCount.Clear();
 switch (FSelectionMethod) {
		case 0:  PreselectionRoulette(); break;
		case 1:  PreselectionTournament(); break;
		case 2:  PreselectionRank(); break;
		default: PreselectionTournament(); break;
 }

 // - sprawdzenie, czy zosta� wybrany najlepszy chromosom (strategia elitarna)
 if (FElitistStrategy && !FChromosomeSelctionCount[FMaxFitnessValueIndex]) {
	 for (int i = 0; i < FPopulationSize; ++i) if (FChromosomeSelctionCount[i] > 0) {--FChromosomeSelctionCount[i]; break;}
	 FChromosomeSelctionCount[FMaxFitnessValueIndex] = 1;
 }

 // - uporz�dkowanie chromosomm�w do kopiowania w w�z�ach i w�tkach
 PostselectionChromosomes();

 // - kopiowanie chromosom�w i wymiana populacji
 FThreads->ExecuteUnitThreads(NumaUnitExchangePopulation);
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::CrossoverChromosomes(void)
{
 FRandomPermutation->Reset();
 int LCount = FPopulationSize / 2;
 for (int i = 0; i < LCount; ++i) {
	  TAIEGeneticChromosome* CH1 = FChromosomeObjects[FRandomPermutation->Random()];
	  TAIEGeneticChromosome* CH2 = FChromosomeObjects[FRandomPermutation->Random()];
	  if (FCrossOverProbability > FRandomGenerator->RandomDbl(0.0, 1.0))
		  if (CH1->FitnessID != CH2->FitnessID) {
			  FCrossoverOperator(this, CH1->Data, CH2->Data);
			  CH1->FitnessID = 0;
			  CH2->FitnessID = 0;
		  }
 }
}
//---------------------------------------------------------------------------
void __stdcall TAIEGeneticAlgorithm::MutateChromosomes(void)
{
 for (int i = 0; i < FPopulationSize; ++i)
	  if (FMutationProbability > FRandomGenerator->RandomDbl(0.0, 1.0)) {
		  FMutationOperator(this, FChromosomeObjects[i]->Data);
		  FChromosomeObjects[i]->FitnessID = 0;
	  }
}
//---------------------------------------------------------------------------

