#pragma once
#include "NeighborFinderFactoryData.h"
#include "BaseFindNeighbors.h"

namespace hiveFluidSimulation
{
	template<class DeriveNeighborFinder>
	class CNeighborFinderFactory
	{
	public:
		CNeighborFinderFactory(const std::string& vCreationSig)
		{
			CNeighborFinderFactoryData::getInstance()->registerFactory(vCreationSig, reinterpret_cast<CNeighborFinderFactory<CBaseFindNeighbors>*>(this));
		}
		~CNeighborFinderFactory() {}

	protected:
		virtual CBaseFindNeighbors* _createNeighborFinderV() {return new DeriveNeighborFinder;}

		friend class CNeighborFinderFactoryData;
	};
}