#pragma once
#include <glm/glm.hpp>
#include <vector>
 
 namespace hiveFluidSimulation
 {
 	class CParticle;
 
 	class CBaseFindNeighbors
 	{
 	public:
 		CBaseFindNeighbors();
 		virtual ~CBaseFindNeighbors();
		 
		virtual void prepareV(const glm::vec3& vDivideRange, float vUnitGridLength){}
 		virtual void findNeighborsV(const CParticle* vParticle, float vRadius, const std::vector<CParticle*>& vParticleSet, std::vector<CParticle*>& voFluidNeighborSet, std::vector<CParticle*>& voObstacleNeighborSet);
		virtual void updateV(const std::vector<CParticle*>& vParticleSet) {}

	protected:
		void _exeFindNeighbors(const CParticle* vParticle, float vRadius, const std::vector<CParticle*>& vParticleSet, std::vector<CParticle*>& voFluidNeighborSet, std::vector<CParticle*>& voObstacleNeighborSet);
	};
 }