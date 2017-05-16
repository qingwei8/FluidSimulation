#pragma once
#include "BaseParticleGenerator.h"
#include "Common.h"

namespace hiveFluidSimulation
{
	class CParticle;

	class CVoxelParticleGenerator : public CBaseParticleGenerator
	{
	public:
		CVoxelParticleGenerator();
		virtual ~CVoxelParticleGenerator();

		void setVoxelGenInfo(const ETypeParticle& vExtralTag, const std::string& vFile, float vSpacing) {m_ParticleType = vExtralTag; m_ModelFile = vFile; m_IsVoxelInfoSpecified = true; m_Spacing = vSpacing;}
		virtual void generateParticleV(std::vector<CParticle*>& voParticleSet) override;

	private:
		bool m_IsVoxelInfoSpecified;
		float         m_Spacing;
		ETypeParticle m_ParticleType;
		std::string   m_ModelFile;
	};
}