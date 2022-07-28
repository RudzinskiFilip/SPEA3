//---------------------------------------------------------------------------
#ifndef UAIELinguisticFuzzyStructureGeneticOperators1H
#define UAIELinguisticFuzzyStructureGeneticOperators1H
//---------------------------------------------------------------------------
// FUNKCJE DLA OPTYMALIZACJI BAZY DANYCH I BAZY REGU�
// Z WYKORZYSTANIEM ALGORYTMU GENETYCZNEGO
// BEZ BINARNEGO KODOWANIA BAZY DANYCH
//---------------------------------------------------------------------------
// OPERATORY GENETYCZNE - OPERATORY KRZY�OWANIA
//---------------------------------------------------------------------------
// RulebaseExchange1 - Wymiana wielu regu� w bazach regu� (umieszczonych na tych samych miejscach)
// (dzia�anie analogiczne do binarnego operatora wielopunktowego)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange1(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// RulebaseExchange2 - Wymiana jednej regu�y w bazach regu� (umieszczonych na tych samych miejscach)
// (dzia�anie analogiczne do binarnego operatora jednopunktowego)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange2(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// RulebaseExchange3 - Wymiana przes�anek w wielu losowo wybranych regu�ach (umieszczonych na tych samych miejscach)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange3(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// RulebaseExchange4 - Wymiana przes�anek w jednej parze losowo wybranych regu� (umieszczonych na tych samych miejscach)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange4(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// RulebaseExchange5 - Wymiana jednej przes�anki w jednej parze losowo wybranych regu� (umieszczonych na tych samych miejscach)
//---------------------------------------------------------------------------
void __stdcall RulebaseExchange5(IAIELinguisticFuzzyRulebase* FuzzyRulebase1, IAIELinguisticFuzzyRulebase* FuzzyRulebase2, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// AttrExchange - wymiana parametr�w funkcji przynale�no�ci w atrybutach
//---------------------------------------------------------------------------
void __stdcall AttrExchange(IAIELinguisticFuzzyCollection* FuzzyCollection1, IAIELinguisticFuzzyCollection* FuzzyCollection2, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// OPERATORY GENETYCZNE - OPERATORY MUTACJI
//---------------------------------------------------------------------------
// RuleInsert - dodanie nowej, losowej regu�y
//---------------------------------------------------------------------------
void __stdcall RuleInsert(IAIELinguisticFuzzyRulebase* FuzzyRulebase, bool OnlyFullFuzzyRule, bool OnlyTrueFuzzyRule, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// RuleDelete - usuwanie regu�y
//---------------------------------------------------------------------------
void __stdcall RuleDelete(IAIELinguisticFuzzyRulebase* FuzzyRulebase, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// RuleFuzzySetChange - losowa zmiana jednej przes�anki w wybranej regule
//---------------------------------------------------------------------------
void __stdcall RuleFuzzySetChange(IAIELinguisticFuzzyRule* FuzzyRule, bool OnlyFullFuzzyRule, bool OnlyTrueFuzzyRule, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// RuleFuzzyAttrChange - losowa zmiana jednego wej�cia w wybranej regule
//---------------------------------------------------------------------------
void __stdcall RuleFuzzyAttrChange(IAIELinguisticFuzzyRule* FuzzyRule, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// MembershipParamsChange - mutacja parametr�w funkcji przynale�no�ci zbior�w rozmytych
//---------------------------------------------------------------------------
void __stdcall MembershipParamsChange(IAIELinguisticFuzzyCollection* FuzzyCollection, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// MembershipTypeChange - mutacja typu funkcji przynale�no�ci zbior�w rozmytych
//---------------------------------------------------------------------------
void __stdcall MembershipTypeChange(IAIELinguisticFuzzyCollection* FuzzyCollection, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// InferenceOperatorsChange - mutacja operator�w modu�u wnioskowania
//---------------------------------------------------------------------------
void __stdcall InferenceOperatorsChange(IAIELinguisticFuzzyRulebase* FuzzyRulebase, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
#endif
