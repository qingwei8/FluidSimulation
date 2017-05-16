#include "Grid.h"

using namespace hiveFluidSimulation;

CGrid::CGrid()
{
}

CGrid::~CGrid()
{
}

//*******************************************************************
//FUNCTION:
void hiveFluidSimulation::CGrid::dumpParticleSet(std::vector<CParticle*>& voParticleSet) const
{
	voParticleSet.insert(voParticleSet.end(), m_GridParticleSet.begin(), m_GridParticleSet.end());
}

//*******************************************************************
//FUNCTION:
void hiveFluidSimulation::CGrid::addParticleToGrid(CParticle* vParticle)
{
	_ASSERT(vParticle);
	m_GridParticleSet.push_back(vParticle);
}

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CGrid::clearGridParticles()
{
	m_GridParticleSet.clear();
}