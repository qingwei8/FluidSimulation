#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Export.h"
#include "Common.h"

namespace hiveFluidSimulation
{
	struct SBox3;
	class CParticle;
	class CBaseParticleGenerator;
	class CBaseFindNeighbors;

	FLUIDSIMULATION_DLL_EXPORT void initParticles(const std::string& vRunConfig);
	FLUIDSIMULATION_DLL_EXPORT void initSimulation(const std::string& vRunConfig);
	FLUIDSIMULATION_DLL_EXPORT void doSimulation();

	FLUIDSIMULATION_DLL_EXPORT float calculatePoly6(const glm::vec3& vPositionI, const glm::vec3& vPositionJ);
	FLUIDSIMULATION_DLL_EXPORT float calculateDistance(const glm::vec3& vPositionI, const glm::vec3& vPositionJ);
	FLUIDSIMULATION_DLL_EXPORT void testFindNeighbors(CBaseFindNeighbors* vFindNeighbor, const CParticle* vParticle, float vRadius, const std::vector<CParticle*>& vParticleSet, std::vector<CParticle*>& voFluidSet, std::vector<CParticle*>& voObstacleSet);
	FLUIDSIMULATION_DLL_EXPORT void initForTestingInteractiveForce(const std::string& vContainerFile, const std::vector<glm::vec4>& vFluidParticle);
	FLUIDSIMULATION_DLL_EXPORT glm::vec3 calculateSpiky(const glm::vec3& vPositionI, const glm::vec3& vPositionJ);

	void setParticleGeneratorInfo4Voxel(const std::string& vModelFile, float vSpacing, const ETypeParticle& vTag, CBaseParticleGenerator* &voParticleGen);
	void setParticleGeneratorInfo4Box(const SBox3& vBox, float vSpacing, const ETypeParticle& vTag, CBaseParticleGenerator* &voParticleGen);
	void initNeighborFinder();
	EParticleInitial parseInitalMethod(const std::string& vInitialStr);
}