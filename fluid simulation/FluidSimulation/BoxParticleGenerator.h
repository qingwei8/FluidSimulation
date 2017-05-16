#pragma once
#include "BaseParticleGenerator.h"
#include "Common.h"

namespace hiveFluidSimulation
{
	class CBoxParticleGenerator : public CBaseParticleGenerator
	{
	public:
		CBoxParticleGenerator();
		~CBoxParticleGenerator();

		void setBoxGenInfo(const ETypeParticle& vExtralTag, const SBox3& vBox, float vSpacing) {m_ParticleType = vExtralTag; m_Box = vBox; m_IsBoxInfoSpecified = true; m_Spacing = vSpacing;}
		virtual void generateParticleV(std::vector<CParticle*>& voParticleSet) override;

	private:
		void __createParticles(const glm::vec3& vStartPoint, const glm::uvec3& vDistributionRange, float vSpacing, const ETypeParticle& vExtralTag, std::vector<CParticle*>& voParticleSet);

	private:
		SBox3 m_Box;
		bool  m_IsBoxInfoSpecified;
		float m_Spacing;
		ETypeParticle m_ParticleType;
	};
}