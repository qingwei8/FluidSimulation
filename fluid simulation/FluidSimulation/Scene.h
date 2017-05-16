#pragma once
#include "common/Singleton.h"
#include <Eigen/dense>
#include "Common.h"
#include "Particle.h"
#include "BaseParticleGenerator.h"
#include "BaseFindNeighbors.h"
#include "NeighborhoodSearchSpatialHashing.h"

namespace hiveFluidSimulation
{
	class CScene : public hiveCommon::CSingleton<CScene>
	{
	public:
		virtual ~CScene();

		void initMass();
		void initBoundryPsi();
		void initNeighborFinder(unsigned int vNumParticles, float vRadius);
		void parseGLMArray2EigenArry(const std::vector<CParticle*>& vRigidParticleSet, std::vector<Eigen::Matrix<float, 3, 1>>& voPositionSet);
		CNeighborhoodSearchSpatialHashing* getNeighborFinder() const {return m_pNeighborhoodSearch;}

		void setNeighborFinder(CBaseFindNeighbors* vNeighborFinder);
		void updateFindNeighborInfo();
		void initScene(CBaseParticleGenerator* vFluidParticleGen, CBaseParticleGenerator* vObstacleParticle);
		void dumpNeighbors(const CParticle* vParticle, const std::vector<CParticle*> vTotalParticle, float vRadius, std::vector<CParticle*>& voFluidNeighborSet, std::vector<CParticle*>& voObstacleNeighborSet);

		unsigned int getNumFluidParticle()               const {return m_FluidParticleSet.size();}
		unsigned int getNumObstacleParticle()            const {return m_ObstacleParticleSet.size();}
		CParticle* getFluidParticle(unsigned int vIndex) const;
		CParticle* getObstacleParticle(unsigned int vIndex) const;

		void dumpFluidParticle(std::vector<CParticle*>& voParticleSet) {voParticleSet = m_FluidParticleSet;}
		void dumpRigidParticle(std::vector<CParticle*>& voParticleSet) {voParticleSet = m_ObstacleParticleSet;}
		void dumpTotalParticle(std::vector<CParticle*>& voParticleSet);
		void setParticleSet(const std::vector<CParticle*>& vFluidParticle, const std::vector<CParticle*>& vContainerParticle);

	protected:
		CScene();

	private:
		std::vector<CParticle*>  m_FluidParticleSet;
		std::vector<CParticle*>  m_ObstacleParticleSet;
		CBaseFindNeighbors*      m_pFindNeighbors;
		CNeighborhoodSearchSpatialHashing* m_pNeighborhoodSearch;

		friend class hiveCommon::CSingleton<CScene>;
	};
}