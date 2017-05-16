#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Export.h"

namespace hiveFluidSimulation
{
	class CRigidParticle;
	class CFluidParticle;

	class CParticle
	{
	public:
		CParticle();
		virtual ~CParticle();

		void setMass(float vMass);
		float getMass() const {return m_Mass;}

		void setPosition(const glm::vec4& vPos)                                        {m_NewPosition = vPos;}
		void setOldPosition(const glm::vec4& vPos)                                     {m_OldPosition = vPos;}
		void setFluidNeighbors(const std::vector<CParticle*>& vFluidParticleSet)       {m_FluidNeighborSet = vFluidParticleSet;}
		void setObstacleNeighbors(const std::vector<CParticle*>& vObstacleParticleSet) {m_RigidNeighborSet = vObstacleParticleSet;}
		void changeState();
		bool getState() const {return m_IsStateChanged;}

		const glm::vec3& getVelocity()                                   const {return m_Velocity;}
		const glm::vec4& getOldPosition()                                const {return m_OldPosition;}
		const glm::vec4& getNewPosition()                                const {return m_NewPosition;}
		const CParticle* getFluidNeighborByIndex(unsigned int vIndex)    const;
		const CParticle* getObstacleNeighborByIndex(unsigned int vIndex) const;
		unsigned int getNumFluidNeighbors()                              const {return m_FluidNeighborSet.size();}
		unsigned int getNumObstacleNeighbors()                           const {return m_RigidNeighborSet.size();}

		virtual void predictInfoV() {}
		virtual void correctInfoV() {}
		virtual void updateInfoV()  {}

	protected:
		glm::vec3 m_DeltaPosition;
		glm::vec4 m_OldPosition;
		glm::vec4 m_NewPosition; //w:1.0 FluidParticle w:0.0 obstacle
		glm::vec3 m_Velocity;

		float m_Mass;
		float m_InvMass;

		glm::vec3 m_PressureFromRigid;
		bool m_IsStateChanged;

		std::vector<CParticle*> m_FluidNeighborSet;
		std::vector<CParticle*> m_RigidNeighborSet;
		std::vector<CParticle*> m_NeighborSet;

		friend class CFluidManager;
	};
}