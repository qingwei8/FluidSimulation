#pragma once
#include <vector>
#include "Grid.h"
#include "Particle.h"

namespace hiveFluidSimulation
{
	class CGrid
	{
	public:
		CGrid();
		~CGrid();
	
		void setGridPos(const glm::vec3& vLeftDownPos, const glm::vec3& vRightUpPos) {m_LeftDownPos = vLeftDownPos; m_RightUpPos = vRightUpPos;}
		void addParticleToGrid(CParticle* vParticle);
		void dumpParticleSet(std::vector<CParticle*>& voParticleSet) const;
		void clearGridParticles();

		glm::vec3 getGridLeftDownPos()      const {return m_LeftDownPos;}
		glm::vec3 getGridRightUpPos()       const {return m_RightUpPos;}
		unsigned int getGridParticleTotal() const {return m_GridParticleSet.size();}

	private:
		glm::vec3 m_LeftDownPos;
		glm::vec3 m_RightUpPos;
		std::vector<CParticle*> m_GridParticleSet;
	};
}   