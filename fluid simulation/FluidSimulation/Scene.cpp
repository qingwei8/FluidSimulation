#include "Scene.h"
#include "BaseParticleGenerator.h"
#include "RigidParticle.h"
#include "SPHKernels.h"
#include "FluidParticle.h"

using namespace hiveFluidSimulation;

CScene::CScene() : m_pFindNeighbors(NULL), m_pNeighborhoodSearch(NULL)
{
}

CScene::~CScene()
{
	if (m_FluidParticleSet.size() > 0)
	{
		delete[] m_FluidParticleSet[0];
	}

	if (m_ObstacleParticleSet.size() > 0)
	{
		delete[] m_ObstacleParticleSet[0];
		m_ObstacleParticleSet.clear();
	}

	if (m_pFindNeighbors)
	{
		delete m_pFindNeighbors;
	}

	if (m_pNeighborhoodSearch)
	{
		delete m_pNeighborhoodSearch;
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CScene::initScene(CBaseParticleGenerator* vFluidParticleGen, CBaseParticleGenerator* vObstacleParticle)
{
	_ASSERT(vFluidParticleGen);
	vFluidParticleGen->generateParticleV(m_FluidParticleSet);

	if (vObstacleParticle)
	{
		vObstacleParticle->generateParticleV(m_ObstacleParticleSet);
	}
}

//********************************************************************
//FUNCTION:
CParticle* hiveFluidSimulation::CScene::getFluidParticle(unsigned int vIndex) const
{
	_ASSERT(vIndex < m_FluidParticleSet.size());
	return m_FluidParticleSet[vIndex];
}

//*******************************************************************
//FUNCTION:
void hiveFluidSimulation::CScene::dumpNeighbors(const CParticle* vParticle, const std::vector<CParticle*> vTotalParticle,  float vRadius, std::vector<CParticle*>& voFluidNeighborSet, std::vector<CParticle*>& voObstacleNeighborSet)
{
	_ASSERT(m_pFindNeighbors);

	m_pFindNeighbors->findNeighborsV(vParticle, vRadius, vTotalParticle, voFluidNeighborSet, voObstacleNeighborSet);
}

//*******************************************************************
//FUNCTION:
CParticle* hiveFluidSimulation::CScene::getObstacleParticle(unsigned int vIndex) const
{
	_ASSERT(vIndex < m_ObstacleParticleSet.size());
	return m_ObstacleParticleSet[vIndex];
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CScene::setParticleSet(const std::vector<CParticle*>& vFluidParticle, const std::vector<CParticle*>& vContainerParticle)
{
	m_FluidParticleSet    = vFluidParticle;
	m_ObstacleParticleSet = vContainerParticle;
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CScene::updateFindNeighborInfo()
{
	_ASSERT(m_pFindNeighbors);

	std::vector<CParticle*> TotalParticleSet;
	dumpTotalParticle(TotalParticleSet);
	m_pFindNeighbors->updateV(TotalParticleSet);
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CScene::dumpTotalParticle(std::vector<CParticle*>& voParticleSet)
{
	voParticleSet.insert(voParticleSet.end(), m_FluidParticleSet.begin(), m_FluidParticleSet.end());
	voParticleSet.insert(voParticleSet.end(), m_ObstacleParticleSet.begin(), m_ObstacleParticleSet.end());
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CScene::initMass()
{
	float Diam = 2.0 * g_Radius;
	for (unsigned int i=0; i<m_FluidParticleSet.size(); ++i)
	{
		m_FluidParticleSet[i]->setMass(0.8 * Diam * Diam * Diam * g_Density0);
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CScene::parseGLMArray2EigenArry(const std::vector<CParticle*>& vRigidParticleSet, std::vector<Eigen::Matrix<float, 3, 1>>& voPositionSet)
{
	for (auto& Particle : vRigidParticleSet)
	{
		glm::vec4 Pos = Particle->getNewPosition();
		voPositionSet.push_back(Eigen::Matrix<float, 3, 1>(Pos.x, Pos.y, Pos.z));
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CScene::initBoundryPsi()
{
	if (CScene::getInstance()->getNumObstacleParticle() <= 0) return;

	std::vector<Eigen::Matrix<float, 3, 1>> BoundaryXSet;
	std::vector<CParticle*> BoundryParticleSet;
	CScene::getInstance()->dumpRigidParticle(BoundryParticleSet);
	parseGLMArray2EigenArry(BoundryParticleSet, BoundaryXSet);

	CNeighborhoodSearchSpatialHashing NeighborhoodSearchSH(CScene::getInstance()->getNumObstacleParticle(), g_SmoothKernelRadius);
	NeighborhoodSearchSH.neighborhoodSearch(&BoundaryXSet[0]);

	unsigned int **pNeighbors = NeighborhoodSearchSH.getNeighbors();
	unsigned int *pNumNeighbors = NeighborhoodSearchSH.getNumNeighbors();

#pragma omp parallel default(shared)
	{
#pragma omp for schedule(static)
		for (int i = 0; i < (int) BoundryParticleSet.size(); i++)
		{
			float Delta = CCubicKernel::W_zero();
			for (unsigned int j = 0; j < pNumNeighbors[i]; j++)
			{
				const unsigned int NeighborIndex = pNeighbors[i][j];
				Delta += CCubicKernel::W(BoundaryXSet[i] - BoundaryXSet[NeighborIndex]);
			}
			const float Volume = 1.0 / Delta;
			dynamic_cast<CRigidParticle*>(m_ObstacleParticleSet[i])->setBoundryPsi(g_Density0 * Volume);
		}
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CScene::initNeighborFinder(unsigned int vNumParticles, float vRadius)
{
	if (m_pNeighborhoodSearch == NULL)
		m_pNeighborhoodSearch = new CNeighborhoodSearchSpatialHashing(vNumParticles, vRadius);
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CScene::setNeighborFinder(CBaseFindNeighbors* vNeighborFinder)
{
	_ASSERT(vNeighborFinder); m_pFindNeighbors = vNeighborFinder;
}