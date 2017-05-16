#include "GridManager.h"
#include <math.h>
#include "Grid.h"

using namespace hiveFluidSimulation;

CGridManager::CGridManager() : m_IsDivideInfoSet(false), m_UnitGridLength(0.0)
{
}

CGridManager::~CGridManager()
{
	for(auto& Iter = m_GridSet.begin(); Iter != m_GridSet.end(); )
	{
		if(Iter->second != NULL)
		{
			delete Iter->second; 
			Iter->second = NULL;
		}
		m_GridSet.erase(Iter++);
	}
	m_GridSet.clear();
}

//*******************************************************************
//FUNCTION:
void hiveFluidSimulation::CGridManager::divideGrid()
{
	_ASSERT(m_IsDivideInfoSet);

	int X_DivideCount = ceil(m_DivideRange.x / m_UnitGridLength);
	int Y_DivideCount = ceil(m_DivideRange.y / m_UnitGridLength);
	int Z_DivideCount = ceil(m_DivideRange.z / m_UnitGridLength);
	
	CGrid* pGridArray = new CGrid[X_DivideCount * Y_DivideCount * Z_DivideCount]();
	unsigned int Index = 0;

	for(int i=0; i<X_DivideCount; ++i)
	{
		for(int k=0; k<Y_DivideCount; ++k)
		{	
			for(int m=0; m<Z_DivideCount; ++m)
			{				
				glm::vec3 GridLeftDownPos = glm::vec3(i*m_UnitGridLength, k*m_UnitGridLength, m*m_UnitGridLength);		
				glm::vec3 GridRighUpPos = GridLeftDownPos + m_UnitGridLength;
				glm::vec3 FinderKey = floor(GridLeftDownPos / m_UnitGridLength);
				
				_ASSERT(Index < X_DivideCount * Y_DivideCount * Z_DivideCount);
				CGrid* pGrid = &pGridArray[Index++];
				pGrid->setGridPos(GridLeftDownPos, GridRighUpPos);

				m_GridSet.insert(std::make_pair(FinderKey, pGrid));
			}
		}
	}
}

//********************************************************************
//FUNCTION:
CGrid* hiveFluidSimulation::CGridManager::findGridByPos(const glm::vec3& vPos) const
{
	_ASSERT(m_UnitGridLength);
	_ASSERT(__isWithinDivideRange(vPos));

	glm::vec3 FinderKey = (floor(vPos/m_UnitGridLength));
	auto& Iter = m_GridSet.find(FinderKey);
	if(Iter != m_GridSet.end())
		return Iter->second;
	else
		return NULL;
}

//*******************************************************************
//FUNCTION:
void hiveFluidSimulation::CGridManager::__calNeighborsGridFinderKey(const glm::vec3& vPos, std::vector<glm::vec3>& voNeighborsGridCenterPosSet)
{
	//与6个面相邻的立方体的中心
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x-m_UnitGridLength, vPos.y, vPos.z));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x+m_UnitGridLength, vPos.y, vPos.z));	
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x, vPos.y-m_UnitGridLength, vPos.z));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x, vPos.y+m_UnitGridLength, vPos.z));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x, vPos.y, vPos.z-m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x, vPos.y, vPos.z+m_UnitGridLength));

	//与12条凌相邻的立方体的中心
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x, vPos.y-m_UnitGridLength, vPos.z-m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x, vPos.y-m_UnitGridLength, vPos.z+m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x, vPos.y+m_UnitGridLength, vPos.z+m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x, vPos.y+m_UnitGridLength, vPos.z-m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x-m_UnitGridLength, vPos.y, vPos.z-m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x-m_UnitGridLength, vPos.y, vPos.z+m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x+m_UnitGridLength, vPos.y, vPos.z-m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x+m_UnitGridLength, vPos.y, vPos.z+m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x-m_UnitGridLength, vPos.y-m_UnitGridLength, vPos.z));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x-m_UnitGridLength, vPos.y+m_UnitGridLength, vPos.z));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x+m_UnitGridLength, vPos.y-m_UnitGridLength, vPos.z));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x+m_UnitGridLength, vPos.y+m_UnitGridLength, vPos.z));

	//与8个顶点相邻的立方体的中心
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x-m_UnitGridLength, vPos.y-m_UnitGridLength, vPos.z-m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x-m_UnitGridLength, vPos.y-m_UnitGridLength, vPos.z+m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x-m_UnitGridLength, vPos.y+m_UnitGridLength, vPos.z-m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x-m_UnitGridLength, vPos.y+m_UnitGridLength, vPos.z+m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x+m_UnitGridLength, vPos.y-m_UnitGridLength, vPos.z-m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x+m_UnitGridLength, vPos.y-m_UnitGridLength, vPos.z+m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x+m_UnitGridLength, vPos.y+m_UnitGridLength, vPos.z-m_UnitGridLength));
	voNeighborsGridCenterPosSet.push_back(glm::vec3(vPos.x+m_UnitGridLength, vPos.y+m_UnitGridLength, vPos.z+m_UnitGridLength));
}

//*******************************************************************
//FUNCTION:
void hiveFluidSimulation::CGridManager::findGridNeighbor(const CGrid* vGrid, std::vector<CGrid*>& voGridNeighbors)
{
	_ASSERT(vGrid);
	_ASSERT(m_UnitGridLength);

	std::vector<glm::vec3> NeighborsGridCenterPosSet;
	__calNeighborsGridFinderKey(vGrid->getGridLeftDownPos()+(m_UnitGridLength/2), NeighborsGridCenterPosSet);
	
	for(auto& NeighborIter : NeighborsGridCenterPosSet)
	{ 
		if(__isWithinDivideRange(NeighborIter))
			voGridNeighbors.push_back(findGridByPos(NeighborIter));
	}
}

//*******************************************************************
//FUNCTION:
void hiveFluidSimulation::CGridManager::updateGridsParticles(const std::vector<CParticle*>& vParticleSet)
{
	for(auto& GridMap : m_GridSet)
	{
		CGrid* pGrid = GridMap.second;
		if(pGrid->getGridParticleTotal()) pGrid->clearGridParticles();
	}

	for(auto& Partilce : vParticleSet)
	{	
		_ASSERT(Partilce);
		_ASSERT(__isWithinDivideRange((glm::vec3)Partilce->getNewPosition()));

		CGrid* pGrid = findGridByPos((glm::vec3)Partilce->getNewPosition());
		pGrid->addParticleToGrid(Partilce);
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CGridManager::setDivideInfo(const glm::vec3& vDivideRange, float vUnitGridLength)
{
	_ASSERT(vUnitGridLength > 0.0);
	_ASSERT(vDivideRange.x >= 0.0 && vDivideRange.y >= 0.0 && vDivideRange.z >= 0.0);

	m_DivideRange     = vDivideRange;
	m_UnitGridLength  = vUnitGridLength;
	m_IsDivideInfoSet = true;
}

//*******************************************************************
//FUNCTION:
bool hiveFluidSimulation::CGridManager::__isWithinDivideRange(const glm::vec3& vPos) const
{
	if((vPos.x>=0.0 && vPos.y>=0.0 && vPos.z>=0.0) && 
		(vPos.x<=m_DivideRange.x && vPos.y<=m_DivideRange.y && vPos.z<=m_DivideRange.z))
		return true;
	else
		return false;
}