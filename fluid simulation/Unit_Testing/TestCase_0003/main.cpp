#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include <Eigen/dense>
#include <fstream>
#include "VoxelInterface.h"

//FUNCTION: detect the memory leak in DEBUG mode
void installMemoryLeakDetector()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	//_CRTDBG_LEAK_CHECK_DF: Perform automatic leak checking at program exit through a call to _CrtDumpMemoryLeaks and generate an error 
	//report if the application failed to free all the memory it allocated. OFF: Do not automatically perform leak checking at program exit.
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//the following statement is used to trigger a breakpoint when memory leak happens
	//comment it out if there is no memory leak report;
	//_crtBreakAlloc = 22455;
#endif
}

void writeToFile(const std::vector<Eigen::Vector3d>& vPointSet, const std::string& vFileName)
{
	std::ofstream Fout(vFileName); 
	for (unsigned int i=0; i<vPointSet.size(); i++)
	{
		Eigen::Vector3d Pos = vPointSet[i];
		Fout << Pos.x() << " " << Pos.y() << " " << Pos.z() << std::endl;
		//std::cout << Pos.x() << " " << Pos.y() << " " << Pos.z() << std::endl;
	}
	Fout.close();
}

int main()
{
	installMemoryLeakDetector();

	try
	{
		std::string Model = "../../Model/Box_thin331.obj";
		std::string FluidModel = "../../Model/Fluid_thin331.obj";

		std::vector<Eigen::Vector3d> BoxParticleSet, FluidParticleSet;

		hiveVoxel::voxelize(Model, 0.05, BoxParticleSet);
		hiveVoxel::voxelize(FluidModel, 0.05, FluidParticleSet);
		writeToFile(BoxParticleSet, "Box_thin331.txt");
		writeToFile(FluidParticleSet, "Fluid_thin331.txt");

	}
	catch (...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error.");
	}
}