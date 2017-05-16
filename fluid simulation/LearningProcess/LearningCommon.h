#pragma once
#include <glm/glm.hpp>

namespace hiveLearningProcess
{
	const float Mu = 0.01f;
	const float RestDensity = 6378.0f;

	struct SFeatureVector
	{
		glm::vec3 Viscosity;
		glm::vec3 SurfaceTension;
		glm::vec3 Pressure;
		unsigned int Incompressibility;

		SFeatureVector(const glm::vec3& vViscosity, const glm::vec3& vSurfaceTension, const glm::vec3& vPressure, unsigned int vIncompressibility)
		{
			Viscosity         = vViscosity;
			SurfaceTension    = vSurfaceTension;
			Pressure          = vPressure;
			Incompressibility = vIncompressibility;
		}
	};
}