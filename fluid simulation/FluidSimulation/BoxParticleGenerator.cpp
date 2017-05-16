#include "BoxParticleGenerator.h"
#include "FluidParticle.h"

using namespace hiveFluidSimulation;

CBoxParticleGenerator::CBoxParticleGenerator() : m_IsBoxInfoSpecified(false), m_Spacing(0.0)
{
}

CBoxParticleGenerator::~CBoxParticleGenerator()
{
}

void hiveFluidSimulation::CBoxParticleGenerator::generateParticleV(std::vector<CParticle*>& voParticleSet)
{
	_ASSERT(m_IsBoxInfoSpecified);
	if (!m_IsBoxInfoSpecified)
	{
		return;
	}

	glm::uvec3 GridDim = glm::uvec3(m_Box.Max);
	voParticleSet.resize(GridDim.x*GridDim.y*GridDim.z);

	__createParticles(m_Box.Min, GridDim, m_Spacing, m_ParticleType, voParticleSet);
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CBoxParticleGenerator::__createParticles(const glm::vec3& vStartPoint, const glm::uvec3& vDistributionRange, float vSpacing, const ETypeParticle& vExtralTag, std::vector<CParticle*>& voParticleSet)
{
	switch (vExtralTag)
	{
	case FLUID:
		{
			CFluidParticle* pFluid = new CFluidParticle[voParticleSet.size()]();
			for (unsigned int i=0; i<voParticleSet.size(); i++)
			{
				voParticleSet[i] = &pFluid[i];
			}
		}
		break;
	case CONTAINER:
		{

		}
		break;
	default:
		break;
	}

	unsigned int Num = 0;
	for (unsigned int i=0; i<vDistributionRange.x; ++i)
	{
		for (unsigned int k=0; k<vDistributionRange.y; k++)
		{
			for (unsigned int m=0; m<vDistributionRange.z; m++)
			{
				glm::vec3 Pos = vStartPoint + glm::vec3((float)i, (float)k, (float)m) * vSpacing;

				_ASSERT((Num < voParticleSet.size()) && voParticleSet[Num]);			
				voParticleSet[Num]->setPosition(glm::vec4(Pos, vExtralTag));
				voParticleSet[Num]->setOldPosition(glm::vec4(Pos, vExtralTag));
				Num++;
			}
		}
	}
}