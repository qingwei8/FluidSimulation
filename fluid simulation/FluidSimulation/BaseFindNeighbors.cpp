#include "BaseFindNeighbors.h"
#include "Particle.h"
#include "Common.h"
#include "NeighborFinderFactory.h"

using namespace hiveFluidSimulation;

CNeighborFinderFactory<CBaseFindNeighbors> TheCreator(NEIGHBORFINDER_KEYWORD::FOR_SIG);
	
CBaseFindNeighbors::CBaseFindNeighbors()
{
}

CBaseFindNeighbors::~CBaseFindNeighbors()
{
}

//********************************************************************
//FUNCTION:
void CBaseFindNeighbors::_exeFindNeighbors(const CParticle* vParticle, float vRadius, const std::vector<CParticle*>& vParticleSet, std::vector<CParticle*>& voFluidNeighborSet, std::vector<CParticle*>& voObstacleNeighborSet)
{
	_ASSERT(vParticle);

	for(unsigned int Index=0; Index<vParticleSet.size(); Index++)
	{
		glm::vec4 ObjParticlePos = vParticleSet[Index]->getNewPosition();
		if(glm::vec3(vParticle->getNewPosition()) == glm::vec3(ObjParticlePos)) continue;
		
		if(glm::distance(glm::vec3(vParticle->getNewPosition()), glm::vec3(ObjParticlePos)) < vRadius)
		{
			if (ObjParticlePos.w == 1.0 && voFluidNeighborSet.size() < g_MaxNeighbors)
			{
				voFluidNeighborSet.push_back(vParticleSet[Index]);
			}

			if (ObjParticlePos.w == 0.0 && voObstacleNeighborSet.size() < g_MaxNeighbors)
			{
				voObstacleNeighborSet.push_back(vParticleSet[Index]);
			}
		}
	}
}

//*******************************************************************
//FUNCTION:
void hiveFluidSimulation::CBaseFindNeighbors::findNeighborsV(const CParticle* vParticle, float vRadius, const std::vector<CParticle*>& vParticleSet, std::vector<CParticle*>& voFluidNeighborSet, std::vector<CParticle*>& voObstacleNeighborSet)
{
	_exeFindNeighbors(vParticle, vRadius, vParticleSet, voFluidNeighborSet, voObstacleNeighborSet);
}