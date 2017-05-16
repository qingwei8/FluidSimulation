#include "Particle.h"
#include "RunConfig.h"

using namespace hiveFluidSimulation;

CParticle::CParticle() : m_Velocity(glm::vec3(0.0f, 0.0f, 0.0f)), m_Mass(0.0), m_InvMass(0.0), m_IsStateChanged(false)
{
}

CParticle::~CParticle()
{
}

//********************************************************************
//FUNCTION:
const CParticle* hiveFluidSimulation::CParticle::getFluidNeighborByIndex(unsigned int vIndex) const
{
	_ASSERT(vIndex<m_FluidNeighborSet.size());
	return m_FluidNeighborSet[vIndex];
}

//********************************************************************
//FUNCTION:
const CParticle* hiveFluidSimulation::CParticle::getObstacleNeighborByIndex(unsigned int vIndex) const
{
	_ASSERT(vIndex<m_RigidNeighborSet.size());
	return m_RigidNeighborSet[vIndex];
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CParticle::setMass(float vMass)
{
	m_Mass = vMass;
	m_InvMass = (vMass == 0) ? 0.0 : 1.0/vMass;
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CParticle::changeState()
{
	m_IsStateChanged = !m_IsStateChanged;
}