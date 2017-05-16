#include "VoxelParticleGenerator.h"
#include "VoxelInterface.h"
#include "FluidParticle.h"
#include "RigidParticle.h"

using namespace hiveFluidSimulation;

CVoxelParticleGenerator::CVoxelParticleGenerator() : m_IsVoxelInfoSpecified(false), m_Spacing(0.0)
{
}

CVoxelParticleGenerator::~CVoxelParticleGenerator()
{
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CVoxelParticleGenerator::generateParticleV(std::vector<CParticle*>& voParticleSet)
{
	_ASSERT(m_IsVoxelInfoSpecified);
	if (!m_IsVoxelInfoSpecified)
	{
		return;
	}

	std::vector<Eigen::Vector3d> ParticlePosSet;
	hiveVoxel::voxelize(m_ModelFile, m_Spacing, ParticlePosSet);
	voParticleSet.resize(ParticlePosSet.size());

	float Tag = -1;
	switch (m_ParticleType)
	{
	case FLUID:
		{
			Tag = 1.0;
			CFluidParticle* pFluid = new CFluidParticle[voParticleSet.size()]();
			for (unsigned int i=0; i<voParticleSet.size(); i++)
			{
				voParticleSet[i] = dynamic_cast<CParticle*>(pFluid + i);
			}
		}
		break;
	case CONTAINER:
		{
			Tag = 0.0;
			CRigidParticle* pObs = new CRigidParticle[voParticleSet.size()]();
			for (unsigned int i=0; i<voParticleSet.size(); i++)
			{
				voParticleSet[i] = dynamic_cast<CParticle*>(pObs + i);
			}
		}
		break;
	}

	for (unsigned int i=0; i<ParticlePosSet.size(); i++)
	{
		glm::vec4 InitPos = glm::vec4(ParticlePosSet[i].x(), ParticlePosSet[i].y(), ParticlePosSet[i].z(), Tag);
		CParticle* pTest = voParticleSet[i];
		voParticleSet[i]->setPosition(InitPos);
		voParticleSet[i]->setOldPosition(InitPos);
	}
}