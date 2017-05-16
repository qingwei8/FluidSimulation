#pragma once
#include <glm/glm.hpp>
#include "BaseParticleGenerator.h"

namespace hiveFluidSimulation
{
	class COtherParticleGenerator : public CBaseParticleGenerator
	{
	public:
		COtherParticleGenerator();
		~COtherParticleGenerator();

		void setOtherGenInfo(const ETypeParticle& vParticleType) {m_ParticleType = vParticleType; m_IsInfoSpecified = true;}
		virtual void generateParticleV(std::vector<CParticle*>& voParticleSet) override;
	
	private:
		void __generateFluidPos(float vA, glm::vec3 vStartPoint, std::vector<glm::vec3>& voFluidPosSet);
		void __generateBoundryPos(std::vector<glm::vec3>& voRigidPosSet);
		void __addWall(const glm::vec3 &minX, const glm::vec3 &maxX, std::vector<glm::vec3> &boundaryParticles);

	private:
		ETypeParticle m_ParticleType;
		bool m_IsInfoSpecified;
	};
}