//---------------------------------------------------------------------------
#ifndef UAIELinguisticFuzzyStructureGeneticOperators1H
#define UAIELinguisticFuzzyStructureGeneticOperators1H
//---------------------------------------------------------------------------
// FUNKCJE DLA OPTYMALIZACJI BAZY DANYCH I BAZY REGU£
// Z WYKORZYSTANIEM ALGORYTMU GENETYCZNEGO
// BEZ BINARNEGO KODOWANIA BAZY DANYCH
//---------------------------------------------------------------------------
// OPERATORY GENETYCZNE - OPERATORY KRZY¯OWANIA
//---------------------------------------------------------------------------
// RulebaseExchange1 - Wymiana wielu regu³ w bazach regu³ (umieszczonych na tych samych miejscach)
// (dzia³anie analogiczne do binarnego operatora wielopunktowego)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange1(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// RulebaseExchange2 - Wymiana jednej regu³y w bazach regu³ (umieszczonych na tych samych miejscach)
// (dzia³anie analogiczne do binarnego operatora jednopunktowego)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange2(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// RulebaseExchange3 - Wymiana przes³anek w wielu losowo wybranych regu³ach (umieszczonych na tych samych miejscach)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange3(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// RulebaseExchange4 - Wymiana przes³anek w jednej parze losowo wybranych regu³ (umieszczonych na tych samych miejscach)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange4(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// RulebaseExchange5 - Wymiana jednej przes³anki w jednej parze losowo wybranych regu³ (umieszczonych na tych samych miejscach)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange5(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// AttrExchange - wymiana parametrów funkcji przynale¿noœci w atrybutach
//---------------------------------------------------------------------------
void __stdcall AttrExchange(IAIELinguisticFuzzyCollection* FuzzyCollection1, IAIELinguisticFuzzyCollection* FuzzyCollection2, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// OPERATORY GENETYCZNE - OPERATORY MUTACJI
//---------------------------------------------------------------------------
// RuleInsert - dodanie nowej, losowej regu³y
//---------------------------------------------------------------------------
void __stdcall RuleInsert(IAIELinguisticFuzzyRulebase* FuzzyRulebase, bool OnlyFullFuzzyRule, bool OnlyTrueFuzzyRule, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// RuleDelete - usuwanie regu³y
//---------------------------------------------------------------------------
void __stdcall RuleDelete(IAIELinguisticFuzzyRulebase* FuzzyRulebase, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// RuleFuzzySetChange - losowa zmiana jednej przes³anki w wybranej regule
//---------------------------------------------------------------------------
void __stdcall RuleFuzzySetChange(IAIELinguisticFuzzyRule* FuzzyRule, bool OnlyFullFuzzyRule, bool OnlyTrueFuzzyRule, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// RuleFuzzyAttrChange - losowa zmiana jednego wejœcia w wybranej regule
//---------------------------------------------------------------------------
void __stdcall RuleFuzzyAttrChange(IAIELinguisticFuzzyRule* FuzzyRule, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// MembershipParamsChange - mutacja parametrów funkcji przynale¿noœci zbiorów rozmytych
//---------------------------------------------------------------------------
void __stdcall MembershipParamsChange(IAIELinguisticFuzzyCollection* FuzzyCollection, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// MembershipTypeChange - mutacja typu funkcji przynale¿noœci zbiorów rozmytych
//---------------------------------------------------------------------------
void __stdcall MembershipTypeChange(IAIELinguisticFuzzyCollection* FuzzyCollection, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// InferenceOperatorsChange - mutacja operatorów modu³u wnioskowania
//---------------------------------------------------------------------------
void __stdcall InferenceOperatorsChange(IAIELinguisticFuzzyRulebase* FuzzyRulebase, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
#endif
