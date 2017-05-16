#include "GridFindNeighbors.h"
#include "NeighborFinderFactory.h"
#include "RunConfig.h"

using namespace hiveFluidSimulation;

CNeighborFinderFactory<CGridFindNeighbors> TheCreator(NEIGHBORFINDER_KEYWORD::GRID_SIG);

CGridFindNeighbors::CGridFindNeighbors()
{
	m_pGridManager = new CGridManager();
}

CGridFindNeighbors::~CGridFindNeighbors()
{
	delete m_pGridManager;
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CGridFindNeighbors::findNeighborsV(const CParticle* vParticle, float vRadius, const std::vector<CParticle*>& vParticleSet, std::vector<CParticle*>& voFluidNeighborSet, std::vector<CParticle*>& voObstacleNeighborSet)
{	
	_ASSERT(vParticle);

	CGrid* pGrid = m_pGridManager->findGridByPos((glm::vec3)vParticle->getNewPosition());
	_ASSERT(pGrid);

	std::vector<CParticle*> GridNeighborParticleSet;
	pGrid->dumpParticleSet(GridNeighborParticleSet);

	std::vector<CGrid*> GridNeighbors;
	m_pGridManager->findGridNeighbor(pGrid, GridNeighbors);

	for(auto& Grid : GridNeighbors)
	{
		Grid->dumpParticleSet(GridNeighborParticleSet);
	}

	_exeFindNeighbors(vParticle, vRadius, GridNeighborParticleSet, voFluidNeighborSet, voObstacleNeighborSet);
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CGridFindNeighbors::updateV(const std::vector<CParticle*>& vParticleSet)
{
	m_pGridManager->updateGridsParticles(vParticleSet);
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CGridFindNeighbors::prepareV(const glm::vec3& vDivideRange, float vUnitGridLength)
{
	m_pGridManager->setDivideInfo(vDivideRange, vUnitGridLength);
	m_pGridManager->divideGrid();
}