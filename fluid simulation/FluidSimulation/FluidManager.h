#pragma once
#include "Common/singleton.h"
#include "Scene.h"

namespace hiveFluidSimulation
{
	class CFluidManager : public hiveCommon::CSingleton<CFluidManager>
	{
	public:
		~CFluidManager(void);		

	public:
		void doSimulation(unsigned int vMaxFrame);

	private:
		CFluidManager(void);

		void __predictInitialInfo();
		void __neighborSearch();
		void __spatialHashNeighborSearch();
		void __correctPosition();
		void __updateVelocity();
		void __updateNeighbor();
		void __recordFluidSimulationInfo(std::ofstream& voOutputStream);
		void __recordObstacleSimulationInfo(std::ofstream& voOutputStream);

		friend class hiveCommon::CSingleton<CFluidManager>;
	};
}