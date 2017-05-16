#pragma once
#include <vector>

namespace hiveFluidSimulation
{
	class CParticle;
	enum ETypeParticle;

	class CBaseParticleGenerator
	{
	public:
		CBaseParticleGenerator() {}
		virtual ~CBaseParticleGenerator() {}

		virtual void generateParticleV(std::vector<CParticle*>& voParticleSet) = 0;
	};
}