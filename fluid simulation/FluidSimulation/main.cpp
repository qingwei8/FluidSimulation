#include "FluidManager.h"
#include "DeriveParticleGenerator.h"
#include "Particle.h"

using namespace hiveFluidSimulation;

int main()
{
	hiveFluidSimulation::CScene* pScene = new hiveFluidSimulation::CScene();
	
	hiveFluidSimulation::CBaseParticleGenerator* pObstacleGen1 = new hiveFluidSimulation::CBaseParticleGenerator();
	hiveFluidSimulation::CBaseParticleGenerator* pObstacleGen2 = new hiveFluidSimulation::CDeriveParticleGenerator();
	hiveFluidSimulation::CDeriveParticleGenerator* pFluidGen  = new hiveFluidSimulation::CDeriveParticleGenerator();
	
	hiveFluidSimulation::SBox3 Box(glm::vec3(0, 0, 0), glm::vec3(3, 3, 3));
	std::vector<CParticle*>  m_FluidParticleSet;
	
	pObstacleGen1->setPartitionInfo(1, 1);
	pObstacleGen1->setModelFile("Model1");
	//pObstacleGen1->setBox(Box);
	pObstacleGen1->generateParticle(m_FluidParticleSet);
	
	pObstacleGen2->setPartitionInfo(2, 2);
	pObstacleGen2->setModelFile("Model2");
	//pObstacleGen2->setBox(Box);
	m_FluidParticleSet.clear();
	pObstacleGen1->generateParticle(m_FluidParticleSet);

	pFluidGen->setPartitionInfo(1.0, 0.0);
	pFluidGen->setModelFile("Model3");
	pFluidGen->setBox(Box);
	m_FluidParticleSet.clear();
	pFluidGen->generateParticle(m_FluidParticleSet);

	CBaseParticleGenerator* pTest = NULL;
	pTest = pFluidGen;
	pTest->generateParticle(m_FluidParticleSet);

	pObstacleGen1->setPartitionInfo(10, 1);
	pObstacleGen2->setPartitionInfo(11, 2);


	pScene->initScene(pFluidGen, NULL);

	hiveFluidSimulation::CFluidManager* pManager = new hiveFluidSimulation::CFluidManager();
	pManager->setScene(pScene);
	pManager->doSimulation();

	delete pScene;
	delete pFluidGen;
	delete pManager;

	return 0;
}