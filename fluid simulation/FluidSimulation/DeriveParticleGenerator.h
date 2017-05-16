#pragma once
#include "BaseParticleGenerator.h"

namespace hiveFluidSimulation
{
	class CDeriveParticleGenerator : public CBaseParticleGenerator
	{
	public:
		CDeriveParticleGenerator();
		virtual ~CDeriveParticleGenerator();

		void setBox(const SBox3& vBox) {m_Box = vBox; m_IsBoxSpecified = true;}
		virtual void generateParticleV(std::vector<CParticle*>& voParticleSet) override;

	private:
		void __createParticles(const glm::vec3& vStartPoint, const glm::uvec3& vDistributionRange, float vSpacing, const ETypeParticle& vExtralTag, std::vector<CParticle*>& voParticleSet);

	private:
		SBox3 m_Box;
		bool  m_IsBoxSpecified;
	};
}