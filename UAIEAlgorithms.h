//---------------------------------------------------------------------------
#ifndef UAIEAlgorithmsH
#define UAIEAlgorithmsH
//---------------------------------------------------------------------------
#include "IAIEAlgorithms.h"
//---------------------------------------------------------------------------
class TAIESystem;
//---------------------------------------------------------------------------
class TAIEAlgorithms : public IAIEAlgorithms
{
 private:
		TAIESystem* FSystem;

 public:
		__stdcall TAIEAlgorithms(TAIESystem* ASystem);
		__stdcall ~TAIEAlgorithms(void);

		IAIERandomPermutation* __stdcall InitRandomPermutation(unsigned int N);
		IAIEVoronoiGenerator*  __stdcall InitVoronoiGenerator(void);

		IAIEDataSetsPreprocess* __stdcall InitDataSetsPreprocess(IAIEDataSet* RefDataSet);
		IAIEDataSetsPartition* __stdcall InitDataSetsPartition(void);

		IAIESelfOrganizingNet* __stdcall InitSelfOrganizingNet(void);
		IAIESelfOrganizingChain* __stdcall InitSelfOrganizingChain(void);
		IAIESelfOrganizingMap* __stdcall InitSelfOrganizingMap(void);
		IAIESelfOrganizingTree* __stdcall InitSelfOrganizingTree(void);
		IAIESONNStructure* __stdcall InitSelfOrganizingNeuralNetwork(void);
		IAIESelfOrganizingNetWTMAlgorithm* __stdcall InitSelfOrganizingNetWTMAlgorithm(void);
		IAIESelfOrganizingChainWTMAlgorithm* __stdcall InitSelfOrganizingChainWTMAlgorithm(void);
		IAIESelfOrganizingMapWTMAlgorithm* __stdcall InitSelfOrganizingMapWTMAlgorithm(void);
		IAIESelfOrganizingTreeWTMAlgorithm* __stdcall InitSelfOrganizingTreeWTMAlgorithm(void);
		IAIESelfOrganizingNeuralNetworkWTMAlgorithm* __stdcall InitSelfOrganizingNeuralNetworkLearningAlgorithm(const char* Name);

		IAIEMultilayerPerceptron* __stdcall InitMultilayerPerceptron(void);
		IAIEMultilayerPerceptronLrnAlgorithm* __stdcall InitMultilayerPerceptronLrnAlgorithm(void);

		IAIELinguisticFuzzyStructure* __stdcall InitLinguisticFuzzyStructure(void);
		IAIELinguisticFuzzyStructureLrnAlgorithm* __stdcall InitLinguisticFuzzyStructureLrnAlgorithm(void);
		IAIELinguisticFuzzyStructureMOALrnAlgorithm* __stdcall InitLinguisticFuzzyStructureMOALrnAlgorithm(void);
};
//---------------------------------------------------------------------------
#endif
