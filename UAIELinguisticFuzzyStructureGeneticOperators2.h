//---------------------------------------------------------------------------
#ifndef UAIELinguisticFuzzyStructureGeneticOperators2H
#define UAIELinguisticFuzzyStructureGeneticOperators2H
//---------------------------------------------------------------------------
// FUNKCJE DLA OPTYMALIZACJI BAZY DANYCH I BAZY REGU£
// Z WYKORZYSTANIEM ALGORYTMU GENETYCZNEGO
// Z BINARNYM KODOWANIEM BAZY DANYCH
unsigned int __stdcall BinaryEncode(double Value, double Max, double Min);
double __stdcall BinaryDecode(unsigned int Value, double Max, double Min);
//---------------------------------------------------------------------------
// OPERATORY GENETYCZNE - OPERATORY KRZY¯OWANIA
//---------------------------------------------------------------------------
void __stdcall BinaryAttrExchange1(char* BinaryDatabase1, int BinaryDatabaseSize1, char* BinaryDatabase2, int BinaryDatabaseSize2, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
// OPERATORY GENETYCZNE - OPERATORY MUTACJI
//---------------------------------------------------------------------------
void __stdcall BinaryAttrMutation1(char* BinaryDatabase, int BinaryDatabaseSize, IAIERandomGenerator64* RandomGenerator);
void __stdcall BinaryAttrMutation2(char* BinaryDatabase, int BinaryDatabaseSize, IAIERandomGenerator64* RandomGenerator);
void __stdcall BinaryAttrMutation3(char* BinaryDatabase, int BinaryDatabaseSize, IAIERandomGenerator64* RandomGenerator);
void __stdcall BinaryAttrMutation4(char* BinaryDatabase, int BinaryDatabaseSize, IAIERandomGenerator64* RandomGenerator);
void __stdcall BinaryAttrMutation5(char* BinaryDatabase, int BinaryDatabaseSize, IAIERandomGenerator64* RandomGenerator);
void __stdcall BinaryAttrMutation6(char* BinaryDatabase, int BinaryDatabaseSize, IAIERandomGenerator64* RandomGenerator);
void __stdcall BinaryAttrMutation7(char* BinaryDatabase, int BinaryDatabaseSize, IAIERandomGenerator64* RandomGenerator);
//---------------------------------------------------------------------------
#endif
