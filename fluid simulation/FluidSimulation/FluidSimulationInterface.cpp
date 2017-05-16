#include "FluidSimulationInterface.h"
#include "RunConfig.h"
#include "VoxelParticleGenerator.h"
#include "BoxParticleGenerator.h"
#include "OtherParticleGenerator.h"
#include "FluidManager.h"
#include "FluidParticle.h"
#include "SPHKernels.h"
#include "NeighborFinderFactoryData.h"
#include "NeighborhoodSearchSpatialHashing.h"

using namespace hiveFluidSimulation;

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::initParticles(const std::string& vRunConfig)
{
	_ASSERT(!vRunConfig.empty());
	CRunConfig::getInstance()->parseV(vRunConfig);

	std::string InitialStr= CRunConfig::getInstance()->getAttribute<std::string>(CONFIG_KEYWORD::PARTICLEINITIAL);
	EParticleInitial InitialUsage = parseInitalMethod(InitialStr);

	CBaseParticleGenerator* pFluidParticleGen    = NULL;
	CBaseParticleGenerator* pObstacleParticleGen = NULL;

	switch (InitialUsage)
	{
	case INITIAL_VOXEL:
		{
			std::string ModelName = CRunConfig::getInstance()->getAttribute<std::string>(CONFIG_KEYWORD::FLUIDMODELFILE);
			ModelName = strlwr(const_cast<char*>(ModelName.c_str()));
			setParticleGeneratorInfo4Voxel(ModelName, CRunConfig::getInstance()->getAttribute<float>(CONFIG_KEYWORD::PARTICLESPACING), ETypeParticle::FLUID, pFluidParticleGen);
		
			if (CRunConfig::getInstance()->isAttributeExisted(CONFIG_KEYWORD::CONTAINERMODELFILE))
			{
				std::string ContainerFile = CRunConfig::getInstance()->getAttribute<std::string>(CONFIG_KEYWORD::CONTAINERMODELFILE);
				ContainerFile = strlwr(const_cast<char*>(ContainerFile.c_str()));
				setParticleGeneratorInfo4Voxel(ContainerFile, CRunConfig::getInstance()->getAttribute<float>(CONFIG_KEYWORD::PARTICLESPACING), ETypeParticle::CONTAINER, pObstacleParticleGen);
			}
		}
		break;
	case INITIAL_BOX:
		{
			SBox3 FluidBox = SBox3(CRunConfig::getInstance()->getLeftDownPoint(), CRunConfig::getInstance()->getRightUpPoint());
			setParticleGeneratorInfo4Box(FluidBox, CRunConfig::getInstance()->getAttribute<float>(CONFIG_KEYWORD::PARTICLESPACING), ETypeParticle::FLUID, pFluidParticleGen);
		}
		break;
	case INITIAL_OTHER:
		{
			pFluidParticleGen = new COtherParticleGenerator();
			dynamic_cast<COtherParticleGenerator*>(pFluidParticleGen)->setOtherGenInfo(ETypeParticle::FLUID);

			pObstacleParticleGen = new COtherParticleGenerator();
			dynamic_cast<COtherParticleGenerator*>(pObstacleParticleGen)->setOtherGenInfo(ETypeParticle::CONTAINER);
		}
		break;
	default:
		break;
	}

	_ASSERT(pFluidParticleGen);
	CScene::getInstance()->initScene(pFluidParticleGen, pObstacleParticleGen);
	CCubicKernel::setRadius(g_SmoothKernelRadius);
	CScene::getInstance()->initMass();
	CScene::getInstance()->initBoundryPsi();

	if (pFluidParticleGen)
		delete pFluidParticleGen;

	if (pObstacleParticleGen)
		delete pObstacleParticleGen;
}

//********************************************************************
//FUNCTION:
EParticleInitial hiveFluidSimulation::parseInitalMethod(const std::string& vInitialStr)
{
	EParticleInitial InitalSig;
	if (vInitialStr == hiveFluidSimulation::INITIALMETHOD::BOX)   InitalSig = INITIAL_BOX;
	if (vInitialStr == hiveFluidSimulation::INITIALMETHOD::VOXEL) InitalSig = INITIAL_VOXEL;
	if (vInitialStr == hiveFluidSimulation::INITIALMETHOD::OTHER) InitalSig = INITIAL_OTHER;
	return InitalSig;
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::setParticleGeneratorInfo4Voxel(const std::string& vModelFile, float vSpacing, const ETypeParticle& vTag, CBaseParticleGenerator* &voParticleGen)
{
	_ASSERT(!vModelFile.empty() && !voParticleGen);
	voParticleGen = new CVoxelParticleGenerator();
	dynamic_cast<CVoxelParticleGenerator*>(voParticleGen)->setVoxelGenInfo(vTag, vModelFile, vSpacing);
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::setParticleGeneratorInfo4Box(const SBox3& vBox, float vSpacing, const ETypeParticle& vTag, CBaseParticleGenerator* &voParticleGen)
{
	_ASSERT(!voParticleGen);
	voParticleGen =  new CBoxParticleGenerator();
	dynamic_cast<CBoxParticleGenerator*>(voParticleGen)->setBoxGenInfo(vTag, vBox, vSpacing);
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::initNeighborFinder()
{
	std::string NeighborFindSig = CRunConfig::getInstance()->getAttribute<std::string>(CONFIG_KEYWORD::FINDNEIGHBORMETHOD);
	NeighborFindSig = strupr(const_cast<char*>(NeighborFindSig.c_str()));

	if (NeighborFindSig == NEIGHBORFINDER_KEYWORD::SPATIALHASHING_SIG)
	{
		CScene::getInstance()->initNeighborFinder(CScene::getInstance()->getNumFluidParticle(), g_SmoothKernelRadius);
	}
	else
	{
		CBaseFindNeighbors* pFinder = CNeighborFinderFactoryData::getInstance()->createNeighborFinder(NeighborFindSig);
		_ASSERT(pFinder);

		pFinder->prepareV(CRunConfig::getInstance()->getDivideRange(), CRunConfig::getInstance()->getAttribute<float>(CONFIG_KEYWORD::UNITLENGTH));
		CScene::getInstance()->setNeighborFinder(pFinder);
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::initSimulation(const std::string& vRunConfig)
{
	_ASSERT(!vRunConfig.empty());
	initParticles(vRunConfig);
	initNeighborFinder();
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::doSimulation()
{
	CFluidManager::getInstance()->doSimulation(CRunConfig::getInstance()->getAttribute<int>(CONFIG_KEYWORD::NUMFRAME));
}

//********************************************************************
//FUNCTION:
float hiveFluidSimulation::calculateDistance(const glm::vec3& vPositionI, const glm::vec3& vPositionJ)
{
	return CSolverFunction::calDistance(vPositionI, vPositionJ);
}

//*******************************************************************
//FUNCTION:
void hiveFluidSimulation::testFindNeighbors(CBaseFindNeighbors* vFindNeighbor, const CParticle* vParticle, float vRadius, const std::vector<CParticle*>& vParticleSet, std::vector<CParticle*>& voFluidSet, std::vector<CParticle*>& voObstacleSet)
{
	vFindNeighbor->findNeighborsV(vParticle, vRadius, vParticleSet, voFluidSet, voObstacleSet);
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::initForTestingInteractiveForce(const std::string& vRunFile, const std::vector<glm::vec4>& vFluidParticle)
{
	_ASSERT(!vRunFile.empty());
	CRunConfig::getInstance()->parseV(vRunFile);

	CBaseParticleGenerator* pObstacleParticleGen = NULL;
	std::vector<CParticle*> ContainerParticleSet;

	if (CRunConfig::getInstance()->isAttributeExisted(CONFIG_KEYWORD::CONTAINERMODELFILE))
	{
		std::string ModelName = CRunConfig::getInstance()->getAttribute<std::string>(CONFIG_KEYWORD::CONTAINERMODELFILE);
		ModelName = strlwr(const_cast<char*>(ModelName.c_str()));
		setParticleGeneratorInfo4Voxel(ModelName, CRunConfig::getInstance()->getAttribute<float>(CONFIG_KEYWORD::PARTICLESPACING), ETypeParticle::CONTAINER, pObstacleParticleGen);
		pObstacleParticleGen->generateParticleV(ContainerParticleSet);
	}

	std::vector<CParticle*> FluidParticleSet = std::vector<CParticle*>(vFluidParticle.size());
	CParticle* pFluidParticleArray = new CFluidParticle[vFluidParticle.size()];
	for (unsigned int i=0; i<vFluidParticle.size(); i++)
	{
		FluidParticleSet[i] = &pFluidParticleArray[i];
		FluidParticleSet[i]->setPosition(vFluidParticle[i]);
		FluidParticleSet[i]->setOldPosition(vFluidParticle[i]);
	}

	CScene::getInstance()->setParticleSet(FluidParticleSet, ContainerParticleSet);

	std::string NeighborFindSig = CRunConfig::getInstance()->getAttribute<std::string>(CONFIG_KEYWORD::FINDNEIGHBORMETHOD);
	NeighborFindSig = strupr(const_cast<char*>(NeighborFindSig.c_str()));
	CBaseFindNeighbors* pFinder = CNeighborFinderFactoryData::getInstance()->createNeighborFinder(NeighborFindSig);
	_ASSERT(pFinder);

	pFinder->prepareV(CRunConfig::getInstance()->getDivideRange(), CRunConfig::getInstance()->getAttribute<float>(CONFIG_KEYWORD::UNITLENGTH));
	CScene::getInstance()->setNeighborFinder(pFinder);

	if (pObstacleParticleGen)
	{
		delete pObstacleParticleGen;
	}
}