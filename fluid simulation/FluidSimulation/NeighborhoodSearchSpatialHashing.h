#ifndef __NEIGHBORHOODSEARCHSPATIALHASHING_H__
#define __NEIGHBORHOODSEARCHSPATIALHASHING_H__

#include "Hashmap.h"
#include <vector>
#include <Eigen/dense>
#include "Common.h"

namespace hiveFluidSimulation
{
	typedef Eigen::Vector3i NeighborhoodSearchCellPos;

	template<>
	inline unsigned int hashFunction<NeighborhoodSearchCellPos*>(NeighborhoodSearchCellPos* const &vKey)
	{
		const int p1 = 73856093 * (*vKey)[0];
		const int p2 = 19349663 * (*vKey)[1];
		const int p3 = 83492791 * (*vKey)[2];
		return p1 + p2 + p3;
	}

	class CNeighborhoodSearchSpatialHashing
	{
	public: 
		CNeighborhoodSearchSpatialHashing(const unsigned int vNumParticles = 0, const float vRadius = 0.1, const unsigned int vMaxNeighbors = 60u, const unsigned int vMaxParticlesPerCell = 50u);
		~CNeighborhoodSearchSpatialHashing();

		// Spatial hashing
		struct SHashEntry
		{
			SHashEntry() {};
			unsigned long m_Timestamp;
			std::vector<unsigned int> m_ParticleIndices;
		};

		FORCE_INLINE static int floor(const float vVaule)
		{
			return (int)(vVaule + 32768.f) - 32768;			// Shift to get positive values 
		}

		void cleanup();
		void neighborhoodSearch(Eigen::Matrix<float, 3, 1> *vPos);
		void neighborhoodSearch(Eigen::Matrix<float, 3, 1> *vPos, const unsigned int vNumBoundaryParticles, Eigen::Matrix<float, 3, 1> *vBoundaryX);
		void update();
		unsigned int **getNeighbors() const;
		unsigned int *getNumNeighbors() const;
		const unsigned int getMaxNeighbors() const { return m_MaxNeighbors;	}

		unsigned int getNumParticles() const;
		void setRadius(const float vRadius);
		float getRadius() const;

		FORCE_INLINE unsigned int getNumNeighbors(unsigned int vIndex) const 
		{
			return m_pNumNeighbors[vIndex];
		}
		FORCE_INLINE unsigned int getNeighbor(unsigned int vIndex_i, unsigned int vIndex_k) const 
		{
			return m_pNeighbors[vIndex_i][vIndex_k];
		}


	private: 
		unsigned int m_NumParticles;
		unsigned int m_MaxNeighbors;
		unsigned int m_MaxParticlesPerCell;
		unsigned int **m_pNeighbors;
		unsigned int *m_pNumNeighbors;
		float m_CellGridSize;
		float m_Radius2;
		unsigned int m_CurrentTimestamp;
		CHashmap<NeighborhoodSearchCellPos*, SHashEntry*> m_GridMap;	
	};
}

#endif
