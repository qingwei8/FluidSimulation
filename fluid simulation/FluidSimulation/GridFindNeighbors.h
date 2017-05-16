#pragma once
#include "BaseFindNeighbors.h"
#include "GridManager.h"

namespace hiveFluidSimulation
{
	class CGrid;

	class CGridFindNeighbors : public CBaseFindNeighbors
	{
	public:
		CGridFindNeighbors();
		virtual ~CGridFindNeighbors();

		virtual void prepareV(const glm::vec3& vDivideRange, float vUnitGridLength) override;
		virtual void findNeighborsV(const CParticle* vParticle, float vRadius, const std::vector<CParticle*>& vParticleSet, std::vector<CParticle*>& voFluidNeighborSet, std::vector<CParticle*>& voObstacleNeighborSet) override;
		virtual void updateV(const std::vector<CParticle*>& vParticleSet) override;
	
	private:
		CGridManager* m_pGridManager;
	};
}