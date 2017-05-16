#pragma once
#include <string>
#include <map>
#include "common/Singleton.h"

namespace hiveFluidSimulation
{
	template<typename T>
	class CNeighborFinderFactory;

	class CBaseFindNeighbors;

	class CNeighborFinderFactoryData : public hiveCommon::CSingleton<CNeighborFinderFactoryData>
	{
	public:
		~CNeighborFinderFactoryData() {}
		void registerFactory(const std::string& vSig, CNeighborFinderFactory<CBaseFindNeighbors>* vFactory);
		CBaseFindNeighbors* createNeighborFinder(const std::string& vSig);

	protected:
		CNeighborFinderFactoryData(){}
		bool _isSigRegistered(const std::string& vSig) const;

	private:
		std::map<std::string, CNeighborFinderFactory<CBaseFindNeighbors>*> m_NeighborFinderMap;

		friend class hiveCommon::CSingleton<CNeighborFinderFactoryData>;
	};
}