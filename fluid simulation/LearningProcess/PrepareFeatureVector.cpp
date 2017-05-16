#include "PrepareFeatureVector.h"
#include "Particle.h"

using namespace hiveLearningProcess;

CPrepareFeatureVector::CPrepareFeatureVector()
{
}

CPrepareFeatureVector::~CPrepareFeatureVector()
{
}

//********************************************************************
//FUNCTION:
void hiveLearningProcess::CPrepareFeatureVector::computeFeatureVector(const hiveFluidSimulation::CParticle* vParticle, SFeatureVector& voFeatureVector) const
{
	_ASSERT(vParticle);
	glm::vec3 ViscosityFeature, SurfaceTensionFeature, PressureFearture;
	std::vector<glm::vec3> NeighborVelocitySet;
	std::vector<glm::vec4> NeighborPositionSet;

	for (unsigned int i=0; i<vParticle->getNumFluidNeighbors(); i++)
	{
// 		const hiveFluidSimulation::CParticle* pNeighborParticle = vParticle->getFluidNeighborByIndex(i);
// 		_ASSERT(pNeighborParticle);
// 		NeighborVelocitySet.push_back(pNeighborParticle->getVelocity());
// 		NeighborPositionSet.push_back(pNeighborParticle->getNewPosition());
	}
	
	voFeatureVector.Viscosity = __computeViscosityFeature(vParticle->getVelocity(), NeighborVelocitySet);
	voFeatureVector.SurfaceTension = __computeSurfaceTensionFeature(glm::vec3(vParticle->getNewPosition()), NeighborPositionSet);
	voFeatureVector.Pressure = __computePressureFeature(); //????
	voFeatureVector.Incompressibility = 1;//???
}

//********************************************************************
//FUNCTION:
glm::vec3 hiveLearningProcess::CPrepareFeatureVector::__computeViscosityFeature(const glm::vec3& vVelocity, const std::vector<glm::vec3>& vNeighborVelocitySet) const
{
	glm::vec3 ViscosityFeature;
	for (unsigned int i=0; i<vNeighborVelocitySet.size(); i++)
	{
		glm::vec3 NeighborVelocity = vNeighborVelocitySet[i];
		ViscosityFeature.x += NeighborVelocity.x;
		ViscosityFeature.y += NeighborVelocity.y;
		ViscosityFeature.z += NeighborVelocity.z;
	}

	ViscosityFeature.x -= vVelocity.x * vNeighborVelocitySet.size();
	ViscosityFeature.y -= vVelocity.y * vNeighborVelocitySet.size();
	ViscosityFeature.z -= vVelocity.z * vNeighborVelocitySet.size();
	ViscosityFeature *= (hiveLearningProcess::Mu / hiveLearningProcess::RestDensity);

	return ViscosityFeature;
}

//********************************************************************
//FUNCTION:
glm::vec3 hiveLearningProcess::CPrepareFeatureVector::__computeSurfaceTensionFeature(const glm::vec3& vPosition, const std::vector<glm::vec4>& vNeighborPositionSet) const
{
	return glm::vec3(0);
}