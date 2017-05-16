#include "OtherParticleGenerator.h"
#include "SPHKernels.h"
#include "FluidParticle.h"
#include "RigidParticle.h"

using namespace hiveFluidSimulation;

COtherParticleGenerator::COtherParticleGenerator() : m_IsInfoSpecified(false)
{
}

COtherParticleGenerator::~COtherParticleGenerator()
{
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::COtherParticleGenerator::generateParticleV(std::vector<CParticle*>& voParticleSet)
{
	_ASSERT(m_IsInfoSpecified);
	if (!m_IsInfoSpecified) return;

	std::vector<glm::vec3> ParticlePosSet;
	float Tag = 0.0;
	switch (m_ParticleType)
	{
	case FLUID:
		{
			Tag = 1.0;

			float Diam = 2.0f * g_Radius;
			float StartX = -0.5f * g_containerWidth + Diam;
			float StartY = Diam;
			float StartZ = -0.5f * g_containerDepth + Diam;

			__generateFluidPos(Diam, glm::vec3(StartX, StartY, StartZ), ParticlePosSet);
			voParticleSet.resize(ParticlePosSet.size());

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

			__generateBoundryPos(ParticlePosSet);
			voParticleSet.resize(ParticlePosSet.size());

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
		glm::vec4 InitPos = glm::vec4(ParticlePosSet[i].x, ParticlePosSet[i].y, ParticlePosSet[i].z, Tag);
		CParticle* pTest = voParticleSet[i];
		voParticleSet[i]->setPosition(InitPos);
		voParticleSet[i]->setOldPosition(InitPos);
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::COtherParticleGenerator::__generateFluidPos(float vA, glm::vec3 vStartPoint, std::vector<glm::vec3>& voFluidPosSet)
{
	voFluidPosSet.clear();

	for (int i=0; i<(int)g_width; ++i)
	{
		for (int j=0; j<(int)g_height; ++j)
		{
			for (int k=0; k<(int)g_depth; ++k)
			{
				glm::vec3 Temp = vA * glm::vec3((float)i, (float)j, (float)k) + vStartPoint;
				voFluidPosSet.push_back(Temp);
			}
		}
	}
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::COtherParticleGenerator::__generateBoundryPos(std::vector<glm::vec3>& voRigidPosSet)
{
	const float x1 = -g_containerWidth / 2.0;
	const float x2 =  g_containerWidth / 2.0;
	const float y1 = 0.0; 
	const float y2 = g_containerHeight;
	const float z1 = -g_containerDepth / 2.0;
	const float z2 = g_containerDepth / 2.0;

	const float diam = 2.0 * g_Radius;

	// Floor
	__addWall(glm::vec3(x1, y1, z1), glm::vec3(x2, y1, z2), voRigidPosSet);
	// Top
	__addWall(glm::vec3(x1, y2, z1), glm::vec3(x2, y2, z2), voRigidPosSet);
	// Left
	__addWall(glm::vec3(x1, y1, z1), glm::vec3(x1, y2, z2), voRigidPosSet);
	// Right
	__addWall(glm::vec3(x2, y1, z1), glm::vec3(x2, y2, z2), voRigidPosSet);
	// Back
	__addWall(glm::vec3(x1, y1, z1), glm::vec3(x2, y2, z1), voRigidPosSet);
	// Front
	__addWall(glm::vec3(x1, y1, z2), glm::vec3(x2, y2, z2), voRigidPosSet);
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::COtherParticleGenerator::__addWall(const glm::vec3 &minX, const glm::vec3 &maxX, std::vector<glm::vec3> &boundaryParticles)
{
	const float particleDistance = 2.0*g_Radius;

	const glm::vec3 diff = maxX - minX;
	const unsigned int stepsX = (unsigned int)(diff[0] / particleDistance) + 1u;
	const unsigned int stepsY = (unsigned int)(diff[1] / particleDistance) + 1u;
	const unsigned int stepsZ = (unsigned int)(diff[2] / particleDistance) + 1u;

	const unsigned int startIndex = (unsigned int) boundaryParticles.size();
	boundaryParticles.resize(startIndex + stepsX*stepsY*stepsZ);

	//#pragma omp parallel default(shared)
	//	{
	//#pragma omp for schedule(static)
	for (int j = 0; j < (int)stepsX; j++)
	{
		for (unsigned int k = 0; k < stepsY; k++)
		{
			for (unsigned int l = 0; l < stepsZ; l++)
			{
				const glm::vec3 currPos = minX + glm::vec3(j*particleDistance, k*particleDistance, l*particleDistance);
				boundaryParticles[startIndex + j*stepsY*stepsZ + k*stepsZ + l] = currPos;
			}
		}
	}
	//}
}