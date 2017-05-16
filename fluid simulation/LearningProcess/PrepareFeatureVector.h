#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "LearningCommon.h"

namespace hiveFluidSimulation
{
	class CParticle;
}

namespace hiveLearningProcess
{
	class CPrepareFeatureVector
	{
	public:
		CPrepareFeatureVector();
		~CPrepareFeatureVector();

		void computeFeatureVector(const hiveFluidSimulation::CParticle* vParticle, SFeatureVector& voFeatureVector) const;

	private:
		glm::vec3 __computeViscosityFeature(const glm::vec3& vVelocity, const std::vector<glm::vec3>& vNeighborVelocitySet) const;
	    glm::vec3 __computeSurfaceTensionFeature(const glm::vec3& vPosition, const std::vector<glm::vec4>& vNeighborPositionSet) const;
		glm::vec3 __computePressureFeature() const {return glm::vec3(0);};
	};
}