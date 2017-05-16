#include "NeighborhoodSearchSpatialHashing.h"

using namespace hiveFluidSimulation;

CNeighborhoodSearchSpatialHashing::CNeighborhoodSearchSpatialHashing(const unsigned int vNumParticles, const float vRadius, const unsigned int vMaxNeighbors, const unsigned int vMaxParticlesPerCell) :
	m_GridMap(vNumParticles*2)
{
	m_CellGridSize = vRadius;
	m_Radius2 = vRadius*vRadius;
	m_NumParticles = vNumParticles;
	m_MaxParticlesPerCell = vMaxParticlesPerCell;
	m_MaxNeighbors = vMaxNeighbors;

	m_pNumNeighbors = NULL;
	m_pNeighbors = NULL;

	if (vNumParticles != 0)
	{
		m_pNumNeighbors = new unsigned int[m_NumParticles];
		m_pNeighbors = new unsigned int*[m_NumParticles];
		for (unsigned int i = 0; i < m_NumParticles; i++)
			m_pNeighbors[i] = new unsigned int[m_MaxNeighbors];
	}

	m_CurrentTimestamp = 0;
}

CNeighborhoodSearchSpatialHashing::~CNeighborhoodSearchSpatialHashing()
{
	cleanup();
}

void CNeighborhoodSearchSpatialHashing::cleanup()
{
	for (unsigned int i=0; i < m_NumParticles; i++)
		delete [] m_pNeighbors[i];
	delete [] m_pNeighbors;
	delete [] m_pNumNeighbors;
	m_NumParticles = 0;

	for (unsigned int i=0; i < m_GridMap.bucket_count(); i++)
	{
		CHashmap<NeighborhoodSearchCellPos*, CNeighborhoodSearchSpatialHashing::SHashEntry*>::KeyValueMap *pKeyValueMap = m_GridMap.getKeyValueMap(i);
		if (pKeyValueMap)
		{
			for (CHashmap<NeighborhoodSearchCellPos*, CNeighborhoodSearchSpatialHashing::SHashEntry*>::KeyValueMap::iterator Iter=pKeyValueMap->begin(); Iter != pKeyValueMap->end(); Iter++)
			{
				CNeighborhoodSearchSpatialHashing::SHashEntry* pEntry = Iter->second;
				delete pEntry;
				Iter->second = NULL;
			}
		}
	}
}

unsigned int ** CNeighborhoodSearchSpatialHashing::getNeighbors() const
{
	return m_pNeighbors;
}

unsigned int * CNeighborhoodSearchSpatialHashing::getNumNeighbors() const
{
	return m_pNumNeighbors;
}

unsigned int CNeighborhoodSearchSpatialHashing::getNumParticles() const
{
	return m_NumParticles;
}

void CNeighborhoodSearchSpatialHashing::setRadius(const float vRadius)
{
	m_CellGridSize = vRadius;
	m_Radius2 = vRadius*vRadius;
}

float CNeighborhoodSearchSpatialHashing::getRadius() const
{
	return sqrt(m_Radius2);
}

void CNeighborhoodSearchSpatialHashing::update()
{
	m_CurrentTimestamp++;
}


void CNeighborhoodSearchSpatialHashing::neighborhoodSearch(Eigen::Matrix<float, 3, 1> *vPos) 
{		
	const float Factor = 1.0/m_CellGridSize;
	for (int i=0; i < (int) m_NumParticles; i++)
	{
		const int CellPos1 = CNeighborhoodSearchSpatialHashing::floor(vPos[i][0] * Factor)+1;
		const int CellPos2 = CNeighborhoodSearchSpatialHashing::floor(vPos[i][1] * Factor)+1;
		const int CellPos3 = CNeighborhoodSearchSpatialHashing::floor(vPos[i][2] * Factor)+1;
		NeighborhoodSearchCellPos cellPos(CellPos1, CellPos2, CellPos3);
		SHashEntry *&pEntry = m_GridMap[&cellPos];

		if (pEntry != NULL)
		{
			if (pEntry->m_Timestamp != m_CurrentTimestamp)
			{
				pEntry->m_Timestamp = m_CurrentTimestamp;
				pEntry->m_ParticleIndices.clear();
			}
		}
		else
		{
			SHashEntry *pNewEntry = new SHashEntry(); 	
			pNewEntry->m_ParticleIndices.reserve(m_MaxParticlesPerCell);
			pNewEntry->m_Timestamp = m_CurrentTimestamp; 
			pEntry = pNewEntry;
		}
		pEntry->m_ParticleIndices.push_back(i);
	}

	// loop over all 27 neighboring cells
	#pragma omp parallel default(shared)
	{
		#pragma omp for schedule(static) 
		for (int i=0; i < (int) m_NumParticles; i++)
		{
			m_pNumNeighbors[i] = 0;
			const int CellPos1 = CNeighborhoodSearchSpatialHashing::floor(vPos[i][0] * Factor);
			const int CellPos2 = CNeighborhoodSearchSpatialHashing::floor(vPos[i][1] * Factor);
			const int CellPos3 = CNeighborhoodSearchSpatialHashing::floor(vPos[i][2] * Factor);
			for(unsigned char j=0; j < 3; j++)
			{				
				for(unsigned char k=0; k < 3; k++)
				{									
					for(unsigned char l=0; l < 3; l++)
					{
						NeighborhoodSearchCellPos CellPos(CellPos1+j, CellPos2+k, CellPos3+l);
						SHashEntry * const *pEntry = m_GridMap.query(&CellPos);
					
						if ((pEntry != NULL) && (*pEntry != NULL) && ((*pEntry)->m_Timestamp == m_CurrentTimestamp))
						{
							for (unsigned int m=0; m < (*pEntry)->m_ParticleIndices.size(); m++)
							{
								const unsigned int Index = (*pEntry)->m_ParticleIndices[m];
								if (Index != i)
								{
									const float Dist2 = (vPos[i]-vPos[Index]).squaredNorm();
									if (Dist2 < m_Radius2)
									{
										if (m_pNumNeighbors[i] < m_MaxNeighbors)
											m_pNeighbors[i][m_pNumNeighbors[i]++] = Index;
//									else
// 											std::cout << "too many neighbors detected\n";
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void CNeighborhoodSearchSpatialHashing::neighborhoodSearch(Eigen::Matrix<float, 3, 1> *vPos, const unsigned int vNumBoundaryParticles, Eigen::Matrix<float, 3, 1> *vBoundaryX)
{
	const float Factor = 1.0/m_CellGridSize;
	for (int i=0; i < (int) m_NumParticles; i++)
	{
		const int CellPos1 = CNeighborhoodSearchSpatialHashing::floor(vPos[i][0] * Factor)+1;
		const int CellPos2 = CNeighborhoodSearchSpatialHashing::floor(vPos[i][1] * Factor)+1;
		const int CellPos3 = CNeighborhoodSearchSpatialHashing::floor(vPos[i][2] * Factor)+1;
		NeighborhoodSearchCellPos cellPos(CellPos1, CellPos2, CellPos3);
		SHashEntry *&pEntry = m_GridMap[&cellPos];

		if (pEntry != NULL)
		{
			if (pEntry->m_Timestamp != m_CurrentTimestamp)
			{
				pEntry->m_Timestamp = m_CurrentTimestamp;
				pEntry->m_ParticleIndices.clear();
			}
		}
		else
		{
			SHashEntry *pNewEntry = new SHashEntry(); 	
			pNewEntry->m_ParticleIndices.reserve(m_MaxParticlesPerCell);
			pNewEntry->m_Timestamp = m_CurrentTimestamp; 
			pEntry = pNewEntry;
		}
		pEntry->m_ParticleIndices.push_back(i);
	}

	for (int i = 0; i < (int)vNumBoundaryParticles; i++)
	{
		const int CellPos1 = CNeighborhoodSearchSpatialHashing::floor(vBoundaryX[i][0] * Factor) + 1;
		const int CellPos2 = CNeighborhoodSearchSpatialHashing::floor(vBoundaryX[i][1] * Factor) + 1;
		const int CellPos3 = CNeighborhoodSearchSpatialHashing::floor(vBoundaryX[i][2] * Factor) + 1;
		NeighborhoodSearchCellPos CellPos(CellPos1, CellPos2, CellPos3);
		SHashEntry *&pEntry = m_GridMap[&CellPos];

		if (pEntry != NULL)
		{
			if (pEntry->m_Timestamp != m_CurrentTimestamp)
			{
				pEntry->m_Timestamp = m_CurrentTimestamp;
				pEntry->m_ParticleIndices.clear();
			}
		}
		else
		{
			SHashEntry *pNewEntry = new SHashEntry();
			pNewEntry->m_ParticleIndices.reserve(m_MaxParticlesPerCell);
			pNewEntry->m_Timestamp = m_CurrentTimestamp;
			pEntry = pNewEntry;
		}
		pEntry->m_ParticleIndices.push_back(m_NumParticles + i);
	}

	// loop over all 27 neighboring cells
	#pragma omp parallel default(shared)
	{
		#pragma omp for schedule(static)  
		for (int i=0; i < (int) m_NumParticles; i++)
		{
			m_pNumNeighbors[i] = 0;
			const int CellPos1 = CNeighborhoodSearchSpatialHashing::floor(vPos[i][0] * Factor);
			const int CellPos2 = CNeighborhoodSearchSpatialHashing::floor(vPos[i][1] * Factor);
			const int CellPos3 = CNeighborhoodSearchSpatialHashing::floor(vPos[i][2] * Factor);
			for(unsigned char j=0; j < 3; j++)
			{				
				for(unsigned char k=0; k < 3; k++)
				{									
					for(unsigned char l=0; l < 3; l++)
					{
						NeighborhoodSearchCellPos CellPos(CellPos1+j, CellPos2+k, CellPos3+l);
						SHashEntry * const *pEntry = m_GridMap.query(&CellPos);
					
						if ((pEntry != NULL) && (*pEntry != NULL) && ((*pEntry)->m_Timestamp == m_CurrentTimestamp))
						{
							for (unsigned int m=0; m < (*pEntry)->m_ParticleIndices.size(); m++)
							{
								const unsigned int Index = (*pEntry)->m_ParticleIndices[m];
								if (Index != i)
								{
									float Dist2;
									if (Index < m_NumParticles)
										Dist2 = (vPos[i]-vPos[Index]).squaredNorm();
									else
										Dist2 = (vPos[i] - vBoundaryX[Index - m_NumParticles]).squaredNorm();

									if (Dist2 < m_Radius2)
									{
										if (m_pNumNeighbors[i] < m_MaxNeighbors)
											m_pNeighbors[i][m_pNumNeighbors[i]++] = Index;
// 										else
// 											std::cout << "too many neighbors detected\n";
									}
								}
							}
						}
					}
				}
			}
		}
	}
}