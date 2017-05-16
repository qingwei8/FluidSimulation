#include "RunConfig.h"
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"

using namespace hiveFluidSimulation;

CRunConfig::CRunConfig() : m_IsGridInfoSpecified(true)
{
	defineAttribute(CONFIG_KEYWORD::PARTICLESPACING,      hiveConfig::ATTRIBUTE_FLOAT);
	defineAttribute(CONFIG_KEYWORD::FLUIDMODELFILE,       hiveConfig::ATTRIBUTE_STRING);
	defineAttribute(CONFIG_KEYWORD::BOXLEFTDOWN,          hiveConfig::ATTRIBUTE_VEC3F);
	defineAttribute(CONFIG_KEYWORD::BOXRIGHTUP,           hiveConfig::ATTRIBUTE_VEC3F);
	defineAttribute(CONFIG_KEYWORD::NUMFRAME,             hiveConfig::ATTRIBUTE_INT);
	defineAttribute(CONFIG_KEYWORD::FINDNEIGHBORMETHOD,   hiveConfig::ATTRIBUTE_STRING);
	defineAttribute(CONFIG_KEYWORD::DIVIDERANGE,          hiveConfig::ATTRIBUTE_VEC3F);
	defineAttribute(CONFIG_KEYWORD::UNITLENGTH,           hiveConfig::ATTRIBUTE_FLOAT);
	defineAttribute(CONFIG_KEYWORD::CONTAINERMODELFILE,   hiveConfig::ATTRIBUTE_STRING);
	defineAttribute(CONFIG_KEYWORD::PARTICLEINITIAL,      hiveConfig::ATTRIBUTE_STRING);
}

CRunConfig::~CRunConfig()
{
}

//********************************************************************
//FUNCTION:
void CRunConfig::_onLoadedV()
{
	if (!isAttributeExisted(CONFIG_KEYWORD::PARTICLESPACING))
	{
		setAttribute(CONFIG_KEYWORD::PARTICLESPACING, g_Radius * 2.0);
	}

	if (!isAttributeExisted(CONFIG_KEYWORD::FINDNEIGHBORMETHOD))
	{
		setAttribute(CONFIG_KEYWORD::FINDNEIGHBORMETHOD, NEIGHBORFINDER_KEYWORD::FOR_SIG);
	}

	if (!isAttributeExisted(CONFIG_KEYWORD::DIVIDERANGE))
	{
		boost::tuple<double, double, double> DivideRange(0.0, 0.0, 0.0);
		setAttribute(CONFIG_KEYWORD::DIVIDERANGE, DivideRange);
		m_IsGridInfoSpecified = false;
	}

	if (!isAttributeExisted(CONFIG_KEYWORD::UNITLENGTH))
	{
		setAttribute(CONFIG_KEYWORD::UNITLENGTH, 0.0f);
		m_IsGridInfoSpecified = false;
	}
}

//********************************************************************
//FUNCTION:
bool CRunConfig::_verifyConfigV() const 
{
	if (!isAttributeExisted(CONFIG_KEYWORD::PARTICLEINITIAL))
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The configuration must specify the particle inital method. The configuration of 'ParticleInitial' is missed.");
		return false;
	}

	if (getAttribute<std::string>(CONFIG_KEYWORD::PARTICLEINITIAL) != INITIALMETHOD::OTHER)
	{
		if (getAttribute<std::string>(CONFIG_KEYWORD::PARTICLEINITIAL) == INITIALMETHOD::VOXEL)
		{
			if (!isAttributeExisted(CONFIG_KEYWORD::FLUIDMODELFILE))
			{
				hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The model path is not specified.");
				return false;
			}
		}
		else if (getAttribute<std::string>(CONFIG_KEYWORD::PARTICLEINITIAL) == INITIALMETHOD::BOX)
		{
			if (!isAttributeExisted(CONFIG_KEYWORD::BOXRIGHTUP) || !isAttributeExisted(CONFIG_KEYWORD::BOXRIGHTUP))
			{
				hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The box information is not specified.");
				return false;
			}
		}
	}

	if (getAttribute<std::string>(CONFIG_KEYWORD::FINDNEIGHBORMETHOD) == NEIGHBORFINDER_KEYWORD::GRID_SIG)
	{
		if (!m_IsGridInfoSpecified)
		{
			hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The grid information is not specified.");
			return false;
		}
	}
	return true;
}

//********************************************************************
//FUNCTION:
glm::vec3 CRunConfig::__getPoint(const std::string& vName) const
{
	_ASSERT(!vName.empty());
	boost::tuple<double, double, double> Point = getAttribute<boost::tuple<double, double, double>>(vName);
	return glm::vec3(Point.get<0>(), Point.get<1>(), Point.get<2>());
}