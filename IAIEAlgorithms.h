//---------------------------------------------------------------------------
#ifndef IAIEAlgorithmsH
#define IAIEAlgorithmsH
//---------------------------------------------------------------------------
// IAIEAlgorithms.h
// Copyright 2007-2008 by Filip Rudziñski
// -
// Algorithms interface
//---------------------------------------------------------------------------
class IAIEDataSet;
class IAIERandomPermutation;
class IAIEVoronoiGenerator;
class IAIEDataSetsPreprocess;
class IAIEDataSetsPartition;
class IAIESelfOrganizingNet;
class IAIESelfOrganizingChain;
class IAIESelfOrganizingMap;
class IAIESelfOrganizingTree;
class IAIESelfOrganizingNetWTMAlgorithm;
class IAIESelfOrganizingChainWTMAlgorithm;
class IAIESelfOrganizingMapWTMAlgorithm;
class IAIESelfOrganizingTreeWTMAlgorithm;
class IAIESelfOrganizingNeuralNetworkWTMAlgorithm;
class IAIESONNStructure;
class IAIEMultilayerPerceptron;
class IAIEMultilayerPerceptronLrnAlgorithm;
class IAIELinguisticFuzzyStructure;
class IAIELinguisticFuzzyStructureLrnAlgorithm;
class IAIELinguisticFuzzyStructureMOALrnAlgorithm;
//---------------------------------------------------------------------------
class IAIEAlgorithms
{
 public:
		//FUNKCJA TWORZY PERMUTACJÊ LICZB BEZ POWTÓRZEÑ
		virtual IAIERandomPermutation* __stdcall InitRandomPermutation(unsigned int N) = 0;

		//FUNKCJA TWORZY GENERATOR WIELOBOKÓW VORONOI'A DLA DANYCH DWUWYMIAROWYCH
		virtual IAIEVoronoiGenerator* __stdcall InitVoronoiGenerator(void) = 0;

		// FUNKCJA TWORZY NARZÊDZIE DO WSTÊPNEGO PRZETWARZANIA DANYCH
		virtual IAIEDataSetsPreprocess* __stdcall InitDataSetsPreprocess(IAIEDataSet* RefDataSet) = 0;

		// FUNKCJA TWORZY NARZÊDZIE DO PODZIA£U ZBIORÓW DANYCH
		virtual IAIEDataSetsPartition* __stdcall InitDataSetsPartition(void) = 0;

		// FUNKCJA TWORZY SIEÆ NEURONOW¥ SAMOORGANIZUJ¥C¥ SIÊ BEZ S¥SIEDZTWA TOPOLOGICZNEGO
		virtual IAIESelfOrganizingNet* __stdcall InitSelfOrganizingNet(void) = 0;
		// FUNKCJA TWORZY SIEÆ NEURONOW¥ SAMOORGANIZUJ¥C¥ SIÊ Z JEDNOWYMIAROWYM S¥SIEDZTWEM
		// Z MO¯LIWOŒCI¥ DZIELENIA £AÑCUCHA SIECI NA POD£AÑCUCHY
		virtual IAIESelfOrganizingChain* __stdcall InitSelfOrganizingChain(void) = 0;
		// FUNKCJA TWORZY SIEÆ NEURONOW¥ SAMOORGANIZUJ¥C¥ Z DWUWYMIAROWYM S¥SIEDZTWEM
		virtual IAIESelfOrganizingMap* __stdcall InitSelfOrganizingMap(void) = 0;
		// FUNKCJA TWORZY SIEÆ NEURONOW¥ SAMOORGANIZUJ¥C¥ SIÊ Z JEDNOWYMIAROWYM S¥SIEDZTWEM
		// Z MO¯LIWOŒCI¥ REORGANIZACJI STRUKTURY £AÑCUCHA SIECI DO POSTACI DRZEWA
		virtual IAIESelfOrganizingTree* __stdcall InitSelfOrganizingTree(void) = 0;

		virtual IAIESONNStructure* __stdcall InitSelfOrganizingNeuralNetwork(void) = 0;
		virtual IAIESelfOrganizingNeuralNetworkWTMAlgorithm* __stdcall InitSelfOrganizingNeuralNetworkLearningAlgorithm(const char* Name) = 0;

		// FUNKCJA TWORZ¥CA ALGORYTM WTM DO UCZENIA SIECI SAMOORGANIZUJ¥CYCH SIÊ
		virtual IAIESelfOrganizingNetWTMAlgorithm* __stdcall InitSelfOrganizingNetWTMAlgorithm(void) = 0;
		virtual IAIESelfOrganizingChainWTMAlgorithm* __stdcall InitSelfOrganizingChainWTMAlgorithm(void) = 0;
		virtual IAIESelfOrganizingMapWTMAlgorithm* __stdcall InitSelfOrganizingMapWTMAlgorithm(void) = 0;
		virtual IAIESelfOrganizingTreeWTMAlgorithm* __stdcall InitSelfOrganizingTreeWTMAlgorithm(void) = 0;

		// FUNKCJA TWORZY SIEÆ NEURONOW¥ TYPU PERCEPTRON WIELOWARSTWOWY
		virtual IAIEMultilayerPerceptron* __stdcall InitMultilayerPerceptron(void) = 0;
		// FUNKCJA TWORZ¥CA ALGORYTM UCZENIA SIECI NEURONOWEJ TYPU PERCEPTRON WIELOWARSTWOWY
		virtual IAIEMultilayerPerceptronLrnAlgorithm* __stdcall InitMultilayerPerceptronLrnAlgorithm(void) = 0;

		// FUNKCJA TWORZY STRUKTURÊ SYSTEMU REGU£OWO-ROZMYTEGO
		virtual IAIELinguisticFuzzyStructure* __stdcall InitLinguisticFuzzyStructure(void) = 0;
		// FUNKCJA TWORZ¥CA ALGORYTM UCZENIA SYSTEMU REGU£OWO-ROZMYTEGO
		virtual IAIELinguisticFuzzyStructureLrnAlgorithm* __stdcall InitLinguisticFuzzyStructureLrnAlgorithm(void) = 0;
		// FUNKCJA TWORZ¥CA ALGORYTM UCZENIA SYSTEMU REGU£OWO-ROZMYTEGO (OPTYMALIZACJA WIELOKRYTERIALNA)
		virtual IAIELinguisticFuzzyStructureMOALrnAlgorithm* __stdcall InitLinguisticFuzzyStructureMOALrnAlgorithm(void) = 0;
};
//---------------------------------------------------------------------------
#endif