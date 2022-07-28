//---------------------------------------------------------------------------
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "UTemplates.h"
#include "UDynamicPointers.h"
#include "UAIESystem.h"
#include "UAIERandomGenerator64.h"

#include "UAIELinguisticFuzzyStructureGeneticOperators2.h"
//---------------------------------------------------------------------------
unsigned int __stdcall BinaryEncode(double Value, double Max, double Min)
{
 return (unsigned int)(MAX_UINT32_VALUE * ((Value - Min) / (Max - Min)));
}
//---------------------------------------------------------------------------
double __stdcall BinaryDecode(unsigned int Value, double Max, double Min)
{
 return ((double)Value / (double)MAX_UINT32_VALUE) * (Max - Min) + Min;
}
//---------------------------------------------------------------------------
// OPERATORY GENETYCZNE - OPERATORY KRZY¯OWANIA (PRACUJ¥CE NA BINARNEJ REPREZENTACJI BAZY DANYCH)
//---------------------------------------------------------------------------
// BinaryDataBaseExchange1 - krzy¿owanie dwupunktowe
//---------------------------------------------------------------------------
void __stdcall BinaryAttrExchange1(char* BD1, int BDSize1, char* BD2, int BDSize2, IAIERandomGenerator64* RandomGenerator)
{
 int Index1, Index2;
 if (BDSize1 > BDSize2) {
	 Index1 = (int)RandomGenerator->RandomInt(0, BDSize2 - 1);
	 Index2 = (int)RandomGenerator->RandomInt(0, BDSize2 - 1);
 } else {
	 Index1 = (int)RandomGenerator->RandomInt(0, BDSize1 - 1);
	 Index2 = (int)RandomGenerator->RandomInt(0, BDSize1 - 1);
 }
 if (Index1 != Index2) {
	if (Index1 > Index2) SWAP(Index1, Index2);

	TVector<char> Tmp(Index2 - Index1);
	memcpy(Tmp.GetData(), BD1 + Index1, Index2 - Index1);
	memcpy(BD1 + Index1, BD2 + Index1, Index2 - Index1);
	memcpy(BD2 + Index1, Tmp.GetData(), Index2 - Index1);

	char Mask1 = 0xFF << (int)RandomGenerator->RandomInt(0, 7);
	char Mask2 = 0xFF << (int)RandomGenerator->RandomInt(0, 7);

	char T1 = BD1[Index1];
	char T2 = BD1[Index2];
	BD1[Index1] = (T1 & ~Mask1) | (BD2[Index1] & Mask1);
	BD2[Index1] = (T1 & Mask1)  | (BD2[Index1] & ~Mask1);
	BD1[Index2] = (T2 & ~Mask2) | (BD2[Index2] & Mask2);
	BD2[Index2] = (T2 & Mask2)  | (BD2[Index2] & ~Mask2);
 } else {
	char Mask1 = 0xFF << (int)RandomGenerator->RandomInt(0, 7);
	char Mask2 = 0xFF << (int)RandomGenerator->RandomInt(0, 7);

	char T1 = BD1[Index1];
	BD1[Index1] = (T1 & ~Mask1) | (BD2[Index1] & Mask1);
	BD2[Index1] = (T1 & Mask1)  | (BD2[Index1] & ~Mask1);
	T1 = BD1[Index1];
	BD1[Index1] = (T1 & ~Mask2) | (BD2[Index1] & Mask2);
	BD2[Index1] = (T1 & Mask2)  | (BD2[Index1] & ~Mask2);
 }
}
//---------------------------------------------------------------------------
// OPERATORY GENETYCZNE - OPERATORY MUTACJI (PRACUJ¥CE NA BINARNEJ REPREZENTACJI BAZY DANYCH)
//---------------------------------------------------------------------------
// BinaryAttrMutation1 - mutacja jednego genu
//---------------------------------------------------------------------------
void __stdcall BinaryAttrMutation1(char* BD, int BDSize, IAIERandomGenerator64* RandomGenerator)
{
 int Index = (int)RandomGenerator->RandomInt(0, BDSize - 1);
 BD[Index] = BD[Index] ^ (0x01 << (int)RandomGenerator->RandomInt(0, 7));
}
//---------------------------------------------------------------------------
// BinaryAttrMutation2 - mutacja 4 najm³odzych genów
//---------------------------------------------------------------------------
void __stdcall BinaryAttrMutation2(char* BD, int BDSize, IAIERandomGenerator64* RandomGenerator)
{
 int Index = 4 * (int)RandomGenerator->RandomInt(0, BDSize / 4 - 1) + 3;
 BD[Index] = (BD[Index] & 0xF0) | ((int)RandomGenerator->RandomInt(0, 15) & 0x0F);
}
//---------------------------------------------------------------------------
// BinaryAttrMutation3 - mutacja 8 najm³odzych genów
//---------------------------------------------------------------------------
void __stdcall BinaryAttrMutation3(char* BD, int BDSize, IAIERandomGenerator64* RandomGenerator)
{
 int Index = 4 * (int)RandomGenerator->RandomInt(0, BDSize / 4 - 1);
 BD[Index + 3] = (int)RandomGenerator->RandomInt(0, 255);
}
//---------------------------------------------------------------------------
// BinaryAttrMutation4 - mutacja 16 najm³odzych genów
//---------------------------------------------------------------------------
void __stdcall BinaryAttrMutation4(char* BD, int BDSize, IAIERandomGenerator64* RandomGenerator)
{
 int Index = 4 * (int)RandomGenerator->RandomInt(0, BDSize / 4 - 1);
 BD[Index + 2] = (int)RandomGenerator->RandomInt(0, 255);
 BD[Index + 3] = (int)RandomGenerator->RandomInt(0, 255);
}
//---------------------------------------------------------------------------
// BinaryAttrMutation5 - mutacja 24 najm³odzych genów
//---------------------------------------------------------------------------
void __stdcall BinaryAttrMutation5(char* BD, int BDSize, IAIERandomGenerator64* RandomGenerator)
{
 int Index = 4 * (int)RandomGenerator->RandomInt(0, BDSize / 4 - 1);
 BD[Index + 1] = (int)RandomGenerator->RandomInt(0, 255);
 BD[Index + 2] = (int)RandomGenerator->RandomInt(0, 255);
 BD[Index + 3] = (int)RandomGenerator->RandomInt(0, 255);
}
//---------------------------------------------------------------------------
// BinaryAttrMutation6 - mutacja 32 najm³odzych genów
//---------------------------------------------------------------------------
void __stdcall BinaryAttrMutation6(char* BD, int BDSize, IAIERandomGenerator64* RandomGenerator)
{
 int Index = 4 * (int)RandomGenerator->RandomInt(0, BDSize / 4 - 1);
 BD[Index] = (int)RandomGenerator->RandomInt(0, 255);
 BD[Index + 1] = (int)RandomGenerator->RandomInt(0, 255);
 BD[Index + 2] = (int)RandomGenerator->RandomInt(0, 255);
 BD[Index + 3] = (int)RandomGenerator->RandomInt(0, 255);
}
//---------------------------------------------------------------------------
void __stdcall BinaryAttrMutation7(char* BD, int BDSize, IAIERandomGenerator64* RandomGenerator)
{
 //operator nie sprawdzi³ siê
 int Index = (int)RandomGenerator->RandomInt(0, BDSize - 2);
 BD[Index] = BD[Index] & 0xF0 + (int)RandomGenerator->RandomInt(0, 15);
 BD[Index + 1] = BD[Index + 1] & 0x0F + ((int)RandomGenerator->RandomInt(0, 15) << 4);
}
//---------------------------------------------------------------------------
