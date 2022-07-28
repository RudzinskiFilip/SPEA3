//---------------------------------------------------------------------------
#include <stdlib.h>
#include <math.h>
#pragma hdrstop

#include "UAIEMOGASpea3.h"
#include "UAIERandomPermutation.h"
//---------------------------------------------------------------------------
// TAIEMultiobjectiveGeneticAlgorithmXSPEA2
//---------------------------------------------------------------------------
__stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA3::TAIEMultiobjectiveGeneticAlgorithmSPEA3(void)
			: TAIEMultiobjectiveGeneticAlgorithmSPEA2()
{
 MArchive.Clear();
}
//---------------------------------------------------------------------------
__stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA3::~TAIEMultiobjectiveGeneticAlgorithmSPEA3(void)
{
}
//---------------------------------------------------------------------------
// Selekcja rozwi�za� z poprzedniego archiwum i z populacji do nowego archiwum
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA3::CopyBestSolutionsToArchive(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* AArchive,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation)
{
 if (AArchive->GetCount() < 3) {
	// PROCEDURA KOPIOWANIA DO ARCHIWUM ROZWI�ZA� NAJLEPSZYCH,
	// OCENIANYCH PO WZGL�DEM KA�DEGO Z KRYTERI�W Z OSOBNA
	/*
	for (int k = 0; k < FObjectiveCount; ++k) {
		// - wyznaczenie rozwi�zania o najmniejszej warto�ci funkcji celu
		double VMin = MAX_REAL_VALUE; int IMin = -1;
		for (int i = 0; i < APopulation.GetCount(); ++i)
			 if (VMin > APopulation[i]->FObjectiveValues[k]) {VMin = APopulation[i]->FObjectiveValues[k]; IMin = i;}
		if (IMin >= 0) {
			AArchive.Add(SolutionClone(APopulation[IMin]));
			MArchive.Add(SolutionClone(APopulation[IMin]));
		}
	}
	*/
		int i = 0;
		while(AArchive->GetCount() < 3) AArchive->Add(SolutionClone(APopulation->GetData(i++)));

 } else {
	// PROCEDURA TWORZENIA POPULACJI POMOCNICZEJ UNIKATOWYCH ROZWI�ZA� NIEZDOMINOWANYCH
	// I NIE ISTNIEJ�CYCH W BIE��CYM ARCHIWUM
	// (UNIKATOWE ROZWI�ZANIA = NIE POWTARZAJ�CE SI� W POPULACJI)
	TQuickList<TAIEMultiobjectiveGeneticSolution*> XArchive;
	for (int i = 0; i < APopulation->GetCount(); ++i) {
		 if (APopulation->GetData(i)->IsDominatedOrEqual(AArchive)) continue;
		 if (APopulation->GetData(i)->IsDominated(APopulation)) continue;
		 XArchive.Add(APopulation->GetData(i));
	}
	if (!XArchive.GetCount()) return;

	// - utworzenie tymczasowego archiwum
	TQuickList<TAIEMultiobjectiveGeneticSolution*> NArchive;
	for (int i = 0; i < AArchive->GetCount(); ++i) NArchive.Add(AArchive->GetData(i));

	// - je�eli archiwum jest zbyt ma�e, to uzupe�nianie go rozwi�zaniami z populacji pomocniczej
	//if ((FGenerationIndex % 10 == 0) && (NArchive.GetCount() < FFrontMaxSize)) AddAuxiliarySolutionsToArchive(NArchive, XArchive);
	if (NArchive.GetCount() < FFrontMaxSize) AddAuxiliarySolutionsToArchive(&NArchive, &XArchive);

	// - wymie� zdominowane rozwi�zania w archiwum na rozwi�zania z populacji pomocniczej
	ExchangeDominatedSolutionsInArchive(&NArchive, &XArchive);

	// - wyr�wnaj odleg�o�ci pomi�dzy rozwi�zaniami w archiwum
	FullAlignDistancesBetweenSolutionsInArchive(&NArchive, &XArchive);

	// wymiana skrajnych rozwi�za�
//	for (int k = 0; k < FObjectiveCount; ++k) {
//		 - wyznaczenie rozwi�zania o najmniejszej warto�ci funkcji celu
//		double VMin = MAX_REAL_VALUE; int IMin = -1;
//		for (int i = 0; i < NArchive.GetCount(); ++i)
//			 if (VMin > NArchive[i]->FObjectiveValues[k]) {VMin = NArchive[i]->FObjectiveValues[k]; IMin = i;}
//		if (VMin > MArchive[k]->FObjectiveValues[k]) NArchive[IMin] = MArchive[k];
//		if (VMin < MArchive[k]->FObjectiveValues[k]) {SolutionDelete(MArchive[k]); MArchive[k] = SolutionClone(NArchive[IMin]);}
//	}

	// PROCEDURA USUWANIA ZDOMINOWANYCH ROZWI�ZA� Z ARCHIWUM
	// if (AArchive.GetCount() == FFrontMaxSize)
	ClearDominatedSolutions(&NArchive);

	// - podmiana poprzedniego archiwum na nowe
	for (int i = 0; i < NArchive.GetCount(); ++i) NArchive.SetData(i, SolutionClone(NArchive.GetData(i)));
	ClearSolutions(AArchive);
	for (int i = 0; i < NArchive.GetCount(); ++i) AArchive->Add(NArchive.GetData(i));
 }
}
//---------------------------------------------------------------------------
/*
bool __stdcall IsDominated(TAIEMultiobjectiveGeneticSolution* ASolution1, TAIEMultiobjectiveGeneticSolution* ASolution2)
{
 for (int i = 0; i < 2; ++i)
	  if (ASolution1->GetObjectiveValue(i) >= ASolution2->GetObjectiveValue(i)) return false;
 return true;
}
*/
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA3::AddAuxiliarySolutionsToArchive(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* NArchive,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* XArchive)
{
 // -- wykonanie kopii archiwum
 TQuickList<TAIEMultiobjectiveGeneticSolution*> PArchive;
 for (int i = 0; i < NArchive->GetCount(); ++i) PArchive.Add(NArchive->GetData(i));

 while ((PArchive.GetCount() > 1) && ((NArchive->GetCount() < FFrontMaxSize))) {
		// -- wyznaczenie rozwi�zania w archiwum (IMax) o najwi�kszej odleg�o�ci
		//    do najbli�szego s�siada (KMax)
		double DMax = MIN_REAL_VALUE;
		int IMax = -1;
		int KMax = -1;
		for (int i = 0; i < PArchive.GetCount(); ++i) {
			 // --- wyznaczanie najblizszego s�siada w archiwum
			 double DMin; int KMin;
			 FindNearestNeighourhood(PArchive.GetData(i), &PArchive, DMin, KMin);
			 if (DMax < DMin) {DMax = DMin; IMax = i; KMax = KMin;}
		}

		// -- wyznaczenie rozwi�zania z populacji tymczasowej, kt�rego r�nica
		//    odleg�o�ci do rozwi�za� IMax i KMax jest najmniejsza
		double DMinI = MAX_REAL_VALUE;
		double DMinK = 0;
		int JMin = -1;
		for (int j = 0; j < XArchive->GetCount(); ++j) {
			 double D1 = NormalizedDistanceBetweenSolutions(PArchive.GetData(IMax), XArchive->GetData(j), &FFinalObjectiveMinValues, &FFinalObjectiveMaxValues);
			 double D2 = NormalizedDistanceBetweenSolutions(PArchive.GetData(KMax), XArchive->GetData(j), &FFinalObjectiveMinValues, &FFinalObjectiveMaxValues);
			 if (ABS(DMinI - DMinK) > ABS(D1 - D2)) {DMinI = D1; DMinK = D2; JMin = j;}
		}

		// -- je�eli odleg�o�� pomi�dzy rozwi�zaniami IMax i JMin jest
		//	  wi�ksza od odleg�o�ci pomi�dzy IMax i KMax to odrzu� rozwi�zanie IMax
		if (DMax < DMinI) PArchive.Delete(IMax);
		else
		// -- je�eli odleg�o�� pomi�dzy rozwi�zaniami KMax i JMin jest
		//	  wi�ksza od odleg�o�ci pomi�dzy IMax i KMax to odrzu� rozwi�zanie KMax
		if (DMax < DMinK) PArchive.Delete(KMax);
		// -- dodaj JMin do nowego archiwum i usu� z pomocniczego archiwum
		//    i przerwij procedur� uzupe�niania archiwum
		else {
			NArchive->Add(XArchive->GetData(JMin));
			XArchive->Delete(JMin);
			PArchive.Delete(IMax);
			PArchive.Delete(KMax);
			break;   //dodawanie po jednym rozwi�zaniu lepiej wp�ywa na rozw�j frontu pareto
		}
 }
}
//---------------------------------------------------------------------------
// PROCEDURA ZAST�POWANIA ROZWI�ZA� ZDOMINOWANYCH W ARCHIWUM
// ROZWI�ZANIAMI NIEZDOMINOWANYMI Z POPULACJI POMOCNICZEJ
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA3::ExchangeDominatedSolutionsInArchive(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* NArchive,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* XArchive)
{
 for (int i = 0; i < NArchive->GetCount(); ++i)
	  if (NArchive->GetData(i)->IsDominated(XArchive)) {
		  double Dij; int j;
		  FindNearestNeighourhood(NArchive->GetData(i), XArchive, Dij, j);
		  NArchive->SetData(i, XArchive->GetData(j));
		  XArchive->Delete(j--);
	  }
}
//---------------------------------------------------------------------------
//PROCEDURA WYR�WNYWANIA ODLEG�O�CI POMI�DZY ROZWI�ZANIAMI W ARCHIWUM
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA3::FullAlignDistancesBetweenSolutionsInArchive(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* NArchive,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* XArchive)
{
 if (NArchive->GetCount() < 2) return;
 if (XArchive->GetCount() < 1) return;

 int RepeatCount = 0;
 do {
	 if (!StepAlignDistancesBetweenSolutionsInArchive(NArchive, XArchive)) break;
 } while (++RepeatCount < 1);
 //liczba powt�rze� mo�e by� wi�ksza przy monotonicznych frontach Pareto
 //w�wczas nast�puje przy�pieszenie wyr�wnywania front�w
}
//---------------------------------------------------------------------------
//PROCEDURA WYR�WNYWANIA ODLEG�O�CI POMI�DZY ROZWI�ZANIAMI W ARCHIWUM
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA3::StepAlignDistancesBetweenSolutionsInArchive(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* NArchive,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* XArchive)
{
 bool IsChanged = false;
 for (int i = 0; i < NArchive->GetCount(); ++i) {
	  TQuickList<TAIEMultiobjectiveGeneticSolution*> PArchive;
	  for (int q = 0; q < XArchive->GetCount(); ++q) PArchive.Add(XArchive->GetData(q));
	  while (PArchive.GetCount()) {
			 // - wyznaczanie rozwi�zania "j" w populacji pomocniczej,
			 //   po�o�onego najbli�ej rozwi�zania "i" w archiwum
			 double Dij; int j;
			 FindNearestNeighourhood(NArchive->GetData(i), &PArchive, Dij, j);
			 if (j < 0) break;

			 // - wyznaczanie rozwi�zania "k" w archiwum, nie b�d�cego rozwi�zaniem "i",
			 //   po�o�onego najbli�ej rozwi�zania "j" w populacji pomocniczej
			 double Djk = MAX_REAL_VALUE; int k = -1;
			 for (int l = 0; l < NArchive->GetCount(); ++l) {
				  if (l == i) continue;
				  double D = NormalizedDistanceBetweenSolutions(PArchive.GetData(j), NArchive->GetData(l), &FFinalObjectiveMinValues, &FFinalObjectiveMaxValues);
				  if (Djk > D) {Djk = D; k = l;}
			 }
			 if (k < 0) break;

			 // - wyznaczenie odleg�o�ci pomi�dzy rozwi�zaniami "i" i "k"
			 double Dik = NormalizedDistanceBetweenSolutions(NArchive->GetData(i), NArchive->GetData(k), &FFinalObjectiveMinValues, &FFinalObjectiveMaxValues);

			 // - je�eli odleg�o�� Djk jest wi�ksza od Dik to zast�pi� ("i" := "j")
			 if (Djk > Dik) {
				 NArchive->SetData(i, PArchive.GetData(j));
				 IsChanged = true;
			 }
			 PArchive.Delete(j);
	  }
 }
 return IsChanged;
}
//---------------------------------------------------------------------------
// PROCEDURA WYSZUKIWANIA NAJBLI�SZEGO ROZWI�ZANIA W POPULACJI
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA3::FindNearestNeighourhood(
	TAIEMultiobjectiveGeneticSolution* ASolution,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation, double& ADistance, int& AIndex)
{
 // - wyznaczanie najblizszego s�siada w populacji
 AIndex = -1;
 ADistance = MAX_REAL_VALUE;
 for (int i = 0; i < APopulation->GetCount(); ++i) {
	  if (ASolution == APopulation->GetData(i)) continue;
	  double D = NormalizedDistanceBetweenSolutions(ASolution, APopulation->GetData(i), &FFinalObjectiveMinValues, &FFinalObjectiveMaxValues);
	  if (ADistance > D) {ADistance = D; AIndex = i;}
 }
 return (AIndex >= 0);
}
//---------------------------------------------------------------------------
// PROCEDURA WYSZUKIWANIA NAJDALSZEGO ROZWI�ZANIA W POPULACJI
//---------------------------------------------------------------------------
bool __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA3::FindFarthestNeighourhood(
	TAIEMultiobjectiveGeneticSolution* ASolution,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* APopulation, double& ADistance, int& AIndex)
{
 // - wyznaczanie najdalszego s�siada w populacji
 AIndex = -1;
 ADistance = MIN_REAL_VALUE;
 for (int i = 0; i < APopulation->GetCount(); ++i) {
	  if (ASolution == APopulation->GetData(i)) continue;
	  double D = NormalizedDistanceBetweenSolutions(ASolution, APopulation->GetData(i), &FFinalObjectiveMinValues, &FFinalObjectiveMaxValues);
	  if (ADistance < D) {ADistance = D; AIndex = i;}
 }
 return (AIndex >= 0);
}
//---------------------------------------------------------------------------
// Usuwanie wszystkich zdominowanych rozwi�za� z archiwum
//---------------------------------------------------------------------------
void __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA3::ClearDominatedSolutions(
	TQuickList<TAIEMultiobjectiveGeneticSolution*>* NArchive)
{
 for (int i = 0; i < NArchive->GetCount(); ++i)
	  for (int j = i+1; j < NArchive->GetCount(); ++j)
		   if (NArchive->GetData(i)->Dominates(NArchive->GetData(j))) {
			   NArchive->Delete(j--);
		   } else
		   if (NArchive->GetData(j)->Dominates(NArchive->GetData(i))) {
			   NArchive->Delete(i--);
			   break;
		   }
}
//---------------------------------------------------------------------------



/*
double __stdcall TAIEMultiobjectiveGeneticAlgorithmSPEA3::SumDistanceToKNearestNeighbourhoods(
	TAIEMultiobjectiveGeneticSolution* ASolution,
	TQuickList<TAIEMultiobjectiveGeneticSolution*>& APopulation, int K)
{
 TQuickList<double> LDistances;
 for (int i = 0; i < APopulation.GetCount(); ++i)
	  if (APopulation[i] != ASolution) LDistances.Add(NormalizedDistanceBetweenSolutions(*ASolution, (*APopulation[i])));
 LDistances.Sort(COMPARE_DOUBLE);

 double Sum = 0.0;
 for (int k = 0; (k < K) && (k < LDistances.GetCount()); ++k) Sum += LDistances[k];

// double Sum = MAX_REAL_VALUE;
// for (int k = 0; (k < K) && (k < LDistances.GetCount()); ++k)
//	  for (int k2 = k + 1; (k2 < K) && (k2 < LDistances.GetCount()); ++k2)
//		   if (Sum > ABS(LDistances[k] - LDistances[k2])) Sum = ABS(LDistances[k] - LDistances[k2]);

 return Sum;
}
*/
//---------------------------------------------------------------------------

