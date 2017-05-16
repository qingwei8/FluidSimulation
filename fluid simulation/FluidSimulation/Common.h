#pragma once
#include <crtdbg.h>
#include <glm/glm.hpp>
#include <fstream>

namespace hiveFluidSimulation
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64)
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE __attribute__((always_inline))
#endif

	struct SBox3
	{
		glm::vec3 Min, Max;

		SBox3() {}
		SBox3(const SBox3& vOther) : Min(vOther.Min), Max(vOther.Max) {}
		SBox3(const glm::vec3& vMin, const glm::vec3& vMax) 
		{
			_ASSERT(vMin.x<=vMax.x && vMin.y<=vMax.y && vMin.z<=vMax.z);
			Min = vMin;
			Max = vMax;
		}
	};

	enum ETypeParticle
	{
		CONTAINER = 0,
		FLUID = 1,
	};

	enum EParticleInitial
	{
		INITIAL_VOXEL = 0,
		INITIAL_BOX,
		INITIAL_OTHER,
	};

	namespace INITIALMETHOD
	{
		const std::string BOX   = "BOX";
		const std::string VOXEL = "VOXEL";
		const std::string OTHER = "OTHER";
	}

	namespace NEIGHBORFINDER_KEYWORD
	{
		const std::string FOR_SIG = "FOR_SIG";
		const std::string GRID_SIG  = "GRID_SIG";
		const std::string SPATIALHASHING_SIG = "SPATIALHASHING_SIG";
	}

	const glm::vec3 g_Gravity = glm::vec3(0.0f, -9.8f, 0.0f);
	const float g_Radius = 0.025f;
	const float g_Density0 = 1000.0f;
	const float g_SmoothKernelRadius = g_Radius * 4.0;
    const float g_DeltaT = 0.0025f;
	const int   g_MaxIteration = 5;
	const float g_Viscosity = 0.02;

	const unsigned int g_width = 15;
	const unsigned int g_depth = 15;
	const unsigned int g_height = 20;
	const float g_containerWidth = (g_width + 1) * g_Radius * 2.0 * 5.0;
	const float g_containerDepth = (g_depth + 1) * g_Radius * 2.0;
	const float g_containerHeight = 4.0;
	
	const int   g_MaxNeighbors = 50;
	const float g_ParticalStiffness = 1.5f; 
	const float g_PI = 3.14159265358979323846f;

	const float g_RestDistance = g_Radius * 0.5f;
	const float g_RestDensityOfRigid = 100000.0f;
	const float g_TimeStep = 0.0025f;

	const float g_Constant_K = 0.00001f;
	const float g_Constant_C = 0.01f;
	const int   g_CorrectTime = 4;
 	const float g_KPoly6 = 315.0f / (64.0f * g_PI * pow(g_SmoothKernelRadius, 9));
 	const float g_Spiky  = 45.0f / (g_PI * pow(g_SmoothKernelRadius, 6));
	const float g_DqMag = 0.2f * g_Radius;
	const float g_WDeltaQH = g_KPoly6 * pow((g_SmoothKernelRadius * g_SmoothKernelRadius - g_DqMag * g_DqMag), 3);
	const float g_LambdaEps    = 600.0f;
	const float g_VorticityEps = 0.0001f;

	static void writePosInfo2File(const glm::vec3& vPos, std::ofstream& vioFileStream)
	{
		vioFileStream << vPos.x << " " << vPos.y << " " << vPos.z << std::endl;
	}

	class CSolverFunction
	{
	public:
		static float calPoly6(const glm::vec3& vPositionI, const glm::vec3& vPositionJ)
		{
			glm::vec3 RelativePos = vPositionI - vPositionJ;
			float Length = length(RelativePos);
			if (Length > g_SmoothKernelRadius) return 0;
			return g_KPoly6 * pow((pow(g_SmoothKernelRadius, 2) - pow(Length, 2)), 3);
		}

		static float calPoly6(const glm::vec3& vVector)
		{
			float VectorLength = length(vVector);
			if (VectorLength > g_SmoothKernelRadius)
			{
				return 0;
			}
			return g_KPoly6 * pow((g_SmoothKernelRadius * g_SmoothKernelRadius - length(vVector)), 3);
		}

		static glm::vec3 calSpiky(const glm::vec3& vPositionI, const glm::vec3& vPositionJ)
		{
			glm::vec3 Vector = vPositionI - vPositionJ;
			float VectorLength = length(Vector);
			if (VectorLength > g_SmoothKernelRadius || VectorLength == 0)
			{
				return glm::vec3(0.0f);
			}

			float Coeff = (g_SmoothKernelRadius - VectorLength) * (g_SmoothKernelRadius - VectorLength);
			Coeff *= g_Spiky;
			Coeff /= VectorLength;
			return Vector * -Coeff;
		}

		static float calDistance(const glm::vec3& vPositionI, const glm::vec3& vPositionJ)
		{
			return glm::distance(vPositionI, vPositionJ);
		}
	};
}