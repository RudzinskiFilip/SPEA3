//---------------------------------------------------------------------------
#include <windows.h>

#include "UAIESystem.h"
#include "UAIEAlgorithms.h"
#include "UAIERandomPermutation.h"
#include "UAIEVoronoiGenerator.h"
#include "UAIEDataSetsPreprocess.h"
#include "UAIEDataSetsPartition.h"
#include "UAIESelfOrganizingTypes.h"
#include "UAIESelfOrganizingNet.h"
#include "UAIESelfOrganizingMap.h"
#include "UAIESelfOrganizingChain.h"
#include "UAIESelfOrganizingTree.h"
#include "UAIESelfOrganizingNetWTMAlgorithm.h"
#include "UAIESelfOrganizingMapWTMAlgorithm.h"
#include "UAIESelfOrganizingChainWTMAlgorithm.h"
#include "UAIESelfOrganizingTreeWTMAlgorithm.h"
#include "UAIESelfOrganizingNeuralNetwork.h"
#include "UAIESelfOrganizingNeuralNetworkWTMAlgorithm.h"
#include "UAIESelfOrganizingNeuralNetworkIGGAlgorithm.h"
#include "UAIESelfOrganizingNeuralNetworkGNGAlgorithm.h"
#include "UAIEMultilayerPerceptron.h"
#include "UAIEMultilayerPerceptronLrnAlgorithm.h"
#include "UAIELinguisticFuzzyStructure.h"
#include "UAIELinguisticFuzzyStructureLrnAlgorithm.h"
#include "UAIELinguisticFuzzyStructureMOALrnAlgorithm.h"
//---------------------------------------------------------------------------
__stdcall TAIEAlgorithms::TAIEAlgorithms(TAIESystem* ASystem)
{
 FSystem = ASystem;
}
//---------------------------------------------------------------------------
__stdcall TAIEAlgorithms::~TAIEAlgorithms(void)
{
}
//---------------------------------------------------------------------------
IAIERandomPermutation* __stdcall TAIEAlgorithms::InitRandomPermutation(unsigned int N)
{
 return new TAIERandomPermutation(N);
}
//---------------------------------------------------------------------------
IAIEVoronoiGenerator* __stdcall TAIEAlgorithms::InitVoronoiGenerator(void)
{
 return new TAIEVoronoiGenerator();
}
//---------------------------------------------------------------------------
IAIEDataSetsPreprocess* __stdcall TAIEAlgorithms::InitDataSetsPreprocess(IAIEDataSet* RefDataSet)
{
 return new TAIEDataSetsPreprocess(FSystem, RefDataSet);
}
//---------------------------------------------------------------------------
IAIEDataSetsPartition* __stdcall TAIEAlgorithms::InitDataSetsPartition(void)
{
 return new TAIEDataSetsPartition(FSystem);
}
//---------------------------------------------------------------------------
IAIESelfOrganizingNet* __stdcall TAIEAlgorithms::InitSelfOrganizingNet(void)
{
 return new TAIESelfOrganizingNet(FSystem);
}
//---------------------------------------------------------------------------
IAIESelfOrganizingChain* __stdcall TAIEAlgorithms::InitSelfOrganizingChain(void)
{
 return new TAIESelfOrganizingChain(FSystem);
}
//---------------------------------------------------------------------------
IAIESelfOrganizingMap* __stdcall TAIEAlgorithms::InitSelfOrganizingMap(void)
{
 return new TAIESelfOrganizingMap(FSystem);
}
//---------------------------------------------------------------------------
IAIESelfOrganizingTree* __stdcall TAIEAlgorithms::InitSelfOrganizingTree(void)
{
 return new TAIESelfOrganizingTree(FSystem);
}
//---------------------------------------------------------------------------
IAIESONNStructure* __stdcall TAIEAlgorithms::InitSelfOrganizingNeuralNetwork(void)
{
 return new TAIESONNStructure(FSystem);
}
//---------------------------------------------------------------------------
IAIESelfOrganizingNetWTMAlgorithm* __stdcall TAIEAlgorithms::InitSelfOrganizingNetWTMAlgorithm(void)
{
 return new NAIESelfOrganizingNetWTMAlgorithm::TAIESelfOrganizingNetWTMAlgorithm(FSystem);
}
//---------------------------------------------------------------------------
IAIESelfOrganizingChainWTMAlgorithm* __stdcall TAIEAlgorithms::InitSelfOrganizingChainWTMAlgorithm(void)
{
 return new NAIESelfOrganizingChainWTMAlgorithm::TAIESelfOrganizingChainWTMAlgorithm(FSystem);
}
//---------------------------------------------------------------------------
IAIESelfOrganizingMapWTMAlgorithm* __stdcall TAIEAlgorithms::InitSelfOrganizingMapWTMAlgorithm(void)
{
 return new NAIESelfOrganizingMapWTMAlgorithm::TAIESelfOrganizingMapWTMAlgorithm(FSystem);
}
//---------------------------------------------------------------------------
IAIESelfOrganizingTreeWTMAlgorithm* __stdcall TAIEAlgorithms::InitSelfOrganizingTreeWTMAlgorithm(void)
{
 return new NAIESelfOrganizingTreeWTMAlgorithm::TAIESelfOrganizingTreeWTMAlgorithm(FSystem);
}
//---------------------------------------------------------------------------
IAIESelfOrganizingNeuralNetworkWTMAlgorithm* __stdcall TAIEAlgorithms::InitSelfOrganizingNeuralNetworkLearningAlgorithm(const char* Name)
{
 if (!strcmp(Name, "WTM")) return new TAIESelfOrganizingNeuralNetworkWTMAlgorithm(FSystem);
 if (!strcmp(Name, "IGG")) return new TAIESelfOrganizingNeuralNetworkIGGAlgorithm(FSystem);
 if (!strcmp(Name, "GNG")) return new TAIESelfOrganizingNeuralNetworkGNGAlgorithm(FSystem);
 return NULL;
}
//---------------------------------------------------------------------------
IAIEMultilayerPerceptron* __stdcall TAIEAlgorithms::InitMultilayerPerceptron(void)
{
 return new TAIEMultilayerPerceptron(FSystem);
}
//---------------------------------------------------------------------------
IAIEMultilayerPerceptronLrnAlgorithm* __stdcall TAIEAlgorithms::InitMultilayerPerceptronLrnAlgorithm(void)
{
 return new TAIEMultilayerPerceptronLrnAlgorithm(FSystem);
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyStructure* __stdcall TAIEAlgorithms::InitLinguisticFuzzyStructure(void)
{
 return new TAIELinguisticFuzzyStructure(FSystem, 0);
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyStructureLrnAlgorithm* __stdcall TAIEAlgorithms::InitLinguisticFuzzyStructureLrnAlgorithm(void)
{
 return new TAIELinguisticFuzzyStructureLrnAlgorithm(FSystem);
}
//---------------------------------------------------------------------------
IAIELinguisticFuzzyStructureMOALrnAlgorithm* __stdcall TAIEAlgorithms::InitLinguisticFuzzyStructureMOALrnAlgorithm(void)
{
 return new TAIELinguisticFuzzyStructureMOALrnAlgorithm(FSystem);
}
//---------------------------------------------------------------------------

