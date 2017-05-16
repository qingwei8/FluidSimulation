#pragma once
#include <string>
#include "VoxelExport.h"
#include "Voxelize.h"

namespace hiveVoxel
{
	VOXEL_DLL_EXPORTS void voxelize(const std::string& vModelFilePath, float vRadius, std::vector<Eigen::Vector3d>& voParticles);//ply or obj
}