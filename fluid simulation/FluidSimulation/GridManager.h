#pragma once
#include <map>
#include <glm/glm.hpp>
#include "Grid.h"

namespace hiveFluidSimulation
{
	struct SComparator
	{
		bool operator() (const glm::vec3& vFirst, const glm::vec3& vSecond) const
		{
			if(vFirst.x < vSecond.x)
				return true;
			else if(vFirst.x == vSecond.x && vFirst.y < vSecond.y)
				return true;
			else if(vFirst.x == vSecond.x && vFirst.y == vSecond.y && vFirst.z < vSecond.z)
				return true;

			return false;
		}
	};

	class CParticle;

	class CGridManager
	{
	public:
		CGridManager();
		virtual ~CGridManager();

		void divideGrid();
		void setDivideInfo(const glm::vec3& vDivideRange, float vUnitGridLength);
		void updateGridsParticles(const std::vector<CParticle*>& vParticleSet);
		void findGridNeighbor(const CGrid* vGrid, std::vector<CGrid*>& voGridNeighbors);

		CGrid* findGridByPos(const glm::vec3& vPos) const;
			
	private:		
		void __calNeighborsGridFinderKey(const glm::vec3& vPos, std::vector<glm::vec3>& voNeighborsGridCenterPosSet);
		bool __isWithinDivideRange(const glm::vec3& vPos) const;

	private:
		float     m_UnitGridLength;
		bool      m_IsDivideInfoSet;
		glm::vec3 m_DivideRange;
		std::map<glm::vec3, CGrid*, SComparator> m_GridSet;
	};
}