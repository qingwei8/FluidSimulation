#include "NeighborFinderFactoryData.h"
#include "NeighborFinderFactory.h"

using namespace hiveFluidSimulation;

//********************************************************************
//FUNCTION:
void hiveFluidSimulation::CNeighborFinderFactoryData::registerFactory(const std::string& vSig, CNeighborFinderFactory<CBaseFindNeighbors>* vFactory)
{
	m_NeighborFinderMap[vSig] = vFactory;
}

//********************************************************************
//FUNCTION:
CBaseFindNeighbors* hiveFluidSimulation::CNeighborFinderFactoryData::createNeighborFinder(const std::string& vSig)
{
	if (!_isSigRegistered(vSig))
		return NULL;

	return m_NeighborFinderMap[vSig]->_createNeighborFinderV();
}

//********************************************************************
//FUNCTION:
bool hiveFluidSimulation::CNeighborFinderFactoryData::_isSigRegistered(const std::string& vSig) const
{
	return m_NeighborFinderMap.count(vSig) > 0;
}