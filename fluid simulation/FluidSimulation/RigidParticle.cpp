#include "RigidParticle.h"
#include "Common.h"

using namespace hiveFluidSimulation;

CRigidParticle::CRigidParticle() : m_Volume(0.0), m_BoundaryPsi(0.0)
{
}

CRigidParticle::~CRigidParticle()
{
}

//********************************************************************
//FUNCTION:
void CRigidParticle::calVolume()
{
	m_Volume = 0.0f;
	float TempVolume = 0.0f;
	for (auto Rigid : m_RigidNeighborSet)
	{
		TempVolume += CSolverFunction::calPoly6(glm::vec3(m_NewPosition), glm::vec3(Rigid->getNewPosition()));		
	}

	if (TempVolume != 0.0f)
	{
		m_Volume = 1 / TempVolume;
	}	
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CRigidParticle::correctInfoV()
{
	calVolume();
}