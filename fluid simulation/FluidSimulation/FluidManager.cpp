#include "FluidManager.h"
#include <fstream>
#include "Common.h"
#include "SPHKernels.h"
#include "FluidParticle.h"
#include "RunConfig.h"

using namespace hiveFluidSimulation;

hiveFluidSimulation::CFluidManager::CFluidManager()
{
}

hiveFluidSimulation::CFluidManager::~CFluidManager()
{	
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidManager::doSimulation(unsigned int vNumFrame)
{
	std::ofstream FContainerout("ContainerResult_0417.txt");
	//FContainerout << CScene::getInstance()->getNumObstacleParticle() << std::endl;
	__recordObstacleSimulationInfo(FContainerout);
	std::cout << "ContainerNum: " << CScene::getInstance()->getNumObstacleParticle() << std::endl;

	std::ofstream Fout("FluidResult_0417.txt");
	//Fout << CScene::getInstance()->getNumFluidParticle() << std::endl;
	std::cout << "FluidNum: " << CScene::getInstance()->getNumFluidParticle() << std::endl;

	unsigned int i = 0;
	while (i < vNumFrame)
	{
		__recordFluidSimulationInfo(Fout);
		//__recordObstacleSimulationInfo(FContainerout);

		__predictInitialInfo();
		__neighborSearch();
	
		int NumIter = 0;
		while (NumIter < g_MaxIteration)
		{
			__correctPosition();
			NumIter++;
		}
		__updateVelocity();
		__updateNeighbor();

		i++;
		std::cout << i << std::endl;
	}

	Fout.close();
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidManager::__recordFluidSimulationInfo(std::ofstream& voOutputStream)
{
	for (unsigned int i=0; i<CScene::getInstance()->getNumFluidParticle(); i++)
	{
		CParticle* pParticle = CScene::getInstance()->getFluidParticle(i);
		writePosInfo2File(glm::vec3(pParticle->getNewPosition()), voOutputStream);
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidManager::__recordObstacleSimulationInfo(std::ofstream& voOutputStream)
{
	for (unsigned int i=0; i<CScene::getInstance()->getNumObstacleParticle(); i++)
	{
		CParticle* pParticle = CScene::getInstance()->getObstacleParticle(i);
		writePosInfo2File(glm::vec3(pParticle->getNewPosition()), voOutputStream);
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidManager::__predictInitialInfo()
{
	for (unsigned int i=0; i<CScene::getInstance()->getNumFluidParticle(); ++i)
	{
		CParticle* pParticle = CScene::getInstance()->getFluidParticle(i);
		pParticle->predictInfoV();
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidManager::__correctPosition()
{
	for (unsigned int i=0; i<CScene::getInstance()->getNumFluidParticle(); ++i)
	{
		CParticle* pParticleI = CScene::getInstance()->getFluidParticle(i);
		pParticleI->correctInfoV();
	}
}

//*******************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidManager::__neighborSearch()
{
	std::string NeighborFindSig = CRunConfig::getInstance()->getAttribute<std::string>(CONFIG_KEYWORD::FINDNEIGHBORMETHOD);
	NeighborFindSig = strupr(const_cast<char*>(NeighborFindSig.c_str()));

	if (NeighborFindSig != NEIGHBORFINDER_KEYWORD::SPATIALHASHING_SIG)
	{
		std::vector<CParticle*> TotalParticle;
		CScene::getInstance()->dumpTotalParticle(TotalParticle);
		CScene::getInstance()->updateFindNeighborInfo();

		for(unsigned int i=0; i<CScene::getInstance()->getNumFluidParticle(); ++i)
		{
			CParticle* pParticle = CScene::getInstance()->getFluidParticle(i);
			std::vector<CParticle*> FluidParticleNeighbors, ObstacleParticleNeighbors;
			CScene::getInstance()->dumpNeighbors(pParticle, TotalParticle, g_SmoothKernelRadius, FluidParticleNeighbors, ObstacleParticleNeighbors);
			pParticle->setFluidNeighbors(FluidParticleNeighbors);
			pParticle->setObstacleNeighbors(ObstacleParticleNeighbors);
		}
	}
	else
	{
		__spatialHashNeighborSearch();
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidManager::__spatialHashNeighborSearch()
{
	std::vector<Eigen::Matrix<float, 3, 1>> FluidPosSet;
	std::vector<CParticle*> FluidParticleSet;
	CScene::getInstance()->dumpFluidParticle(FluidParticleSet);
	CScene::getInstance()->parseGLMArray2EigenArry(FluidParticleSet, FluidPosSet);

	std::vector<Eigen::Matrix<float, 3, 1>> BoundaryXSet;
	std::vector<CParticle*> BoundryParticleSet;
	CScene::getInstance()->dumpRigidParticle(BoundryParticleSet);
	CScene::getInstance()->parseGLMArray2EigenArry(BoundryParticleSet, BoundaryXSet);

	if (CScene::getInstance()->getNumObstacleParticle() <= 0)
	{
		CScene::getInstance()->getNeighborFinder()->neighborhoodSearch(&FluidPosSet[0], CScene::getInstance()->getNumObstacleParticle(), NULL);
	}
	else
	{
		CScene::getInstance()->getNeighborFinder()->neighborhoodSearch(&FluidPosSet[0], CScene::getInstance()->getNumObstacleParticle(), &BoundaryXSet[0]);
	}

	unsigned int **neighbors = CScene::getInstance()->getNeighborFinder()->getNeighbors();
	unsigned int *numNeighbors = CScene::getInstance()->getNeighborFinder()->getNumNeighbors();

	int NumFluidParticle = CScene::getInstance()->getNumFluidParticle();

	std::vector<CParticle*> FluidNeighbor, RigidNeighbor;
	for (unsigned int i=0; i<CScene::getInstance()->getNumFluidParticle(); ++i)
	{
		FluidNeighbor.clear();
		RigidNeighbor.clear();

		int NumNeighbor = numNeighbors[i];
		for (unsigned int k=0; k<numNeighbors[i]; ++k)
		{
			int Index = neighbors[i][k];
			if (Index < NumFluidParticle)
			{
				FluidNeighbor.push_back(CScene::getInstance()->getFluidParticle(Index));
			}
			else
			{
				RigidNeighbor.push_back(CScene::getInstance()->getObstacleParticle(Index - NumFluidParticle));
			}
		}
		CParticle* pParticle = CScene::getInstance()->getFluidParticle(i);
		pParticle->setFluidNeighbors(FluidNeighbor);
		pParticle->setObstacleNeighbors(RigidNeighbor);
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidManager::__updateNeighbor()
{
	if (CRunConfig::getInstance()->getAttribute<std::string>(CONFIG_KEYWORD::FINDNEIGHBORMETHOD) == NEIGHBORFINDER_KEYWORD::SPATIALHASHING_SIG)
	{
		CScene::getInstance()->getNeighborFinder()->update();
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CFluidManager::__updateVelocity()
{
	for (unsigned int i=0; i<CScene::getInstance()->getNumFluidParticle(); ++i)
	{
		CParticle* pParticle = CScene::getInstance()->getFluidParticle(i);
		pParticle->updateInfoV();
	}
	
	for (unsigned int i=0; i<CScene::getInstance()->getNumFluidParticle(); ++i)
	{
		CParticle* pParticle = CScene::getInstance()->getFluidParticle(i);
		dynamic_cast<CFluidParticle*>(pParticle)->computeXSPHViscosity();
	}
}