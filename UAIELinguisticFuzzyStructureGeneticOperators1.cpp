//---------------------------------------------------------------------------
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "UTemplates.h"
#include "UDynamicPointers.h"
#include "UAIESystem.h"
#include "UAIEBinaryFile.h"
#include "UAIEComputingTimer.h"
#include "UAIERandomPermutation.h"
#include "UAIERandomGenerator64.h"
#include "UAIEGeneticAlgorithm.h"
#include "UAIEStopConditions.h"
#include "UAIEInitConditions.h"
#include "UAIENumaDevice.h"
#include "UAIENumaNode.h"
#include "UAIENumaUnit.h"
#include "UAIENumaThreads.h"
#include "UAIEFunction.h"
#include "UAIELinguisticFuzzySet.h"
#include "UAIELinguisticFuzzyRule.h"
#include "UAIELinguisticFuzzyStructure.h"

#include "UAIEDataSet.h"
#include "UAIEDataSetRecord.h"
#include "UAIEDataSetColumn.h"
#include "UAIENumaCommonDataSet.h"

#include "UAIELinguisticFuzzyStructureGeneticOperators1.h"
//---------------------------------------------------------------------------
// OPERATORY GENETYCZNE - OPERATORY KRZY¯OWANIA
//---------------------------------------------------------------------------
// RulebaseExchange1 - Wymiana regu³ w bazach regu³ (umieszczonych na tych samych miejscach)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange1(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator)
{
 int FuzzyRuleCount1 = FuzzyRulebase1->GetFuzzyRuleCount();
 int FuzzyRuleCount2 = FuzzyRulebase2->GetFuzzyRuleCount();
 int InputFuzzyAttrCount = FuzzyRulebase1->GetInpFuzzyAttrCount();
 int i = 0;
 for (; (i < FuzzyRuleCount1) && (i < FuzzyRuleCount2); ++i) {
	if (RandomGenerator->RandomInt(0, 1)) {
		IAIELinguisticFuzzyRule* FuzzyRule1 = FuzzyRulebase1->GetFuzzyRule(i);
		IAIELinguisticFuzzyRule* FuzzyRule2 = FuzzyRulebase2->GetFuzzyRule(i);
		for (int j = 0; j < InputFuzzyAttrCount; ++j) {
			 int Tmp = FuzzyRule1->GetInpFuzzySetIndex(j);
			 FuzzyRule1->SetInpFuzzySetIndex(j, FuzzyRule2->GetInpFuzzySetIndex(j));
			 FuzzyRule2->SetInpFuzzySetIndex(j, Tmp);

			 bool Tmp2 = FuzzyRule1->GetInpFuzzySetNegated(j);
			 FuzzyRule1->SetInpFuzzySetNegated(j, FuzzyRule2->GetInpFuzzySetNegated(j));
			 FuzzyRule2->SetInpFuzzySetNegated(j, Tmp2);
		}
		int Tmp = FuzzyRule1->GetOutFuzzySetIndex();
		FuzzyRule1->SetOutFuzzySetIndex(FuzzyRule2->GetOutFuzzySetIndex());
		FuzzyRule2->SetOutFuzzySetIndex(Tmp);
	}
 }
 // - przesuniêcie regu³ z systemu 1 (o wiêkszej liczbie regu³) do systemu 2 (o mniejszej liczbie regu³)
 for (; i < FuzzyRulebase1->GetFuzzyRuleCount(); ++i) {
	if (RandomGenerator->RandomInt(0, 1)) {
		IAIELinguisticFuzzyRule* FuzzyRule1 = FuzzyRulebase1->GetFuzzyRule(i);
		IAIELinguisticFuzzyRule* FuzzyRule3 = FuzzyRulebase2->AppendFuzzyRule();
		for (int j = 0; j < InputFuzzyAttrCount; ++j) {
			 FuzzyRule3->SetInpFuzzySetIndex(j, FuzzyRule1->GetInpFuzzySetIndex(j));
			 FuzzyRule3->SetInpFuzzySetNegated(j, FuzzyRule1->GetInpFuzzySetNegated(j));
		}
		FuzzyRule3->SetOutFuzzySetIndex(FuzzyRule1->GetOutFuzzySetIndex());
		FuzzyRulebase1->DeleteFuzzyRule(i--);
	}
 }
 // - przesuniêcie regu³ z systemu 2 (o wiêkszej liczbie regu³) do systemu 1 (o mniejszej liczbie regu³)
 for (; i < FuzzyRulebase2->GetFuzzyRuleCount(); ++i) {
	if (RandomGenerator->RandomInt(0, 1)) {
		IAIELinguisticFuzzyRule* FuzzyRule2 = FuzzyRulebase2->GetFuzzyRule(i);
		IAIELinguisticFuzzyRule* FuzzyRule3 = FuzzyRulebase1->AppendFuzzyRule();
		for (int j = 0; j < InputFuzzyAttrCount; ++j) {
			 FuzzyRule3->SetInpFuzzySetIndex(j, FuzzyRule2->GetInpFuzzySetIndex(j));
			 FuzzyRule3->SetInpFuzzySetNegated(j, FuzzyRule2->GetInpFuzzySetNegated(j));
		}
		FuzzyRule3->SetOutFuzzySetIndex(FuzzyRule2->GetOutFuzzySetIndex());
		FuzzyRulebase2->DeleteFuzzyRule(i--);
	}
 }
}
//---------------------------------------------------------------------------
// RulebaseExchange2 - Wymiana jednej regu³y w bazach regu³ (umieszczonych na tych samych miejscach)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange2(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator)
{
 int FuzzyRuleCount1 = FuzzyRulebase1->GetFuzzyRuleCount();
 int FuzzyRuleCount2 = FuzzyRulebase2->GetFuzzyRuleCount();
 int InputFuzzyAttrCount = FuzzyRulebase1->GetInpFuzzyAttrCount();
 int i = (int)RandomGenerator->RandomInt(0, MAX(FuzzyRuleCount1, FuzzyRuleCount2) - 1);

 if ((i < FuzzyRuleCount1) && (i < FuzzyRuleCount2)) {
	  IAIELinguisticFuzzyRule* FuzzyRule1 = FuzzyRulebase1->GetFuzzyRule(i);
	  IAIELinguisticFuzzyRule* FuzzyRule2 = FuzzyRulebase2->GetFuzzyRule(i);
	  for (int j = 0; j < InputFuzzyAttrCount; ++j) {
		   int Tmp = FuzzyRule1->GetInpFuzzySetIndex(j);
		   FuzzyRule1->SetInpFuzzySetIndex(j, FuzzyRule2->GetInpFuzzySetIndex(j));
		   FuzzyRule2->SetInpFuzzySetIndex(j, Tmp);

		   bool Tmp2 = FuzzyRule1->GetInpFuzzySetNegated(j);
		   FuzzyRule1->SetInpFuzzySetNegated(j, FuzzyRule2->GetInpFuzzySetNegated(j));
		   FuzzyRule2->SetInpFuzzySetNegated(j, Tmp2);
	  }
	  int Tmp = FuzzyRule1->GetOutFuzzySetIndex();
	  FuzzyRule1->SetOutFuzzySetIndex(FuzzyRule2->GetOutFuzzySetIndex());
	  FuzzyRule2->SetOutFuzzySetIndex(Tmp);
 } else
 // - przesuniêcie regu³y z systemu 1 (o wiêkszej liczbie regu³) do systemu 2 (o mniejszej liczbie regu³)
 if (i < FuzzyRuleCount1) {
	  IAIELinguisticFuzzyRule* FuzzyRule1 = FuzzyRulebase1->GetFuzzyRule(i);
	  IAIELinguisticFuzzyRule* FuzzyRule3 = FuzzyRulebase2->AppendFuzzyRule();
	  for (int j = 0; j < InputFuzzyAttrCount; ++j) {
		   FuzzyRule3->SetInpFuzzySetIndex(j, FuzzyRule1->GetInpFuzzySetIndex(j));
		   FuzzyRule3->SetInpFuzzySetNegated(j, FuzzyRule1->GetInpFuzzySetNegated(j));
	  }
	  FuzzyRule3->SetOutFuzzySetIndex(FuzzyRule1->GetOutFuzzySetIndex());
	  FuzzyRulebase1->DeleteFuzzyRule(i);
 } else
 // - przesuniêcie regu³y z systemu 2 (o wiêkszej liczbie regu³) do systemu 1 (o mniejszej liczbie regu³)
 if (i < FuzzyRuleCount2) {
	  IAIELinguisticFuzzyRule* FuzzyRule2 = FuzzyRulebase2->GetFuzzyRule(i);
	  IAIELinguisticFuzzyRule* FuzzyRule3 = FuzzyRulebase1->AppendFuzzyRule();
	  for (int j = 0; j < InputFuzzyAttrCount; ++j) {
		   FuzzyRule3->SetInpFuzzySetIndex(j, FuzzyRule2->GetInpFuzzySetIndex(j));
		   FuzzyRule3->SetInpFuzzySetNegated(j, FuzzyRule2->GetInpFuzzySetNegated(j));
	  }
	  FuzzyRule3->SetOutFuzzySetIndex(FuzzyRule2->GetOutFuzzySetIndex());
	  FuzzyRulebase2->DeleteFuzzyRule(i);
 }
}
//---------------------------------------------------------------------------
// RulebaseExchange3 - Wymiana przes³anek w wielu losowo wybranych regu³ach (umieszczonych na tych samych miejscach)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange3(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator)
{
 int FuzzyRuleCount1 = FuzzyRulebase1->GetFuzzyRuleCount();
 int FuzzyRuleCount2 = FuzzyRulebase2->GetFuzzyRuleCount();
 int InputFuzzyAttrCount = FuzzyRulebase1->GetInpFuzzyAttrCount();

 for (int i = 0; (i < FuzzyRuleCount1) && (i < FuzzyRuleCount2); ++i) {
	if (RandomGenerator->RandomInt(0, 1)) {
		IAIELinguisticFuzzyRule* FuzzyRule1 = FuzzyRulebase1->GetFuzzyRule(i);
		IAIELinguisticFuzzyRule* FuzzyRule2 = FuzzyRulebase2->GetFuzzyRule(i);
		// -- wymiana przes³anek
		for (int j = 0; j < InputFuzzyAttrCount; ++j)
			if (!RandomGenerator->RandomInt(0, 1)) {
				int Tmp = FuzzyRule1->GetInpFuzzySetIndex(j);
				FuzzyRule1->SetInpFuzzySetIndex(j, FuzzyRule2->GetInpFuzzySetIndex(j));
				FuzzyRule2->SetInpFuzzySetIndex(j, Tmp);

				bool Tmp2 = FuzzyRule1->GetInpFuzzySetNegated(j);
				FuzzyRule1->SetInpFuzzySetNegated(j, FuzzyRule2->GetInpFuzzySetNegated(j));
				FuzzyRule2->SetInpFuzzySetNegated(j, Tmp2);
			}
		// -- wymiana konkluzji
		if (!RandomGenerator->RandomInt(0, 1)) {
			int Tmp = FuzzyRule1->GetOutFuzzySetIndex();
			FuzzyRule1->SetOutFuzzySetIndex(FuzzyRule2->GetOutFuzzySetIndex());
			FuzzyRule2->SetOutFuzzySetIndex(Tmp);
		}
	}
 }
}
//---------------------------------------------------------------------------
// RulebaseExchange4 - Wymiana przes³anek w parze losowo wybranych regu³ (umieszczonych na tych samych miejscach)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange4(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator)
{
 int FuzzyRuleCount1 = FuzzyRulebase1->GetFuzzyRuleCount();
 int FuzzyRuleCount2 = FuzzyRulebase2->GetFuzzyRuleCount();
 int InputFuzzyAttrCount = FuzzyRulebase1->GetInpFuzzyAttrCount();

 int i = (int)RandomGenerator->RandomInt(0, MIN(FuzzyRuleCount1, FuzzyRuleCount2) - 1);

 IAIELinguisticFuzzyRule* FuzzyRule1 = FuzzyRulebase1->GetFuzzyRule(i);
 IAIELinguisticFuzzyRule* FuzzyRule2 = FuzzyRulebase2->GetFuzzyRule(i);
 // -- wymiana przes³anek
 for (int j = 0; j < InputFuzzyAttrCount; ++j)
	if (!RandomGenerator->RandomInt(0, 1)) {
		int Tmp = FuzzyRule1->GetInpFuzzySetIndex(j);
		FuzzyRule1->SetInpFuzzySetIndex(j, FuzzyRule2->GetInpFuzzySetIndex(j));
		FuzzyRule2->SetInpFuzzySetIndex(j, Tmp);

		bool Tmp2 = FuzzyRule1->GetInpFuzzySetNegated(j);
		FuzzyRule1->SetInpFuzzySetNegated(j, FuzzyRule2->GetInpFuzzySetNegated(j));
		FuzzyRule2->SetInpFuzzySetNegated(j, Tmp2);
	}
 // -- wymiana konkluzji
 if (!RandomGenerator->RandomInt(0, 1)) {
	int Tmp = FuzzyRule1->GetOutFuzzySetIndex();
	FuzzyRule1->SetOutFuzzySetIndex(FuzzyRule2->GetOutFuzzySetIndex());
	FuzzyRule2->SetOutFuzzySetIndex(Tmp);
 }
}
//---------------------------------------------------------------------------
// RulebaseExchange5 - Wymiana przes³anki w parze losowo wybranych regu³ (umieszczonych na tych samych miejscach)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange5(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator)
{
 int FuzzyRuleCount1 = FuzzyRulebase1->GetFuzzyRuleCount();
 int FuzzyRuleCount2 = FuzzyRulebase2->GetFuzzyRuleCount();
 int InputFuzzyAttrCount = FuzzyRulebase1->GetInpFuzzyAttrCount();

 int i = (int)RandomGenerator->RandomInt(0, MIN(FuzzyRuleCount1, FuzzyRuleCount2) - 1);

 IAIELinguisticFuzzyRule* FuzzyRule1 = FuzzyRulebase1->GetFuzzyRule(i);
 IAIELinguisticFuzzyRule* FuzzyRule2 = FuzzyRulebase2->GetFuzzyRule(i);

 int j = (int)RandomGenerator->RandomInt(0, InputFuzzyAttrCount);
 if (j < InputFuzzyAttrCount) {
	// -- wymiana przes³anki
	int Tmp = FuzzyRule1->GetInpFuzzySetIndex(j);
	FuzzyRule1->SetInpFuzzySetIndex(j, FuzzyRule2->GetInpFuzzySetIndex(j));
	FuzzyRule2->SetInpFuzzySetIndex(j, Tmp);

	bool Tmp2 = FuzzyRule1->GetInpFuzzySetNegated(j);
	FuzzyRule1->SetInpFuzzySetNegated(j, FuzzyRule2->GetInpFuzzySetNegated(j));
	FuzzyRule2->SetInpFuzzySetNegated(j, Tmp2);
 } else {
	 // -- wymiana konkluzji
	 int Tmp = FuzzyRule1->GetOutFuzzySetIndex();
	 FuzzyRule1->SetOutFuzzySetIndex(FuzzyRule2->GetOutFuzzySetIndex());
	 FuzzyRule2->SetOutFuzzySetIndex(Tmp);
 }
}
//---------------------------------------------------------------------------
// AttrExchange - wymiana parametrów bazy danych
//---------------------------------------------------------------------------
void __stdcall AttrExchange(IAIELinguisticFuzzyCollection* FuzzyCollection1, IAIELinguisticFuzzyCollection* FuzzyCollection2, IAIERandomGenerator64* RandomGenerator)
{
 int FuzzySetIndex = (int)RandomGenerator->RandomInt(0, FuzzyCollection1->GetFuzzySetCount() - 1);
 IAIELinguisticFuzzySet* FuzzySet1 = FuzzyCollection1->GetFuzzySet(FuzzySetIndex);
 IAIELinguisticFuzzySet* FuzzySet2 = FuzzyCollection2->GetFuzzySet(FuzzySetIndex);

 double F = RandomGenerator->RandomDbl(0.0, 1.0);
 double C1 = FuzzySet1->GetLCenter();
 double C2 = FuzzySet2->GetLCenter();
 FuzzySet1->SetLCenter(C1 * F + (1.0 - F) * C2);
 FuzzySet2->SetLCenter(C2 * F + (1.0 - F) * C1);

 F = RandomGenerator->RandomDbl(0.0, 1.0);
 C1 = FuzzySet1->GetRCenter();
 C2 = FuzzySet2->GetRCenter();
 FuzzySet1->SetRCenter(C1 * F + (1.0 - F) * C2);
 FuzzySet2->SetRCenter(C2 * F + (1.0 - F) * C1);

 F = RandomGenerator->RandomDbl(0.0, 1.0);
 C1 = FuzzySet1->GetLAlpha();
 C2 = FuzzySet2->GetLAlpha();
 FuzzySet1->SetLAlpha(C1 * F + (1.0 - F) * C2);
 FuzzySet2->SetLAlpha(C2 * F + (1.0 - F) * C1);

 F = RandomGenerator->RandomDbl(0.0, 1.0);
 C1 = FuzzySet1->GetRAlpha();
 C2 = FuzzySet2->GetRAlpha();
 FuzzySet1->SetRAlpha(C1 * F + (1.0 - F) * C2);
 FuzzySet2->SetRAlpha(C2 * F + (1.0 - F) * C1);

 F = RandomGenerator->RandomDbl(0.0, 1.0);
 C1 = FuzzySet1->GetRContrast();
 C2 = FuzzySet2->GetRContrast();
 FuzzySet1->SetRContrast(C1 * F + (1.0 - F) * C2);
 FuzzySet2->SetRContrast(C2 * F + (1.0 - F) * C1);

 F = RandomGenerator->RandomDbl(0.0, 1.0);
 C1 = FuzzySet1->GetLContrast();
 C2 = FuzzySet2->GetLContrast();
 FuzzySet1->SetLContrast(C1 * F + (1.0 - F) * C2);
 FuzzySet2->SetLContrast(C2 * F + (1.0 - F) * C1);
}
//---------------------------------------------------------------------------
// OPERATORY GENETYCZNE - OPERATORY MUTACJI
//---------------------------------------------------------------------------
// RuleInsert - dodanie nowej, losowej regu³y
//---------------------------------------------------------------------------
void __stdcall RuleInsert(IAIELinguisticFuzzyRulebase* FuzzyRulebase, bool OnlyFullFuzzyRule, bool OnlyTrueFuzzyRule, IAIERandomGenerator64* RandomGenerator)
{
 // - dodanie regu³y
 IAIELinguisticFuzzyRule* FuzzyRule = FuzzyRulebase->AppendFuzzyRule();
 // - losowanie przes³anek
 int InputFuzzyAttrCount = FuzzyRulebase->GetInpFuzzyAttrCount();
 if (OnlyFullFuzzyRule) {
	for (int i = 0; i < InputFuzzyAttrCount; ++i) {
		 FuzzyRule->SetInpFuzzySetIndex(i, (int)RandomGenerator->RandomInt(0, FuzzyRule->GetInpFuzzySetCount(i) - 1));
		 FuzzyRule->SetInpFuzzySetNegated(i,  (OnlyTrueFuzzyRule ? false : ((int)RandomGenerator->RandomInt(0, 1) == 1)));
	}
 } else {
	int LAttrCount = (int)RandomGenerator->RandomInt(1, InputFuzzyAttrCount);
	for (int i = 0; i < InputFuzzyAttrCount; ++i) FuzzyRule->SetInpFuzzySetIndex(i, -1);
	while(LAttrCount-- > 0) {
		 int k = (int)RandomGenerator->RandomInt(0, InputFuzzyAttrCount - 1);
		 while (FuzzyRule->GetInpFuzzySetIndex(k) >= 0) k = (int)RandomGenerator->RandomInt(0, InputFuzzyAttrCount - 1);
		 FuzzyRule->SetInpFuzzySetIndex(k, (int)RandomGenerator->RandomInt(0, FuzzyRule->GetInpFuzzySetCount(k) - 1));
		 FuzzyRule->SetInpFuzzySetNegated(k, (OnlyTrueFuzzyRule ? false : ((int)RandomGenerator->RandomInt(0, 1) == 1)));
	}
 }
 FuzzyRule->SetOutFuzzySetIndex((int)RandomGenerator->RandomInt(0, FuzzyRule->GetOutFuzzySetCount() - 1));
 FuzzyRule->SetEnabled(true);
}
//---------------------------------------------------------------------------
// RuleDelete - usuwanie regu³y
//---------------------------------------------------------------------------
void __stdcall RuleDelete(IAIELinguisticFuzzyRulebase* FuzzyRulebase, IAIERandomGenerator64* RandomGenerator)
{
 if (FuzzyRulebase->GetFuzzyRuleCount() < 2) return;

 // - losowanie regu³y
 int FuzzyRuleIndex = (int)RandomGenerator->RandomInt(0, FuzzyRulebase->GetFuzzyRuleCount() - 1);

 // - usuniêcie regu³y
 FuzzyRulebase->DeleteFuzzyRule(FuzzyRuleIndex);
}
//---------------------------------------------------------------------------
// RuleFuzzySetChange - losowa zmiana jednej przes³anki lub konkluzji w wybranej regule
//---------------------------------------------------------------------------
void __stdcall RuleFuzzySetChange(IAIELinguisticFuzzyRule* FuzzyRule, bool OnlyFullFuzzyRule, bool OnlyTrueFuzzyRule, IAIERandomGenerator64* RandomGenerator)
{
 // - losowanie atrybutu wejœciowego lub wyjœciowego
 int FuzzyAttrIndex = (int)RandomGenerator->RandomInt(0, FuzzyRule->GetInpFuzzyAttrCount());
 if (FuzzyAttrIndex < FuzzyRule->GetInpFuzzyAttrCount()) {
	// - losowa zmiana jednej przes³anki w wybranej regule
	int FuzzySetCount = FuzzyRule->GetInpFuzzySetCount(FuzzyAttrIndex);
	int FuzzySetIndex = (int)RandomGenerator->RandomInt(OnlyFullFuzzyRule ? 0 : -1, FuzzySetCount - 1);
	int Count = 10;
	if (OnlyFullFuzzyRule) while (Count-- && (FuzzySetIndex == FuzzyRule->GetInpFuzzySetIndex(FuzzyAttrIndex))) FuzzySetIndex = (int)RandomGenerator->RandomInt(0, FuzzySetCount - 1);
					  else while (Count-- && (FuzzySetIndex == FuzzyRule->GetInpFuzzySetIndex(FuzzyAttrIndex))) FuzzySetIndex = (int)RandomGenerator->RandomInt(-1, FuzzySetCount - 1);
	FuzzyRule->SetInpFuzzySetIndex(FuzzyAttrIndex, FuzzySetIndex);
	FuzzyRule->SetInpFuzzySetNegated(FuzzyAttrIndex, (OnlyTrueFuzzyRule ? false : ((int)RandomGenerator->RandomInt(0, 1) == 1)));
 } else {
	// - losowa zmiana konkluzji w wybranej regule
	int FuzzySetCount = FuzzyRule->GetOutFuzzySetCount();
	int FuzzySetIndex = (int)RandomGenerator->RandomInt(0, FuzzySetCount - 1);
	int Count = 10;
	while (Count-- && (FuzzySetIndex == FuzzyRule->GetOutFuzzySetIndex())) FuzzySetIndex = (int)RandomGenerator->RandomInt(0, FuzzySetCount - 1);
	FuzzyRule->SetOutFuzzySetIndex(FuzzySetIndex);
 }
}
//---------------------------------------------------------------------------
// RuleFuzzyAttrChange - losowa zmiana jednego wejœcia w wybranej regule
//---------------------------------------------------------------------------
void __stdcall RuleFuzzyAttrChange(IAIELinguisticFuzzyRule* FuzzyRule, IAIERandomGenerator64* RandomGenerator)
{
 int LFuzzyRuleLength = FuzzyRule->GetLength();
 if (LFuzzyRuleLength < 1) return;
 int LInpFuzzyAttrCount = FuzzyRule->GetInpFuzzyAttrCount();
 if (LInpFuzzyAttrCount < 2) return;
 if (LInpFuzzyAttrCount == LFuzzyRuleLength) return;

 // - losowanie w³¹czonego atrybutu wejœciowego
 int EnabledFuzzyAttrIndex = -1;
 int EnabledInputIndex = (int)RandomGenerator->RandomInt(1, LFuzzyRuleLength);
 while (EnabledInputIndex) if (FuzzyRule->GetInpFuzzySetIndex(++EnabledFuzzyAttrIndex) >= 0) --EnabledInputIndex;

 // - losowanie wy³¹czonego atrybutu wejœciowego
 int DisabledFuzzyAttrIndex = -1;
 int DisabledInputIndex = (int)RandomGenerator->RandomInt(1, LInpFuzzyAttrCount - LFuzzyRuleLength);
 while (DisabledInputIndex) if (FuzzyRule->GetInpFuzzySetIndex(++DisabledFuzzyAttrIndex) < 0) --DisabledInputIndex;

 // - zamiana wejœæ
 FuzzyRule->SetInpFuzzySetIndex(DisabledFuzzyAttrIndex, FuzzyRule->GetInpFuzzySetIndex(EnabledFuzzyAttrIndex));
 FuzzyRule->SetInpFuzzySetIndex(EnabledFuzzyAttrIndex, -1);
}
//---------------------------------------------------------------------------
// MembershipParamsChange - mutacja parametrów zbioru rozmytego
//---------------------------------------------------------------------------
void __stdcall MembershipParamsChange(IAIELinguisticFuzzyCollection* FuzzyCollection, IAIERandomGenerator64* RandomGenerator)
{
 // - wybór zbioru rozmytego
 int FuzzySetIndex = (int)RandomGenerator->RandomInt(0, FuzzyCollection->GetFuzzySetCount() - 1);
 IAIELinguisticFuzzySet* FuzzySet = FuzzyCollection->GetFuzzySet(FuzzySetIndex);

 double LDelta = 0.2 * (FuzzyCollection->GetMaxValue() - FuzzyCollection->GetMinValue()) * RandomGenerator->RandomDbl(-1.0, 1.0);
 switch (RandomGenerator->RandomInt(0, 5)) {
	 case 0: FuzzySet->SetLCenter(FuzzySet->GetLCenter() + LDelta); break;
	 case 1: FuzzySet->SetRCenter(FuzzySet->GetRCenter() + LDelta); break;
	 case 2: FuzzySet->SetLAlpha(FuzzySet->GetLAlpha() + LDelta); break;
	 case 3: FuzzySet->SetRAlpha(FuzzySet->GetRAlpha() + LDelta); break;
	 case 4: FuzzySet->SetLContrast(FuzzySet->GetLContrast() + RandomGenerator->RandomDbl(-0.1, 0.1)); break;
	 case 5: FuzzySet->SetRContrast(FuzzySet->GetRContrast() + RandomGenerator->RandomDbl(-0.1, 0.1)); break;
 }
}
//---------------------------------------------------------------------------
// MembershipTypeChange - mutacja typu funkcji przynale¿noœci zbiorów rozmytych
//---------------------------------------------------------------------------
void __stdcall MembershipTypeChange(IAIELinguisticFuzzyCollection* FuzzyCollection, IAIERandomGenerator64* RandomGenerator)
{
 // - wybór zbioru rozmytego
 int FuzzySetIndex = (int)RandomGenerator->RandomInt(0, FuzzyCollection->GetFuzzySetCount() - 1);
 IAIELinguisticFuzzySet* FuzzySet = FuzzyCollection->GetFuzzySet(FuzzySetIndex);

 // -- mutacja CoreEnabled, ContrastEnabled lub typu funkcji
 switch (RandomGenerator->RandomInt(0, 1)) { //wy³¹czony kontrast
	case 0: switch (RandomGenerator->RandomInt(1, 4)) {
				case 1: FuzzySet->SetMembershipFunction(fuzzyMembershipFunction1); break;
				case 2: FuzzySet->SetMembershipFunction(fuzzyMembershipFunction2); break;
				case 3: FuzzySet->SetMembershipFunction(fuzzyMembershipFunction3); break;
				case 4: FuzzySet->SetMembershipFunction(fuzzyMembershipFunction4); break;
			}
			break;
	case 1: FuzzySet->SetCoreEnabled(!FuzzySet->GetCoreEnabled());
			break;
	case 2: //FuzzySet->SetContrastEnabled(!FuzzySet->GetContrastEnabled());
			break;
 }
}
//---------------------------------------------------------------------------
// InferenceOperatorsChange - mutacja operatorów modu³u wnioskowania
//---------------------------------------------------------------------------
void __stdcall InferenceOperatorsChange(IAIELinguisticFuzzyRulebase* FuzzyRulebase, IAIERandomGenerator64* RandomGenerator)
{
 // - losowanie atrybutu
 int ParamIndex = (int)RandomGenerator->RandomInt(0, 5);
 if (ParamIndex == 0)  //mutacja operatora t-tmormy
	 switch (RandomGenerator->RandomInt(0, 5)) {
		case 0: FuzzyRulebase->SetTNormType(fuzzyTNorm1); break;
		case 1: FuzzyRulebase->SetTNormType(fuzzyTNorm2); break;
		case 2: FuzzyRulebase->SetTNormType(fuzzyTNorm3); break;
		case 3: FuzzyRulebase->SetTNormType(fuzzyTNorm4); break;
		case 4: FuzzyRulebase->SetTNormType(fuzzyTNorm5); break;
		case 5: FuzzyRulebase->SetTNormType(fuzzyTNorm6); break;
	 } else
 if (ParamIndex == 1)  //mutacja operatora s-tmormy
	 switch (RandomGenerator->RandomInt(0, 5)) {
		case 0: FuzzyRulebase->SetSNormType(fuzzySNorm1); break;
		case 1: FuzzyRulebase->SetSNormType(fuzzySNorm2); break;
		case 2: FuzzyRulebase->SetSNormType(fuzzySNorm3); break;
		case 3: FuzzyRulebase->SetSNormType(fuzzySNorm4); break;
		case 4: FuzzyRulebase->SetSNormType(fuzzySNorm5); break;
		case 5: FuzzyRulebase->SetSNormType(fuzzySNorm6); break;
	 } else
 if (ParamIndex == 2)  //mutacja operatora implikacji
	 switch (RandomGenerator->RandomInt(0, 11)) {
		case 0: FuzzyRulebase->SetImplicationOperator(fuzzyImplicationOperator1); break;
		case 1: FuzzyRulebase->SetImplicationOperator(fuzzyImplicationOperator2); break;
		case 2: FuzzyRulebase->SetImplicationOperator(fuzzyImplicationOperator3); break;
		case 3: FuzzyRulebase->SetImplicationOperator(fuzzyImplicationOperator4); break;
		case 4: FuzzyRulebase->SetImplicationOperator(fuzzyImplicationOperator5); break;
		case 5: FuzzyRulebase->SetImplicationOperator(fuzzyImplicationOperator6); break;
		case 6: FuzzyRulebase->SetImplicationOperator(fuzzyImplicationOperator7); break;
		case 7: FuzzyRulebase->SetImplicationOperator(fuzzyImplicationOperator8); break;
		case 8: FuzzyRulebase->SetImplicationOperator(fuzzyImplicationOperator9); break;
		case 9: FuzzyRulebase->SetImplicationOperator(fuzzyImplicationOperator10); break;
		case 10:FuzzyRulebase->SetImplicationOperator(fuzzyImplicationOperator11); break;
		case 11:FuzzyRulebase->SetImplicationOperator(fuzzyImplicationOperator12); break;
	 } else
 if (ParamIndex == 3)  //mutacja operatora agregacji
	 switch (RandomGenerator->RandomInt(0, 4)) {
		case 0: FuzzyRulebase->SetAggregationOperator(fuzzyAggregationOperator1); break;
		case 1: FuzzyRulebase->SetAggregationOperator(fuzzyAggregationOperator2); break;
		case 2: FuzzyRulebase->SetAggregationOperator(fuzzyAggregationOperator3); break;
		case 3: FuzzyRulebase->SetAggregationOperator(fuzzyAggregationOperator4); break;
		case 4: FuzzyRulebase->SetAggregationOperator(fuzzyAggregationOperator5); break;
	 } else
 if (ParamIndex == 4)  //mutacja operatora defuzyfikacji
	 switch (RandomGenerator->RandomInt(0, 4)) {
		case 0: FuzzyRulebase->SetDefuzzificationMethod(fuzzyDefuzzificationCOG); break;
		case 1: FuzzyRulebase->SetDefuzzificationMethod(fuzzyDefuzzificationCOA); break;
		case 2: FuzzyRulebase->SetDefuzzificationMethod(fuzzyDefuzzificationFOM); break;
		case 3: FuzzyRulebase->SetDefuzzificationMethod(fuzzyDefuzzificationMOM); break;
		case 4: FuzzyRulebase->SetDefuzzificationMethod(fuzzyDefuzzificationLOM); break;
	 } else
 if (ParamIndex == 5)  //mutacja trybu pracy
	 switch (RandomGenerator->RandomInt(0, 1)) {
		case 0: FuzzyRulebase->SetInferenceMode(fuzzyInferenceModeFATI); break;
		case 1: FuzzyRulebase->SetInferenceMode(fuzzyInferenceModeFITA); break;
	 }
}
//---------------------------------------------------------------------------
